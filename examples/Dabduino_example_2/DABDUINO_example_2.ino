/*
  DABDUINO basic example 2s
  DABDUINO is DAB+ digital radio shield for Arduino
  Created by Tomas Urbanek, Montyho technology Ltd., Januar 2, 2017.
  www.dabduino.com
*/

#include "DABDUINO.h"

#define _DAB_SERIAL_PORT Serial1
#define _DAB_RESET_PIN 2
#define _DAB_DAC_MUTE_PIN 3
#define _DAB_DAC_FLT_PIN 4
#define _DAB_DAC_DEMP_PIN 5
#define _DAB_SPI_CS_PIN 6

DABDUINO dab = DABDUINO(_DAB_SERIAL_PORT, _DAB_RESET_PIN, _DAB_DAC_MUTE_PIN, _DAB_DAC_FLT_PIN, _DAB_DAC_DEMP_PIN, _DAB_SPI_CS_PIN);

// DAB variables
char dabText[DAB_MAX_TEXT_LENGTH];



void setup() {
}

void loop() {
}




