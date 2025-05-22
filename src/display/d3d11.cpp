
#include <map>
#include <SDL2/SDL_scancode.h>
#include <display.hpp>

namespace display {
namespace {

constexpr D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_1
};

constexpr D3D_DRIVER_TYPE drivers[] = {
	D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE
};

std::map<uint32_t, uint32_t> keymap = {
	{ SDL_SCANCODE_ESCAPE,			0x01 },
	{ SDL_SCANCODE_1,				0x02 },
	{ SDL_SCANCODE_2,				0x03 },
	{ SDL_SCANCODE_3,				0x04 },
	{ SDL_SCANCODE_4,				0x05 },
	{ SDL_SCANCODE_5,				0x06 },
	{ SDL_SCANCODE_6,				0x07 },
	{ SDL_SCANCODE_7,				0x08 },
	{ SDL_SCANCODE_8,				0x09 },
	{ SDL_SCANCODE_9,				0x0a },
	{ SDL_SCANCODE_0,				0x0b },
	{ SDL_SCANCODE_MINUS,			0x0c },
	{ SDL_SCANCODE_EQUALS,			0x0d },
	{ SDL_SCANCODE_BACKSPACE,		0x0e },
	{ SDL_SCANCODE_TAB,				0x0f },
	{ SDL_SCANCODE_Q,				0x10 },
	{ SDL_SCANCODE_W,				0x11 },
	{ SDL_SCANCODE_E,				0x12 },
	{ SDL_SCANCODE_R,				0x13 },
	{ SDL_SCANCODE_T,				0x14 },
	{ SDL_SCANCODE_Y,				0x15 },
	{ SDL_SCANCODE_U,				0x16 },
	{ SDL_SCANCODE_I,				0x17 },
	{ SDL_SCANCODE_O,				0x18 },
	{ SDL_SCANCODE_P,				0x19 },
	{ SDL_SCANCODE_LEFTBRACKET,		0x1a },
	{ SDL_SCANCODE_RIGHTBRACKET,	0x1b },
	{ SDL_SCANCODE_RETURN,			0x1c },
	{ SDL_SCANCODE_LCTRL,			0x1d },
	{ SDL_SCANCODE_A,				0x1e },
	{ SDL_SCANCODE_S,				0x1f },
	{ SDL_SCANCODE_D,				0x20 },
	{ SDL_SCANCODE_F,				0x21 },
	{ SDL_SCANCODE_G,				0x22 },
	{ SDL_SCANCODE_H,				0x23 },
	{ SDL_SCANCODE_J,				0x24 },
	{ SDL_SCANCODE_K,				0x25 },
	{ SDL_SCANCODE_L,				0x26 },
	{ SDL_SCANCODE_SEMICOLON,		0x27 },
	{ SDL_SCANCODE_APOSTROPHE,		0x28 },
	{ SDL_SCANCODE_GRAVE,			0x29 },
	{ SDL_SCANCODE_LSHIFT,			0x2a },
	{ SDL_SCANCODE_BACKSLASH,		0x2b },
	{ SDL_SCANCODE_Z,				0x2c },
	{ SDL_SCANCODE_X,				0x2d },
	{ SDL_SCANCODE_C,				0x2e },
	{ SDL_SCANCODE_V,				0x2f },
	{ SDL_SCANCODE_B,				0x30 },
	{ SDL_SCANCODE_N,				0x31 },
	{ SDL_SCANCODE_M,				0x32 },
	{ SDL_SCANCODE_COMMA,			0x33 },
	{ SDL_SCANCODE_PERIOD,			0x34 },
	{ SDL_SCANCODE_SLASH,			0x35 },
	{ SDL_SCANCODE_RSHIFT,			0x36 },
	{ SDL_SCANCODE_KP_MULTIPLY,		0x37 },
	{ SDL_SCANCODE_LALT,			0x38 },
	{ SDL_SCANCODE_SPACE,			0x39 },
	{ SDL_SCANCODE_CAPSLOCK,		0x3a },
	{ SDL_SCANCODE_F1,				0x3b },
	{ SDL_SCANCODE_F2,				0x3c },
	{ SDL_SCANCODE_F3,				0x3d },
	{ SDL_SCANCODE_F4,				0x3e },
	{ SDL_SCANCODE_F5,				0x3f },
	{ SDL_SCANCODE_F6,				0x40 },
	{ SDL_SCANCODE_F7,				0x41 },
	{ SDL_SCANCODE_F8,				0x42 },
	{ SDL_SCANCODE_F9,				0x43 },
	{ SDL_SCANCODE_F10,				0x44 },
	{ SDL_SCANCODE_NUMLOCKCLEAR,	0x45 },
	{ SDL_SCANCODE_SCROLLLOCK,		0x46 },
	{ SDL_SCANCODE_KP_7,			0x47 },
	{ SDL_SCANCODE_KP_8,			0x48 },
	{ SDL_SCANCODE_KP_9,			0x49 },
	{ SDL_SCANCODE_KP_MINUS,		0x4a },
	{ SDL_SCANCODE_KP_4,			0x4b },
	{ SDL_SCANCODE_KP_5,			0x4c },
	{ SDL_SCANCODE_KP_6,			0x4d },
	{ SDL_SCANCODE_KP_PLUS,			0x4e },
	{ SDL_SCANCODE_KP_1,			0x4f },
	{ SDL_SCANCODE_KP_2,			0x50 },
	{ SDL_SCANCODE_KP_3,			0x51 },
	{ SDL_SCANCODE_KP_0,			0x52 },
	{ SDL_SCANCODE_KP_PERIOD,		0x53 },
	{ SDL_SCANCODE_NONUSBACKSLASH,	0x56 },
	{ SDL_SCANCODE_F11,				0x57 },
	{ SDL_SCANCODE_F12,				0x58 },
	{ SDL_SCANCODE_KP_EQUALS,		0x59 },
	{ SDL_SCANCODE_INTERNATIONAL6,	0x5c },
	{ SDL_SCANCODE_F13,				0x64 },
	{ SDL_SCANCODE_F14,				0x65 },
	{ SDL_SCANCODE_F15,				0x66 },
	{ SDL_SCANCODE_F16,				0x67 },
	{ SDL_SCANCODE_F17,				0x68 },
	{ SDL_SCANCODE_F18,				0x69 },
	{ SDL_SCANCODE_F19,				0x6a },
	{ SDL_SCANCODE_F20,				0x6b },
	{ SDL_SCANCODE_F21,				0x6c },
	{ SDL_SCANCODE_F22,				0x6d },
	{ SDL_SCANCODE_F23,				0x6e },
	{ SDL_SCANCODE_INTERNATIONAL2,	0x70 },
	{ SDL_SCANCODE_LANG2,			0x71 },
	{ SDL_SCANCODE_LANG1,			0x72 },
	{ SDL_SCANCODE_INTERNATIONAL1,	0x73 },
	{ SDL_SCANCODE_F24,				0x76 },
	{ SDL_SCANCODE_LANG4,			0x77 },
	{ SDL_SCANCODE_LANG3,			0x78 },
	{ SDL_SCANCODE_INTERNATIONAL4,	0x79 },
	{ SDL_SCANCODE_INTERNATIONAL5,	0x7b },
	{ SDL_SCANCODE_INTERNATIONAL3,	0x7d },
	{ SDL_SCANCODE_KP_COMMA,		0x7e },
	{ SDL_SCANCODE_PASTE,			0xe00a },
	{ SDL_SCANCODE_CUT,				0xe017 },
	{ SDL_SCANCODE_COPY,			0xe018 },
	{ SDL_SCANCODE_KP_ENTER,		0xe01c },
	{ SDL_SCANCODE_RCTRL,			0xe01d },
	{ SDL_SCANCODE_MUTE,			0xe020 },
	{ SDL_SCANCODE_VOLUMEDOWN,		0xe02e },
	{ SDL_SCANCODE_VOLUMEUP,		0xe030 },
	{ SDL_SCANCODE_AC_HOME,			0xe032 },
	{ SDL_SCANCODE_KP_DIVIDE,		0xe035 },
	{ SDL_SCANCODE_PRINTSCREEN,		0xe037 },
	{ SDL_SCANCODE_RALT,			0xe038 },
	{ SDL_SCANCODE_HELP,			0xe03b },
	{ SDL_SCANCODE_NUMLOCKCLEAR,	0xe045 },
	{ SDL_SCANCODE_PAUSE,			0xe046 },
	{ SDL_SCANCODE_HOME,			0xe047 },
	{ SDL_SCANCODE_UP,				0xe048 },
	{ SDL_SCANCODE_PAGEUP,			0xe049 },
	{ SDL_SCANCODE_LEFT,			0xe04b },
	{ SDL_SCANCODE_RIGHT,			0xe04d },
	{ SDL_SCANCODE_END,				0xe04f },
	{ SDL_SCANCODE_DOWN,			0xe050 },
	{ SDL_SCANCODE_PAGEDOWN,		0xe051 },
	{ SDL_SCANCODE_INSERT,			0xe052 },
	{ SDL_SCANCODE_DELETE,			0xe053 },
	{ SDL_SCANCODE_LGUI,			0xe05b },
	{ SDL_SCANCODE_RGUI,			0xe05c },
	{ SDL_SCANCODE_APPLICATION,		0xe05d },
	{ SDL_SCANCODE_POWER,			0xe05e },
	{ SDL_SCANCODE_SLEEP,			0xe05f },
	{ SDL_SCANCODE_AC_SEARCH,		0xe065 },
	{ SDL_SCANCODE_AC_BOOKMARKS,	0xe066 },
	{ SDL_SCANCODE_AC_REFRESH,		0xe067 },
	{ SDL_SCANCODE_AC_STOP,			0xe068 },
	{ SDL_SCANCODE_AC_FORWARD,		0xe069 },
	{ SDL_SCANCODE_AC_BACK,			0xe06a }
};

}

