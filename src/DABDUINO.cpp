/*
 *  DABDUINO.cpp - Library for DABDUINO / DAB+ - digital radio shield for Arduino.
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

void DABDUINO::initialization() {

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

  while (!isDabReady()) {
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
  unsigned int byteIndex = 0;
  unsigned int dataIndex = 0;
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
      if ((byteIndex - *eventDataSize) > 5 && serialData == 0xFD) {
        isPacketCompleted = 1;
        break;
      }
      byteIndex++;
    }
  }
  while (_Serial->available() > 0) {
    _Serial->read();
  }
  if (dabReturn[1] == 0x07 && isPacketCompleted == 1) {
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
  unsigned int byteIndex = 0;
  unsigned int dataIndex = 0;
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
      if ((byteIndex - *dabDataSize) >= 5 && serialData == 0xFD) {
        isPacketCompleted = 1;
        break;
      }
      byteIndex++;
    }
  }
  if (isPacketCompleted == 1 && !(dabReturn[1] == 0x00 && dabReturn[2] == 0x02)) {
    if (dataIndex) {
      dabData[dataIndex - 1] = 0x00;
    }
    return 1;
  } else {
    return 0;
  }
}


/*
 *   Reset DAB module with database
 */
int8_t DABDUINO::resetDabModule() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[8] = { 0xFE, 0x00, 0x01, 0x00, 0x00, 0x01, 1, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    initialization();
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Test for DAB module is ready for communication
 */
int8_t DABDUINO::isDabReady() {

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
 *   Enable event notification
 */
int8_t DABDUINO::enableEventNotification() {

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
int8_t DABDUINO::disableNotification() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[9] = { 0xFE, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 *   Radio module play status
 *   RETURN VALUE: 1=playing, 2=searching, 3=tuning, 4=stop, 5=sorting change, 6=reconfiguration
 */
int8_t DABDUINO::getPlayStatus() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x05, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize) {
      return dabData[0] + 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*
 *   Set audio output channels (SPDIV, I2S /DAC/)
 *   DABDUINO: I2S for analog output (cinch), SPDIV for optical digital output
 */
int8_t DABDUINO::setAudioOutputType(boolean spdiv, boolean i2s) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte s;
  if (spdiv && spdiv) {
    s = B00000011;
  } else if (spdiv && !i2s) {
    s = B00000001;
  } else if (!spdiv && i2s) {
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

/*
 *   Tune DAB program
 *   programIndex = 1..9999999 (see programs index)
 */
int8_t DABDUINO::tuneDabProgram(unsigned int programIndex) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  programIndex--;
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
 *   Set radio volume
 *   volumeLevel = 0..16
 */
int8_t DABDUINO::setVolume(byte volumeLevel) {

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
 * Search all DAB bands for programs
 */
int8_t DABDUINO::startSearchDabPrograms() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte chStart = 0;
  byte chEnd = 40; // 40=EU, 94=whole world
  byte dabCommand[9] = { 0xFE, 0x01, 0x03, 0x00, 0x00, 0x02, chStart, chEnd, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB stations index (number of programs in database)
 */
unsigned int DABDUINO::getDabProgramsIndex() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x16, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize == 4) {
      return (((long)dabData[0] << 24) + ((long)dabData[1] << 16) + ((long)dabData[2] << 8) + (long)dabData[3]);
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get DAB station index, get tuned FM station frequency
 */
unsigned int DABDUINO::getTunedIndex() {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x07, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    if (dabDataSize == 4) {
      return (((long)dabData[0] << 24) + ((long)dabData[1] << 16) + ((long)dabData[2] << 8) + (long)dabData[3]);
    }
    return 0;
  } else {
    return 0;
  }
}

/*
 * Get DAB station short name
 */
int8_t DABDUINO::getProgramShortName(unsigned int programIndex, char dabText[]) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x0F, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, programIndex - 1 , 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      dabText[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB station long name
 */
int8_t DABDUINO::getProgramLongName(unsigned int programIndex, char dabText[]) {

  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[12] = { 0xFE, 0x01, 0x0F, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, programIndex - 1 , 0x01, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      dabText[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    return 1;
  } else {
    return 0;
  }
}

/*
 * Get DAB text event
 */
int8_t DABDUINO::getProgrameText(char dabText[]) {

  char dabTextLast[DAB_MAX_TEXT_LENGTH];
  memcpy(dabTextLast, dabText, sizeof(dabText[0])*DAB_MAX_TEXT_LENGTH);
  byte dabData[DAB_MAX_DATA_LENGTH];
  unsigned int dabDataSize;
  byte dabCommand[7] = { 0xFE, 0x01, 0x10, 0x00, 0x00, 0x00, 0xFD };
  if (sendCommand(dabCommand, dabData, &dabDataSize)) {
    int j = 0;
    for (unsigned int i = 0; i < dabDataSize; i = i + 2) {
      dabText[j++] = (char)charToAscii(dabData[i], dabData[i + 1]);
    }
    for (unsigned int i = 0; i < j; i++) {
      if (dabText[i] != dabTextLast[i]) {
        return 1; // New dab text
      }
    }
    return 2; // Same dab text
  } else {
    return 0;
  }
}

/*
 * Tune FM program - frequency: 87500..108000 MHz
 */
int8_t DABDUINO::tuneFmProgram(long frequency) {

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
 * Seek FM program - searchDirection: 0=backward, 1=forward
 */
int8_t DABDUINO::seekFmProgram(byte searchDirection) {

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




