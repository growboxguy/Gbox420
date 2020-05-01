#pragma once

///This class represents the website component of a HempyModule
///Allows controlling the HempyModule wirelessly and receiving a status report from it

#include "TimeLib.h"     ///keeping track of time
#include "420Common_Web.h"
#include "420Module_Web.h"
#include "../Wireless_HempyModule.h"

///forward declaration of classes


class HempyModule_Web : public Common_Web
{
public:
  HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::HempyModuleSettings *DefaultSettings); ///constructor
  void syncModule(const byte WirelessChannel[], hempyCommand *Command, hempyResponse *Response);
  void websiteEvent_Refresh(__attribute__((unused)) char *url); 
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  void websiteEvent_Field(__attribute__((unused)) char *Field);
  void report();
  void refresh_FiveSec();
  void refresh_Minute();
  void updateCommand();

  void TriggerWateringBucket1();
  void TriggerWateringBucket2();
     
private:  
  struct hempyCommand Command;  //Commands sent to the external Module
  struct hempyResponse Response; //The response from the external Module will be stored here, represents the current status of the external Module

protected:
  Module_Web *Parent;
  const byte WirelessChannel[6];

};

