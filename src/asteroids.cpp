
#define _USING_V110_SDK71_ 1
#include "windows.h"

#include <cstdio>

#include "lang_helpers.hpp"
#include "math.hpp"
#include "vector/vector.hpp"
#include "random.hpp"

typedef s32v2	iv2;
typedef s32v3	iv3;
typedef s32v4	iv4;
typedef fv2		v2;
typedef fv3		v3;
typedef fv4		v4;
typedef fm2		m2;
typedef fm3		m3;
typedef fm4		m4;

#include "glad.c"
#include "GLFW/glfw3.h"

STATIC_ASSERT(sizeof(GLint) ==		sizeof(s32));
STATIC_ASSERT(sizeof(GLuint) ==		sizeof(u32));
STATIC_ASSERT(sizeof(GLsizei) ==	sizeof(u32));
STATIC_ASSERT(sizeof(GLsizeiptr) ==	sizeof(u64));

struct Rect {
	iv2 pos;
	iv2 dim;
};

//
static GLFWmonitor*	primary_monitor;
static GLFWwindow*	wnd;
static bool			fullscreen;
static Rect			_suggested_wnd_rect;
static Rect			_restore_wnd_rect;

static u32			frame_indx; // probably should only used for debug logic

static f64			t;
static f32			dt;

static iv2			wnd_dim;
static v2			wnd_dim_aspect;
static iv2			cursor_pos;
static bool			cursor_in_wnd;
static s32			scrollwheel_diff;

#include "buttons.hpp"

static void glfw_error_proc (int err, const char* msg) {
	fprintf(stderr, ANSI_COLOUR_CODE_RED "GLFW Error! 0x%x '%s'\n" ANSI_COLOUR_CODE_NC, err, msg);
}
static void glfw_scroll_proc (GLFWwindow* window, double xoffset, double yoffset) {
	scrollwheel_diff += (s32)floor(yoffset);
}

static void setup_glfw () {
	glfwSetErrorCallback(glfw_error_proc);
	
	dbg_assert( glfwInit() );
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,	3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,	3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE,			GLFW_OPENGL_CORE_PROFILE);
	
	primary_monitor = glfwGetPrimaryMonitor();
	
	glfwWindowHint(GLFW_VISIBLE,				0);
	wnd = glfwCreateWindow(1280, 720, u8"", NULL, NULL);
	dbg_assert(wnd);
	
	glfwGetWindowPos(wnd, &_suggested_wnd_rect.pos.x,&_suggested_wnd_rect.pos.y);
	glfwGetWindowSize(wnd, &_suggested_wnd_rect.dim.x,&_suggested_wnd_rect.dim.y);
	
	glfwSetKeyCallback(wnd, glfw_key_proc);
	glfwSetScrollCallback(wnd, glfw_scroll_proc);
	glfwSetMouseButtonCallback(wnd, glfw_mousebutton_proc);
	
	glfwMakeContextCurrent(wnd);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	
}

static void toggle_fullscreen () {
	if (fullscreen) {
		glfwSetWindowMonitor(wnd, NULL, _restore_wnd_rect.pos.x,_restore_wnd_rect.pos.y,
				_restore_wnd_rect.dim.x,_restore_wnd_rect.dim.y, GLFW_DONT_CARE);
	} else {
		glfwGetWindowPos(wnd, &_restore_wnd_rect.pos.x,&_restore_wnd_rect.pos.y);
		glfwGetWindowSize(wnd, &_restore_wnd_rect.dim.x,&_restore_wnd_rect.dim.y);
		
		auto* r = glfwGetVideoMode(primary_monitor);
		glfwSetWindowMonitor(wnd, primary_monitor, 0,0, r->width,r->height, r->refreshRate);
	}
	fullscreen = !fullscreen;
	
	glfwSwapInterval(1); // seems like vsync needs to be set after switching to from the inital hidden window to a fullscreen one, or there will be no vsync
}
static void init_show_window (bool fullscreen, Rect rect=_suggested_wnd_rect) {
	::fullscreen = !fullscreen;
	
	printf(">> %d %d %d %d\n", rect.pos.x,rect.pos.y, rect.dim.x,rect.dim.y);
	
	_restore_wnd_rect = rect;
	
	toggle_fullscreen();
	
	glfwShowWindow(wnd);
}
static Rect get_monitor_rect () {
	auto* r = glfwGetVideoMode(primary_monitor);
	Rect ret;
	ret.dim = iv2(r->width,r->height);
	glfwGetMonitorPos(primary_monitor, &ret.pos.x,&ret.pos.y);
	return ret;
}

