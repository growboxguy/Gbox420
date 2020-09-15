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
  virtual void refresh_Sec() = 0;
  void setSprayOnOff(bool State);
  char *sprayStateToText();
  virtual void sprayNow(bool UserRequest = false) = 0;
  void setDayMode(bool State);
  void setDayInterval(int Interval);
  void setDayDuration(int Duration);
  void setNightInterval(int Interval);
  void setNightDuration(int Duration);
  int getDayInterval();
  int getDayDuration();
  int getNightInterval();
  int getNightDuration();
  char *getDayIntervalText();
  char *getNightIntervalText();
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
  bool *SprayEnabled;          ///Enable/disable misting
  int *DayInterval;            ///Spray every 15 minutes - With lights ON
  int *DayDuration;            ///Spray time in seconds - With lights ON
  int *NightInterval;          ///Spray every 15 minutes - With lights OFF
  int *NightDuration;          ///Spray time in seconds - With lights OFF
  float *MinPressure;          ///Minimum acceptable spray pressure
  float *MaxPressure;          ///Maximum allowed pressure
  float LastSprayPressure = 0; ///tracks the last pressure reading during a spray cycle
  bool DayMode = true;         ///Switch between Day and Night spray interval and duration.
};