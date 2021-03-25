#pragma once

///< This class represents a complete MainModule with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

#include "420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API
#include "../WirelessCommands_Hempy.h"

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
  Sound *Sound1;                                                                                          ///< Pointer to a Piezo speaker - sound feedback
  Fan *IFan;                                                                                              ///< Internal fan
  Fan *EFan;  
  Fan_PWM *FanI;                                                                                              ///< Internal fan
  Fan_PWM *FanE;                                                                                               ///< Exhaust fan
  AirPump *APump1;                                                                                        ///< Air pump
  Lights *Lt1;                                                                                            ///< Pointer to a Light assembly No1
  Lights *Lt2;                                                                                            ///< Pointer to a Light assembly No2
  LightSensor *LtSen1;                                                                                    ///< Pointer to a Light Sensor object measuring light intensity in the grow box
  //PowerSensor *Pow1;   ///< For PZEM004T V1.0 or PZEM004T V2.0
  PowerSensorV3 *Pow1;               ///< Only for PZEM004T V3.0
  DHTSensor *DHT1;                   ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  AeroModule_Web *AeroModule1;           ///< <Represents the website controls and feedback for a AeroModule
  ReservoirModule_Web *ReservoirModule1; ///< <Represents the website controls and feedback for a ReservoirModule
  HempyModule_Web *HempyModule1;         ///< <Represents the website controls and feedback for a HempyModule
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh(char *url);
  void commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);
  void report(bool JSONReport = false);
  void refresh_FiveSec();
  void refresh_Minute();
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToMQTTTrigger(bool ForceRun = false);
  bool getDayMode(); ///< Returns true if the lights are on or daylight is detected
                     // void relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength]);

private:

  void setDebug(bool DebugEnabled);
  void setMetric(bool MetricEnabled);
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
  uint16_t *SheetsReportingFrequency;
  bool *ReportToGoogleSheets;
  uint8_t SheetsRefreshCounter = 0;
  uint16_t *MQTTReportingFrequency;
  bool *ReportToMQTT;
  uint8_t MQTTRefreshCounter = 0;
  bool *RealTimeMode;                 ///< Enable/disable sending a full report every 5 seconds via MQTT and Serial. Enables the JSONToSerialMode as well!  
};