#if 0
#define GLSL_VERSION "#version 140\n"
#else
#define GLSL_VERSION "#version 330\n"
#endif

//
struct Camera {
	v2	pos_world;
	f32	radius; // zoom adjusted so that either window width or height fits 2*radus (whichever is smaller)
	
	m4 world_to_clip;
};
static Camera		cam;

static v2			cursor_pos_world;

static GLuint vbo_init_and_upload_static (void* data, uptr data_size) {
	GLuint vbo;
	glGenBuffers(1, &vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	return vbo;
}
static bool shad_check_compile_status (GLuint shad) {
	GLint status;
	glGetShaderiv(shad, GL_COMPILE_STATUS, &status);
	bool error = status == GL_FALSE;
	
	s32 log_size; // including null terminator
	glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &log_size);
	
	if (log_size > 1) { // 0=>no log  1=>empty log
		char* log_str = (char*)malloc(log_size);
		defer { free(log_str); };
		
		s32 written_len;
		glGetShaderInfoLog(shad, log_size, &written_len, log_str);
		dbg_assert(written_len == (log_size -1) && log_str[written_len] == '\0');
		
		if (error) {
			printf(	ANSI_COLOUR_CODE_RED
					"Shader compilation failed!\n"
					"  '%s'\n" ANSI_COLOUR_CODE_NC, log_str);
		} else {
			printf(	ANSI_COLOUR_CODE_YELLOW
					"Shader compilation info log:\n"
					"  '%s'\n" ANSI_COLOUR_CODE_NC, log_str);
		}
	}
	return !error;
}
static bool shad_check_link_status (GLuint prog) {
	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	bool error = status == GL_FALSE;
	
	s32 log_size; // including null terminator
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);
	
	if (log_size > 1) { // 0=>no log  1=>empty log
		char* log_str = (char*)malloc(log_size);
		defer { free(log_str); };
		
		s32 written_len;
		glGetProgramInfoLog(prog, log_size, &written_len, log_str);
		dbg_assert(written_len == (log_size -1) && log_str[written_len] == '\0');
		
		if (error) {
			printf(	ANSI_COLOUR_CODE_RED
					"Shader linking failed!\n"
					"  '%s'\n" ANSI_COLOUR_CODE_NC, log_str);
		} else {
			printf(	ANSI_COLOUR_CODE_YELLOW
					"Shader linking info log:\n"
					"  '%s'\n" ANSI_COLOUR_CODE_NC, log_str);
		}
	}
	return !error;
}

struct Basic_Shader {
	GLuint		prog;
	
	cstr		vert_src;
	cstr		frag_src;
	
	Basic_Shader (cstr v, cstr f): vert_src{v}, frag_src{f} {}
	
	void compile () {
		prog = glCreateProgram();
		
		GLuint vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &vert_src, NULL);
		glCompileShader(vert);
		shad_check_compile_status(vert);
		glAttachShader(prog, vert);
		
		GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &frag_src, NULL);
		glCompileShader(frag);
		shad_check_compile_status(frag);
		glAttachShader(prog, frag);
		
		glLinkProgram(prog);
		shad_check_link_status(prog);
		
		glDetachShader(prog, vert);
		glDeleteShader(vert);
		
		glDetachShader(prog, frag);
		glDeleteShader(frag);
	}
	void bind () {
		glUseProgram(prog);
	}
	
};

struct Unif_s32 {
	GLint loc;
	void set (s32 i) const {
		glUniform1i(loc, i);
	}
};
struct Unif_flt {
	GLint loc;
	void set (f32 f) const {
		glUniform1f(loc, f);
	}
};
struct Unif_fv2 {
	GLint loc;
	void set (fv2 v) const {
		glUniform2f(loc, v.x,v.y);
	}
};
struct Unif_fv3 {
	GLint loc;
	void set (fv3 cr v) const {
		glUniform3fv(loc, 1, &v.x);
	}
};
struct Unif_fm2 {
	GLint loc;
	void set (fm2 m) const {
		glUniformMatrix2fv(loc, 1, GL_FALSE, &m.arr[0][0]);
	}
};
struct Unif_fm4 {
	GLint loc;
	void set (fm4 m) const {
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m.arr[0][0]);
	}
};

struct VBO_Pos_Col {
	GLuint	vbo;
	struct V {
		v2	pos;
		v4	col;
	};
	
