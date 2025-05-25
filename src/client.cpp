
#include <thread>
#include <cmath>
#include <macro.hpp>
#include <net.hpp>
#include <codec.hpp>
#include <client.hpp>

namespace client {
namespace {

bool alive, refresh = false;
byte *windowbuff;

void
screen(void) {
	byte *buff = new byte[codec::max()];
	net::status status;
	uint64_t size;
	DIE(!buff);

	while (alive) {
		status = net::recv(reinterpret_cast<byte *>(&size), 8);
		BREAK_IF(status == net::status::FAIL);
		NEXT_DELAY(status == net::status::EMPTY || size < 2);

		status = net::recv(buff, size);
		if (status != net::status::OK) {
			INFO(ERR"Invalid screen package");
			alive = false;
			break;
		}

		codec::set(windowbuff, buff, size);
		refresh = true;
		gui::refresh();
#ifdef TEST
	::exit(0);
#endif	
	}
#ifdef TEST
	::exit(1);
#endif
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

	const std::string ip = args["ip"];
	if (ip.empty()) {
		INFO(ERR"Incorrect ip address");
		return 4;
	}

	INFO(NOTE"Trying to connect to " + ip);
	if (!net::start(ip, port, args.mode())) {
		INFO(ERR"Can't connect to the server");
		net::close();
		return 5;
	}

	int num = args.num("color-distance");
	int fps = args.num("fps");
	net::hello msg = {
		static_cast<byte>(num == -1 ? 2 : std::min(254, num)),
		static_cast<byte>(fps  < 1 ? 50 : std::min(255, fps))
	};
	if (!net::send(msg)) {
		INFO(ERR"Can't send 'hello' message");
		net::close();
		return 6;
	}

	net::screen srv;
	if (!net::recv(srv)) {
		INFO(ERR"Can't receive screen config");
		net::close();
		return 7;
	}
	if (srv.width < SCR_X_MIN || srv.height < SCR_Y_MIN) {
		INFO(ERR"Screen resolution is too small");
		net::close();
		return 8;
	}
	if (!gui::init()) {
		INFO(ERR"Can't init GUI module");
		net::close();
		return 9;
	}

	double x = gui::width(), y = gui::height();
	net::skipxy skip;
	if (x < srv.width || y < srv.height) {
		INFO(WARN"Scaling enabled, distortion may occur");
		skip.x = x == srv.width  ? 1 : std::ceil(srv.width  / x);
		skip.y = y == srv.height ? 1 : std::ceil(srv.height / y);
		srv.width  = srv.width  / skip.x;
		srv.height = srv.height / skip.y;
		skip.x--;
		skip.y--;
	}
	if (!net::send(skip)) {
		INFO(ERR"Can't send 'skip' message");
		net::close();
		return 6;
	}

	skip.x++;
	skip.y++;
	windowbuff = gui::window(srv.width, srv.height);
	if (!windowbuff) {
		INFO(ERR"Can't create new window");
		net::close();
		return 10;
	}

	codec::init(srv.width, srv.height, msg.delta);
	alive = true;
	std::thread thr(screen);
	if (!thr.joinable()) {
		INFO(ERR"Can't start screen thread");
		gui::close();
		net::close();
		return 11;
	}

	byte *buff = new byte[display::emsg];
	std::pair<bool, bool> flags = { false, false};
	display::events elist;
	net::status status;
	DIE(!buff);
	
	INFO(NOTE"Ready to use");
	while (alive) {
		/*if (refresh && !gui::refresh()) {
			INFO(ERR"Can't refresh surface");
			alive = false;
			break;
		}*/

		refresh = false;
		flags = gui::events(elist);
		BREAK_IF(flags.second);
		NEXT_IF(!flags.first);

		elist.mouse.first  *= skip.x;
		elist.mouse.second *= skip.y;
		elist.pack(buff);
		status = net::send(buff, display::emsg);
		BREAK_IF(status != net::status::OK);
	}

	INFO(NOTE"Session is dropped, waiting for"
			 " work to complete");
	alive = false;
	thr.join();
	delete[] buff;
	gui::close();
	net::close();
	INFO(NOTE"Quit");
	return 0;
}

}