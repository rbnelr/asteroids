
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct File_Data {
	byte*	data;
	u64		size;
	
	void free () { ::free(data); }
};
static File_Data load_file (cstr filename) {
	auto f = fopen(filename, "rb");
	if (!f) return {}; // fail
	defer { fclose(f); };
	
	fseek(f, 0, SEEK_END);
	u64 file_size = ftell(f); // only 32 support for now
	rewind(f);
	
	byte* data = (byte*)malloc(file_size);
	
	auto ret = fread(data, 1,file_size, f);
	dbg_assert(ret == file_size);
	file_size = ret;
	
	return {data,file_size};
}

struct Texture {
	GLuint	gl;
	u8*		data;
	u32		w;
	u32		h;
	
	void alloc (u32 w, u32 h) {
		glGenTextures(1, &gl);
		this->w = w;
		this->h = h;
		data = (u8*)::malloc(w*h*sizeof(u8));
	}
	
	u8* operator[] (u32 row) { return &data[row*w]; }
	
	void inplace_vertical_flip () {
		u8* row_a = (*this)[0];
		u8* row_b = (*this)[h -1];
		for (u32 y=0; y<(h/2); ++y) {
			dbg_assert(row_a < row_b);
			for (u32 x=0; x<w; ++x) {
				u8 tmp = row_a[x];
				row_a[x] = row_b[x];
				row_b[x] = tmp;
			}
			row_a += w;
			row_b -= w;
		}
	}
};

namespace font {
	namespace mapping {
		#define ASCII_FIRST		' '
		#define ASCII_LAST		'~'
		enum ascii_e {
			ASCII_INDX			=0,
			ASCII_NUM			=(ASCII_LAST -ASCII_FIRST) +1
		};
		
		enum de_e {
			DE_INDX				=ASCII_INDX +ASCII_NUM,
			DE_UU				=0,
			DE_AE				,
			DE_OE				,
			DE_UE				,
			DE_ae				,
			DE_oe				,
			DE_ue				,
			DE_NUM
		};
		static constexpr utf32 DE_CHARS[DE_NUM] = {
			/* DE_UU			*/	U'ß',
			/* DE_AE			*/	U'Ä',
			/* DE_OE			*/	U'Ö',
			/* DE_UE			*/	U'Ü',
			/* DE_ae			*/	U'ä',
			/* DE_oe			*/	U'ö',
			/* DE_ue			*/	U'ü',
		};
		
		enum jp_e {
			JP_INDX				=DE_INDX +DE_NUM,
			JP_SPACE			=0,
			JP_COMMA			,
			JP_PEROID			,
			JP_SEP_DOT			,
			JP_DASH				,
			JP_UNDERSCORE		,
			JP_BRACKET_OPEN		,
			JP_BRACKET_CLOSE	,
			JP_NUM
		};
		static constexpr utf32 JP_CHARS[JP_NUM] = {
			/* JP_SPACE			*/	U'　',
			/* JP_COMMA			*/	U'、',
			/* JP_PEROID		*/	U'。',
			/* JP_SEP_DOT		*/	U'・',
			/* JP_DASH			*/	U'ー',
			/* JP_UNDERSCORE	*/	U'＿',
			/* JP_BRACKET_OPEN	*/	U'「',
			/* JP_BRACKET_CLOSE	*/	U'」',
			
		};
		
		#define JP_HG_FIRST	U'あ'
		#define JP_HG_LAST	U'ゖ'
		enum jp_hg_e {
			JP_HG_INDX		=JP_INDX +JP_NUM,
			JP_HG_NUM		=(JP_HG_LAST -JP_HG_FIRST) +1
		};
		
		#define TOTAL_CHARS	(JP_HG_INDX +JP_HG_NUM)
		
