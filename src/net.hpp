
#ifndef DESKX_NET_HPP
#define DESKX_NET_HPP

#include <args.hpp>

namespace net {

struct hello {
	byte delta, fps;
};

struct screen {
	uint16_t width, height;
};

struct skipxy {
	byte x = 0, y = 0;
};

enum status {
	OK, EMPTY, FAIL
};

void
buff(const uint64_t &);

bool
start(const std::string &, const size_t &, const args::type);

bool
connection(void);

void
kick(void);

status
recv(byte *, int);

status
send(const byte *, int);

void
close(void);

template<typename T> bool
send(const T &msg) {
	constexpr size_t size = sizeof(T);
	return send(reinterpret_cast<const byte *>(&msg), size) == status::OK;
}

template<typename T> bool
recv(T &msg) {
	constexpr size_t size = sizeof(T);
	return recv(reinterpret_cast<byte *>(&msg), size) == status::OK;
}

}

#endif