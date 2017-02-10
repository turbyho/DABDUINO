/*
 * DABDUINO.h - Library for DABDUINO / DAB+ - digital radio shield for Arduino.
 * Created by Tomas Urbanek, Januar 2, 2017.
 * Released into the public domain.
 */

#include "Arduino.h"

#define DAB_MAX_TEXT_LENGTH 128
#define DAB_MAX_DATA_LENGTH 2 * DAB_MAX_TEXT_LENGTH

namespace constants
{
const int8_t DEMO = 0;
}

class DABDUINO
{
public:

  DABDUINO (HardwareSerial& serial, int8_t RESET_PIN, int8_t DAC_MUTE_PIN, int8_t DAC_FLT_PIN, int8_t DAC_DEMP_PIN, int8_t SPI_CS_PIN);
  Stream& _s;

  unsigned char SimplifyDoubleCharForSorting( unsigned char c1, unsigned char c2, bool changeToLowerCase );
  unsigned char SimplifySingleCharForSorting( unsigned char c, bool changeToLowerCase );
  unsigned char charToAscii(byte byte0, byte byte1);

  void initialization();
  int8_t resetDabModule();
  int8_t isDabReady();
  int8_t isEvent();
  int8_t readEvent(byte eventData[], unsigned int *eventDataSize);
  int8_t sendCommand(byte dabCommand[], byte dabData[], unsigned int *dabDataSize);

  int8_t enableNotification();
  int8_t disableNotification();
  int8_t setAudioOutputType(boolean spdiv, boolean i2s);
  int8_t setVolume(byte volumeLevel);
  int8_t startSearchDabPrograms();
  int8_t getPlayStatus();
  unsigned int getTunedIndex();
  unsigned int getDabProgramsIndex();
  int8_t tuneDabProgram(unsigned int programIndex);
  int8_t getProgramShortName(unsigned int programIndex, char dabText[]);
  int8_t getProgramLongName(unsigned int programIndex, char dabText[]);
  int8_t getProgrameText(char dabText[]);

  int8_t tuneFmProgram(long frequency);
  int8_t seekFmProgram(byte seekDirection);

private:
  HardwareSerial *_Serial;
  int8_t resetPin;
  int8_t dacMutePin;
  int8_t dacFltPin;
  int8_t dacDempPin;
  int8_t spiCsPin;
};

