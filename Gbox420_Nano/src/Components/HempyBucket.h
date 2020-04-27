#pragma once
#include "420Common.h"
#include "420Module.h"
#include "WeightSensor.h"

//struct HempyBucketSettings HempyB1 = {.StartWateringWeight = 4.9,.StopWateringWeight = 6.5, .PumpEnabled = true, .PumpTimeout = 120,};  ///Default settings for the Hempy Module
//struct HempyBucketSettings HempyB2 = {.StartWateringWeight = 4.9,.StopWateringWeight = 6.5, .PumpEnabled = true, .PumpTimeout = 120,};  ///Default settings for the Hempy Module
  

class HempyBucket : virtual public Common
{
public:
  HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings, WeightSensor *BucketWeightSensor);
  void refresh_FiveSec();
  void refresh_Sec();
  void report();
  void checkWeight();
  float *StartWateringWeight;
  float *StopWateringWeight;
  bool *PumpEnabled;
  int *PumpTimeout;
  bool PumpOn = false;
  
  /* char *getWeightText(bool IncludeUnits);
  void triggerTare();
  void triggerCalibration(int CalibrationWeight); 
  void setScale(float Scale); 
  float Weight;
  float *Scale;  ///Calibration value: Raw reading of one unit
  long *TareOffset; ///Read value at no weight on the scale */

private:  

protected:
  Module* Parent;
  WeightSensor *BucketWeightSensor;
/*   void tare();
  void calibrate();

  int CalibrationWeight;
  bool TareRequested = false;
  bool CalibrateRequested = false; */
};