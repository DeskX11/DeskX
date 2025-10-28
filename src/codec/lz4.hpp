
#ifndef DESKX_CODEC_LZ4_HPP
#define DESKX_CODEC_LZ4_HPP

namespace codec {
namespace lz4 {

uint64_t
compress(byte *, byte *, uint64_t);

uint64_t
decompress(byte *, byte *, uint64_t, int);

}
}

#endif