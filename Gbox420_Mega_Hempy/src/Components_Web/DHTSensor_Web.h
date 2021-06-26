#pragma once

#include "DHT.h"
#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/DHTSensor.h"

///< Digital Humidity and Temperature sensor Web (DHT11 or DHT22) - website component

class DHTSensor_Web : public DHTSensor, public Common_Web
{
public:
  DHTSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DHTSensorSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){};      ///< Not used
  void websiteEvent_Button(__attribute__((unused)) char *Button){}; ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){};   ///< Not used

private:
protected:
  Module_Web *Parent;
};