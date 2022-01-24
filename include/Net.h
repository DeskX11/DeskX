
#ifndef _DESKX_NET_H_
#define _DESKX_NET_H_

struct Sddr {
	sockaddr  *ptr;
	sockaddr_in	in;

	Sddr(void) {
		ptr = reinterpret_cast<sockaddr *>(&in);
	}
	void operator=(const Sddr &element) {
		in = element.in;
	}
};

class Net {
protected:
	int master = -1, slave = -1;
	Sddr own, out;
	bool server;

	void rm(int &);
	void opts(int &);

public:
	Net(void) { }
	Net(const int p, const std::string, const bool);
	bool recv(byte *, uint32_t);
	bool send(byte *, uint32_t);
	const std::string from(void);
	void buffer(const uint64_t);
	bool connect(void);
	bool accept(void);
	void close(void);
	void operator=(const Net &);
	void closeAll(void);
};

#endif
