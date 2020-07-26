#pragma once

/**
 * @file ReservoirModule.h
 * @author GrowBoxGuy (growboxguy@gmail.com)
 * @brief Website component for the Reservoir Module
 * @version 4.20

 * 
 * @copyright https://sites.google.com/site/growboxguy
 * 
 */

#include "TimeLib.h"     ///keeping track of time
#include "../Components_Web/420Common_Web.h"
#include "../Components_Web/420Module_Web.h"
#include "../WirelessCommands_Reservoir.h"

class ReservoirModule_Web : public Common_Web
{
public:
  ReservoirModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::ReservoirModuleSettings *DefaultSettings); ///constructor
  void syncModule(const byte WirelessChannel[], reservoirCommand *Command, reservoirResponse *Response);
  void websiteEvent_Refresh(__attribute__((unused)) char *url); 
  void websiteEvent_Load(__attribute__((unused)) char *url){};  ///not used \todo Remove the need to declare these when not needed (Should not be virtual in Common_Web)
  void websiteEvent_Button(__attribute__((unused)) char *Button){}; ///not used
  void websiteEvent_Field(__attribute__((unused)) char *Field){} ///not used
  void report();
  void reportToJSON();
  void refresh_FiveSec();
  void updateCommand();
  
private:  
  bool SyncRequested = true;    //Trigger a sync with the external Module within 1 second
  bool OnlineStatus = false;  /// Start in Offline state, a successful sync will set this to true
  struct reservoirCommand Command;  //Commands sent to the external Module
  struct reservoirResponse Response; //The response from the external Module will be stored here, represents the current status of the external Module

protected:
  Module_Web *Parent;
  const byte WirelessChannel[6];
  Settings::ReservoirModuleSettings *DefaultSettings;

};

