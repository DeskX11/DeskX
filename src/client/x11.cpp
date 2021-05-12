
#include "deskx.h"

x11_client::x11_client(headers hdrs) {
	disp = XOpenDisplay(nullptr);
	scr  = XDefaultScreen(disp);

	unsigned owidth  = (unsigned)DisplayWidth (disp, scr);
	unsigned oheight = (unsigned)DisplayHeight(disp, scr);

	assert(hdrs.width <= owidth && hdrs.height <= oheight);
	bool eq = owidth == hdrs.width && hdrs.height == oheight;

	win = new_window(hdrs.width, hdrs.height, eq);
	gc  = XCreateGC(disp, win, 0, nullptr);
	height = hdrs.height;
	width  = hdrs.width;
	maxpix = width * height;

	img = XShmCreateImage(disp, DefaultVisual(disp, 0),
						  DefaultDepth(disp, scr),
						  ZPixmap, nullptr, &shm, width, height);
	assert(img);

	sharedmem_alloc(img->bytes_per_line * img->height);
	XShmAttach(disp, &shm);
	XSync(disp, false);

	XSelectInput(disp, win, KeyPressMask     | KeyReleaseMask    |
							ButtonPressMask  | Button1MotionMask |
							ButtonReleaseMask					 );
	XFlush(disp);
}

Window x11_client::new_window(int width, int height, bool full) {
	int black = BlackPixel(disp, scr);
	int white = WhitePixel(disp, scr);

	Window w = XCreateSimpleWindow(disp, RootWindow(disp, scr), 0, 0,
								   width, height, 1, black, white );
	if (full) {
		fullscreen(w);
	}
    XMapWindow(disp, w);

	return w;
}

void x11_client::fullscreen(Window win) {
  Atom atoms[2] = { XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", false), None };
  XChangeProperty(disp, win, XInternAtom(disp, "_NET_WM_STATE", false),
				  XA_ATOM, 32, PropModeReplace, (const unsigned char *)atoms, 1);
}

void x11_client::sharedmem_alloc(int size) {
	shm.shmid = shmget(IPC_PRIVATE, size * 2, IPC_CREAT | 0777);
	assert(shm.shmid > 0);

	img->data = (char *)shmat(shm.shmid, 0, 0);
	shm.shmaddr = img->data;
	shm.readOnly = false;
	assert(shm.shmaddr != (char *)-1);

	shmctl(shm.shmid, IPC_RMID, 0);
}

void x11_client::set_pixels(byte *ptr, uint32_t size) {
	unsigned int pos = 0, sh;
	uint8_t number;
	uint32_t skip;
	byte *pixs;

	while (size != 0) {
		// Linked block.
		if ((number = *ptr) == 0) {
			number = ptr[1];
			BREAK_IF(ptr[2] > lnum);
			pixs = ((byte *)&links[ptr[2]]) + 1;
			ptr  += 3;
			size -= 3;
		}
		// Same block.
		else if (number == 0xff) {
			memcpy(&skip, ptr + 1, U32S);
			ptr += ESIZE;
			pos += skip;
			number = 0;
			size -= ESIZE;
		}
		// Compressed block.
		else {
			pixs = ptr + 1;
			ptr  += 4;
			size -= 4;
		}

		for (uint8_t i = 0; i < number; i++) {
			sh = pos * 4;
			img->data[sh]	  = pixs[0];
			img->data[sh + 1] = pixs[1];
			img->data[sh + 2] = pixs[2];
			pos++;
		}
	}

	XShmPutImage(disp, win, gc, img, 0, 0, 0, 0,
				 width, height, false         );
}

size_t x11_client::get_events(byte *buff, bool &done) {
	uint8_t id, type, i = 0;
	unsigned int mask;
	XEvent event;
	int rx, ry;
	Window r;

	XQueryPointer(disp, win, &r, &r, &rx, &ry,
				  &rx, &ry, &mask);
	uint16_t x = (rx < 0) ? 0 : rx;
	uint16_t y = (ry < 0) ? 0 : ry;

	memcpy(buff, &x, U16S);
	memcpy(buff + U16S, &y, U16S);

	byte *p = buff + U16S * 2;

	while (XPending(disp)) {
		XNextEvent(disp, &event);
		type = 0;

		switch (event.type) {
		case ButtonPress:
			type = 1;
		case ButtonRelease:
			id = event.xbutton.button;
			type = (type != 0) ? type : 0;
			break;

		case KeyPress:
			type = 3;
		case KeyRelease:
			id = event.xkey.keycode;
			type = (type != 0) ? type : 2;
			break;

		default: continue;
		}

		if (id == EXIT_KEY && type > 1) {
			done = true;
			return 0;
		}

		p[i * 2 + 1] = type;
		p[i * 2 + 2] = id;
		i++;
	}

	*p = i;

	return i;
}

void x11_client::add_links(byte *buff, uint32_t size) {
	uint32_t link = 0;
	size_t shift;

	lnum = size;

	for (uint32_t i = 0; i < lnum; i++) {
		shift = i * U32S;
		memcpy(&link, buff + shift, U32S);
		links.push_back(link);
	}
}

x11_client::~x11_client(void) {
	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}
