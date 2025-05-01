
#include <map>
#include <SDL2/SDL_scancode.h>
#include <gio/gio.h>
#include <display.hpp>

namespace display {
namespace {

struct arg {
	bool error = false;
	XdpSession *session;
	uint32_t node;
	int fd = -1;
};

constexpr auto devices = static_cast<XdpDeviceType>(XDP_DEVICE_POINTER
												  | XDP_DEVICE_KEYBOARD);
std::map<uint32_t, uint32_t> keymap = {
	{ SDL_SCANCODE_ESCAPE,			1 },
	{ SDL_SCANCODE_F1,				59 },
	{ SDL_SCANCODE_F2,				60 },
	{ SDL_SCANCODE_F3,				61 },
	{ SDL_SCANCODE_F4,				62 },
	{ SDL_SCANCODE_F5,				63 },
	{ SDL_SCANCODE_F6,				64 },
	{ SDL_SCANCODE_F7,				65 },
	{ SDL_SCANCODE_F8,				66 },
	{ SDL_SCANCODE_F9,				67 },
	{ SDL_SCANCODE_F10,				68 },
	{ SDL_SCANCODE_F11,				87 },
	{ SDL_SCANCODE_F12,				88 },
	{ SDL_SCANCODE_PRINTSCREEN,		183 },
	{ SDL_SCANCODE_PAUSE,			184 },
	{ SDL_SCANCODE_GRAVE,			41 },
	{ SDL_SCANCODE_1,				2 },
	{ SDL_SCANCODE_2,				3 },
	{ SDL_SCANCODE_3,				4 },
	{ SDL_SCANCODE_4,				5 },
	{ SDL_SCANCODE_5,				6 },
	{ SDL_SCANCODE_6,				7 },
	{ SDL_SCANCODE_7,				8 },
	{ SDL_SCANCODE_8,				9 },
	{ SDL_SCANCODE_9,				10 },
	{ SDL_SCANCODE_0,				11 },
	{ SDL_SCANCODE_MINUS,			12 },
	{ SDL_SCANCODE_EQUALS,			13 },
	{ SDL_SCANCODE_BACKSPACE,		14 },
	{ SDL_SCANCODE_TAB,				15 },
	{ SDL_SCANCODE_Q,				16 },
	{ SDL_SCANCODE_W,				17 },
	{ SDL_SCANCODE_E,				18 },
	{ SDL_SCANCODE_R,				19 },
	{ SDL_SCANCODE_T,				20 },
	{ SDL_SCANCODE_Y,				21 },
	{ SDL_SCANCODE_U,				22 },
	{ SDL_SCANCODE_I,				23 },
	{ SDL_SCANCODE_O,				24 },
	{ SDL_SCANCODE_P,				25 },
	{ SDL_SCANCODE_LEFTBRACKET,		26 },
	{ SDL_SCANCODE_RIGHTBRACKET,	27 },
	{ SDL_SCANCODE_RETURN,			28 },
	{ SDL_SCANCODE_CAPSLOCK,		58 },
	{ SDL_SCANCODE_A,				30 },
	{ SDL_SCANCODE_S,				31 },
	{ SDL_SCANCODE_D,				32 },
	{ SDL_SCANCODE_F,				33 },
	{ SDL_SCANCODE_G,				34 },
	{ SDL_SCANCODE_H,				35 },
	{ SDL_SCANCODE_J,				36 },
	{ SDL_SCANCODE_K,				37 },
	{ SDL_SCANCODE_L,				38 },
	{ SDL_SCANCODE_SEMICOLON,		39 },
	{ SDL_SCANCODE_APOSTROPHE,		40 },
	{ SDL_SCANCODE_BACKSLASH,		43 },
	{ SDL_SCANCODE_Z,				44 },
	{ SDL_SCANCODE_X,				45 },
	{ SDL_SCANCODE_C,				46 },
	{ SDL_SCANCODE_V,				47 },
	{ SDL_SCANCODE_B,				48 },
	{ SDL_SCANCODE_N,				49 },
	{ SDL_SCANCODE_M,				50 },
	{ SDL_SCANCODE_COMMA,			51 },
	{ SDL_SCANCODE_PERIOD,			52 },
	{ SDL_SCANCODE_SLASH,			53 },
	{ SDL_SCANCODE_SPACE,			57 },
	{ SDL_SCANCODE_UP,				103 },
	{ SDL_SCANCODE_DOWN,			108 },
	{ SDL_SCANCODE_LEFT,			105 },
	{ SDL_SCANCODE_RIGHT,			106 },
	{ SDL_SCANCODE_HOME,			102 },
	{ SDL_SCANCODE_END,				107 },
	{ SDL_SCANCODE_INSERT,			110 },
	{ SDL_SCANCODE_DELETE,			111 },
	{ SDL_SCANCODE_KP_DIVIDE,		98 },
	{ SDL_SCANCODE_KP_MULTIPLY,		55 },
	{ SDL_SCANCODE_KP_MINUS,		74 },
	{ SDL_SCANCODE_KP_PLUS,			78 },
	{ SDL_SCANCODE_KP_PERIOD,		83 },
	{ SDL_SCANCODE_KP_EQUALS,		117 },
	{ SDL_SCANCODE_LCTRL,			29 },
	{ SDL_SCANCODE_RCTRL,			97 },
	{ SDL_SCANCODE_LALT,			56 },
	{ SDL_SCANCODE_RALT,			100 },
	{ SDL_SCANCODE_LSHIFT,			42 },
	{ SDL_SCANCODE_RSHIFT,			54 }
};

void
finish(GObject *source, GAsyncResult *res, void *data) {
	arg *ptr = reinterpret_cast<arg *>(data);
	GVariant *streams, *props = nullptr;
	GError *error = nullptr;
	GVariantIter it;

	if (!::xdp_session_start_finish(ptr->session, res, &error)) {
		INFO(WARN"XDP " + std::string(error->message));
		ptr->error = true;
		return;
	}

	ptr->fd = ::xdp_session_open_pipewire_remote(ptr->session);
	if (ptr->fd < 0) {
		INFO(WARN"XDP returned incorrect pipewire descriptor");
		ptr->error = true;
	}

	streams = ::xdp_session_get_streams(ptr->session);
	g_variant_iter_init(&it, streams);
	g_variant_iter_loop(&it, "(u@a{sv})", &ptr->node, &props);
}

void
req(GObject *source, GAsyncResult *res, void *data) {
	XdpPortal *portal = reinterpret_cast<XdpPortal *>(source);
	arg *ptr = reinterpret_cast<arg *>(data);
	GError *error = nullptr;

	ptr->session = ::xdp_portal_create_remote_desktop_session_finish(portal, res, &error);
	if (error) {
		INFO(WARN"Incorrect source: " + std::string(error->message));
		ptr->error = true;
		return;
	}
	if (!ptr->session) {
		INFO(WARN"Can't open xdg session");
		ptr->error = true;
		return;
	}

	::xdp_session_start(ptr->session, nullptr, nullptr, finish, data);
}

}

bool
wayland::init(void) {
	portal = ::xdp_portal_new();
	RET_IF(!portal, false);

	arg arg;
	void *ptr = reinterpret_cast<void *>(&arg);
	::xdp_portal_create_remote_desktop_session(portal, devices, XDP_OUTPUT_MONITOR,
											   XDP_REMOTE_DESKTOP_FLAG_MULTIPLE,
											   XDP_CURSOR_MODE_HIDDEN, nullptr,
											   req, ptr);
	while (arg.fd < 0) {
		RET_IF(arg.error, false);
		g_main_context_iteration(nullptr, true);
	}
	RET_IF(!arg.session, false);

	session = arg.session;
	node = arg.node;
	pwr = new pipewire(arg.fd);
	DIE(!pwr);

	::g_main_context_wakeup(nullptr);
	scr = pwr->connect();
	const bool ok = scr.width && scr.height;
	if (!ok) {
		INFO(WARN"Pipewire can't connect to the screen");
	}
	return ok;
}

void
wayland::close(void) {
	RET_IF(!portal);
	if (pwr) {
		delete pwr;
		pwr = nullptr;
	}
	if (session) {
		::xdp_session_close(session);
	}
}

xy
wayland::res(void) {
	return std::make_pair(scr.width, scr.height);
}

void
wayland::refresh(pixs &arg) {
	arg.ptr = reinterpret_cast<byte *>(pwr->next());
	arg.shift = scr.shift;
	arg.type  = scr.format;
	DIE(!arg.ptr);
}

void
wayland::set(const events &arg) {
	const double x = std::min(static_cast<int>(arg.mouse.first),  scr.width  - 1);
	const double y = std::min(static_cast<int>(arg.mouse.second), scr.height - 1);
	std::map<uint32_t, int>::iterator it;
	uint32_t key;

	::xdp_session_pointer_position(session, node, x, y);
	for (size_t i = 0; i < MAXKEYS; i++) {	
		BREAK_IF(arg.buttons[i].type == display::keys::types::NO_TYPE);
		key = arg.buttons[i].key;

		if (arg.buttons[i].type <= display::keys::types::MOUSE_DOWN) {
			switch (arg.buttons[i].key) {
			case 1:	 key = 0x110;
					 break;
			case 2:	 key = 0x112;
					 break;
			case 3:	 key = 0x111;
					 break;
			default: continue;
			}

			auto state = static_cast<XdpButtonState>(arg.buttons[i].type);
			::xdp_session_pointer_button(session, key, state);
			continue;
		}

		it = keymap.find(arg.buttons[i].key);
		if (it != keymap.end()) {
			key = it->second;
		}
		auto state = static_cast<XdpKeyState>(arg.buttons[i].type - 2);
		::xdp_session_keyboard_key(session, false, key, state);
	}
}

}