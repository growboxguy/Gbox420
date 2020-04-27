
// 18 Mar 2018 - simple program to verify connection between Arduino Mega and nRF24L01+
//  This program does NOT attempt any communication with another nRF24

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

#define CSN_PIN 49
#define MISO_PIN  50
#define MOSI_PIN  51
#define SCK_PIN  52
#define CE_PIN  53


const byte ChannelAddress[6] = {"Hemp1"};

RF24 radio(CE_PIN, CSN_PIN);

void setup() {
    Serial.begin(115200);
    printf_begin();

    Serial.println("CheckConnection Starting");
    Serial.println();
    Serial.println("FIRST WITH THE DEFAULT ADDRESSES after power on");
    Serial.println("  Note that RF24 does NOT reset when Arduino resets - only when power is removed");
    Serial.println("  If the numbers are mostly 0x00 or 0xff it means that the Arduino is not");
    Serial.println("     communicating with the nRF24");
    Serial.println();
    radio.begin();
    radio.printDetails();
    Serial.println();
    Serial.println();
    Serial.println("AND NOW WITH NEW CHANNEL ADDRESS");
    Serial.println(" and 250KBPS data rate");
    Serial.println();
    radio.openReadingPipe(1, ChannelAddress);
    radio.setDataRate( RF24_250KBPS );
    radio.printDetails();

}


void loop() {

}
