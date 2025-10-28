
#include <codec/axis.hpp>

namespace codec {

void
axis::set(const uint16_t &num, const axis::type xy) {
	val = num;
	type_ = xy;
}

size_t
axis::encode(byte *ptr) const {
	if (val <= 0x1F) {
		*ptr = (type_ == type::X ? 0x00 : 0x20) | (val & 0x1F);
		return 1;
	}

	uint16_t &tmp = *reinterpret_cast<uint16_t *>(ptr);
	tmp = 0x4000 | (type_ == type::X ? 0x0000 : 0x2000) | (val & 0x1FFF);
	tmp = htons(tmp);
	return 2;
}

size_t
axis::decode(const size_t &width, const size_t &shift, byte **ptr, byte **scr) {
	uint16_t ret;
	size_t val;
	type xy;

	const byte b = *(*ptr);
	switch (b & 0xE0) {
	case 0x00: xy  = type::X;
			   val = b & 0x1F;
			   ret = 1;
			   break;
	case 0x20: xy = type::Y;
			   val = b & 0x1F;
			   ret = 1;
			   break;
	case 0x40: xy = type::X;
			   ret = ntohs(*reinterpret_cast<uint16_t *>(*ptr));
			   val = ret & 0x1FFF;
			   ret = 2;
			   break;
	case 0x60: xy = type::Y;
			   ret = ntohs(*reinterpret_cast<uint16_t *>(*ptr));
			   val = ret & 0x1FFF;
			   ret = 2;
			   break;
	default:   INFO(WARN"Incorrect Axis block, shift 0");
			   return 0;
	}

	(*ptr) += ret;
	(*scr) += val * shift * (xy == type::X ? 1 : width);
	return ret;
}

}
