
#ifndef _DESKX_MAIN_H_
#define _DESKX_MAIN_H_
#ifndef VERSION
#define VERSION "0.0"
#endif

#include <netinet/ip.h>
#include <netinet/tcp.h>
#if defined(__APPLE__) || defined(__CYGWIN__)
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
#include <sstream>
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
#include <cmath>
#include <map>

#define ERR(cond, text)			if ((cond)) { std::cout << "Error: " << std::string(text) << ".\n"; exit(1); }
#define RET_IF(cond, ...)		if ((cond)) return __VA_ARGS__;
#define NEXT_IF(cond)			if ((cond)) continue;
#define BREAK_IF(cond)			if ((cond)) break;

typedef unsigned char byte;

namespace Consts {
	constexpr size_t sddr = sizeof(sockaddr_in);
	constexpr size_t u64  = sizeof(uint64_t);
	constexpr size_t u32  = sizeof(uint32_t);
	constexpr size_t u16  = sizeof(uint16_t);
	constexpr size_t req  = u16 * 2 + 4;
	constexpr size_t res  = u16 * 2 + 1;
	constexpr size_t key  = Consts::u32 + 1;
	constexpr size_t emax = key * 256 + u16 * 2 + 1;
	constexpr size_t rgba = 4;
	constexpr size_t rgb5bit = 8;		// 255 / 31
	constexpr size_t rgb4bit = 17;		// 255 / 15
	constexpr size_t rgb2bit = 85;		// 255 / 3
	constexpr size_t rgb4to2bit = 5;	// 15 / 3
	constexpr size_t rgb5to2bit = 10;	// 31 / 3 
	const std::string logo = "\n"
" __   ___  __       \\   /  Remote access software from a remote client\n"
"|  \\ |__  /__` |__/  \\_/   to a computer hosting an X Window session.\n"
"|__/ |___ .__/ |  \\  / \\   Version: " + std::string(VERSION) + "\n"
"                    /   \\  https://github.com/DeskX11/DeskX\n\n";
}

#if !defined(__APPLE__) && !defined(__CYGWIN__)
inline void sharedMem(XShmSegmentInfo &shm, char **ptr, int length) {
	shm.shmid = shmget(IPC_PRIVATE, length * 2, IPC_CREAT | 0777);
	assert(shm.shmid > 0);

	*ptr = (char *)shmat(shm.shmid, 0, 0);
	shm.readOnly = false;
	shm.shmaddr  = *ptr;
	assert(shm.shmaddr != (char *)-1);

	shmctl(shm.shmid, IPC_RMID, 0);
}
#endif

#include "Net.h"
#include "Args.h"
#include "Events.h"
#include "Codec.h"
#endif
