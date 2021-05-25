
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
}

bool UDP::BufferCheck(size_t len) {
	byte tmp[len];
	return recvfrom(sock, tmp, len, MSG_PEEK, nullptr, 0) == len;
}
/**
 *	Sending data with a guaranteed receipt. The receiver is obliged
 *	to send back a byte sum to acknowledge receipt.
 */
int UDP::SendV(byte *buff) {
	uint32_t in = std::accumulate(buff, buff + packlim, 0);
	uint32_t out = 0;

	byte *back = reinterpret_cast<byte *>(&out);

	for (uint8_t atts = 0; atts < UDP_ATTEMPTS; atts++) {
		RET_IF(Send(buff, packlim) != packlim, -2);
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
	bool flag = false;
	uint32_t out;
	uint8_t atts;
	int length;

	byte *back = reinterpret_cast<byte *>(&out);

	for (atts = 0; atts < UDP_ATTEMPTS; atts++) {
		length = Recv(buff, packlim);
		RET_IF(length < 0, -1);
		BREAK_IF((flag = length == packlim));
	}

	RET_IF(!flag, -1);
	out = std::accumulate(buff, buff + packlim, 0);

	for (atts = 0; atts < UDP_ATTEMPTS; atts++) {
		RET_IF(Send(back, U32S) != U32S, -2);
		RET_IF(BufferCheck(1), 1);
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