		static int map_char (char c) {
			return (s32)(c -ASCII_FIRST) +ASCII_INDX;
		}
		static int map_char (utf32 u) {
			if (u >= ASCII_FIRST && u <= ASCII_LAST) {
				return map_char((char)u);
			}
			switch (u) {
				case DE_CHARS[DE_UU]:		return DE_UU +DE_INDX;
				case DE_CHARS[DE_AE]:		return DE_AE +DE_INDX;
				case DE_CHARS[DE_OE]:		return DE_OE +DE_INDX;
				case DE_CHARS[DE_UE]:		return DE_UE +DE_INDX;
				case DE_CHARS[DE_ae]:		return DE_ae +DE_INDX;
				case DE_CHARS[DE_oe]:		return DE_oe +DE_INDX;
				case DE_CHARS[DE_ue]:		return DE_ue +DE_INDX;
				
				case JP_CHARS[JP_SPACE			]:	return JP_SPACE			+JP_INDX;
				case JP_CHARS[JP_COMMA			]:	return JP_COMMA			+JP_INDX;
				case JP_CHARS[JP_PEROID			]:	return JP_PEROID		+JP_INDX;
				case JP_CHARS[JP_SEP_DOT		]:	return JP_SEP_DOT		+JP_INDX;
				case JP_CHARS[JP_DASH			]:	return JP_DASH			+JP_INDX;
				case JP_CHARS[JP_UNDERSCORE		]:	return JP_UNDERSCORE	+JP_INDX;
				case JP_CHARS[JP_BRACKET_OPEN	]:	return JP_BRACKET_OPEN	+JP_INDX;
				case JP_CHARS[JP_BRACKET_CLOSE	]:	return JP_BRACKET_CLOSE	+JP_INDX;
				
			}
			if (u >= JP_HG_FIRST && u <= JP_HG_LAST) {
				return (s32)(u -JP_HG_FIRST) +JP_HG_INDX;
			}
			
			dbg_assert(false, "Char '%c' [%x] missing in font", u, u);
			return map_char('!'); // missing char
		}
	}
	using namespace mapping;
	
	struct Font {
		Texture					tex;
		VBO_Pos_Tex_Col			vbo;
		
		stbtt_packedchar		chars[TOTAL_CHARS];
		
