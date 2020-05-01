//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control command to an Arduino RF-Nano and reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time

/*
#define CE_PIN  10
#define CSN_PIN 9
*/
#define CE_PIN  53
#define CSN_PIN 49

const byte ChannelAddress[6] = {"Hemp1"};
RF24 Wireless(CE_PIN, CSN_PIN);

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
    Wireless.begin();
    Wireless.setDataRate( RF24_250KBPS );
    Wireless.enableAckPayload();  ///< Enable custom payloads on the acknowledge packets. Ack payloads are a handy way to return data back to senders without manually changing the radio modes on both units.
    Wireless.setRetries(5,5); // Parameters: delay [R], count [How many retries before giving up, max 15] 
    Wireless.openWritingPipe(ChannelAddress);
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
    rslt = Wireless.write( &FakeCommand, sizeof(FakeCommand) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print(F("Data Sent."));
    if (rslt) {
        if ( Wireless.isAckPayloadAvailable() ) {
            Wireless.read(&AckResponse, sizeof(AckResponse));
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
