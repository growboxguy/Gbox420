#pragma once

#include "420Common.h"
#include "Sound.h"
#include "PressureSensor.h"
#include "PressurePump.h"

///< Aeroponics tote without pressure tank

class Aeroponics_NoTank : public Common
{
public:
  Aeroponics_NoTank(const __FlashStringHelper *Name, 
                    Module *Parent, 
                    Settings::AeroponicsSettings *DefaultSettings, ///< Reference instead of pointer
                    PressureSensor *FeedbackPressureSensor, 
                    PressurePump *Pump); ///< constructor

  void refresh_Sec();
  void report(bool FriendlyFormat = false);
  void sprayNow(bool UserRequest = false);
  void sprayOff();
  void checkPump(bool OnlyTurnOff = false); ///< Turn the pump on or off based on the current state
  void processTimeCriticalStuff();          ///< Process things that cannot wait or need precise timing

  PressurePump *Pump;
  void updateState(AeroNoTankStates State);
  AeroNoTankStates getState();
  char *getStateText(bool FriendlyFormat = false);
  void setSprayOnOff(bool State);
  char *sprayStateToText();
  void setDayMode(bool State);
  char *getDayModeText(bool FriendlyFormat = false);
  void setDuration(float Duration);
  void setDayInterval(int Interval);
  float getDuration();
  char *getDurationText(bool FriendlyFormat = false);
  void setNightInterval(int Interval);
  int getDayInterval();
  int getNightInterval();
  char *getDayIntervalText(bool FriendlyFormat = false);
  char *getNightIntervalText(bool FriendlyFormat = false);
  char *getSprayDurationText(bool FriendlyFormat = false);
  bool getSprayEnabled();
  char *getSprayEnabledText(bool FriendlyFormat = false);
  float getPressure();
  float getLastSprayPressure();
  char *getLastSprayPressureText(bool FriendlyFormat = false);
  void setMinPressure(float Pressure);
  char *getMinPressureText(bool FriendlyFormat = false);
  void setMaxPressure(float Pressure);
  char *getMaxPressureText(bool FriendlyFormat = false);

private:
  bool RunTillTimeout; ///< While true only the Pump Timeout will stop the pump.
  Module *Parent;
  AeroNoTankStates State; ///< Stores the current state of the Aeroponics tote
  PressureSensor *FeedbackPressureSensor; ///< Pressure sensor object that will monitor the spray pressure
  uint32_t SprayTimer = millis();
  
  bool &SprayEnabled;   ///< Enable/disable misting
  float &Duration;      ///< Spray time in seconds
  int &DayInterval;     ///< Spray every X minutes - With lights ON
  int &NightInterval;   ///< Spray every X minutes - With lights OFF
  float &MinPressure;   ///< Minimum acceptable spray pressure
  float &MaxPressure;   ///< Maximum allowed pressure
  float LastSprayPressure = 0; ///< Tracks the last pressure reading during a spray cycle
  bool DayMode = true;  ///< Switch between Day and Night spray interval and duration.
};
