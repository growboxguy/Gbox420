//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: DS18B20 waterproof temperature sensor

//Libraries
#include "OneWire.h"
#include "DallasTemperature.h"

//Pins
const byte TempSensorInPin = 4; //Data(yellow) - DS18B20 waterproof temp sensor

//Global Variables
float TempC; // water temperature (°C)
float TempF; // water temperature (°F)

//Component initialization
OneWire TempSensorWire(TempSensorInPin);
DallasTemperature TempSensor(&TempSensorWire);

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: DS18B20 waterproof temperature sensor"));
  Serial.println();
  TempSensor.begin();
}

void loop()
{ //Read data and store it to variables hum and temp
  TempSensor.requestTemperatures();
  TempC = TempSensor.getTempCByIndex(0);
  TempF = TempSensor.getTempFByIndex(0);
  //Print temp and humidity values to serial monitor
  Serial.print(F("Temp(°C): "));
  Serial.print(TempC);
  Serial.print(F(", Temp(°F): "));
  Serial.println(TempF);
  delay(2000); //Delay 2 sec.
}
