//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: AHT20 + BME280 sensor (Two chips on a single board with I2C)
//Based on: https://forum.arduino.cc/index.php?topic=421081.0


#include <Adafruit_AHTX0.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_AHTX0 aht;  // Temp and Humidity sensor over I2C
Adafruit_BMP280 bme; // BME280 barometric sensor over I2C
Adafruit_Sensor *aht_humidity, *aht_temp;

/*
// I2C Ports for Arduino Nano or RF-Nano (Pre-defined)
//const byte SDA = A4;
//const byte SCL = A5;

// I2C PortsPorts for Arduino Mega (Pre-defined)
//const byte SDA = 20;
//const byte SCL = 21;
*/


void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);
  
  if (!bme.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  }
  
  if (!aht.begin()) {
    Serial.println("Failed to find AHT10/AHT20 chip");
    }
  aht_temp = aht.getTemperatureSensor();
  aht_humidity = aht.getHumiditySensor();
}

void loop() {
  // put your main code here, to run repeatedly:

  /* BMP280 reading */
  Serial.println(F("BMP280:"));
  
  Serial.print("  Temp: ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");
   
  Serial.print("  Pressure:");
  Serial.print(bme.readPressure());
  Serial.println(" Pa");
   
  Serial.print("  Altitude:");
  Serial.print(bme.readAltitude(1022)); // this should be adjusted to your local Sea level pressure (hPa)
  Serial.println(" m");

  /* AHT20 reading */
  sensors_event_t humidity;
  sensors_event_t temp;
  aht_humidity->getEvent(&humidity);
  aht_temp->getEvent(&temp);
  
  Serial.println(F("AHT20:"));
  
  Serial.print("  Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");
    
  Serial.print("  Humidity: ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" % rH");

  delay(1000);
}
