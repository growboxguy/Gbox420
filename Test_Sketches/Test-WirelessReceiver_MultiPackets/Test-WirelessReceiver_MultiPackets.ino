//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Receiving a control message using Arduino Nano and nRF24L01+ (or RF-Nano) and Acknowledging it with a response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "TimeLib.h" // Keeping track of time
#include "WirelessCommands_Test.h"

const uint8_t PayloadSize = 32; ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
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
uint8_t NextSequenceID = HempyMessage::Module1Response;
void *ReceivedCommand = malloc(PayloadSize);
struct ModuleResponse Module1ResponseToSend = {HempyMessage::Module1Response, 1};          //Fake response sent to the Transmitter
struct BucketResponse Bucket1ResponseToSend = {HempyMessage::Bucket1Response, 0, 0, 4.20}; //Fake response sent to the Transmitter
struct BucketResponse Bucket2ResponseToSend = {HempyMessage::Bucket2Response, 1, 1, 4.20}; //Fake response sent to the Transmitter
struct DHTResponse DHT1ResponseToSend = {HempyMessage::DHT1Response, 23.4, 42.0};          //Fake response sent to the Transmitter
struct CommonTemplate LastResponseToSend = {HempyMessage::Reset};                          ///< Special response signaling the end of a message exchange to the Transmitter

const uint8_t WirelessChannel[6] = {"Hemp1"}; //Identifies the communication channel, needs to match on the Transmitter
RF24 Wireless(CE_PIN, CSN_PIN);

unsigned long LastFakeMessageUpdate = 0;          //When was the last time the example data was refreshed
unsigned long LastMessageSent = 0;                //When was the last message sent
const unsigned long WirelessMessageTimeout = 500; //Default 0.5sec -  One package should be exchanged within this timeout

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println(F("Setting up the wireless receiver..."));
    Wireless.begin();
    Wireless.setDataRate(RF24_250KBPS);
    Wireless.setCRCLength(RF24_CRC_8);    ///< RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
    Wireless.setPALevel(RF24_PA_MAX);     //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
    Wireless.setPayloadSize(PayloadSize); ///< Set the number of bytes in the payload
    Wireless.openReadingPipe(1, WirelessChannel);
    Wireless.enableAckPayload();
    Wireless.startListening();
    updateAckData(); ///< Updates what data should be sent back in the Acknowledgement package
    Serial.println(F("Listening..."));
}

