#pragma once

#include "movingAvg.h"
#include "420Common.h"
#include "420Module.h"
#include "WaterTempSensor.h"

class TDSSensor : virtual public Common
{
public:
  TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings);
  TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1);
  void refresh_FiveSec();
  void report(bool FriendlyFormat = false);
  float getTDS(bool ReturnAverage = true);
  char *getTDSText(bool ReturnAverage = true, bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  WaterTempSensor *WaterTempSensor1 = NULL;
  uint8_t *Pin;
  float TDS;
  movingAvg *AverageTDS;
  void updateTDS(bool ShowRaw);
};