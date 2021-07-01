#pragma once

#include "DHT.h"
#include "../Components/DHTSensor.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components_Web/420Common_Web.h"

///< Digital Humidity and Temperature sensor Web (DHT11 or DHT22) - website component

class DHTSensor_Web : public Common_Web, public DHTSensor
{
public:
  DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);

private:
protected:
  Module_Web *Parent;
};