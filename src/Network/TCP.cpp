
#include "../../include/DeskX.h"

TCP::TCP(int port, std::string ip) {
	int ret;

	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	ERROR(sock1 < 0, "Can't create new TCP socket.");

	Tools::Sddr(own, port, ip);
	SetOpts(sock1);

	sender = !ip.empty();
	init = true;

	if (sender) {
		ret = connect(sock1, own.ptr, SDDR_SIZE);
		ERROR(ret != 0, "Fail to connect.");
		return;
	}

	ret = bind(sock1, own.ptr, SDDR_SIZE);
	ERROR(ret != 0, "Current TCP port is busy.");
	assert(listen(sock1, 5) == 0);
}

void TCP::BufferSize(uint64_t size) {
	auto SockBuffer = [](int &sock, uint64_t size) {
		setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&size, U64S);
		setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&size, U64S);
	};

	SockBuffer(sock1, size);
	if (!sender) {
		SockBuffer(sock2, size);
	}
}

bool TCP::Accept(void) {
	socklen_t size = SDDR_SIZE;
	assert(!sender);

	sock2 = accept(sock1, out.ptr, &size);
	RET_IF(sock2 < 0, false);

	SetOpts(sock2);
	return true;
}

bool TCP::Recv(byte *buff, uint32_t size) {
	int sock = (sender) ? sock1 : sock2;
	int step = 0, len;

	while ((len = recv(sock, buff + step, size, 0)) != size) {
		if (len < 1) {
			Tools::RmSocket(sock);
			return false;
		}
		size -= len;
		step += len;
	}

	return true;
}

bool TCP::Send(byte *buff, uint32_t size) {
	int sock = (sender) ? sock1 : sock2;
	int step = 0, len;

	RET_IF(sock == -1, false);

	while ((len = send(sock, buff + step, size, 0)) != size) {
		RET_IF(len < 0, false);
		size -= len;
		step += len;
	}

	return true;
}

std::string TCP::GetIp(void) {
	char tmp[21];
	assert(!sender);

	inet_ntop(AF_INET, &out.sddr.sin_addr, tmp, 20);
	return std::string(tmp);
}

void TCP::SetOpts(int &s) {
	struct timeval tv;
	int re = 1;

	char *opt = reinterpret_cast<char *>(&tv);
	tv.tv_sec  = 6;
	tv.tv_usec = 0;

	setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &re, sizeof(re));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,  opt, sizeof(tv));
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO,  opt, sizeof(tv));
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re));
}

void TCP::CloseConnection(void) {
	Tools::RmSocket(sock2);
}

TCP::~TCP(void) {
	Tools::RmSocket(sock1);
	Tools::RmSocket(sock2);
}