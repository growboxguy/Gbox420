//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control message from an Arduino Mega and nRF24L01+, then reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h"     ///keeping track of time
#include "WirelessCommands_Test.h"

const uint8_t WirelessPayloadSize = 32; //Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
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
void * ReceivedResponse = malloc(WirelessPayloadSize);  ///< Pointer to the data sent back in the acknowledgement.
struct ModuleCommand Module1CommandToSend = {HempyMessage::Module1Command,1587399600,1,1};  //Fake commands sent to the Receiver
struct BucketCommand Bucket1CommandToSend = {HempyMessage::Bucket1Command,1,0,0,420,3.8,4.8};  //Fake commands sent to the Receiver
struct BucketCommand Bucket2CommandToSend = {HempyMessage::Bucket2Command,0,0,0,420,3.9,4.9};  //Fake commands sent to the Receiver
struct CommonTemplate GetNextResponse = {HempyMessage::GetNext};  //< Special command to fetch the next Response from the Receiver

static const uint8_t WirelessChannel[6] ={"Hemp1"}; //Identifies the communication channel, needs to match on the Receiver
RF24 Wireless(CE_PIN, CSN_PIN);

//Variables and constants for timing Command messages 
static unsigned long LastCommandSent = 0;  //When was the last message sent
static unsigned long LastResponseReceived = 0;  //When was the last response received
static const unsigned long MessageInterval = 15000; // send a control message once per 15 seconds
static const uint16_t WirelessMessageTimeout = 500; //Default 0.5sec -  One package should be exchanged within this timeout (Including retries and delays)
static const uint8_t RetryDelay = 10; //How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
static const uint8_t RetryCount = 15; //How many retries before giving up, max 15

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println(F("Setting up the wireless transmitter..."));
    Wireless.begin();
    Wireless.setDataRate( RF24_250KBPS );
    Wireless.setCRCLength(RF24_CRC_8);  /// RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
    Wireless.setPALevel(RF24_PA_MAX);  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
    Wireless.setPayloadSize(WirelessPayloadSize);  ///Set the number of bytes in the payload
    Wireless.enableAckPayload();  ///< Enable custom payloads on the acknowledge packets. Ack payloads are a handy way to return data back to senders without changing the radio modes on both units.
    Wireless.setRetries(RetryDelay,RetryCount); 
    Wireless.openWritingPipe(WirelessChannel); 
    sendMessages(); 
}

void loop() {
    if (millis() - LastCommandSent >= MessageInterval) //Check if it is time to exchange messages
    {
      sendMessages();      
    }
}

void sendMessages(){
    updateFakeData();
    sendCommand(&Module1CommandToSend);  //< Command - Response exchange
    sendCommand(&Bucket1CommandToSend);  //< Command - Response exchange
    sendCommand(&Bucket2CommandToSend);  //< Command - Response exchange
    while(sendCommand(&GetNextResponse) < HempyMessage::GetNext && millis()- LastResponseReceived < WirelessMessageTimeout);   //< special Command, only exchange Response.
    if(Debug)Serial.println(F("Message exchange finished"));
}

void updateFakeData() {        // so you can see that new data is being sent
    //Command.TurnOnPump_B1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exclusive!
    //Command.TurnOnPump_B2 = random(0,2);
    Bucket1CommandToSend.StopWeight = random(400, 500) / 100.0;
    Bucket2CommandToSend.StopWeight = random(400, 500) / 100.0;
}

HempyMessage sendCommand(void* CommandToSend){
    HempyMessage SequenceIDToSend = ((CommonTemplate*)CommandToSend) -> SequenceID; 
    HempyMessage ReceivedSequenceID = NULL;       
    if(Debug)
    {
        Serial.print(F("Sending command SequenceID: "));
        Serial.print(SequenceIDToSend);
        Serial.print(F(" - "));
        Serial.print(sequenceIDToText(SequenceIDToSend));
        Serial.println(F(" and waiting for Acknowledgment..."));
    }
    Wireless.flush_rx();  ///< Dump all previously received but unprocessed messages
    bool Result = Wireless.write( CommandToSend, WirelessPayloadSize );
    delay(50); //< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    Serial.print(F("  Data Sent, "));
    if (Result) {
        if ( Wireless.isAckPayloadAvailable()) {
            Wireless.read(ReceivedResponse, WirelessPayloadSize);
            ReceivedSequenceID = ((CommonTemplate*)ReceivedResponse) -> SequenceID;
            
            if(Debug)
            {
                Serial.print(F("Response SequenceID: "));
                Serial.print(ReceivedSequenceID);
                Serial.print(F(" - "));
                Serial.println(sequenceIDToText(ReceivedSequenceID));
            }         

            switch (ReceivedSequenceID)
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
                case HempyMessage::GetNext :
                    Serial.println(F("  Last message received"));
                    break;
                default:
                    Serial.println(F("  SequenceID not known, ignoring package"));
                    break;
            }
        }
        else {
            Serial.println(F("acknowledgement received without any data."));
        }  
        LastResponseReceived = millis();      
    }
    else {
        Serial.println(F("no response."));
    }
    LastCommandSent = millis();
    return ReceivedSequenceID;
}
