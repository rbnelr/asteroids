
enum button_e : u32 {
	B_LMB				=0,
	B_RMB				,
	B_MMB				,
	B_MOUSE_4			,
	B_MOUSE_5			,
	B_MOUSE_6			,
	B_MOUSE_7			,
	B_MOUSE_8			,
	//
	B_SPACE				,
	B_APOSTROPHE		,
	B_COMMA				,
	B_MINUS				,
	B_PERIOD			,
	B_SLASH				,
	B_0					,
	B_1					,
	B_2					,
	B_3					,
	B_4					,
	B_5					,
	B_6					,
	B_7					,
	B_8					,
	B_9					,
	B_SEMICOLON			,
	B_EQUAL				,
	B_A					,
	B_B					,
	B_C					,
	B_D					,
	B_E					,
	B_F					,
	B_G					,
	B_H					,
	B_I					,
	B_J					,
	B_K					,
	B_L					,
	B_M					,
	B_N					,
	B_O					,
	B_P					,
	B_Q					,
	B_R					,
	B_S					,
	B_T					,
	B_U					,
	B_V					,
	B_W					,
	B_X					,
	B_Y					,
	B_Z					,
	B_LEFT_BRACKET		,
	B_BACKSLASH			,
	B_RIGHT_BRACKET		,
	B_GRAVE_ACCENT		,
	B_WORLD_1			,
	B_WORLD_2			,
	B_ESCAPE			,
	B_ENTER				,
	B_TAB				,
	B_BACKSPACE			,
	B_INSERT			,
	B_DELETE			,
	B_RIGHT				,
	B_LEFT				,
	B_DOWN				,
	B_UP				,
	B_PAGE_UP			,
	B_PAGE_DOWN			,
	B_HOME				,
	B_END				,
	B_CAPS_LOCK			,
	B_SCROLL_LOCK		,
	B_NUM_LOCK			,
	B_PRINT_SCREEN		,
	B_PAUSE				,
	B_F1				,
	B_F2				,
	B_F3				,
	B_F4				,
	B_F5				,
	B_F6				,
	B_F7				,
	B_F8				,
	B_F9				,
	B_F10				,
	B_F11				,
	B_F12				,
	B_NP_0				,
	B_NP_1				,
	B_NP_2				,
	B_NP_3				,
	B_NP_4				,
	B_NP_5				,
	B_NP_6				,
	B_NP_7				,
	B_NP_8				,
	B_NP_9				,
	B_NP_DECIMAL		,
	B_NP_DIVIDE			,
	B_NP_MULTIPLY		,
	B_NP_SUBTRACT		,
	B_NP_ADD			,
	B_NP_ENTER			,
	B_NP_EQUAL			,
	B_LEFT_SHIFT		,
	B_LEFT_CONTROL		,
	B_LEFT_ALT			,
	B_LEFT_SUPER		,
	B_RIGHT_SHIFT		,
	B_RIGHT_CONTROL		,
	B_RIGHT_ALT			,
	B_RIGHT_SUPER		,
	B_MENU				,
	
	BUTTONS_COUNT		,
	BUTTONS_FIRST		=0,
	B_IGNORED			=BUTTONS_COUNT
};
DEFINE_ENUM_ITER_OPS(button_e, u32)

