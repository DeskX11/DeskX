
#ifndef _LOCAL_DESK_
#define _LOCAL_DESK_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/XTest.h>
#include <netinet/tcp.h>
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

#define RET_IF(cond, value)		if ((cond)) return (value);
#define NEXT_IF(cond)			if ((cond)) continue;
#define RET_IF_VOID(cond)		if ((cond)) return;
#define BREAK_IF(cond)			if ((cond)) break;
#define SOCKBUF (uint32_t)50368000

constexpr size_t U32S = sizeof(uint32_t);
constexpr size_t U16S = sizeof(uint16_t);
constexpr size_t U8TS = sizeof(uint8_t);
constexpr size_t BSIZE = U8TS * 4;
constexpr size_t MSIZE = U16S * 2 + U8TS;
constexpr size_t RSIZE = U8TS * 2 + MD5_DIGEST_LENGTH;

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

struct input {
	std::string ip = "", pass = "", cmd = "rat";
	bool full = false;
	uint8_t comp = 10;
	int port = 0;
};

class netw {
private:
	sddr_struct own;
	int sock;

public:
	netw(std::string, int);
	bool recv_data(byte *, int);
	bool send_data(byte *, int);
	~netw(void);
};

class x11_client {
private:
	int scr, height, width;
	unsigned int maxpix;
	Display *disp = nullptr;
	XShmSegmentInfo shm;
	Window win;
	XImage *img;
	GC gc;

	std::vector<uint32_t> links;
	uint32_t lnum = 0;

	Window new_window(int, int);
	void sharedmem_alloc(int);

public:
	size_t get_events(byte *);
	void set_pixels(byte *, uint32_t);
	x11_client(headers, bool);
	void add_links(byte *, uint32_t);
	~x11_client(void);
};

#endif