
#ifndef DESKX_DISPLAY_WAYLAND_PIPEWIRE_HPP
#define DESKX_DISPLAY_WAYLAND_PIPEWIRE_HPP

#include <spa/param/video/format-utils.h>
#include <pipewire/main-loop.h>
#include <pipewire/pipewire.h>
#include <macro.hpp>

namespace display {

class pipewire {
public:
struct screen {
	uint16_t width = 0, height = 0;
	byte format;
	size_t shift;
};
struct cbdata {
	pw_stream *stream;
	screen scr;
	void *pixs;
	bool error = false;	
	bool start = false;
}	data;

	pipewire(const int &, const uint32_t &);

	~pipewire(void);

	screen
	connect(void);

	void *
	next(void);

private:
	pw_main_loop *loop = nullptr;
	pw_context *context;
	pw_core *core;
	pw_stream *stream = nullptr;
	spa_hook listener;
	uint32_t node;
};

}

#endif