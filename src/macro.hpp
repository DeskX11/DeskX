
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

#define RET_IF(cond, ...) if ((cond)) return __VA_ARGS__
#define BREAK_IF(cond)	  if ((cond)) break
#define NEXT_IF(cond)	  if ((cond)) continue
#define NEXT_DELAY(cond)  if ((cond)) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue; }
#define NOW_MSEC 		  std::chrono::system_clock::now().time_since_epoch()
#define INFO(txt)		  { std::cout << (logo_ ? LOGO : "") << std::string((txt)) << ".\n"; logo_ = false; }
#define DIE(cond)		  if ((cond)) { std::cout << (logo_ ? LOGO : "") << "\033[1;31mCrash\033[0m:\t" \
										"In file " __FILE__ ", line " <<  __LINE__ << ".\n"; ::exit(1); }
#define RGB		byte{0}
#define RGBA	byte{1}
#define BGRA	byte{2}
#define OSX		1
#define WIN		2
#define LINUX	3
#define OTHER	0
#define ERR		"\033[1;31mError\033[0m:\t"
#define NOTE	"\033[1;32mNote\033[0m:\t"
#define WARN	"\033[1;33mWarn\033[0m:\t"

#define SCR_X_MIN 0x0280
#define SCR_Y_MIN 0x01E0
#define SCR_X_MAX 0x1FFF
#define SCR_Y_MAX SCR_X_MAX

#if defined(__APPLE__)
#define OS	OSX
#elif (defined(__CYGWIN__) || defined(_WIN32))
#define OS	WIN
#elif defined(__linux__)
#define OS	LINUX
#define TTY		byte{0}
#define X11		byte{1}
#define WAYLAND	byte{2}
#else
#define OS	OTHER
#endif

#endif
