
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

bool UDP::BufferCheck(byte &id) {
	return recvfrom(sock, &id, 1, MSG_PEEK, nullptr, 0) == 1;
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
		RET_IF(Recv(back, U32S) < 0, -1);
		RET_IF(out == in, 1);
	}

	return 0;
}
/**
 *	Receiving data with sending a response. After receiving data, the
 *	byte sum of the packet is sent in response, followed by waiting
 *	for new data.
 */
int UDP::RecvV(byte *buff) {
	byte pack[packlim];
	bool flag = false;
	uint8_t atts, id;
	uint32_t out;
	int length;

	byte *back = reinterpret_cast<byte *>(&out);

	for (atts = 0; atts < UDP_ATTEMPTS; atts++) {
		length = Recv(pack, packlim);
		RET_IF(length < 0, -1);
		BREAK_IF((flag = length == packlim));
	}

	RET_IF(!flag, -1);
	out = std::accumulate(pack + 1, pack + packlim, 0);

	for (atts = 0; atts < UDP_ATTEMPTS; atts++) {
		RET_IF(Send(back, U32S) != U32S, -2);
		BREAK_IF(!BufferCheck(id));

		BREAK_IF((flag = id != pack[0]));
	}

	if (flag) {
		memcpy(buff, pack + 1, userlim);
		return 1;
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