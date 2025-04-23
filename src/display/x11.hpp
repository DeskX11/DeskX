
#ifndef DESKX_DISPLAY_X11_HPP
#define DESKX_DISPLAY_X11_HPP

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

namespace display {

class x11 : public tpl {
private:
	XWindowAttributes attrs;
	Display *disp = nullptr;
	XImage  *img  = nullptr;
	XShmSegmentInfo shm;
	Window root;

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