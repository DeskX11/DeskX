
#include <string.h>
#include <codec.hpp>
#include <codec/lz4.hpp>
#include <codec/rgb.hpp>
#include <codec/axis.hpp>

#define LINE byte{1}
#define AXIS byte{0}

namespace codec {
namespace {

size_t width, height, pixnum, xmax, framemax;
byte delta, skipx, skipy;
byte *prev = nullptr;
byte *next = nullptr;
byte *lz4m = nullptr;
bool start;

byte
is(const byte s) {
	return (s & 0x80) == 0x80 ? LINE : AXIS;
}

}

void
init(const size_t &x, const size_t &y, const byte num) {
	DIE(x > SCR_X_MAX || y > SCR_Y_MAX);
	pixnum = x * y;
	framemax = pixnum * 3;
	DIE(!framemax);

	start  = true;
	delta  = num;
	width  = x;
	height = y;
	xmax   = x;
	lz4m   = new byte[framemax];
	DIE(!lz4m);
}

void
skip(const byte x, const byte y) {
	xmax = width / (x + 1);
	skipx = x;
	skipy = y;
}

void
allocate(void) {
	prev = new byte[framemax];
	next = new byte[framemax];
	DIE(!prev || !next);
}

void
free(void) {
	RET_IF(!prev);
	delete[] prev;
	delete[] next;
	prev = nullptr;
	next = nullptr;
}

size_t
max(void) {
	return pixnum * 4;
}

bool
get(display::pixs &pixs, byte *msg, uint64_t &size) {
	RET_IF(!pixs.ptr, false);

	size_t shift, skip = 0, x = 0;
	bool flag = false;
	rgb color, tmp;
	axis xy;
	size = 0;

	color.set(pixs.ptr);
	byte *pbuff = prev;
	byte *nbuff = next;
	byte *buff  = lz4m;

	auto step = [&pixs, &pbuff, &nbuff, &x](size_t &num) {
		for (byte i = 0; i <= skipx; i++) {
			pixs.next();
			num++;
			x++;
		}

		pbuff += 3;
		nbuff += 3;
		RET_IF(x < width);

		x = 0;
		for (byte i = 0; i < skipy; i++) {
			pixs.next(width);
			num += width;
		}
	};

	step(shift);
	for (size_t i = 1 + skipx; i < pixnum; step(i)) {
		::memcpy(nbuff, pixs.ptr, 3);
		if (::memcmp(pbuff, pixs.ptr, 3) == 0 && !start) {
			skip++;
			continue;
		}
		if (skip) {
			shift = color.encode(skipy ? false : flag, buff);
			size += shift;
			buff += shift;
			flag  = false;

			shift = skip  / xmax;
			skip -= shift * xmax;
			if (shift) {
				xy.set(shift, axis::type::Y);
				shift = xy.encode(buff);
				size += shift;
				buff += shift;
			}
			if (skip) {
				xy.set(skip, axis::type::X);
				shift = xy.encode(buff);
				size += shift;
				buff += shift;
				skip = 0;
			}

			color.set(pixs.ptr);
			continue;
		}

		tmp.set(pixs.ptr);
		if (color.full()) {
			shift = color.encode(false, buff);
			size += shift;
			buff += shift;
			color = tmp;
			continue;
		}

		if (color.eq(tmp, delta)) {
			++color;
			continue;
		}
		else {
			shift = color.encode(skipy ? false : flag, buff);
			size += shift;
			buff += shift;
			color = tmp;
			flag  = false;
			NEXT_IF(i <= width);
			tmp.set(pixs.ptr - width * pixs.shift);
			flag = color == tmp;
		}
	}
	if (color.size() > 1) {
		shift = color.encode(skipy ? false : flag, buff);
		size += shift;
	}

	start = false;
	std::swap(prev, next);
	RET_IF(!size, false);

	size = lz4::compress(msg + 8, lz4m, size);
	const uint64_t num = htonll(size);
	::memcpy(msg, &num, 8);
	size += 8;
	return true;
}

void
set(byte *win, byte *msg, uint64_t &size) {
	size = lz4::decompress(lz4m, msg, size, framemax);
	DIE(!size);

	byte *buff = lz4m;
	size_t tmp;
	for (uint64_t num = 0; num < size;) {
		if (is(*buff) == AXIS) {
			tmp = axis::decode(width, 4, &buff, &win);
			num += tmp;
			NEXT_IF(tmp);
			INFO(WARN"Shift is broken, skip frame");
			return;
		}

		tmp = rgb::decode(width, 4, &buff, &win);
		num += tmp;
		if (!tmp) {
			INFO(WARN"Line is broken, skip frame");
			return;
		}
	}
}

}