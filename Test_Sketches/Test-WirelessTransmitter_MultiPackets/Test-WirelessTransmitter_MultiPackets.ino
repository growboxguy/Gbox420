//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control message from an Arduino Mega and nRF24L01+, then reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time
#include "WirelessCommands_Test.h"

const uint8_t PayloadSize = 32; //Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+

///< Ports for Arduino Nano or RF-Nano
const byte CE_PIN = 10;
const byte CSN_PIN = 9;

/*
///< Ports for Arduino Mega
const byte CSN_PIN = 49;
const byte CE_PIN = 53;
*/

///< Variables used during wireless communication
void * ReceivedResponse = malloc(PayloadSize);  ///< Pointer to the data sent back in the acknowledgement.

struct DefaultCommand DefaultCommandToSend = {HempyMessage::DefaultCommand,1587399600,1,1};  //Fake commands sent to the Receiver
struct BucketCommand Bucket1CommandToSend = {HempyMessage::Bucket1Command,0,0,0,120,3.8,4.8};  //Fake commands sent to the Receiver
struct BucketCommand Bucket2CommandToSend = {HempyMessage::Bucket2Command,0,0,0,120,3.9,4.9};  //Fake commands sent to the Receiver

const uint8_t WirelessChannel[6] ={"Test1"}; //Identifies the communication channel, needs to match on the Receiver
RF24 Wireless(CE_PIN, CSN_PIN);

//Variables and constants for timing Control messages 
unsigned long LastMessageSent = 0;  //When was the last message sent
const unsigned long MessageInterval = 15000; // send a control message once per 15 seconds
const uint8_t RetryDelay = 5; //How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
const uint8_t RetryCount = 5; //How many retries before giving up, max 15

void setup() {
    Serial.begin(115200);
    Serial.println(F("Setting up the wireless transmitter..."));
    Wireless.begin();
    Wireless.setDataRate( RF24_250KBPS );
    Wireless.enableAckPayload();  ///< Enable custom payloads on the acknowledge packets. Ack payloads are a handy way to return data back to senders without manually changing the radio modes on both units.
    Wireless.setRetries(RetryDelay,RetryCount); 
    Wireless.openWritingPipe(WirelessChannel); 
    updateCommand();
    sendCommand(&DefaultCommandToSend);
    sendCommand(&Bucket1CommandToSend);
    sendCommand(&Bucket2CommandToSend);
}

void loop() {
    if (millis() - LastMessageSent >= MessageInterval) //Check if it is time to send a command
    {
      sendCommand(&DefaultCommandToSend);
      sendCommand(&Bucket1CommandToSend);
      sendCommand(&Bucket2CommandToSend);
    }
}

void updateCommand() {        // so you can see that new data is being sent
    //Command.TurnOnPump_B1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exclusive!
    //Command.TurnOnPump_B2 = random(0,2);
    Bucket1CommandToSend.StartWeight = random(400, 500) / 100.0;
    Bucket2CommandToSend.StopWeight = random(400, 500) / 100.0;
}

void sendCommand(void* CommandToSend){
  Serial.println(F("Sending command and waiting for Acknowledgment..."));
  bool Result = Wireless.write( CommandToSend, PayloadSize );

  Serial.print(F("  Data Sent, "));
  if (Result) {
      if ( Wireless.isAckPayloadAvailable() ) {
          Wireless.read(ReceivedResponse, PayloadSize);
          Serial.print(F("acknowledgement received ["));            
          Serial.print(sizeof(ReceivedResponse));
          Serial.println(F(" bytes]"));
          Serial.print(F("   SequenceID: "));
          Serial.print(((commonTemplate*)ReceivedResponse) -> SequenceID);
          /*

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
          */
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
