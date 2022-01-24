#include <iostream>
#include <SDL2/SDL.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <map>

int main() {
	// If you know how to make it more easy - write me.
	// Part of constants were taken from here:
	// https://github.com/emscripten-ports/SDL2/blob/master/src/video/x11/SDL_x11keyboard.c
	std::map<uint32_t, std::string> list = {
		{ XK_Return, "SDL_SCANCODE_RETURN" },
	    { XK_Escape, "SDL_SCANCODE_ESCAPE" },
	    { XK_BackSpace, "SDL_SCANCODE_BACKSPACE" },
	    { XK_Tab, "SDL_SCANCODE_TAB" },
	    { XK_Caps_Lock, "SDL_SCANCODE_CAPSLOCK" },
	    { XK_F1, "SDL_SCANCODE_F1" },
	    { XK_F2, "SDL_SCANCODE_F2" },
	    { XK_F3, "SDL_SCANCODE_F3" },
	    { XK_F4, "SDL_SCANCODE_F4" },
	    { XK_F5, "SDL_SCANCODE_F5" },
	    { XK_F6, "SDL_SCANCODE_F6" },
	    { XK_F7, "SDL_SCANCODE_F7" },
	    { XK_F8, "SDL_SCANCODE_F8" },
	    { XK_F9, "SDL_SCANCODE_F9" },
	    { XK_F10, "SDL_SCANCODE_F10" },
	    { XK_F11, "SDL_SCANCODE_F11" },
	    { XK_F12, "SDL_SCANCODE_F12" },
	    { XK_Print, "SDL_SCANCODE_PRINTSCREEN" },
	    { XK_Scroll_Lock, "SDL_SCANCODE_SCROLLLOCK" },
	    { XK_Pause, "SDL_SCANCODE_PAUSE" },
	    { XK_Insert, "SDL_SCANCODE_INSERT" },
	    { XK_Home, "SDL_SCANCODE_HOME" },
	    { XK_Prior, "SDL_SCANCODE_PAGEUP" },
	    { XK_Delete, "SDL_SCANCODE_DELETE" },
	    { XK_End, "SDL_SCANCODE_END" },
	    { XK_Next, "SDL_SCANCODE_PAGEDOWN" },
	    { XK_Right, "SDL_SCANCODE_RIGHT" },
	    { XK_Left, "SDL_SCANCODE_LEFT" },
	    { XK_Down, "SDL_SCANCODE_DOWN" },
	    { XK_Up, "SDL_SCANCODE_UP" },
	    { XK_Num_Lock, "SDL_SCANCODE_NUMLOCKCLEAR" },
	    { XK_KP_Divide, "SDL_SCANCODE_KP_DIVIDE" },
	    { XK_KP_Multiply, "SDL_SCANCODE_KP_MULTIPLY" },
	    { XK_KP_Subtract, "SDL_SCANCODE_KP_MINUS" },
	    { XK_KP_Add, "SDL_SCANCODE_KP_PLUS" },
	    { XK_KP_Enter, "SDL_SCANCODE_KP_ENTER" },
	    { XK_KP_Delete, "SDL_SCANCODE_KP_PERIOD" },
	    { XK_KP_End, "SDL_SCANCODE_KP_1" },
	    { XK_KP_Down, "SDL_SCANCODE_KP_2" },
	    { XK_KP_Next, "SDL_SCANCODE_KP_3" },
	    { XK_KP_Left, "SDL_SCANCODE_KP_4" },
	    { XK_KP_Begin, "SDL_SCANCODE_KP_5" },
	    { XK_KP_Right, "SDL_SCANCODE_KP_6" },
	    { XK_KP_Home, "SDL_SCANCODE_KP_7" },
	    { XK_KP_Up, "SDL_SCANCODE_KP_8" },
	    { XK_KP_Prior, "SDL_SCANCODE_KP_9" },
	    { XK_KP_Insert, "SDL_SCANCODE_KP_0" },
	    { XK_KP_Decimal, "SDL_SCANCODE_KP_PERIOD" },
	    { XK_KP_1, "SDL_SCANCODE_KP_1" },
	    { XK_KP_2, "SDL_SCANCODE_KP_2" },
	    { XK_KP_3, "SDL_SCANCODE_KP_3" },
	    { XK_KP_4, "SDL_SCANCODE_KP_4" },
	    { XK_KP_5, "SDL_SCANCODE_KP_5" },
	    { XK_KP_6, "SDL_SCANCODE_KP_6" },
	    { XK_KP_7, "SDL_SCANCODE_KP_7" },
	    { XK_KP_8, "SDL_SCANCODE_KP_8" },
	    { XK_KP_9, "SDL_SCANCODE_KP_9" },
	    { XK_KP_0, "SDL_SCANCODE_KP_0" },
	    { XK_KP_Decimal, "SDL_SCANCODE_KP_PERIOD" },
	    { XK_Hyper_R, "SDL_SCANCODE_APPLICATION" },
	    { XK_KP_Equal, "SDL_SCANCODE_KP_EQUALS" },
	    { XK_F13, "SDL_SCANCODE_F13" },
	    { XK_F14, "SDL_SCANCODE_F14" },
	    { XK_F15, "SDL_SCANCODE_F15" },
	    { XK_F16, "SDL_SCANCODE_F16" },
	    { XK_F17, "SDL_SCANCODE_F17" },
	    { XK_F18, "SDL_SCANCODE_F18" },
	    { XK_F19, "SDL_SCANCODE_F19" },
	    { XK_F20, "SDL_SCANCODE_F20" },
	    { XK_F21, "SDL_SCANCODE_F21" },
	    { XK_F22, "SDL_SCANCODE_F22" },
	    { XK_F23, "SDL_SCANCODE_F23" },
	    { XK_F24, "SDL_SCANCODE_F24" },
	    { XK_Execute, "SDL_SCANCODE_EXECUTE" },
	    { XK_Help, "SDL_SCANCODE_HELP" },
	    { XK_Menu, "SDL_SCANCODE_MENU" },
	    { XK_Select, "SDL_SCANCODE_SELECT" },
	    { XK_Cancel, "SDL_SCANCODE_STOP" },
		{ XK_Redo, "SDL_SCANCODE_AGAIN" },
	    { XK_Undo, "SDL_SCANCODE_UNDO" },
	    { XK_Find, "SDL_SCANCODE_FIND" },
	    { XK_KP_Separator, "SDL_SCANCODE_KP_COMMA" },
	    { XK_Sys_Req, "SDL_SCANCODE_SYSREQ" },
	    { XK_Control_L, "SDL_SCANCODE_LCTRL" },
	    { XK_Shift_L, "SDL_SCANCODE_LSHIFT" },
	    { XK_Alt_L, "SDL_SCANCODE_LALT" },
	    { XK_Meta_L, "SDL_SCANCODE_LGUI" },
	    { XK_Super_L, "SDL_SCANCODE_LGUI" },
	    { XK_Control_R, "SDL_SCANCODE_RCTRL" },
	    { XK_Shift_R, "SDL_SCANCODE_RSHIFT" },
	    { XK_Alt_R, "SDL_SCANCODE_RALT" },
	    { XK_ISO_Level3_Shift, "SDL_SCANCODE_RALT" },
	    { XK_Meta_R, "SDL_SCANCODE_RGUI" },
	    { XK_Super_R, "SDL_SCANCODE_RGUI" },
	    { XK_Mode_switch, "SDL_SCANCODE_MODE" },
	    { XK_period, "SDL_SCANCODE_PERIOD" },
	    { XK_comma, "SDL_SCANCODE_COMMA" },
	    { XK_slash, "SDL_SCANCODE_SLASH" },
	    { XK_backslash, "SDL_SCANCODE_BACKSLASH" },
	    { XK_minus, "SDL_SCANCODE_MINUS" },
	    { XK_equal, "SDL_SCANCODE_EQUALS" },
	    { XK_space, "SDL_SCANCODE_SPACE" },
	    { XK_grave, "SDL_SCANCODE_GRAVE" },
	    { XK_apostrophe, "SDL_SCANCODE_APOSTROPHE" },
	    { XK_bracketleft, "SDL_SCANCODE_LEFTBRACKET" },
	    { XK_bracketright, "SDL_SCANCODE_RIGHTBRACKET" },
	    { XK_0, "SDL_SCANCODE_0"},
	    { XK_colon, "SDL_SCANCODE_SEMICOLON"}
	};
	
	for (auto &p : list) {
		std::cout << "{ " << p.second << ", 0x"
				  << std::hex << p.first << " },\n";
	}
	for (uint8_t i = 0; i < 26; i++) {
		std::cout << "{ " << "SDL_SCANCODE_"
				  << (char)(0x41 + i) << ", 0x"
				  << std::hex << XK_a + i << " },\n";
	}
	for (uint8_t i = 0; i < 9; i++) {
		std::cout << "{ " << "SDL_SCANCODE_"
				  << (i + 1) << ", 0x"
				  << std::hex << XK_1 + i << " },\n";
	}
}