		bool init (cstr filepath, u32 fontsize=16) {
			
			vbo.init();
			
			auto f = load_file(filepath);
			defer { f.free(); };
			
			auto jp_f = load_file("c:/windows/fonts/meiryo.ttc"); // always use meiryo for now
			defer { jp_f.free(); };
			
			bool big = 0;
			
			u32 texw, texh;
			f32 sz, jpsz;
			switch (fontsize) {
				case 16:	sz=16; jpsz=24;	texw=256;texh=128+16;	break;
				case 42:	sz=42; jpsz=64;	texw=512;texh=512-128;	break;
				default: dbg_assert(false, "not implemented"); texw=0; texh=0; sz=0; jpsz=0;
			}
			tex.alloc(texw, texh);
			
			stbtt_pack_context spc;
			stbtt_PackBegin(&spc, tex.data, (s32)tex.w,(s32)tex.h, (s32)tex.w, 1, nullptr);
			
			//stbtt_PackSetOversampling(&spc, 1,1);
			
			
			stbtt_pack_range ranges[] = {
				{ sz, ASCII_FIRST, nullptr, ASCII_NUM, &chars[ASCII_INDX] },
				{ sz, 0, (int*)&DE_CHARS, DE_NUM, &chars[DE_INDX] },
			};
			dbg_assert( stbtt_PackFontRanges(&spc, f.data, 0, ranges, arrlent(s32, ranges)) > 0);
			
			
			stbtt_pack_range ranges_jp[] = {
				{ jpsz, 0, (int*)&JP_CHARS, JP_NUM, &chars[JP_INDX] },
				{ jpsz, JP_HG_FIRST, nullptr, JP_HG_NUM, &chars[JP_HG_INDX] },
			};
			dbg_assert( stbtt_PackFontRanges(&spc, jp_f.data, 0, ranges_jp, arrlent(s32, ranges_jp)) > 0);
			
			stbtt_PackEnd(&spc);
			
			tex.inplace_vertical_flip(); // TODO: could get rid of this simply by flipping the uv's of the texture
			
			glBindTexture(GL_TEXTURE_2D, tex.gl);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, tex.w,tex.h, 0, GL_RED, GL_UNSIGNED_BYTE, tex.data);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL,	0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,	0);
			return true;
		}
		
		static array<utf32> utf8_to_utf32 (array<utf8 const> cr str) {
			utf8 const* cur = str.arr;
			
			auto ret = array<utf32>::malloc(str.len); // can never be longer than input
			utf32* out = ret.arr;
			
			for (;;) {
				if ((*cur & 0b10000000) == 0b00000000) {
					char c = *cur++;
					*out++ = c;
					if (c == '\0') break;
					continue;
				}
				if ((*cur & 0b11100000) == 0b11000000) {
					dbg_assert((cur[1] & 0b11000000) == 0b10000000);
					utf8 a = *cur++ & 0b00011111;
					utf8 b = *cur++ & 0b00111111;
					*out++ = a<<6|b;
					continue;
				}
				if ((*cur & 0b11110000) == 0b11100000) {
					dbg_assert((cur[1] & 0b11000000) == 0b10000000);
					dbg_assert((cur[2] & 0b11000000) == 0b10000000);
					utf8 a = *cur++ & 0b00001111;
					utf8 b = *cur++ & 0b00111111;
					utf8 c = *cur++ & 0b00111111;
					*out++ = a<<12|b<<6|c;
					continue;
				}
				if ((*cur & 0b11111000) == 0b11110000) {
					dbg_assert((cur[1] & 0b11000000) == 0b10000000);
					dbg_assert((cur[2] & 0b11000000) == 0b10000000);
					dbg_assert((cur[3] & 0b11000000) == 0b10000000);
					utf8 a = *cur++ & 0b00000111;
					utf8 b = *cur++ & 0b00111111;
					utf8 c = *cur++ & 0b00111111;
					utf8 d = *cur++ & 0b00111111;
					*out++ = a<<18|b<<12|c<<6|d;
					continue;
				}
				dbg_assert(false);
			}
			
			ret.len = out -ret.arr;
			return ret;
		}
		
		//void draw_text_lines (Basic_Shader cr shad, array< array<utf8>* > text_lines, v2 pos_screen, v4 col) {
		void draw_text_lines (Basic_Shader cr shad, array<utf8 const> cr text_, v2 pos_screen, v4 col) {
			
			v2 pos = v2(pos_screen.x, pos_screen.y -wnd_dim.y);
			
			constexpr v2 _quad[] = {
				v2(1,0),
				v2(1,1),
				v2(0,0),
				v2(0,0),
				v2(1,1),
				v2(0,1),
			};
			
			dbg_assert(text_.len > 0);
			
			array<utf32> line = utf8_to_utf32(text_);
			dbg_assert(line.len > 0);
			defer { line.free(); };
			
			#define SHOW_TEXTURE 0
			
			auto text_data = array<VBO_Pos_Tex_Col::V>::malloc( line.len * 6
					#if SHOW_TEXTURE
					+6
					#endif
					);
			
			auto* out = &text_data[0];
			
			for (u32 i=0; i<line.len-1; ++i) {
				utf32 c = line[i];
				
				stbtt_aligned_quad quad;
				
				stbtt_GetPackedQuad(chars, (s32)tex.w,(s32)tex.h, map_char(c),
						&pos.x,&pos.y, &quad, 1);
				
				for (u32 vert_i=0; vert_i<6; ++vert_i) {
					out->pos =	lerp(v2(quad.x0,-quad.y0), v2(quad.x1,-quad.y1), _quad[vert_i]) / (v2)wnd_dim * 2 -1;
					out->uv =	lerp(v2(quad.s0,-quad.t0), v2(quad.s1,-quad.t1), _quad[vert_i]);
					out->col =	col;
					++out;
				}
			}
			
			#if SHOW_TEXTURE
			for (u32 j=0; j<6; ++j) {
				out->pos =	lerp( ((v2)wnd_dim -v2((f32)tex.w,(f32)tex.h)) / (v2)wnd_dim * 2 -1, 1, _quad[j]);
				out->uv =	_quad[j];
				out->col =	col;
				++out;
			}
			#endif
			
			#undef SHOW_TEXTURE
			
			vbo.upload(text_data);
			vbo.bind(shad);
			
			glDrawArrays(GL_TRIANGLES, 0, text_data.len);
		}
	};
	
}
