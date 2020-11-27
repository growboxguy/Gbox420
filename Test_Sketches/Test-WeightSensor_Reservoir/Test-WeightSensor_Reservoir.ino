//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating the reservoir weight sensor

#include "HX711.h"

//Pins
const byte WeightSensor_DT = 5;
const byte WeightSensor_SCK = 6;

//Weights used during the calibration
const float WeightSensor_CalibrationWeight = 10.0; //SET THIS TO the number of weight units (kg or lbs) that will be used during the calibration

//Initial calibration values, once the calibration you can update these values and change CalibrationComplete to true
bool CalibrationComplete = false; //Set this to true to skip the calibration and use the Offset and Scale values defined in the sketch
long WeightSensor_Offset = 255457;
float WeightSensor_Scale = -19615.0;

HX711 WeightSensor;
float WeightSensor_Weight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the reservoir weight sensor"));
  Serial.println();

  WeightSensor.begin(WeightSensor_DT, WeightSensor_SCK);
  WeightSensor.set_offset(WeightSensor_Offset);
  WeightSensor.set_scale(WeightSensor_Scale);

  if (CalibrationComplete)
  {
    Serial.println(F("Calibration is already complete, reading weight..."));
  }
  else
  {
    Serial.println(F("!!Start the sketch with all weight removed from scale!!"));
    Serial.println(F("!!Use the reset button to restart the sketch if any weight was left on the scale!!"));
    Serial.println();

    Serial.println(F("Taring WeightSensor..."));
    WeightSensor.tare(); //Reset the scale to 0
    WeightSensor_Offset = WeightSensor.get_offset();
    Serial.print(F("  WeightSensor offset: "));
    Serial.println(WeightSensor_Offset);

    Serial.println();
    Serial.println(F("Please place the following calibration weight on the scale:"));
    Serial.print(F("   WeightSensor: "));
    Serial.print(WeightSensor_CalibrationWeight);
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

    Serial.println(F("WeightSensor calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(WeightSensor_Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(WeightSensor_Scale);
    Serial.println();

    Serial.println(F("Calibration is complete, reading weight..."));
    Serial.println();
  }

  if (CalibrationComplete)
  {
    WeightSensor_Weight = WeightSensor.get_units();
    Serial.print(F("WeightSensor weight: "));
    Serial.print(WeightSensor_Weight);
    Serial.println(F(" units"));
  }
  else //When sensors are under calibration
  {
    if (WeightSensor.wait_ready_timeout(200))
    {
      WeightSensor_Scale = WeightSensor.get_value() / WeightSensor_CalibrationWeight;
      WeightSensor.set_scale(WeightSensor_Scale);
      WeightSensor_Weight = WeightSensor.get_units();
      Serial.print(F("WeightSensor Scale: "));
      Serial.print(WeightSensor_Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightSensor_Weight);
    }
  }
  Serial.println();
  delay(10000);
}
