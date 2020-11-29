#pragma once

/**
 * @file DEVModule_Web.h
 * @author GrowBoxGuy 
 * @brief This module is for development purposes only
 * @version 4.20
 *
 */

#include "../Components_Web/420Module_Web.h"
#include "ELClientRest.h" ///< ESP-link - REST API
#include "../WirelessCommands_Hempy.h"

// forward declaration of classes
class WaterPump;
class DHTSensor_Web;
class DistanceSensor_Web;
class LightSensor_Web;
class Lights_Web;
class Sound_Web;
class Fan_Web;
class PowerSensor_Web;
class PowerSensorV3_Web; ///< Only for PZEM004T V3.0
class PHSensor_Web;
class PressureSensor_Web;
class Aeroponics_NoTank_Web;
class Aeroponics_Tank_Web;
class WaterTempSensor_Web;
class WaterLevelSensor_Web;
class WeightSensor_Web;
class HempyModule_Web;
class AeroModule_Web;
class ReservoirModule_Web;
class ModuleSkeleton_Web;

extern ELClientRest PushingBoxRestAPI;

///< Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

class DEVModule_Web : public Common_Web, public Module_Web
{
public:
  DEVModule_Web(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless); ///< constructor
  Sound_Web *Sound1;                                                                                             ///< Pointer to a Piezo speaker - sound feedback
  Fan_Web *IFan;                                                                                                 ///< Internal fan
  Fan_Web *EFan;                                                                                                 ///< Exhaust fan
  DHTSensor_Web *DHT1;                                                                                           ///< Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  LightSensor_Web *LtSen1;                                                                                       ///< Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights_Web *Lt1;                                                                                               ///< Pointer to a Light assembly
  PowerSensor_Web *Pow1;
  //PowerSensorV3_Web * Pow1;  ///< Only for PZEM004T V3.0
  PressureSensor_Web *Pres1;
  PHSensor_Web *PHSen1;
  WaterPump *AeroPump1;
  Aeroponics_Tank_Web *AeroT1;
  Aeroponics_NoTank_Web *AeroNT1;
  WaterTempSensor_Web *WTemp1;
  WaterLevelSensor_Web *WLev1;
  DistanceSensor_Web *Dist1;
  HempyModule_Web *HempyModule1; ///< <Represents the website controls and feedback for a HempyModule
  AeroModule_Web *AeroModule1;   ///< <Represents the website controls and feedback for a AeroModule
  ReservoirModule_Web *ReservoirModule1;
  //WeightSensor_Web *Weight1;
  //WeightSensor_Web *Weight2;
  //ModuleSkeleton_Web *ModuleSkeleton1; ///< Only for demonstration purposes
  //ModuleSkeleton_Web *ModuleSkeleton2; ///< Only for demonstration purposes
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh(char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour();
  void reportToGoogleSheetsTrigger(bool ForceRun = false);
  void reportToJSON();
  // void relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength]);

private:
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(uint16_t Frequency);
  void setDebug(bool DebudEnabled);
  void setMetric(bool MetricEnabled);
  void setPushingBoxLogRelayID(const char *ID);

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  uint16_t *SheetsReportingFrequency;
  bool *ReportToGoogleSheets;
  uint8_t SheetsRefreshCounter = 0;
};