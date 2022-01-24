
#include "../../include/DeskX.h"

Codec::Axis::Axis(const Codec::Axis &obj) {
	val_ = obj.val_;
	type_ = obj.type_;
}

uint16_t &Codec::Axis::value(void) {
	return val_;
}

Codec::AxisType &Codec::Axis::type(void) {
	return type_;
}

uint8_t Codec::Axis::encode(byte *ptr) {
	RET_IF(!ptr, 0);

	val_ = std::min(val_, (uint16_t)0x1FFF);
	ptr[0] = 0x80;
	// 1 - flag
	// 1 - 2 bytes length
	// 0 - x or y(1)
	if (val_ <= 31) {
		ptr[0] |= type_ | (val_ & 0x1F);
		return 1;
	}

	byte *bytes = reinterpret_cast<byte *>(&val_);
	ptr[0] |= 0x40 | type_ | bytes[1];
	ptr[1] = bytes[0];
	return 2;
}

uint8_t Codec::Axis::decode(const byte *ptr) {
	RET_IF(!ptr, 0);

	type_ = (ptr[0] & Y) != 0 ? Y : X;

	if ((ptr[0] & 0x40) == 0) {
		val_ = ptr[0] & 0x1F;
		return 1;
	}

	byte *bytes = reinterpret_cast<byte *>(&val_);
	bytes[1] = ptr[0] & 0x1F;
	bytes[0] = ptr[1];
	return 2;
}