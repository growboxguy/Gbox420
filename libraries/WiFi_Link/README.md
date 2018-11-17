# WiFi Link Library 

This is a fork of Arduino.org WiFi Link library. The goal of this fork is make improvements and provide documentation.

WiFi Link was developed for the Arduino Star Otto, Arduino Primo and [Uno WiFi Developer Edition](https://github.com/jandrassy/UnoWiFiDevEdSerial1). With changes in this fork, it can be used with generic esp8266 in combination with an Arduino MCU.

Use WiFi link library with [WiFi Link firmware](https://github.com/jandrassy/arduino-firmware-wifilink) in ESP8266.

WiFi Link is the best general networking library for Arduino using esp8266 connected with Serial as WiFi network adapter. API of the WiFi Link library is almost the same as for the Ethernet, WiFi, WiFiEsp libraries. It is easy to port sketches and examples.

WiFi Link library implements the Arduino classes Server and Client which makes it compatible with libraries created upon Ethernet or WiFi library like [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient.) library, [MQTT PubSubClient](https://github.com/knolleary/pubsubclient) or [Blynk](https://github.com/jandrassy/arduino-library-wifilink/tree/master/examples/WiFiLinkBlynk).

WiFi Link can be compiled to [support SPI connection](https://github.com/jandrassy/arduino-firmware-wifilink#spi-connection) of esp8266 to MCU. It is used on Primo board.

## Install this library

Older version of WiFi Link library is available in Library Manager. It is usable only for Primo, Otto and Uno WiFi. For use with generic esp8266 and Uno WiFi Dev Ed Serial1, you must download the library from this repository.

Download the contents of this GitHub repository as ZIP using the green 'Clone or download' button. Extract the ZIP and copy the folder arduino-library-wifilink-master into your Arduino libraries folder (a subfolder in the folder where sketches are saved by IDE). Rename the folder to WiFi_Link. Reopen the IDE.

## WiFi initialization

WiFi Link can be used with ESP8266 connected over some serial interface or over SPI.

To initialize WiFi Link library to comunicate with WiFi Link firmware over serial interface call WiFi.init() with some Stream implementation like SoftwareSerial, Serial1, Serial3. Without init() call the library uses Serial for communication or SPI if XY.build.extra_flags=-DESP_CH_SPI is  in boards.txt for Arduino board.

## Reseting the ESP

If the sketch is restarted after upload or with Serial Monitor, the ESP is not restarted like after power-up and the previous connections remain open. Since the count of open connections is limited, it is better to restart the ESP with WiFi.resetESP() in sketch setup(). After the reset of ESP put a 500 ms delay to let the ESP and firmware start.

Better then using the software reset WiFi.resetESP(), is wire the ESP reset to an Arduino pin or ensuring a power cycle of the ESP. The software reset can't reset the ESP if the firmware doesn't response.

## Connecting to WiFi network

With WiFi Link library you can connect to a network calling WiFi.begin(), but better is let the firmware use the settings you made in WiFi Link firmware's Web Panel.

Connecting to a WiFi network takes time. To wait for the connection test WiFi.state() for WL_CONNECTED. If the ESP should connect to WiFi with Web Panel setting, put a 3000 ms delay before testing for WL_CONNECTED.

## Examples

After installing the library, you can find the examples on the usual place in IDE menu.

The example sketch assumes use of Serial1 if ESP_CH_SPI is not defined. If your board doesn't have a Serial1 interface then the example creates a SoftwareSerial Serial1. For use with Serial or with Serial2/3 on Mega, modify the example. 
