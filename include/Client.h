
#ifndef _DESKX_CLIENT_H_
#define _DESKX_CLIENT_H_

#include "DeskX.h"
#include "client/Actions.h"
#if defined(__APPLE__) || defined(_WIN32)
#include "client/SDL.h"
#define _EXIT_KEY SDL_SCANCODE_F7
#else
#include "client/X11.h"
#define _EXIT_KEY (uint8_t)73
#endif

inline std::string readme(Consts::logo + "Usage: ./dxс [options]\n"
"Options:\n"
"	--ip			Ip address of the server\n"
"	--port			Port of the server\n"
"	--color-distance	Compression range (0 to 255) (default: 2)\n"
"	--cmd			Server side command (default: 0)\n\n"
"Commands:\n"
"	rat (0) 		Start remote control\n"
"	exit(1)			Command to shutdown the server side\n\n"
"Example:\n"
"	./dxс --ip=192.168.0.1 --port=4431 --color-distance=16 --cmd=0\n");

namespace Client {
	inline Args args;
	inline Net tcp;
#if defined(__APPLE__) || defined(_WIN32)
	inline SDL *scr = nullptr;
#else
	inline X11 *scr = nullptr;
#endif
}

#endif