## A new DABDUINO is coming

DABDUINO comes back. We develop a new DABDUINO: a complete DAB+, FM and
internet radio board for makers, with an ESP32-S3 microcontroller on the
board. It plays without other boards, and other microcontroller boards can
also control it. These are the planned functions:

**Radio**

* DAB, DAB+ and FM with RDS. DAB and DAB+ in Band III.
* Station names and text information (DLS), and station pictures (slideshow)
  on a display
* Internet radio through Wi-Fi (MP3 and AAC streams)
* An indoor antenna or an outdoor antenna on the roof, for example a Yagi
  antenna on a 75 Ohm cable. The firmware adjusts the antenna input to your
  antenna.
* ESD protection on the antenna input
* A connector for an external Wi-Fi antenna, for a good Wi-Fi signal also in
  a metal case

**Control**

* From your phone or from a web browser through Wi-Fi, with a web interface
  in the firmware
* From your phone through Bluetooth LE (control only, no audio)
* From a smart home system, for example Home Assistant (MQTT), also as a
  player for the audio of Home Assistant
* A rotary encoder with a push button, 7 buttons and an IR receiver for a
  remote control
* The controls header also has I2C and 3.3 V, for example for a light sensor
  for automatic dimming of the display
* Clock and alarm, with the time from DAB or from the internet. A real-time
  clock on the board keeps the time when the power goes. With an optional
  supercapacitor, it keeps the time for some days without power.

**Display**

* A display header with SPI for the picture and I2C, for example for a touch
  panel
* Dimming of the display with PWM, for example at night or in standby. A
  logic output for displays with a backlight driver, and a transistor output
  for displays with no backlight driver.
* The firmware controls the reset of the display
* The board switches off the supply of the display when the radio is off

**Sound**

* Line output and an SPDIF output: coaxial, or optical with a Toslink module
* High sound quality: each supply of the DAC has its own low-noise
  regulator, and the output filter has film capacitors
* No click in the speakers when you disconnect the power: the board detects
  the power loss and mutes the DAC before the supply stops
* Volume control in the firmware
* An expansion connector for audio modules: you select a headphone amplifier
  or a speaker amplifier for your project, to your preferences
* Or a Bluetooth module on the same connector, which sends the audio to
  Bluetooth headphones and speakers

**Power**

* Only a regulated 5 V supply. USB-C does not supply power: it is only for
  programming
* Standby to save energy: the firmware switches off the radio receiver, the
  DAC and the supply of the modules on the headers. The ESP32-S3 can go to
  deep sleep.
* A button, the rotary encoder or the alarm of the clock switches the radio
  on again. The remote control can also do this when the ESP32-S3 is not in
  deep sleep.
* Protection against a supply with the wrong polarity. When the power goes,
  the board puts the radio receiver into reset before the supply voltages
  change.

**For makers**

* Write your own firmware for the ESP32-S3 with Arduino or ESP-IDF. An open
  source DABDUINO library controls the radio and the other parts of the
  board.
* Or use the board as a radio for Arduino, STM32, Raspberry Pi and other
  boards, through UART with an interrupt line and a reset line, with any
  logic voltage from 1.8 V to 5 V
* All connections are on 2.54 mm pin headers: display, controls, IR
  receiver, audio modules, line output, SPDIF and host board
* Firmware updates through Wi-Fi (OTA) or USB-C
* Open hardware and open source firmware. The firmware of the radio receiver
  is installed in the flash memory of the board.

Open source projects can add more functions.

The new DABDUINO is in development. Information and news:
https://www.dabduino.com

Tell us which functions you want: open an issue in this repository, or send
an email to turbyho@gmail.com.

The text below describes the original DABDUINO (2017).

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
Currently unavailable 
<!-- DABDUINO is ready for dispatch. Price is 69 EUR. Postage for Europe 6 EUR, for rest of the world 10 EUR. Send me order by [email](mailto:turbyho@gmail.com) please.-->

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

[Montyho technology s.r.o.](https://www.montyho.com)