	void init () {
		glGenBuffers(1, &vbo);
	}
	void upload (array<V> data) {
		uptr data_size = data.len * sizeof(V);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, data_size, data.arr, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void bind (Basic_Shader cr shad) {
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		GLint	pos =	glGetAttribLocation(shad.prog, "attrib_pos");
		GLint	col =	glGetAttribLocation(shad.prog, "attrib_col");
		
		dbg_assert(pos >= 0);
		dbg_assert(col >= 0);
		
		glEnableVertexAttribArray(pos);
		glVertexAttribPointer(pos,	2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V,pos));
		
		glEnableVertexAttribArray(col);
		glVertexAttribPointer(col,	4, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V,col));
		
	}
	
};
struct Shader_Clip_Col : Basic_Shader {
	Shader_Clip_Col (): Basic_Shader(
// Vertex shader
GLSL_VERSION R"_SHAD(
	in		vec2	attrib_pos; // clip
	in		vec4	attrib_col;
	out		vec4	color;
	
	void main() {
		gl_Position = vec4(attrib_pos, 0.0, 1.0);
		color = attrib_col;
	}
)_SHAD",
// Fragment shader
GLSL_VERSION R"_SHAD(
	in		vec4	color;
	out		vec4	frag_col;
	
	void main() {
		frag_col = color;
	}
)_SHAD"
	) {}
	
	void init () {
		compile();
	}
};
struct Shader_World_Col : Basic_Shader {
	Shader_World_Col (): Basic_Shader(
// Vertex shader
GLSL_VERSION R"_SHAD(
	in		vec2	attrib_pos; // world
	in		vec4	attrib_col;
	out		vec4	color;
	uniform	mat4	world_to_clip;
	
	void main() {
		gl_Position = world_to_clip * vec4(attrib_pos, 0.0, 1.0);
		color = attrib_col;
	}
)_SHAD",
// Fragment shader
GLSL_VERSION R"_SHAD(
	in		vec4	color;
	out		vec4	frag_col;
	
	void main() {
		frag_col = color;
	}
)_SHAD"
	) {}
	
	// uniforms
	Unif_fm4	world_to_clip;
	
	void init () {
		compile();
		
		world_to_clip.loc =		glGetUniformLocation(prog, "world_to_clip");
		
		dbg_assert(world_to_clip.loc >= 0);
		
	}
};

struct VBO_Pos_Tex_Col {
	GLuint	vbo;
	struct V {
		v2	pos;
		v2	uv;
		v4	col;
	};
	
	void init () {
		glGenBuffers(1, &vbo);
	}
	void upload (array<V> cr data) {
		uptr data_size = data.len * sizeof(V);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, data_size, data.arr, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void bind (Basic_Shader cr shad) {
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		GLint	pos =	glGetAttribLocation(shad.prog, "attrib_pos");
		GLint	uv =	glGetAttribLocation(shad.prog, "attrib_uv");
		GLint	col =	glGetAttribLocation(shad.prog, "attrib_col");
		
		dbg_assert(pos >= 0);
		dbg_assert(uv >= 0);
		dbg_assert(col >= 0);
		
		glEnableVertexAttribArray(pos);
		glVertexAttribPointer(pos,	2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V,pos));
		
		glEnableVertexAttribArray(uv);
		glVertexAttribPointer(uv,	2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V,uv));
		
		glEnableVertexAttribArray(col);
		glVertexAttribPointer(col,	4, GL_FLOAT, GL_FALSE, sizeof(V), (void*)offsetof(V,col));
		
	}
	
};
#include "font.hpp"
struct Shader_Clip_Tex_Col : Basic_Shader {
	Shader_Clip_Tex_Col (): Basic_Shader(
// Vertex shader
GLSL_VERSION R"_SHAD(
	in		vec2	attrib_pos; // clip
	in		vec2	attrib_uv;
	in		vec4	attrib_col;
	out		vec4	color;
	out		vec2	uv;
	
	void main() {
		gl_Position =	vec4(attrib_pos, 0.0, 1.0);
		uv =			attrib_uv;
		color =			attrib_col;
	}
)_SHAD",
// Fragment shader
GLSL_VERSION R"_SHAD(
	in		vec4	color;
	in		vec2	uv;
	uniform	sampler2D	tex;
	
	out		vec4	frag_col;
	
