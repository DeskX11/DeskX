
#include "../../include/Server.h"

bool Actions::work = true;
/**
 *	Header data transfer function. The package includes screen
 *	resolution and color table.
 */
bool Actions::SendHeaders(uint8_t compression, bool sdl) {
	constexpr size_t size = U32S * 2 + TABLE_SIZE;
	byte res[size];
	uint32_t width, height;

	Global::x11->Start(compression, sdl);

	Global::x11->GetResolution(width, height);
	memcpy(res + U32S, &height, U32S);
	memcpy(res, &width, U32S);

	res[U32S * 2] = Global::x11->PackLinks(res + U32S * 2 + 1);
	return Global::net->Send(res, size);
}
/**
 *	Synchronization of two ports. Return true if free prototypes
 *	were found.
 */
bool Actions::ProtsSync(uint16_t &port1, uint16_t &port2) {
	byte req[U16S * 2], res = 1;
	uint16_t *buff2 = reinterpret_cast<uint16_t *>(req + U16S);
	uint16_t *buff1 = reinterpret_cast<uint16_t *>(req);

	for (uint8_t atts = 0; atts < 10; atts++) {
		BREAK_IF(!Global::net->Recv(req, U16S * 2));

		port1 = Tools::FreePort(*buff1);
		port2 = Tools::FreePort(*buff2);
		res = (port1 == *buff1 && port2 == *buff2) ? 0 : 1;

		BREAK_IF(!Global::net->Send(&res, 1) || res == 0);
	}

	return res == 0;
}
/**
 *	Screen data transmission function via UDP protocol. The data
 *	is split into several packets, indicating the number of
 *	color blocks in the packet.
 */
void Actions::ScreenUDP(uint16_t port) {
	UDP net(&Actions::work, port, Global::args.ip);
	byte buff[net.Size()], *ptr;
	std::vector<pix> pixlist;
	uint64_t tmp, size;
	uint8_t  len;

	uint16_t *num = (uint16_t *)buff;

	while (Actions::work) {
		Global::x11->Vector(pixlist);

		ptr = buff + U16S;
		size = U16S;
		*num = 0;

		for (auto &p : pixlist) {
			tmp = p.get();
			len = p.size();

			if (size + len > net.Size()) {
				RET_IF_VOID(!net.SendV(buff));
				ptr  = buff + U16S;
				size = U16S;
				*num = 0;
			}

			memcpy(ptr, &tmp, len);
			ptr  += len;
			size += len;
			(*num)++;
		}

		BREAK_IF(!net.SendV(buff));
		pixlist.clear();
	}

	Actions::work = false;
}
/**
 *	Screen data transmission function via TCP protocol. All data
 *	is transmitted immediately with the packet size indicated at
 *	the beginning.
 */
void Actions::ScreenTCP(void) {
	std::vector<pix> pixlist;
	uint64_t tmp, size;
	uint8_t  len;
	byte *ptr;

	byte *buff = new byte[Global::x11->Size() + U32S];
	uint32_t *num = (uint32_t *)buff;
	assert(buff);

	while (Actions::work) {
		Global::x11->Vector(pixlist);
		ptr = buff + U32S;
		*num = 0;

		for (auto &p : pixlist) {
			tmp = p.get();
			len = p.size();

			memcpy(ptr, &tmp, len);
			ptr	   += len;
			(*num) += len;
		}

		size = *num + U32S;
		BREAK_IF(!Global::net->Send(buff, size));

		pixlist.clear();
	}

	Actions::work = false;
	delete[] buff;
}
/**
 *	Function of receiving keyboard and mouse events via UDP
 *	protocol. Runs in a separate thread.
 */
void Actions::EventsUDP(uint16_t port) {
	UDP net(&Actions::work, port);
	byte buff[net.Size()], *ptr = buff + 1 + U16S * 2;
	uint16_t x, y;

	while (Actions::work) {
		BREAK_IF(!net.RecvV(buff));

		memcpy(&y, buff + 1 + U16S, U16S);
		memcpy(&x, buff + 1,  U16S);
		Global::x11->MouseXY(x, y);

		if (*buff > 0) {
			Global::x11->NewEvents(ptr, *buff);
		}
	}

	Actions::work = false;
}
/**
 *	Function of receiving keyboard and mouse events via TCP
 *	protocol. Runs in a separate thread.
 */
void Actions::EventsTCP(uint16_t port) {
	byte buff[100], *ptr = buff + U16S * 2;
	uint16_t x, y, len;
	uint8_t  esize;

	while (Actions::work) {
		BREAK_IF(!Global::net->Recv(buff, 1));
		len = U16S * 2 + (esize = *buff) * KEY_BLOCK;

		BREAK_IF(!Global::net->Recv(buff, len));

		memcpy(&y, buff + U16S, U16S);
		memcpy(&x, buff,  U16S);
		Global::x11->MouseXY(x, y);

		if (esize > 0) {
			Global::x11->NewEvents(ptr, esize);
		}
	}

	Actions::work = false;
}
/**
 *	Remote control start.
 */
void Actions::StartStream(uint8_t compression, bool screen, bool events,
						  bool sdl) {
	uint16_t port1, port2;

	Actions::work = true;

	RET_IF_VOID(!Actions::SendHeaders(compression, sdl));
	RET_IF_VOID(!Actions::ProtsSync(port1, port2));

	if (!screen) {
		Global::net->BufferSize(Global::x11->Size());
	}

	std::thread thr((events) ? Actions::EventsUDP
							 : Actions::EventsTCP, port2);
	(screen) ? Actions::ScreenUDP(port1) : Actions::ScreenTCP();
	if (thr.joinable()) {
		thr.join();
	}
}
