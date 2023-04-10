
#include "../../include/Client.h"

uint8_t Actions::sync(void) {
	if (Client::args.get().ip.empty() || Client::args.get().port < 1) {
		return 1;
	}

	bool end = Client::args.get().mode.func == END;

	Client::tcp = Net(Client::args.get().port, Client::args.get().ip, false);
	RET_IF(!Client::tcp.connect(), 2);

	byte *config = Client::args.pack();
	RET_IF(!Client::tcp.send(config, Consts::req), 3);
	RET_IF(!Client::tcp.recv(config, Consts::res), end ? 6 : 4);
	RET_IF(*config, 5);

	memcpy(&Client::args.get().mode.width,  config + 1,  Consts::u16);
	memcpy(&Client::args.get().mode.height, config + 1 + Consts::u16,
														 Consts::u16);
	delete[] config;
	return 0;
}

void Actions::start(void) {
#if defined(__APPLE__) || defined(__CYGWIN__)
	assert(Client::scr = new SDL);
#else
	assert(Client::scr = new X11);
#endif
	Client::tcp.buffer(Client::scr->bufferTCP() + Consts::u64);
	std::thread screen(Actions::screen);
	Actions::events();

	if (screen.joinable()) {
		screen.join();
	}

	delete Client::scr;
}

void Actions::screen(void) {
	byte *pack = new byte[Client::scr->bufferTCP()];
	uint64_t number;

	byte *ptr = reinterpret_cast<byte *>(&number);
	assert(pack);

	while (Client::tcp.alive()) {
		NEXT_IF(!Client::tcp.recv(ptr, Consts::u64));
		if (number > 0) {
			BREAK_IF(!Client::tcp.recv(pack, number));
			Client::scr->screen(number, pack);
		}
	}

	delete[] pack;
}

void Actions::events(void) {
	byte *pack = new byte[Consts::emax];
	Events events;

	assert(pack);

	while (Client::tcp.alive()) {
		events = Client::scr->events();
		NEXT_IF (!events.encode(pack));
		BREAK_IF(!Client::tcp.send(pack, events.need() + 1));
	}

	delete[] pack;
}