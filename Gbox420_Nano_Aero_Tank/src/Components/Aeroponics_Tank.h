#pragma once

#include "420Common.h"
#include "Switch.h"
#include "Sound.h"
#include "PressureSensor.h"
#include "PressurePump.h"

///< Aeroponics tote with pressure tank
class Aeroponics_Tank : public Common
{
public:
  // Constructor with SpraySolenoidClosingDelay as a reference
  Aeroponics_Tank(const __FlashStringHelper *Name,
                  Module *Parent,
                  Settings::AeroponicsSettings *DefaultSettings, ///< Pointer to settings
                  PressurePump *Pump,                            ///< Reference to PressurePump
                  PressureSensor *FeedbackPressureSensor        ///< Reference to PressureSensor
  );                                                             ///< Reference to SpraySolenoidClosingDelay
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing
  void refresh_Sec();
  void report(bool FriendlyFormat = false);
  void updateState(AeroTankStates NewState);
  void sprayNow(bool UserRequest = false); ///< Start spraying
  void sprayOff(bool UserRequest = false); ///< Stop spraying
  void refillTank();                       ///< Recharge the pressure tank
  void drainTank();                        ///< Release pressure from the tank
  void startMixing();                      ///< Start spraying
  void setSprayOnOff(bool State);
  char *sprayStateToText();
  void setDayMode(bool State);
  char *getDayModeText(bool FriendlyFormat = false);
  void setDuration(float Duration);
  float getDuration();
  char *getDurationText(bool FriendlyFormat = false);
  void setDayInterval(uint16_t Interval);
  void setNightInterval(uint16_t Interval);
  char *getDayIntervalText(bool FriendlyFormat = false);
  char *getNightIntervalText(bool FriendlyFormat = false);
  char *getSprayDurationText(bool FriendlyFormat = false);
  bool getSprayEnabled();
  char *getSprayEnabledText(bool FriendlyFormat = false);
  AeroTankStates getState();
  char *getStateText(bool FriendlyFormat = false);
  float getLastSprayPressure();
  char *getLastSprayPressureText(bool FriendlyFormat = false);
  void setMinPressure(float Pressure);
  char *getMinPressureText(bool FriendlyFormat = false);
  void setMaxPressure(float Pressure);
  char *getMaxPressureText(bool FriendlyFormat = false);
  PressurePump *Pump;
  PressureSensor *FeedbackPressureSensor; ///< Pressure sensor that will monitor the spray pressure

private:
  Module *Parent;
  Switch *SpraySwitch;                         // Relay or MOSFET controlling the spray solenoid
  AeroTankStates State = AeroTankStates::IDLE; //< Stores the current state of the Aeroponics tote
  uint16_t &SpraySolenoidClosingDelay;         //< (ms) Time required for the solenoid to close
  uint32_t SprayTimer = millis();              ///< Times functions regarding to spraying
  bool SprayEnabled;                           ///< Enable/disable misting
  float &Duration;                             ///< Spray time in seconds
  uint16_t &DayInterval;                       ///< Spray every X minutes - With lights ON
  uint16_t &NightInterval;                     ///< Spray every X minutes - With lights OFF
  float &MinPressure;                          ///< Minimum acceptable spray pressure
  float &MaxPressure;                          ///< Maximum allowed pressure
  float LastSprayPressure = 0;                 ///< tracks the last pressure reading during a spray cycle
  bool DayMode = true;                         ///< Switch between Day and Night spray interval and duration.
};