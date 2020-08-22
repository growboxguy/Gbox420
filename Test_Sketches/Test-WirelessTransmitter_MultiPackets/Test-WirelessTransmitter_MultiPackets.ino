//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control message from an Arduino Mega and nRF24L01+, then reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time
#include "WirelessCommands_Test.h"

const uint8_t PayloadSize = 32; //Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
const bool Debug = true;

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

struct ModuleCommand Module1CommandToSend = {HempyMessage::Module1Command,1587399600,1,1};  //Fake commands sent to the Receiver
struct BucketCommand Bucket1CommandToSend = {HempyMessage::Bucket1Command,1,0,0,420,3.8,4.8};  //Fake commands sent to the Receiver
struct BucketCommand Bucket2CommandToSend = {HempyMessage::Bucket2Command,0,0,0,420,3.9,4.9};  //Fake commands sent to the Receiver
struct commonTemplate GetNextToSend = {HempyMessage::GetNext};  //< Special command to fetch the next Response from the Receiver

const uint8_t WirelessChannel[6] ={"Test1"}; //Identifies the communication channel, needs to match on the Receiver
RF24 Wireless(CE_PIN, CSN_PIN);

//Variables and constants for timing Control messages 
unsigned long LastMessageSent = 0;  //When was the last message sent
const unsigned long MessageInterval = 15000; // send a control message once per 15 seconds
const uint8_t RetryDelay = 10; //How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
const uint8_t RetryCount = 15; //How many retries before giving up, max 15

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println(F("Setting up the wireless transmitter..."));
    Wireless.begin();
    Wireless.setDataRate( RF24_250KBPS );
    Wireless.enableAckPayload();  ///< Enable custom payloads on the acknowledge packets. Ack payloads are a handy way to return data back to senders without manually changing the radio modes on both units.
    Wireless.setRetries(RetryDelay,RetryCount); 
    Wireless.openWritingPipe(WirelessChannel); 
    updateCommand();
    sendCommand(&Module1CommandToSend);
    sendCommand(&Bucket1CommandToSend);
    sendCommand(&Bucket2CommandToSend);
    sendCommand(&GetNextToSend);  
}

void loop() {
    if (millis() - LastMessageSent >= MessageInterval) //Check if it is time to send a command
    {
      sendCommand(&Module1CommandToSend);
      sendCommand(&Bucket1CommandToSend);
      sendCommand(&Bucket2CommandToSend);
      sendCommand(&GetNextToSend);
    }
    //TODO: NumberOfResponses
}

void updateCommand() {        // so you can see that new data is being sent
    //Command.TurnOnPump_B1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exclusive!
    //Command.TurnOnPump_B2 = random(0,2);
    Bucket1CommandToSend.StopWeight = random(400, 500) / 100.0;
    Bucket2CommandToSend.StopWeight = random(400, 500) / 100.0;
}

void sendCommand(void* CommandToSend){
    if(Debug)
    {
        Serial.print(F("Sending command SequenceID: '"));
        Serial.print(((commonTemplate*)CommandToSend) -> SequenceID);
        Serial.println(F("' and waiting for Acknowledgment..."));
    }
    Wireless.flush_rx();  ///< Dump all previously received but unprocessed messages
    bool Result = Wireless.write( CommandToSend, PayloadSize );
    delay(10); //< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    Serial.print(F("  Data Sent, "));
    if (Result) {
        if ( Wireless.isAckPayloadAvailable()) {
            Wireless.read(ReceivedResponse, PayloadSize);
            if(Debug)
            {
                Serial.print(F("Response received, SequenceID: "));
                Serial.print(((commonTemplate*)ReceivedResponse) -> SequenceID);
                Serial.println();
            }         

            switch (((commonTemplate*)ReceivedResponse) -> SequenceID)
            {
                case HempyMessage::Module1Response :
                    Serial.print(F("  ModuleOK: "));
                    Serial.println(((ModuleResponse*)ReceivedResponse) -> Status);
                break;
                case HempyMessage::Bucket1Response :
                    Serial.print(F("  Bucket1: "));
                    Serial.print(((BucketResponse*)ReceivedResponse) -> PumpOn);
                    Serial.print(F(", "));
                    Serial.print(((BucketResponse*)ReceivedResponse) -> PumpEnabled);
                    Serial.print(F(", "));
                    Serial.println(((BucketResponse*)ReceivedResponse) -> Weight);
                    break;
                case HempyMessage::Bucket2Response :
                    Serial.print(F("  Bucket2: "));
                    Serial.print(((BucketResponse*)ReceivedResponse) -> PumpOn);
                    Serial.print(F(", "));
                    Serial.print(((BucketResponse*)ReceivedResponse) -> PumpEnabled);
                    Serial.print(F(", "));
                    Serial.println(((BucketResponse*)ReceivedResponse) -> Weight);
                    break;
                case HempyMessage::DHT1Response :
                    Serial.print(F("  DHT: "));
                    Serial.print(((DHTResponse*)ReceivedResponse) -> Temp);
                    Serial.print(F(", "));
                    Serial.println(((DHTResponse*)ReceivedResponse) -> Humidity);
                    break;
                default:
                    Serial.println(F("  SequenceID not known, ignoring package"));
                    break;
            }
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
