
#include "../../include/DeskX.h"

TCP::TCP(int port, std::string ip) {
	int ret;

	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	ERROR(sock1 < 0, "Can't create new TCP socket.");

	Tools::Sddr(own, port, ip);
	Tools::SetOpts(sock1);

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
	Tools::SockBuffer(sock1, size);

	if (!sender) {
		Tools::SockBuffer(sock2, size);
	}
}

bool TCP::Accept(void) {
	socklen_t size = SDDR_SIZE;
	assert(!sender);

	sock2 = accept(sock1, out.ptr, &size);
	RET_IF(sock2 < 0, false);

	Tools::SetOpts(sock2);
	return true;
}

bool TCP::Recv(byte *buff, uint32_t size) {
	int sock = (sender) ? sock1 : sock2;
	int step = 0, len;

	while ((len = recv(sock, buff + step, size, 0)) != size) {
		RET_IF(len < 0, false);
		size -= len;
		step += len;
	}

	return true;
}

bool TCP::Send(byte *buff, uint32_t size) {
	int sock = (sender) ? sock1 : sock2;
	int step = 0, len;

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

void TCP::CloseConnection(void) {
	Tools::RmSocket(sock2);
}

TCP::~TCP(void) {
	Tools::RmSocket(sock1);
	Tools::RmSocket(sock2);
}