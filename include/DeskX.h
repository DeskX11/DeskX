
#ifndef _DESKX_MAIN_H_
#define _DESKX_MAIN_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/XTest.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/shm.h>
#include <functional>
#include <algorithm>
#include <numeric>
#include <unistd.h>
#include <assert.h>
#include <cstring>
#include <string>
#include <thread>
#include <map>
/**
 *	Macro set.
 */
#define ERROR(cond, text)		if ((cond)) { std::cout << "Error: " << std::string((text)) << "\n"; exit(1); }
#define RET_IF(cond, value)		if ((cond)) return (value);
#define NEXT_IF(cond)			if ((cond)) continue;
#define RET_IF_VOID(cond)		if ((cond)) return;
#define BREAK_IF(cond)			if ((cond)) break;

typedef unsigned char byte;
/**
 *	The size of the data types used.
 */
constexpr size_t SDDR_SIZE	= sizeof(sockaddr_in);
constexpr size_t U64S		= sizeof(uint64_t);
constexpr size_t U32S		= sizeof(uint32_t);
constexpr size_t U16S		= sizeof(uint16_t);
/**
 *	The size of the transmitted color coding blocks.
 *
 *	COLOR_BLOCK:
 *		1. Number of repetitions
 *		2. Red color
 *		3. Green color
 *		4. Blue color
 *
 *	LINKED_BLOCK:
 *		1. Flag (0x00)
 *		2. Number of repetitions
 *		3. Color id
 *
 *	EQUAL_BLOCK:
 *		1. Flag (0xFF)
 *		2. Number of repetitions
 */
constexpr size_t COLOR_BLOCK	= 4;
constexpr size_t LINKED_BLOCK	= 3;
constexpr size_t EQUAL_BLOCK	= U32S + 1;
/**
 *	Packet sizes transmitted over TCP protocol (Max size).
 *
 *	AUTH_SIZE:
 *		1. Mode
 *		2. Password (MD5)
 *		3. Difference between pixels
 *
 *	TABLE_SIZE:
 *		1. Number of items per table
 *		2. Table elements
 */
constexpr size_t AUTH_SIZE	= 2 + MD5_DIGEST_LENGTH;
constexpr size_t TABLE_SIZE	= U32S * 255 + 1;

constexpr size_t UDP_HEADER = 68;
/**
 *	Declaration of used structures.
 */
struct sddr_struct {
	sockaddr_in	sddr;
	sockaddr	*ptr;

	sddr_struct(void) {
		ptr = reinterpret_cast<sockaddr *>(&sddr);
	}
	void operator=(sddr_struct &element) {
		sddr = element.sddr;
	}
};

struct input {
	std::string ip = "", pass = "", cmd = "rat",
				events = "tcp",  screen = "tcp";
	bool secure = false;
	uint8_t comp = 15;
	int port = 0;
};

#include "Tools.h"
#include "Network/UDP.h"
#include "Network/TCP.h"

#endif
