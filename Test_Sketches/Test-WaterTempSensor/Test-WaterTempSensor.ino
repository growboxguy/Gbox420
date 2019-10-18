//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: DS18B20 waterproof temperature sensor

//Libraries
#include "OneWire.h";
#include "DallasTemperature.h";

//Pins
const byte ReservoirTempSensorInPin = 45; //Data(yellow) - DS18B20 waterproof temp sensor
  
//Global Variables
float ReservoirTempC; //Reservoir water temperature (°C)
float ReservoirTempF; //Reservoir water temperature (°F)

//Component initialization
OneWire ReservoirTempSensorWire(ReservoirTempSensorInPin);
DallasTemperature ReservoirTempSensor(&ReservoirTempSensorWire);

void setup(){
  Serial.begin(115200);                                                                 
  ReservoirTempSensor.begin();
}

void loop(){  //Read data and store it to variables hum and temp
    ReservoirTempSensor.requestTemperatures(); 
    ReservoirTempC = ReservoirTempSensor.getTempCByIndex(0);
    ReservoirTempF = ReservoirTempSensor.getTempFByIndex(0);
    //Print temp and humidity values to serial monitor
    Serial.print(F("Temp(°C): "));
    Serial.print(ReservoirTempC);
    Serial.print(F(", Temp(°F): "));
    Serial.println(ReservoirTempF);    
    delay(2000); //Delay 2 sec.
}
