//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating the weight sensors for the Hempy module - Hempy Bucket 1 and Waste Reservoir 1 

#include "HX711.h"

//Pins
const byte WeightB1_DT = 4;  //Hempy Bucket 1 - HX711 weight sensor DT pin
const byte WeightB1_SCK = 6; //Hempy Bucket 1 - HX711 weight sensor SCK pin
const byte WeightWR1_DT = A0; //Waste Reservoir 1 - HX711 weight sensor DT pin
const byte WeightWR1_SCK = A1; //Waste Reservoir 1 - HX711 weight sensor SCK pin

//Known weights used during the calibration
const float WeightB1_CalibrationWeight = 10.0; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration
const float WeightWR1_CalibrationWeight = 10.0; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration

//Initial calibration values, once the calibration you can update these values and change CalibrationComplete to true
bool CalibrationComplete = false; //Set this to true to skip the calibration and use the Offset and Scale values defined in the sketch
long WeightB1_Offset = -133716;
float WeightB1_Scale = -7369.10;
long WeightWR1_Offset = -76382;
float WeightWR1_Scale = -22697.10;

HX711 WeightB1;
HX711 WeightWR1;
float WeightB1_Weight = 0;
float WeightWR1_Weight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the Hempy Module - Bucket 1 and Waste Reservoir 1 weight sensors"));
  Serial.println();

  WeightB1.begin(WeightB1_DT, WeightB1_SCK);
  WeightB1.set_offset(WeightB1_Offset);
  WeightB1.set_scale(WeightB1_Scale);
  WeightWR1.begin(WeightWR1_DT, WeightWR1_SCK);
  WeightWR1.set_offset(WeightWR1_Offset);
  WeightWR1.set_scale(WeightWR1_Scale);

  if (CalibrationComplete)
  {
    Serial.println(F("Calibration is already complete, reading weight..."));
  }
  else
  {
    Serial.println(F("!!Start the sketch with the weights removed from the scales!!"));
    Serial.println(F("!!Use the reset button to restart the sketch if any weight was left on the scale!!"));
    Serial.println();

    Serial.println(F("Taring Hempy Bucket 1 - WeightB1..."));
    WeightB1.tare(); //Reset the scale to 0
    WeightB1_Offset = WeightB1.get_offset();
    Serial.print(F("  WeightB1 offset: "));
    Serial.println(WeightB1_Offset);

    Serial.println(F("Taring Waste Reservoir 1 - WeightWR1..."));
    WeightWR1.tare(); //Reset the scale to 0
    WeightWR1_Offset = WeightWR1.get_offset();
    Serial.print(F("  WeightWR1 offset: "));
    Serial.println(WeightWR1_Offset);

    Serial.println();
    Serial.println(F("Please place the following calibration weights on the scales:"));
    Serial.print(F("   Hempy Bucket 1 - WeightB1: "));
    Serial.print(WeightB1_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    Serial.print(F("   Waste Reservoir 1 - WeightWR1: "));
    Serial.print(WeightWR1_CalibrationWeight);
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

    Serial.println(F("Hempy Bucket 1 - WeightB1 calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightB1_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightB1_Scale);
    Serial.println();

    Serial.println(F("Waste Reservoir 1 - WeightWR1 calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightWR1_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightWR1_Scale);
    Serial.println();

    Serial.println(F("Calibration is complete, reading weight..."));
    Serial.println();
  }

  if (CalibrationComplete)
  {
    WeightB1_Weight = WeightB1.get_units();
    Serial.print(F("WeightB1 weight: "));
    Serial.print(WeightB1_Weight);
    Serial.println(F(" units"));

    WeightWR1_Weight = WeightWR1.get_units();
    Serial.print(F("WeightWR1 weight: "));
    Serial.print(WeightWR1_Weight);
    Serial.println(F(" units"));
  }
  else //When sensors are under calibration
  {
    if (WeightB1.wait_ready_timeout(200))
    {
      WeightB1_Scale = WeightB1.get_value() / WeightB1_CalibrationWeight;
      WeightB1.set_scale(WeightB1_Scale);
      WeightB1_Weight = WeightB1.get_units();
      Serial.print(F("WeightB1 Scale: "));
      Serial.print(WeightB1_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightB1_Weight);
    }

    if (WeightWR1.wait_ready_timeout(200))
    {
      WeightWR1_Scale = WeightWR1.get_value() / WeightWR1_CalibrationWeight;
      WeightWR1.set_scale(WeightWR1_Scale);
      WeightWR1_Weight = WeightWR1.get_units();
      Serial.print(F("WeightWR1 Scale: "));
      Serial.print(WeightWR1_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightWR1_Weight);
    }
  }
  Serial.println();
  delay(10000);
}
