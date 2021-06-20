#pragma once

///< This class represents a complete MainModule with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API
#include "RF24.h"

// forward declaration of classes
class Lights;
class Sound;
class Fan;
class Fan_PWM;
class AirPump;
//class PowerSensor;  ///< For PZEM004T V1.0 or PZEM004T V2.0
class PowerSensorV3; ///< Only for PZEM004T V3.0
class LightSensor;
class DHTSensor;
class HempyModule_Web;
class AeroModule_Web;
class ReservoirModule_Web;

extern ELClientRest PushingBoxRestAPI;

///< Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

class MainModule : virtual public Common_Web, public Module_Web
{
public:
  MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless); ///< constructor
  RF24 *Wireless = NULL;  //Pointer to NRF24L01 wireless transmitter
  Sound *Sound1 = NULL;                                                                                              ///< Pointer to a Piezo speaker - sound feedback
  Fan *IFan = NULL;                                                                                                  ///< Internal fan
  Fan *EFan = NULL;
  Fan_PWM *FanI = NULL;       ///< Internal fan
  Fan_PWM *FanE = NULL;       ///< Exhaust fan
  AirPump *APump1 = NULL;     ///< Air pump
  Lights *Lt1 = NULL;         ///< Pointer to a Light assembly No1
  Lights *Lt2 = NULL;         ///< Pointer to a Light assembly No2
  LightSensor *LtSen1 = NULL; ///< Pointer to a Light Sensor object measuring light intensity in the grow box
  //PowerSensor *Pow1 = NULL;   ///< For PZEM004T V1.0 or PZEM004T V2.0
  PowerSensorV3 *Pow1 = NULL;                   ///< Only for PZEM004T V3.0
  DHTSensor *DHT1 = NULL;                       ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  AeroModule_Web *AeroModule1 = NULL;           ///< <Represents the website controls and feedback for a AeroModule
  ReservoirModule_Web *ReservoirModule1 = NULL; ///< <Represents the website controls and feedback for a ReservoirModule
  HempyModule_Web *HempyModule1 = NULL;         ///< <Represents the website controls and feedback for a HempyModule
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh(char *url);
  void commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool FriendlyFormat = false);
  void refresh_FiveSec();
  void refresh_Minute();
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToMQTTTrigger(bool ForceRun = false);
  bool getDayMode(); ///< Returns true if the lights are on or daylight is detected
  char *getDayModeText(bool FriendlyFormat = false);

private:
  void setDebug(bool DebugEnabled);
  char *getDebugText(bool FriendlyFormat = false);
  void setMetric(bool MetricEnabled);
  char *getMetricText(bool FriendlyFormat = false);
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint16_t Frequency);
  void setPushingBoxLogRelayID(const char *ID);
  void setMQTTReportingOnOff(bool State);
  void setMQTTReportingFrequency(uint16_t Frequency);
  void setMqttPublishTopic(const char *ID);
  void setMqttSubscribeTopic(const char *ID);
  void setMQTTLWTTopic(const char *ID);
  void setMQTTLWTMessage(const char *ID);

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  bool MQTTReportRequested = false;
  bool *ReportToGoogleSheets;
  uint16_t *SheetsReportingFrequency;
  uint8_t SheetsTriggerCounter = 0;
  bool *ReportToMQTT;
  uint16_t *MQTTReportFrequency;
  uint16_t MQTTTriggerCounter = 0;
};