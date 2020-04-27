//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control command to an Arduino RF-Nano and reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time

#define CE_PIN  10
#define CSN_PIN 9

const byte ChannelAddress[6] = {"Hemp1"};
RF24 radio(CE_PIN, CSN_PIN);

struct commandTemplate  //Max 32bytes. Template of the command sent to the Receiver. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   
   bool DisablePump1;   
   bool TurnOnPump1;   
   bool TurnOffPump1;   
   int TimeOutPump1;   
   float StartWeightBucket1;
   float StopWeightBucket1;
   
   bool DisablePump2;
   bool TurnOnPump2;
   bool TurnOffPump2;
   int TimeOutPump2;
   float StartWeightBucket2;
   float StopWeightBucket2;
};
struct commandTemplate FakeCommand = {1587936134,0,0,0,120,3.8,4.8,0,0,0,120,3.9,4.9};  //Fake commands sent to the Receiver


struct responseTemplate  //Max 32bytes. Template of the response back from the Receiver. Both Transmitter and Receiver needs to know this structure
{
   bool OnStatePump1; 
   bool EnabledStatePump1;
   float WeightBucket1;

   bool OnStatePump2;
   bool EnabledStatePump2; 
   float WeightBucket2;
   
   float Temp;
   float Humidity;
};
struct responseTemplate AckResponse; //The response from the Receiver will be stored here, represents the current status of the Receiver

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 15000; // send once per 15 seconds

void setup() {
    Serial.begin(115200);
    Serial.println(F("Sending and waiting for ACK"));
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.enableAckPayload();
    radio.setRetries(5,5); // delay, count. 5 gives a 1500 µsec delay which is needed for a 32 byte ackPayload
    radio.openWritingPipe(ChannelAddress);
    send();
}

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
    }
}

void send() {
    bool rslt;
    rslt = radio.write( &FakeCommand, sizeof(FakeCommand) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print(F("Data Sent."));
    if (rslt) {
        if ( radio.isAckPayloadAvailable() ) {
            radio.read(&AckResponse, sizeof(AckResponse));
            Serial.print(F(" Acknowledgement received[ "));            
            Serial.print(sizeof(AckResponse));
            Serial.println(F(" bytes]"));
            Serial.print(F("Bucket1: "));
            Serial.print(AckResponse.OnStatePump1);
            Serial.print(F(", "));
            Serial.print(AckResponse.EnabledStatePump1);
            Serial.print(F(", "));
            Serial.print(AckResponse.WeightBucket1);
            Serial.print(F(" ; Bucket2: "));
            Serial.print(AckResponse.OnStatePump2);
            Serial.print(F(", "));
            Serial.print(AckResponse.EnabledStatePump2);
            Serial.print(F(", "));
            Serial.print(AckResponse.WeightBucket2);
            Serial.print(F(" ; DHT: "));
            Serial.print(AckResponse.Temp);
            Serial.print(F(", "));
            Serial.print(AckResponse.Humidity);
            Serial.println();
            
            updateMessage();
        }
        else {
            Serial.println(F(" Acknowledgement received without any data."));
        }        
    }
    else {
        Serial.println(F(" No response."));
    }
    prevMillis = millis();
 }

void updateMessage() {        // so you can see that new data is being sent
    //FakeCommand.TurnOnPump1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exlusive!
    //FakeCommand.TurnOnPump2 = random(0,2);
    FakeCommand.StopWeightBucket1 = random(400, 500) / 100.0;
    FakeCommand.StopWeightBucket2 = random(400, 500) / 100.0;
}
