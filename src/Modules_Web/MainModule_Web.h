#pragma once

///< This class represents a complete MainModule with all of its components
///< Responsible for setting up each module, updating their statuses and reporting it

#include "../Components_Web/420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API
#include "../WirelessCommands_Hempy.h"

// forward declaration of classes
class Lights_Web;
class Sound_Web;
class Fan_Web;
class Fan_PWM_Web;
class AirPump_Web;
//class PowerSensor_Web;  ///< For PZEM004T V1.0 or PZEM004T V2.0
class PowerSensorV3_Web; ///< Only for PZEM004T V3.0
class LightSensor_Web;
class HempyModule_Web;
class DHTSensor_Web;
class AeroModule_Web;
class ReservoirModule_Web;

extern ELClientRest PushingBoxRestAPI;

///< Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

class MainModule : public Common, public Module_Web
{
public:
  MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless); ///< constructor
  Sound_Web *Sound1;                                                                                          ///< Pointer to a Piezo speaker - sound feedback
  Fan_Web *IFan;                                                                                              ///< Internal fan
  Fan_Web *EFan;  
  Fan_PWM_Web *FanI;                                                                                              ///< Internal fan
  Fan_PWM_Web *FanE;                                                                                               ///< Exhaust fan
  AirPump_Web *APump1;                                                                                        ///< Air pump
  Lights_Web *Lt1;                                                                                            ///< Pointer to a Light assembly No1
  Lights_Web *Lt2;                                                                                            ///< Pointer to a Light assembly No2
  LightSensor_Web *LtSen1;                                                                                    ///< Pointer to a Light Sensor object measuring light intensity in the grow box
  //PowerSensor_Web *Pow1;   ///< For PZEM004T V1.0 or PZEM004T V2.0
  PowerSensorV3_Web *Pow1;               ///< Only for PZEM004T V3.0
  DHTSensor_Web *DHT1;                   ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  AeroModule_Web *AeroModule1;           ///< <Represents the website controls and feedback for a AeroModule
  ReservoirModule_Web *ReservoirModule1; ///< <Represents the website controls and feedback for a ReservoirModule
  HempyModule_Web *HempyModule1;         ///< <Represents the website controls and feedback for a HempyModule
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh(char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void report();
  void refresh_FiveSec();
  void refresh_Minute();
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToMQTTTrigger(bool ForceRun = false);
  void reportToJSON();
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
  void setMQTTTopic(const char *ID);
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
};