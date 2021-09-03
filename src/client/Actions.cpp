
#include "../../include/Client.h"

bool Actions::work = true;
/**
 *	Authorization function. Password hash and compression level
 *	are sent
 */
void Actions::Authorization(byte *req, bool screen, bool events,
							uint8_t mod) {
	byte res, pscr = screen ? 1 : 0, pevt = events ? 1 : 0;
	bool status;

	req[0] = mod;
	req[1] = Global::args.comp;
	req[2] = pscr;
	req[3] = pevt;
	req[4] = Global::args.dvert ? 0 : 1;
	req[5] = 0;
#ifdef __APPLE__
	req[5] = 1;
#endif

	status = Global::net->Send(req, AUTH_SIZE);
	ERROR(!status, "Unable to send authorization package.");
}
/**
 *	Header data receiving function. The package includes screen
 *	resolution and color table.
 */
void Actions::GetHeaders(void) {
	byte headers[TABLE_SIZE];
	uint32_t width, height;

	bool s1 = Global::net->Recv((byte *)&width,  U32S);
	bool s2 = Global::net->Recv((byte *)&height, U32S);
	ERROR(!s1 || !s2, "Unable to get server's resolution.");

#ifdef __APPLE__
	assert(Global::scr = new SDL(width, height));
#else
	assert(Global::scr = new X11(width, height));
#endif

	s1 = Global::net->Recv(headers, TABLE_SIZE);
	ERROR(!s1, "Unable to get color reference table.");

	Global::scr->AddLinks(*headers, headers + 1);
}
/**
 *	Synchronization of two ports.
 */
void Actions::ProtsSync(uint16_t &port1, uint16_t &port2) {
	port1 = Global::args.port + 1;
	byte ports[U16S * 2], res = 1;

	for (uint8_t atts = 0; atts < 10; atts++) {
		port1 = Tools::FreePort(port1);
		port2 = Tools::FreePort((port2 = port1 + 1));

		memcpy(ports + U16S, &port2, U16S);
		memcpy(ports, &port1, U16S);

		BREAK_IF(!Global::net->Send(ports, U16S * 2));
		BREAK_IF(!Global::net->Recv(&res, 1) || res == 0);
		port1 += 2;
	}

	ERROR(res != 0, "Unable to synchronize ports.");
}
/**
 *	Screen data transmission function via TCP protocol.
 */
void Actions::ScreenTCP(uint16_t port) {
	uint32_t size;

	byte *pack = new byte[Global::scr->Size()];
	byte *sptr = (byte *)&size;
	assert(pack);

	for (;;) {
		BREAK_IF(!Global::net->Recv(sptr, U32S));
		BREAK_IF(size < COLOR_BLOCK);

		BREAK_IF(!Global::net->Recv(pack, size));
		Global::scr->Set(pack, size);
	}

	delete[] pack;
	ERROR(true, "Server stopped sending frames.");
}
/**
 *	Screen data transmission function via UDP protocol.
 */
void Actions::ScreenUDP(uint16_t port) {
	UDP net(&Actions::work, port);
	byte pack[net.Size()];

	for (;;) {
		BREAK_IF(!net.RecvV(pack));
		Global::scr->Set(pack);
	}

	ERROR(true, "Server stopped sending frames.");
}
/**
 *	Function of receiving keyboard and mouse events via UDP
 *	protocol. Runs in a separate thread.
 */
void Actions::EventsUDP(uint16_t port) {
	UDP net(&Actions::work, port, Global::args.ip);
	byte pack[net.Size()], *buff = pack + 1;

	for (;;) {
		*pack = Global::scr->GetEvents(buff);
		BREAK_IF(!net.SendV(pack));
	}

	ERROR(true, "The server stopped responding to events.")
}
/**
 *	Function of receiving keyboard and mouse events via TCP
 *	protocol. Runs in a separate thread.
 */
void Actions::EventsTCP(uint16_t port) {
	constexpr size_t size = U16S * 2 + 1;
	byte pack[100], *buff = pack + 1;
	size_t len;

	for (;;) {
		*pack = Global::scr->GetEvents(buff);
		len = *pack * KEY_BLOCK + size;

		BREAK_IF(!Global::net->Send(pack, len));
	}

	ERROR(true, "The server stopped responding to events.")
}
/**
 *	Remote control start.
 */
void Actions::StartStreaming(byte *request) {
	bool events = Global::args.events == "UDP";
	bool screen = Global::args.screen == "UDP";
	uint16_t port1, port2;

	Actions::Authorization(request, screen, events, 0);
	Actions::GetHeaders();
	Actions::ProtsSync(port1, port2);

	Global::scr->ScreenProtocol(screen);
	if (!screen) {
		Global::net->BufferSize(Global::scr->Size());
	}
/*
	std::thread thr((events) ? Actions::EventsUDP
							 : Actions::EventsTCP, port2);
	(screen) ? Actions::ScreenUDP(port1) : Actions::ScreenTCP();*/
	std::thread thr((screen) ? Actions::ScreenUDP
							 : Actions::ScreenTCP, port1);
	(events) ? Actions::EventsUDP(port2) : Actions::EventsTCP(port2);

	if (thr.joinable()) {
		thr.join();
	}

	delete Global::scr;
}
