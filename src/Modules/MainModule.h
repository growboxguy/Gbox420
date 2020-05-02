#pragma once

///This class represents a complete MainModule with all of its components
///Responsible for setting up each module, updating their statuses and reporting it

#include "../Components_Web/420Module_Web.h"
#include "ELClientRest.h" ///ESP-link - REST API
#include "../WirelessCommands_Hempy.h"

///forward declaration of classes
class Lights_Web;
class Sound_Web;
class Fan_Web;
class PowerSensor_Web;
class PowerSensorV3_Web;  ///Only for PZEM004T V3.0
class LightSensor_Web;
class HempyModule_Web;
class ModuleSkeleton_Web;

extern ELClientRest PushingBoxRestAPI;

/// Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

class MainModule : public Common_Web, public Module_Web
{
public:
  MainModule(const __FlashStringHelper *Name, Settings::GrowModuleSettings *DefaultSettings,RF24 *Wireless); ///constructor
  Sound_Web *Sound1;             ///Pointer to a Piezo speaker - sound feedback
  Fan_Web *IFan;                ///Internal fan
  Fan_Web *EFan;                ///Exhaust fan  
  Lights_Web *Lt1;            ///Pointer to a Light assembly
  PowerSensor_Web *Pow1;
  //PowerSensorV3_Web * Pow1;  ///Only for PZEM004T V3.0
  HempyModule_Web *HempyModule1;    /// <Represents the website controls and feedback for a HempyModule  
  //ModuleSkeleton_Web *ModuleSkeleton1; ///Only for demonstration purposes
  //ModuleSkeleton_Web *ModuleSkeleton2; ///Only for demonstration purposes
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh( char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour();  
  void reportToGoogleSheetsTrigger();
  void reportToGoogleSheets(bool CalledFromWebsite);
 // void relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength]);
 
private:
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint8_t Frequency);
  void setDebug(bool DebudEnabled);
  void setMetric(bool MetricEnabled);
  void setPushingBoxLogRelayID(const char *ID);

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  uint8_t *SheetsReportingFrequency;
  bool *ReportToGoogleSheets;
  uint8_t SheetsRefreshCounter = 0;
};