void loop()
{
    ///< Periodically refresh the content (5sec default)
    if (millis() - LastFakeMessageUpdate > 5000)
    {
        updateFakeData();
        LastFakeMessageUpdate = millis();
    }

    ///< Checking arrived wireless Commands
    if (Wireless.available())
    {                                                //When a command is received
        Wireless.read(ReceivedCommand, PayloadSize); //Load the command to the ReceivedCommand variable
        HempyMessage ReceivedSequenceID = ((CommonTemplate *)ReceivedCommand)->SequenceID;
        LastMessageSent = millis(); ///< Store current time
        if (Debug)
        {
            Serial.print(F("Command received with SequenceID: "));
            Serial.print(ReceivedSequenceID);
            Serial.print(F(" - "));
            Serial.print(sequenceIDToText(ReceivedSequenceID));
            Serial.print(F(", Acknowledgement sent with SequenceID: "));
            Serial.print(NextSequenceID);
            Serial.print(F(" - "));
            Serial.println(sequenceIDToText(NextSequenceID));
        }

        switch (ReceivedSequenceID)
        {
        case HempyMessage::Module1Command:
            Serial.print(F("  Module: "));
            Serial.print(((ModuleCommand *)ReceivedCommand)->Time);
            Serial.print(F(", "));
            Serial.print(((ModuleCommand *)ReceivedCommand)->Debug);
            Serial.print(F(", "));
            Serial.println(((ModuleCommand *)ReceivedCommand)->Metric);
            NextSequenceID = HempyMessage::Bucket1Response; // update the next Message that will be copied to the buffer
            break;
        case HempyMessage::Bucket1Command:
            Serial.print(F("  Bucket1: "));
            Serial.print(((BucketCommand *)ReceivedCommand)->DisablePump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->TurnOnPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->TurnOffPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->TimeOutPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->StartWeight);
            Serial.print(F(", "));
            Serial.println(((BucketCommand *)ReceivedCommand)->StopWeight);
            NextSequenceID = HempyMessage::Bucket2Response; // update the next Message that will be copied to the buffer
            break;
        case HempyMessage::Bucket2Command:
            Serial.print(F("  Bucket2: "));
            Serial.print(((BucketCommand *)ReceivedCommand)->DisablePump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->TurnOnPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->TurnOffPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->TimeOutPump);
            Serial.print(F(", "));
            Serial.print(((BucketCommand *)ReceivedCommand)->StartWeight);
            Serial.print(F(", "));
            Serial.println(((BucketCommand *)ReceivedCommand)->StopWeight);
            NextSequenceID = HempyMessage::DHT1Response; // update the next Message that will be copied to the buffer
            break;
        case HempyMessage::Reset: ///< Used to get all Responses that do not have a corresponding Command
            if (++NextSequenceID > HempyMessage::Reset)
            {                                                   ///< If the end of HempyMessage enum is reached
                NextSequenceID = HempyMessage::Module1Response; ///< Load the first response for the next message exchange
                if (Debug)
                {
                    Serial.println(F("Message exchange finished"));
                }
            }
            break;
        default:
            Serial.println(F("  SequenceID unknown, ignoring message"));
            //NextSequenceID = HempyMessage::Module1Response; // update the next Message that will be copied to the buffer
            break;
        }
        updateAckData(); ///< Loads the next ACK that will be sent out
    }
    if (NextSequenceID != HempyMessage::Module1Response && millis() - LastMessageSent >= WirelessMessageTimeout)
    {                                                   ///< If there is a package exchange in progress
        NextSequenceID = HempyMessage::Module1Response; ///< Reset back to the first response
        Serial.println(F("Timeout during message exchange, reseting to first response"));
        updateAckData();
    }
}

void updateFakeData()
{ // - Simulates sensor readings changin
    Bucket1ResponseToSend.Weight = random(400, 500) / 100.0;
    Bucket2ResponseToSend.Weight = random(400, 500) / 100.0;
    DHT1ResponseToSend.Humidity = random(0, 10000) / 100.0;
}

void updateAckData()
{ // so you can see that new data is being sent
    if (Debug)
    {
        Serial.print(F("  Updating Acknowledgement to: "));
        Serial.println(NextSequenceID);
    }
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)

    switch (NextSequenceID) // based on the NextSeqenceID load the next response into the Acknowledgement buffer
    {
    case HempyMessage::Module1Response:
        Wireless.writeAckPayload(1, &Module1ResponseToSend, PayloadSize);
        break;
    case HempyMessage::Bucket1Response:
        Wireless.writeAckPayload(1, &Bucket1ResponseToSend, PayloadSize);
        break;
    case HempyMessage::Bucket2Response:
        Wireless.writeAckPayload(1, &Bucket2ResponseToSend, PayloadSize);
        break;
    case HempyMessage::DHT1Response:
        Wireless.writeAckPayload(1, &DHT1ResponseToSend, PayloadSize);
        break;
    case HempyMessage::Reset: ///< Reset should always be the last element in the HempyMessage enum: Signals to stop the message exchange
        Wireless.writeAckPayload(1, &LastResponseToSend, PayloadSize);
        break;
    default:
        Serial.println(F("   Unknown next Sequence number, Ack defaults loaded"));
        Wireless.writeAckPayload(1, &Module1ResponseToSend, PayloadSize); // load the first Response into the buffer
        break;
    }
}
