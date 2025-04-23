
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <macro.hpp>
#include <net.hpp>
#include <display.hpp>
#include <codec.hpp>
#include <server.hpp>



#include <netinet/in.h>
namespace server {
namespace {

display::tpl *disp = nullptr;
bool alive;

int
session_type(void) {
#if OS != LINUX
	return 0;
#else
	const char *env = ::getenv("XDG_SESSION_TYPE");
	RET_IF(::strcmp("wayland", env) == 0, WAYLAND);
	return ::strcmp("x11", env) == 0 ? X11 : TTY;
#endif
}

void
events(void) {
	byte *buff = new byte[display::emsg];
	display::events elist;
	net::status ret;
	DIE(!buff);
	
	while (alive) {
		ret = net::recv(buff, display::emsg);
		BREAK_IF(ret == net::status::ERROR);

		if (ret == net::status::EMPTY) {
			YIELD_NEXT;
		}
		elist.set(buff);
		disp->set(elist);
	}

	alive = false;
	delete[] buff;
}

}

int
start(const args &args) {
	const int port = args.num("port");
	if (port < 1) {
		INFO(ERR"Incorrect port number");
		return 3;
	}

	if (!net::start(args["bind-ip"], port, args.mode())) {
		INFO(ERR"Can't start TCP server");
		net::close();
		return 4;
	}

	int code = 0;
	for (net::hello usr;;) {
		net::kick();
		if (!net::connection() || !net::recv(usr)) {
			YIELD_NEXT;
		}

		usr.delta = std::min(byte{0xFE}, usr.delta);
		disp = display::get(session_type());
		if (!disp) {
			INFO(ERR"Unsupported screen session type");
			code = 5;
			break;
		}
		if (!disp->init()) {
			INFO(WARN"Can't init screen session");
			disp->close();
			delete disp;
			continue;
		}

		net::screen res;
		std::tie(res.width, res.height) = disp->res();
		if (!net::send(res)) {
			disp->close();
			delete disp;
			continue;
		}

		alive = true;
		std::thread keys(events);
		DIE(!keys.joinable());

		codec::init(res.width, res.height, usr.delta);
		codec::alloc();
		byte *buff = new byte[codec::max() + 8];
		DIE(!buff);
		
		auto &size = *reinterpret_cast<uint64_t *>(buff);
		byte *msg = buff + 8;
		display::pixs pixs;
		net::status status;

		while (alive) {
			disp->refresh(pixs);
			if (!codec::get(pixs, msg, size)) {
				YIELD_NEXT;
			}
			status = net::send(buff, size + 8);
			BREAK_IF(status != net::status::OK);
		}

		alive = false;
		keys.join();
		codec::free();
		disp->close();
		delete disp;
		delete[] buff;
	}

	net::close();
	if (disp) {
		disp->close();
		delete disp;
	}

	return code;
}

}