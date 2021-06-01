
#ifndef _DESKX_CLIENT_X11_H_
#define _DESKX_CLIENT_X11_H_

#define EXIT_KEY (uint8_t)73 // F7

class X11 {
private:
	std::vector<uint32_t> links;
	Display *disp = nullptr;
	int scr;
	size_t height, width, max, scrshift = 0;
	XShmSegmentInfo shm;
	uint32_t linksnum = 0;
	Window win;
	XImage *img;
	GC gc;

	uint8_t size1, size2, size3;

	Window NewWindow(int, int, bool);
	void FullScreen(Window &);

public:
	void Set(byte *, uint32_t len = 0);
	uint8_t GetEvents(byte *);
	void SetLinks(byte *, uint32_t);
	void AddLinks(uint8_t, byte *);
	X11(uint32_t, uint32_t);
	size_t Size(void) { return max; }
	void ScreenProtocol(bool);
	~X11(void);
};

#endif