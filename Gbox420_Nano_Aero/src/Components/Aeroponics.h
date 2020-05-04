#pragma once

#include "420Common.h"
#include "Sound.h"
#include "PressureSensor.h"

///Aeroponics tote virtual parent class

class Aeroponics : virtual public Common
{
public:
  Aeroponics(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor);
  virtual void report();
  
  void setPumpOn(bool UserRequest);
  void setPumpOff(bool UserRequest);
  void setPumpDisable();
  void setPumpTimeout(int Timeout);
  void setPrimingTime(int Timing); 
  void mixReservoir();
  void setSprayOnOff(bool State); 
  virtual void sprayOff() = 0;
  virtual char *sprayStateToText() = 0; ///pure virtual function
  virtual void sprayNow(bool UserRequest = false) = 0;
  void setSprayInterval(int interval);
  void setSprayDuration(int duration);
  int getSprayInterval();
  int getSprayDuration();
  char *getSprayIntervalText();
  char *getSprayDurationText();    

private:

protected:
  Module *Parent;
  PressureSensor *FeedbackPressureSensor; ///Pressure sensor object that will monitor the spray pressure
  uint8_t *PumpPin;
  uint8_t *BypassSolenoidPin;
  bool *SprayEnabled;             ///Enable/disable misting
  int *Interval;                  ///Aeroponics - Spray every 15 minutes
  int *Duration;                  ///Aeroponics - Spray time in seconds
  int *PumpTimeout;               /// Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
  int *PrimingTime;               /// Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
  uint32_t PumpTimer = millis();  ///Aeroponics - Pump cycle timer
  uint32_t SprayTimer = millis(); ///Aeroponics - Spray cycle timer - https:///www.arduino.cc/reference/en/language/functions/time/millis/
  bool PumpOK = true;             ///Aeroponics - High pressure pump health
  bool PumpOn = false;            ///Aeroponics - High pressure pump state
  bool BypassSolenoidOn = false;  ///Aeroponics - Controls the bypass valve, true opens the solenoid
  bool MixInProgress = false;     ///Aeroponics - Used to temporary suspend pump timer and keep the high pressure pump on. Do not change.
  void checkRelays();  
};
