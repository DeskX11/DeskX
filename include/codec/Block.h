
#ifndef _DESKX_CODEC_BLOCK_H_
#define _DESKX_CODEC_BLOCK_H_

class Codec::Block {
protected:
	Codec::RGB rgb_;
	Codec::BlockType type_ = LINE;
	uint8_t repeat_ = 0;

	bool try8bit(Codec::RGB);
	bool tryBlack(Codec::RGB);
	void dec8bit(void);
	void decBlack(void);

public:
	Block(void) { }
	Block(const byte *);
	Block(const Codec::RGB);
	Codec::RGB rgb24(void);
	Codec::RGB rgb16(void);
	Codec::RGB rgb14(void);
	Codec::RGB rgb8(void);
	bool equal(Codec::Block, const uint8_t);
	Codec::BlockType &type(void);
	uint8_t &repeat(void);
	uint64_t write(byte *, size_t);
	uint8_t decode(const byte *, Codec::RGB);
	uint8_t encode(byte *);
	uint8_t same(byte *);
	void write(byte *, const size_t, uint16_t &, uint16_t &);
};

#endif