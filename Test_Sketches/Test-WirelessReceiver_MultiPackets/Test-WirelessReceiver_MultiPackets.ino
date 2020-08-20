//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Receiving a control message using Arduino Nano and nRF24L01+ (or RF-Nano) and Acknowledging it with a response message
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


///Global constants
const uint8_t NumberOfCommands = 2;  //How many 32byte Command packages are there to exchange
const uint8_t NumberOfResponses = 1; //How many 32byte Responses packages are there to exchange

void * ReceivedCommand = malloc(PayloadSize);

const byte WirelessChannel[6] ={"Test1"};  //Identifies the communication channel, needs to match on the Transmitter
RF24 radio(CE_PIN, CSN_PIN);

///< Stucts of data to exchange


struct Command1 ReceivedCommand1;  //Variable where the command values received from the Transmitter will get stored
struct Command2 ReceivedCommand2;  //Variable where the command values received from the Transmitter will get stored
struct Response1 ResponseToSend = {1,1,4.20,0,0,1.23,23.4,50.1};  //Fake response sent back in the Acknowledgement after receiving a command

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
        Serial.print(F("Command received, SequenceID: "));
        Serial.print(((commonTemplate*)ReceivedCommand) -> SequenceID);
        Serial.println();

        switch (((commonTemplate*)ReceivedCommand) -> SequenceID)
        {
        case 0:
            Serial.println(F("  Casting to Command1"));
            Serial.print(F("  Bucket1: "));
            Serial.print(((Command1*)ReceivedCommand) -> DisablePump_B1);
            Serial.print(F(", "));
            Serial.print(((Command1*)ReceivedCommand) -> TurnOnPump_B1);
            Serial.print(F(", "));
            Serial.print(((Command1*)ReceivedCommand) -> TurnOffPump_B1);
            Serial.print(F(", "));
            Serial.print(((Command1*)ReceivedCommand) -> TimeOutPump_B1);
            Serial.print(F(", "));
            Serial.print(((Command1*)ReceivedCommand) -> StartWeightBucket_B1);
            Serial.print(F(", "));
            Serial.println(((Command1*)ReceivedCommand) -> StopWeightBucket_B1);
            
            break;
        case 1:
            Serial.println(F("  Casting to Command2"));
            Serial.print(F("  Bucket2: "));
            Serial.print(((Command2*)ReceivedCommand) -> DisablePump_B2);
            Serial.print(F(", "));
            Serial.print(((Command2*)ReceivedCommand) -> TurnOnPump_B2);
            Serial.print(F(", "));
            Serial.print(((Command2*)ReceivedCommand) -> TurnOffPump_B2);
            Serial.print(F(", "));
            Serial.print(((Command2*)ReceivedCommand) -> TimeOutPump_B2);
            Serial.print(F(", "));
            Serial.print(((Command2*)ReceivedCommand) -> StartWeightBucket_B2);
            Serial.print(F(", "));
            Serial.println(((Command2*)ReceivedCommand) -> StopWeightBucket_B2);
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
    ResponseToSend.Weight_B1 = random(400, 500) / 100.0;
    ResponseToSend.Weight_B2 = random(400, 500) / 100.0;
    ResponseToSend.Humidity = random(0, 10000) /100;
    radio.writeAckPayload(1, &ResponseToSend, sizeof(ResponseToSend)); // load the payload to get sent out when the next control message is received
}