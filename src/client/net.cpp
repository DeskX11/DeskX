
#include "deskx.h"

netw::netw(std::string ip, int port) {
	constexpr socklen_t size = sizeof(sockaddr_in);
	timeval to;

	own.sddr.sin_addr.s_addr = inet_addr(ip.c_str());
	own.sddr.sin_port = htons(port);
	own.sddr.sin_family = AF_INET;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock > -1);

	char *popt = reinterpret_cast<char *>(&to);
	int opt = sizeof(to);
	to.tv_sec  = 0;
	to.tv_usec = 0;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, popt, opt);
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, popt, opt);
			
	uint32_t val = SOCKBUF;
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &val, U32S);
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &val, U32S);

	assert(connect(sock, own.ptr, size) == 0);
}

bool netw::recv_data(byte *buff, int size) {
	RET_IF(size < 1 || !buff, false);
	int step = 0, rl;

	while ((rl = recv(sock, buff + step, size, 0)) != size) {
		RET_IF(rl == -1, false);
		size -= rl;
		step += rl;
	}

	return true;
}

bool netw::send_data(byte *buff, int size) {
	RET_IF(size < 1 || !buff, false);
	int step = 0, rl;

	while ((rl = send(sock, buff + step, size, 0)) != size) {
		RET_IF(rl == -1, false);
		size -= rl;
		step += rl;
	}

	return true;
}

netw::~netw(void) {
	if (sock > -1) {
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
}