	void main() {
		frag_col = color * vec4(1,1,1, texture(tex, uv).r);
	}
)_SHAD"
	) {}
	
	void init () {
		compile();
		
		auto tex = glGetUniformLocation(prog, "tex");
		dbg_assert(tex != -1);
		glUniform1i(tex, 0);
	}
	void bind_texture (Texture tex) {
		glActiveTexture(GL_TEXTURE0 +0);
		glBindTexture(GL_TEXTURE_2D, tex.gl);
	}
};

using font::Font;
static Font	dbg_font;

struct Dbg_Text {
	
	//shad_tex.bind();
	//shad_tex.bind_texture(dbg_font.tex);
	//
	//dbg_font.draw_text(shad_tex, U"Hello WOrld あいうえお「＿＿＿」^^__||!", v2(400, 100), 1);
	//dbg_font.draw_text(shad_tex, U"Hello WOrld あいうえお「＿＿＿」^^__||!", v2(400, 117), 1);
	
};

namespace asteroids {
	
	static Shader_Clip_Tex_Col	shad_tex;
	static Shader_World_Col		shad_world_col;
	
	static VBO_Pos_Col			vbo_world_col;
	
	static v2 world_radius = v2(50);
	
	typedef VBO_Pos_Col::V Vertex;
	
	v2 wrap (v2 pos) {
		return mymod(pos +world_radius, world_radius*2) -world_radius;
	}
	
	void draw_with_fake_wrapping (GLenum primitive, array<v2> cr vertecies) {
		
		constexpr v2 fake_wrap_instances[4] = {
			v2(0,0),
			v2(2,0),
			v2(0,2),
			v2(2,2),
		};
		
		auto data = array<Vertex>::malloc(arrlen(fake_wrap_instances) * vertecies.len);
		auto* out = &data[0];
		
		v4 col = v4(1);
		for (v2 fake_wrap : fake_wrap_instances) {
			for (u32 i=0; i<vertecies.len; ++i) {
				v2 v = vertecies[i];
				
				v2 object_pos = v;
				if (primitive == GL_LINES) object_pos = vertecies[i & ~(u32)1];
				
				v2 flip_wrap = select(v2(+1),v2(-1), object_pos < 0);
				v2 fake_wrap_offs = world_radius * fake_wrap*flip_wrap;
				
				out->pos = v +fake_wrap_offs;
				out->col = col;
				++out;
			}
		}
		
		vbo_world_col.upload(data);
		
		glDrawArrays(primitive, 0, data.len);
	}
	
	struct Ship {
		v2	pos;
		v2	vel;
		f32	ori;
	};
	static Ship ship;
	
	struct Asteroid {
		v2 pos;
		v2 vel;
		
		enum size_e : u32 {
			SMALL	=0,
			MEDIUM	=1,
			BIG		=2,
		};
		size_e size;
		
		static constexpr u32 VERTEX_COUNTS[3] = {
			5,
			9,
			12,
		};
		static constexpr f32 VERTEX_RADII[3] = {
			1,
			3,
			5,
		};
		
		
		u32 get_vertex_count () {
			return VERTEX_COUNTS[size];
		}
		
		v2 vertecies[VERTEX_COUNTS[BIG]];
		
		void generate_mesh () {
			// maybe generate mesh based on sin() with different frequencies over circle
			
			u32 vertex_count = VERTEX_COUNTS[size];
			u32 r = VERTEX_RADII[size];
			dbg_assert(vertex_count == VERTEX_COUNTS[size]);
			
			u32 deep_v = (u32)round(random::f32_01() * (vertex_count-1));
			dbg_assert(deep_v >= 0 && deep_v < vertex_count);
			
			f32 c_step = 1.0f / (f32)vertex_count;
				
			for (u32 i=0; i<vertex_count; ++i) {
				f32 c_step_offs;
				if (		i+1 == deep_v )	c_step_offs = lerp(0, +c_step/2, random::f32_01());
				else if (	i-1 == deep_v)	c_step_offs = lerp(-c_step/2, 0, random::f32_01());
				else						c_step_offs = lerp(-c_step/2, +c_step/2, random::f32_01());
				
				f32 t = (f32)i * c_step +c_step_offs;
				
				auto random_r = [&] () {
					f32 y = random::f32_01();
					y = y*y;
					if (i != deep_v)	return r * lerp(1.2f, 0.75f, y);
					else				return r * lerp(0.75f, 0.12f, y);
				};
				vertecies[i] = rotate2(t * RAD_360) * v2(0,random_r());
			}
		}
	};
	constexpr u32 Asteroid::VERTEX_COUNTS[3];
	constexpr f32 Asteroid::VERTEX_RADII[3];
	
