#pragma once

#include "movingAvg.h"
#include "420Common.h"
#include "420Module.h"

class PHSensor : virtual public Common
{
public:
  PHSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PHSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  float getPH(bool ReturnAverage = true);
  char *getPHText(bool ReturnAverage = true);

private:
protected:
  Module *Parent;
  uint8_t &Pin;
  float PH;
  movingAvg *AveragePH;
  float &Intercept;
  float &Slope;
  void updatePH(bool ShowRaw);
  void setSlope(float Value);
  void setIntercept(float Value);
};