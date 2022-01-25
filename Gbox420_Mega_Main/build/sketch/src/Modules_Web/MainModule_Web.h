#pragma once

///< This class represents a complete MainModule with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API
#include "RF24.h"

// forward declaration of classes
class Sound_Web;
class Lights_Web;
class Fan_Web;
//class Fan_PWM_Web;
class AirPump_Web;
//class PowerSensor_Web;  ///< For PZEM004T V1.0 or PZEM004T V2.0
class PowerSensorV3_Web; ///< Only for PZEM004T V3.0
class LightSensor_Web;
class DHTSensor_Web;
class HempyModule_Web;
class AeroModule_Web;
class ACMotorModule_Web;
class ReservoirModule_Web;

extern ELClientRest PushingBoxRestAPI;

class MainModule : virtual public Common_Web, virtual public Module_Web
{
public:
  MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless); ///< constructor
  RF24 *Wireless = NULL;                                                                                      //Pointer to NRF24L01 wireless transmitter
  Sound_Web *Sound1 = NULL;                                                                                   ///< Pointer to a Piezo speaker - sound feedback
  Fan_Web *FanI = NULL;                                                                                       ///< Internal fan
  Fan_Web *FanE = NULL;
  //Fan_PWM_Web *FanI = NULL;       ///< Internal fan
  //Fan_PWM_Web *FanE = NULL;       ///< Exhaust fan
  AirPump_Web *Ap1 = NULL;     ///< Air pump
  Lights_Web *Lt1 = NULL;      ///< Pointer to a Light assembly No1
  Lights_Web *Lt2 = NULL;      ///< Pointer to a Light assembly No2
  LightSensor_Web *Ls1 = NULL; ///< Pointer to a Light Sensor object measuring light intensity in the grow box
  //PowerSensor_Web *Pow1 = NULL;   ///< For PZEM004T V1.0 or PZEM004T V2.0
  PowerSensorV3_Web *Pow1 = NULL;               ///< Only for PZEM004T V3.0
  DHTSensor_Web *DHT1 = NULL;                   ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  AeroModule_Web *AeroModule1 = NULL;           ///< <Represents the website controls and feedback for a AeroModule
  ReservoirModule_Web *ReservoirModule1 = NULL; ///< <Represents the website controls and feedback for a ReservoirModule
  HempyModule_Web *HempyModule1 = NULL;         ///< <Represents the website controls and feedback for a HempyModule
  //ACMotorModule_Web *ACMotorModule1 = NULL;         ///< <Represents the website controls and feedback for a AC Motor module
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();
  bool getDayMode(); ///< Returns true if the lights are on or daylight is detected
  char *getDayModeText(bool FriendlyFormat = false);

private:
protected:
};