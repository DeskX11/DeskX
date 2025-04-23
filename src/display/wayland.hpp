
#ifndef DESKX_DISPLAY_WAYLAND_HPP
#define DESKX_DISPLAY_WAYLAND_HPP

namespace display {

class wayland : public tpl {
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