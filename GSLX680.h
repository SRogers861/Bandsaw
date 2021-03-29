#ifndef GSLX680_H_
#define GSLX680_H_

#define GSL1680_WAKE 6
#define GSL1680_INT   7

struct fw_data {
	uint8_t offset;
	uint32_t val;
};

struct _ts_event {
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
	uint16_t x3;
	uint16_t y3;
	uint16_t x4;
	uint16_t y4;
	uint16_t x5;
	uint16_t y5;
	uint8_t fingers;
};

#endif /* GSLX680_H_ */
