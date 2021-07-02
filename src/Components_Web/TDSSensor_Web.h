
#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/TDSSensor.h"

///< TDS Meter V1.0 for Arduino

class TDSSensor_Web : public TDSSensor, public Common_Web
{
public:
  TDSSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);

private:
protected:
  Module_Web *Parent;
};