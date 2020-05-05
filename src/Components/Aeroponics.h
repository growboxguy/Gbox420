#pragma once

#include "420Common.h"
#include "Sound.h"
#include "PressureSensor.h"
#include "WaterPump.h"

///Aeroponics tote virtual parent class

class Aeroponics : virtual public Common
{
public:
  Aeroponics(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump);
  WaterPump *Pump;
  virtual void report();
  void startPump(bool UserRequest);
  void stopPump(bool UserRequest);

  void setPumpDisable();
  void setPumpTimeout(int TimeOut);
  void mixReservoir();

  void setSprayOnOff(bool State); 
  virtual void sprayOff(bool UserRequest) = 0;
  char *sprayStateToText();
  virtual void sprayNow(bool UserRequest = false) = 0;
  void setSprayInterval(int interval);
  void setSprayDuration(int duration);
  int getSprayInterval();
  int getSprayDuration();
  char *getSprayIntervalText();
  char *getSprayDurationText();
  bool getSprayEnabled();
  float getPressure(); 
  void setHighPressure(float HighPressure);
  
private:

protected:
  Module *Parent;
  PressureSensor *FeedbackPressureSensor; ///Pressure sensor object that will monitor the spray pressure
  float *HighPressure;          ///Aeroponics - Turn off pump above this pressure (bar)
  bool *SprayEnabled;             ///Enable/disable misting
  int *Interval;                  ///Aeroponics - Spray every 15 minutes
  int *Duration;                  ///Aeroponics - Spray time in seconds
  uint32_t SprayTimer = millis(); 

};