constexpr cstr button_names[BUTTONS_COUNT] = {
	"B_LMB",
	"B_RMB",
	"B_MMB",
	"B_MOUSE_4",
	"B_MOUSE_5",
	"B_MOUSE_6",
	"B_MOUSE_7",
	"B_MOUSE_8",
	//
	"B_SPACE",
	"B_APOSTROPHE",
	"B_COMMA",
	"B_MINUS",
	"B_PERIOD",
	"B_SLASH",
	"B_0",
	"B_1",
	"B_2",
	"B_3",
	"B_4",
	"B_5",
	"B_6",
	"B_7",
	"B_8",
	"B_9",
	"B_SEMICOLON",
	"B_EQUAL",
	"B_A",
	"B_B",
	"B_C",
	"B_D",
	"B_E",
	"B_F",
	"B_G",
	"B_H",
	"B_I",
	"B_J",
	"B_K",
	"B_L",
	"B_M",
	"B_N",
	"B_O",
	"B_P",
	"B_Q",
	"B_R",
	"B_S",
	"B_T",
	"B_U",
	"B_V",
	"B_W",
	"B_X",
	"B_Y",
	"B_Z",
	"B_LEFT_BRACKET",
	"B_BACKSLASH",
	"B_RIGHT_BRACKET",
	"B_GRAVE_ACCENT",
	"B_WORLD_1",
	"B_WORLD_2",
	"B_ESCAPE",
	"B_ENTER",
	"B_TAB",
	"B_BACKSPACE",
	"B_INSERT",
	"B_DELETE",
	"B_RIGHT",
	"B_LEFT",
	"B_DOWN",
	"B_UP",
	"B_PAGE_UP",
	"B_PAGE_DOWN",
	"B_HOME",
	"B_END",
	"B_CAPS_LOCK",
	"B_SCROLL_LOCK",
	"B_NUM_LOCK",
	"B_PRINT_SCREEN",
	"B_PAUSE",
	"B_F1",
	"B_F2",
	"B_F3",
	"B_F4",
	"B_F5",
	"B_F6",
	"B_F7",
	"B_F8",
	"B_F9",
	"B_F10",
	"B_F11",
	"B_F12",
	"B_NP_0",
	"B_NP_1",
	"B_NP_2",
	"B_NP_3",
	"B_NP_4",
	"B_NP_5",
	"B_NP_6",
	"B_NP_7",
	"B_NP_8",
	"B_NP_9",
	"B_NP_DECIMAL",
	"B_NP_DIVIDE",
	"B_NP_MULTIPLY",
	"B_NP_SUBTRACT",
	"B_NP_ADD",
	"B_NP_ENTER",
	"B_NP_EQUAL",
	"B_LEFT_SHIFT",
	"B_LEFT_CONTROL",
	"B_LEFT_ALT",
	"B_LEFT_SUPER",
	"B_RIGHT_SHIFT",
	"B_RIGHT_CONTROL",
	"B_RIGHT_ALT",
	"B_RIGHT_SUPER",
	"B_MENU",
};

