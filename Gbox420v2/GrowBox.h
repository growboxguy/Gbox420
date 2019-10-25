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
class Aeroponics_NoTank;
class Aeroponics_Tank;

class GrowBox : public Common
{ 
  public:
    GrowBox(const __FlashStringHelper * Name, Settings *BoxSettings); //constructor
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
    Aeroponics_NoTank * Aeroponics_NoTank1;
    void websiteLoadEvent(char * url);
    void websiteRefreshEvent(char * url);
    void websiteButtonPressEvent(char * Button){};
    void websiteFieldSubmitEvent(char * Field);
    void refresh();
    void report(){};
    void refreshAll();
 
    void runSec(); //trigger all threads at startup
    void runFiveSec(); //needs to run first to get sensor readings
    void runMinute();
    void runHalfHour(); 

    char* eventLogToJSON(bool Append); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]  
    void addToLog(const __FlashStringHelper* Text,byte indent=3);
    void addToLog(const char* Text,byte indent=3);
    void setDebugOnOff(bool State);
    void setMetricSystemEnabled(bool MetricEnabled); 

    Common* RefreshQueue_Sec[QueueDepth];
    Common* RefreshQueue_FiveSec[QueueDepth]; 
    Common* RefreshQueue_Minute[QueueDepth];
    Common* RefreshQueue_HalfHour[QueueDepth];
    Common* WebsiteQueue_Load[QueueDepth];
    Common* WebsiteQueue_Refresh[QueueDepth]; 
    Common* WebsiteQueue_Button[QueueDepth];
    Common* WebsiteQueue_Field[QueueDepth];
    byte refreshQueueLength_Sec = 0;
    byte refreshQueueLength_FiveSec = 0;
    byte refreshQueueLength_Minute = 0;
    byte refreshQueueLength_HalfHour = 0;
    byte WebsiteQueueLength_Load = 0;
    byte WebsiteQueueLength_Refresh = 0;
    byte WebsiteQueueLength_Button = 0;
    byte WebsiteQueueLength_Field = 0;
    void AddToRefreshQueue_Sec(Common* Component);
    void AddToRefreshQueue_FiveSec(Common* Component);
    void AddToRefreshQueue_Minute(Common* Component);
    void AddToRefreshQueue_HalfHour(Common* Component);
    void AddToWebsiteQueue_Load(Common* Component);
    void AddToWebsiteQueue_Refresh(Common* Component);
    void AddToWebsiteQueue_Button(Common* Component);
    void AddToWebsiteQueue_Field(Common* Component);
  
  private: 

  protected:
};
