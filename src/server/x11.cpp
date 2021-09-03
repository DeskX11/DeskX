
#include "../../include/Server.h"

X11::X11(void) {
	XInitThreads();

	ERROR(!(disp = XOpenDisplay(nullptr)), "Can't open X-Display.");
	root = DefaultRootWindow(disp);
	scr  = XDefaultScreen(disp);
	shm.shmaddr = nullptr;
}

void X11::Start(uint8_t value, bool sdl) {
	XGetWindowAttributes(disp, root, &attrs);
	int depth = DefaultDepth(disp, scr);

	DestroyBuffer();

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

	maxval = Global::args.dvert ? 0xFE : 0xFD;
	comp   = value;
	sdlevs = sdl;
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
		size_t step = i * KEY_BLOCK;
		bool flag = true;

		uint32_t *key = (uint32_t *)&buff[step + 1];

		switch (buff[step]) {
		// mouse events
		case 0:
			flag = false;
		case 1:
			XTestFakeButtonEvent(disp, *key, flag, 0);
			continue;
		// keyboard events
		case 2:
			flag = false;
		case 3:
			if (sdlevs) {
				*key = XKeysymToKeycode(disp, *key);
			}
			XTestFakeKeyEvent(disp, *key, flag, 0);
		// incorrect event
		default: continue;
		}
	}
}

void X11::LinkColor(pix &pixel) {
	auto it = std::lower_bound(links.begin(), links.end(), pixel.u32());

	if (it != links.end() && pixel.u32() == *it) {
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
			 + abs(orig[2] - one.b) <= comp;
	};

	auto inl = [&](void) {
		RET_IF(arr.empty(), false);
		/**
		 *	If the eq flag is on, we are looking for the
		 *	screen segment that was before the current
		 *	block, otherwise we compare it with the color
		 *	of the previous block.
		 */
		one.linkp = (!arr.back().eq) ? arr.back() == one
				  : eq(orig - (one.num+1) * 4, orig - 4);
		return one.linkp;
	};

	auto vrl = [&](void) {
		RET_IF(size + one.num + attrs.width > maxpix
			   || Global::args.dvert, false);
		/**
		 *	Checking the color of the screen segment on
		 *	top of the current block. If the colors are
		 *	the same - refer to this color.
		 */
		byte *p1 = orig - (one.num + attrs.width) * 4;
		byte *p2 = orig - one.num * 4;
		return one.vert = eq(p1, p2);
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
		while (cmp() && one.num < maxval && size > 0) {
			memcpy(next, orig, 3);
			one.num++;
			size--;

			orig += 4;
			next += 4;
			prev += 4;
		}

		if (one.num != 0) {
			/**
			 *	Trying to reference some existing screen
			 *	color.
			 */
			if (firstsend && !vrl() && !inl()) {
				LinkColor(one);
			}
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
 *	of colors must not exceed 256 colors, so that the color
 *	link does not exceed 1 byte.
 */
void X11::MakeLinksTable(void) {
	std::map<uint32_t, size_t>::iterator it1, it2;
	std::map<uint32_t, size_t> mp;
	std::vector<pix> list;
	uint32_t prev, next;

	Vector(list);
	firstsend = false;
	prev = 0;

	for (auto &p : list) {
		it2 = mp.end();

		if ((it1 = mp.find(p.u32())) != it2) {
			it1->second++;
			continue;
		}

		mp.insert(std::make_pair(p.u32(), 1));
	}

	while (mp.size() > 0xFF) {
		it1 = mp.begin();
		it2 = mp.begin();

		for (; it1 != mp.end(); it1++) {
			if (it1->second < it2->second) {
				it2 = it1;
			}
		}

		mp.erase(it2);
	}

	for (auto &g : mp) {
		next = g.first;

		for (auto &p : mp) {
			NEXT_IF(next <= p.first);
			NEXT_IF(prev >= p.first);
			next = p.first;
		}

		links.push_back(prev = next);
	}
}

void X11::SetLinks(byte *ptr, uint8_t size) {
	uint32_t buff;

	for (uint8_t i = 0; i < size; i++) {
		memcpy(&buff, ptr + i * U32S, U32S);
		links.push_back(buff);
	}
}

uint8_t X11::PackLinks(byte *buff) {
	for (auto &p : links) {
		memcpy(buff, &p, U32S);
		buff += U32S;
	}

	return static_cast<uint8_t>(links.size());
}

void X11::DestroyBuffer(void) {
	RET_IF_VOID(!nextb);

	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	firstsend = false;

	delete[] nextb;
	delete[] prevb;
}

X11::~X11(void) {
	DestroyBuffer();
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}
