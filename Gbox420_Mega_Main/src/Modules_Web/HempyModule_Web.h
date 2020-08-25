#pragma once

///This class represents the website component of a HempyModule
///Allows controlling the HempyModule wirelessly and receiving a status report from it

#include "TimeLib.h"     ///keeping track of time
#include "../Components_Web/420Common_Web.h"
#include "../Components_Web/420Module_Web.h"
#include "../WirelessCommands_Hempy.h"

///forward declaration of classes


class HempyModule_Web : public Common_Web
{
public:
  HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::HempyModuleSettings *DefaultSettings); ///constructor
  void sendMessages();
  HempyMessages sendCommand(void* CommandToSend);
  void websiteEvent_Refresh(__attribute__((unused)) char *url); 
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Button(__attribute__((unused)) char *Button);
  void websiteEvent_Field(__attribute__((unused)) char *Field);
  void report();
  void reportToJSON();
  void refresh_Sec();
  void refresh_FiveSec();
  void refresh_Minute();
  void updateCommands();
  
private:  
  bool SyncRequested = true;    //Trigger a sync with the external Module within 1 second
  bool OnlineStatus = false;  /// Start in Offline state, a successful sync will set this to true
  void *ReceivedResponse = malloc(WirelessPayloadSize);                       ///< Pointer to the data sent back in the acknowledgement.
  struct HempyModuleCommand Module1CommandToSend = {HempyMessages::HempyModule1Command};  ///Command to send will be stored here
  struct HempyBucketCommand Bucket1CommandToSend = {HempyMessages::HempyBucket1Command}; ///Command to send will be stored here
  struct HempyBucketCommand Bucket2CommandToSend = {HempyMessages::HempyBucket2Command}; ///Command to send will be stored here
  struct HempyCommonTemplate GetNextResponse = {HempyMessages::HempyGetNext};            //< Special command to fetch the next Response from the Receiver
  struct HempyModuleResponse * Module1ReceivedResponse = malloc(sizeof(struct HempyModuleResponse));  /// Response will be stored here
  struct HempyBucketResponse * Bucket1ReceivedResponse = malloc(sizeof(struct HempyBucketResponse));  /// Response will be stored here
  struct HempyBucketResponse * Bucket2ReceivedResponse = malloc(sizeof(struct HempyBucketResponse));  /// Response will be stored here
  unsigned long LastResponseReceived = 0;   //Timestamp of the last response received

protected:
  Module_Web *Parent;
  const byte WirelessChannel[6];
  Settings::HempyModuleSettings *DefaultSettings;

};

