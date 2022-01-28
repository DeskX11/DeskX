
#include "../../include/Client.h"

SDL::SDL(void) {
	//ERR(SDL_Init(SDL_INIT_VIDEO) < 0, "Can't init SDL");
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
	    std::cout << "Error : " << SDL_GetError() << std::endl;
	    exit(1);
	}

	width  = Client::args.get().mode.width;
	height = Client::args.get().mode.height;
	SDL_DisplayMode res;
	SDL_GetCurrentDisplayMode(0, &res);

	std::stringstream ss;
	ss << "Server screen resolution is greater than client resolution";
	ss << " (" << width << "x" << height << ")";
	ERR(res.w < width || res.h < height, ss.str());

	auto mode = res.w == width && res.h == height ? SDL_WINDOW_FULLSCREEN_DESKTOP
												  : SDL_WINDOW_SHOWN;
	win = SDL_CreateWindow("DeskX - Remote control", SDL_WINDOWPOS_CENTERED,
													 SDL_WINDOWPOS_CENTERED,
													 width, height, mode);
	assert(win && (surface = SDL_GetWindowSurface(win)));
	tcpbuff = width * height * 4;
}

void SDL::screen(uint64_t size, byte *blocks) {
	byte *orig = (byte *)surface->pixels;
	Codec::Block pix;
	Codec::Axis axis;
	Codec::Type type;
	Codec::RGB  prev;
	uint16_t x = 0, y = 0;
	uint8_t step;

	while (size) {
		type = Codec::is(blocks);
		step = (type == Codec::Type::AXIS) ? axis.decode(blocks)
										   :  pix.decode(blocks, prev);
		blocks += step;
		size   -= step;

		if (type == Codec::Type::BLOCK) {
			pix.write(orig, width, x, y);
			prev = pix.rgb24();
		}
		else {
			uint16_t &tmp = axis.type() == Codec::AxisType::X ? x : y;
			tmp += axis.value();
		}
	}

	ERR(SDL_UpdateWindowSurface(win) != 0, "SDL can't update surface");
}

Events SDL::events(void) {
	SDL_Event event;
	Events list;
	int x, y;
	bool stop = false;

	SDL_PumpEvents();
	SDL_GetMouseState(&x, &y);

	list.getX() = std::max(x, 0);
	list.getY() = std::max(y, 0);

	while (SDL_PollEvent(&event)) {
		bool flag = false;

		switch(event.type) {
		case SDL_MOUSEBUTTONDOWN:
			flag = true;
		case SDL_MOUSEBUTTONUP:
			list.add(event.button.button, flag ? MOUSE_DOWN
											   : MOUSE_UP);
			break;

		case SDL_KEYDOWN:
			flag = true;
		case SDL_KEYUP:
			list.add(event.key.keysym.scancode, flag ? KEY_DOWN
													 : KEY_UP);
			stop = event.key.keysym.scancode == _EXIT_KEY;
			break;

		case SDL_QUIT:
			exit(1);
		default:
			break;
		}

		if (stop) {
			exit(1);
		}
	}

	return list;
}

size_t SDL::bufferTCP(void) {
	return tcpbuff;
}

SDL::~SDL(void) {
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(win);
	SDL_Quit();
}