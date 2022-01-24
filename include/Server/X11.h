
#ifndef _DESKX_SERVER_X11_H_
#define _DESKX_SERVER_X11_H_

#include "../DeskX.h"

class X11 {
private:
	XShmSegmentInfo shm;
	XWindowAttributes attrs;
	Display *disp = nullptr;
	byte *prevb = nullptr, *nextb = nullptr;
	XImage *img;
	int scr;
	Window root;
	size_t maxpix;
	bool firststart = true;

	void destroyBuffers(void);

public:
	void resolution(uint16_t &, uint16_t &);
	size_t bufferTCP(void);
	void events(Events &);
	uint64_t screen(byte *);
	void start(void);
	X11(void);
	~X11(void);
};

#endif