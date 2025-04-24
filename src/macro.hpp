
#ifndef DESKX_MACRO_HPP
#define DESKX_MACRO_HPP

#include <chrono>
#include <iostream>
#include <unistd.h>

inline bool logo_ = true;
typedef unsigned char byte;

#ifndef VERSION
#define VERSION	"Unknown"
#endif
#define LOGO	"\n" \
				" __   ___  __       \\   /  Universal light and fast program for remote\n" \
				"|  \\ |__  /__` |__/  \\_/   control of a computer. (v" VERSION ")\n" \
				"|__/ |___ .__/ |  \\  / \\\n" \
				"                    /   \\  https://github.com/DeskX11/DeskX\n\n"

#define NOW_MS 			  std::chrono::system_clock::now().time_since_epoch()
#define WAIT_NEXT		  std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue
#define RET_IF(cond, ...) if ((cond)) return __VA_ARGS__
#define BREAK_IF(cond)	  if ((cond)) break
#define NEXT_IF(cond)	  if ((cond)) continue
#define INFO(txt)		  { std::cout << (logo_ ? LOGO : "") << std::string((txt)) << ".\n"; logo_ = false; }
#define DIE(cond)		  if ((cond)) { std::cout << (logo_ ? LOGO : "") << "\033[1;31mCrash\033[0m:\t" \
										"In file " __FILE__ ", line " <<  __LINE__ << ".\n"; ::exit(1); }
#define OSX		1
#define WIN		2
#define LINUX	3
#define OTHER	0
#define ERR		"\033[1;31mError\033[0m:\t"
#define NOTE	"\033[1;32mNote\033[0m:\t"
#define WARN	"\033[1;33mWarn\033[0m:\t"

#if defined(__APPLE__)
#define OS	OSX
#elif (defined(__CYGWIN__) || defined(_WIN32))
#define OS	WIN
#elif __linux__
#define OS	LINUX
#define TTY		0
#define X11		1
#define WAYLAND	2
#else
#define OS	OTHER
#endif

#endif
