LG TV Controller for HomeKit
----------
Flash to an [M5Atom RS232 Kit](https://shop.m5stack.com/collections/atom-series/products/atom-rs232-kit), and add your LG TV to your HomeKit home. Should work with most other ESP32 kits as well.

Quick start
-----------

* Clone the repository
* Open repository in PlatformIO
* Build and Upload
* Open Serial Monitor
* Enter 'S <HOMEKIT PIN>' to define the HomeKit pairing pin (eg 'S 11122333')
* Enter 'W' to enter WiFi connection details
* Connect the M5Atom to the TV via the RS232 connector
  * Check your TVs manual for the pin configuration (eg pg.27 in [this manual](https://www.lg.com/us/business/download/resources/BT00001837/UV340C-U_Install_Manual_170825.pdf) )

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