	static dynarr<Asteroid*> asteroids;
	
	static void spawn_asteroids (u32 count) {
		for (u32 i=0; i<count; ++i) {
			
			auto* a = (Asteroid*)malloc(sizeof(Asteroid));
			a->pos = random::v2_n1p1() * world_radius;
			a->vel = rotate2(random::f32_01() * RAD_360) * lerp(4, 7, random::f32_01());
			a->size = Asteroid::BIG;
			a->generate_mesh();
			
			asteroids.push(a);
		}
	}
	static void update_asteroids () {
		for (u32 i=0; i<asteroids.len; ++i) {
			auto* a = asteroids[i];
			a->pos += a->vel * dt;
			
			a->pos = wrap(a->pos);
		}
	}
	static void split_asteroid (u32 i) {
		auto* tmp = asteroids[i];
		
		if (		tmp->size == Asteroid::SMALL ) {
			
		} else if (	tmp->size == Asteroid::MEDIUM ) {
			auto* a = (Asteroid*)malloc(sizeof(Asteroid));
			auto* b = (Asteroid*)malloc(sizeof(Asteroid));
			auto* c = (Asteroid*)malloc(sizeof(Asteroid));
			
			a->pos = tmp->pos;
			b->pos = tmp->pos;
			c->pos = tmp->pos;
			
			a->size = (Asteroid::size_e)(tmp->size -1);
			b->size = (Asteroid::size_e)(tmp->size -1);
			c->size = (Asteroid::size_e)(tmp->size -1);
			
			v2 split_vel_a = rotate2(random::f32_01() * RAD_360) * lerp(7, 12, random::f32_01());
			v2 split_vel_b = rotate2(random::f32_01() * RAD_360) * lerp(7, 12, random::f32_01());
			v2 split_vel_c = -split_vel_a -split_vel_b;
			
			a->vel = tmp->vel +split_vel_a;
			b->vel = tmp->vel +split_vel_b;
			c->vel = tmp->vel +split_vel_c;
			
			a->generate_mesh();
			b->generate_mesh();
			c->generate_mesh();
			
			asteroids.push(a);
			asteroids.push(b);
			asteroids.push(c);
			
		} else if (	tmp->size == Asteroid::BIG ) {
			auto* a = (Asteroid*)malloc(sizeof(Asteroid));
			auto* b = (Asteroid*)malloc(sizeof(Asteroid));
			
			a->pos = tmp->pos;
			b->pos = tmp->pos;
			
			a->size = (Asteroid::size_e)(tmp->size -1);
			b->size = (Asteroid::size_e)(tmp->size -1);
			
			v2 split_vel = rotate2(random::f32_01() * RAD_360) * lerp(2, 6, random::f32_01());
			
			a->vel = tmp->vel +split_vel;
			b->vel = tmp->vel -split_vel;
			
			a->generate_mesh();
			b->generate_mesh();
			
			asteroids.push(a);
			asteroids.push(b);
			
		}
		
		asteroids.delete_by_moving_last(i);
		free(tmp);
	}
	
	static bool test_collison (Asteroid* aster, v2 v) {
		v = v -aster->pos;
		
		u32 vertex_count = Asteroid::VERTEX_COUNTS[aster->size];
		for (u32 i=0; i<vertex_count; ++i) {
			v2 a = aster->vertecies[i];
			v2 b = aster->vertecies[(i+1) % vertex_count];
			v2 c = 0.0f;
			
			v2 ca = a -c;
			ca = v2(-ca.y,ca.x);
			
			v2 cb = b -c;
			cb = v2(cb.y,-cb.x);
			
			v2 ab = b -a;
			ab = v2(-ab.y,ab.x);
			
			auto s = dot(v, ca);
			auto t = dot(v, cb);
			auto u = dot(v -a, ab);
			if (s >= 0 && t >= 0 && u >= 0) return true;
			
		}
		
		return false;
	}
	
	struct Bullet {
		v2	pos;
		v2	vel;
		
		f32	time_to_live;
	};
	static dynarr<Bullet*> bullets = {}; // non allocated
	
	static f32 bullet_muzzle_vel = 60;
	static f64 t_last_shot = 0;
	
