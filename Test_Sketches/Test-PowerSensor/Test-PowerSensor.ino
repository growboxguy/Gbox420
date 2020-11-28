//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Power sensor
//Displays the measured voltage, current, actual and total energy consumption and prints it on the Serial output every 2 seconds.

//Libraries
//#include "SoftwareSerial.h" // Arduino IDE >1.6.6
#include "PZEM004T.h"

//Component initialization
PZEM004T PowerSensor(&Serial2);
IPAddress PowerSensorIP(192, 168, 1, 1);

//For testing: Turn on the lights
const byte LightRelayPin = 29; //Pin that controls the relay turning the lights ON/OFF

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: Power sensor"));
  PowerSensor.setAddress(PowerSensorIP);

  Serial.println(F("Turning on the lights..."));
  pinMode(LightRelayPin, OUTPUT);
  digitalWrite(LightRelayPin, LOW); ///< True turns relay ON (LOW signal activates Relay)
}

void loop()
{
  Serial.print(F("Voltage: "));
  Serial.print(PowerSensor.voltage(PowerSensorIP));
  Serial.println(F("V"));

  Serial.print(F("Current: "));
  Serial.print(PowerSensor.current(PowerSensorIP));
  Serial.println(F("A"));

  Serial.print(F("Current power usage: "));
  Serial.print(PowerSensor.power(PowerSensorIP));
  Serial.println(F("W"));

  Serial.print(F("Total energy usage: "));
  Serial.print(PowerSensor.energy(PowerSensorIP));
  Serial.println(F("Wh"));

  Serial.println(F(""));
  delay(2000);
}
