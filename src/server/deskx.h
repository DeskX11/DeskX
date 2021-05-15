
#ifndef _DESKX_SERVER_
#define _DESKX_SERVER_

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
#include <map>

#define MD5S MD5_DIGEST_LENGTH
#define RET_IF(cond, value)		if ((cond)) return (value);
#define NEXT_IF(cond)			if ((cond)) continue;
#define RET_IF_VOID(cond)		if ((cond)) return;
#define BREAK_IF(cond)			if ((cond)) break;
#define SOCKBUF  (uint32_t)50368000
#define BLOCKMAX 255

constexpr size_t U32S = sizeof(uint32_t);
constexpr size_t U16S = sizeof(uint16_t);
constexpr size_t U8TS = sizeof(uint8_t);
constexpr size_t ESIZE = U32S + 1;
constexpr size_t BSIZE = U8TS * 4;
constexpr size_t MSIZE = U16S * 2 + 1;

typedef unsigned char byte;

inline std::string man_text("\033[1mDeskX\033[0m - Program for remote "
"control of a computer in a local network. Server side.\n\n"
"Usage: ./dxs [options]\n"
"Options (All are required):\n"
"	--port			Connection port\n"
"	--password		Verification secret word without spaces\n\n"
"Example:\n"
"	./dxs --port=4431 --password=secret\n");

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

struct input {
	std::string pass = "";
	int tcpport = 0, udpport = 0;
};

struct pix {
	uint8_t num = 0, link_id = 0;
	uint32_t eqn;
	byte r, g, b;
	bool link, eq;

	uint32_t u32(void) {
		uint32_t buff;
		char *ptr = reinterpret_cast<char *>(&buff);
		ptr[0] = 0;
		ptr[1] = r;
		ptr[2] = g;
		ptr[3] = b;
		return buff;
	}

	void set(byte *pixs) {
		link = false;
		eq   = false;
		r = pixs[0];
		g = pixs[1];
		b = pixs[2];
		num = 0;
		eqn = 0;
	}
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


class netw {
private:
	sddr_struct own, client;
	bool init = false;
	int sock1, sock2;

	void close_socket(int);
	void set_options(int);

public:
	netw(int);
	~netw(void);
	bool accept_connection(void);
	void close_connection(void);
	bool recv_data(byte *, int);
	bool send_data(byte *, int);
	void stop(void);
	bool work(void) { return init; }
};

class x11_server {
private:
	std::map<uint32_t, size_t> links;
	Display *disp = nullptr;
	XWindowAttributes attrs;
	XShmSegmentInfo shm;
	bool inited = false, fsend = false;
	byte *prevb, *nextb;
	uint8_t comp;
	Window root;
	XImage *img;
	int scr, depth;

	void sharedmem_alloc(void);
	void color_linking(pix &);

public:
	void set_mouse(uint16_t, uint16_t);
	void set_events(byte *, uint8_t);
	size_t pixels_buffer(byte *);
	void pixels_vector(std::vector<pix> &arr);
	headers get_headers(void);
	int pixs_num(void);
	x11_server(uint8_t);
	~x11_server(void);
	void links_table(byte *, size_t &);
};

#endif