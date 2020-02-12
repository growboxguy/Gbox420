#pragma once
#include "420Module_Web.h"
#include "ELClientRest.h" //ESP-link - REST API

//This class represents a complete growbox with all of its components
//Responsible for setting up each module, updating their statuses and reporting it

//forward declaration of classes
class DHTSensor_Web;
class LightSensor_Web;
class Lights_Web;
class Sound_Web;
class Fan_Web;
class PowerSensor_Web;
class PowerSensorV3_Web;  //Only for PZEM004T V3.0
class PHSensor_Web;
class LightSensor_Web;
class PressureSensor_Web;
class Aeroponics_NoTank_Web;
class Aeroponics_Tank_Web;
class WaterTempSensor_Web;
class WaterLevelSensor_Web;
class WeightSensor_Web;
class ModuleSkeleton_Web; //Only for demonstration purposes

extern ELClientRest PushingBoxRestAPI;

class GrowBox : public Module_Web, public Common_Web
{
public:
  GrowBox(const __FlashStringHelper *Name, Settings::GrowBoxSettings *DefaultSettings); //constructor
  Sound_Web *Sound1;             //Pointer to a Piezo speaker - sound feedback
  Fan_Web *IFan;                //Internal fan
  Fan_Web *EFan;                //Exhaust fan
  DHTSensor_Web *IDHT;          //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  DHTSensor_Web *EDHT;          //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  LightSensor_Web *LtSen1; //Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights_Web *Lt1;            //Pointer to a Light assembly
  PowerSensor_Web *Pow1;
  //PowerSensorV3_Web * Pow1;  //Only for PZEM004T V3.0
  PressureSensor_Web *Pressure1;
  PHSensor_Web *PHSensor1;
  Aeroponics_Tank_Web *AeroT1;
  Aeroponics_NoTank_Web *Aero1;   
  WaterTempSensor_Web *WaterTemp1;
  WaterLevelSensor_Web *WaterLevel1;
  WeightSensor_Web *Weight1; 
  WeightSensor_Web *Weight2;  
  ModuleSkeleton_Web *ModuleSkeleton1; //Only for demonstration purposes
  ModuleSkeleton_Web *ModuleSkeleton2; //Only for demonstration purposes
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh( char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour();  
  void reportToGoogleSheetsTrigger();
  void reportToGoogleSheets(bool CalledFromWebsite);
  void relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength]);

private:
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(int Frequency);
  void setDebugOnOff(bool State);
  void setMetricSystemEnabled(bool MetricEnabled);
  void setPushingBoxLogRelayID(const char *ID);

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  int *SheetsReportingFrequency;
  bool *ReportToGoogleSheets;
  int SheetsRefreshCounter = 0;
};