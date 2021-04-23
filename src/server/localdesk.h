
#ifndef _LOCAL_DESK_
#define _LOCAL_DESK_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/XTest.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/shm.h>
#include <functional>
#include <unistd.h>
#include <assert.h>
#include <cstring>
#include <string>
#include <fstream>

#define MD5S MD5_DIGEST_LENGTH
#define RET_IF(cond, value)		if ((cond)) return (value);
#define RET_IF_VOID(cond)		if ((cond)) return;
#define BREAK_IF(cond)			if ((cond)) break;
#define BLOCKMAX 65533

constexpr size_t U32S = sizeof(uint32_t);
constexpr size_t U16S = sizeof(uint16_t);
constexpr size_t U8TS = sizeof(uint8_t);
constexpr size_t BSIZE = U16S + 3;
constexpr size_t MSIZE = U16S * 2 + 1;

typedef unsigned char byte;

struct sddr_struct {
	sockaddr_in sddr;
	sockaddr *ptr;

	sddr_struct(void) {
		ptr = reinterpret_cast<sockaddr *>(&sddr);
	}

	void operator=(sddr_struct &element) {
		sddr = element.sddr;
	}
};

struct pix {
	uint16_t num = 0;
	char r, g, b;
};

struct headers {
	static constexpr size_t size = U32S * 4;
	uint32_t depth = 0, bitmap = 0,
			 height = 0, width = 0;

	void from(byte *ptr) {
		assert(ptr);
		memcpy(&depth,	ptr,			U32S);
		memcpy(&bitmap,	ptr + U32S,		U32S);
		memcpy(&width,	ptr + U32S * 2,	U32S);
		memcpy(&height,	ptr + U32S * 3,	U32S);
	}

	void to(byte *ptr) {
		assert(ptr);
		memcpy(ptr,				&depth,  U32S);
		memcpy(ptr + U32S,		&bitmap, U32S);
		memcpy(ptr + U32S * 2,	&width,  U32S);
		memcpy(ptr + U32S * 3,	&height, U32S);
	}
};


class tcp_net {
private:
	sddr_struct own, client;
	bool init = false;
	int sock1, sock2;

	void close_socket(int);

public:
	tcp_net(int);
	~tcp_net(void);
	bool accept_connection(void);
	void close_connection(void);
	bool recv_data(byte *, int);
	bool send_data(byte *, int);
	void stop(void);
	bool work(void) { return init; }
};

class x11_server {
private:
	Display *disp = nullptr;
	XWindowAttributes attrs;
	XShmSegmentInfo shm;
	bool inited = false;
	uint8_t comp;
	Window root;
	XImage *img;
	int scr, depth;

	void sharedmem_alloc(void);

public:
	void set_mouse(uint16_t, uint16_t);
	void set_events(byte *, uint8_t);
	void pixels_vector(std::vector<pix> &);
	headers get_headers(void);
	int pixs_num(void);
	x11_server(uint8_t);
	~x11_server(void);
};

#endif