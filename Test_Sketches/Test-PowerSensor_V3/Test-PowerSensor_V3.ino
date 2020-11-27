//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Power sensor v3.0
//Displays the measured voltage, current, actual and total energy consumption and prints it on the Serial output every 2 seconds.

//Libraries
#include "SoftwareSerial.h" // Arduino IDE >1.6.6
#include "PZEM004Tv30.h"

//Component initialization
PZEM004Tv30 PowerSensor(&Serial2);

//For testing: Turn on the lights
const byte LightRelayPin = 29; //Pin that controls the relay turning the lights ON/OFF

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: Power sensor v3.0"));
  //PowerSensor.resetEnergy(); //ResetS COUNTER!!!!!!

  Serial.println(F("Turning on the lights..."));
  pinMode(LightRelayPin, OUTPUT);
  digitalWrite(LightRelayPin, LOW); ///True turns relay ON (LOW signal activates Relay)
}

void loop()
{
  Serial.print(F("Voltage: "));
  Serial.print(PowerSensor.voltage());
  Serial.println(F("V"));

  Serial.print(F("Current: "));
  Serial.print(PowerSensor.current());
  Serial.println(F("A"));

  Serial.print(F("Current power usage: "));
  Serial.print(PowerSensor.power());
  Serial.println(F("W"));

  Serial.print(F("Total energy usage: "));
  Serial.print(PowerSensor.energy());
  Serial.println(F("kWh"));

  Serial.print(F("Frequency: "));
  Serial.print(PowerSensor.frequency());
  Serial.println(F("hz"));

  Serial.print(F("Power factor: "));
  Serial.println(PowerSensor.pf());

  Serial.println(F(""));
  delay(2000);
}
