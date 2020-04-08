

// SimpleRxAckPayload- the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  10
#define CSN_PIN 9

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[10]; // this must match dataToSend in the TX
int ackData[2] = {109, -4000}; // the two values to be sent to the master
bool newData = false;

//==============

void setup() {

    Serial.begin(115200);

    Serial.println("SimpleRxAckPayload Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);

    radio.enableAckPayload();
    
    radio.startListening();

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // pre-load data
}

//==========

void loop() {
    getData();
    showData();
}

//============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        updateReplyData();
        newData = true;
    }
}

//================

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        Serial.print(" ackPayload sent ");
        Serial.print(ackData[0]);
        Serial.print(", ");
        Serial.println(ackData[1]);
        newData = false;
    }
}

//================

void updateReplyData() {
    ackData[0] -= 1;
    ackData[1] -= 1;
    if (ackData[0] < 100) {
        ackData[0] = 109;
    }
    if (ackData[1] < -4009) {
        ackData[1] = -4000;
    }
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time
}
