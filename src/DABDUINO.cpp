/*
 *  DABDUINO.cpp - Library for DABDUINO - DAB/DAB+ digital radio shield for Arduino.
 *  Created by Tomas Urbanek, Januar 2, 2017.
 *  Released into the public domain.
 */

#include "DABDUINO.h"

DABDUINO::DABDUINO(HardwareSerial& serial, int8_t RESET_PIN, int8_t DAC_MUTE_PIN, int8_t DAC_FLT_PIN, int8_t DAC_DEMP_PIN, int8_t SPI_CS_PIN) : _s(serial) {

  _Serial = &serial;
  resetPin = RESET_PIN;
  dacMutePin = DAC_MUTE_PIN;
  dacFltPin = DAC_FLT_PIN;
  dacDempPin = DAC_DEMP_PIN;
  spiCsPin = SPI_CS_PIN;

}

/*
 * Convert two byte char from DAB to one byte char. Add next chars...
 */
byte DABDUINO::charToAscii(byte byte1, byte byte0) {

  if (byte1 == 0x00) {

    if (byte0 == 0x0) {
      return (byte0);
    }

    if (byte0 < 128) {
      return (byte0);
    }

    switch (byte0)
    {
    case 0x8A: return (0x53); break;
    case 0x8C: return (0x53); break;
    case 0x8D: return (0x54); break;
    case 0x8E: return (0x5a); break;
    case 0x8F: return (0x5a); break;
    case 0x9A: return (0x73); break;
    case 0x9D: return (0x74); break;
    case 0x9E: return (0x7a); break;
    case 0xC0: return (0x41); break;
    case 0xC1: return (0x41); break;
    case 0xC2: return (0x41); break;
    case 0xC3: return (0x41); break;
    case 0xC4: return (0x41); break;
    case 0xC5: return (0x41); break;
    case 0xC7: return (0x43); break;
    case 0xC8: return (0x45); break;
    case 0xC9: return (0x45); break;
    case 0xCA: return (0x45); break;
    case 0xCB: return (0x45); break;
    case 0xCC: return (0x49); break;
    case 0xCD: return (0x49); break;
    case 0xCE: return (0x49); break;
    case 0xCF: return (0x49); break;
    case 0xD0: return (0x44); break;
    case 0xD1: return (0x4e); break;
    case 0xD2: return (0x4f); break;
    case 0xD3: return (0x4f); break;
    case 0xD4: return (0x4f); break;
    case 0xD5: return (0x4f); break;
    case 0xD6: return (0x4f); break;
    case 0xD8: return (0x4f); break;
    case 0xD9: return (0x55); break;
    case 0xDA: return (0x55); break;
    case 0xDB: return (0x55); break;
    case 0xDC: return (0x55); break;
    case 0xDD: return (0x59); break;
    case 0xE0: return (0x61); break;
    case 0xE1: return (0x61); break;
    case 0xE2: return (0x61); break;
    case 0xE3: return (0x61); break;
    case 0xE4: return (0x61); break;
    case 0xE5: return (0x61); break;
    case 0xE7: return (0x63); break;
    case 0xE8: return (0x65); break;
    case 0xE9: return (0x65); break;
    case 0xEA: return (0x65); break;
    case 0xEB: return (0x65); break;
    case 0xEC: return (0x69); break;
    case 0xED: return (0x69); break;
    case 0xEE: return (0x69); break;
    case 0xEF: return (0x69); break;
    case 0xF1: return (0x6e); break;
    case 0xF2: return (0x6f); break;
    case 0xF3: return (0x6f); break;
    case 0xF4: return (0x6f); break;
    case 0xF5: return (0x6f); break;
    case 0xF6: return (0x6f); break;
    case 0xF9: return (0x75); break;
    case 0xFA: return (0x75); break;
    case 0xFB: return (0x75); break;
    case 0xFC: return (0x75); break;
    case 0xFD: return (0x79); break;
    case 0xFF: return (0x79); break;
    }
  }

  if (byte1 == 0x01) {
    switch (byte0)
    {
    case 0x1B: return (0x65); break; // ě > e
    case 0x48: return (0x6e); break; // ň > n
    case 0x59: return (0x72); break; // ř > r
    case 0x0D: return (0x63); break; // č > c
    case 0x7E: return (0x7A); break; // ž > z
    case 0x0C: return (0x43); break; // Č > C
    }
  }

  return  (0x20);
}

