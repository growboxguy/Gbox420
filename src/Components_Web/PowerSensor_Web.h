#pragma once

#include "PZEM004T.h"
#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/PowerSensor.h"

///< PZEM004T power sensor - website component

class PowerSensor_Web : public PowerSensor, public Common_Web
{
public:
  PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};