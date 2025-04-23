
#ifndef DESKX_CODEC_AXIS_HPP
#define DESKX_CODEC_AXIS_HPP

#include <macro.hpp>

namespace codec {

class axis {
public:
enum type {
	X, Y
};
	void
	set(const uint16_t &, const axis::type);

	size_t
	encode(byte *) const;

	static size_t
	decode(const size_t &, const size_t &, byte **, byte **);

protected:
	uint16_t val = 0;
	type type_;
};
}

#endif