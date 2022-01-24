
#ifndef _DESKX_CODEC_AXIS_H_
#define _DESKX_CODEC_AXIS_H_

class Codec::Axis {
protected:
	uint16_t val_ = 0;
	Codec::AxisType type_ = X;

public:
	Axis(void) { }
	Axis(const Codec::Axis &);
	uint16_t &value(void);
	Codec::AxisType &type(void);
	uint8_t encode(byte *);
	uint8_t decode(const byte *);

};

#endif