//This is a virtual class from which all other classes are derived (GrowBox, PH reader, Lights, DHTSensor..)
//Enforces a set of common functions all clases need to implement
#pragma once

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "420Settings.h"  //for storing/reading defaults
#include "420Helpers.h"  //global functions

extern Settings MySettings;
extern char Message[512];
extern char CurrentTime[20];

class Common{
  private:

  
  protected:
    Common(){
      //HealthStatus = new AlertHandler();
    }
  
  public:
    virtual void refresh() = 0;  //every class needs to have an implementation of this method. Called when component should refresh its state (for example Reading temperature and humidity using the DHTsensor class)
    virtual void report() = 0;  //Prints current state of the object to the Serial outputs
    //AlertHandler * HealthStatus; //Tracks the state of a component  
      
};
