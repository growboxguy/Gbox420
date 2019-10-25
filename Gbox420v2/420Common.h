#pragma once   //header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

//This is a virtual class from which all other classes inherit from (GrowBox, PH reader, Lights, DHTSensor..)
//Enforces a set of common functions all classes shared by the components

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "420Settings.h"  //for loading defaults and storing/reading user settings 
#include "420Helpers.h"  //global functions

//class ELClientWebServer; //forward declaration

extern Settings MySettings;
extern char ShortMessage[MaxTextLength];
extern char LongMessage[512];
extern char CurrentTime[20];
extern ELClientWebServer WebServer;

class Common{
  public:
    bool DebugEnabled;
    byte RefreshCounter = 0; //counts how many times the refresh() function was called. Used for timing
    const __FlashStringHelper * Name; 
    virtual void refresh();  //every class needs to have an implementation of this method. Called when component should refresh its state (for example Reading temperature and humidity using the DHTsensor class)
    virtual void report() = 0;  //Prints current state of the object to the Serial outputs
    virtual void websiteLoadEvent(char * url){};
    virtual void websiteRefreshEvent(char * url){};
    virtual void websiteButtonPressEvent(char * Button){};
    virtual void websiteFieldSubmitEvent(char * Button){};
    bool isThisMyComponent(char const * lookupName); //When it return true the component belongs to this object  
    //AlertHandler * HealthStatus; //Tracks the state of a component  
    char * getWebsiteComponentName(const __FlashStringHelper * Name);
  
  private:
  
  protected:
    Common(const __FlashStringHelper * Name); //Constructor
    Common(){}  //Constructor
};
