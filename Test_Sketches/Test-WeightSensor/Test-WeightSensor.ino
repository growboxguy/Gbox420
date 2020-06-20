//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating the hempy bucket weight sensors

#include "HX711.h"

const byte  WeightSensor1_DT =  5;
const byte WeightSensor1_SCK = 6;
const byte  WeightSensor2_DT =  7;
const byte WeightSensor2_SCK = 8;

HX711 WeightSensor1;
HX711 WeightSensor2;

float WeightSensor1_Weight = 0;
float WeightSensor2_Weight = 0;
float WeightSensor1_Scale = 129509.50;
float WeightSensor2_Scale = 121565.00;
float WeightSensor1_Offset = 267728.00;
float WeightSensor2_Offset = 156819.00;
const byte WeightSensor1_CalibrationWeight = 2;  //The number of weight units (kg or lbs) that will be used during the calibration
const byte WeightSensor2_CalibrationWeight = 2;  //The number of weight units (kg or lbs) that will be used during the calibration

bool CalibrationComplete = false;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the hempy bucket weight sensors"));
  Serial.println(F("!!Start the sketch with all weight remove from scale!!"));
  Serial.println();
  WeightSensor1.begin(WeightSensor1_DT, WeightSensor1_SCK);
  WeightSensor1.set_scale(WeightSensor1_Scale);

  WeightSensor2.begin(WeightSensor2_DT, WeightSensor2_SCK);
  WeightSensor2.set_scale(WeightSensor2_Scale);

  Serial.println(F("Taring WeightSensor1..."));
  WeightSensor1.tare(); //Reset the scale to 0  
  WeightSensor1_Offset = WeightSensor1.get_offset();
  Serial.print(F("  WeightSensor1 offset: "));
  Serial.println(WeightSensor1_Offset);

  Serial.println(F("Taring WeightSensor2..."));
  WeightSensor2.tare(); //Reset the scale to 0  
  WeightSensor2_Offset = WeightSensor2.get_offset();
  Serial.print(F("  WeightSensor2 offset: "));
  Serial.println(WeightSensor2_Offset);
  
  Serial.println(F("Taring done."));
  Serial.println();
  Serial.println();
  Serial.println(F("Please place the following calibration weights on the scales:"));
  Serial.print(F("   WeightSensor1: "));
  Serial.print(WeightSensor1_CalibrationWeight);
  Serial.println(F(" unit(s)"));
  Serial.print(F("   WeightSensor2: "));
  Serial.print(WeightSensor1_CalibrationWeight);
  Serial.println(F(" unit(s)"));
  delay(10000);
  Serial.println();
  Serial.println(F("Calibrating every 10 seconds"));
  Serial.println(F("Send any character on Serial to finish the calibration..."));
  Serial.println();
}

void loop() {
  if(!CalibrationComplete && Serial.available())
  {
    CalibrationComplete = true;
    Serial.println(F("WeightSensor1 calibration complete:"));
    Serial.print(F("  Scale: "));
    Serial.println(WeightSensor1_Scale);
    Serial.print(F("  Offset: "));
    Serial.println(WeightSensor1_Offset);
    Serial.println();

    Serial.println(F("WeightSensor2 calibration complete:"));
    Serial.print(F("  Scale: "));
    Serial.println(WeightSensor2_Scale);
    Serial.print(F("  Offset: "));
    Serial.println(WeightSensor2_Offset);
    Serial.println();
  }

  if(CalibrationComplete)
  {
    WeightSensor1_Weight = WeightSensor1.get_units();
    Serial.print(F("WeightSensor1 weight: "));
    Serial.print(WeightSensor1_Weight);
    Serial.println(F(" units"));

    WeightSensor2_Weight = WeightSensor2.get_units();
    Serial.print(F("WeightSensor2 weight: "));
    Serial.print(WeightSensor2_Weight);
    Serial.println(F(" units"));
  }
  else
  {
    if (WeightSensor1.wait_ready_timeout(200)) 
    {
      WeightSensor1_Scale = WeightSensor1.get_value() / WeightSensor1_CalibrationWeight;
      WeightSensor1.set_scale(WeightSensor1_Scale);
      WeightSensor1_Weight = WeightSensor1.get_units();
      Serial.print(F("WeightSensor1 Scale: "));
      Serial.print(WeightSensor1_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightSensor1_Weight);
    }
  
    if (WeightSensor2.wait_ready_timeout(200)) 
    {
      WeightSensor2_Scale = WeightSensor2.get_value() / WeightSensor2_CalibrationWeight;
      WeightSensor2.set_scale(WeightSensor2_Scale);
      WeightSensor2_Weight = WeightSensor2.get_units();
      Serial.print(F("WeightSensor2 Scale: "));
      Serial.print(WeightSensor2_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightSensor2_Weight);
    }
  }
    
  Serial.println();
  delay(10000);
} 
