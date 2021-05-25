
#include "../../include/Server.h"

X11::X11(uint8_t value) {
	XInitThreads();

	disp  = XOpenDisplay(nullptr);
	scr   = XDefaultScreen(disp);
	root  = DefaultRootWindow(disp);
	comp  = value;
	XGetWindowAttributes(disp, root, &attrs);
	int depth = DefaultDepth(disp, scr);

	img = XShmCreateImage(disp, DefaultVisual(disp, 0), depth, ZPixmap,
						  nullptr, &shm, attrs.width, attrs.height   );
	assert(img);

	maxpix = attrs.width * attrs.height;
	Tools::SharedMem(shm, &img->data, maxpix * 4);

	XShmAttach(disp, &shm);
	XSync(disp, true);
	XTestGrabControl(disp, true);
	/**
	 *	The current frame will be written to one buffer, and the
	 *	other will be compared with the current one. Further,
	 *	the pointers are swapped.
	 */
	nextb = new byte[attrs.width * attrs.height * 4];
	prevb = new byte[attrs.width * attrs.height * 4];
	assert(prevb && nextb);
}

void X11::MouseXY(uint16_t x, uint16_t y) {
	XTestFakeMotionEvent(disp, 0, x, y, 0);
}

void X11::GetResolution(uint32_t &width, uint32_t &height) {
	width  = attrs.width;
	height = attrs.height;
}

void X11::NewEvents(byte *buff, uint8_t len) {
	uint8_t number = (len > 4) ? 4 : len;

	for (uint8_t i = 0; i < number; i++) {
		uint16_t shift = i * 2;
		bool flag = true;

		switch (buff[i * 2]) {
		// mouse events
		case 0:
			flag = false;
		case 1:
			XTestFakeButtonEvent(disp, buff[shift + 1], flag, 0);
			continue;
		// keyboard events
		case 2:
			flag = false;
		case 3:
			XTestFakeKeyEvent(disp, buff[shift + 1], flag, 0);
		// incorrect event
		default: continue;
		}	
	}
}

void X11::LinkColor(pix &pixel) {
	std::map<uint32_t, size_t>::iterator it;

	if ((it = links.find(pixel.u32())) != links.end()) {
		pixel.link_id = std::distance(links.begin(), it);
		pixel.link = true;
	}
}

void X11::Vector(std::vector<pix> &arr) {
	uint32_t size = maxpix;
	pix one;

	XShmGetImage(disp, root, img, 0, 0, AllPlanes);
	byte *orig = (byte *)img->data;
	byte *prev = prevb;
	byte *next = nextb;

	auto eq = [](byte *first, byte *second) {
		return memcmp(first, second, 3) == 0;
	};

	auto cmp = [&](void) {
		return abs(orig[0] - one.r)
			 + abs(orig[1] - one.g)
			 + abs(orig[2] - one.b) < comp;
	};
	/**
	 *	Defines the segments of the screen that have not
	 *	changed from the previous frame.
	 */
	auto eqlseg = [&](void) {
		RET_IF_VOID(!firstsend);
		one.eqn = 0;

		while (eq(orig, prev) && size > 0) {
			memcpy(next, orig, 3);

			one.eqn++;
			one.eq = true;
			size--;

			orig += 4;
			next += 4;
			prev += 4;
		}

		if (one.eqn != 0) {
			arr.push_back(one);
		}
	};
	/**
	 *	Defines equal pixels based on a custom parameter.
	 *	If the color was in the hash table, set the flag.
	 */
	auto cmpseg = [&](void) {
		one.set(orig);

		while (cmp() && one.num < 0xFE && size > 0) {
			memcpy(next, orig, 3);
			one.num++;
			size--;

			orig += 4;
			next += 4;
			prev += 4;
		}

		if (one.num != 0) {
			LinkColor(one);
			arr.push_back(one);
		}
	};


	while (size != 0) {
		eqlseg();
		cmpseg();
	}

	orig  = prevb;
	prevb = nextb;
	nextb = orig;
	firstsend = true;
}
/**
 *	Function for creating a hash table of colors. The number
 *	of colors must not exceed 255 colors, so that the color
 *	link does not exceed 1 byte.
 */
uint8_t X11::LinksTable(byte *links_table) {
	std::map<uint32_t, size_t>::iterator it1, it2;
	std::vector<pix> list;

	Vector(list);
	firstsend = false;

	for (auto &p : list) {
		it2 = links.end();

		if ((it1 = links.find(p.u32())) != it2) {
			it1->second++;
			continue;
		}

		links.insert(std::make_pair(p.u32(), 1));
	}

	while (links.size() > 0xFF) {
		it1 = links.begin();
		it2 = links.begin();

		for ( ; it1 != links.end(); it1++) {
			if (it1->second < it2->second) {
				it2 = it1;
			}
		}

		links.erase(it2);
	}

	for (auto &p : links) {
		memcpy(links_table, &p.first, U32S);
		links_table += U32S;
	}

	return static_cast<uint8_t>(links.size());
}

X11::~X11(void) {
	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);

	delete[] nextb;
	delete[] prevb;
}
