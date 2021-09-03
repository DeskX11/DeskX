
#ifndef _DESKX_MAIN_H_
#define _DESKX_MAIN_H_

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#ifdef __APPLE__
#define iphdr ip
#include <SDL2/SDL.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/XTest.h>
#endif
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/shm.h>
#include <sys/time.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <ctime>
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
constexpr size_t TV_SIZE	= sizeof(timeval);
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
 *		2. Flag (0x00)
 *		3. Number of repetitions
 *
 *	INSIDE_BLOCK:
 *		1. Flag (0xFF)
 *		2. Number of repetitions
 *
 *	VERT_BLOCK:
 *		1. Flag (0xFE)
 *		2. Number of repetitions
 */
constexpr size_t COLOR_BLOCK	= 4;
constexpr size_t LINKED_BLOCK	= 3;
constexpr size_t EQUAL_BLOCK	= U32S + 2;
constexpr size_t INSIDE_BLOCK	= 2;
constexpr size_t VERT_BLOCK		= 2;
/**
 *	Packet sizes transmitted over TCP protocol (Max size).
 *
 *	AUTH_SIZE:
 *		1. Mode
 *		2. Difference between pixels
 *		3. Screen protocol
 *		4. Events protocol
 *		5. Vertical compression
 *		6. X11/SDL events
 *
 *	TABLE_SIZE:
 *		1. Number of items per table
 *		2. Table elements
 *
 *	KEY_BLOCK:
 *		1. Event type
 *		2. Key id
 */
constexpr size_t AUTH_SIZE	= 6;
constexpr size_t TABLE_SIZE	= U32S * 255 + 1;
constexpr size_t KEY_BLOCK = 1 + U32S;
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
	std::string ip = "", cmd = "rat", events = "tcp", screen = "tcp",
				display = "", xauth  = "", path = "./palette.deskx";
	bool secure = false, dvert = false;
	uint8_t comp = 16;
	int port = 0;
};

#include "Tools.h"
#include "Network/UDP.h"
#include "Network/TCP.h"

#endif
