
#include <SDL2/SDL.h>
#include <client.hpp>

namespace client {
namespace gui {
namespace {

SDL_Window *win = nullptr;
SDL_Surface *surface = nullptr;
int xp = 0, yp = 0;
size_t xmax, ymax;

}

bool
init(void) {
	RET_IF(::SDL_Init(SDL_INIT_VIDEO) < 0, false);

	SDL_DisplayMode res;
	::SDL_GetCurrentDisplayMode(0, &res);
	xmax = res.w;
	ymax = res.h;
	return xmax && ymax;
}

byte *
window(const size_t &width, const size_t &height) {
	RET_IF(!xmax || !ymax, nullptr);
	INFO(NOTE"New window with resolution " + std::to_string(width) + "x"
										   + std::to_string(height));
	auto mode = xmax == width && ymax == height ? SDL_WINDOW_FULLSCREEN_DESKTOP
												: SDL_WINDOW_SHOWN;
	win = ::SDL_CreateWindow("DeskX", SDL_WINDOWPOS_CENTERED,
									  SDL_WINDOWPOS_CENTERED,
									  width, height, mode);
	RET_IF(!win, nullptr);
	surface = ::SDL_GetWindowSurface(win);
	return !surface ? nullptr : reinterpret_cast<byte *>(surface->pixels);
}

const size_t &
width(void) {
	return xmax;
}

const size_t &
height(void) {
	return ymax;
}

void
close(void) {
	if (xmax && ymax) {
		::SDL_FreeSurface(surface);
		::SDL_DestroyWindow(win);
	}
	::SDL_Quit();
	win = nullptr;
}

bool
refresh(void) {
	return ::SDL_UpdateWindowSurface(win) == 0;
}

std::pair<bool, bool>
events(display::events &e) {
	bool emouse = false, ekeys = false, quit = false;
	SDL_Event event;
	int x, y, num = 0;

	e.clear();
	::SDL_PumpEvents();
	::SDL_GetMouseState(&x, &y);

	e.mouse = std::make_pair(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
	emouse = x != xp || y != yp;
	xp = x;
	yp = y;

	while (::SDL_PollEvent(&event) && num < 5) {
		bool flag = false;
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN: flag = true;
		case SDL_MOUSEBUTTONUP:   e.buttons[num].key  = event.button.button;
								  e.buttons[num].type = flag ? display::keys::MOUSE_DOWN
								  					  		 : display::keys::MOUSE_UP;
								  break;
		case SDL_KEYDOWN:		  flag = true;
		case SDL_KEYUP:			  e.buttons[num].key  = event.key.keysym.scancode;
								  e.buttons[num].type = flag ? display::keys::KEY_DOWN
								  					  		 : display::keys::KEY_UP;
								  break;
		case SDL_QUIT:			  quit = true;
		default:				  continue;
		}

		ekeys = true;
		num++;
	}

	return std::make_pair(emouse || ekeys, quit);
}

}
}