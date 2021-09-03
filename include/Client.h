
#ifndef _DESKX_CLIENT_
#define _DESKX_CLIENT_

#include "DeskX.h"
#include "Client/Actions.h"

#ifdef __APPLE__
#include "Client/SDL.h"
#else
#include "Client/X11.h"
#endif

inline std::string man_text("\033[1mDeskX\033[0m - Program for remote control "
"of a computer in a local network. Client side.\nGitHub: https://github.com/DeskX11/DeskX\n\n"
"Usage: ./dxс [options]\n"
"Options:\n"
"	--ip			Ip address of the server\n"
"	--port			Port of the server\n"
"	--compression		Compression range (0 to 255)\n"
"	--events		Protocol for events, TCP or UDP (default: TCP)\n"
"	--screen		Protocol for screen, TCP or UDP (default: TCP)\n"
"	--disable-vert		Disable vertical compression.\n"
"	--cmd			Server side command (default: rat)\n\n"
"Commands:\n"
"	exit			Command to shutdown the server side\n"
"	rat 			Start remote control\n\n"
"Example:\n"
"	./dxс --ip=192.168.0.1 --port=4431 --compression=16\n");

namespace Global {
	inline TCP		*net;
	inline input	args;
#ifdef __APPLE__
	inline SDL		*scr;
#else
	inline X11		*scr;
#endif
}

#endif
