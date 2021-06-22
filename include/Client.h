
#ifndef _DESKX_CLIENT_
#define _DESKX_CLIENT_

#include "DeskX.h"
#include "Client/X11.h"
#include "Client/Actions.h"

inline std::string man_text("\033[1mDeskX\033[0m - Program for remote control "
"of a computer in a local network. Client side.\nGitHub: https://github.com/DeskX11/DeskX\n\n"
"Usage: ./dxс [options]\n"
"Options:\n"
"	--ip			Ip address of the server\n"
"	--port			Port of the server\n"
"	--password		Verification secret word without spaces\n"
"	--compression		Compression range (0 to 255)\n"
"	--events		Protocol for events, TCP or UDP (default: TCP)\n"
"	--screen		Protocol for screen, TCP or UDP (default: TCP)\n"
"	--disable-vert		Disable vertical compression.\n"
"	--cmd			Server side command (default: rat)\n\n"
"Commands:\n"
"	exit			Command to shutdown the server side\n"
"	rat 			Start remote control\n\n"
"Example:\n"
"	./dxс --ip=192.168.0.1 --port=4431 --password=secret --compression=16\n");

namespace Global {
	inline TCP		*net;
	inline X11		*x11;
	inline input	args;
}

#endif
