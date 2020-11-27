//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: connection between Arduino Nano (or RF-Nano) and nRF24L01+
//This program does NOT attempt to communicate with another wireless module

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

const byte CSN_PIN = 9;
const byte CE_PIN = 10;
const byte MOSI_PIN = 11;
const byte MISO_PIN = 12;
const byte SCK_PIN = 13;
const byte WirelessChannel[6] = {"Test1"};

RF24 radio(CE_PIN, CSN_PIN);

void setup()
{
    Serial.begin(115200);
    printf_begin();

    Serial.println(F("Sketch for testing: connection between Arduino Nano (or RF-Nano) and nRF24L01+"));
    Serial.println();
    Serial.println(F("DEFAULT SETTINGS after power on"));
    Serial.println(F("  Note that RF24 does NOT reset when Arduino resets - only when power is removed"));
    Serial.println(F("  If the numbers are mostly 0x00 or 0xff it means that the Arduino is not"));
    Serial.println(F("  communicating with the nRF24"));
    Serial.println();
    radio.begin();
    radio.printDetails();
    Serial.println();
    Serial.println();
    Serial.println(F("Setting data rate to 250KBPS..."));
    Serial.println();
    radio.openReadingPipe(1, WirelessChannel);
    radio.setDataRate(RF24_250KBPS);
    radio.printDetails();
    Serial.println();
    Serial.println();
    Serial.println(F("If the connection is OK the 'Data Rate' row should have changed from 1MBPS to 250KBPS"));
    Serial.println(F("If there is no change check the wiring and disconnect/reconnect the power source"));
}

void loop()
{
}
