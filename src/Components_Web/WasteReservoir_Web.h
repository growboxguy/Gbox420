#pragma once

#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/WasteReservoir.h"

///< Weight sensor load cell - website component

class WasteReservoir_Web : public WasteReservoir, public Common_Web
{
public:
  WasteReservoir_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WasteReservoirSettings *DefaultSettings, WeightSensor *WasteWeightSensor);
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};