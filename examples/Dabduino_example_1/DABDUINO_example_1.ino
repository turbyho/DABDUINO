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

// Others constants & variables

void setup() {

  Serial.begin(57600);

  Serial.println("DAB RESET & START");
  dab.initialization();
  if (!dab.resetDabModule()) {
    Serial.println("DAB NOT READY");
    while (1) {}
  }
  Serial.println("DAB READY");

  Serial.print("Search for DAB programs:");
  dab.startSearchDabPrograms();

  int8_t status;
  int8_t lastStatus;
  while (true) {
    status = dab.getPlayStatus();
    if (status != lastStatus) {
      Serial.println();
      // 1=playing, 2=searching, 3=tuning, 4=stop, 5=sorting change, 6=reconfiguration
      switch (status) {
        case 1:
          Serial.print("Playing");
          break;
        case 2:
          Serial.print("Searching");
          break;
        case 3:
          Serial.print("Tuning");
          break;
        case 4:
          Serial.print("Stop");
          break;
        case 5:
          Serial.print("Sorting");
          break;
        case 6:
          Serial.print("Reconfiguration");
          break;
      }
    }
    lastStatus = status;
    if (status == 1 || status == 4) break;
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");

  programsIndex = dab.getDabProgramsIndex();
  Serial.println("Available programs: ");
  for (unsigned int i = 1; i <= programsIndex; i++) {
    if (dab.getProgramLongName(i, dabText)) {
      Serial.print(i);
      Serial.print("\t ");
      Serial.println(dabText);
    }
  }
  Serial.println();

  if (dab.setAudioOutputType(true, true)) { // 1st = spdiv, 2st = i2s/dac
    Serial.println("Set audio output");
  }

  if (dab.setVolume(8)) { // Set volume: 0..16
    Serial.println("Set volume");
  }

  if (dab.enableNotification()) {
    Serial.println("Notification enabled");
  }

  programIndex = 1;

  if (dab.tuneDabProgram(programIndex)) {
    if (dab.getProgramLongName(programIndex, dabText)) {
      Serial.print("Tuned program: (");
      Serial.print(programIndex);
      Serial.print(") ");
      Serial.println(dabText);
    }
  }
}

void loop() {

  // EVENTS
  // EVENT TYP: 1=scan finish, 2=got new DAB program text, 3=DAB reconfiguration, 4=DAB channel list order change, 5=RDS group, 6=Got new FM radio text, 7=Return the scanning frequency /FM/
  if (dab.isEvent()) {
    byte eventData[16];
    unsigned int dataSize;
    int8_t eventTyp = dab.readEvent(eventData, &dataSize);
    switch (eventTyp) {
      case 1:
        Serial.println("DAB program search finished.");
        break;
      case 2:
        //do something when New DAB progam text
        int8_t res = dab.getProgrameText(dabText);
        if (res == 1) { // new text
          Serial.print("DAB text event: ");
          Serial.println(dabText);
        } else if (res == 2) { // same text
          Serial.println("DAB text event: the new text is the same...");
        }
        break;
    }
  }

}




