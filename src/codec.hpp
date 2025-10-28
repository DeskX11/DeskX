
#ifndef DESKX_CODEC_HPP
#define DESKX_CODEC_HPP

#include <cstddef>
#include <display.hpp>

namespace codec {

void
init(const size_t &, const size_t &, const byte);

void
skip(const byte, const byte);

void
allocate(void);

void
free(void);

size_t
max(void);

bool
get(display::pixs &, byte *, uint64_t &);

void
set(byte *win, byte *buff, uint64_t &);

}

#endif