//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating the bucket weight sensors on the 2 platform table

#include "HX711.h"

//Pins
const byte WeightSensor1_DT = 4;
const byte WeightSensor1_SCK = 6;
const byte WeightSensor2_DT = 7;
const byte WeightSensor2_SCK = 8;

//Weights used during the calibration
const float WeightSensor1_CalibrationWeight = 10.0; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration
const float WeightSensor2_CalibrationWeight = 10.0; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration

//Initial calibration values, once the calibration you can update these values and change CalibrationComplete to true
bool CalibrationComplete = false; //Set this to true to skip the calibration and use the Offset and Scale values defined in the sketch
long WeightSensor1_Offset = 378161;
float WeightSensor1_Scale = -21484.20;
long WeightSensor2_Offset = -182833;
float WeightSensor2_Scale = -22089.00;

HX711 WeightSensor1;
HX711 WeightSensor2;
float WeightSensor1_Weight = 0;
float WeightSensor2_Weight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the hempy bucket weight sensors"));
  Serial.println();

  WeightSensor1.begin(WeightSensor1_DT, WeightSensor1_SCK);
  WeightSensor1.set_offset(WeightSensor1_Offset);
  WeightSensor1.set_scale(WeightSensor1_Scale);
  WeightSensor2.begin(WeightSensor2_DT, WeightSensor2_SCK);
  WeightSensor2.set_offset(WeightSensor2_Offset);
  WeightSensor2.set_scale(WeightSensor2_Scale);

  if (CalibrationComplete)
  {
    Serial.println(F("Calibration is already complete, reading weight..."));
  }
  else
  {
    Serial.println(F("!!Start the sketch with all weight removed from the scale!!"));
    Serial.println(F("!!Use the reset button to restart the sketch if any weight was left on the scale!!"));
    Serial.println();

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

    Serial.println();
    Serial.println(F("Please place the following calibration weights on the scales:"));
    Serial.print(F("   WeightSensor1: "));
    Serial.print(WeightSensor1_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    Serial.print(F("   WeightSensor2: "));
    Serial.print(WeightSensor2_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    delay(10000);
    Serial.println();
    Serial.println(F("Calibrating every 10 seconds"));
    Serial.println(F("Send any character on Serial to finish the calibration..."));
    Serial.println();
  }
}

void loop()
{
  if (!CalibrationComplete && Serial.available())
  {
    CalibrationComplete = true;
    Serial.print(F("Serial input received: "));
    Serial.print(Serial.readString());
    Serial.println(F("Stopping calibration..."));
    Serial.println();

    Serial.println(F("WeightSensor1 calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightSensor1_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightSensor1_Scale);
    Serial.println();

    Serial.println(F("WeightSensor2 calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightSensor2_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightSensor2_Scale);
    Serial.println();

    Serial.println(F("Calibration is complete, reading weight..."));
    Serial.println();
  }

  if (CalibrationComplete)
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
  else //When sensors are under calibration
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
