
#include "../../include/Client.h"

X11::X11(void) {
	XInitThreads();

	width  = Client::args.get().mode.width;
	height = Client::args.get().mode.height;
	disp   = XOpenDisplay(nullptr);
	scr    = XDefaultScreen(disp);
	int depth = DefaultDepth(disp, scr);

	unsigned scr_width  = (unsigned)DisplayWidth (disp, scr);
	unsigned scr_height = (unsigned)DisplayHeight(disp, scr);

	std::stringstream ss;
	ss << "Server screen resolution is greater than client resolution";
	ss << " (" << width << "x" << height << ")";
	ERR(scr_width < width || scr_height < height, ss.str());
	ERR(depth != 24, "The color depth on your computer is not 24 bits");

	bool fullsize = width == scr_width && scr_height == height;
	win = newWindow(width, height, fullsize);
	gc  = XCreateGC(disp, win, 0, nullptr);

	img = XShmCreateImage(disp, DefaultVisual(disp, 0),
						  depth, ZPixmap, nullptr,
						  &shm, width, height);
	assert(img);

	tcpbuff = width * height * 4;

	sharedMem(shm, &img->data, tcpbuff);
	XShmAttach(disp, &shm);
	XSync(disp, false);

	XSelectInput(disp, win, KeyPressMask     | KeyReleaseMask    |
							ButtonPressMask  | Button1MotionMask |
							ButtonReleaseMask					 );
	XFlush(disp);
}

Window X11::newWindow(int width, int height, bool full) {
	int black = BlackPixel(disp, scr);
	int white = WhitePixel(disp, scr);

	Window w = XCreateSimpleWindow(disp, RootWindow(disp, scr), 0, 0,
								   width, height, 1, black, white  );
	XStoreName(disp, w, "DeskX - Remote control");
	if (full) {
		Atom atoms[2] = { XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", false), None };
		XChangeProperty(disp, w, XInternAtom(disp, "_NET_WM_STATE", false),
						XA_ATOM, 32, PropModeReplace, (const unsigned char *)atoms, 1);
	}

    XMapWindow(disp, w);
	return w;
}

void X11::screen(uint64_t size, byte *blocks) {
	byte *orig = (byte *)img->data;
	Codec::Block pix;
	Codec::Axis axis;
	Codec::Type type;
	Codec::RGB  prev;
	uint16_t x = 0, y = 0;
	uint8_t step;

	while (size) {
		type = Codec::is(blocks);
		step = (type == Codec::Type::AXIS) ? axis.decode(blocks)
										   :  pix.decode(blocks, prev);
		blocks += step;
		size   -= step;

		if (type == Codec::Type::BLOCK) {
			pix.write(orig, width, x, y);
			prev = pix.rgb24();
		}
		else {
			uint16_t &tmp = axis.type() == Codec::AxisType::X ? x : y;
			tmp += axis.value();
		}
	}

	XShmPutImage(disp, win, gc, img, 0, 0, 0, 0, width, height, false);
	XSync(disp, false);
}

Events X11::events(void) {
	unsigned int mask;
	XEvent event;
	Events list;
	Window r;
	int x, y;
	bool stop = false;

	XQueryPointer(disp, win, &r, &r, &x, &y, &x, &y, &mask);
	list.getX() = std::max(x, 0);
	list.getY() = std::max(y, 0);

	while (XPending(disp)) {
		XNextEvent(disp, &event);
		bool flag = false;

		switch (event.type) {
		case ButtonPress:
			flag = true;
		case ButtonRelease:
			list.add(event.xbutton.button, flag ? MOUSE_DOWN
											    : MOUSE_UP);
			break;

		case KeyPress:
			flag = true;
		case KeyRelease:
			list.add(event.xkey.keycode, flag ? KEY_DOWN
											  : KEY_UP);
			stop = event.xkey.keycode == _EXIT_KEY;
		default:
			break;
		}

		if (stop) {
			exit(1);
		}
	}

	return list;
}

size_t X11::bufferTCP(void) {
	return tcpbuff;
}

X11::~X11(void) {
	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}
