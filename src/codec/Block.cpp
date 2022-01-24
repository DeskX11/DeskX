
#include "../../include/DeskX.h"

Codec::Block::Block(const byte *ptr) {
	RETVOID_IF(!ptr);
	rgb_.r = ptr[0];
	rgb_.g = ptr[1];
	rgb_.b = ptr[2];
}

Codec::Block::Block(const Codec::RGB rgb) {
	rgb_ = rgb;
}

Codec::RGB Codec::Block::rgb24(void) {
	return rgb_;
}

Codec::RGB Codec::Block::rgb16(void) {
	Codec::RGB rgb;

	rgb.r = rgb_.r / Consts::rgb5bit;
	rgb.g = rgb_.g / Consts::rgb5bit;
	rgb.b = rgb_.b / Consts::rgb5bit;

	return rgb;
}

Codec::RGB Codec::Block::rgb14(void) {
	Codec::RGB rgb;

	rgb.r = rgb_.r / Consts::rgb4bit;
	rgb.g = rgb_.g / Consts::rgb5bit;
	rgb.b = rgb_.b / Consts::rgb5bit;

	return rgb;
}

Codec::RGB Codec::Block::rgb8(void) {
	Codec::RGB rgb;

	rgb.r = rgb_.r / Consts::rgb2bit;
	rgb.g = rgb_.g / Consts::rgb2bit;
	rgb.b = rgb_.b / Consts::rgb2bit;

	return rgb;
}

bool Codec::Block::equal(Codec::Block block, const uint8_t dist) {
	Codec::RGB c1 = rgb24(), c2 = block.rgb24();

	return abs(c1.r - c2.r)
		 + abs(c1.g - c2.g)
		 + abs(c1.b - c2.b) <= dist;
}

Codec::BlockType &Codec::Block::type(void) {
	return type_;
}

uint8_t &Codec::Block::repeat(void) {
	return repeat_;
}

bool Codec::Block::try8bit(Codec::RGB color) {
	return color.r % Consts::rgb4to2bit == 0 
		&& color.g % Consts::rgb5to2bit == 0 
		&& color.b % Consts::rgb5to2bit == 0;
}

bool Codec::Block::tryBlack(Codec::RGB color) {
	return color.r <= 3
		&& color.g <= 3
		&& color.b <= 3;
}

void Codec::Block::dec8bit(void) {
	rgb_.r *= Consts::rgb2bit;
	rgb_.g *= Consts::rgb2bit;
	rgb_.b *= Consts::rgb2bit;
}

void Codec::Block::decBlack(void) {
	rgb_.r *= Consts::rgb4bit;
	rgb_.g *= Consts::rgb5bit;
	rgb_.b *= Consts::rgb5bit;
}

uint8_t Codec::Block::encode(byte *ptr) {
	RET_IF(!ptr, 0);

	if (type_ == LEFT || type_ == TOP) {
		ptr[0] = type_ == TOP ? 0x00 : 0x40;
		ptr[1] = 0;
		return 2;
	}

	Codec::RGB rgb = rgb14();
	bool b8b2 = tryBlack(rgb);
	bool b8b1 = try8bit (rgb);
	
	// rgb8 bug, try b8b1 = false and look at some video.
	if (repeat_ <= 127 && (b8b2 || b8b1)) {
		rgb = (b8b1) ? rgb8() : rgb;
		ptr[0] = 0x40 | (rgb.r << 4) | (rgb.g << 2) | rgb.b;
		ptr[1] = ((b8b1) ? 0x80 : 0x00) | repeat_;
		return 2;
	}

	uint16_t color = (rgb.r << 10) | (rgb.g << 5) | rgb.b;
	byte *bytes = reinterpret_cast<byte *>(&color);

	ptr[0] = bytes[1];
	ptr[1] = repeat_;
	ptr[2] = bytes[0];

	return Consts::u16 + 1;
}

uint8_t Codec::Block::same(byte *ptr) {
	RET_IF(!ptr, 0);

	ptr[0] = 0x80;
	ptr[1] = repeat_;
	return 2;
}

uint8_t Codec::Block::decode(const byte *ptr, Codec::RGB prev) {
	RET_IF(!ptr, 0);
	type_ = LINE;

	if ((repeat_ = ptr[1]) == 0) {
		type_ = ptr[0] == 0 ? TOP : LEFT;
		repeat_ = 1;
		return 2;
	}

	if ((ptr[0] & 0x40) != 0) {
		rgb_.r = (ptr[0] & 0x30) >> 4;
		rgb_.g = (ptr[0] & 0xC ) >> 2;
		rgb_.b =  ptr[0] & 0x3;
		((ptr[1] & 0x80) != 0) ? dec8bit() : decBlack();
		repeat_ = ptr[1] & 0x7F;
		return 2;
	}

	if (ptr[0] == 0x80) {
		rgb_ = prev;
		repeat_ = ptr[1];
		return 2;
	}

	uint16_t color;
	byte *bytes = reinterpret_cast<byte *>(&color);

	bytes[0] = ptr[2];
	bytes[1] = ptr[0];

	rgb_.r = ((color & 0x3C00) >> 10) * Consts::rgb4bit;
	rgb_.g = ((color & 0x3E0 ) >> 5 ) * Consts::rgb5bit;
	rgb_.b =  (color & 0x1F) * Consts::rgb5bit;

	return Consts::u16 + 1;
}

void Codec::Block::write(byte *orig, const size_t width,
						 uint16_t &x, uint16_t &y) {
	RETVOID_IF(!orig || !width);

	orig += y * width * Consts::rgba + x * Consts::rgba;
	uint64_t step = 0, num;

	if (type_ == LEFT) {
		byte *tmp = orig - Consts::rgba;
		rgb_.r = tmp[0];
		rgb_.g = tmp[1];
		rgb_.b = tmp[2];
	}
	if (type_ == TOP) {
		byte *tmp = orig - width * Consts::rgba;
		rgb_.r = tmp[0];
		rgb_.g = tmp[1];
		rgb_.b = tmp[2];
	}

	for (uint8_t i = 0; i < repeat_; i++) {
		orig[0] = rgb_.r;
		orig[1] = rgb_.g;
		orig[2] = rgb_.b;
		orig += Consts::rgba;
		step++;
	}

	x += step;
	num = x / width;
	y += num;
	x -= num * width;
}
