
#include "../../include/Server.h"

X11::X11(void) {
	XInitThreads();

	ERR(!(disp = XOpenDisplay(nullptr)), "Can't open X-Display.");
	root = DefaultRootWindow(disp);
	scr  = XDefaultScreen(disp);
	shm.shmaddr = nullptr;
	XGetWindowAttributes(disp, root, &attrs);

	maxpix = attrs.width * attrs.height;
}

void X11::start(void) {
	int depth = DefaultDepth(disp, scr);
	assert(depth == 24);
	destroyBuffers();

	img = XShmCreateImage(disp, DefaultVisual(disp, 0), depth, ZPixmap,
						  nullptr, &shm, attrs.width, attrs.height   );
	assert(img);

	sharedMem(shm, &img->data, maxpix * Consts::rgba);
	XShmAttach(disp, &shm);
	XSync(disp, true);
	XTestGrabControl(disp, true);
	/**
	 *	The current frame will be written to one buffer, and the
	 *	other will be compared with the current one. Further,
	 *	the pointers are swapped.
	 */
	size_t buff_size = maxpix * Consts::rgba + Consts::rgba;

	nextb = new byte[buff_size];
	prevb = new byte[buff_size];
	assert(prevb && nextb);
}

size_t X11::bufferTCP(void) {
	return maxpix * 4;
}

void X11::resolution(uint16_t &x, uint16_t &y) {
	x = attrs.width;
	y = attrs.height;
}

void X11::events(Events &list) {
	int x = std::min((int)list.getX(), attrs.width  - 1);
	int y = std::min((int)list.getY(), attrs.height - 1);
	XTestFakeMotionEvent(disp, 0, x, y, 0);

	for (int i = 0; i < list.size(); i++) {
		if (Server::args.get().mode.sdl && list[i].second > MOUSE_DOWN) {
			list[i].first = XKeysymToKeycode(disp, list[i].first);
		}
		(list[i].second > MOUSE_DOWN) ? XTestFakeKeyEvent(disp, list[i].first, list[i].second % 2 != 0, 0)
								   : XTestFakeButtonEvent(disp, list[i].first, list[i].second % 2 != 0, 0);
	}
}

uint64_t X11::screen(byte *buff) {
	XShmGetImage(disp, root, img, 0, 0, AllPlanes);
	byte *orig = (byte *)img->data;
	byte *prev = prevb;
	byte *next = nextb;

	uint64_t &size  = *reinterpret_cast<uint64_t *>(buff);
	uint64_t passed = 0;

	Codec::Block rgb(orig);
	Codec::Axis axis;
	Codec::RGB color;

	uint8_t hdist = Server::args.get().mode.hdistance;
	uint8_t ret;
	uint16_t x, y;

	buff += Consts::u64;
	size = 0;

	auto eq = [](byte *first, byte *second) {
		return memcmp(first, second, 3) == 0;
	};

	auto over = [&](void) {
		return passed >= maxpix;
	};

	auto go_next = [&](void) {
		memcpy(next, orig, 3);

		next += Consts::rgba;
		prev += Consts::rgba;
		orig += Consts::rgba;
		passed++;
	};

	auto toplink = [&](void) {
		byte *top = orig - (attrs.width + rgb.repeat()) * Consts::rgba;
		ret = 0;

		if (rgb.equal(top, hdist)) {
			rgb.type() = Codec::BlockType::TOP;
			ret = rgb.encode(buff);
			color = rgb.rgb14();
			buff += ret;
			size += ret;
		}

		return ret;
	};

	auto leftlink = [&](void) {
		byte *left = orig - 2 * Consts::rgba;
		ret = 0;

		if (rgb.equal(left, hdist)) {
			rgb.type() = Codec::BlockType::LEFT;
			ret = rgb.encode(buff);
			color = rgb.rgb14();
			buff += ret;
			size += ret;
		}

		return ret;
	};

	auto block_fn = [&](void) {
		RET_IF(over());
		rgb = Codec::Block(orig);

		while (rgb.equal(orig, hdist) && rgb.repeat() < 0xFD) {
			go_next();
			rgb.repeat()++;
			BREAK_IF(over());
		}

		if (rgb.repeat() == 1 && passed > 3) {
			RET_IF(leftlink());
		}
		if (rgb.repeat() == 1 && passed > attrs.width) {
			RET_IF(toplink());
		}

		if (rgb.repeat()) {
			rgb.type() = Codec::BlockType::LINE;
			ret = (color != rgb.rgb14()) ? rgb.encode(buff)
										 :   rgb.same(buff);
			color = rgb.rgb14();
			buff += ret;
			size += ret;
		}
	};

	while (!over()) {
		uint64_t step = 0;

		while (!firststart && !over() && eq(orig, prev)) {
			go_next();
			step++;
		}

		if (!over() && step) {
			y = step / attrs.width;
			x = step - y * attrs.width;

			if (y) {
				axis.type() = Codec::AxisType::Y;
				axis.value() = y;

				ret = axis.encode(buff);
				buff += ret;
				size += ret;
			}
			if (x) {
				axis.type() = Codec::AxisType::X;
				axis.value() = x;

				ret = axis.encode(buff);
				buff += ret;
				size += ret;
			}
		}

		block_fn();
	}

	firststart = false;
	orig  = prevb;
	prevb = nextb;
	nextb = orig;

	return size;
}

void X11::destroyBuffers(void) {
	RET_IF(!nextb);

	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);

	delete[] nextb;
	delete[] prevb;
}

X11::~X11(void) {
	destroyBuffers();
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}