void DABDUINO::init() {

  // DAC MUTE
  pinMode(dacMutePin, OUTPUT);
  digitalWrite(dacMutePin, HIGH);

  // DAC FLT
  pinMode(dacFltPin, OUTPUT);
  digitalWrite(dacFltPin, HIGH);

  // DAC DEMP
  pinMode(dacDempPin, OUTPUT);
  digitalWrite(dacDempPin, LOW);

  // SPI CS
  pinMode(spiCsPin, OUTPUT);
  digitalWrite(spiCsPin, LOW);

  // DAB module SERIAL
  _Serial->begin(57600);
  _Serial->setTimeout(50);

  // DAB module RESET
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);
  delay(100);
  digitalWrite(resetPin, HIGH);
  delay(1000);

  while (!isReady()) {
    delay(100);
  }
}

int8_t DABDUINO::isEvent() {
  return _Serial->available();
}

/*
 *   Read event
 *   RETURN EVENT TYP: 1=scan finish, 2=got new DAB program text, 3=DAB reconfiguration, 4=DAB channel list order change, 5=RDS group, 6=Got new FM radio text, 7=Return the scanning frequency /FM/
 */
int8_t DABDUINO::readEvent(byte eventData[], unsigned int *eventDataSize) {

  byte dabReturn[6];
  byte isPacketCompleted = 0;
  uint16_t byteIndex = 0;
  uint16_t dataIndex = 0;
  byte serialData = 0;
  *eventDataSize = 128;
  unsigned long endMillis = millis() + 200; // timeout for answer from module = 200ms
  while (millis() < endMillis && dataIndex < DAB_MAX_DATA_LENGTH) {
    if (_Serial->available() > 0) {
      serialData = _Serial->read();
      if (serialData == 0xFE) {
        byteIndex = 0;
        dataIndex = 0;
      }
      if (*eventDataSize && dataIndex < *eventDataSize) {
        eventData[dataIndex++] = serialData;
      }
      if (byteIndex <= 5) {
        dabReturn[byteIndex] = serialData;
      }
      if (byteIndex == 5) {
        *eventDataSize = (((long)dabReturn[4] << 8) + (long)dabReturn[5]);
      }
      if ((int16_t)(byteIndex - *eventDataSize) >= 5 && serialData == 0xFD) {
        isPacketCompleted = 1;
        break;
      }
      byteIndex++;
    }
  }
  while (_Serial->available() > 0) {
    _Serial->read();
  }
  if (isPacketCompleted == 1 && dabReturn[1] == 0x07) {
    return dabReturn[2] + 1;
  } else {
    return 0;
  }
}

/*
 *  Send command to DAB module and wait for answer
 */
