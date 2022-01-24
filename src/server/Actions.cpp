
#include "../../include/Server.h"

void Actions::start(void) {
	byte req[Consts::req], res[Consts::res];

	if (!Server::tcp.recv(req, Consts::req)) {
		return;
	}
	*res = Server::args.unpack(req) ? 0 : 1;
	Server::args.unpack(req);

	switch (Server::args.get().mode.func) {
	case END:
		Server::work = false;
		return;

	//case SHOT:
	case RAT:
	//case XYLIST:
	default:
		break;
	}

	uint16_t width, height;
	Server::x11->resolution(width, height);
	memcpy(res + 1 + Consts::u16, &height, Consts::u16);
	memcpy(res + 1, &width, Consts::u16);

	RETVOID_IF(!Server::tcp.send(res, Consts::res));
	Server::x11->start();

	Server::tcp.buffer(Server::x11->bufferTCP() + Consts::u64);
	std::thread screen(Actions::screen);
	Actions::events();

	if (screen.joinable()) {
		screen.join();
	}
}

void Actions::screen(void) {
	byte *pack = new byte[Server::x11->bufferTCP() + Consts::u64];
	uint64_t size;

	assert(pack);

	while (true) {
		size = Server::x11->screen(pack);
		BREAK_IF(!Server::tcp.send(pack, size + Consts::u64));
	}

	delete[] pack;
}

void Actions::events(void) {
	byte *pack = new byte[Consts::emax];
	uint8_t number;

	byte *ptr = reinterpret_cast<byte *>(&number);
	assert(pack);

	while (true) {
		BREAK_IF(!Server::tcp.recv(ptr, 1));

		Events events = Events(number);

		BREAK_IF(!Server::tcp.recv(pack, events.need()));	
		events.decode(pack);

		Server::x11->events(events);
	}

	delete[] pack;
}