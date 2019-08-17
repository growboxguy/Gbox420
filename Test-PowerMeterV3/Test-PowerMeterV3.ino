//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Power meter
//Displays the measured voltage, current, actual and total energy consumption and prints it on the Serial output every 5 seconds.

//Libraries
#include "SoftwareSerial.h" // Arduino IDE >1.6.6
#include "PZEM004Tv30.h"

//Component initialization
PZEM004Tv30 PowerSensor(&Serial2);

void setup() {
  Serial.begin(115200);
  PowerSensor.resetEnergy();   //RESETS COUNTER!!!!!!
}

void loop() {
  float v = PowerSensor.voltage();
  Serial.print(v);Serial.print("V; ");
  float i = PowerSensor.current();
  Serial.print(i);Serial.print("A; ");   
  float p = PowerSensor.power();
  Serial.print(p);Serial.print("W; ");
  float e = PowerSensor.energy();
  Serial.print(e);Serial.println("Wh; ");
  delay(2000);
}
