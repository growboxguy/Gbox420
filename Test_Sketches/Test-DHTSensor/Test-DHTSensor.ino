//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: DHT sensor (Humidity and temperature sensor)

//Libraries
#include "DHT.h";

//Pins
const int TempSensorInPin = 3; // DAT digital input - DHT22 sensor

//Global Variables
float Humidity;    //Stores humidity
float Temperature; //Stores temperature

//Component initialization
const uint8_t DHTType = DHT22;            //specify temp/humidity sensor type: DHT21(AM2301) or DHT22(AM2302,AM2321)
DHT TempSensor(TempSensorInPin, DHTType); // Initialize DHT sensor

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: DHT sensor (Humidity and temperature sensor)"));
  TempSensor.begin();
}

void loop()
{ //Read data and store it to variables hum and temp
  Humidity = TempSensor.readHumidity();
  Temperature = TempSensor.readTemperature();
  //Print temp and humidity values to serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(Humidity);
  Serial.print(F("%, Temp: "));
  Serial.print(Temperature);
  Serial.print(F(" °C / "));
  Serial.print(Temperature * 1.8 + 32);
  Serial.println(F(" °F"));
  delay(5000); //Delay 5 sec.
}
