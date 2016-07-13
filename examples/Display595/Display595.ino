#include <Display595.h>

const int CLOCK_PIN = 5;  //SCK
const int LATCH_PIN = 6; //RCK
const int DATA_PIN = 3;  //DIO
const int DISPLAY_DIGIT_COUNT = 8;

Display595 display;


void setup() {

  display.initialize(CLOCK_PIN, DATA_PIN, LATCH_PIN, DISPLAY_DIGIT_COUNT);

  for(int i=0; i<DISPLAY_DIGIT_COUNT; i++) {
    display.setNumberAndPoint(i, i, false);
  }
}

void loop() {
}
