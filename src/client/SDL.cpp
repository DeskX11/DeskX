
#include "../../include/Client.h"

SDL::SDL(uint32_t in_width, uint32_t in_height) {
	ERROR(SDL_Init(SDL_INIT_VIDEO) < 0, "Can't init SDL");

	SDL_DisplayMode res;
	SDL_GetCurrentDisplayMode(0, &res);
	ERROR(res.w < in_width || res.h < in_height,
		  "Server screen resolution is greater than client resolution.");
	width = in_width;
	height = in_height;

	auto arg = res.w == width && res.h == height ? SDL_WINDOW_FULLSCREEN_DESKTOP
												 : SDL_WINDOW_SHOWN;
	win = SDL_CreateWindow("DeskX - Remote control", SDL_WINDOWPOS_CENTERED,
													 SDL_WINDOWPOS_CENTERED,
													 width, height, arg);
	assert(win && (surface = SDL_GetWindowSurface(win)));
	max = width * height * 4;
}

void SDL::AddLinks(uint8_t size, byte *buff) {
	uint32_t num;
	byte *ptr = reinterpret_cast<byte *>(&num);

	linksnum = size;

	for (size_t i = 0; i < size; i++) {
		memcpy(ptr, buff + i * U32S, U32S);
		links.push_back(num);
	}
}

void SDL::ScreenProtocol(bool udp = false) {
	size1 = (udp) ? 1 : LINKED_BLOCK;
	size2 = (udp) ? 1 : EQUAL_BLOCK;
	size3 = (udp) ? 1 : COLOR_BLOCK;
	size4 = (udp) ? 1 : INSIDE_BLOCK;
	size5 = (udp) ? 1 : VERT_BLOCK;
}

void SDL::Set(byte *ptr, uint32_t input) {
	uint32_t skip, len;
	uint8_t number;
	byte *pixs, *orig = (byte *)surface->pixels;

	if (input == 0) {
		len  = *((uint16_t *)ptr);
		ptr += U16S;
	}
	else {
		scrshift = 0;
		len = input;
	}

	while (len != 0) {
		/**
		 *	Linked blocks
		 */
		if ((number = *ptr) == 0) {
			number = ptr[1];
			RET_IF_VOID(ptr[2] > linksnum);
			pixs = ((byte *)&links[ptr[2]]) + 1;
			ptr += LINKED_BLOCK;
			len -= size1;
		}
		/**
		 *	Equal blocks
		 */
		else if (number == 0xFF && ptr[1] == 0x00) {
			memcpy(&skip, ptr + 2, U32S);
			scrshift += skip * 4;
			ptr += EQUAL_BLOCK;
			len -= size2;
			number = 0;
		}
		/**
		 *	Link to previous color
		 */
		else if (number == 0xFF) {
			number = ptr[1];
			ptr += INSIDE_BLOCK;
			len -= size4;
			pixs = orig + scrshift - 4;
		}
		/**
		 *	Color reference above horizontally
		 */
		else if (!Global::args.dvert && number == 0xFE) {
			number = ptr[1];
			ptr += VERT_BLOCK;
			len -= size5;
			pixs = orig + scrshift - width * 4;
		}
		/**
		 *	Compressed blocks
		 */
		else {
			pixs = ptr + 1;
			ptr += COLOR_BLOCK;
			len -= size3;
		}

		BREAK_IF(scrshift >= max);

		for (uint8_t i = 0; i < number; i++) {
			orig[scrshift]	   = pixs[0];
			orig[scrshift + 1] = pixs[1];
			orig[scrshift + 2] = pixs[2];
			scrshift += 4;
		}
	}

	if (scrshift >= max || input != 0) {
		ERROR(SDL_UpdateWindowSurface(win) != 0, "SDL can't update surface");
		scrshift = 0;
	}
}

uint8_t SDL::GetEvents(byte *buff) {
	uint16_t *y = reinterpret_cast<uint16_t *>(buff + U16S);
	uint16_t *x = reinterpret_cast<uint16_t *>(buff);
	SDL_Event event;
	int ix, iy;

	SDL_PumpEvents();
	SDL_GetMouseState(&ix, &iy);
	*x = (ix < 0) ? 0 : ix;
	*y = (iy < 0) ? 0 : iy;
	buff += U16S * 2;

	uint8_t  type, size = 0;
	uint32_t value;

	while (SDL_PollEvent(&event)) {
		type = 0;

		switch(event.type) {
		/**
		 *	Mouse events
		 */
		case SDL_MOUSEBUTTONDOWN:
			type = 1;
		case SDL_MOUSEBUTTONUP:
			value = event.button.button;
			type = (type != 0) ? type : 0;
			break;
		/**
		 *	Keyboard events
		 */
		case SDL_KEYDOWN:
			type = 3;
		case SDL_KEYUP:
			NEXT_IF(!X11KeySym(event.key.keysym.scancode, value));
			type = (type != 0) ? type : 2;
			break;

		case SDL_QUIT:
			exit(1);

		default: continue;
		}

		ix = size * KEY_BLOCK;
		memcpy(buff + 1 + ix, &value, U32S);
		buff[ix] = type;
		size++;

		BREAK_IF(size > 10);
	}

	return size;
}

bool SDL::X11KeySym(uint32_t keycode, uint32_t &ret) {
	std::map<uint32_t, uint32_t>::iterator it;

	if ((it = x11keys.find(keycode)) != x11keys.end()) {
		ret = it->second;
		return true;
	}

	return false;
}

SDL::~SDL(void) {
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(win);
	SDL_Quit();
}