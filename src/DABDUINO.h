/*
 * DABDUINO.h - Library for DABDUINO - DAB/DAB+ digital radio shield for Arduino.
 * Created by Tomas Urbanek, Montyho technology Ltd., Januar 2, 2017.
 * www.dabduino.com
 * @license  BSD (see license.txt)
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

  DABDUINO (HardwareSerial& serial, int8_t RESET_PIN, int8_t DAC_MUTE_PIN, int8_t SPI_CS_PIN);
  Stream& _s;

  unsigned char charToAscii(byte byte0, byte byte1);

  void init();

  int8_t isEvent();
  int8_t readEvent();
  int8_t sendCommand(byte dabCommand[], byte dabData[], unsigned int *dabDataSize);

  // *************************
  // ***** SYSETEM ***********
  // *************************

  int8_t reset();
  int8_t resetCleanDB();
  int8_t isReady();
  int8_t setAudioOutput(boolean spdiv, boolean cinch);

  // *************************
  // ***** STREAM ************
  // *************************

  int8_t playDAB(unsigned int programIndex);
  int8_t playFM(unsigned int frequency);
  int8_t playBEEP();
  int8_t playSTOP();
  int8_t searchDAB(unsigned int band);
  int8_t searchFM(unsigned int seekDirection);
  int8_t playStatus(unsigned int *data);
  int8_t playMode(unsigned int *data);
  int8_t getPlayIndex(unsigned int *data);
  int8_t getSignalStrength(unsigned int *signalStrength, unsigned int *bitErrorRate);
  int8_t setStereoMode(boolean stereo);
  int8_t getStereoMode(unsigned int *data);
  int8_t getStereoType(unsigned int *data);
  int8_t setVolume(unsigned int volumeLevel);
  int8_t getVolume(unsigned int *data);
  int8_t getProgramType(unsigned int *data);
  int8_t getProgramShortName(unsigned int programIndex, char text[]);
  int8_t getProgramLongName(unsigned int programIndex, char text[]);
  int8_t getProgramText(char text[]);
  int8_t getSamplingRate(unsigned int *data);
  int8_t getDataRate(unsigned int *data);
  int8_t getSignalQuality(unsigned int *data);
  int8_t getFrequency(unsigned int programIndex, unsigned int *data);
  int8_t getEnsembleShortName(unsigned int programIndex, char text[]);
  int8_t getEnsembleLongName(unsigned int programIndex, char text[]);
  int8_t getProgramIndex(unsigned int *data);
  int8_t isProgramOnAir(unsigned int programIndex);
  int8_t getServiceShortName(unsigned int programIndex, char text[]);
  int8_t getServiceLongName(unsigned int programIndex, char text[]);
  int8_t getSearchIndex(unsigned int *data);
  int8_t getServCompType(unsigned int programIndex, unsigned int *data);
  int8_t setPreset(unsigned int programIndex, unsigned int presetIndex, unsigned int presetMode);
  int8_t getPreset(unsigned int presetIndex, unsigned int presetMode, unsigned int *data);
  int8_t getProgramInfo(unsigned int programIndex, unsigned int *serviceId, unsigned int *ensembleId);
  int8_t getProgramSorter(unsigned int *data);
  int8_t setProgramSorter(unsigned int sortMethod);
  int8_t getDRC(unsigned int *data);
  int8_t setDRC(unsigned int setDRC);
  int8_t prunePrograms(unsigned int *prunedTotalPrograms, unsigned int *prunedProgramIndex);
  int8_t getECC(unsigned int *ECC, unsigned int *countryId);
  int8_t getRdsPIcode(unsigned int *data);
  int8_t setFMstereoThdLevel(unsigned int RSSItresholdLevel);
  int8_t getFMstereoThdLevel(unsigned int *data);
  int8_t getRDSrawData(unsigned int *RDSblockA, unsigned int *RDSblockB, unsigned int *RDSblockC, unsigned int *RDSblockD, unsigned int *BlerA, unsigned int *BlerB, unsigned int *BlerC, unsigned int *BlerD);
  int8_t setFMseekTreshold(unsigned int RSSItreshold);
  int8_t getFMseekTreshold(unsigned int *data);
  int8_t setFMstereoTreshold(unsigned int RSSIstereoTreshold);
  int8_t getFMstereoTreshold(unsigned int *data);
  int8_t getFMexactStation(unsigned int *data);

  // *************************
  // ***** RTC ***************
  // *************************

  int8_t setRTCclock(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);
  int8_t getRTCclock(unsigned int *year, unsigned int *month, unsigned int *week, unsigned int *day, unsigned int *hour, unsigned int *minute, unsigned int *second);
  int8_t RTCsyncEnable();
  int8_t RTCsyncDisable();
  int8_t getRTCsyncStatus(unsigned int *data);
  int8_t getRTCclockStatus(unsigned int *data);

  // ********************************************
  // ***** MOT (Multimedia Object Transfer) *****
  // ********************************************



  // *************************
  // ***** NOTIFY ************
  // *************************

  int8_t eventNotificationEnable();
  int8_t eventNotificationDisable();



private:
  HardwareSerial *_Serial;
  int8_t resetPin;
  int8_t dacMutePin;
  int8_t spiCsPin;
};

