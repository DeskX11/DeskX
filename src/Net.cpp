
#include "../include/DeskX.h"

Net::Net(const int port, const std::string ip, const bool srv_) {
	RETVOID_IF(!port);

	own.in.sin_addr.s_addr = srv_ && ip.empty() ? INADDR_ANY : inet_addr(ip.c_str());
	own.in.sin_port = htons(port);
	own.in.sin_family = AF_INET;

	master = socket(AF_INET, SOCK_STREAM, 0);
	ERR(master < 0, "Unable to create new socket");
	opts(master);

	if ((server = srv_)) {
		ERR(bind(master, own.ptr, Consts::sddr) != 0, "Unable to bind network port to socket");
		ERR(listen(master, 5) != 0, "Unable to start listening to socket");
	}
}

void Net::opts(int &sock) {
	timeval tv = {10, 0};
	char *opt = reinterpret_cast<char *>(&tv);
	int re = 1;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,  opt, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,  opt, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re));
}

void Net::buffer(const uint64_t size) {
	auto set = [](int &sock, const uint64_t size) {
		setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&size, Consts::u64);
		setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&size, Consts::u64);
	};

	server ? set(slave, size) : set(master, size);
}

bool Net::connect(void) {
	RET_IF(server, false);
	return ::connect(master, own.ptr, Consts::sddr) == 0;
}

bool Net::accept(void) {
	socklen_t size = Consts::sddr;

	RET_IF(!server, false);
	slave = ::accept(master, out.ptr, &size);
	RET_IF(slave < 0, false);

	opts(slave);
	return true;
}

bool Net::send(byte *ptr, uint32_t size) {
	int sock = server ? slave : master;
	int ret, step = 0, flag = 0;

#if !defined(__APPLE__) && !defined(__CYGWIN__)
	flag = MSG_NOSIGNAL;
#endif
	while ((ret = ::send(sock, ptr + step, size, flag)) != size) {
		RET_IF(ret < 1, false);
		step += ret;
		size -= ret;
	}
	return true;
}

bool Net::recv(byte *ptr, uint32_t size) {
	int sock = server ? slave : master;
	int ret, step = 0;

	while ((ret = ::recv(sock, ptr + step, size, 0)) != size) {
		RET_IF(ret < 1, false);
		step += ret;
		size -= ret;
	}
	return true;
}

const std::string Net::from(void) {
	char tmp[21];

	RET_IF(!server, std::string());
	inet_ntop(AF_INET, &out.in.sin_addr, tmp, 20);
	return std::string(tmp);
}

void Net::rm(int &sock) {
	RETVOID_IF(sock < 0);
	shutdown(sock, SHUT_RDWR);
	::close(sock);
	sock = -1;
}

void Net::operator=(const Net &tcp) {
	master = tcp.master;
	slave  = tcp.slave;
	server = tcp.server;
	own = tcp.own;
	out = tcp.out;
}

void Net::close(void) {
	rm(slave);
}

void Net::closeAll(void) {
	close();
	rm(master);
}
