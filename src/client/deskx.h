
#ifndef _DESKX_CLIENT_
#define _DESKX_CLIENT_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
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
#define EXIT_KEY (uint8_t)73 // F7

constexpr size_t U32S = sizeof(uint32_t);
constexpr size_t U16S = sizeof(uint16_t);
constexpr size_t U8TS = sizeof(uint8_t);
constexpr size_t BSIZE = U8TS * 4;
constexpr size_t ESIZE = U32S + 1;
constexpr size_t MSIZE = U16S * 2 + U8TS;
constexpr size_t RSIZE = U8TS * 2 + MD5_DIGEST_LENGTH;

typedef unsigned char byte;

inline std::string man_text("\033[1mDeskX\033[0m - Program for remote control "
"of a computer in a local network. Client side.\n\n"
"Usage: ./dxс [options]\n"
"Options:\n"
"	--ip			Ip address of the server\n"
"	--port			Port of the server\n"
"	--password		Verification secret word without spaces\n"
"	--compression		Compression range (0 to 255)\n"
"	--cmd			Server side command (default: rat)\n\n"
"Commands:\n"
"	exit			Command to shutdown the server side\n"
"	rat 			Start remote control\n"
"	screenshot		Get a picture of the server desktop\n\n"
"Example:\n"
"	./dxс --ip=192.168.1.1 --port=4431 --password=secret --compression=15\n");

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
	bool secure = false;
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

	Window new_window(int, int, bool);
	void sharedmem_alloc(int);
	void fullscreen(Window);

public:
	size_t get_events(byte *, bool &);
	void set_pixels(byte *, uint32_t);
	x11_client(headers);
	void add_links(byte *, uint32_t);
	~x11_client(void);
};

#endif