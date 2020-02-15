#pragma once

//This class represents an autonomous module with an Arduino Nano, like the Aeroponics or Hempy bucket modules
//Responsible for setting the components attached to the module (Temp, weight, ph...sensors), 
//updating their statuses and reporting it to the Main module wirelessly
//Listens and reacts to wireless commands from the main module

#include "src/Components/420Module.h"

//forward declaration of classes
class DHTSensor;
class Sound;
class PHSensor;
class WaterTempSensor;
class WaterLevelSensor;
class WeightSensor;

class HempyModule : public Common, public Module
{
public:
  HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings); //constructor
  Sound *Sound1;             //Pointer to a Piezo speaker - sound feedback
  DHTSensor *IDHT;          //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  //DHTSensor *EDHT;          //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  //PHSensor *PHSensor1;
  //WaterTempSensor *WaterTemp1;
  //WaterLevelSensor *WaterLevel1;
  //WeightSensor *Weight1; 
  //WeightSensor *Weight2;  
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour(); 

private:
  void setDebugOnOff(bool State);
  void setMetric(bool MetricEnabled);

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false; 
};