bool
d3d11::init(void) {
	D3D_FEATURE_LEVEL lvl;
	HRESULT res;

	for (const auto &driver : drivers) {
		res = ::D3D11CreateDevice(nullptr, driver, nullptr, 0, levels, 4,
								  D3D11_SDK_VERSION, &device, &lvl, &ctx);
		BREAK_IF(SUCCEEDED(res));
		if (ctx) {
			ctx->Release();
			ctx = nullptr;
		}
		if (device) {
			device->Release();
			device = nullptr;
		}
	}
	if (FAILED(res)) {
		INFO(WARN"Can't create DirectX device");
		return false;
	}

	IDXGIOutput1 *out1dxgi = nullptr;
	IDXGIAdapter  *adpdxgi = nullptr;
	IDXGIDevice   *devdxg  = nullptr;
	IDXGIOutput   *outdxgi = nullptr;

	res = device->QueryInterface(__uuidof(devdxg), reinterpret_cast<void **>(&devdxg));
	DIE(FAILED(res));
	res = devdxg->GetParent(__uuidof(adpdxgi), reinterpret_cast<void **>(&adpdxgi));
	DIE(FAILED(res));
	res = adpdxgi->EnumOutputs(0, &outdxgi);
	DIE(FAILED(res));
	res = outdxgi->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void **>(&out1dxgi));
	DIE(FAILED(res));
	res = out1dxgi->DuplicateOutput(device, &outdup);
	DIE(FAILED(res));

	outdup->GetDesc(&outdesc);
	desc.SampleDesc.Quality	= 0;
	desc.SampleDesc.Count	= 1;
	desc.ArraySize			= 1;
	desc.BindFlags			= 0;
	desc.MiscFlags			= 0;
	desc.MipLevels			= 1;
	desc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ;
	desc.Usage				= D3D11_USAGE_STAGING;
	desc.Width				= outdesc.ModeDesc.Width;
	desc.Height				= outdesc.ModeDesc.Height;
	desc.Format				= outdesc.ModeDesc.Format;

	devdxg->Release();
	adpdxgi->Release();
	outdxgi->Release();
	out1dxgi->Release();

	const bool ok = outdesc.ModeDesc.Width && outdesc.ModeDesc.Height;
	switch (static_cast<int>(outdesc.ModeDesc.Format)) {
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return ok;
	default:							  INFO(WARN"Incorrect color format");
										  return false;
	}
}

