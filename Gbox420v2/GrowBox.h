#pragma once
#include "420Common.h"

class DHTSensor;  //forward declaration of classes
class LightSensor;
class Lights;
class Sound;
class PowerSensor;
class PHSensor;
class LightSensor;
class PHSensor;
class Aeroponics_Pump;
class Aeroponics_Tank;

class GrowBox : public Common
{
  //friend class RollingAverage;
  protected:

  
  public:
  GrowBox(Settings *BoxSettings); //constructor
  Settings * BoxSettings;
  Sound * Sound1; //Pointer to a Piezo Sound - sound feedback
  DHTSensor * InternalDHTSensor;  //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  DHTSensor * ExternalDHTSensor; //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  LightSensor * LightSensor1; //Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights * Light1;  //Pointer to a Light assembly 
  Lights * Light2;  //Pointer to a Light assembly 
  PowerSensor * PowerSensor1;
  PHSensor * PHSensor1;
  Aeroponics_Tank * Aeroponics_Tank1;
  Aeroponics_Pump * Aeroponics_Pump1;
  void refresh();
  void report();
 
  void runSec(); //trigger all threads at startup
  void runFiveSec(); //needs to run first to get sensor readings
  void runMinute();
  void runHalfHour(); 

  char* eventLogToJSON(bool Append); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]  
  void addToLog(const __FlashStringHelper* Text,byte indent=3);
  void addToLog(const char* Text,byte indent=3);
  void setDebugOnOff(bool State);
  void setMetricSystemEnabled(bool MetricEnabled); 
  
  private: 
  
};
