#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "WeightSensor.h"

///UNDER DEVELOPMENT - Class representing a single hempy Bucket
///Weight sensor measures the bucket weight and watering is based on the measured weight

class HempyBucket : virtual public Common
{
public:
  HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *FeedbackWeightSensor);
  void refresh_Minute();
  void refresh_Sec();
  void report();
  void waterNow(bool UserRequest);  ///Turn on water pump, run until StopWeight is reached
  void pumpOff(bool UserRequest);  ///Turn the pump off
  void checkRelay(); ///Sets the relay controlling the pump
  void checkBucketWeight();
  void setStartWeight(float Weight);
  void setStopWeight(float Weight);
  char *getStopWeightText(bool IncludeUnits);
  char *getStartWeightText(bool IncludeUnits); 

private:  

protected:
  Module *Parent;
  WeightSensor *FeedbackWeightSensor;  ///Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  byte *PumpPin;
  bool *PumpEnabled;  ///Enable/disable watering based on weight
  bool PumpOn = false;  
  int *PumpTimeout;  ///Max pump run time in seconds
  uint32_t PumpTimer = millis();  ///Used for Pump runtime tracking
  float *StartWeight;  ///Start watering below this weight
  float *StopWeight; ///Stop watering above this weight
};