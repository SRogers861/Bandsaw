/***************************************************
 //Web: http://www.buydisplay.com
 EastRising Technology Co.,LTD
 Examples for ER-TFTM050A2-3 with Capacitive Touch Panel
 Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply
 Capacitive Touch Panel is I2C Interface
 Tested and worked with:
 Works with Arduino 1.6.0 IDE
 ****************************************************/
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>
#include <AccelStepper.h>
#include "GSLX680.h"
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

uint8_t addr = 0x40;
int menuPage;
int zeroHeight = 0;
int zeroLength = 0;
float manHeight = 0;  //Manual starting Height Variable
float manThick = 0;
float manLength = 0;
float autoHeight = 0;
float autoThick = 0;
float autoLength = 0;
float autoQuantity = 0;
float kerfThick = 0;
float feedSpeed = 0;
float pauseLength = 0;

//LCD:hardware SPI  CTP:hardware IIC
//Arduino mega2560 +ER-AS8875+ER-TFTM050A2-3
#define RA8875_INT 4
#define RA8875_CS 10 
#define RA8875_RESET 9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

#define dirPin 24
#define stepPin 26
#define motorInterfaceType 1
#define zeroPin 28

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

extern struct _ts_event ts_event;

void setup() {
	stepper.setMaxSpeed(5000);  //sets max speed of stepper in pulses per second

	pinMode(dirPin, OUTPUT);  // direction pin for stepper is set for output
	pinMode(stepPin, OUTPUT); // step/pulse pin for steper is set for output
	pinMode(zeroPin, INPUT_PULLUP); // Zero pin is set for pullup to be grounded when zeroed
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);

	Serial.begin(9600);
	Serial.println("RA8875 start");
	Wire.begin();        // join i2c bus (address optional for master)

	while (!tft.begin(RA8875_800x480)) {
		Serial.println("RA8875 Not Found!");
		delay(100);
	}
	Serial.println("Found RA8875");

	tft.displayOn(true);
	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	tft.PWM1out(255);

	tft.fillScreen(RA8875_BLACK);
	tft.textMode();
	tft.textSetCursor(0, 0);
	tft.textColor(RA8875_RED, RA8875_BLACK);
	tft.textEnlarge(0);
	tft.textWrite(
			"High Gravity Millworks Woodland Mills automation booting...");

	GSLX680_setup();

	menuPage = 0;
	drawBegin();
}

void loop() {
	static uint16_t total = 256;
	static uint16_t w = tft.width();
	static uint16_t h = tft.height();

	float xScale = 1024.0F / w;
	float yScale = 1024.0F / h;

	if (digitalRead(GSL1680_INT) == LOW) {
		return;
	}
	if (digitalRead(GSL1680_INT) == HIGH) {
		GSLX680_read_data();
	}

	int x = ts_event.x1;
	int y = ts_event.y1 - 4096;

	switch (menuPage) {
	case 0: //Begin Page
		handleMenuPage0(x, y);
		break;
	case 1: //Zero Page
		handleMenuPage1(x, y);
		break;
	case 2: //Main Page
		handleMenuPage2(x, y);
		break;
	case 3: //Manual Menu
		handleMenuPage3(x, y);
		break;
	case 4: //Automatic Menu
		handleMenuPage4(x, y);
		break;
	case 5: //Settings Menu
		handleMenuPage5(x, y);
		break;
	case 6: //Manual Starting Height Keypad
		handleMenuPage6(x, y);
		break;
	case 7: //Manual Board Thickness Keypad
		handleMenuPage7(x, y);
		break;
	case 8: //Manual Log Length Keypad
		handleMenuPage8(x, y);
		break;
	case 9: //Manual Go Menu
		handleMenuPage9(x, y);
		break;
	case 10: //Auto Starting Height Keypad
		handleMenuPage10(x, y);
		break;
	case 11: //Auto Board Thickness Keypad
		handleMenuPage11(x, y);
		break;
	case 12: //Auto Log Length Keypad
		handleMenuPage12(x, y);
		break;
	case 13: //Auto Quantity Keypad
		handleMenuPage13(x, y);
		break;
	case 14: //Auto Go Menu
		handleMenuPage14(x, y);
		break;
	case 15: //Kerf Thickness Keypad
		handleMenuPage15(x, y);
		break;
	case 16: //Forward Speed Keypad
		handleMenuPage16(x, y);
		break;
	case 17: //Automatic Settings Menu
		handleMenuPage17(x, y);
		break;
	case 18: //Length of Pause Keypad
		handleMenuPage18(x, y);
		break;
	}
}

