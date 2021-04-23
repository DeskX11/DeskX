
#include "localdesk.h"

tcp_net::tcp_net(int port) {
	constexpr socklen_t size = sizeof(sockaddr_in);
	timeval to;

	own.sddr.sin_addr.s_addr = INADDR_ANY;
	own.sddr.sin_port = htons(port);
	own.sddr.sin_family = AF_INET;

	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock1 > -1);

	char *popt = reinterpret_cast<char *>(&to);
	int opt = sizeof(to);
	to.tv_sec  = 0;
	to.tv_usec = 0;

	setsockopt(sock1, SOL_SOCKET, SO_RCVTIMEO, popt, opt);
	setsockopt(sock1, SOL_SOCKET, SO_SNDTIMEO, popt, opt);

	assert(bind(sock1, own.ptr, size) == 0);
	assert(listen(sock1, 5) == 0);

	init = true;
}

bool tcp_net::accept_connection(void) {
	socklen_t size = sizeof(sockaddr_in);
	
	sock2 = accept(sock1, client.ptr, &size);
	return sock2 != -1;
}

void tcp_net::close_connection(void) {
	close_socket(sock2);
}

bool tcp_net::recv_data(byte *buff, int size) {
	RET_IF(size < 1 || !buff, false);
	int step = 0, rl;

	while ((rl = recv(sock2, buff + step, size, 0)) != size) {
		RET_IF(rl == -1, false);
		size -= rl;
		step += rl;
	}

	return true;
}

bool tcp_net::send_data(byte *buff, int size) {
	RET_IF(size < 1 || !buff, false);
	int step = 0, rl;

	while ((rl = send(sock2, buff + step, size, 0)) != size) {
		RET_IF(rl == -1, false);
		size -= rl;
		step += rl;
	}

	return true;
}

void tcp_net::close_socket(int fd) {
	if (fd > -1) {
		shutdown(fd, SHUT_RDWR);
		close(fd);
	}
}

void tcp_net::stop(void) {
	init = false;
}

tcp_net::~tcp_net(void) {
	close_socket(sock1);
	close_socket(sock2);
}