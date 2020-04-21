//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Sending a control command to an Arduino RF-Nano and reading the Acknowledgement response message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  10
#define CSN_PIN 9

const byte ChannelAddress[6] = {"Hemp1"};
RF24 radio(CE_PIN, CSN_PIN);

struct commandTemplate  //Max 32bytes. Template of the command sent to the Receiver. Both Transmitter and Receiver needs to know this structure
{
   bool pump1Enabled; 
   bool pump2Enabled;
   bool pump1Stop; 
   bool pump2Stop;
   bool bucket1StartWatering; 
   bool bucket2StartWatering;
   float bucket1StartWeight;
   float bucket1StopWeight;
   float bucket2StartWeight;
   float bucket2StopWeight;
};
struct commandTemplate FakeCommand = {1,1,0,0,1,0,3.9,5.0,3.8,4.9};  //Fake commands sent to the Receiver


struct responseTemplate  //Max 32bytes. Template of the response back from the Receiver. Both Transmitter and Receiver needs to know this structure
{
   bool pump1Enabled; 
   bool pump2Enabled;
   bool pump1State; 
   bool pump2State;
   float bucket1Weight;
   float bucket2Weight;
};
struct responseTemplate AckResponse; //The response from the Receiver will be stored here, represents the current status of the Receiver

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

void setup() {
    Serial.begin(115200);
    Serial.println(F("Sending and waiting for ACK"));
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.enableAckPayload();
    radio.setRetries(5,5); // delay, count. 5 gives a 1500 µsec delay which is needed for a 32 byte ackPayload
    radio.openWritingPipe(ChannelAddress);
}

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
    }
}

void send() {
    bool rslt;
    rslt = radio.write( &FakeCommand, sizeof(FakeCommand) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print(F("Data Sent."));
    if (rslt) {
        if ( radio.isAckPayloadAvailable() ) {
            radio.read(&AckResponse, sizeof(AckResponse));
            Serial.print(F(" Acknowledgement received[ "));            
            Serial.print(sizeof(AckResponse));
            Serial.println(F(" bytes]"));
            Serial.print(AckResponse.pump1Enabled);
            Serial.print(", ");
            Serial.print(AckResponse.pump2Enabled);
            Serial.print(", ");
            Serial.print(AckResponse.pump1State);
            Serial.print(", ");
            Serial.print(AckResponse.pump2State);
            Serial.print(", ");
            Serial.print(AckResponse.bucket1Weight);
            Serial.print(", ");
            Serial.println(AckResponse.bucket2Weight);
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
    FakeCommand.pump1Enabled = random(0,2);  //Generate random bool: 0 or 1. The max limit is exlusive!
    FakeCommand.pump2Enabled = random(0,2);
    FakeCommand.pump1Stop = random(0,2); 
    FakeCommand.pump2Stop = random(0,2);
    FakeCommand.bucket1StartWatering = random(0,2); 
    FakeCommand.bucket2StartWatering = random(0,2);
}
