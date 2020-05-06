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
  float getLastSprayPressure();
  char *getLastSprayPressureText(bool IncludeCurrentPressure);
  void setMinPressure(float Pressure);
  void setMaxPressure(float Pressure);
  
private:

protected:
  Module *Parent;
  PressureSensor *FeedbackPressureSensor; ///Pressure sensor object that will monitor the spray pressure
  uint32_t SprayTimer = millis(); 
  bool *SprayEnabled;             ///Enable/disable misting
  int *Interval;                  ///Aeroponics - Spray every 15 minutes
  int *Duration;                  ///Aeroponics - Spray time in seconds
  float *MinPressure;           ///Aeroponics - Minimum acceptable spray pressure
  float *MaxPressure;          ///Aeroponics - Maximum allowed pressure 
  float LastSprayPressure = 0; ///tracks the last pressure reading during a spray cycle
};
