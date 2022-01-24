
#ifndef _DESKX_CLIENT_X11_H_
#define _DESKX_CLIENT_X11_H_

#include "../DeskX.h"

class X11 {
protected:
	size_t height, width, tcpbuff;
	Display *disp = nullptr;
	XShmSegmentInfo shm;
	Window win;
	XImage *img;
	GC gc;
	int scr;

	Window newWindow(int, int, bool);

public:
	size_t bufferTCP(void);
	void screen(uint64_t, byte *);
	Events events(void);
	X11(void);
	~X11(void);
};

#endif