#pragma once

///This class represents the website component of a ReservoirModule
///Allows controlling the ReservoirModule wirelessly and receiving a status report from it

#include "TimeLib.h"     ///keeping track of time
#include "../Components_Web/420Common_Web.h"
#include "../Components_Web/420Module_Web.h"
#include "../WirelessCommands_Reservoir.h"

///forward declaration of classes


class ReservoirModule_Web : public Common_Web
{
public:
  ReservoirModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::ReservoirModuleSettings *DefaultSettings); ///constructor
  void syncModule(const byte WirelessChannel[], reservoirCommand *Command, reservoirResponse *Response);
  void websiteEvent_Refresh(__attribute__((unused)) char *url); 
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  void websiteEvent_Field(__attribute__((unused)) char *Field);
  void report();
  void refresh_Sec();
  void refresh_FiveSec();
  void refresh_Minute();
  void updateCommand();
     
private:  
  bool SyncRequested = true;    //Trigger a sync with the external Module within 1 second
  struct reservoirCommand Command;  //Commands sent to the external Module
  struct reservoirResponse Response; //The response from the external Module will be stored here, represents the current status of the external Module

protected:
  Module_Web *Parent;
  const byte WirelessChannel[6];
  Settings::ReservoirModuleSettings *DefaultSettings;

};

