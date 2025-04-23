
#ifndef DESKX_CODEC_RGB_HPP
#define DESKX_CODEC_RGB_HPP

#include <macro.hpp>

namespace codec {

class rgb {
protected:
	byte r, g, b;
	uint16_t size_ = 1;

public:
	void
	set(const byte *);

	void
	operator++(void);

	void
	operator--(void);

	void
	operator=(const rgb &);

	bool
	operator==(const rgb &) const;

	bool
	operator!=(const rgb &) const;

	bool
	full(void) const;

	const uint16_t &
	size(void) const;

	uint16_t
	rgb14(void) const;

	uint32_t
	rgb24(void) const;

	bool
	eq(const rgb &, const byte) const;

	size_t
	encode(const bool &, byte *) const;

	static size_t
	decode(const size_t &, const size_t &, byte **, byte **);

};

}

#endif