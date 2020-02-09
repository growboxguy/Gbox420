#pragma once
#include "../Components/420Module.h"
#include "ELClientRest.h" //ESP-link - REST API

//This class represents a complete growbox with all of its components
//Responsible for setting up each module, updating their statuses and reporting it

//forward declaration of classes
class DHTSensor;
class LightSensor;
class Lights;
class Sound;
class Fan;
class PowerSensor;
//class PowerSensorV3;  //Only for PZEM004T V3.0
class PHSensor;
class LightSensor;
class PressureSensor;
class Aeroponics_NoTank;
class Aeroponics_Tank;
class WaterTempSensor;
class WaterLevelSensor;
class WeightSensor;
class ModuleSkeleton; //Only for demonstration purposes

extern ELClientRest PushingBoxRestAPI;

class GrowBox : public Module
{
public:
  GrowBox(const __FlashStringHelper *Name, Settings::GrowBoxSettings *DefaultSettings); //constructor
  Sound *Sound1;             //Pointer to a Piezo speaker - sound feedback
  Fan *InFan;                //Internal fan
  Fan *ExFan;                //Exhaust fan
  DHTSensor *InDHT;          //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  DHTSensor *ExDHT;          //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  LightSensor *LightSensor1; //Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights *Light1;            //Pointer to a Light assembly
  PowerSensor *Power1;
  //PowerSensorV3 * Power1;  //Only for PZEM004T V3.0
  PressureSensor *Pressure1;
  PHSensor *PHSensor1;
  Aeroponics_Tank *Aero_T1;
  Aeroponics_NoTank *Aero_NT1;
  WaterTempSensor *WaterTemp1;
  WaterLevelSensor *WaterLevel1;
  WeightSensor *Weight1; 
  WeightSensor *Weight2;  
  ModuleSkeleton *ModuleSkeleton1; //Only for demonstration purposes
  ModuleSkeleton *ModuleSkeleton2; //Only for demonstration purposes
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour();  
  void addToLog(const __FlashStringHelper *Text, byte indent = 3);
  void addToLog(const char *Text, byte indent = 3);
  void reportToGoogleSheetsTrigger();
  void reportToGoogleSheets(bool CalledFromWebsite);
  void relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength]);
  void AddToWebsiteQueue_Load(Common *Component);
  void AddToWebsiteQueue_Refresh(Common *Component);
  void AddToWebsiteQueue_Button(Common *Component);
  void AddToWebsiteQueue_Field(Common *Component);
  void loadEvent(char *Url);
  void refreshEvent(char *Url);
  void buttonEvent(char *Button);
  void setFieldEvent(char *Field);

private:
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(int Frequency);
  void setDebugOnOff(bool State);
  void setMetricSystemEnabled(bool MetricEnabled);
  void setPushingBoxLogRelayID(const char *ID);
  char *eventLogToJSON(bool Append = false); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]
  Common *WebsiteQueue_Load[QueueDepth];
  Common *WebsiteQueue_Refresh[QueueDepth];
  Common *WebsiteQueue_Button[QueueDepth];
  Common *WebsiteQueue_Field[QueueDepth]; 
  byte WebsiteQueueItemCount_Load = 0;
  byte WebsiteQueueItemCount_Refresh = 0;
  byte WebsiteQueueItemCount_Button = 0;
  byte WebsiteQueueItemCount_Field = 0;

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  int *SheetsReportingFrequency;
  bool *ReportToGoogleSheets;
  int SheetsRefreshCounter = 0;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>Settings</h1>
  <button id="GBox1_RestoreDefaults" type="button" style="margin-top:0px" title="Restore settings to sketch defaults">Restore defaults</button>
  <form><label><input type="checkbox" name="GBox1_DebugEnabled" title="Debug entres on Serial log">Debug enabled </label><input type="submit" value="Set"></form>
  <form><label><input type="checkbox" name="GBox1_MetricSystemEnabled" title="Use imperial units(F,PSI) or metric (C,bar)">Metric units </label><input type="submit" value="Set"></form>				
</div>

<div class="card" style="width:90%">
  <h1>Reporting</h1>					
  <form><label><input type="checkbox" name="GBox1_SheetsEnabled">Google Sheets, every <input style="width: 55px;" min=15 max=1440 step=15 name="GBox1_SheetsFrequency" type="number"/> min </label><input type="submit" value="Set"></form>
  <form>RelayID:<input type="text" name="GBox1_PushingBoxLogRelayID"  style="width: 150px;" title="DeviceID of the PushingBox logging scenario"/><input type="submit" value="Set"></form>
</div>

<div class="pure-u-1 pure-u-md-1-2" style="max-width:370px">
  <div class="card" style="width:90%">	
    <h1><span id="Time"></span></h1>				
    <ul id="list_SerialLog"></ul>
    <button id="GBox1_Refresh" type="button" style="margin-top:0px" title="Updates the status of every module">Refresh</button>	
  </div>					
</div>
*/