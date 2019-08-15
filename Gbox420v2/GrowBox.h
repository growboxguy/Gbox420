#ifndef GrowBox_H
#define GrowBox_H

//#include "Arduino.h"  //every inheriting class have Arduino commands available
//#include "420Settings.h"  //for storing/reading defaults
#include "Common.h"
#include "DHTSensor.h"
#include "LightSensor.h"
#include "Lights.h"
#include "Buzzer.h"

class GrowBox : public Common
{
  protected:
  Settings * BoxSettings; 
  
  public:
  GrowBox(Settings *BoxSettings);
  void refresh();
  char * reportToSerials();
  Buzzer * Buzzer1; //Pointer to a Piezo Buzzer - sound feedback
  DHTSensor * InternalDHTSensor;  //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  DHTSensor * ExternalDHTSensor; //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  LightSensor * LightSensor1; //Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights * Light1;  //Pointer to a Light assembly 
  void runSec(); //triger all threads at startup
  void runFiveSec(); //needs to run first to get sensor readings
  void runMinute();
  void runHalfHour();  
  
  private: 
  
};

#endif
