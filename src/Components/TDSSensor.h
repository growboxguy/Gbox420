#pragma once

#include "420Common.h"
#include "420Module.h"
#include "WaterTempSensor.h"

class TDSSensor : virtual public Common
{
public:
  TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings);
  TDSSensor(const __FlashStringHelper *Name, Module *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1);
  void refresh_Minute();
  void report(bool FriendlyFormat = false);
  float getTDS();
  char *getTDSText(bool FriendlyFormat = false);

private:
protected:
  Module *Parent;
  WaterTempSensor *WaterTempSensor1 = NULL;
  uint8_t *Pin;       //A port - Analog tds reading
  uint8_t *PowerPin;  //+ port - Power to the module is provided by an Arduino I/O pin (analog or digital) Up to 40mA supported (KS0429 needs 6mA). The module is only powered on while taking a reading - Else it messes with pH readings
  float TDS;
  void updateTDS(bool ShowRaw);
};