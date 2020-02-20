#pragma once
#include "HX711.h"
#include "420Common.h"
#include "420Module.h"

///UNDER DEVELOPMENT - Weight sensor load cell

class WeightSensor : virtual public Common
{
public:
  WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void refresh_Minute();
  void refresh_Sec();
  void report();
  void readWeight();
  float getWeight(bool ReturnAverage = true);
  char *getWeightText(bool IncludeUnits, bool ReturnAverage);
  void triggerTare();
  void triggerCalibration(int CalibrationWeight);  
  RollingAverage *Weight;

private:  

protected:
  Module* Parent;
  HX711 *Sensor;
  void tare();
  void calibrate();
  void setScale(float Scale);
  float *Scale;  ///Calibration value: Raw reading of one unit
  long *TareOffset; ///Read value at no weight on the scale
  int CalibrationWeight;
  bool TareRequested = false;
  bool CalibrateRequested = false;
};