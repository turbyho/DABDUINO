## What is the DABDUINO?
The DABDUINO is Arduino DAB/DAB+ (digital radio) + FM shield based on Keystone T3B module and with 32-bit, 384kHz PCM DAC (cinch) + SPDIF optical digital audio output. The DABDUINO shield is compatible with 3.3V boards (Zero, M0, M0 PRO, Due etc.). DABDUINO Library is designed specifically to work with the DABDUINO and works too with Keystone T3B module. 

## How to install
Open library manager in Arduino IDE and install the DABDUINO library.

## Where buy the DABDUINO?
I waiting for first batch of DABDUINO boards. First boards will be ready for dispatch on 05/2017. Send me preorders please.

### Second prototype video
[<img src="https://img.youtube.com/vi/LBgsKTtB7Bs/0.jpg">](https://www.youtube.com/watch?v=LBgsKTtB7Bs)

### First prototype video
[<img src="https://img.youtube.com/vi/Ivv96sOVvz4/0.jpg">](https://www.youtube.com/watch?v=Ivv96sOVvz4)

### Pinout
![DABDUINO PINOUT](doc/dabduino_pinout.png)

## References

// *************************
// ***** SYSETEM ***********
// *************************

int8_t reset();
int8_t resetCleanDB();
int8_t isReady();
int8_t setAudioOutput(boolean spdiv, boolean cinch);
int8_t isEvent();
int8_t readEvent(byte eventData[], unsigned int *eventDataSize);

// *************************
// ***** STREAM ************
// *************************

int8_t playDAB(unsigned int programIndex);
int8_t playFM(unsigned int frequency);
int8_t playBEEP();
int8_t playSTOP();
int8_t searchDAB(unsigned int band);
int8_t searchFM(unsigned int seekDirection);
int8_t playStatus(unsigned int data);
int8_t playMode(unsigned int data);
int8_t getPlayIndex(unsigned int data);
int8_t getSignalStrength(unsigned int signalStrength, unsigned int bitErrorRate);
int8_t setStereoMode(boolean stereo);
int8_t getStereoMode(unsigned int data);
int8_t getStereoType(unsigned int data);
int8_t setVolume(unsigned int volumeLevel);
int8_t getVolume(unsigned int data);
int8_t getProgramType(unsigned int data);
int8_t getProgramShortName(unsigned int programIndex, char text[]);
int8_t getProgramLongName(unsigned int programIndex, char text[]);
int8_t getProgramText(char text[]);
int8_t getSamplingRate(unsigned int data);
int8_t getDataRate(unsigned int data);
int8_t getSignalQuality(unsigned int data);
int8_t getFrequency(unsigned int programIndex, unsigned int data);
int8_t getEnsembleShortName(unsigned int programIndex, char text[]);
int8_t getEnsembleLongName(unsigned int programIndex, char text[]);
int8_t getProgramIndex(unsigned int data);
int8_t isProgramOnAir(unsigned int programIndex);
int8_t getServiceShortName(unsigned int programIndex, char text[]);
int8_t getServiceLongName(unsigned int programIndex, char text[]);
int8_t getSearchIndex(unsigned int data);
int8_t getServCompType(unsigned int programIndex, unsigned int data);
int8_t setPreset(unsigned int programIndex, unsigned int presetIndex, unsigned int presetMode);
int8_t getPreset(unsigned int presetIndex, unsigned int presetMode);
int8_t getProgramInfo(unsigned int programIndex, unsigned int serviceId, unsigned int ensembleId);
int8_t getProgramSorter(unsigned int data);
int8_t setProgramSorter(unsigned int sortMethod);
int8_t getDRC(unsigned int data);
int8_t setDRC(unsigned int setDRC);
int8_t prunePrograms(unsigned int prunedTotalPrograms, unsigned int prunedProgramIndex);
int8_t getECC(unsigned int ECC, unsigned int countryId);
int8_t getRdsPIcode(unsigned int data);
int8_t setFMstereoThdLevel(unsigned int RSSItresholdLevel);
int8_t getFMstereoThdLevel(unsigned int data);
int8_t getRDSrawData(unsigned int RDSblockA, unsigned int RDSblockB, unsigned int RDSblockC, unsigned int RDSblockD, unsigned int BlerA, unsigned int BlerB, unsigned int BlerC, unsigned int BlerD);
int8_t setFMseekTreshold(unsigned int RSSItreshold);
int8_t getFMseekTreshold(unsigned int data);
int8_t setFMstereoTreshold(unsigned int RSSIstereoTreshold);
int8_t getFMstereoTreshold(unsigned int data);
int8_t getFMexactStation(unsigned int data);

// *************************
// ***** RTC ***************
// *************************

int8_t setRTCclock(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);
int8_t getRTCclock(unsigned int year, unsigned int month, unsigned int week, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);
int8_t setRTCsyncEnable();
int8_t setRTCsyncDisable();
int8_t getRTCsysncStatus(unsigned int data);
int8_t getRTCclockStatus(unsigned int data);

// ********************************************
// ***** MOT (Multimedia Object Transfer) *****
// ********************************************



// *************************
// ***** NOTIFY ************
// *************************

int8_t enableEventNotification();
int8_t disableNotification();

