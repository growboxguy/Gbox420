#pragma once

///< This class represents the website component of a HempyModule
///< Allows controlling the HempyModule wirelessly and receiving a status report from it

#include "TimeLib.h" // Keeping track of time
#include "../Components_Web/420Common_Web.h"
#include "420Module_Web.h"
#include "../Modules_Web/MainModule_Web.h"
#include "../WirelessCommands_Hempy.h"

class HempyModule_Web : public Common_Web
{
public:
  HempyModule_Web(const __FlashStringHelper *Name, MainModule *Parent, Settings::HempyModuleSettings *DefaultSettings); ///< constructor
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_Sec();
  void refresh_FiveSec();
  void refresh_Minute();
  void updateCommands();
  void sendMessages();
  HempyMessages sendCommand(void *CommandToSend);

private:
  bool SyncRequested = true;                            //Trigger a sync with the external Module within 1 second
  bool OnlineStatus = false;                            ///< Start in Offline state, a successful sync will set this to true
  void *ReceivedResponse = malloc(WirelessPayloadSize); ///< Pointer to the data sent back in the acknowledgement.
  unsigned long LastResponseReceived = 0;               //Timestamp of the last response received

protected:
  MainModule *Parent;
  byte WirelessChannel[6];
  Settings::HempyModuleSettings *DefaultSettings;
};