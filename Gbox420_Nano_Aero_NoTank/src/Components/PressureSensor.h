#pragma once

#include "420Common.h"
#include "420Module.h"

///Pressure sensor

class PressureSensor : virtual public Common
{
public:
  PressureSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report();
  void readPressure();
  float getPressure(bool ReturnAverage = true);
  char *getPressureText(bool IncludeUnits, bool ReturnAverage);
  RollingAverage *Pressure;

private:  
  
protected:
  Module *Parent;
  void readOffset();
  void setOffset(float Value);
  void setRatio(float Value);
  uint8_t *Pin;
  float *Offset;
  float *Ratio;
};