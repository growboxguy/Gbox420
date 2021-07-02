#pragma once

///< This class represents a complete DevModule_Web with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API
#include "RF24.h"
#include "../WirelessCommands_Dev.h"

// forward declaration of classes
class Sound_Web;
class DHTSensor_Web;

extern ELClientRest PushingBoxRestAPI;

///< Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

class DevModule_Web : public Module_Web
{
public:
  DevModule_Web(const __FlashStringHelper *Name, Settings::DevModule_WebSettings *DefaultSettings, RF24 *Wireless); ///< constructor
  Sound_Web *Sound1;                                                                                                ///< Pointer to a Piezo speaker - sound feedback
  DHTSensor_Web *DHT1;                                                                                              ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  void websiteEvent_Load(char *Url);
  void websiteEvent_Refresh(char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();

private:
protected:
};