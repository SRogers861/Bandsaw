/*
 * GSLX680.ino
 *
 *  Created on: Mar 27, 2021
 *      Author: gtaylor
 */
#include "GSLX680.h"

#define GET_FAR_ADDRESS(var) \
({ \
uint_farptr_t tmp; \
\
__asm__ __volatile__( \
\
"ldi %A0, lo8(%1)" "\n\t" \
"ldi %B0, hi8(%1)" "\n\t" \
"ldi %C0, hh8(%1)" "\n\t" \
"clr %D0" "\n\t" \
: \
"=d" (tmp) \
: \
"p" (&(var)) \
); \
tmp; \
})

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

#define GSL1680_WAKE 6
#define GSL1680_INT   7

struct _ts_event ts_event;

extern const struct fw_data GSLX680_FW[] PROGMEM;
extern const uint16_t sizeof_GSLX680_fw;
extern Adafruit_RA8875 tft;

static void GSLX680_I2C_Write(uint8_t regAddr, uint8_t *val, uint16_t cnt);
uint8_t GSLX680_I2C_Read(uint8_t regAddr, uint8_t *pBuf, uint8_t len);
static void _GSLX680_clr_reg(void);
static void _GSLX680_reset_chip(void);
static void _GSLX680_load_fw(void);
static void _GSLX680_startup_chip(void);
static void check_mem_data(void);

//GSLX680_I2C_Write
static void GSLX680_I2C_Write(uint8_t regAddr, uint8_t *val, uint16_t cnt) {
	uint16_t i = 0;

	Wire.beginTransmission(addr);
	Wire.write(regAddr);  // register 0
	for (i = 0; i < cnt; i++, val++)  //data
			{
		Wire.write(*val);  // value
	}
	uint8_t retVal = Wire.endTransmission();
}

//GSLX680_I2C_Read
uint8_t GSLX680_I2C_Read(uint8_t regAddr, uint8_t *pBuf, uint8_t len) {
	Wire.beginTransmission(addr);
	Wire.write(regAddr);  // register 0
	uint8_t retVal = Wire.endTransmission();

	uint8_t returned = Wire.requestFrom(addr, len); // request 1 bytes from slave device #2

	uint8_t i;
	for (i = 0; (i < len) && Wire.available(); i++) {
		pBuf[i] = Wire.read();
	}

	return i;
}

//GSLX680 Clear reg
static void _GSLX680_clr_reg(void) {
	uint8_t regAddr;
	uint8_t Wrbuf[4] = { 0x00 };
	uint8_t len = 1;

	regAddr = 0xe0;
	Wrbuf[0] = 0x88;
	GSLX680_I2C_Write(regAddr, Wrbuf, 1);
	delay(1);
	regAddr = 0x80;
	Wrbuf[0] = 0x03;
	GSLX680_I2C_Write(regAddr, Wrbuf, 1);
	delay(1);
	regAddr = 0xe4;
	Wrbuf[0] = 0x04;
	GSLX680_I2C_Write(regAddr, Wrbuf, 1);
	delay(1);
	regAddr = 0xe0;
	Wrbuf[0] = 0x00;
	GSLX680_I2C_Write(regAddr, Wrbuf, 1);
	delay(1);
}

//_GSLX680 Reset
static void _GSLX680_reset_chip(void) {
	uint8_t regAddr;
	uint8_t Wrbuf[4] = { 0x00 };

	regAddr = 0xe0;
	Wrbuf[0] = 0x88;
	GSLX680_I2C_Write(regAddr, Wrbuf, 1);
	delay(1);

	regAddr = 0xe4;
	Wrbuf[0] = 0x04;
	GSLX680_I2C_Write(regAddr, Wrbuf, 1);
	delay(1);

	regAddr = 0xbc;
	Wrbuf[0] = 0x00;
	Wrbuf[1] = 0x00;
	Wrbuf[2] = 0x00;
	Wrbuf[3] = 0x00;
	GSLX680_I2C_Write(regAddr, Wrbuf, 4);
	delay(1);

}

