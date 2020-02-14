#pragma once

#include "420Common.h"
#include "../Modules/420Module.h"

class Lights : virtual public Common
{
public:
  Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings); //constructor
  void refresh_Minute();
  void report();
  void setBrightness(byte Brightness, bool AddToLog);
  void setLightOnOff(bool State, bool AddToLog);
  bool getStatus();
  int getBrightness();
  char *getBrightnessText();
  char *getTimerOnOffText(bool UseText);
  char *getStatusText(bool UseText);
  char *getOnTimeText();
  char *getOffTimeText();

private:  

protected:
  Module *Parent;
  void checkLightStatus();
  void checkLightTimer();
  void setTimerOnOff(bool State);
  void setOnHour(byte OnHour);
  void setOnMinute(byte OnMinute);
  void setOffHour(byte OffHour);
  void setOffMinute(byte OffMinute);
  byte *RelayPin;     //the Arduino pin controlling the AC relay
  byte *DimmingPin;   //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
  byte *DimmingLimit; //Sets the LED dimming limit (Usually around 5%)
  bool *Status;       //Lights ON or OFF
  bool *TimerEnabled; //Enable timer controlling lights: true - Timer enabled, false - Timer disabled
  byte *OnHour;       //Light ON time - hour
  byte *OnMinute;     //Light ON time - minute
  byte *OffHour;      //Light OFF time - hour
  byte *OffMinute;    //Light OFF time - minute
  byte *Brightness;   //Light intensity: 0 - 100 range for controlling led driver output
};