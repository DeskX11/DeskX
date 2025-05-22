
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <macro.hpp>
#include <net.hpp>
#include <display.hpp>
#include <codec.hpp>
#include <server.hpp>

namespace server {
namespace {

display::tpl *disp = nullptr;
bool alive;

byte
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
		BREAK_IF(ret == net::status::FAIL);
		NEXT_DELAY(ret == net::status::EMPTY);
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

	disp = display::get(session_type());
	if (!disp) {
		INFO(ERR"Unsupported screen session type");
		net::close();
		return 5;
	}
	if (!disp->init()) {
		INFO(ERR"Can't init screen session");
		disp->close();
		delete disp;
		net::close();
		return 6;
	}

	for (net::hello usr;;) {
		net::kick();
		NEXT_DELAY(!net::connection() || !net::recv(usr));

		usr.delta = std::min(byte{0xFE}, usr.delta);
		usr.fps   = std::max(byte{0x01}, usr.fps);
		net::screen res;
		std::tie(res.width, res.height) = disp->res();
		NEXT_IF(!net::send(res));

		net::skipxy skip;
		NEXT_IF(!net::recv(skip));

		alive = true;
		std::thread keys(events);
		DIE(!keys.joinable());

		codec::init(res.width, res.height, usr.delta);
		codec::skip(skip.x, skip.y);
		codec::alloc();
		byte *buff = new byte[codec::max() + 8];
		byte *msg = buff + 8;
		DIE(!buff);
		
		auto &size = *reinterpret_cast<uint64_t *>(buff);
		std::chrono::milliseconds delay(1000 / usr.fps);
		auto now = NOW_MSEC, prev = now;
		display::pixs pixs;
		net::status status;

		while (alive) {
			now = NOW_MSEC;
			if (now - prev < delay) {
				prev = delay - (now - prev);
				std::this_thread::sleep_for(prev);
			}

			prev = now;
			disp->refresh(pixs);
			NEXT_IF(!codec::get(pixs, msg, size));

			status = net::send(buff, size + 8);
			BREAK_IF(status != net::status::OK);
		}

		alive = false;
		keys.join();
		codec::free();
		delete[] buff;
	}

	net::close();
	disp->close();
	delete disp;
	return 7;
}

}