//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Humidity and temperature sensor

//Libraries
#include "DHT.h";

//Pins
const int TempSensorInPin=43;     // DAT digital input - DHT22 sensor
  
//Global Variables
float hum;  //Stores humidity
float temp; //Stores temperature

//Component initialization
const uint8_t  DHTType=DHT22; //specify temp/humidity sensor type: DHT21(AM2301) or DHT22(AM2302,AM2321)
DHT TempSensor(TempSensorInPin, DHTType); // Initialize DHT sensor

void setup(){
  Serial.begin(115200);                                                                    
  TempSensor.begin();
}

void loop(){  //Read data and store it to variables hum and temp
    hum = TempSensor.readHumidity();
    temp= TempSensor.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print("%, Temp: ");
    Serial.print(temp);
    Serial.print(" °C / ");
    Serial.print(temp * 1.8 + 32);
    Serial.println(" °F");
    delay(2000); //Delay 2 sec.
}
