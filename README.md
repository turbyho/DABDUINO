## What is the DABDUINO?

The DABDUINO is good starting point for high quality DIY DAB/DAB+/FM home receiver project. Optical audio outout (SPDIF) and 32-bit, 384kHz PCM DAC offer the same audio quality as expensive DAB/DAB+ receivers.

The DABDUINO is Arduino DAB/DAB+ (digital radio) + FM shield based on Keystone T3B module and with 32-bit, 384kHz PCM DAC (cinch) + SPDIF optical digital audio output. The DABDUINO shield is compatible with 3.3V boards (Zero, M0, M0 PRO, Due etc.). DABDUINO Library is designed specifically to work with the DABDUINO and works too with Keystone T3B module.

The library support many function for control DABDUINO and for access to broadcast data. For example - automatic station search and store stations in database, watching and reading DAB/DAB+ and FM station name, program type, information texts and data from DAB/DAB+/FM RDS, signal strength and quality, volume and stereo mode settings and many others. In near future I will add function for access to media data from DAB/DAB+ stream. For example - images, audio stream, etc. 

**Bands:**
* DAB/DAB+: BAND 3, CHINA BAND, L-BAND
* FM: 87.5 - 108MHz

**Input:**
* RF (antenna): SMA connector, solder pads

**Output:**
* Analog audio: 32-bit, 384kHz PCM DAC - stereo cinch (golden), solder pads
* Digital audio: SPDIF (optical)

**Real Time Clock (RTC):**
* on board, synchronised from DAB/DAB+ stream 

**Power:**
* DABDUINO is powered only from Arduino pin heads. No need external power source. 

## How to install
Open library manager in Arduino IDE and install the DABDUINO library.

## References
For command reference visit [DABDUINO.cpp](https://github.com/turbyho/DABDUINO/blob/master/src/DABDUINO.cpp). 
Example is here [DABDUINO_example_1.ino](https://github.com/turbyho/DABDUINO/blob/master/examples/Dabduino_example_1/DABDUINO_example_1.ino).

## Have some problem or questions?
Send me [email](mailto:turbyho@gmail.com) :)

## Where to buy the DABDUINO?
I waiting for first batch of DABDUINO boards. First boards will be ready for dispatch in 05/2017. Price is 69 EUR. Postage for Europe 6 EUR, for outside Europe 10 EUR. Send me preorder by [email](mailto:turbyho@gmail.com) please.

## Final DABDUINO
<img src="doc/dabduino_01.jpg">
<img src="doc/dabduino_02.jpg">

### Second prototype video
[<img src="https://img.youtube.com/vi/LBgsKTtB7Bs/0.jpg">](https://www.youtube.com/watch?v=LBgsKTtB7Bs)

### First prototype video
[<img src="https://img.youtube.com/vi/Ivv96sOVvz4/0.jpg">](https://www.youtube.com/watch?v=Ivv96sOVvz4)

### Pinout
![DABDUINO PINOUT](doc/dabduino_pinout.png)
