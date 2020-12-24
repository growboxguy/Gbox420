#pragma once

#include "420Common.h"
#include "420Module.h"
#include "WaterTempSensor.h"

class TDSSensor : virtual public Common
{
public:
  TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report();
  float getTDS(bool ReturnAverage = true);
  char *getTDSText(bool ReturnAverage = true);

private:
protected:
  Module *Parent;
  WaterTempSensor *WaterTempSensor1 = NULL;
  uint8_t *Pin;
  RollingAverage *TDS;
  void updateTDS(bool ShowRaw);
};