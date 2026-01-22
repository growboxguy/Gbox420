//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for calibrating the weight sensors for the Hempy module - Hempy Bucket 1 and Waste Reservoir 1 

#include "HX711.h"

//Pins
const byte DTPin = 5;  //Hempy Bucket 1 - HX711 weight sensor DT pin
const byte SCKPin = 6; //Hempy Bucket 1 - HX711 weight sensor SCK pin

//Known weights used during the calibration
const float CalibrationWeight = 4.739; //SET THIS TO the reference weight units (kg or lbs) that will be used during the calibration

//Initial calibration values, once the calibration you can update these values and change CalibrationComplete to true
bool CalibrationComplete = true; //Set this to true to skip the calibration and use the Offset and Scale values defined in the sketch
long Offset = 252769;
float Scale = -21033.98;


HX711 WeightSensor;
float WeightSensor_Weight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for calibrating the Hempy Module - Bucket 1 and Waste Reservoir 1 weight sensors"));
  Serial.println();

  WeightSensor.begin(DTPin, SCKPin);
  WeightSensor.set_offset(Offset);
  WeightSensor.set_scale(Scale);
  

  if (CalibrationComplete)
  {
    Serial.println(F("Calibration is already complete, reading weight..."));
  }
  else
  {
    Serial.println(F("!!Start the sketch with the weights removed from the scales!!"));
    Serial.println(F("!!Use the reset button to restart the sketch if any weight was left on the scale!!"));
    Serial.println();

    Serial.println(F("Taring Reservoir weight sensor..."));
    WeightSensor.tare(); //Reset the scale to 0
    Offset = WeightSensor.get_offset();
    Serial.print(F("  Reservoir offset: "));
    Serial.println(Offset);

    
    Serial.println();
    Serial.println(F("Please place the following calibration weights on the scale:"));
    Serial.print(CalibrationWeight);
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

    Serial.println(F("Reservoir weight platform calibration complete:"));
    Serial.print(F("  .Offset = "));
    Serial.print(Offset);
    Serial.print(F(", .Scale = "));
    Serial.println(Scale);
    Serial.println();

  
    Serial.println(F("Calibration is complete, reading weight..."));
    Serial.println();
  }

  if (CalibrationComplete)
  {
    WeightSensor_Weight =WeightSensor.get_units();
    Serial.print(F("Reservoir weight: "));
    Serial.print(WeightSensor_Weight);
    Serial.println(F(" units"));
  }
  else //When sensors are under calibration
  {
    if (WeightSensor.wait_ready_timeout(200))
    {
      Scale = WeightSensor.get_value() / CalibrationWeight;
      WeightSensor.set_scale(Scale);
      WeightSensor_Weight = WeightSensor.get_units();
      Serial.print(F("Reservoir Scale: "));
      Serial.print(Scale);
      Serial.print(F(" , Reading: "));
      Serial.println(WeightSensor_Weight);
    }
   
  }
  Serial.println();
  delay(10000);
}
