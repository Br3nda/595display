#include "Display595.h"

#include <Arduino.h>

// TODO JV remove this constant
const int DISPLAY_DIGIT_COUNT = 8;

const int DISPLAY_DIGIT_COUNT_BITMASK = B00000111;

const uint8_t EMPTY_DIGIT_VALUE = 128;
const uint8_t EMPTY_DIGIT_DATA = B00000000;
const uint8_t DECIMAL_POINT_BITMASK = B10000000;

const uint16_t MINIMUM_DISPLAY_REFRESH_RATE = 10;
const uint16_t MAXIMIMUM_DISPLAY_REFRESH_RATE = 100;
const uint16_t DEFAULT_DISPLAY_REFRESH_RATE = 2;

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

uint8_t numberBuffer[DISPLAY_DIGIT_COUNT];
uint8_t decimalPointBuffer[DISPLAY_DIGIT_COUNT];
volatile uint8_t currentDisplayDigit = 0;

volatile uint16_t displayRefreshRate;
uint8_t clockPin;
uint8_t dataPin;
uint8_t latchPin;
uint8_t displayDigitCount;

// static instance needed for ISR callback
Display595 *display595;

Display595::Display595() {
	display595 = this;
}

Display595::~Display595() {
}

void Display595::initialize(uint8_t aClockPin, uint8_t aDataPin, uint8_t aLatchPin, uint8_t aDisplayDigitCount) {

	displayDigitCount = aDisplayDigitCount;

	clockPin = aClockPin;
	dataPin = aDataPin;
	latchPin = aLatchPin;

	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

	Serial.begin(9600);

	clear();
	initializeDisplay();
}

/**
 * Called by the timer ISR to update the display
 */
void Display595::updateDisplay() {
		uint8_t numberDisplayData = EMPTY_DIGIT_DATA;

		uint8_t number = numberBuffer[currentDisplayDigit];
		if (number != EMPTY_DIGIT_VALUE) {
			numberDisplayData = NUMBER_DISPLAY_DATA[number];
		}
		if (decimalPointBuffer[currentDisplayDigit]) {
			numberDisplayData &= DECIMAL_POINT_BITMASK;
		}

		writeSingleDigitData(currentDisplayDigit, numberDisplayData);

		(++currentDisplayDigit) &= DISPLAY_DIGIT_COUNT_BITMASK;
}

void Display595::writeSingleDigitData(uint8_t digitIndex, uint8_t digitData) {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, (1 << digitIndex));
	shiftOut(dataPin, clockPin, MSBFIRST, ~digitData);				// write bit-inverted value
	digitalWrite(latchPin, HIGH);
}

void Display595::initializeDisplay() {
	setRefreshRate(DEFAULT_DISPLAY_REFRESH_RATE);
}

void Display595::setRefreshRate(uint16_t newRefreshRate) {
	displayRefreshRate = newRefreshRate;
	displayRefreshRate = max(displayRefreshRate, MINIMUM_DISPLAY_REFRESH_RATE);
	displayRefreshRate = min(displayRefreshRate, MAXIMIMUM_DISPLAY_REFRESH_RATE);

	Serial.print("displayRefreshRate: ");
	Serial.println(displayRefreshRate);

	resetRefreshRate();
}

/**
 * Called by the timer ISR to reset the refresh rate
 */
void Display595::resetRefreshRate() {

	uint16_t interruptsPerSecond = (displayRefreshRate * displayDigitCount);

	noInterrupts();

#define PRESCALER_1		B00000001
#define PRESCALER_8		B00000010
#define PRESCALER_64	B00000011
#define PRESCALER_256	B00000100
#define PRESCALER_1024	B00000101


	// initialize timer2
	TCCR2A = 0;					// no output compare and no pwm
	TCCR2B = 0;					// ...
	TCNT2 = 0;					//	...

	// set display refresh rate
//	TCNT1 = 65536 - (62500 / interruptsPerSecond);			// (62500 = 16Mhz / 256)
	TCNT2 = 256 - (15625 / interruptsPerSecond);			// (15625 = 16Mhz / 1024)
//	TCCR1B |= (PRESCALER_256);
	TCCR2B |= (PRESCALER_1024);
//	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
	TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt
	interrupts();
}

void Display595::handleInterrupt()
{
	updateDisplay();
	resetRefreshRate();
}

ISR(TIMER2_OVF_vect)        // interrupt service routine
{
	display595->handleInterrupt();
}

void Display595::clear() {
	for (int i = 0; i < DISPLAY_DIGIT_COUNT; i++) {
		setNumber(i, EMPTY_DIGIT_VALUE);
	}
}

void Display595::setDecimalPoint(uint8_t index, boolean enabled) {
	decimalPointBuffer[index] = (enabled ? 1 : 0);
}

void Display595::setNumber(uint8_t index, uint8_t value) {
	numberBuffer[index] = value;
}

void Display595::setNumberAndPoint(uint8_t index, uint8_t value, boolean decimalPoint) {
	setNumber(index, value);
	setDecimalPoint(index, decimalPoint);
}
