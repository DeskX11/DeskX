
#include "../../include/Client.h"

void X11::FullScreen(Window &nwindow) {
	Atom atoms[2] = { XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", false), None };
	XChangeProperty(disp, nwindow, XInternAtom(disp, "_NET_WM_STATE", false),
					XA_ATOM, 32, PropModeReplace, (const unsigned char *)atoms, 1);
}

X11::X11(uint32_t in_width, uint32_t in_height) {
	XInitThreads();

	disp = XOpenDisplay(nullptr);
	scr  = XDefaultScreen(disp);

	unsigned scr_width  = (unsigned)DisplayWidth (disp, scr);
	unsigned scr_height = (unsigned)DisplayHeight(disp, scr);
	height = in_height;
	width  = in_width;

	ERROR(width > scr_width || height > scr_height,
		  "Server screen resolution is greater than client resolution.");

	bool fullsize = width == scr_width && scr_height == height;
	win = NewWindow(width, height, fullsize);
	gc  = XCreateGC(disp, win, 0, nullptr);

	img = XShmCreateImage(disp, DefaultVisual(disp, 0),
						  DefaultDepth(disp, scr), ZPixmap, nullptr,
						  &shm, width, height);
	assert(img);

	max = width * height * 4;

	Tools::SharedMem(shm, &img->data, max);
	XShmAttach(disp, &shm);
	XSync(disp, false);

	XSelectInput(disp, win, KeyPressMask     | KeyReleaseMask    |
							ButtonPressMask  | Button1MotionMask |
							ButtonReleaseMask					 );
	XFlush(disp);
}

Window X11::NewWindow(int width, int height, bool full) {
	int black = BlackPixel(disp, scr);
	int white = WhitePixel(disp, scr);

	Window w = XCreateSimpleWindow(disp, RootWindow(disp, scr), 0, 0,
								   width, height, 1, black, white  );
	XStoreName(disp, w, "DeskX - Remote control in a local network");
	if (full) {
		FullScreen(w);
	}
    XMapWindow(disp, w);

	return w;
}

void X11::AddLinks(uint8_t size, byte *buff) {
	uint32_t link = 0;
	size_t shift;

	linksnum = size;

	for (uint32_t i = 0; i < linksnum; i++) {
		shift = i * U32S;
		memcpy(&link, buff + shift, U32S);
		links.push_back(link);
	}
}

void X11::ScreenProtocol(bool udp = false) {
	size1 = (udp) ? 1 : LINKED_BLOCK;
	size2 = (udp) ? 1 : EQUAL_BLOCK;
	size3 = (udp) ? 1 : COLOR_BLOCK;
	size4 = (udp) ? 1 : INSIDE_BLOCK;
	size5 = (udp) ? 1 : VERT_BLOCK;
}

void X11::Set(byte *ptr, uint32_t input) {
	uint32_t skip, len;
	uint8_t number;
	byte *pixs, *orig = (byte *)img->data;

	if (input == 0) {
		len  = *((uint16_t *)ptr);
		ptr += U16S;
	}
	else {
		scrshift = 0;
		len = input;
	}

	while (len != 0) {
		/**
		 *	Linked blocks
		 */
		if ((number = *ptr) == 0) {
			number = ptr[1];
			RET_IF_VOID(ptr[2] > linksnum);
			pixs = ((byte *)&links[ptr[2]]) + 1;
			ptr += LINKED_BLOCK;
			len -= size1;
		}
		/**
		 *	Equal blocks
		 */
		else if (number == 0xFF && ptr[1] == 0x00) {
			memcpy(&skip, ptr + 2, U32S);
			scrshift += skip * 4;
			ptr += EQUAL_BLOCK;
			len -= size2;
			number = 0;
		}
		/**
		 *	Link to previous color
		 */
		else if (number == 0xFF) {
			number = ptr[1];
			ptr += INSIDE_BLOCK;
			len -= size4;
			pixs = orig + scrshift - 4;
		}
		/**
		 *	Color reference above horizontally
		 */
		else if (!Global::args.dvert && number == 0xFE) {
			number = ptr[1];
			ptr += VERT_BLOCK;
			len -= size5;
			pixs = orig + scrshift - width * 4;
		}
		/**
		 *	Compressed blocks
		 */
		else {
			pixs = ptr + 1;
			ptr += COLOR_BLOCK;
			len -= size3;
		}

		BREAK_IF(scrshift >= max);

		for (uint8_t i = 0; i < number; i++) {
			img->data[scrshift]		= pixs[0];
			img->data[scrshift + 1] = pixs[1];
			img->data[scrshift + 2] = pixs[2];
			scrshift += 4;
		}
	}

	if (scrshift >= max || input != 0) {
		XShmPutImage(disp, win, gc, img, 0, 0, 0,
					 0, width, height, false   );
		XSync(disp, false);
		scrshift = 0;
	}
}

uint8_t X11::GetEvents(byte *buff) {
	uint16_t *y = reinterpret_cast<uint16_t *>(buff + U16S);
	uint16_t *x = reinterpret_cast<uint16_t *>(buff);
	unsigned int mask;
	int rx, ry;
	Window r;

	XQueryPointer(disp, win, &r, &r, &rx, &ry, &rx, &ry, &mask);
	*x = (rx < 0) ? 0 : rx;
	*y = (ry < 0) ? 0 : ry;

	uint8_t value, type, size = 0;
	XEvent event;
	buff += U16S * 2;

	while (XPending(disp)) {
		XNextEvent(disp, &event);
		type = 0;

		switch (event.type) {
		/**
		 *	Mouse events
		 */
		case ButtonPress:
			type = 1;
		case ButtonRelease:
			value = event.xbutton.button;
			type = (type != 0) ? type : 0;
			break;
		/**
		 *	Keyboard events
		 */
		case KeyPress:
			type = 3;
		case KeyRelease:
			value = event.xkey.keycode;
			type = (type != 0) ? type : 2;
			break;

		default: continue;
		}
		/**
		 *	If exit key is pressed
		 */
		if (value == EXIT_KEY && type > 1) {
			exit(1);
		}

		buff[size * 2 + 1] = value;
		buff[size * 2] = type;
		size++;

		BREAK_IF(size > 10);
	}

	return size;
}

X11::~X11(void) {
	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}
