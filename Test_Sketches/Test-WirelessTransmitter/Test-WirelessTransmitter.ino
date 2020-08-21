//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control message from an Arduino Mega and nRF24L01+, then reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time

//Ports for Arduino Nano or RF-Nano
const byte CE_PIN = 10;
const byte CSN_PIN = 9;

/*
//Ports for Arduino Mega
const byte CSN_PIN = 49;
const byte CE_PIN = 53;
*/

const uint8_t WirelessChannel[6] ={"Test1"}; //Identifies the communication channel, needs to match on the Receiver
RF24 Wireless(CE_PIN, CSN_PIN);

//Variables and constants for timing Control messages 
unsigned long LastMessageSent = 0;  //When was the last message sent
const unsigned long MessageInterval = 15000; // send a control message once per 15 seconds
const uint8_t RetryDelay = 5; //How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
const uint8_t RetryCount = 5; //How many retries before giving up, max 15

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
struct commandTemplate Command = {1587936134,0,0,0,120,3.8,4.8,0,0,0,120,3.9,4.9};  //Fake commands sent to the Receiver

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
struct responseTemplate ReceivedResponse; //The response from the Receiver will be stored here, represents the current status of the Receiver

void setup() {
    Serial.begin(115200);
    Serial.println(F("Setting up the wireless transmitter..."));
    Wireless.begin();
    Wireless.setDataRate( RF24_250KBPS );
    Wireless.enableAckPayload();  ///< Enable custom payloads on the acknowledge packets. Ack payloads are a handy way to return data back to senders without manually changing the radio modes on both units.
    Wireless.setRetries(RetryDelay,RetryCount); 
    Wireless.openWritingPipe(WirelessChannel); 
    sendCommand();   
}

void loop() {
    if (millis() - LastMessageSent >= MessageInterval) //Check if it is time to send a command
    {
      sendCommand();
    }
}

void updateCommand() {        // so you can see that new data is being sent
    //Command.TurnOnPump_B1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exlusive!
    //Command.TurnOnPump_B2 = random(0,2);
    Command.StopWeightBucket_B1 = random(400, 500) / 100.0;
    Command.StopWeightBucket_B2 = random(400, 500) / 100.0;
}

void sendCommand(){
  Serial.println(F("Sending command and waiting for Acknowledgment..."));
  bool Result = Wireless.write( &Command, sizeof(Command) );

  Serial.print(F("  Data Sent, "));
  if (Result) {
      if ( Wireless.isAckPayloadAvailable() ) {
          Wireless.read(&ReceivedResponse, sizeof(ReceivedResponse));
          Serial.print(F("acknowledgement received ["));            
          Serial.print(sizeof(ReceivedResponse));
          Serial.println(F(" bytes]"));
          Serial.print(F("   Bucket1: "));
          Serial.print(ReceivedResponse.PumpOn_B1);
          Serial.print(F(", "));
          Serial.print(ReceivedResponse.PumpEnabled_B1);
          Serial.print(F(", "));
          Serial.println(ReceivedResponse.Weight_B1);
          Serial.print(F("   Bucket2: "));
          Serial.print(ReceivedResponse.PumpOn_B2);
          Serial.print(F(", "));
          Serial.print(ReceivedResponse.PumpEnabled_B2);
          Serial.print(F(", "));
          Serial.println(ReceivedResponse.Weight_B2);
          Serial.print(F("   DHT: "));
          Serial.print(ReceivedResponse.Temp);
          Serial.print(F(", "));
          Serial.print(ReceivedResponse.Humidity);
          Serial.println();
          
          updateCommand();
      }
      else {
          Serial.println(F("acknowledgement received without any data."));
      }        
  }
  else {
      Serial.println(F("no response."));
  }
  LastMessageSent = millis();
}
