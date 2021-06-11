
#include "../../include/Client.h"
/**
 *	Authorization function. Password hash and compression level
 *	are sent
 */
void Actions::Authorization(byte *req, bool screen, bool events,
							uint8_t mod) {
	byte res, pscr = screen ? 1 : 0, pevt = events ? 1 : 0;
	bool status;

	req[MD5_DIGEST_LENGTH + 1] = Global::args.comp;
	req[MD5_DIGEST_LENGTH + 2] = pscr;
	req[MD5_DIGEST_LENGTH + 3] = pevt;
	req[0] = mod;

	status = Global::net->Send(req, AUTH_SIZE);
	ERROR(!status, "Unable to send authorization package.");

	status = Global::net->Recv(&res, 1);
	ERROR(!status, "Unable to get server response.");
	ERROR(res != 0, "Incorrect password or generated packet.");
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

	Global::x11 = new X11(width, height);
	assert(Global::x11);

	s1 = Global::net->Recv(headers, TABLE_SIZE);
	ERROR(!s1, "Unable to get color reference table.");

	Global::x11->AddLinks(*headers, headers + 1);
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
void Actions::ScreenTCP(void) {
	uint32_t size;

	byte *pack = new byte[Global::x11->Size()];
	byte *sptr = (byte *)&size;
	assert(pack);

	for (;;) {
		BREAK_IF(!Global::net->Recv(sptr, U32S));
		BREAK_IF(size < COLOR_BLOCK);

		BREAK_IF(!Global::net->Recv(pack, size));
		Global::x11->Set(pack, size);
	}

	delete[] pack;
	ERROR(true, "Server stopped sending frames.");
}
/**
 *	Screen data transmission function via UDP protocol.
 */
void Actions::ScreenUDP(uint16_t port) {
	UDP net(port);
	byte pack[net.Size()];

	for (;;) {
		BREAK_IF(!net.RecvV(pack));
		Global::x11->Set(pack);
	}

	ERROR(true, "Server stopped sending frames.");
}
/**
 *	Function of receiving keyboard and mouse events via UDP
 *	protocol. Runs in a separate thread.
 */
void Actions::EventsUDP(uint16_t port) {
	UDP net(port, Global::args.ip);
	byte pack[net.Size()], *buff = pack + 1;

	for (;;) {
		*pack = Global::x11->GetEvents(buff);
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
		*pack = Global::x11->GetEvents(buff);
		len = *pack * 2 + size;

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

	Global::x11->ScreenProtocol(screen);
	if (!screen) {
		Global::net->BufferSize(Global::x11->Size());
	}

	std::thread thr((events) ? Actions::EventsUDP
							 : Actions::EventsTCP, port2);
	(screen) ? Actions::ScreenUDP(port1) : Actions::ScreenTCP();
	if (thr.joinable()) {
		thr.join();
	}

	delete Global::x11;
}

void Actions::ScreenShot(byte *request) {
	/* Later */
}
