//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Power meter
//Displays the measured voltage, current, actual and total energy consumption and prints it on the Serial output every 5 seconds.

//Libraries
#include "SoftwareSerial.h" // Arduino IDE >1.6.6
#include "PZEM004T.h"

//Component initialization
PZEM004T pzem(&Serial2);
IPAddress ip(192,168,1,1);

void setup() {
  Serial.begin(115200);
  pzem.setAddress(ip);
    pinMode(28, OUTPUT);
     digitalWrite(28,LOW);
}

void loop() {
  float v = pzem.voltage(ip);
  Serial.print(v);Serial.print("V; ");
  float i = pzem.current(ip);
  Serial.print(i);Serial.print("A; ");   
  float p = pzem.power(ip);
  Serial.print(p);Serial.print("W; ");
  float e = pzem.energy(ip);
  Serial.print(e);Serial.println("Wh; ");
  delay(5000);
}
