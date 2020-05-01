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
  HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyModuleSettings *DefaultSettings); ///constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url); 
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  void websiteEvent_Field(__attribute__((unused)) char *Field);
  void report();
  void refresh_FiveSec();
  void refresh_Minute();
  void updateMessage();
     
private:  
  struct commandTemplate FakeCommand = {1587936134,0,0,0,120,3.8,4.8,0,0,0,120,3.9,4.9};  //Fake commands sent to the Receiver
  struct responseTemplate FakeResponse; //The response from the Receiver will be stored here, represents the current status of the Receiver

protected:
  Module_Web *Parent;
  const uint8_t *WirelessChannel;
};

