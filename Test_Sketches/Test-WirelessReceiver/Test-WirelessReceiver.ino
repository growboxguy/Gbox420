//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Receiving a control command using Arduino RF-Nano and Acknowledging it with a custom message
//Based on: https://forum.arduino.cc/index.php?topic=421081.0

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  10
#define CSN_PIN 9

const byte ChannelAddress[6] = {"Hemp1"};
RF24 radio(CE_PIN, CSN_PIN);

struct commandTemplate  //Max 32bytes. Template of the command sent by the Transmitter. Both Transmitter and Receiver needs to know this structure
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
struct commandTemplate ReceivedCommand;  //Variable where the actual command values will get stored

struct responseTemplate  //Max 32bytes. Template of the response sent back to the Transmitter. Both Transmitter and Receiver needs to know this structure
{
   bool pump1Enabled; 
   bool pump2Enabled;
   bool pump1State; 
   bool pump2State;
   float bucket1Weight;
   float bucket2Weight;
};
struct responseTemplate FakeResponse = {1,1,0,1,4.20,1.23};  //Fake response sent back in the Acknowledgement after receiving a command from the Transmitter

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
        Serial.println(F(" bytes], AckPayload sent")); 
        Serial.print(ReceivedCommand.pump1Enabled);
        Serial.print(", ");
        Serial.print(ReceivedCommand.pump2Enabled);
        Serial.print(", ");
        Serial.print(ReceivedCommand.pump1Stop);
        Serial.print(", ");
        Serial.print(ReceivedCommand.pump2Stop);
        Serial.print(", ");
        Serial.print(ReceivedCommand.bucket1StartWatering);
        Serial.print(", ");
        Serial.print(ReceivedCommand.bucket2StartWatering);
        Serial.print(", ");
        Serial.print(ReceivedCommand.bucket1StartWeight);
        Serial.print(", ");
        Serial.print(ReceivedCommand.bucket1StopWeight);
        Serial.print(", ");
        Serial.print(ReceivedCommand.bucket2StartWeight);
        Serial.print(", ");
        Serial.println(ReceivedCommand.bucket2StopWeight);
        Serial.println();

        updateReplyData();
    }
}

void updateReplyData() { // so you can see that new data is being sent
    FakeResponse.bucket1Weight = random(400, 500) / 100.0;
    FakeResponse.bucket2Weight = random(400, 500) / 100.0;
    radio.writeAckPayload(1, &FakeResponse, sizeof(FakeResponse)); // load the payload for the next time
}
