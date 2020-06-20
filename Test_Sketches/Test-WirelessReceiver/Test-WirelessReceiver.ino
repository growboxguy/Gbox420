//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Receiving a control message using Arduino Nano and nRF24L01+ (or RF-Nano) and Acknowledging it with a response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time

//Ports for Arduino Nano or RF-Nano
const byte CE_PIN = 10;
const byte CSN_PIN = 9;

const byte WirelessChannel[6] ={"Test1"};  //Identifies the communication channel, needs to match on the Transmitter
RF24 radio(CE_PIN, CSN_PIN);

struct commandTemplate  //Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   time_t Time;
   
   bool DisablePump1_B1;   
   bool TurnOnPump_B1;   
   bool TurnOffPump_B1;   
   int TimeOutPump_B1;   
   float StartWeightBucket_B1;
   float StopWeightBucket_B1;
   
   bool DisablePump_B2;
   bool TurnOnPump_B2;
   bool TurnOffPump_B2;
   int TimeOutPump_B2;
   float StartWeightBucket_B2;
   float StopWeightBucket_B2;
};
struct commandTemplate ReceivedCommand;  //Variable where the command values received from the Transmitter will get stored

struct responseTemplate  //Max 32bytes. Template of the response sent back by the Receiver. Both Transmitter and Receiver needs to know this structure
{
   bool PumpOn_B1; 
   bool PumpEnabled_B1;
   float Weight_B1;

   bool PumpOn_B2;
   bool PumpEnabled_B2; 
   float Weight_B2;
   
   float Temp;
   float Humidity;
};
struct responseTemplate Response = {1,1,4.20,0,0,1.23,23.4,50.1};  //Fake response sent back in the Acknowledgement after receiving a command

void setup() {
    Serial.begin(115200);
    Serial.println(F("Setting up the wireless receiver..."));
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, WirelessChannel);
    radio.enableAckPayload();
    updateReplyData();
    radio.startListening();
    Serial.println(F("Listening..."));
}

void loop() {
    if ( radio.available() ) {  //When a command is received
        radio.read( &ReceivedCommand, sizeof(ReceivedCommand) );    //Load the command to the ReceivedCommand variable
        Serial.print(F("Command received ["));
        Serial.print(sizeof(ReceivedCommand));       
        Serial.println(F(" bytes]"));
        Serial.print(F("  Bucket1: "));
        Serial.print(ReceivedCommand.DisablePump1_B1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOnPump_B1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOffPump_B1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TimeOutPump_B1);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.StartWeightBucket_B1);
        Serial.print(F(", "));
        Serial.println(ReceivedCommand.StopWeightBucket_B1);
        Serial.print(F("  Bucket2: "));
        Serial.print(ReceivedCommand.DisablePump_B2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOnPump_B2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TurnOffPump_B2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.TimeOutPump_B2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.StartWeightBucket_B2);
        Serial.print(F(", "));
        Serial.print(ReceivedCommand.StopWeightBucket_B2);       
        Serial.println();
        
        updateReplyData();
    }
}

void updateReplyData() { // so you can see that new data is being sent
    Response.Weight_B1 = random(400, 500) / 100.0;
    Response.Weight_B2 = random(400, 500) / 100.0;
    Response.Humidity = random(0, 10000) /100;
    radio.writeAckPayload(1, &Response, sizeof(Response)); // load the payload to get sent out when the next control message is received
}