int8_t DABDUINO::sendCommand(byte dabCommand[], byte dabData[], unsigned int *dabDataSize) {

  byte dabReturn[6];
  byte isPacketCompleted = 0;
  uint16_t byteIndex = 0;
  uint16_t dataIndex = 0;
  byte serialData = 0;
  *dabDataSize = 0;
  while (_Serial->available() > 0) {
    _Serial->read();
  }
  while (byteIndex < 255) {
    if (dabCommand[byteIndex++] == 0xFD) break;
  }
  _Serial->write(dabCommand, byteIndex);
  _Serial->flush();
  byteIndex = 0;
  unsigned long endMillis = millis() + 200; // timeout for answer from module = 200ms
  while (millis() < endMillis && dataIndex < DAB_MAX_DATA_LENGTH) {
    if (_Serial->available() > 0) {
      serialData = _Serial->read();
      if (serialData == 0xFE) {
        byteIndex = 0;
        dataIndex = 0;
      }
      if (*dabDataSize && dataIndex < *dabDataSize) {
        dabData[dataIndex++] = serialData;
      }
      if (byteIndex <= 5) {
        dabReturn[byteIndex] = serialData;
      }
      if (byteIndex == 5) {
        *dabDataSize = (((long)dabReturn[4] << 8) + (long)dabReturn[5]);
      }
      if ((int16_t)(byteIndex - *dabDataSize) >= 5 && serialData == 0xFD) {
        isPacketCompleted = 1;
        break;
      }
      byteIndex++;
    }
  }
  if (isPacketCompleted == 1 && !(dabReturn[1] == 0x00 && dabReturn[2] == 0x02)) {
    return 1;
  } else {
    return 0;
  }
}

// *************************
// ***** SYSETEM ***********
// *************************

/*
 *   Reset DAB module only
 */
