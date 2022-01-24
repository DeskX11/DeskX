
#ifndef _DESKX_CODEC_H_
#define _DESKX_CODEC_H_

namespace Codec {
	class Block;
	class  Axis;

	struct RGB {
		byte r = 0, g = 0, b = 0;

		bool operator==(const RGB &color) {
			return r == color.r && g == color.g && b == color.b;
		}
		bool operator!=(const RGB &color) {
			return r != color.r || g != color.g || b != color.b;
		}
	};
	enum BlockType {
		LINE, RECTANGLE, TOP, LEFT
	};
	enum AxisType : byte {
		X = 0x00, Y = 0x20
	};
	enum Type {
		AXIS, BLOCK
	};

	inline Codec::Type is(byte *ptr) {
		return (*ptr & 0x80) != 0 && *ptr != 0x80 ? AXIS : BLOCK;
	}
}

#include "codec/Block.h"
#include "codec/Axis.h"
#endif