
#pragma once

#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/TDSSensor.h"

///< TDS Meter V1.0 for Arduino

class TDSSensor_Web : public TDSSensor, public Common_Web
{
public:
  TDSSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::TDSSensorSettings *DefaultSettings, WaterTempSensor *WaterTempSensor1);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button){};    ///< Not used
  void websiteEvent_Load(__attribute__((unused)) char *url){};    ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){}; ///< Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};