void
d3d11::close(void) {
	RET_IF(!device);
	deskres->Release();
	device->Release();
	outdup->Release();
	ctx->Release();
}

xy
d3d11::res(void) {
	return std::make_pair(outdesc.ModeDesc.Width, outdesc.ModeDesc.Height);
}

void
d3d11::refresh(pixs &arg) {
	deskres = nullptr;
	arg.ptr = nullptr;

	auto res = outdup->AcquireNextFrame(500, &fi, &deskres);
	if (SUCCEEDED(res) && (fi.LastPresentTime.QuadPart == 0) || FAILED(res)) {
		if (deskres) {
			deskres->Release();
		}
		outdup->ReleaseFrame();
		return;
	}

	res = deskres->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&img0));
	if (FAILED(res)) {
		deskres->Release();
		outdup->ReleaseFrame();
		INFO(WARN"Can't select frame texture");
		return;
	}

	res = device->CreateTexture2D(&desc, nullptr, &img1);
	if (FAILED(res)) {
		deskres->Release();
		outdup->ReleaseFrame();
		img0->Release();
		INFO(WARN"Can't create new texture");
		DIE(!img1);
		return;
	}

	ctx->CopyResource(img1, img0);
	img0->Release();
	deskres->Release();
	outdup->ReleaseFrame();

	res = ctx->Map(img1, ::D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &sres);
	if (FAILED(res)) {
		INFO(WARN"Can't map screen texture");
		return;
	}

	arg.ptr = reinterpret_cast<byte *>(sres.pData);
	arg.shift = 4;
	arg.type  = RGBA;
	ctx->Unmap(img1, 0);
	img1->Release();
}

