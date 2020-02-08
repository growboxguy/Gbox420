/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch
 

 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state
 This example code uses bogde's excellent library:"https://github.com/bogde/HX711"
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include "HX711.h"
const byte PumpRelay = 1;
const byte  WeightSensor1_DT =  2;
const byte WeightSensor1_SCK = 3;
const byte  WeightSensor2_DT =  4;
const byte WeightSensor2_SCK = 5;

HX711 scale;
HX711 scale2;

float calibration_factor = 125000; //-7050 worked for my 440lb max scale setu
float calibration_factor2 = 126000; //-7050 worked for my 440lb max scale setu

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  
  scale.begin(WeightSensor1_DT, WeightSensor1_SCK);
  scale2.begin(WeightSensor2_DT, WeightSensor2_SCK);
  scale.set_scale();
  scale2.set_scale();
  scale.tare(); //Reset the scale to 0
  scale2.tare(); //Reset the scale to 0

}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor2); //Adjust to this calibration factor

  ReadScale(&scale);
  ReadScale(&scale2);

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 1000;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 1000;
    else if(temp == 'x')
       {
        scale.tare(); //Reset the scale to 0
        Serial.println("Scale Tare");
       }
  }
  delay(1000);
 }

void ReadScale(HX711 * Sensor){
    Serial.print("Scale, Reading: ");
    Serial.print(Sensor -> read());
    Serial.print(", Reading-tare: ");
    Serial.print(Sensor -> get_value());  
    Serial.print(", Units: ");
    Serial.print(Sensor -> get_units(), 1);
    Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    Serial.print(", Calibration_factor: ");
    Serial.println(Sensor -> get_offset());
  }



  
