
#include <macro.hpp>
#if OS == WIN
#include <time.h>
#endif
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net.hpp>
#if OS == WIN
struct timeval {
	long tv_sec, tv_usec;

};
#endif

namespace net {
namespace {

constexpr socklen_t size = sizeof(sockaddr_in);
sockaddr_in own, client;
sockaddr *ptr = reinterpret_cast<sockaddr *>(&own);
int sock = -1, trg = -1;
args::type mode_;

void
rmsocket(int &fd) {
	RET_IF(fd < 0);
	::shutdown(fd, SHUT_RDWR);
	::close(fd);
	fd = -1;
}

void
opts(int &fd) {
	timeval tv = {10, 0};
	char *opt = reinterpret_cast<char *>(&tv);
	int flag;

	flag = ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,  opt, sizeof(tv));
	if (flag != 0) {
		INFO(WARN"Can't set flag SO_RCVTIMEO for socket");
	}
	
	flag = ::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO,  opt, sizeof(tv));
	if (flag != 0) {
		INFO(WARN"Can't set flag SO_SNDTIMEO for socket");
	}

	flag = 1;
	flag = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag));
	if (flag != 0) {
		INFO(WARN"Can't set flag SO_KEEPALIVE for socket");
	}

	flag = 4;
	flag = ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &flag, sizeof(flag));
	if (flag != 0) {
		INFO(WARN"Can't set flag TCP_KEEPCNT for socket");
	}
}

void
reuse(int &fd) {
	int flag = 1;
	flag = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	if (flag != 0) {
		INFO(WARN"Can't set flag SO_REUSEADDR for socket");
	}
}

void
buff(int &fd) {
	constexpr uint32_t max = 8192;
	const void *ptr = reinterpret_cast<const void *>(&max);
	int flag;

	flag = ::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, ptr, sizeof(max));
	if (flag != 0) {
		INFO(WARN"Can't set flag SO_RCVBUF for socket");
	}

	flag = ::setsockopt(fd, SOL_SOCKET, SO_SNDBUF, ptr, sizeof(max));
	if (flag != 0) {
		INFO(WARN"Can't set flag SO_SNDBUF for socket");
	}
}

}

bool
start(const std::string &ip, const size_t &port, const args::type mode) {
	mode_ = mode;
	RET_IF(mode == args::type::UNKNOWN, false);

	own.sin_addr.s_addr = ip.empty() ? INADDR_ANY : ::inet_addr(ip.c_str());
	own.sin_port = htons(port);
	own.sin_family = AF_INET;

	sock = ::socket(AF_INET, SOCK_STREAM, 0);
	RET_IF(sock < 0, false);
	reuse(sock);

	if (mode == args::type::CLIENT) {
		opts(sock);
		return ::connect(sock, ptr, size) == 0;
	}
	return ::bind(sock, ptr, size) == 0 && ::listen(sock, 2) == 0;
}

bool
connection(void) {
	RET_IF(mode_ != args::type::SERVER, false);

	sockaddr *cptr = reinterpret_cast<sockaddr *>(&client);
	socklen_t tmp = size; 
	trg = ::accept(sock, cptr, &tmp);
	RET_IF(trg < 0, false);

	opts(trg);
	return true;
}

void
kick(void) {
	rmsocket(trg);
}

status
recv(byte *buff, int size) {
	const int &fd = mode_ == args::type::SERVER ? trg : sock;
	int shift = 0, ret;

	while ((ret = ::recv(fd, buff + shift, size, 0)) != size) {
		RET_IF(ret < 1, status::EMPTY);
		size  -= ret;
		shift += ret;
	}

	return ret == size ? status::OK : status::FAIL;
}

status
send(const byte *buff, int size) {
	const int &fd = mode_ == args::type::SERVER ? trg : sock;
	int shift = 0, ret;

	while ((ret = ::send(fd, buff + shift, size, MSG_NOSIGNAL)) != size) {
		RET_IF(ret < 1, status::FAIL);
		size  -= ret;
		shift += ret;
	}

	return ret == size ? status::OK : status::FAIL;
}

void
close(void) {
	rmsocket(trg);
	rmsocket(sock);
}

}