#pragma once

#include "420Common.h"
#include "420Module.h"

class PHSensor : virtual public Common
{
public:
  PHSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PHSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report();
  float getPH(bool ReturnAverage = true);
  char *getPHText(bool ReturnAverage = true);

private:
protected:
  Module *Parent;
  uint8_t *Pin;
  RollingAverage *PH;
  float *Intercept;
  float *Slope;
  void updatePH(bool ShowRaw);
  void setSlope(float Value);
  void setIntercept(float Value);
};