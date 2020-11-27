//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating two weight sensors connected to a single HX711 load cell amplifier
//Uses both channel A and B

#include "HX711.h"

//Pins
const byte WeightSensor_DT = 5;
const byte WeightSensor_SCK = 6;

//Weights used during the calibration
const float WeightSensor_A_CalibrationWeight = 2.0; //SET THIS TO the number of weight units (kg or lbs) that will be used during the calibration
const float WeightSensor_B_CalibrationWeight = 2.0; //SET THIS TO the number of weight units (kg or lbs) that will be used during the calibration

//Initial calibration values, once the calibration you can update these values and change CalibrationComplete to true
bool CalibrationComplete = false; //Set this to true to skip the calibration and use the Offset and Scale values defined in the sketch
long WeightSensor_A_Offset = -115149;
float WeightSensor_A_Scale = -25084.50;
long WeightSensor_B_Offset = -3198;
float WeightSensor_B_Scale = -179337.50;

HX711 WeightSensor;
float WeightSensor_A_Weight = 0;
float WeightSensor_B_Weight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the hempy bucket weight sensors"));
  Serial.println();

  WeightSensor.begin(WeightSensor_DT, WeightSensor_SCK);

  if (CalibrationComplete)
  {
    Serial.println(F("Calibration is already complete, reading weight..."));
  }
  else
  {
    Serial.println(F("!!Start the sketch with all weight removed from the scale!!"));
    Serial.println(F("!!Use the reset button to restart the sketch if any weight was left on the scale!!"));
    Serial.println();

    Serial.println(F("Taring Channel A"));
    WeightSensor.set_gain(128); //Change to Channel A
    WeightSensor.wait_ready(100);
    WeightSensor.tare(); //Reset the scale to 0
    WeightSensor_A_Offset = WeightSensor.get_offset();
    Serial.print(F("  Channel A offset: "));
    Serial.println(WeightSensor_A_Offset);

    Serial.println(F("Taring Channel B"));
    WeightSensor.set_gain(32); //Change to Channel B
    WeightSensor.wait_ready(100);
    WeightSensor.tare(); //Reset the scale to 0
    WeightSensor_B_Offset = WeightSensor.get_offset();
    Serial.print(F("  Channel B offset: "));
    Serial.println(WeightSensor_B_Offset);

    Serial.println();
    Serial.println(F("Please place the following calibration weights on the scales:"));
    Serial.print(F("   WeightSensor - Channel A: "));
    Serial.print(WeightSensor_A_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    Serial.print(F("   WeightSensor - Channel B: "));
    Serial.print(WeightSensor_B_CalibrationWeight);
    Serial.println(F(" unit(s)"));
    delay(5000);
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

    Serial.println(F("Channel A calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightSensor_A_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightSensor_A_Scale);
    Serial.println();

    Serial.println(F("Channel B calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.println(WeightSensor_B_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightSensor_B_Scale);
    Serial.println();

    Serial.println(F("Calibration is complete, reading weight..."));
    Serial.println();
  }

  if (CalibrationComplete)
  {
    WeightSensor.set_gain(128);                     //Change to Channel A
    WeightSensor.set_offset(WeightSensor_A_Offset); //Set the calibration values
    WeightSensor.set_scale(WeightSensor_A_Scale);   //Set the calibration values
    WeightSensor.wait_ready(100);
    WeightSensor_A_Weight = WeightSensor.get_units(10);
    Serial.print(F("WeightSensor A channel weight: "));
    Serial.print(WeightSensor_A_Weight);
    Serial.println(F(" units"));

    WeightSensor.set_gain(32);                      //Change to Channel B
    WeightSensor.set_offset(WeightSensor_B_Offset); //Set the calibration values
    WeightSensor.set_scale(WeightSensor_B_Scale);   //Set the calibration values
    WeightSensor.wait_ready(100);
    WeightSensor_B_Weight = WeightSensor.get_units(10);
    Serial.print(F("WeightSensor B channel weight: "));
    Serial.print(WeightSensor_B_Weight);
    Serial.println(F(" units"));
  }
  else //When sensors are under calibration
  {
    WeightSensor.set_gain(128); //Change to Channel A
    WeightSensor.wait_ready(100);
    WeightSensor_A_Scale = WeightSensor.get_value(10) / WeightSensor_A_CalibrationWeight;
    WeightSensor.set_scale(WeightSensor_A_Scale);
    WeightSensor.wait_ready(100);
    WeightSensor_A_Weight = WeightSensor.get_units(10);
    Serial.print(F("WeightSensor Channel A Scale: "));
    Serial.print(WeightSensor_A_Scale);
    Serial.print(F(" , Reading: "));
    Serial.println(WeightSensor_A_Weight);

    WeightSensor.set_gain(32); //Change to Channel B
    WeightSensor.wait_ready(100);
    WeightSensor_B_Scale = WeightSensor.get_value(10) / WeightSensor_B_CalibrationWeight;
    WeightSensor.set_scale(WeightSensor_B_Scale);
    WeightSensor.wait_ready(100);
    WeightSensor_B_Weight = WeightSensor.get_units(10);
    Serial.print(F("WeightSensor Channel B Scale: "));
    Serial.print(WeightSensor_B_Scale);
    Serial.print(F(" , Reading: "));
    Serial.println(WeightSensor_B_Weight);
  }
  Serial.println();
  delay(10000);
}