void
d3d11::set(const events &arg) {
	constexpr auto KEYUP = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	constexpr auto KEYDOWN = KEYEVENTF_SCANCODE;
	constexpr size_t size = sizeof(INPUT);
	std::map<uint32_t, uint32_t>::iterator it;
	uint32_t key;
	INPUT inp;

	inp.ki.dwExtraInfo = 0;
	inp.ki.time = 0;
	inp.ki.wVk  = 0;
	::SetCursorPos(std::min(static_cast<UINT>(arg.mouse.first),  outdesc.ModeDesc.Width  - 1),
				   std::min(static_cast<UINT>(arg.mouse.second), outdesc.ModeDesc.Height - 1));

	for (size_t i = 0; i < MAXKEYS; i++) {	
		BREAK_IF(arg.buttons[i].type == keys::types::NO_TYPE);
		key = arg.buttons[i].key;

		if (arg.buttons[i].type <= keys::types::MOUSE_DOWN) {
			inp.type = INPUT_MOUSE;
			switch (key) {
			case 1:	 key = arg.buttons[i].type == keys::types::MOUSE_DOWN ? MOUSEEVENTF_LEFTDOWN
																		  : MOUSEEVENTF_LEFTUP;
					 break;
			case 2:	 key = arg.buttons[i].type == keys::types::MOUSE_DOWN ? MOUSEEVENTF_MIDDLEDOWN
																		  : MOUSEEVENTF_MIDDLEUP;
					 break;
			case 3:	 key = arg.buttons[i].type == keys::types::MOUSE_DOWN ? MOUSEEVENTF_RIGHTDOWN
																		  : MOUSEEVENTF_RIGHTUP;
					 break;
			default: continue;
			}
			
			inp.mi.dwFlags = key;
			inp.mi.time = 0;
			inp.mi.dx = 0;
			inp.mi.dy = 0;
			::SendInput(1, &inp, size);
			continue;
		}

		it = keymap.find(key);
		if (it != keymap.end()) {
			key = it->second;
		}

		inp.type = INPUT_KEYBOARD;
		inp.ki.wScan = key;
		inp.ki.dwFlags = arg.buttons[i].type == keys::types::KEY_DOWN ? KEYDOWN : KEYUP;
		::SendInput(1, &inp, size);
	}
}

}
