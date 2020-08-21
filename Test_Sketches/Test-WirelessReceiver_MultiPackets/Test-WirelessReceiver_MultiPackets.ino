//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Receiving a control message using Arduino Nano and nRF24L01+ (or RF-Nano) and Acknowledging it with a response message
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

void * ReceivedCommand = malloc(PayloadSize);

///< Variables used during wireless communication
struct DefaultResponse DefaultResponseToSend = {HempyMessage::DefaultResponse,1};  //Fake response sent to the Transmitter
struct BucketResponse Bucket1ResponseToSend = {HempyMessage::Bucket1Response,0,0,4.20};  //Fake response sent to the Transmitter
struct BucketResponse Bucket2ResponseToSend = {HempyMessage::Bucket2Response,1,1,4.20};  //Fake response sent to the Transmitter
struct DHTResponse DHTResponseToSend = {HempyMessage::DHTResponse,23.4,42.0};

const byte WirelessChannel[6] ={"Test1"};  //Identifies the communication channel, needs to match on the Transmitter
RF24 radio(CE_PIN, CSN_PIN);

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
        radio.read( ReceivedCommand, PayloadSize );    //Load the command to the ReceivedCommand variable
        if(Debug)
        {
            Serial.print(F("Command received, SequenceID: "));
            Serial.print(((commonTemplate*)ReceivedCommand) -> SequenceID);
            Serial.println();
        }

        switch (((commonTemplate*)ReceivedCommand) -> SequenceID)
        {
        case HempyMessage::Bucket1Command :
            Serial.print(F("  Bucket1: "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> DisablePump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> TurnOnPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> TurnOffPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> TimeOutPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> StartWeight);
            Serial.print(F(", "));
            Serial.println(((BucketCommand*)ReceivedCommand) -> StopWeight);
            //radio.writeAckPayload(1, &Bucket2ResponseToSend, PayloadSize); // load the next response into the buffer
            break;
        case HempyMessage::Bucket2Command :
            Serial.print(F("  Bucket2: "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> DisablePump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> TurnOnPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> TurnOffPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> TimeOutPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand*)ReceivedCommand) -> StartWeight);
            Serial.print(F(", "));
            Serial.println(((BucketCommand*)ReceivedCommand) -> StopWeight);
            //radio.writeAckPayload(1, &DHTResponseToSend, PayloadSize); // load the next response into the buffer            
            break;
        default:
            Serial.println(F("  SequenceID not known, ignoring package"));
            break;
        }
        Serial.println();
        
        updateReplyData();
    }
}

void updateReplyData() { // so you can see that new data is being sent
    Bucket1ResponseToSend.Weight = random(400, 500) / 100.0;
    Bucket2ResponseToSend.Weight = random(400, 500) / 100.0;
    DHTResponseToSend.Humidity = random(0, 10000) / 100.0;    
    radio.writeAckPayload(1, &DefaultResponseToSend, PayloadSize); // load the payload to get sent out when the next control message is received
}
