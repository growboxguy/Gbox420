//GrowBoxGuy - https://sites.google.com/site/growboxguy/hempy-bucket/hempy-module#h.p_IOKMKqglaoNq
//Sketch for calibrating the weight sensors for the Hempy module - Hempy Bucket 2 and Waste Reservoir 2 

#include "HX711.h"

//Pins
const byte WeightB2_DT = 7;  //Hempy Bucket 2 - HX711 weight sensor DT pin
const byte WeightB2_SCK = 8; //Hempy Bucket 2 - HX711 weight sensor SCK pin
const byte WeightWR2_DT = A2; //Waste Reservoir 2 - HX711 weight sensor DT pin
const byte WeightWR2_SCK = A3; //Waste Reservoir 2 - HX711 weight sensor SCK pin

//Known weights used during the calibration
const float WeightB2_CalibrationWeight = 10.0; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration
const float WeightWR2_CalibrationWeight = 10.0; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration

//Initial calibration values, once the calibration you can update these values and change CalibrationComplete to true
bool CalibrationComplete = false; //Set this to true to skip the calibration and use the Offset and Scale values defined in the sketch
long WeightB2_Offset = -182833;
float WeightB2_Scale = -22089.00;
long WeightWR2_Offset = 260682;
float WeightWR2_Scale = -22084.60;

HX711 WeightB2;
HX711 WeightWR2;
float WeightB2_Weight = 0;
float WeightWR2_Weight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the Hempy Module - Bucket 1 and Waste Reservoir 2 weight sensors"));
  Serial.println();

  WeightB2.begin(WeightB2_DT, WeightB2_SCK);
  WeightB2.set_offset(WeightB2_Offset);
  WeightB2.set_scale(WeightB2_Scale);
  WeightWR2.begin(WeightWR2_DT, WeightWR2_SCK);
  WeightWR2.set_offset(WeightWR2_Offset);
  WeightWR2.set_scale(WeightWR2_Scale);

  if (CalibrationComplete)
  {
    Serial.println(F("Calibration is already complete, reading weight..."));
  }
  else
  {
    Serial.println(F("!!Start the sketch with the weights removed from the scales!!"));
    Serial.println(F("!!Use the reset button to restart the sketch if any weight was left on the scale!!"));
    Serial.println();

    Serial.println(F("Taring Hempy Bucket 2 - WeightB2..."));
    WeightB2.tare(); //Reset the scale to 0
    WeightB2_Offset = WeightB2.get_offset();
    Serial.print(F("  WeightB2 offset: "));
    Serial.println(WeightB2_Offset);

    Serial.println(F("Taring Waste Reservoir 2 - WeightWR2..."));
    WeightWR2.tare(); //Reset the scale to 0
    WeightWR2_Offset = WeightWR2.get_offset();
    Serial.print(F("  WeightWR2 offset: "));
    Serial.println(WeightWR2_Offset);

    Serial.println();
    Serial.println(F("Please place the following calibration weights on the scales:"));
    Serial.print(F("   Hempy Bucket 2 - WeightB2: "));
    Serial.print(WeightB2_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    Serial.print(F("   Waste Reservoir 2 - WeightWR2: "));
    Serial.print(WeightWR2_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    delay(10000);
    Serial.println();
    Serial.println(F("Calibrating every 10 seconds"));
    Serial.println(F("Wait for the readings to stabilize,"));
    Serial.println(F("send any character on Serial to finish the calibration..."));
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

    Serial.println(F("Hempy Bucket 2 - WeightB2 calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightB2_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightB2_Scale);
    Serial.println();

    Serial.println(F("Waste Reservoir 2 - WeightWR2 calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightWR2_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightWR2_Scale);
    Serial.println();

    Serial.println(F("Calibration is complete, reading weight..."));
    Serial.println();
  }

  if (CalibrationComplete)
  {
    WeightB2_Weight = WeightB2.get_units();
    Serial.print(F("WeightB2 weight: "));
    Serial.print(WeightB2_Weight);
    Serial.println(F(" units"));

    WeightWR2_Weight = WeightWR2.get_units();
    Serial.print(F("WeightWR2 weight: "));
    Serial.print(WeightWR2_Weight);
    Serial.println(F(" units"));
  }
  else //When sensors are under calibration
  {
    if (WeightB2.wait_ready_timeout(200))
    {
      WeightB2_Scale = WeightB2.get_value() / WeightB2_CalibrationWeight;
      WeightB2.set_scale(WeightB2_Scale);
      WeightB2_Weight = WeightB2.get_units();
      Serial.print(F("WeightB2 Scale: "));
      Serial.print(WeightB2_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightB2_Weight);
    }

    if (WeightWR2.wait_ready_timeout(200))
    {
      WeightWR2_Scale = WeightWR2.get_value() / WeightWR2_CalibrationWeight;
      WeightWR2.set_scale(WeightWR2_Scale);
      WeightWR2_Weight = WeightWR2.get_units();
      Serial.print(F("WeightWR2 Scale: "));
      Serial.print(WeightWR2_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightWR2_Weight);
    }
  }
  Serial.println();
  delay(10000);
}
