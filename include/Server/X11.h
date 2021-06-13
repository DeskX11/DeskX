
#ifndef _DESKX_SERVER_X11_H_
#define _DESKX_SERVER_X11_H_

struct pix {
	uint8_t num = 0, link_id = 0;
	uint32_t eqn;
	byte r, g, b;
	bool link, eq, linkp, vert;

	uint32_t u32(void) {
		uint32_t buff;
		char *ptr = reinterpret_cast<char *>(&buff);
		ptr[0] = 0;
		ptr[1] = r;
		ptr[2] = g;
		ptr[3] = b;
		return buff;
	}

	void set(byte *pixs) {
		linkp = false;
		link  = false;
		vert  = false;
		eq    = false;
		r = pixs[0];
		g = pixs[1];
		b = pixs[2];
		num = 0;
		eqn = 0;
	}

	uint64_t get(void) {
		uint64_t buff;
		byte *ptr = reinterpret_cast<byte *>(&buff);

		if (eq) {
			ptr[0] = 0xFF;
			ptr[1] = 0x00;
			memcpy(ptr + 2, &eqn, U32S);
		}
		else if (vert) {
			ptr[0] = 0xFE;
			ptr[1] = num;
		}
		else if (linkp) {
			ptr[0] = 0xFF;
			ptr[1] = num;
		}
		else if (link) {
			ptr[0] = 0;
			ptr[1] = num;
			ptr[2] = link_id;
		}
		else {
			ptr[0] = num;
			ptr[1] = r;
			ptr[2] = g;
			ptr[3] = b;
		}

		return buff;
	}

	bool operator==(pix &block) {
		return r == block.r
			&& g == block.g
			&& b == block.b;
	}

	uint8_t size(void) {
		return (eq) ? EQUAL_BLOCK
					: ((link) ? LINKED_BLOCK
							  : ((linkp) ? INSIDE_BLOCK
										 : ((vert) ? VERT_BLOCK
												   : COLOR_BLOCK)));
	}
};

class X11 {
private:
	bool firstsend = false;
	std::map<uint32_t, size_t> links;
	Display *disp = nullptr;
	XWindowAttributes attrs;
	XShmSegmentInfo shm;
	byte *prevb, *nextb, maxval;
	size_t maxpix;
	uint8_t comp;
	Window root;
	XImage *img;
	int scr;

	void LinkColor(pix &);

public:
	void GetResolution(uint32_t &, uint32_t &);
	void NewEvents(byte *, uint8_t);
	void MouseXY(uint16_t, uint16_t);
	void Vector(std::vector<pix> &);
	uint8_t LinksTable(byte *);
	size_t Size(void) { return maxpix * COLOR_BLOCK; }
	X11(uint8_t);
	~X11(void);
};

#endif