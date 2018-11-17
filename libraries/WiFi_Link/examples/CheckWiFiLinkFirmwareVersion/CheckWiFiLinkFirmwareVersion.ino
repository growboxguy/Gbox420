/*
 * This example check if the WiFi Link firmware is updated.
 *
 * Created 29 July 2015 by Cristian Maglie
 * This code is in the public domain.
 *
 * Modified and adapted 10 March 2017 by Sergio Tomasello
 * Modified and adapted Nov 2017 by Juraj Andrássy
 *
 */

#include <WiFiLink.h>
//#include <UnoWiFiDevEdSerial1.h> // change Serial1.begin to 115200

#if !defined(ESP_CH_SPI) && !defined(HAVE_HWSERIAL1)
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif


void setup() {
  // Initialize serial
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

#if !defined(ESP_CH_SPI)
  Serial1.begin(9600); // speed must match with BAUDRATE_COMMUNICATION setting in firmware config.h
  WiFi.init(&Serial1);
#endif

  // Print a welcome message
  Serial.println("WiFi Link firmware check.");
  Serial.println();

  //Check if communication with the wifi module has been established
  if (WiFi.status() == WL_NO_WIFI_MODULE_COMM) {
    Serial.println("Communication with WiFi module not established.");
    while (true); // don't continue:
  }

  // Print firmware version on the shield
  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  // Print required firmware version
  Serial.print("Firmware version required : ");
  Serial.println(WIFI_FIRMWARE_REQUIRED);

  // Check if the required version is installed
  Serial.println();
  if (WiFi.checkFirmwareVersion(WIFI_FIRMWARE_REQUIRED)) {
    Serial.println("Check result: PASSED");
  } else {
    Serial.println("Check result: NOT PASSED");
    Serial.println(" - The firmware version installed do not match the");
    Serial.println("   version required by the library, you may experience");
    Serial.println("   issues or failures.");
  }
}

void loop() {
  // do nothing
}
