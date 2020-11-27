#pragma once

#include "PZEM004T.h"
#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/PowerSensor.h"

///PZEM004T power sensor - website component

class PowerSensor_Web : public PowerSensor, public Common_Web
{
public:
  PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort); ///constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){};      ///Not used
  void websiteEvent_Button(__attribute__((unused)) char *Button){}; ///Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){};   ///Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};