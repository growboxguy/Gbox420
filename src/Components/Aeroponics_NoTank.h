#pragma once

#include "420Common.h"
#include "Sound.h"
#include "PressureSensor.h"
#include "PressurePump.h"

///< Aeroponics tote without pressure tank

class Aeroponics_NoTank : public Common
{
public:
  Aeroponics_NoTank(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, PressurePump *Pump); ///< constructor
  void refresh_Sec();
  void report(bool JSONReport = false);
  void sprayNow(bool UserRequest = false);
  void sprayOff();
  void checkPump(bool OnlyTurnOff = false); ///< Turn the pump on or off based on the current state
  void processTimeCriticalStuff();          ///< Process things that cannot wait or need precise timing

  PressurePump *Pump;
  void updateState(AeroNoTankStates State);

  void setSprayOnOff(bool State);
  char *sprayStateToText();
  void setDayMode(bool State);
  void setDuration(float Duration);
  void setDayInterval(int Interval);
  void setNightInterval(int Interval);
  float getDuration();
  int getDayInterval();
  int getNightInterval();
  char *getDayIntervalText();
  char *getNightIntervalText();
  char *getSprayDurationText();
  bool getSprayEnabled();
  float getPressure();
  float getLastSprayPressure();
  void setMinPressure(float Pressure);
  void setMaxPressure(float Pressure);

private:
protected:
  bool RunTillTimeout = false; ///< While true only the Pump Timeout will stop the pump.
  Module *Parent;
  AeroNoTankStates State = AeroNoTankStates::IDLE; //< Stores the current state of the Aeroponics tote
  PressureSensor *FeedbackPressureSensor;          ///< Pressure sensor object that will monitor the spray pressure
  uint32_t SprayTimer = millis();
  bool *SprayEnabled;          ///< Enable/disable misting
  float *Duration;             ///< Spray time in seconds
  int *DayInterval;            ///< Spray every X minutes - With lights ON
  int *NightInterval;          ///< Spray every X minutes - With lights OFF
  float *MinPressure;          ///< Minimum acceptable spray pressure
  float *MaxPressure;          ///< Maximum allowed pressure
  float LastSprayPressure = 0; ///< tracks the last pressure reading during a spray cycle
  bool DayMode = true;         ///< Switch between Day and Night spray interval and duration.
};