static button_e glfw_key_to_rz_button (int glfw_key) {
	switch (glfw_key) {
		case GLFW_KEY_SPACE			:	return B_SPACE			;
		case GLFW_KEY_APOSTROPHE	:	return B_APOSTROPHE		;
		case GLFW_KEY_COMMA			:	return B_COMMA			;
		case GLFW_KEY_MINUS			:	return B_MINUS			;
		case GLFW_KEY_PERIOD		:	return B_PERIOD			;
		case GLFW_KEY_SLASH			:	return B_SLASH			;
		case GLFW_KEY_0				:	return B_0				;
		case GLFW_KEY_1				:	return B_1				;
		case GLFW_KEY_2				:	return B_2				;
		case GLFW_KEY_3				:	return B_3				;
		case GLFW_KEY_4				:	return B_4				;
		case GLFW_KEY_5				:	return B_5				;
		case GLFW_KEY_6				:	return B_6				;
		case GLFW_KEY_7				:	return B_7				;
		case GLFW_KEY_8				:	return B_8				;
		case GLFW_KEY_9				:	return B_9				;
		case GLFW_KEY_SEMICOLON		:	return B_SEMICOLON		;
		case GLFW_KEY_EQUAL			:	return B_EQUAL			;
		case GLFW_KEY_A				:	return B_A				;
		case GLFW_KEY_B				:	return B_B				;
		case GLFW_KEY_C				:	return B_C				;
		case GLFW_KEY_D				:	return B_D				;
		case GLFW_KEY_E				:	return B_E				;
		case GLFW_KEY_F				:	return B_F				;
		case GLFW_KEY_G				:	return B_G				;
		case GLFW_KEY_H				:	return B_H				;
		case GLFW_KEY_I				:	return B_I				;
		case GLFW_KEY_J				:	return B_J				;
		case GLFW_KEY_K				:	return B_K				;
		case GLFW_KEY_L				:	return B_L				;
		case GLFW_KEY_M				:	return B_M				;
		case GLFW_KEY_N				:	return B_N				;
		case GLFW_KEY_O				:	return B_O				;
		case GLFW_KEY_P				:	return B_P				;
		case GLFW_KEY_Q				:	return B_Q				;
		case GLFW_KEY_R				:	return B_R				;
		case GLFW_KEY_S				:	return B_S				;
		case GLFW_KEY_T				:	return B_T				;
		case GLFW_KEY_U				:	return B_U				;
		case GLFW_KEY_V				:	return B_V				;
		case GLFW_KEY_W				:	return B_W				;
		case GLFW_KEY_X				:	return B_X				;
		case GLFW_KEY_Y				:	return B_Y				;
		case GLFW_KEY_Z				:	return B_Z				;
		case GLFW_KEY_LEFT_BRACKET	:	return B_LEFT_BRACKET	;
		case GLFW_KEY_BACKSLASH		:	return B_BACKSLASH		;
		case GLFW_KEY_RIGHT_BRACKET	:	return B_RIGHT_BRACKET	;
		case GLFW_KEY_GRAVE_ACCENT	:	return B_GRAVE_ACCENT	;
		case GLFW_KEY_WORLD_1		:	return B_WORLD_1		;
		case GLFW_KEY_WORLD_2		:	return B_WORLD_2		;
		case GLFW_KEY_ESCAPE		:	return B_ESCAPE			;
		case GLFW_KEY_ENTER			:	return B_ENTER			;
		case GLFW_KEY_TAB			:	return B_TAB			;
		case GLFW_KEY_BACKSPACE		:	return B_BACKSPACE		;
		case GLFW_KEY_INSERT		:	return B_INSERT			;
		case GLFW_KEY_DELETE		:	return B_DELETE			;
		case GLFW_KEY_RIGHT			:	return B_RIGHT			;
		case GLFW_KEY_LEFT			:	return B_LEFT			;
		case GLFW_KEY_DOWN			:	return B_DOWN			;
		case GLFW_KEY_UP			:	return B_UP				;
		case GLFW_KEY_PAGE_UP		:	return B_PAGE_UP		;
		case GLFW_KEY_PAGE_DOWN		:	return B_PAGE_DOWN		;
		case GLFW_KEY_HOME			:	return B_HOME			;
		case GLFW_KEY_END			:	return B_END			;
		case GLFW_KEY_CAPS_LOCK		:	return B_CAPS_LOCK		;
		case GLFW_KEY_SCROLL_LOCK	:	return B_SCROLL_LOCK	;
		case GLFW_KEY_NUM_LOCK		:	return B_NUM_LOCK		;
		case GLFW_KEY_PRINT_SCREEN	:	return B_PRINT_SCREEN	;
		case GLFW_KEY_PAUSE			:	return B_PAUSE			;
		case GLFW_KEY_F1			:	return B_F1				;
		case GLFW_KEY_F2			:	return B_F2				;
		case GLFW_KEY_F3			:	return B_F3				;
		case GLFW_KEY_F4			:	return B_F4				;
		case GLFW_KEY_F5			:	return B_F5				;
		case GLFW_KEY_F6			:	return B_F6				;
		case GLFW_KEY_F7			:	return B_F7				;
		case GLFW_KEY_F8			:	return B_F8				;
		case GLFW_KEY_F9			:	return B_F9				;
		case GLFW_KEY_F10			:	return B_F10			;
		case GLFW_KEY_F11			:	return B_F11			;
		case GLFW_KEY_F12			:	return B_F12			;
		case GLFW_KEY_KP_0			:	return B_NP_0			;
		case GLFW_KEY_KP_1			:	return B_NP_1			;
		case GLFW_KEY_KP_2			:	return B_NP_2			;
		case GLFW_KEY_KP_3			:	return B_NP_3			;
		case GLFW_KEY_KP_4			:	return B_NP_4			;
		case GLFW_KEY_KP_5			:	return B_NP_5			;
		case GLFW_KEY_KP_6			:	return B_NP_6			;
		case GLFW_KEY_KP_7			:	return B_NP_7			;
		case GLFW_KEY_KP_8			:	return B_NP_8			;
		case GLFW_KEY_KP_9			:	return B_NP_9			;
		case GLFW_KEY_KP_DECIMAL	:	return B_NP_DECIMAL		;
		case GLFW_KEY_KP_DIVIDE		:	return B_NP_DIVIDE		;
		case GLFW_KEY_KP_MULTIPLY	:	return B_NP_MULTIPLY	;
		case GLFW_KEY_KP_SUBTRACT	:	return B_NP_SUBTRACT	;
		case GLFW_KEY_KP_ADD		:	return B_NP_ADD			;
		case GLFW_KEY_KP_ENTER		:	return B_NP_ENTER		;
		case GLFW_KEY_KP_EQUAL		:	return B_NP_EQUAL		;
		case GLFW_KEY_LEFT_SHIFT	:	return B_LEFT_SHIFT		;
		case GLFW_KEY_LEFT_CONTROL	:	return B_LEFT_CONTROL	;
		case GLFW_KEY_LEFT_ALT		:	return B_LEFT_ALT		;
		case GLFW_KEY_LEFT_SUPER	:	return B_LEFT_SUPER		;
		case GLFW_KEY_RIGHT_SHIFT	:	return B_RIGHT_SHIFT	;
		case GLFW_KEY_RIGHT_CONTROL	:	return B_RIGHT_CONTROL	;
		case GLFW_KEY_RIGHT_ALT		:	return B_RIGHT_ALT		;
		case GLFW_KEY_RIGHT_SUPER	:	return B_RIGHT_SUPER	;
		case GLFW_KEY_MENU			:	return B_MENU			;
		case GLFW_KEY_F13			:
		case GLFW_KEY_F14			:
		case GLFW_KEY_F15			:
		case GLFW_KEY_F16			:
		case GLFW_KEY_F17			:
		case GLFW_KEY_F18			:
		case GLFW_KEY_F19			:
		case GLFW_KEY_F20			:
		case GLFW_KEY_F21			:
		case GLFW_KEY_F22			:
		case GLFW_KEY_F23			:
		case GLFW_KEY_F24			:
		case GLFW_KEY_F25			:
		default				:	return B_IGNORED;
	}
}
static button_e glfw_mouse_button_to_rz_button (int glfw_mouse_button) {
	switch (glfw_mouse_button) {
		case GLFW_MOUSE_BUTTON_LEFT		:	return B_LMB		;
		case GLFW_MOUSE_BUTTON_RIGHT	:	return B_RMB		;
		case GLFW_MOUSE_BUTTON_MIDDLE	:	return B_MMB		;
		case GLFW_MOUSE_BUTTON_4		:	return B_MOUSE_4	;
		case GLFW_MOUSE_BUTTON_5		:	return B_MOUSE_5	;
		case GLFW_MOUSE_BUTTON_6		:	return B_MOUSE_6	;
		case GLFW_MOUSE_BUTTON_7		:	return B_MOUSE_7	;
		case GLFW_MOUSE_BUTTON_8		:	return B_MOUSE_8	;
		default				:	return B_IGNORED;
	}
}

