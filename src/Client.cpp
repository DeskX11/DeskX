
#include <thread>
#include <macro.hpp>
#include <net.hpp>
#include <codec.hpp>
#include <client.hpp>

namespace client {
namespace {

byte *windowbuff;
bool alive;

void
screen(void) {
	byte *buff = new byte[codec::max()];
	net::status status;
	uint64_t size;
	DIE(!buff);

	while (alive) {
		status = net::recv(reinterpret_cast<byte *>(&size), 8);
		BREAK_IF(status == net::status::ERROR);
		if (status == net::status::EMPTY || size < 2) {
			YIELD_NEXT;
		}

		status = net::recv(buff, size);
		if (status != net::status::OK) {
			INFO(ERR"Invalid screen package");
			alive = false;
			break;
		}

		codec::set(windowbuff, buff, size);
		if (!gui::refresh()) {
			INFO(ERR"Can't refresh surface");
			alive = false;
			break;
		}
	}

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
	net::hello msg = {
		static_cast<byte>(num == -1 ? 2 : std::min(254, num)),
		byte{0}
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
	if (srv.width < 640 || srv.height < 480) {
		INFO(ERR"Screen resolution is too small");
		net::close();
		return 8;
	}

	windowbuff = gui::init(srv.width, srv.height);
	std::thread thr;
	if (!windowbuff) {
		INFO(ERR"Can't create new window");
		net::close();
		return 9;
	}

	codec::init(srv.width, srv.height, msg.delta);
	alive = true;
	thr = std::thread(screen);
	if (!thr.joinable()) {
		INFO(ERR"Can't start screen thread");
		gui::close();
		net::close();
		return 10;
	}

	byte *buff = new byte[display::emsg];
	std::pair<bool, bool> flags = { false, false};
	display::events elist;
	net::status status;
	DIE(!buff);
	
	INFO(NOTE"Ready to use");
	while (alive) {
		flags = gui::events(elist);
		BREAK_IF(flags.second);

		if (!flags.first) {
			YIELD_NEXT;
		}
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