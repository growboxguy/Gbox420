#pragma once

#include "movingAvg.h"
#include "420Common.h"
#include "420Module.h"

///< Pressure sensor
class PressureSensor : virtual public Common
{
public:
  PressureSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PressureSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  float readPressure(bool ReturnAverage = true);
  float getPressure(bool ReturnAverage = true);
  char *getPressureText(bool ReturnAverage = true, bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  float Pressure;
  movingAvg *AveragePressure;
  void readOffset();
  void setOffset(float Value);
  void setRatio(float Value);
  const uint8_t &Pin;
  float &Offset; 
  float &Ratio;  
};