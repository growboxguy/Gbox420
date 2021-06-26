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

#include "TimeLib.h" // Keeping track of time
#include "../Components_Web/420Common_Web.h"
#include "420Module_Web.h"
#include "../Modules_Web/MainModule_Web.h"
#include "../WirelessCommands_Reservoir.h"

class ReservoirModule_Web : public Common_Web
{
public:
  ReservoirModule_Web(const __FlashStringHelper *Name, MainModule *Parent, Settings::ReservoirModuleSettings *DefaultSettings); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Load(__attribute__((unused)) char *url){}; //Not used
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_Sec();
  void refresh_FiveSec();
  void updateCommands();
  void sendMessages();
  ReservoirMessages sendCommand(void *CommandToSend);

private:
  bool SyncRequested = true;                            //Trigger a sync with the external Module within 1 second
  bool OnlineStatus = false;                            ///< Start in Offline state, a successful sync will set this to true
  void *ReceivedResponse = malloc(WirelessPayloadSize); ///< Pointer to the data sent back in the acknowledgement.
  unsigned long LastResponseReceived = 0;               //Timestamp of the last response received

protected:
  MainModule *Parent;
  const byte WirelessChannel[6];
  Settings::ReservoirModuleSettings *DefaultSettings;
};