	static void shoot (v2 pos, v2 vel) {
		f32 ttl = 0.9f * world_radius.x*2 / bullet_muzzle_vel;
		auto* b = (Bullet*)malloc(sizeof(Bullet));
		*b = {pos, vel, ttl};
		t_last_shot = t;	
		
		bullets.push(b);
	}
	static void update_bullets () {
		// cull expired bullets
		for (u32 i=0; i<bullets.len;) {
			if (bullets[i]->time_to_live <= 0) {
				bullets.delete_by_moving_last(i);
				continue; // 
			}
			bullets[i]->time_to_live -= dt;
			++i;
		}
		// bullets split asteroids
		auto test_asteroids_collison = [&] (Bullet* b) -> Asteroid* {
			for (u32 i=0; i<asteroids.len; ++i) {
				if (test_collison(asteroids[i], b->pos)) return asteroids[i];
			}
			return nullptr;
		};
		for (u32 i=0; i<bullets.len;) {
			
			bool coll = false;
			u32 ast_i=0;
			for (; ast_i<asteroids.len; ++ast_i) {
				if (test_collison(asteroids[ast_i], bullets[i]->pos)) {
					coll = true;
					break;
				}
			}
			
			if (coll) {
				free(bullets[i]);
				bullets.delete_by_moving_last(i);
				
				split_asteroid(ast_i);
			} else {
				++i;
			}
		}
		// bullet physics
		for (u32 i=0; i<bullets.len; ++i) {
			auto* b = bullets[i];
			b->pos += b->vel * dt;
			b->pos = wrap(b->pos);
		}
	}
	
	constexpr cstr	PROJECT_NAME =		u8"asteroids";
	cstr			game_name =			u8"「アステロイド」ー【ASTEROIDS】";
	
	f32			running_avg_fps;
	
	array<utf8>	dbg_name_and_fps = {}; // non_allocated
	array<utf8>	wnd_title = {}; // non_allocated
	array<utf8>	info = {}; // non_allocated
	
	static void reset () {
		ship = Ship{0,0,0};
		
		bullets.realloc(0);
		asteroids.realloc(0);
		
		spawn_asteroids(10);
	}
	static void init  () {
		cam.pos_world = v2(0);
		cam.radius = MAX(world_radius.x, world_radius.y)*1.0f;
		
		auto mr = get_monitor_rect();
		#if 0
		s32 border_top =		31;			// window title
		s32 border_bottom =		8 +29;		// window bottom border +taskbar
		s32 border_right =		8;			// window right border
		s32 h = mr.dim.y -border_top -border_bottom;
		Rect r = {mr.pos +iv2(mr.dim.x,border_top) -iv2(h +border_right, 0), h};
		#else
		s32 border_top =		31;			// window title
		s32 border_bottom =		8 +29;		// window bottom border +taskbar
		s32 border_left =		8;			// window right border
		s32 h = mr.dim.y -border_top -border_bottom;
		Rect r = {mr.pos +iv2(border_left,border_top), h};
		#endif
		init_show_window(false, r);
		
		running_avg_fps = 60; // assume 60 fps initially
		
		shad_tex.init();
		shad_world_col.init();
		vbo_world_col.init();
		
		reset();
	}
	