struct Button {
	bool	is_down;
	u8		toggle_count; // since last frame (processing happens every frame)
	
};

Button buttons[BUTTONS_COUNT] = {}; // init to not-pressed

static bool button_is_down (button_e id) {
	return buttons[id].is_down;
}
static bool button_is_up (button_e id) {
	return !buttons[id].is_down;
}
static bool button_went_down (button_e id) {
	if (buttons[id].is_down) {
		return buttons[id].toggle_count > 0;
	} else {
		return buttons[id].toggle_count > 1;
	}
}
static bool button_went_up (button_e id) {
	if (buttons[id].is_down) {
		return buttons[id].toggle_count > 1;
	} else {
		return buttons[id].toggle_count > 0;
	}
}

static void buttons_reset_toggle_count () {
	for (button_e id=BUTTONS_FIRST; id<BUTTONS_COUNT; ++id) {
		buttons[id].toggle_count = 0;
	}
}
static void process_generic_button (button_e id, int action) {
	
	auto& b = buttons[id];
	
	dbg_assert(action == GLFW_RELEASE || action == GLFW_PRESS);
	bool new_state = action != GLFW_RELEASE;
	
	if (b.is_down != new_state) {
		dbg_assert(b.toggle_count < 255);
		++b.toggle_count;
	}
	b.is_down = new_state;
	
}

static void glfw_key_proc (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_REPEAT) return;
	
	auto id = glfw_key_to_rz_button(key);
	if (id == B_IGNORED) return;
	
	dbg_assert(action == GLFW_RELEASE || action == GLFW_PRESS);
	
	cstr name = glfwGetKeyName(key, scancode);
	//printf("Button %s: %d\n", name ? name : "<null>", new_state);
	
	process_generic_button(id, action);
}

static void glfw_mousebutton_proc (GLFWwindow* window, int button, int action, int mods) {
	auto id = glfw_mouse_button_to_rz_button(button);
	if (id == B_IGNORED) return;
	
	//printf(">>> %s %d\n", button_names[id], action);
	
	process_generic_button(id, action);
}
