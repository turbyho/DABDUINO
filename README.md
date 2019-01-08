## What is the DABDUINO?

The DABDUINO is good starting point for high quality DIY DAB/DAB+/FM home receiver project, car receiver or anything you want. Digital optical audio outout (Toslink) and 32-bit, 384kHz PCM DAC offers the same audio quality as expensive DAB/DAB+ receivers.

The DABDUINO is Arduino DAB/DAB+ (digital radio) + FM shield with 32-bit, 384kHz PCM DAC (cinch) + Toslink optical digital audio output. DABDUINO Library is designed specifically to work with the DABDUINO.

The library supports many functions for control DABDUINO and for access to broadcast data. For example - automatic station search and store stations in database, watching and reading DAB/DAB+ and FM station name, program type, information texts and data from DAB/DAB+/FM RDS, signal strength and quality, volume and stereo mode settings and many others. In near future I will add functions for access to media data from DAB/DAB+ stream. For example - images, etc. 

**Compatibility:**
* Arduino (as shield): DUE, ZERO, M0, M0 PRO
* Arduino (wired): MKRZero, MRK1000 

> _Generally all Arduino 3.3V boards (Arduino 5V boards only with level shifting)_

* STM32 (as shield): Nucleo L476RG, Nucleo F091RC, Nucleo F103RB
* STM32 (wired): STM32F103C8T6 

> _Generally all supported boards in [stm32duino](https://github.com/stm32duino/Arduino_Core_STM32) and [Arduino_STM32](https://github.com/rogerclarkmelbourne/Arduino_STM32)_

> _SPI is not needed for common operation (only for access to media data)_

**Bands:**
* DAB/DAB+: BAND III, CHINA BAND, L-BAND
* FM: 87.5 - 108MHz

**Input:**
* RF (antenna): SMA connector, solder pads

**Output:**
* Analog audio: 32-bit, 384kHz PCM DAC - stereo cinch (golden), solder pads
* Digital audio: Toslink (optical)

**Real Time Clock (RTC):**
* on board, synchronised from DAB/DAB+ stream 

**Power:**
* DABDUINO is powered only from Arduino pin heads. No need external power source. 

## How to connect (without pin headers)
Minimal wiring is ANTENNA (use correct band), TX, RX, (DABDUINO) RESET, GND, +5V.

## How to install
Open library manager in Arduino IDE and install the DABDUINO library.

## References
For command reference visit [DABDUINO.cpp](https://github.com/turbyho/DABDUINO/blob/master/src/DABDUINO.cpp). 
Example is available here [DABDUINO_example_1.ino](https://github.com/turbyho/DABDUINO/blob/master/examples/Dabduino_example_1/DABDUINO_example_1.ino).

## Where to buy the DABDUINO?
DABDUINO is ready for dispatch. Price is 69 EUR. Postage for Europe 6 EUR, for rest of the world 10 EUR. Send me order by [email](mailto:turbyho@gmail.com) please.

Dabduino is dispatched without pin headers and SMA for easier installation in custom devices.

## Have some problem or questions?
Send me [email](mailto:turbyho@gmail.com) :)

## Final DABDUINO
<img src="doc/dabduino_01.jpg">
<img src="doc/dabduino_02.jpg">

### Second prototype video
[<img src="https://img.youtube.com/vi/LBgsKTtB7Bs/0.jpg">](https://www.youtube.com/watch?v=LBgsKTtB7Bs)

### First prototype video
[<img src="https://img.youtube.com/vi/Ivv96sOVvz4/0.jpg">](https://www.youtube.com/watch?v=Ivv96sOVvz4)

### Pinout
![DABDUINO PINOUT](doc/dabduino_pinout_v1.0.png)
