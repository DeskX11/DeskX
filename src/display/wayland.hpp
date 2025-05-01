
#ifndef DESKX_DISPLAY_WAYLAND_HPP
#define DESKX_DISPLAY_WAYLAND_HPP

#include <display/wayland/pipewire.hpp>
#include <libportal/portal.h>

namespace display {

class wayland : public tpl {
protected:
	pipewire::screen scr;
	XdpSession *session;
	XdpPortal *portal;
	pipewire *pwr = nullptr;
	uint32_t node;

public:
	bool
	init(void);

	void
	close(void);

	xy
	res(void);

	void
	refresh(pixs &);

	void
	set(const events &);
};

}

#endif