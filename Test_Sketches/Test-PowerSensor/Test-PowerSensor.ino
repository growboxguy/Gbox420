//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Power sensor
//Displays the measured voltage, current, actual and total energy consumption and prints it on the Serial output every 2 seconds.

//Libraries
//#include "SoftwareSerial.h" // Arduino IDE >1.6.6
#include "PZEM004T.h"

//Component initialization
PZEM004T PowerSensor(&Serial2);
IPAddress PowerSensorIP(192, 168, 1, 1);

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: Power sensor"));
  PowerSensor.setAddress(PowerSensorIP);
}

void loop()
{
  float v = PowerSensor.voltage(PowerSensorIP);
  Serial.print(v);
  Serial.print(F("V; "));
  float i = PowerSensor.current(PowerSensorIP);
  Serial.print(i);
  Serial.print(F("A; "));
  float p = PowerSensor.power(PowerSensorIP);
  Serial.print(p);
  Serial.print(F("W; "));
  float e = PowerSensor.energy(PowerSensorIP);
  Serial.print(e);
  Serial.println(F("Wh; "));
  delay(2000);
}
