
#ifndef _DESKX_CLIENT_SDL_H_
#define _DESKX_CLIENT_SDL_H_

#include "../DeskX.h"

class SDL {
private:
	size_t height, width, tcpbuff;
	SDL_Window *win;
	SDL_Surface *surface;

public:
	size_t bufferTCP(void);
	void screen(uint64_t, byte *);
	Events events(void);
	SDL(void);
	~SDL(void);
};

#endif

