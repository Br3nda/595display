// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _595Display_H_
#define _595Display_H_
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

void loop();
void setup();

void writeSingleDigitData(uint8_t digitIndex, uint8_t digitData);
void initializeDisplay();
void clearDisplay();
void setDecimalPoint(uint8_t index, boolean enabled);
void setNumber(uint8_t index, uint8_t value);
void setNumberAndPoint(uint8_t index, uint8_t value, boolean decimalPoint);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _595Display_H_ */
