
#include <cmath>
#include <string.h>
#include <lz4.h>
#include <macro.hpp>
#include <codec/lz4.hpp>

namespace codec {
namespace lz4 {

uint64_t
compress(byte *buff, byte *src, uint64_t size) {
	size_t iters = 1;
	if (size > LZ4_MAX_INPUT_SIZE) {
		iters = std::ceil(size / double{LZ4_MAX_INPUT_SIZE});
	}
	
	uint64_t ret = 0;
	uint32_t tmp;
	int len, partlen;

	char *to =   reinterpret_cast<char *>(buff);
	char *from = reinterpret_cast<char *>(src);

	for (size_t i = 0; i < iters; i++) {
		len = ::LZ4_compressBound(size);
		DIE(!len);

		partlen = size > LZ4_MAX_INPUT_SIZE ? LZ4_MAX_INPUT_SIZE : size;
		size -= partlen;

		len = ::LZ4_compress_default(from, to + 4, partlen, len);
		DIE(len <= 0);
		tmp = len;
		::memcpy(to, &tmp, 4);

		from += partlen;
		to  += len + 4;
		ret += len + 4;
	}

	return ret;
}

uint64_t
decompress(byte *buff, byte *src, uint64_t size, int limit) {
	uint64_t ret = 0;
	uint32_t part;
	int len;

	char *to =   reinterpret_cast<char *>(buff);
	char *from = reinterpret_cast<char *>(src);

	while (size) {
		::memcpy(&part, from, 4);
		DIE(!part);

		size -= part + 4;
		from += 4;
		len = ::LZ4_decompress_safe(from, to, part, limit);
		DIE(!len);
		to  += len;
		ret += len;
		limit -= len;
		from  += part + 4;
	}

	return ret;
}

}
}