void handleMenuPage0(int x, int y) {
	if (x > 268 && x < 534 && y > 161 && y < 320) {
		Serial.println("Begin Pressed");
		tft.fillRect(268, 161, 266, 159, RA8875_GREEN);
		tft.drawRect(268, 161, 266, 159, RA8875_WHITE);
		tft.textSetCursor(320, 205);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(7);
		tft.print("Begin");
		delay(100);
		menuPage = 1;
		drawZero();
		delay(100);
	}
}

void handleMenuPage1(int x, int y) {
	if (x > 268 && x < 534 && y > 76 && y < 215) {
		Serial.println("Zero Height Pressed");
		tft.fillRect(268, 76, 266, 139, RA8875_GREEN);
		tft.drawRect(268, 76, 266, 139, RA8875_WHITE);
		tft.textSetCursor(310, 128);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Zero Height");
		delay(100);
		zeroHeight = 1;
	}
	if (x > 268 && x < 534 && y > 265 && y < 404) {
		Serial.println("Zero Length Pressed");
		tft.fillRect(268, 265, 266, 139, RA8875_GREEN);
		tft.drawRect(268, 265, 266, 139, RA8875_WHITE);
		tft.textSetCursor(310, 318);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Zero Length");
		delay(100);
		zeroLength = 1;
	}
	if (zeroHeight == 1 && zeroLength == 1) {
		menuPage = 2;
		drawMain();
		delay(100);
	}
}

