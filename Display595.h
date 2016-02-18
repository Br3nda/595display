
#ifndef _Display595_H_
#define _Display595_H_
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

extern "C" void TIMER2_OVF_vect(void) __attribute__ ((signal));

// TODO JV make displayDigitCount really variable (and remove constant with the same name)
// TODO JV measure performance, both when updating one digit at a time, or when all are updated at the same time
// TODO JV measure performance, with automatic retriggering of the timer interrupt and without.
// TODO JV make timer used configurable -> how? is this configurable at runtime?
// TODO JV add more logic to directly display ints/floats/doubles on the display
// TODO JV add more logic to display more alphanumeric characters

/**
 * Class to drive a 8-digit 7-segment led display using 2 595 shift registers.
 *
 * A default refresh rate of 75 Hz is used for the display, using Timer 2.
 */
class Display595 {

public:
	Display595();
	~Display595();

	void initialize(uint8_t aClockPin, uint8_t aDataPin, uint8_t aLatchPin, uint8_t aDisplayDigitCount);

	void writeSingleDigitData(uint8_t digitIndex, uint8_t digitData);

	/*
	 * Set display refresh rate in Hz
	 */
	void setRefreshRate(uint16_t newRefreshRate);

	void clear();
	void setDecimalPoint(uint8_t index, boolean enabled);
	void setNumber(uint8_t index, uint8_t value);
	void setNumberAndPoint(uint8_t index, uint8_t value, boolean decimalPoint);

private:
	void initializeDisplay();
	void resetRefreshRate();
	void updateDisplay();
	void handleInterrupt();

	friend void TIMER2_OVF_vect();
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _Display595_H_ */
