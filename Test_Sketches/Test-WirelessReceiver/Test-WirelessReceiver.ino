//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Receiving a control command using Arduino RF-Nano and Acknowledging it with a custom message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time

#define CE_PIN  10
#define CSN_PIN 9

const byte ChannelAddress[6] = {"Hemp1"};
RF24 radio(CE_PIN, CSN_PIN);

struct commandTemplate  //Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
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
struct commandTemplate ReceivedCommand;  //Variable where the actual command values will get stored

struct responseTemplate  //Max 32bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
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
struct responseTemplate FakeResponse = {1,1,4.20,0,0,1.23,23.4,50.1};  //Fake response sent back in the Acknowledgement after receiving a command from the Transmitter

void setup() {
    Serial.begin(115200);
    Serial.println(F("Listening..."));
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, ChannelAddress);
    radio.enableAckPayload();
    updateReplyData();
    radio.startListening();
}

void loop() {
    getData();
}

void getData() {
    if ( radio.available() ) {
        radio.read( &ReceivedCommand, sizeof(ReceivedCommand) );        
        Serial.print(F("Command received ["));
        Serial.print(sizeof(ReceivedCommand));       
        Serial.println(F(" bytes]"));
        Serial.print(F("  Bucket1: "));
        Serial.print(ReceivedCommand.DisablePump1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOnPump1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOffPump1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TimeOutPump1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.StartWeightBucket1);
        Serial.print(F(", "));
        Serial.println(ReceivedCommand.StopWeightBucket1);
        Serial.print(F("  Bucket2: "));
        Serial.print(ReceivedCommand.DisablePump2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOnPump2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOffPump2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TimeOutPump2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.StartWeightBucket2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.StopWeightBucket2);       
        Serial.println();
        
        updateReplyData();
    }
}

void updateReplyData() { // so you can see that new data is being sent
    FakeResponse.WeightBucket1 = random(400, 500) / 100.0;
    FakeResponse.WeightBucket2 = random(400, 500) / 100.0;
    radio.writeAckPayload(1, &FakeResponse, sizeof(FakeResponse)); // load the payload for the next time
}
