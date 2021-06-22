
#include "../../include/DeskX.h"

UDP::UDP(bool *flag, int port, std::string ip) {
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	ERROR(sock < 0, "Can't create new UDP socket.");

	int re = 1;

	Tools::Sddr(own, port, ip);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re));

	toptr = reinterpret_cast<char *>(&tout);
	RecvTimeout(0, true);

	sender = !ip.empty();
	w = flag;
	
	if (!sender) {
		bool status = bind(sock, own.ptr, SDDR_SIZE) == 0;
		ERROR(!status, "Current UDP port is busy.");
	}

	packlim = UDP_MTU - sizeof(iphdr) - sizeof(udphdr);
	userlim = packlim - 1;

	pack = new byte[packlim];
	back = new byte[packlim];

	assert(pack && back);
}

void UDP::RecvTimeout(long msec, bool slower) {
	tout.tv_usec = (tout.tv_usec + msec) / 2;
	if (slower) {
		tout.tv_sec += (tout.tv_sec > 3) ? 0 : 1;
	}

	RET_IF_VOID(pid != 0 && !slower);
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, toptr, TV_SIZE);
}

void UDP::ClearBuffer(void) {
	socklen_t sz = SDDR_SIZE;
	while (recvfrom(sock, pack, packlim, MSG_DONTWAIT, out.ptr, &sz) != -1);
}

int UDP::Recv(byte *buff, int size) {
	socklen_t sz = SDDR_SIZE;

	assert(buff && size <= packlim);
	return recvfrom(sock, buff, size, 0, out.ptr, &sz);
}

int UDP::Send(byte *buff, int size) {
	sockaddr *ptr = (sender) ? own.ptr : out.ptr;

	assert(buff && size <= packlim);
	return sendto(sock, buff, size, 0, ptr, SDDR_SIZE);
}
/**
 *	Sending data with a guaranteed receipt. The receiver is obliged
 *	to send back a byte sum to acknowledge receipt.
 */
int UDP::SendV(byte *buff) {
	bool flag = false;
	timeval start, end;

	gettimeofday(&start, nullptr);

	memcpy(pack + 1, buff, packlim);
	pack[0] = pid++;

	uint32_t bs = std::accumulate(pack, pack + packlim, 0);
	uint32_t *out = reinterpret_cast<uint32_t *>(back);
	*out = 0;

	for (uint8_t atts = 0; atts < UDP_ATTEMPTS && *w; atts++) {
		if (atts > 3) {
			RecvTimeout(0, true);
		}
		NEXT_IF(Send(pack, packlim) != packlim);
		NEXT_IF(Recv(back, packlim) != packlim);
		BREAK_IF((flag = *out == bs));
	}

	gettimeofday(&end, nullptr);
	RecvTimeout(abs(end.tv_usec - start.tv_usec));

	return flag ? 1 : 0;
}
/**
 *	Receiving data with sending a response. After receiving data, the
 *	byte sum of the packet is sent in response.
 */
int UDP::RecvV(byte *buff) {
	uint32_t *bs = reinterpret_cast<uint32_t *>(back);
	uint32_t atts = 0;
	timeval start, end;

	gettimeofday(&start, nullptr);

	ClearBuffer();

	for (; atts < UDP_ATTEMPTS && *w; atts++) {
		if (atts > 3) {
			RecvTimeout(0, true);
		}
		NEXT_IF(Recv(pack, packlim) != packlim);
		*bs = std::accumulate(pack, pack + packlim, 0);

		NEXT_IF(Send(back, packlim) != packlim);
		BREAK_IF(pack[0] == pid);
		atts = 0;
	}

	memcpy(buff, pack + 1, userlim);
	pid++;

	gettimeofday(&end, nullptr);
	RecvTimeout(abs(end.tv_usec - start.tv_usec));

	return atts < UDP_ATTEMPTS ? 1 : 0;
}

UDP::~UDP(void) {
	Tools::RmSocket(sock);
	delete[] pack;
	delete[] back;
}

