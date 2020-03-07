#pragma once
#include "420Common.h"
#include "420Module.h"
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
  void waterNow();  ///Turn on water pump, run until StopWeight is reached
  void pumpOff();  ///Turn the pump off
  void checkRelay(); ///Sets the relay controlling the pump
  void checkBucketWeight();
  void setStartWeight(float Weight);
  void setStopWeight(float Weight);
  char *getStopWeightText(bool IncludeUnits);
  char *getStartWeightText(bool IncludeUnits); 

private:  

protected:
  WeightSensor *FeedbackWeightSensor;  ///Weight sensor to monitor the Hempy Bucket's weight, used to figure out when to start and stop watering
  float *StartWeight;  ///Start watering below this weight
  long *StopWeight; ///Stop watering above this weight
  byte *PumpPin;
  bool PumpOn = false;
};