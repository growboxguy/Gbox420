/*
 * EspRecovery.ino
 *
 * This example works as a serial bridge between microntroller and wifi module
 * (esp8266). It's very useful when upload firmware updates to the wifi module.
 * It must be used only on boards where the connection between micontroller and
 * wifi module is over serial, not spi.
 *
 * Note: this is a port of EspRecovery of the Arduino Uno WiFi Dev Ed Library.
 *
 * Circuit:
 * - Arduino STAR OTTO or Uno WiFi Developer Edition
 *
 * Modified 15 March 2017 by Andrea Cannistrá and Sergio Tomasello
 *
 */

 /*
  Define Baud Rate between esp8266 and the microntroller
 */

#if defined(ARDUINO_STM32_STAR_OTTO) // Arduino STAR OTTO
#define BR 230400
#define Serial SerialUSB
#endif

#include <WiFiLink.h>

void setup()
{

  #if defined(ARDUINO_STM32_STAR_OTTO)
    pinMode(WIFI_PWR, OUTPUT);
    digitalWrite(WIFI_PWR, LOW);    //turn OFF ESP
    delay(100);
    digitalWrite(WIFI_PWR, HIGH);   //turn ON ESP
  #endif

  Serial.begin(BR);
  SerialWiFi.begin(BR);
}

void loop()
{

  while (Serial.available()) {
    char inChar = Serial.read();
    SerialWiFi.write(inChar);
  }

  while (SerialWiFi.available()) {
    char inChar = SerialWiFi.read();
    Serial.write(inChar);
  }

}
