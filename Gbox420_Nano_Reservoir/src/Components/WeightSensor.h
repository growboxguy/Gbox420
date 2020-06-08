#pragma once
#include "HX711.h"
#include "420Common.h"
#include "420Module.h"

class WeightSensor : virtual public Common
{
public:
  WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void refresh_Sec();
  void report();
  void readWeight();
  float getWeight();
  char *getWeightText(bool IncludeUnits = false);
  void triggerTare();
  void triggerCalibration(int CalibrationWeight); 
  void setScale(float Scale); 
  float Weight;
  float *Scale;  ///Calibration value: Raw reading of one unit
  long *TareOffset; ///Read value at no weight on the scale

private:  

protected:
  Module* Parent;
  HX711 *Sensor;
  void tare();
  void calibrate();
  
  int CalibrationWeight;
  bool TareRequested = false;
  bool CalibrateRequested = false;
};