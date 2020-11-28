#pragma once

#define PZEM004_NO_SWSERIAL ///< Singals to the PZEM004Tv30 module to use HardwareSerial instead of SoftwareSerial
#include "PZEM004Tv30.h"    ///< 3rd party module for the PZEM004T V3.0 power sensor
#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Components/PowerSensorV3.h"

///< PZEM004T v3.0 power sensor - website component

class PowerSensorV3_Web : public PowerSensorV3, public Common_Web
{
public:
  PowerSensorV3_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){};      ///< Not used
  void websiteEvent_Button(__attribute__((unused)) char *Button){}; ///< Not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){};   ///< Not used
  void reportToJSON();

private:
protected:
  Module_Web *Parent;
};