int8_t DABDUINO::reset() {
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x00, 0x01, 0x00, 0x00, 0x01, 0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    init();
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Clean DAB module database and reset module
 */
int8_t DABDUINO::resetCleanDB() {
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x00, 0x01, 0x00, 0x00, 0x01, 1, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    init();
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Test for DAB module is ready for communication
 */
int8_t DABDUINO::isReady() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Set audio output channels (SPDIV, CINCH /I2S DAC/)
 *   CINCH for analog output, SPDIV for optical digital output
 */
int8_t DABDUINO::setAudioOutput(boolean spdiv = true, boolean cinch = true) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte s;
  if (spdiv && spdiv) {
    s = B00000011;
  } else if (spdiv && !cinch) {
    s = B00000001;
  } else if (!spdiv && cinch) {
    s = B00000010;
  } else {
    s = B00000000;
  }
  byte dabCommand[8] = { 0xFE, 0x00, 0x06, 0x00, 0x00, 0x01, s, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}


// *************************
// ***** STREAM ************
// *************************

/*
 *   Play DAB program
 *   programIndex = 1..9999999 (see programs index)
 */
int8_t DABDUINO::playDAB(unsigned int programIndex) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabCommand[12] = { 0xFE, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Play FM program
 *   frequency = 87500..108000 (MHz)
 */
int8_t DABDUINO::playFM(unsigned int frequency) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte Byte0 = ((frequency >> 0) & 0xFF);
  byte Byte1 = ((frequency >> 8) & 0xFF);
  byte Byte2 = ((frequency >> 16) & 0xFF);
  byte Byte3 = ((frequency >> 24) & 0xFF);
  byte dabCommand[12] = { 0xFE, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Play BEEP
 */
int8_t DABDUINO::playBEEP() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Play STOP
 */
int8_t DABDUINO::playSTOP() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Search DAB bands for programs
 * zone: 1=BAND-3, 2=CHINA-BAND, 3=L-BAND
 */

int8_t DABDUINO::searchDAB(unsigned int band = 1) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;

  byte chStart = 0;
  byte chEnd = 40;

  switch (band) {
  case 1:
    chStart = 0;
    chEnd = 40;
    break;
  case 2:
    chStart = 41;
    chEnd = 71;
    break;
  case 3:
    chStart = 72;
    chEnd = 94;
    break;
  }

  byte dabCommand[9] = { 0xFE, 0x01, 0x03, 0x00, 0x00, 0x02, chStart, chEnd, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Seek FM program - searchDirection: 0=backward, 1=forward
 */
int8_t DABDUINO::searchFM(unsigned int searchDirection) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  if (searchDirection < 0) searchDirection = 0;
  if (searchDirection > 1) searchDirection = 1;
  byte dabCommand[8] = { 0xFE, 0x01, 0x02, 0x00, 0x00, 0x01, searchDirection, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Radio module play status
 *   return data: 0=playing, 1=searching, 2=tuning, 3=stop, 4=sorting change, 5=reconfiguration
 */
int8_t DABDUINO::playStatus(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x05, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Radio module play mode
 *   return data: 0=DAB, 1=FM, 2=BEEP, 255=Stream stop
 */
int8_t DABDUINO::playMode(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x06, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      if (dabData[0] != 0xFF) {
        *data = (unsigned int)dabData[0];
        return 1;
      } else {
        *data = (unsigned int)dabData[0];
        return 1;
      }
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 * Get DAB station index, get tuned FM station frequency
 */
int8_t DABDUINO::getPlayIndex(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x07, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize == 4) {
      *data = (((long)dabData[0] << 24) + ((long)dabData[1] << 16) + ((long)dabData[2] << 8) + (long)dabData[3]);
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get signal strength
 * DAB: signalStrength=0..18, bitErrorRate=
 * FM: signalStrength=0..100
 */
int8_t DABDUINO::getSignalStrength(unsigned int *signalStrength, unsigned int *bitErrorRate) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x08, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    *signalStrength = (unsigned int)dabData[0];
    *bitErrorRate = 0;
    if (dabDataSize > 1) {
      *bitErrorRate =  (((long)dabData[1] << 8) + (long)dabData[2]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Set stereo mode
 *   true=stereo, false=force mono
 */
int8_t DABDUINO::setStereoMode(boolean stereo = true) {

  int value = 0;
  if (stereo == true) {
    value = 1;
  }
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x09, 0x00, 0x00, 0x01, value, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Get stereo mode
 *   0=force mono, 1=auto detect stereo
 */
int8_t DABDUINO::getStereoMode(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x0A, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Get stereo type
 *   return data: 0=stereo, 1=join stereo, 2=dual channel, 3=single channel (mono)
 */
int8_t DABDUINO::getStereoType(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x0B, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Set volume
 *   volumeLevel = 0..16
 */
int8_t DABDUINO::setVolume(unsigned int volumeLevel) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  if (volumeLevel < 0) volumeLevel = 0;
  if (volumeLevel > 16)  volumeLevel = 16;
  byte dabCommand[8] = { 0xFE, 0x01, 0x0C, 0x00, 0x00, 0x01, volumeLevel, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Get volume
 *   return set volumeLevel: 0..16
 */
int8_t DABDUINO::getVolume(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x0D, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Get program type
 *   0=N/A, 1=News, 2=Curent Affairs, 3=Information, 4=Sport, 5=Education, 6=Drama, 7=Arts, 8=Science, 9=Talk, 10=Pop music, 11=Rock music, 12=Easy listening, 13=Light Classical, 14=Classical music, 15=Other music, 16=Weather, 17=Finance, 18=Children's, 19=Factual, 20=Religion, 21=Phone in, 22=Travel, 23=Leisure, 24=Jazz & Blues, 25=Country music, 26=National music, 27=Oldies music, 28=Folk Music, 29=Documentary, 30=undefined, 31=undefined
 */
int8_t DABDUINO::getProgramType(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x0E, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 * Get DAB station short name
 */
int8_t DABDUINO::getProgramShortName(unsigned int programIndex, char text[]) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x0F, 0x00, 0x00, 0x05, Byte3, Byte2, Byte1, Byte0, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB station long name
 */
int8_t DABDUINO::getProgramLongName(unsigned int programIndex, char text[]) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x0F, 0x00, 0x00, 0x05, Byte3, Byte2, Byte1, Byte0, 0x01, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB text event
 * return: 1=new text, 2=text is same, 3=no text
 * dabText: text
 */
int8_t DABDUINO::getProgramText(char text[]) {

  char textLast[DAB_MAX_TEXT_LENGTH];
  memcpy(textLast, text, sizeof(text[0])*DAB_MAX_TEXT_LENGTH);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x10, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize == 1) {
      text[0] = dabData[0]; // 0=There is no text in programe, 1=The program text received, but no text available
      text[1] = 0x00;
      return 3; // No error, but no text
    }
    int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    for (unsigned int i = 0; i < j; i++) {
      if (text[i] != textLast[i]) {
        return 1; // New dab text
      }
    }
    return 2; // Same dab text
  } else {
    return 0;
  }
}

/*
 *   Get sampling rate (DAB/FM)
 *   return data: 1=32kHz, 2=24kHz, 3=48kHz
 */
int8_t DABDUINO::getSamplingRate(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x11, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Get data rate (DAB)
 *   return data: data rate in kbps
 */
int8_t DABDUINO::getDataRate(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x12, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (((long)dabData[0] << 8) + (long)dabData[1]);
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Get DAB signal quality
 *   return: 0..100
 *   0..19 = playback stop
 *   20..30 = the noise (short break) appears
 *   100 = the bit error rate is 0
 */
int8_t DABDUINO::getSignalQuality(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x13, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Get DAB frequency for program index
 *   return: frequency index
 *   0=174.928MHz, 1=176.64, 2=178.352,...
 *
 *  // TODO: add conversion table for index2freqency
 */
int8_t DABDUINO::getFrequency(unsigned int programIndex, unsigned int *data) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[11] = { 0xFE, 0x01, 0x14, 0x00, 0x00, 0x04, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 * Get DAB program ensemble short name
 */
int8_t DABDUINO::getEnsembleShortName(unsigned int programIndex, char text[]) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x15, 0x00, 0x00, 0x05, Byte3, Byte2, Byte1, Byte0, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB program ensemble long name
 */
int8_t DABDUINO::getEnsembleLongName(unsigned int programIndex, char text[]) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x15, 0x00, 0x00, 0x05, Byte3, Byte2, Byte1, Byte0, 0x01, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB stations index (number of programs in database)
 */
int8_t DABDUINO::getProgramIndex(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x16, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize == 4) {
      *data = (((long)dabData[0] << 24) + ((long)dabData[1] << 16) + ((long)dabData[2] << 8) + (long)dabData[3]);
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Test DAB program is active (on-air)
 *   return: 0=off-air, 1=on-air
 */
int8_t DABDUINO::isProgramOnAir(unsigned int programIndex) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[11] = { 0xFE, 0x01, 0x17, 0x00, 0x00, 0x04, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      return (unsigned int)dabData[0];
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 * Get DAB program service short name
 */
int8_t DABDUINO::getServiceShortName(unsigned int programIndex, char text[]) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x1A, 0x00, 0x00, 0x05, Byte3, Byte2, Byte1, Byte0, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB program service long name
 */
int8_t DABDUINO::getServiceLongName(unsigned int programIndex, char text[]) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x1A, 0x00, 0x00, 0x05, Byte3, Byte2, Byte1, Byte0, 0x01, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      text[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB search index (number of programs found in search process)
 */
int8_t DABDUINO::getSearchIndex(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x1B, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}


/*
 * Get DAB program service component type (ASCTy)
 * return data: 0=DAB, 1=DAB+, 2=Packet data, 3=DMB (stream data)
 */
int8_t DABDUINO::getServCompType(unsigned int programIndex, unsigned int *data) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[11] = { 0xFE, 0x01, 0x1E, 0x00, 0x00, 0x04, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Set preset
 *   programIndex = DAB: programIndex, FM: frequency
 *   presetIndex = 0..9
 *   presetMode = 0=DAB, 1=FM
 */
int8_t DABDUINO::setPreset(unsigned int programIndex, unsigned int presetIndex, unsigned int presetMode) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[13] = { 0xFE, 0x01, 0x21, 0x00, 0x00, 0x06, presetMode, presetIndex, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *  Get preset
 *  presetIndex = 0..9
 *  presetMode = 0=DAB, 1=FM
 */
int8_t DABDUINO::getPreset(unsigned int presetIndex, unsigned int presetMode, unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[9] = { 0xFE, 0x01, 0x22, 0x00, 0x00, 0x02, presetMode, presetIndex, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if(dabDataSize) {
      *data = (((long)dabData[0] << 24) + ((long)dabData[1] << 16) + ((long)dabData[2] << 8) + (long)dabData[3]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get program info
 * return serviceId = service id of DAB program
 * return ensembleId = ensemble id of DAB program
 */
int8_t DABDUINO::getProgramInfo(unsigned int programIndex, unsigned int *serviceId, unsigned int *ensembleId) {

  byte Byte0 = ((programIndex >> 0) & 0xFF);
  byte Byte1 = ((programIndex >> 8) & 0xFF);
  byte Byte2 = ((programIndex >> 16) & 0xFF);
  byte Byte3 = ((programIndex >> 24) & 0xFF);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[11] = { 0xFE, 0x01, 0x23, 0x00, 0x00, 0x04, Byte3, Byte2, Byte1, Byte0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *serviceId = (((long)dabData[0] << 24) + ((long)dabData[1] << 16) + ((long)dabData[2] << 8) + (long)dabData[3]);
      *ensembleId = (((long)dabData[4] << 8) + (long)dabData[5]);
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get program sorter
 * return data = 0=sort by ensembleID, 1=sort by service name, 2=sort by active and inactive program
 */
int8_t DABDUINO::getProgramSorter(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x24, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Set program sorter
 *   sortMethod = 0=sort by ensembleID, 1=sort by service name, 2=sort by active and inactive program
 */
int8_t DABDUINO::setProgramSorter(unsigned int sortMethod) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x25, 0x00, 0x00, 0x01, sortMethod, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DRC
 * return data = 0=DRC off, 1=DRC low, 2=DRC high
 */
int8_t DABDUINO::getDRC(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x26, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Set DRC
 *   setDRC = 0=DRC off, 1=DRC low, 2=DRC high
 */
int8_t DABDUINO::setDRC(unsigned int setDRC) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x27, 0x00, 0x00, 0x01, setDRC, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}


/*
 *   Prune programs - delete inactive programs (!on-air)
 *
 */
int8_t DABDUINO::prunePrograms(unsigned int *prunedTotalPrograms, unsigned int *prunedProgramIndex) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x2B, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *prunedTotalPrograms = (((long)dabData[0] << 8) + (long)dabData[1]);
      *prunedProgramIndex = (((long)dabData[2] << 8) + (long)dabData[3]);
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get ECC
 * return ECC (Extended Country Code)
 * return countryId (Country identification)
 */
int8_t DABDUINO::getECC(unsigned int *ECC, unsigned int *countryId) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x2D, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *ECC = (unsigned int)dabData[0];
      *countryId = (unsigned int)dabData[1];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get FM RDS PI code
 * return PI code
 */
int8_t DABDUINO::getRdsPIcode(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x2E, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (((long)dabData[0] << 8) + (long)dabData[1]);
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Set FMstereoThdLevel
 *   RSSItresholdLevel = 0..10
 */
int8_t DABDUINO::setFMstereoThdLevel(unsigned int RSSItresholdLevel) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x30, 0x00, 0x00, 0x01, RSSItresholdLevel, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Get FMstereoThdLevel
 *   data return = 0..10
 */
int8_t DABDUINO::getFMstereoThdLevel(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x31, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get RDS raw data
 * return: 1=new RDS data, 2=no new RDS data, 3=no RDS data
 */
int8_t DABDUINO::getRDSrawData(unsigned int *RDSblockA, unsigned int *RDSblockB, unsigned int *RDSblockC, unsigned int *RDSblockD, unsigned int *BlerA, unsigned int *BlerB, unsigned int *BlerC, unsigned int *BlerD) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x32, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize > 1) {
      *RDSblockA = (((long)dabData[0] << 8) + (long)dabData[1]);
      *RDSblockB = (((long)dabData[2] << 8) + (long)dabData[3]);
      *RDSblockB = (((long)dabData[4] << 8) + (long)dabData[5]);
      *RDSblockB = (((long)dabData[6] << 8) + (long)dabData[7]);
      *BlerA = (((long)dabData[8] << 8) + (long)dabData[9]);
      *BlerB = (((long)dabData[10] << 8) + (long)dabData[11]);
      *BlerC = (((long)dabData[12] << 8) + (long)dabData[13]);
      *BlerD = (((long)dabData[14] << 8) + (long)dabData[15]);
      return 1;
    } else {
      if (dabData[0] == 1) {
        return 2;
      } else {
        return 3;
      }
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Set FMseekTreshold
 *   RSSItreshold = 0..100
 */
int8_t DABDUINO::setFMseekTreshold(unsigned int RSSItreshold) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x35, 0x00, 0x00, 0x01, RSSItreshold, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Get FMseekTreshold
 *   data return = 0..100
 */
int8_t DABDUINO::getFMseekTreshold(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x36, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Set FMstereoTreshold
 *   RSSItreshold = 0..100
 */
int8_t DABDUINO::setFMstereoTreshold(unsigned int RSSIstereoTreshold) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x01, 0x37, 0x00, 0x00, 0x01, RSSIstereoTreshold, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Get FMstereoTreshold
 *   data return = 0..100
 */
int8_t DABDUINO::getFMstereoTreshold(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x38, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *   Get FM Exact station
 *   data return: 0=current station is not exact frequency, 1=current station is exact frequency, 0xFE=no station information yet
 */
int8_t DABDUINO::getFMexactStation(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x39, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

// *************************
// ***** RTC ***************
// *************************

/*
 *  Set RTC clock
 *  year: 2017=17,2018=18, month: 1..12, day: 1..31, hour: 0..23, minute: 0..59, second: 0..59 
 */
int8_t DABDUINO::setRTCclock(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[14] = { 0xFE, 0x02, 0x00, 0x00, 0x00, 0x07, second, minute, hour, day, 0x00, month, year, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *  Get RTC ckock
 *  year: 2017=17,2018=18, month: 1..12, week: 0(sat)..6(fri), day: 1..31, hour: 0..23, minute: 0..59, second: 0..59 
 */
int8_t DABDUINO::getRTCclock(unsigned int *year, unsigned int *month, unsigned int *week, unsigned int *day, unsigned int *hour, unsigned int *minute, unsigned int *second) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x02, 0x01, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *second = (unsigned int)dabData[0];
      *minute = (unsigned int)dabData[1];
      *hour = (unsigned int)dabData[2];
      *day = (unsigned int)dabData[3];
      *week = (unsigned int)dabData[4];
      *month = (unsigned int)dabData[5];
      *year = (unsigned int)dabData[6];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *  Set RTC sync clock from stream enable
 */
int8_t DABDUINO::setRTCsyncEnable() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x02, 0x02, 0x00, 0x00, 0x01, 1, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *  Set RTC sync clock from stream disable
 */
int8_t DABDUINO::setRTCsyncDisable() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x02, 0x02, 0x00, 0x00, 0x01, 0, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *  Get RTC sync clock status
 *  return data: 0=disable, 1=enable 
 */
int8_t DABDUINO::getRTCsyncStatus(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x02, 0x03, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 *  Get RTC clock status
 *  return data: 0=unset, 1=set 
 */
int8_t DABDUINO::getRTCclockStatus(unsigned int *data) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x02, 0x04, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      *data = (unsigned int)dabData[0];
      return 1;
    }
    return 0;
  } else {
    return 0;
  }
}

// ********************************************
// ***** MOT (Multimedia Object Transfer) *****
// ********************************************




// *************************
// ***** NOTIFY ************
// *************************

/*
 *   Enable event notification
 */
int8_t DABDUINO::eventNotificationEnable() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[9] = { 0xFE, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x7F, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Disable event notification
 */
int8_t DABDUINO::eventNotificationDisable() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[9] = { 0xFE, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}









