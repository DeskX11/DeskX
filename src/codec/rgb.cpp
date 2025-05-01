
#include <cmath>
#include <netinet/in.h>
#include <codec/rgb.hpp>
#include <codec/palette.hpp>

namespace codec {
namespace {

namespace lims {

constexpr size_t big	= 32767;
constexpr size_t middle	= 127;
constexpr size_t small	= 31;
constexpr size_t top	= 31;

}

namespace enc {

uint16_t
big(const uint16_t &color, const uint16_t &size, byte *ptr) {
	uint32_t &val = *reinterpret_cast<uint32_t *>(ptr);
	val = 0xE0000000 | ((size & 0x7FFF) << 14) | color;
	val = htonl(val);
	return 4;
}

uint16_t
middle(const uint16_t &color, const uint16_t &size, byte *ptr) {
	uint32_t &val = *reinterpret_cast<uint32_t *>(ptr);
	val = 0xC0000000 | ((size & 0x7F) << 22) | (color << 8);
	val = htonl(val);
	return 3;
}

uint16_t
small(const byte color, const uint16_t &size, byte *ptr) {
	uint16_t &val = *reinterpret_cast<uint16_t *>(ptr);
	val = 0xA000 | ((size & 0x1F) << 8) | color;
	val = htons(val);
	return 2;
}

uint16_t
top(const uint16_t &size, byte *ptr) {
	*ptr = 0x80 | (size & 0x1F);
	return 1;
}

}

namespace dec {

uint16_t
big(const byte *ptr, uint32_t &color, uint16_t &size) {
	const uint32_t val = ntohl(*reinterpret_cast<const uint32_t *>(ptr));
	size = (val & 0x1FFFC000) >> 14;
	byte &r = *reinterpret_cast<byte *>(&color),
		 &g = *(&r + 1),
		 &b = *(&r + 2);
	r = ((val & 0x3C00) >> 10) * 17;
	g = ((val & 0x3E0)  >>  5) *  8;
	b =  (val & 0x1F)		   *  8;
	return 4;
}

uint16_t
middle(const byte *ptr, uint32_t &color, uint16_t &size) {
	const uint32_t val = ntohl(*reinterpret_cast<const uint32_t *>(ptr));
	size = (val & 0x1FC00000) >> 22;
	byte &r = *reinterpret_cast<byte *>(&color),
		 &g = *(&r + 1),
		 &b = *(&r + 2);
	r = ((val & 0x3C0000) >> 18) * 17;
	g = ((val & 0x3E000)  >> 13) *  8;
	b = ((val & 0x1F00)   >>  8) *  8;
	return 3;
}

uint16_t
small(const byte *ptr, uint32_t &color, uint16_t &size) {
	const uint16_t val = ntohs(*reinterpret_cast<const uint16_t *>(ptr));
	auto it = p8to24.find(val & 0xFF);
	color = it == p8to24.end() ? 0 : it->second;
	size  = (val & 0x1F00) >> 8;
	return 2;
}

uint16_t
top(const byte *ptr, uint32_t &color, uint16_t &size, const byte *scr) {
	const uint32_t &from = *reinterpret_cast<const uint32_t *>(scr);
	size  = *ptr & 0x1F;
	color = from;
	return 1;
}

}

}

void
rgb::set(const byte *ptr) {
	r = ptr[0];
	g = ptr[1];
	b = ptr[2];
	size_ = 1;
}

void
rgb::operator++(void) {
	RET_IF(full());
	size_++;
}

void
rgb::operator--(void) {
	RET_IF(size_ == 0x01);
	size_--;
}

void
rgb::operator=(const rgb &arg) {
	r = arg.r;
	g = arg.g;
	b = arg.b;
	size_ = arg.size_;
}

bool
rgb::operator==(const rgb &arg) const {
	return r == arg.r && g == arg.g &&
		   b == arg.b && size_ == arg.size_;
}

bool
rgb::operator!=(const rgb &arg) const {
	return !operator==(arg);
}

bool
rgb::full(void) const {
	return size_ >= lims::big;
}

const uint16_t &
rgb::size(void) const {
	return size_;
}

uint32_t
rgb::rgb24(void) const {
	uint32_t ret = 0;
	ret = (r << 16) | (g << 8) | b;
	return ret;
}

uint16_t
rgb::rgb14(void) const {
	uint16_t ret = 0;
	byte r4 = r / 17;
	byte g5 = g /  8;
	byte b5 = b /  8;
	ret = (r4 << 10) | (g5 << 5) | b5;
	return ret;
}

bool
rgb::eq(const rgb &arg, const byte delta) const {
	return std::abs(arg.r - r) + std::abs(arg.g - g) +
		   std::abs(arg.b - b) <= delta || rgb14() == arg.rgb14();
}

size_t
rgb::encode(const bool &flag, byte *buff) const {
	const uint32_t num = size_ - 1;
	RET_IF(num > lims::middle,   enc::big(rgb14(), num, buff));
	RET_IF(flag && num <= lims::top,      enc::top(num, buff));
	RET_IF(num > lims::small, enc::middle(rgb14(), num, buff));

	auto it = p24to8.find(rgb24());
	return it != p24to8.end() ? enc::small(it->second, num, buff)
							  : enc::middle  (rgb14(), num, buff);
}

size_t
rgb::decode(const size_t &width, const size_t &shift, byte **ptr, byte **scr) {
	uint16_t repeat, num;
	uint32_t color;

	const byte b = *(*ptr);
	switch (b & 0xE0) {
	case 0xE0: num = dec::big(*ptr, color, repeat);
			   break;
	case 0xC0: num = dec::middle(*ptr, color, repeat);
			   break;
	case 0xA0: num = dec::small(*ptr, color, repeat);
			   break;
	case 0x80: num = dec::top(*ptr, color, repeat, *scr - width * shift);
			   break;
	default:   INFO(WARN"Incorrect RGB block, shift 0");
			   return 0;
	}

	const byte *bytes = reinterpret_cast<byte *>(&color);
	(*ptr) += num;
	for (size_t i = 0; i <= repeat; i++, (*scr) += shift) {
		(*scr)[0] = bytes[0];
		(*scr)[1] = bytes[1];
		(*scr)[2] = bytes[2];
	}

	return num;
}

}
