
#include <map>
#include <SDL2/SDL_scancode.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <sys/shm.h>
#include <display.hpp>

namespace display {
namespace {

std::map<uint32_t, uint32_t> keymap = {
	{ SDL_SCANCODE_SPACE,			0x20 },
	{ SDL_SCANCODE_APOSTROPHE,		0x27 },
	{ SDL_SCANCODE_COMMA,			0x2c },
	{ SDL_SCANCODE_MINUS,			0x2d },
	{ SDL_SCANCODE_PERIOD,			0x2e },
	{ SDL_SCANCODE_SLASH,			0x2f },
	{ SDL_SCANCODE_0,				0x30 },
	{ SDL_SCANCODE_SEMICOLON,		0x3a },
	{ SDL_SCANCODE_EQUALS,			0x3d },
	{ SDL_SCANCODE_LEFTBRACKET,		0x5b },
	{ SDL_SCANCODE_BACKSLASH,		0x5c },
	{ SDL_SCANCODE_RIGHTBRACKET,	0x5d },
	{ SDL_SCANCODE_GRAVE,			0x60 },
	{ SDL_SCANCODE_RALT,			0xfe03 },
	{ SDL_SCANCODE_BACKSPACE,		0xff08 },
	{ SDL_SCANCODE_TAB,				0xff09 },
	{ SDL_SCANCODE_RETURN,			0xff0d },
	{ SDL_SCANCODE_PAUSE,			0xff13 },
	{ SDL_SCANCODE_SCROLLLOCK,		0xff14 },
	{ SDL_SCANCODE_SYSREQ,			0xff15 },
	{ SDL_SCANCODE_ESCAPE,			0xff1b },
	{ SDL_SCANCODE_HOME,			0xff50 },
	{ SDL_SCANCODE_LEFT,			0xff51 },
	{ SDL_SCANCODE_UP,				0xff52 },
	{ SDL_SCANCODE_RIGHT,			0xff53 },
	{ SDL_SCANCODE_DOWN,			0xff54 },
	{ SDL_SCANCODE_PAGEUP,			0xff55 },
	{ SDL_SCANCODE_PAGEDOWN,		0xff56 },
	{ SDL_SCANCODE_END,				0xff57 },
	{ SDL_SCANCODE_SELECT,			0xff60 },
	{ SDL_SCANCODE_PRINTSCREEN,		0xff61 },
	{ SDL_SCANCODE_EXECUTE,			0xff62 },
	{ SDL_SCANCODE_INSERT,			0xff63 },
	{ SDL_SCANCODE_UNDO,			0xff65 },
	{ SDL_SCANCODE_AGAIN,			0xff66 },
	{ SDL_SCANCODE_MENU,			0xff67 },
	{ SDL_SCANCODE_FIND,			0xff68 },
	{ SDL_SCANCODE_STOP,			0xff69 },
	{ SDL_SCANCODE_HELP,			0xff6a },
	{ SDL_SCANCODE_MODE,			0xff7e },
	{ SDL_SCANCODE_NUMLOCKCLEAR,	0xff7f },
	{ SDL_SCANCODE_KP_ENTER,		0xff8d },
	{ SDL_SCANCODE_KP_7,			0xff95 },
	{ SDL_SCANCODE_KP_4,			0xff96 },
	{ SDL_SCANCODE_KP_8,			0xff97 },
	{ SDL_SCANCODE_KP_6,			0xff98 },
	{ SDL_SCANCODE_KP_2,			0xff99 },
	{ SDL_SCANCODE_KP_9,			0xff9a },
	{ SDL_SCANCODE_KP_3,			0xff9b },
	{ SDL_SCANCODE_KP_1,			0xff9c },
	{ SDL_SCANCODE_KP_5,			0xff9d },
	{ SDL_SCANCODE_KP_0,			0xff9e },
	{ SDL_SCANCODE_KP_PERIOD,		0xff9f },
	{ SDL_SCANCODE_KP_MULTIPLY,		0xffaa },
	{ SDL_SCANCODE_KP_PLUS,			0xffab },
	{ SDL_SCANCODE_KP_COMMA,		0xffac },
	{ SDL_SCANCODE_KP_MINUS,		0xffad },
	{ SDL_SCANCODE_KP_PERIOD,		0xffae },
	{ SDL_SCANCODE_KP_DIVIDE,		0xffaf },
	{ SDL_SCANCODE_KP_0,			0xffb0 },
	{ SDL_SCANCODE_KP_1,			0xffb1 },
	{ SDL_SCANCODE_KP_2,			0xffb2 },
	{ SDL_SCANCODE_KP_3,			0xffb3 },
	{ SDL_SCANCODE_KP_4,			0xffb4 },
	{ SDL_SCANCODE_KP_5,			0xffb5 },
	{ SDL_SCANCODE_KP_6,			0xffb6 },
	{ SDL_SCANCODE_KP_7,			0xffb7 },
	{ SDL_SCANCODE_KP_8,			0xffb8 },
	{ SDL_SCANCODE_KP_9,			0xffb9 },
	{ SDL_SCANCODE_KP_EQUALS,		0xffbd },
	{ SDL_SCANCODE_F1,				0xffbe },
	{ SDL_SCANCODE_F2,				0xffbf },
	{ SDL_SCANCODE_F3,				0xffc0 },
	{ SDL_SCANCODE_F4,				0xffc1 },
	{ SDL_SCANCODE_F5,				0xffc2 },
	{ SDL_SCANCODE_F6,				0xffc3 },
	{ SDL_SCANCODE_F7,				0xffc4 },
	{ SDL_SCANCODE_F8,				0xffc5 },
	{ SDL_SCANCODE_F9,				0xffc6 },
	{ SDL_SCANCODE_F10,				0xffc7 },
	{ SDL_SCANCODE_F11,				0xffc8 },
	{ SDL_SCANCODE_F12,				0xffc9 },
	{ SDL_SCANCODE_F13,				0xffca },
	{ SDL_SCANCODE_F14,				0xffcb },
	{ SDL_SCANCODE_F15,				0xffcc },
	{ SDL_SCANCODE_F16,				0xffcd },
	{ SDL_SCANCODE_F17,				0xffce },
	{ SDL_SCANCODE_F18,				0xffcf },
	{ SDL_SCANCODE_F19,				0xffd0 },
	{ SDL_SCANCODE_F20,				0xffd1 },
	{ SDL_SCANCODE_F21,				0xffd2 },
	{ SDL_SCANCODE_F22,				0xffd3 },
	{ SDL_SCANCODE_F23,				0xffd4 },
	{ SDL_SCANCODE_F24,				0xffd5 },
	{ SDL_SCANCODE_LSHIFT,			0xffe1 },
	{ SDL_SCANCODE_RSHIFT,			0xffe2 },
	{ SDL_SCANCODE_LCTRL,			0xffe3 },
	{ SDL_SCANCODE_RCTRL,			0xffe4 },
	{ SDL_SCANCODE_CAPSLOCK,		0xffe5 },
	{ SDL_SCANCODE_LGUI,			0xffe7 },
	{ SDL_SCANCODE_RGUI,			0xffe8 },
	{ SDL_SCANCODE_LALT,			0xffe9 },
	{ SDL_SCANCODE_RALT,			0xffea },
	{ SDL_SCANCODE_LGUI,			0xffeb },
	{ SDL_SCANCODE_RGUI,			0xffec },
	{ SDL_SCANCODE_APPLICATION,		0xffee },
	{ SDL_SCANCODE_DELETE,			0xffff },
	{ SDL_SCANCODE_A,				0x61 },
	{ SDL_SCANCODE_B,				0x62 },
	{ SDL_SCANCODE_C,				0x63 },
	{ SDL_SCANCODE_D,				0x64 },
	{ SDL_SCANCODE_E,				0x65 },
	{ SDL_SCANCODE_F,				0x66 },
	{ SDL_SCANCODE_G,				0x67 },
	{ SDL_SCANCODE_H,				0x68 },
	{ SDL_SCANCODE_I,				0x69 },
	{ SDL_SCANCODE_J,				0x6a },
	{ SDL_SCANCODE_K,				0x6b },
	{ SDL_SCANCODE_L,				0x6c },
	{ SDL_SCANCODE_M,				0x6d },
	{ SDL_SCANCODE_N,				0x6e },
	{ SDL_SCANCODE_O,				0x6f },
	{ SDL_SCANCODE_P,				0x70 },
	{ SDL_SCANCODE_Q,				0x71 },
	{ SDL_SCANCODE_R,				0x72 },
	{ SDL_SCANCODE_S,				0x73 },
	{ SDL_SCANCODE_T,				0x74 },
	{ SDL_SCANCODE_U,				0x75 },
	{ SDL_SCANCODE_V,				0x76 },
	{ SDL_SCANCODE_W,				0x77 },
	{ SDL_SCANCODE_X,				0x78 },
	{ SDL_SCANCODE_Y,				0x79 },
	{ SDL_SCANCODE_Z,				0x7a },
	{ SDL_SCANCODE_1,				0x31 },
	{ SDL_SCANCODE_2,				0x32 },
	{ SDL_SCANCODE_3,				0x33 },
	{ SDL_SCANCODE_4,				0x34 },
	{ SDL_SCANCODE_5,				0x35 },
	{ SDL_SCANCODE_6,				0x36 },
	{ SDL_SCANCODE_7,				0x37 },
	{ SDL_SCANCODE_8,				0x38 },
	{ SDL_SCANCODE_9,				0x39 }
};

}

bool
x11::init(void) {
	RET_IF(::XInitThreads() == 0, false);
	disp = ::XOpenDisplay(nullptr);
	RET_IF(!disp, false);

	const int depth = DefaultDepth(disp, ::XDefaultScreen(disp));
	root = DefaultRootWindow(disp);
	::XGetWindowAttributes(disp, root, &attrs);
	img = ::XShmCreateImage(disp, DefaultVisual(disp, 0), depth, ZPixmap, nullptr, &shm,
							attrs.width, attrs.height);
	DIE(!img);

	shm.shmid = ::shmget(IPC_PRIVATE, attrs.width * attrs.height * 8, IPC_CREAT | 0777);
	DIE(shm.shmid < 1);

	img->data = reinterpret_cast<char *>(::shmat(shm.shmid, 0, 0));
	shm.readOnly = false;
	shm.shmaddr = img->data;
	DIE(shm.shmaddr == (char *)-1);
	::shmctl(shm.shmid, IPC_RMID, 0);

	::XTestGrabControl(disp, true);
	::XShmAttach(disp, &shm);
	::XSync(disp, true);
	return true;
}

void
x11::close(void) {
	RET_IF(!disp);
	XDestroyImage(img);
	::XTestGrabControl(disp, false);
	::XShmDetach(disp, &shm);
	::shmdt(shm.shmaddr);
	::XCloseDisplay(disp);
	disp = nullptr;
}

xy
x11::res(void) {
	return std::make_pair(attrs.width, attrs.height);
}

void
x11::refresh(pixs &arg) {
	::XShmGetImage(disp, root, img, 0, 0, AllPlanes);
	arg.ptr = reinterpret_cast<byte *>(img->data);
	arg.shift = 4;
	arg.type  = RGBA;
}

void
x11::set(const events &arg) {
	const int x = std::min(static_cast<int>(arg.mouse.first),  attrs.width  - 1);
	const int y = std::min(static_cast<int>(arg.mouse.second), attrs.height - 1);
	std::map<uint32_t, uint32_t>::iterator it;
	uint32_t key;

	::XTestFakeMotionEvent(disp, 0, x, y, 0);
	for (size_t i = 0; i < MAXKEYS; i++) {
		BREAK_IF(arg.buttons[i].type == keys::types::NO_TYPE);
		key = arg.buttons[i].key;
		it = keymap.find(key);
		if (it != keymap.end()) {
			key = it->second;
		}

		if (arg.buttons[i].type > keys::types::MOUSE_DOWN) {
			key = ::XKeysymToKeycode(disp, key);
			::XTestFakeKeyEvent(disp, key, arg.buttons[i].type % 2 != 0, 0);
			continue;
		}
		::XTestFakeButtonEvent(disp, key, arg.buttons[i].type % 2 != 0, 0);
	}
}

}
