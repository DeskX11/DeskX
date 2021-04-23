
#include "localdesk.h"

x11_client::x11_client(headers hdrs, bool full_mod) {
	disp = XOpenDisplay(nullptr);
	scr  = XDefaultScreen(disp);

	assert(hdrs.width  <= (unsigned)DisplayWidth (disp, scr)
		&& hdrs.height <= (unsigned)DisplayHeight(disp, scr));

	win = new_window(hdrs.width, hdrs.height);
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

Window x11_client::new_window(int width, int height) {
	int black = BlackPixel(disp, scr);
	int white = WhitePixel(disp, scr);

	Window w = XCreateSimpleWindow(disp, RootWindow(disp, scr), 0, 0,
								   width, height, 1, black, white  );
	XMapWindow(disp, w);

	return w;
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

void x11_client::set_pixels(byte *ptr, uint32_t blocks) {
	unsigned int shift, pos = 0, sh;
	uint16_t num;

	for (unsigned int i = 0; i < blocks; i++) {
		shift = i * BSIZE + U16S;

		memcpy(&num, ptr + shift - U16S, U16S);
		BREAK_IF(pos + num > maxpix);
	
		for (uint16_t j = 0; j < num; j++) {
			sh = pos * 4;
			img->data[sh + 0] = (ptr + shift)[0];
			img->data[sh + 1] = (ptr + shift)[1];
			img->data[sh + 2] = (ptr + shift)[2];
			pos++;
		}
	}

	XShmPutImage(disp, win, gc, img, 0, 0, 0, 0,
				 width, height, false         );
}

size_t x11_client::get_events(byte *buff) {
	uint8_t id, type, i = 0;
	unsigned int mask;
	XEvent event;
	int rx, ry;
	Window r;

	XQueryPointer(disp, win, &r, &r, &rx,
				  &ry, &rx, &ry, &mask );
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

		p[i * 2 + 1] = type;
		p[i * 2 + 2] = id;
		i++;
	}

	*p = i;
	return i;
}

x11_client::~x11_client(void) {
	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}