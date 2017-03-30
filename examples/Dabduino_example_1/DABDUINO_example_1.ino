/*
  DABDUINO basic example 1
  DABDUINO is DAB+ digital radio shield for Arduino
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
unsigned int programsIndex = 0;
unsigned int programIndex = 0;

void setup() {

  Serial.begin(57600);

  Serial.println("DAB RESET & START");
  dab.init();
  if (!dab.reset()) {
    Serial.println("DAB NOT READY");
    while (1) {}
  }
  Serial.println("DAB READY");

  Serial.print("Search for DAB programs:");
  dab.searchDAB(1);

  unsigned int status;
  unsigned int lastStatus;

  while (true) {
    dab.playStatus(&status);
    if (status != lastStatus) {
      Serial.println();
      switch (status) {
      case 0:
        Serial.print("Playing");
        break;
      case 1:
        Serial.print("Searching");
        break;
      case 2:
        Serial.print("Tuning");
        break;
      case 3:
        Serial.print("Stop");
        break;
      case 4:
        Serial.print("Sorting");
        break;
      case 5:
        Serial.print("Reconfiguration");
        break;
      }
    }
    lastStatus = status;
    if (status == 0 || status == 3) break;
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");

  dab.getProgramIndex(&programsIndex);
  Serial.println("Available programs: ");
  for (unsigned int i = 0; i <= programsIndex; i++) {
    if (dab.getProgramLongName(i, dabText)) {
      Serial.print(i);
      Serial.print("\t ");
      Serial.println(dabText);
    }
  }
  Serial.println();

  if (dab.setAudioOutput(true, true)) { // 1st = spdiv (optical), 2st = cinch (analog)
    Serial.println("Set audio output");
  }

  if (dab.setVolume(8)) { // Set volume: 0..16
    Serial.println("Set volume");
  }

  programIndex = 0;

  if (dab.playDAB(programIndex)) {
    if (dab.getProgramLongName(programIndex, dabText)) {
      Serial.print("Tuned program: (");
      Serial.print(programIndex);
      Serial.print(") ");
      Serial.println(dabText);
    }
  }

  if (dab.eventNotificationEnable()) {
    Serial.println("Event notification enabled");
  }
}

void loop() {

  // EVENTS
  // EVENT TYP: 1=scan finish, 2=got new DAB program text, 3=DAB reconfiguration, 4=DAB channel list order change, 5=RDS group, 6=Got new FM radio text, 7=Return the scanning frequency /FM/
  if (dab.isEvent()) {

    switch (dab.readEvent()) {
    case 1:
      Serial.println("DAB program search finished.");
      break;
    case 2:
      //do something when New DAB progam text
      if (dab.getProgramText(dabText)) { // new text
        Serial.print("DAB text event: ");
        Serial.println(dabText);
      }
      break;
    }
  }
}