void handleMenuPage2(int x, int y) {
	if (x > 0 && x < 267 && y > 0 && y < 160) {
		Serial.println("Manual Mode Pressed");
		tft.fillRect(0, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(42, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Manual Mode");
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 160 && y < 320) {
		Serial.println("Automatic Mode Pressed");
		tft.fillRect(0, 160, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
		tft.textSetCursor(20, 225);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Automatic Mode");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 320 && y < 480) {
		Serial.println("Settings Pressed");
		tft.fillRect(0, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(70, 385);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Settings");
		delay(100);
		menuPage = 5;
		drawSettingsMenu();
		delay(100);
	}
}

void handleMenuPage3(int x, int y) {
	if (x > 0 && x < 267 && y > 0 && y < 160) {
		Serial.println("Starting Height Pressed");
		tft.fillRect(0, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(15, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Starting Height");
		delay(100);
		menuPage = 6;
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 160 && y < 320) {
		Serial.println("Board Thickness Pressed");
		tft.fillRect(0, 160, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
		tft.textSetCursor(15, 225);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Board Thickness");
		delay(100);
		menuPage = 7;
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 320 && y < 480) {
		Serial.println("Log Length Pressed");
		tft.fillRect(0, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(50, 385);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Log Length");
		delay(100);
		menuPage = 8;
		drawFeetKeypad();
		delay(100);
	}
	if (x > 544 && x < 800 && y > 0 && y < 160) {
		Serial.println("Back Pressed");
		tft.fillRect(533, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(638, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 2;
		drawMain();
		delay(100);
	}
	if (x > 533 && x < 800 && y > 320 && y < 480 && manHeight > 0
			&& manThick > 0 && manLength > 0) {
		Serial.println("Manual Go Pressed");
		tft.fillRect(533, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(655, 385);
		tft.textTransparent(RA8875_BLACK);
		tft.textEnlarge(1);
		tft.print("Go");
		delay(100);
		menuPage = 9;
		drawManualGoMenu();
		delay(100);
	}
}

void handleMenuPage4(int x, int y) {
	if (x > 0 && x < 267 && y > 0 && y < 160) {
		Serial.println("Starting Height Pressed");
		tft.fillRect(0, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(15, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Starting Height");
		delay(100);
		menuPage = 10;
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 160 && y < 320) {
		Serial.println("Board Thickness Pressed");
		tft.fillRect(0, 160, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
		tft.textSetCursor(15, 225);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Board Thickness");
		delay(100);
		menuPage = 11;
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 320 && y < 480) {
		Serial.println("Log Length Pressed");
		tft.fillRect(0, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(50, 385);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Log Length");
		delay(100);
		menuPage = 12;
		drawFeetKeypad();
		delay(100);
	}
	if (x > 267 && x < 533 && y > 0 && y < 160) {
		Serial.println("Quantity Pressed");
		tft.fillRect(267, 0, 266, 160, RA8875_GREEN);
		tft.drawRect(267, 0, 266, 160, RA8875_WHITE);
		tft.textSetCursor(335, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Quantity");
		delay(100);
		menuPage = 13;
		drawBoardsKeypad();
		delay(100);
	}
	if (x > 544 && x < 800 && y > 0 && y < 160) {
		Serial.println("Back Pressed");
		tft.fillRect(533, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(638, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 2;
		drawMain();
		delay(100);
	}
	if (x > 533 && x < 800 && y > 320 && y < 480 && autoHeight > 0
			&& autoThick > 0 && autoLength > 0 && autoQuantity > 0) {
		Serial.println("Automatic Go Pressed");
		tft.fillRect(533, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(655, 385);
		tft.textTransparent(RA8875_BLACK);
		tft.textEnlarge(1);
		tft.print("Go");
		delay(100);
		menuPage = 14;
		drawAutoGoMenu();
		delay(100);
	}
}

void handleMenuPage5(int x, int y) {
	if (x > 0 && x < 267 && y > 0 && y < 160) {
		Serial.println("Kerf Thickness Pressed");
		tft.fillRect(0, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(20, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Kerf Thickness");
		delay(100);
		menuPage = 15;
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 267 && y > 160 && y < 320) {
		Serial.println("Forward Speed Pressed");
		tft.fillRect(0, 160, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
		tft.textSetCursor(25, 225);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Forward Speed");
		delay(100);
		menuPage = 16;
		drawIPSKeypad();
		delay(100);
	}
	if (x > 533 && x < 800 && y > 0 && y < 160) {
		Serial.println("Auto Settings Pressed");
		tft.fillRect(533, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(560, 65);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Auto Settings");
		delay(100);
		menuPage = 17;
		drawAutoSettingsMenu();
		delay(100);
	}
	if (x > 533 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(533, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(638, 385);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 2;
		drawMain();
		delay(100);
	}
}

void handleMenuPage6(int x, int y) {
	static String manHeightString = "";

	if (x > 0 && x < 200 && y > 0 && y < 120) {
		Serial.println("1 Pressed");
		manHeightString.concat('1');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 200 && x < 400 && y > 0 && y < 120) {
		Serial.println("2 Pressed");
		manHeightString.concat('2');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 400 && x < 600 && y > 0 && y < 120) {
		Serial.println("3 Pressed");
		manHeightString.concat('3');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 200 && y > 120 && y < 240) {
		Serial.println("4 Pressed");
		manHeightString.concat('4');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 200 && x < 400 && y > 120 && y < 240) {
		Serial.println("5 Pressed");
		manHeightString.concat('5');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 400 && x < 600 && y > 120 && y < 240) {
		Serial.println("6 Pressed");
		manHeightString.concat('6');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 200 && y > 240 && y < 360) {
		Serial.println("7 Pressed");
		manHeightString.concat('7');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 200 && x < 400 && y > 240 && y < 360) {
		Serial.println("8 Pressed");
		manHeightString.concat('8');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 400 && x < 600 && y > 240 && y < 360) {
		Serial.println("9 Pressed");
		manHeightString.concat('9');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 0 && x < 200 && y > 320 && y < 480) {
		Serial.println(". Pressed");
		manHeightString.concat('.');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}
	if (x > 200 && x < 400 && y > 320 && y < 480) {
		Serial.println("0 Pressed");
		manHeightString.concat('0');
		delay(100);
		drawInchesKeypad();
		delay(100);
	}

	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		manHeight = manHeightString.toFloat();
		Serial.println(manHeight);
		manHeightString = ""; // reset string
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		manHeightString = ""; // reset string
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
}

void handleMenuPage7(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
}

void handleMenuPage8(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 3;
		drawManualMenu();
		delay(100);
	}
}

void handleMenuPage9(int x, int y) {

}

void handleMenuPage10(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
}

void handleMenuPage11(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
}

void handleMenuPage12(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
}

void handleMenuPage13(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 4;
		drawAutomaticMenu();
		delay(100);
	}
}

void handleMenuPage14(int x, int y) {

}

void handleMenuPage15(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 5;
		drawSettingsMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 5;
		drawSettingsMenu();
		delay(100);
	}
}

void handleMenuPage16(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 5;
		drawSettingsMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 5;
		drawSettingsMenu();
		delay(100);
	}
}

void handleMenuPage17(int x, int y) {
	if (x > 0 && x < 267 && y > 0 && y < 160) {
		Serial.println("Length of pause Pressed");
		tft.fillRect(0, 0, 267, 160, RA8875_GREEN);
		tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
		tft.textSetCursor(15, 48);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Length of pause");
		tft.textSetCursor(20, 83);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("at end of pass");
		delay(100);
		menuPage = 18;
		drawSecondsKeypad();
		delay(100);
	}
	if (x > 533 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(533, 320, 267, 160, RA8875_GREEN);
		tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
		tft.textSetCursor(638, 385);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 5;
		drawSettingsMenu();
		delay(100);
	}
}

void handleMenuPage18(int x, int y) {
	if (x > 400 && x < 600 && y > 320 && y < 480) {
		Serial.println("Done Pressed");
		tft.fillRect(400, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(465, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Done");
		delay(100);
		menuPage = 17;
		drawAutoSettingsMenu();
		delay(100);
	}
	if (x > 600 && x < 800 && y > 320 && y < 480) {
		Serial.println("Back Pressed");
		tft.fillRect(600, 360, 200, 120, RA8875_GREEN);
		tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
		tft.textSetCursor(670, 405);
		tft.textTransparent(RA8875_WHITE);
		tft.textEnlarge(1);
		tft.print("Back");
		delay(100);
		menuPage = 17;
		drawSettingsMenu();
		delay(100);
	}
}

void drawBegin() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(268, 161, 266, 159, RA8875_RED);
	tft.drawRect(268, 161, 266, 159, RA8875_WHITE);
	tft.textSetCursor(320, 205);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(7);
	tft.print("Begin");
}

void drawZero() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(268, 76, 266, 139, RA8875_RED);
	tft.drawRect(268, 76, 266, 139, RA8875_WHITE);
	tft.textSetCursor(310, 128);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Zero Height");

	tft.fillRect(268, 265, 266, 139, RA8875_RED);
	tft.drawRect(268, 265, 266, 139, RA8875_WHITE);
	tft.textSetCursor(310, 318);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Zero Length");
}

void drawMain() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(0, 0, 267, 160, RA8875_RED);
	tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(42, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Manual Mode");

	tft.fillRect(0, 160, 267, 160, RA8875_RED);
	tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
	tft.textSetCursor(20, 225);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Automatic Mode");

	tft.fillRect(0, 320, 267, 160, RA8875_RED);
	tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(70, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Settings");
}

void drawManualMenu() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(0, 0, 267, 160, RA8875_RED);
	tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(15, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Starting Height");

	tft.fillRect(0, 160, 267, 160, RA8875_RED);
	tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
	tft.textSetCursor(15, 225);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Board Thickness");

	tft.fillRect(0, 320, 267, 160, RA8875_RED);
	tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(50, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Log Length");

	tft.fillRect(533, 0, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");

	tft.fillRect(533, 320, 267, 160, RA8875_GREEN);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(655, 385);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("Go");
}

void drawAutomaticMenu() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(0, 0, 267, 160, RA8875_RED);
	tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(15, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Starting Height");

	tft.fillRect(0, 160, 267, 160, RA8875_RED);
	tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
	tft.textSetCursor(15, 225);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Board Thickness");

	tft.fillRect(0, 320, 267, 160, RA8875_RED);
	tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(50, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Log Length");

	tft.fillRect(267, 0, 266, 160, RA8875_RED);
	tft.drawRect(267, 0, 266, 160, RA8875_WHITE);
	tft.textSetCursor(335, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Quantity");

	tft.fillRect(533, 0, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");

	tft.fillRect(533, 320, 267, 160, RA8875_GREEN);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(655, 385);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("Go");
}

void drawSettingsMenu() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(0, 0, 267, 160, RA8875_RED);
	tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(20, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Kerf Thickness");

	tft.fillRect(0, 160, 267, 160, RA8875_RED);
	tft.drawRect(0, 160, 267, 160, RA8875_WHITE);
	tft.textSetCursor(25, 225);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Forward Speed");

	tft.fillRect(533, 0, 267, 160, RA8875_RED);
	tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(560, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Auto Settings");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawAutoSettingsMenu() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(0, 0, 267, 160, RA8875_RED);
	tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(15, 48);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Length of pause");
	tft.textSetCursor(20, 83);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("at end of pass");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawManualGoMenu() {
	tft.fillScreen(RA8875_BLACK);
	tft.fillRect(0, 0, 267, 160, RA8875_RED);
	tft.drawRect(0, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(110, 65);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Up");

	tft.drawRect(267, 0, 266, 160, RA8875_WHITE);

	tft.fillRect(533, 0, 267, 160, RA8875_GREEN);
	tft.drawRect(533, 0, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 65);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("Down");

	tft.fillRect(267, 160, 266, 160, RA8875_YELLOW);
	tft.drawRect(267, 160, 266, 160, RA8875_WHITE);
	tft.textSetCursor(365, 225);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("Zero");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawAutoGoMenu() {
	tft.fillScreen(RA8875_BLACK);

	tft.drawRect(267, 0, 266, 160, RA8875_WHITE);

	tft.fillRect(267, 160, 266, 160, RA8875_YELLOW);
	tft.drawRect(267, 160, 266, 160, RA8875_WHITE);
	tft.textSetCursor(365, 225);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("Zero");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawSureMenu() {
	tft.fillScreen(RA8875_BLACK);

	tft.textSetCursor(240, 125);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("Are you sure?");

	tft.fillRect(0, 320, 267, 160, RA8875_GREEN);
	tft.drawRect(0, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(104, 385);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("No");

	tft.fillRect(533, 320, 267, 160, RA8875_RED);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(645, 385);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(1);
	tft.print("Yes");
}

void drawStartingHeightError() {
	tft.fillScreen(RA8875_RED);

	tft.textSetCursor(100, 50);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Starting Height for your");

	tft.textSetCursor(130, 100);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("mill has been exceeded");

	tft.textSetCursor(70, 200);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Enter lower starting height");

	tft.textSetCursor(250, 250);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("to continue");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawFinishHeightError() {
	tft.fillScreen(RA8875_RED);

	tft.textSetCursor(100, 50);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Finishing Height for your");

	tft.textSetCursor(130, 100);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("mill has been exceeded");

	tft.textSetCursor(70, 200);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Enter lower finishing height");

	tft.textSetCursor(250, 250);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("to continue");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawThicknessError() {
	tft.fillScreen(RA8875_RED);

	tft.textSetCursor(100, 50);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Throat Capacity for your");

	tft.textSetCursor(130, 100);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("mill has been exceeded");

	tft.textSetCursor(115, 200);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Enter smaller thickness");

	tft.textSetCursor(250, 250);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("to continue");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawQuantityError() {
	tft.fillScreen(RA8875_RED);

	tft.textSetCursor(90, 50);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Thickness and quantity for");

	tft.textSetCursor(80, 100);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("your mill has been exceeded");

	tft.textSetCursor(90, 200);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("Enter smaller thickness or");

	tft.textSetCursor(180, 250);
	tft.textTransparent(RA8875_BLACK);
	tft.textEnlarge(2);
	tft.print("quantity to continue");

	tft.fillRect(533, 320, 267, 160, RA8875_BLUE);
	tft.drawRect(533, 320, 267, 160, RA8875_WHITE);
	tft.textSetCursor(638, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawInchesKeypad() {
	tft.fillScreen(RA8875_BLACK);

	tft.fillRect(0, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("1");

	tft.fillRect(200, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("2");

	tft.fillRect(400, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("3");

	tft.drawRect(600, 0, 200, 120, RA8875_WHITE);

	tft.fillRect(0, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("4");

	tft.fillRect(200, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("5");

	tft.fillRect(400, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("6");

	tft.drawRect(600, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(655, 165);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("inches");

	tft.fillRect(0, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("7");

	tft.fillRect(200, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("8");

	tft.fillRect(400, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("9");

	tft.fillRect(0, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(4);
	tft.print(".");

	tft.fillRect(200, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 395);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("0");

	tft.fillRect(400, 360, 200, 120, RA8875_RED);
	tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(465, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Done");

	tft.fillRect(600, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(670, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawFeetKeypad() {
	tft.fillScreen(RA8875_BLACK);

	tft.fillRect(0, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("1");

	tft.fillRect(200, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("2");

	tft.fillRect(400, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("3");

	tft.drawRect(600, 0, 200, 120, RA8875_WHITE);

	tft.fillRect(0, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("4");

	tft.fillRect(200, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("5");

	tft.fillRect(400, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("6");

	tft.drawRect(600, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(670, 165);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("feet");

	tft.fillRect(0, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("7");

	tft.fillRect(200, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("8");

	tft.fillRect(400, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("9");

	tft.fillRect(0, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(4);
	tft.print(".");

	tft.fillRect(200, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 395);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("0");

	tft.fillRect(400, 360, 200, 120, RA8875_RED);
	tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(465, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Done");

	tft.fillRect(600, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(670, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawBoardsKeypad() {
	tft.fillScreen(RA8875_BLACK);

	tft.fillRect(0, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("1");

	tft.fillRect(200, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("2");

	tft.fillRect(400, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("3");

	tft.drawRect(600, 0, 200, 120, RA8875_WHITE);

	tft.fillRect(0, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("4");

	tft.fillRect(200, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("5");

	tft.fillRect(400, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("6");

	tft.drawRect(600, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(655, 165);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("boards");

	tft.fillRect(0, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("7");

	tft.fillRect(200, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("8");

	tft.fillRect(400, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("9");

	tft.fillRect(0, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(4);
	tft.print(".");

	tft.fillRect(200, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 395);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("0");

	tft.fillRect(400, 360, 200, 120, RA8875_RED);
	tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(465, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Done");

	tft.fillRect(600, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(670, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawIPSKeypad() {
	tft.fillScreen(RA8875_BLACK);

	tft.fillRect(0, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("1");

	tft.fillRect(200, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("2");

	tft.fillRect(400, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("3");

	tft.drawRect(600, 0, 200, 120, RA8875_WHITE);

	tft.fillRect(0, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("4");

	tft.fillRect(200, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("5");

	tft.fillRect(400, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("6");

	tft.drawRect(600, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(620, 165);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("inches/sec");

	tft.fillRect(0, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("7");

	tft.fillRect(200, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("8");

	tft.fillRect(400, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("9");

	tft.fillRect(0, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(4);
	tft.print(".");

	tft.fillRect(200, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 395);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("0");

	tft.fillRect(400, 360, 200, 120, RA8875_RED);
	tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(465, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Done");

	tft.fillRect(600, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(670, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}

void drawSecondsKeypad() {
	tft.fillScreen(RA8875_BLACK);

	tft.fillRect(0, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("1");

	tft.fillRect(200, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("2");

	tft.fillRect(400, 0, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 0, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 35);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("3");

	tft.drawRect(600, 0, 200, 120, RA8875_WHITE);

	tft.fillRect(0, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("4");

	tft.fillRect(200, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("5");

	tft.fillRect(400, 120, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 155);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("6");

	tft.drawRect(600, 120, 200, 120, RA8875_WHITE);
	tft.textSetCursor(650, 165);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("seconds");

	tft.fillRect(0, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("7");

	tft.fillRect(200, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("8");

	tft.fillRect(400, 240, 200, 120, RA8875_BLUE);
	tft.drawRect(400, 240, 200, 120, RA8875_WHITE);
	tft.textSetCursor(490, 275);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("9");

	tft.fillRect(0, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(0, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(90, 385);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(4);
	tft.print(".");

	tft.fillRect(200, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(200, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(290, 395);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(2);
	tft.print("0");

	tft.fillRect(400, 360, 200, 120, RA8875_RED);
	tft.drawRect(400, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(465, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Done");

	tft.fillRect(600, 360, 200, 120, RA8875_BLUE);
	tft.drawRect(600, 360, 200, 120, RA8875_WHITE);
	tft.textSetCursor(670, 405);
	tft.textTransparent(RA8875_WHITE);
	tft.textEnlarge(1);
	tft.print("Back");
}
