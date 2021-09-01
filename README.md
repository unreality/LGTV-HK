LG TV Controller for HomeKit
----------
Flash to an [M5Atom RS232 Kit](https://shop.m5stack.com/collections/atom-series/products/atom-rs232-kit), and add your LG TV to your HomeKit home. Should work with most other ESP32 kits as well.

Quick Start
-----------

Flash the M5Atom:

* Download the latest release
* Download [NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher)
* Plug in your [M5Atom RS232 Kit](https://shop.m5stack.com/collections/atom-series/products/atom-rs232-kit)
* Launch NodeMCU PyFlasher
  * Set your serial port
  * Select the downloaded .bin file as your NodeMCU firmware
  * Select 'Dual I/O (DIO)' as Flash mode
  * Erase flash to 'yes'
* Click 'Flash NodeMCU'

Connect to your TV:
* Purchase a DB9 cable, trim one end, and connect the pins such that:
  * PIN 2 -> TX
  * PIN 3 -> RX
  * PIN 5 -> GND
* You'll need a USB power supply since the TV does not supply 12v

Configure the M5Atom:
* Power on the M5Atom
* Hold down the button for 3 seconds until the LED flashes cyan quickly
* Press the button 3 times, the LED should triple-blink
* Hold the button for 3 seconds again to enter AP configuration mode
* The LED should flash yellow
* Connect to the 'LGTV XXXXXX' WiFi access point that should now be available and enter your WiFi details and setup code

Connect to HomeKit:
* Open Home.app on your iDevice
* Add new Accessory
* Press "My device isnt listed or cannot scan"
* Select the LGTV
* Enter the setup code entered earlier
* Set room/automation options
* You can now control tv power/inputs from Home.app

Serial Configuration
--------------------
You can configure the WiFi settings and Setup Pin from a serial connection
* Open Serial Monitor
* Enter 'S <HOMEKIT PIN>' to define the HomeKit pairing pin (eg 'S 11122333')
* Enter 'W' to enter WiFi connection details
* Enter ? for list of all commands

Building Manually
-----------------

* Clone the repository
* Open repository in PlatformIO
* Build and Upload

Status LED
----------
- Yellow: 
  - Pulsing: WiFi Details Required
  - Flashing: WiFi Connecting
- Magenta:
  - Pulsing: Waiting to pair with HomeKit
- Green: 
  - Solid: Connected and ready to recieve HomeKit commands

QR Code
-------

You can generate a QR code to scan with Home.app by using the [QR code generator tool](https://github.com/maximkulkin/esp-homekit/tree/master/tools)

```#  python3 gen_qrcode 31 111-22-333 HSPN output.png```

Alternatively, you can use an online QR code generator such as [the-qrcode-generator.com](https://www.the-qrcode-generator.com/), just enter the generated setup payload which is printed after you enter your custom pairing pin (eg `X-HM://00KUG4GWTHSPN`)

Other ESP Boards
----------------

This controller should work fine on other ESP boards, with the following setup:
- GPIO 19 - UART RX (will need to be converted to 5v logic level)
- GPIO 22 - UART TX (will need to be converted to 5v logic level)
- GPIO 27 - NeoPixel Compatible LED (SK6812/WS2812C/etc)



