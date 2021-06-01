
#include "../../include/DeskX.h"

UDP::UDP(int mtu, int port, std::string ip) {
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	ERROR(sock < 0, "Can't create new UDP socket.");

	Tools::Sddr(own, port, ip);
	Tools::SetOpts(sock);

	sender = !ip.empty();
	
	if (!sender) {
		bool status = bind(sock, own.ptr, SDDR_SIZE) == 0;
		ERROR(!status, "Current UDP port is busy.");
	}

	assert(mtu >= 1500);
	packlim = mtu - sizeof(iphdr) - sizeof(udphdr);
	userlim = packlim - 1;
	// 1 byte is reserved for packet id.
}

bool UDP::BufferCheck(byte *data, size_t size) {
	return recvfrom(sock, data, size, MSG_PEEK, nullptr, 0) == size;
}
/**
 *	Sending data with a guaranteed receipt. The receiver is obliged
 *	to send back a byte sum to acknowledge receipt.
 */
int UDP::SendV(byte *buff) {
	uint32_t in = std::accumulate(buff, buff + userlim, 0);
	byte pack[packlim];
	uint32_t out = 0;

	memcpy(pack + 1, buff, userlim);
	pack[0] = pid++;

	byte *back = reinterpret_cast<byte *>(&out);

	for (uint8_t atts = 0; atts < UDP_ATTEMPTS; atts++) {
		RET_IF(Send(pack, packlim) != packlim, -2);
		NEXT_IF(!BufferCheck(back, U32S));
		Recv(back, U32S);

		BREAK_IF(out == in);
	}

	return out == in ? 1 : 0;
}
/**
 *	Receiving data with sending a response. After receiving data, the
 *	byte sum of the packet is sent in response, followed by waiting
 *	for new data.
 */
int UDP::RecvV(byte *buff) {
	byte pack[packlim];
	uint32_t out;

	RET_IF(Recv(pack, packlim) != packlim, -1);

	out = std::accumulate(pack + 1, pack + packlim, 0);
	byte *back = reinterpret_cast<byte *>(&out);

	for (uint8_t atts = 0; atts < UDP_ATTEMPTS; atts++) {
		RET_IF(Send(back, U32S) != U32S, -2);
		NEXT_IF(!BufferCheck(&pid, 1));

		if (pid == pack[0]) {
			Recv(pack, packlim);
			continue;
		}

		if (pid != pack[0]) {
			memcpy(buff, pack + 1, userlim);
			return 1;
		}
	}

	return 0;
}

int UDP::Recv(byte *buff, int size) {
	socklen_t sz = SDDR_SIZE;

	assert(buff && size <= packlim);
	return recvfrom(sock, buff, size, MSG_WAITALL, out.ptr, &sz);
}

int UDP::Send(byte *buff, int size) {
	sockaddr *ptr = (sender) ? own.ptr : out.ptr;

	assert(buff && size <= packlim);
	return sendto(sock, buff, size, 0, ptr, SDDR_SIZE);
}

UDP::~UDP(void) {
	Tools::RmSocket(sock);
}