//GSLX680 Main Down
static void _GSLX680_load_fw(void) {
	uint8_t regAddr;
	uint8_t Wrbuf[4] = { 0x00 };
	uint16_t source_line = 0;
	uint16_t source_len = sizeof_GSLX680_fw / sizeof(struct fw_data);
	Serial.print("Firmware size: ");
	Serial.println(sizeof_GSLX680_fw);
	Serial.print("Line numbers : ");
	Serial.println(source_len);

	for (source_line = 0; source_line < source_len; source_line++) {

		regAddr = pgm_read_byte_far(
				GET_FAR_ADDRESS(GSLX680_FW[0].offset) + source_line * 5);
		Wrbuf[0] = (char) (pgm_read_dword_far(
				GET_FAR_ADDRESS(GSLX680_FW[0].val) + source_line * 5)
				& 0x000000ff);
		Wrbuf[1] = (char) ((pgm_read_dword_far(
				GET_FAR_ADDRESS(GSLX680_FW[0].val) + source_line * 5)
				& 0x0000ff00) >> 8);
		Wrbuf[2] = (char) ((pgm_read_dword_far(
				GET_FAR_ADDRESS(GSLX680_FW[0].val) + source_line * 5)
				& 0x00ff0000) >> 16);
		Wrbuf[3] = (char) ((pgm_read_dword_far(
				GET_FAR_ADDRESS(GSLX680_FW[0].val) + source_line * 5)
				& 0xff000000) >> 24);

		/*	regAddr = pgm_read_byte_near(GET_FAR_ADDRESS(GSLX680_FW[0].offset)+source_line*5);
		 Wrbuf[0] = (char) (pgm_read_dword_near(GET_FAR_ADDRESS(GSLX680_FW[0].val)+source_line*5) & 0x000000ff);
		 Wrbuf[1] = (char) ((pgm_read_dword_near(GET_FAR_ADDRESS(GSLX680_FW[0].val)+source_line*5) & 0x0000ff00) >> 8);
		 Wrbuf[2] = (char) ((pgm_read_dword_near(GET_FAR_ADDRESS(GSLX680_FW[0].val)+source_line*5) & 0x00ff0000) >> 16);
		 Wrbuf[3] = (char) ((pgm_read_dword_near(GET_FAR_ADDRESS(GSLX680_FW[0].val)+source_line*5) & 0xff000000) >> 24);*/

		GSLX680_I2C_Write(regAddr, Wrbuf, 4);

	}

}

//startup chip
static void _GSLX680_startup_chip(void) {

	uint8_t Wrbuf[4] = { 0x00 };

	Wrbuf[0] = 0x00;
	GSLX680_I2C_Write(0xe0, Wrbuf, 1);

}

static void check_mem_data(void) {
	uint8_t write_buf;
	uint8_t read_buf[4] = { 0 };

	delay(30);

	GSLX680_I2C_Read(0xb0, read_buf, 4);

	if ((read_buf[3] != 0x5a) & (read_buf[2] != 0x5a) & (read_buf[1] != 0x5a)
			& (read_buf[0] != 0x5a)) {
		Serial.println("init failure,Reinitialize");
		//Serial.println("#########check mem read 0xb0 = %x %x %x %x #########\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		tft.textSetCursor(0, 16);
		tft.textColor(RA8875_RED, RA8875_BLACK);
		tft.textEnlarge(0);
		tft.textWrite("CTP initialization failure  Reinitialize.");
		digitalWrite(GSL1680_WAKE, LOW);
		delay(20);
		digitalWrite(GSL1680_WAKE, HIGH);
		delay(20);
		//test_i2c();
		_GSLX680_clr_reg();
		_GSLX680_reset_chip();
		_GSLX680_load_fw();
		_GSLX680_startup_chip();
		//_GSLX680_reset_chip();
		//_GSLX680_startup_chip();

		if ((read_buf[3] != 0x5a) & (read_buf[2] != 0x5a)
				& (read_buf[1] != 0x5a) & (read_buf[0] != 0x5a)) {
			tft.textSetCursor(0, 16);
			tft.textColor(RA8875_RED, RA8875_BLACK);
			tft.textEnlarge(0);
			tft.textWrite("CTP initialization failure  Reinitialize.");
			while (1)
				;
		}

	} else {
		tft.textMode();
		tft.textColor(RA8875_RED, RA8875_BLACK);
		tft.textSetCursor(0, 16);
		tft.textEnlarge(0);
		tft.textWrite("CTP initialization OK.");
		Serial.println("init done");
	}
}

//get the most data about capacitive touchpanel.
uint8_t GSLX680_read_data(void) {
	uint8_t touch_data[24] = { 0 };
	uint8_t reg = 0x80;
	GSLX680_I2C_Read(reg, touch_data, 24);

	ts_event.fingers = touch_data[0];

	ts_event.y5 = (uint16_t)(touch_data[23]) << 8 | (uint16_t) touch_data[22];
	ts_event.x5 = (uint16_t)(touch_data[21]) << 8 | (uint16_t) touch_data[20];

	ts_event.y4 = (uint16_t)(touch_data[19]) << 8 | (uint16_t) touch_data[18];
	ts_event.x4 = (uint16_t)(touch_data[17]) << 8 | (uint16_t) touch_data[16];

	ts_event.y3 = (uint16_t)(touch_data[15]) << 8 | (uint16_t) touch_data[14];
	ts_event.x3 = (uint16_t)(touch_data[13]) << 8 | (uint16_t) touch_data[12];

	ts_event.y2 = (uint16_t)(touch_data[11]) << 8 | (uint16_t) touch_data[10];
	ts_event.x2 = (uint16_t)(touch_data[9]) << 8 | (uint16_t) touch_data[8];

	ts_event.y1 = (uint16_t)(touch_data[7]) << 8 | (uint16_t) touch_data[6];
	ts_event.x1 = (uint16_t)(touch_data[5]) << 8 | (uint16_t) touch_data[4];

	return 0;
}

