
#ifndef _DESKX_SERVER_H_
#define _DESKX_SERVER_H_

#include "DeskX.h"
#include "server/Actions.h"
#include "server/X11.h"

inline std::string readme(Consts::logo + "Usage: ./dxs [options]\n"
"Options:\n"
"	--bind-ip		IP address to listen on (default: all available IPs)\n"
"	--port			Connection port\n"
"	--display		Screen number (default: :0)\n"
"	--xauth			Path to .Xauthority file\n\n"
"Example:\n"
"	./dxs --bind-ip=127.0.0.1 --port=4431 --display=:0 --xauth=.Xauthority\n");

namespace Server {
	inline bool work = true;
	inline Args args;
	inline Net tcp;
	inline X11 *x11 = nullptr;
}

#endif