	static void frame () {
		
		if (frame_indx != 0) {
			f32 alpha = 0.025f;
			running_avg_fps = running_avg_fps*(1.0f -alpha) +(1.0f/dt)*alpha;
		}
		{
			print_array(&wnd_title, "%s    ~%.1f fps", game_name, running_avg_fps);
			glfwSetWindowTitle(wnd, wnd_title.arr);
			
			
			print_array(&dbg_name_and_fps, "%s  ~%.1f fps  %.3f ms", PROJECT_NAME, running_avg_fps, dt*1000);
		}
		
		dt = 1.0f / 60.0f; // do fixed dt for now
		
		if (button_went_down(B_R))	reset();
		if (button_went_down(B_B))	split_asteroid(0);
		
		//
		{
			f32 dir = 0;
			if (button_is_down(B_LEFT))		dir += 1;
			if (button_is_down(B_RIGHT))	dir -= 1;
			
			ship.ori = mymod(ship.ori +dir*deg(180)*dt, RAD_360);
			
			m2 ship_r = rotate2(ship.ori);
			
			f32 vmag = length(ship.vel);
			
			f32 thruster_accel_mag = 60*2.5f;
			f32 drag_accel_mag = 1.25f * vmag;
			
			v2 thuster_accel = 0;
			if (button_is_down(B_UP)) {
				thuster_accel = ship_r * v2(0,thruster_accel_mag);
			}
			
			v2 drag_accel = vmag == 0 ? 0 : normalize(-ship.vel) * drag_accel_mag;
			
			f64 shoot_cooldown = 1.0 / 5.0f;
			
			if (button_is_down(B_SPACE) && (t -t_last_shot) >= shoot_cooldown) {
				shoot(ship_r * v2(0,2) +ship.pos, ship_r * v2(0,bullet_muzzle_vel) +ship.vel);
			}
			
			//
			ship.vel += (thuster_accel +drag_accel) * dt;
			ship.pos += ship.vel * dt;
			
			//ship.pos = cursor_pos_world;
			ship.pos = wrap(ship.pos);
			
			update_asteroids();
			update_bullets();
		}
		print_array(&info, "%.1f %.1f sv: %.2f bullets: %d asteroids %d",
				ship.pos.x,ship.pos.y, length(ship.vel), bullets.len, asteroids.len);
		
		v4 background_out_of_world_col = v4( srgb(80,52,60) * 0.25f, 1 );
		v4 background_col = v4( srgb(41,49,52) * 0.25f, 1 );
		
		glViewport(0, 0, wnd_dim.x, wnd_dim.y);
		
		glClearColor(background_out_of_world_col.x, background_out_of_world_col.y, background_out_of_world_col.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shad_world_col.bind();
		shad_world_col.world_to_clip.set( cam.world_to_clip );
		
		vbo_world_col.bind(shad_world_col);
		
		{ // World rect
			
			auto data = array<Vertex>{
				{ world_radius*v2(+1,-1), background_col },
				{ world_radius*v2(+1,+1), background_col },
				{ world_radius*v2(-1,-1), background_col },
				{ world_radius*v2(-1,-1), background_col },
				{ world_radius*v2(+1,+1), background_col },
				{ world_radius*v2(-1,+1), background_col },
			};
			
			vbo_world_col.upload(data);
			
			glDrawArrays(GL_TRIANGLES, 0, data.len);
		}
		
		{
			m2 r = rotate2(ship.ori);
			
			auto ship_verts = array<v2>{
				r*v2( 0, 0) +ship.pos,
				r*v2(+1,-1) +ship.pos,
				
				r*v2(+1,-1) +ship.pos,
				r*v2( 0,+2) +ship.pos,
				
				r*v2( 0,+2) +ship.pos,
				r*v2(-1,-1) +ship.pos,
				
				r*v2(-1,-1) +ship.pos,
				r*v2( 0, 0) +ship.pos,
			};
			
			draw_with_fake_wrapping(GL_LINES, ship_verts);
		}
		if (bullets.len > 0) {
			
			auto verts = array<v2>::malloc(bullets.len);
			defer { verts.free(); };
			
			for (u32 i=0; i<bullets.len; ++i) {
				verts[i] = bullets[i]->pos;
			}
			
			draw_with_fake_wrapping(GL_POINTS, verts);
		}
		if (asteroids.len > 0) {
			v4 col = v4(1);
			
			auto verts = array<v2>::malloc(Asteroid::VERTEX_COUNTS[Asteroid::BIG]*2 * asteroids.len); // large enough
			defer { verts.free(); };
			v2* out = &verts[0];
			
			for (auto& a : asteroids) {
				u32 count = a->get_vertex_count();
				for (u32 j=0; j<count; ++j) {
					*out++ = a->vertecies[j] +a->pos;
					*out++ = a->vertecies[(j+1)%count] +a->pos;
				}
			}
			
			verts.len = verts.get_i(out);
			
			draw_with_fake_wrapping(GL_LINES, verts);
		}
		#if 0 // colission visualization
		if (asteroids.len > 0) {
			
			for (auto& a : asteroids) {
				Vertex data[20][20];
				
				auto count = Asteroid::VERTEX_COUNTS[ a->size ];
				
				for (u32 j=0; j<20; ++j) {
					for (u32 i=0; i<20; ++i) {
						data[j][i].pos = a->pos +7 * ((v2)iv2(i,j) / 19 * 2 -1);
						data[j][i].col = test_collison(a, data[j][i].pos) ? v4(1,0.25f,0.25f,1) :  v4(0.25f,1,0.25f,1);
					}
				}
				
				vbo_world_col.upload({&data[0][0], 20*20});
				
				glDrawArrays(GL_POINTS, 0, 20*20);
			}
		}
		#endif
		
		
		shad_tex.bind();
		shad_tex.bind_texture(dbg_font.tex);
		
		dbg_font.draw_text_lines(shad_tex, dbg_name_and_fps,	v2(2, -3 +17*1), 1);
		dbg_font.draw_text_lines(shad_tex, info,				v2(2, -3 +17*2), 1);
		
		
	}
	
}

int main (int argc, char** argv) {
	
	//random::init_same_seed_everytime();
	random::init();
	
	setup_glfw();
	
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glPointSize(5);
	
	//glfwSwapInterval(1);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	{ // for ogl 3.3 and up (i'm not using vaos)
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}
	
