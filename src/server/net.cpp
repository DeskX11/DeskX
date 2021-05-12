
#include "deskx.h"

netw::netw(int port) {
	constexpr socklen_t size = sizeof(sockaddr_in);

	own.sddr.sin_addr.s_addr = INADDR_ANY;
	own.sddr.sin_port = htons(port);
	own.sddr.sin_family = AF_INET;

	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock1 > -1);

	set_options(sock1);

	assert(bind(sock1, own.ptr, size) == 0);
	assert(listen(sock1, 5) == 0);

	init = true;
}

bool netw::accept_connection(void) {
	socklen_t size = sizeof(sockaddr_in);
	
	sock2 = accept(sock1, client.ptr, &size);
	RET_IF(sock2 == -1, false);

	set_options(sock2);
	return true;
}

void netw::close_connection(void) {
	close_socket(sock2);
}

bool netw::recv_data(byte *buff, int size) {
	RET_IF(size < 1 || !buff, false);
	int step = 0, rl;

	while ((rl = recv(sock2, buff + step, size, 0)) != size) {
		RET_IF(rl == -1, false);
		size -= rl;
		step += rl;
	}

	return true;
}

bool netw::send_data(byte *buff, int size) {
	RET_IF(size < 1 || !buff, false);
	int step = 0, rl;

	while ((rl = send(sock2, buff + step, size, 0)) != size) {
		RET_IF(rl == -1, false);
		size -= rl;
		step += rl;
	}

	return true;
}

void netw::close_socket(int fd) {
	if (fd > -1) {
		shutdown(fd, SHUT_RDWR);
		close(fd);
	}
}

void netw::stop(void) {
	init = false;
}

void netw::set_options(int sock) {
	timeval to;
	char *popt = reinterpret_cast<char *>(&to);
	int opt = sizeof(to);
	to.tv_sec  = 0;
	to.tv_usec = 0;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, popt, opt);
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, popt, opt);

	uint32_t val = SOCKBUF;
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &val, U32S);
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &val, U32S);
}

netw::~netw(void) {
	close_socket(sock1);
	close_socket(sock2);
}