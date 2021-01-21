#pragma once
#include "HX711.h"
#include "movingAvgFloat.h"
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"

///< Weight sensor load cell
class WeightSensor : virtual public Common
{
public:
  WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report();
  float readWeight(bool ReturnAverage=true);
  float getWeight(bool ReturnAverage=true);
  char *getWeightText(bool ReturnAverage=true, bool IncludeUnits = false);
  void triggerTare();
  void triggerCalibration(int CalibrationWeight);
  void setScale(float Scale);  
  float *Scale; ///< Calibration value: Raw reading of one unit
  long *Offset; ///< Read value at no weight on the scale

private:
protected:
  Module *Parent;
  HX711 *Sensor;
   
  float Weight = 0.0;
  movingAvgFloat *AverageWeight; 
  void tare();
  void calibrate();

  int CalibrationWeight;
  bool TareRequested = false;
  bool CalibrateRequested = false;
};