	//dbg_font.init("c:/windows/fonts/times.ttf"	, 16);
	//dbg_font.init("c:/windows/fonts/arialbd.ttf", 16);
	dbg_font.init("c:/windows/fonts/consola.ttf", 16);
	
	asteroids::init();
	
	bool	dragging = false;
	v2		dragging_grab_pos_world;
	
	dt = 0;
	u64	initial_ts = glfwGetTimerValue();
	u64	prev_frame_end = initial_ts;
	
	for (frame_indx=0;; ++frame_indx) {
		t = (f64)(prev_frame_end -initial_ts) / glfwGetTimerFrequency();
		
		scrollwheel_diff = 0;
		buttons_reset_toggle_count();
		
		glfwPollEvents();
		
		if (glfwWindowShouldClose(wnd)) break;
		if (button_went_down(B_F11)) toggle_fullscreen();
		
		{
			glfwGetFramebufferSize(wnd, &wnd_dim.x, &wnd_dim.y);
			dbg_assert(wnd_dim.x > 0 && wnd_dim.y > 0);
			
			v2 tmp = (v2)wnd_dim;
			wnd_dim_aspect = tmp / v2(tmp.y, tmp.x);
		}
		
		{
			dv2 pos;
			glfwGetCursorPos(wnd, &pos.x, &pos.y);
			cursor_pos = iv2( (s32)floor(pos.x), (s32)floor(pos.y) );
		}
		
		{
			cursor_in_wnd =	   cursor_pos.x >= 0 && cursor_pos.x < wnd_dim.x
							&& cursor_pos.y >= 0 && cursor_pos.y < wnd_dim.y;
			
			auto calc_world_to_clip = [] () {
				f32 radius_scale = 1.0f / cam.radius;
				v2 scale;
				if (wnd_dim.x > wnd_dim.y) {
					scale = v2(wnd_dim_aspect.y * radius_scale, radius_scale);
				} else {
					scale = v2(radius_scale, wnd_dim_aspect.x * radius_scale);
				}
				
				cam.world_to_clip = scale4(v3(scale, 1)) * translate4(v3(-cam.pos_world, 0));
			};
			
			// TODO: is it possible to not calculate this 3 times per frame without changing the behavoir?
			calc_world_to_clip();
			
			v2 tmp2;
			{
				m2 clip_to_cam2 = inverse( cam.world_to_clip.m2() );
				
				v2 clip = ((v2)cursor_pos / (v2)wnd_dim -0.5f) * v2(2,-2); // mouse cursor points to upper left corner of pixel, so no pixel center calculation here
				tmp2 = clip_to_cam2 * clip +cam.pos_world;
			}
			
			{
				f32 tmp = log2(cam.radius);
				tmp += (f32)scrollwheel_diff * -0.1f;
				cam.radius = pow(2.0f, tmp);
			}
			
			calc_world_to_clip();
			
			if (!dragging) {
				if (cursor_in_wnd && button_is_down(B_RMB)) {
					dragging_grab_pos_world = tmp2;
					dragging = true;
				}
			} else {
				if (button_is_up(B_RMB)) {
					dragging = false;
				}
				tmp2 = dragging_grab_pos_world;
			}
			
			{
				m2 clip_to_cam2 = inverse( cam.world_to_clip.m2() );
				
				v2 clip = ((v2)cursor_pos / (v2)wnd_dim -0.5f) * v2(2,-2); // mouse cursor points to upper left corner of pixel, so no pixel center calculation here
				
				cam.pos_world = -(clip_to_cam2 * clip) +tmp2;
			}
			
			calc_world_to_clip();
			
			cursor_pos_world = tmp2; // already out of date value?, should overthink this algorithm
			
		}
		
		asteroids::frame();
		
		glfwSwapBuffers(wnd);
		
		{
			u64 now = glfwGetTimerValue();
			dt = (f32)(now -prev_frame_end) / (f32)glfwGetTimerFrequency();
			prev_frame_end = now;
		}
	}
	
	glfwDestroyWindow(wnd);
	glfwTerminate();
	
	return 0;
}
