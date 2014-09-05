#include "595Display.h"

#include <Arduino.h>

const int CLOCK_PIN = 9;
const int LATCH_PIN = 10;
const int DATA_PIN = 12;

const int POTMETER_PIN = A0;

const int DISPLAY_DIGIT_COUNT = 8;

const uint8_t EMPTY_DIGIT_VALUE = 128;
const uint8_t EMPTY_DIGIT_DATA = B00000000;
const uint8_t DECIMAL_POINT_BITMASK = B10000000;

/*
	These bits encode for these segments:

	     0
	   ----
	5 |    | 1
	  |  6 |
	   ----
	4 |    | 2
	  |  3 |
	   ----  o 7

 */

const uint8_t NUMBER_DISPLAY_DATA[] = {
	B00111111,			// 0
	B00000110,			// 1
	B01011011,			// 2
	B01001111,			// 3
	B01100110,			// 4
	B01101101,			// 5
	B01111101,			// 6
	B00000111,			// 7
	B01111111,			// 8
	B01101111,			// 9
};

uint8_t numberBuffer[8];
uint8_t decimalPointBuffer[8];

void setup() {
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);

	pinMode(POTMETER_PIN, INPUT);

	clearDisplay();
	initializeDisplay();

	setNumberAndPoint(0, 7, false);
	setNumberAndPoint(1, 0, false);
	setNumberAndPoint(2, 8, false);
	setNumberAndPoint(3, 3, false);
	setNumberAndPoint(4, 1, false);
	setNumberAndPoint(5, 7, false);
	setNumberAndPoint(6, 0, false);
}

void loop() {

	uint8_t delayValue = (analogRead(POTMETER_PIN) / 4);
	if (delayValue == 1) {
		delayValue = 0;
	}

	for (int i = 0; i < DISPLAY_DIGIT_COUNT; i++) {
		uint8_t numberDisplayData = EMPTY_DIGIT_DATA;

		uint8_t number = numberBuffer[i];
		if (number != EMPTY_DIGIT_VALUE) {
			numberDisplayData = NUMBER_DISPLAY_DATA[number];
		}
		if (decimalPointBuffer[i]) {
			numberDisplayData &= DECIMAL_POINT_BITMASK;
		}

		writeSingleDigitData(i, numberDisplayData);
		delay(delayValue);
	}
}

void writeSingleDigitData(uint8_t digitIndex, uint8_t digitData) {
	digitalWrite(LATCH_PIN, LOW);
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (1 << digitIndex));
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, ~digitData);				// write bit-inverted value
	digitalWrite(LATCH_PIN, HIGH);
}

void initializeDisplay() {
	// TODO setup timer routine
}

void clearDisplay() {
	for (int i = 0; i < DISPLAY_DIGIT_COUNT; i++) {
		setNumber(i, EMPTY_DIGIT_VALUE);
	}
}

void setDecimalPoint(uint8_t index, boolean enabled) {
	decimalPointBuffer[index] = (enabled ? 1 : 0);
}

void setNumber(uint8_t index, uint8_t value) {
	numberBuffer[index] = value;
}

void setNumberAndPoint(uint8_t index, uint8_t value, boolean decimalPoint) {
	setNumber(index, value);
	setDecimalPoint(index, decimalPoint);
}
