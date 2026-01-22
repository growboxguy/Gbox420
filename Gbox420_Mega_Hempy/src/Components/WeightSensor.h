#pragma once
#include <HX711.h>
// #include "movingAvg.h"
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"

///< Weight sensor load cell
class WeightSensor : virtual public Common
{
public:
  WeightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WeightSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  float readWeight();
  float getWeight();
  char *getWeightText(bool FriendlyFormat = false);
  void tareRequest();
  void triggerCalibration(float KnownWeight);
  void setScale(float Scale);
  float &Scale; ///< Calibration value: Raw reading of one unit
  long &Offset; ///< Read value at no weight on the scale

private:
protected:
  Module *Parent;
  HX711 *Sensor;

  float Weight = 0.0;
  // movingAvg *AverageWeight;
  void tare();
  void calibrate();

  float KnownWeight = 1.0;
  bool TareRequested = false;
  bool CalibrateRequested = false;
};