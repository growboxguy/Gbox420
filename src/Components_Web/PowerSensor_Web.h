#pragma once

#include "PZEM004T.h" //3rd party module for the PZEM004T power sensor
#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/PowerSensor.h"

class PowerSensor_Web : public PowerSensor, public Common_Web
{
public:
  PowerSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, HardwareSerial *SerialPort); //constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(char *url){};  //Not used
  void websiteEvent_Button( char *Button){};  //Not used
  void websiteEvent_Field( char *Field){};  //Not used
  
private:
 
protected:
  Module_Web *Parent;
};