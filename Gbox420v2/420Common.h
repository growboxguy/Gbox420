#pragma once   //header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

//This is a virtual class from which all other classes are derived (GrowBox, PH reader, Lights, DHTSensor..)
//Enforces a set of common functions all classes need to implement

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "420Settings.h"  //for loading defaults and storing/reading user settings 
#include "420Helpers.h"  //global functions

//class ELClientWebServer; //forward declaration

extern Settings MySettings;
extern char Message[512];
extern char CurrentTime[20];
extern ELClientWebServer WebServer;

class Common{
  private:

  
  protected:
    Common(const __FlashStringHelper * Name);

    Common(){}
  
  public:
    bool DebugEnabled;
    const __FlashStringHelper * Name; 
    virtual void refresh();  //every class needs to have an implementation of this method. Called when component should refresh its state (for example Reading temperature and humidity using the DHTsensor class)
    virtual void report() = 0;  //Prints current state of the object to the Serial outputs
    //AlertHandler * HealthStatus; //Tracks the state of a component  
    char * getWebsiteComponentName(const __FlashStringHelper * ComponentName);
    void websiteLoadEvent();
    void websiteRefreshEvent();
};
