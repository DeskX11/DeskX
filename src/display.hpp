
#ifndef DESKX_DISPLAY_HPP
#define DESKX_DISPLAY_HPP

#include <array>
#include <utility>
#include <inttypes.h>
#include <macro.hpp>

#define MAXKEYS 5

namespace display {

constexpr size_t emsg = MAXKEYS * 5 + 4;
using xy = std::pair<uint16_t, uint16_t>;

struct pixs {
	byte *ptr = nullptr;
	size_t shift = 0;

	void
	next(void);
};

struct keys {
enum types : byte {
	MOUSE_UP = 0, MOUSE_DOWN = 1,
	KEY_UP   = 2, KEY_DOWN   = 3,
	NO_TYPE  = 4
}	type;
	uint32_t key;
};

struct events {
	xy mouse;
	std::array<keys, MAXKEYS> buttons;

	events(void) { }

	events(const byte *);

	void
	clear(void);

	void
	pack(byte *);

	void
	set(const byte *);
};

class tpl {
public:
	virtual
	~tpl(void) = default;

	virtual bool
	init(void) = 0;

	virtual void
	close(void) = 0;

	virtual xy
	res(void) = 0;

	virtual void
	refresh(pixs &) = 0;

	virtual void
	set(const events &) = 0;
};

tpl *
get(const byte type = 0);

}

#if OS == LINUX
	#include <display/x11.hpp>
	#include <display/wayland.hpp>
#elif OS == OSX
	#include <display/osx.hpp>
#elif OS == WIN
	#include <display/win.hpp>
#endif

#endif