#pragma once

#include "420Common.h"
#include "420Module.h"

///<Light controller with PWM dimming

class Lights : virtual public Common
{
public:
  Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings); ///<constructor
  void refresh_Minute();
  void report();
  void setBrightness(uint8_t Brightness, bool AddToLog);
  void setLightOnOff(bool State, bool AddToLog);
  bool getStatus();
  int getBrightness();
  char *getBrightnessText();
  char *getTimerOnOffText(bool UseText);
  char *getStatusText(bool UseText);
  char *getOnTimeText();
  char *getOffTimeText();
  void setFadingOnOff();
  void setFadeStepInterval();
  void setFadeIncrements();

private:

protected:
  Module *Parent;
  void checkLightStatus();
  void checkLightTimer();
  void setTimerOnOff(bool State);
  void setOnHour(uint8_t OnHour);
  void setOnMinute(uint8_t OnMinute);
  void setOffHour(uint8_t OffHour);
  void setOffMinute(uint8_t OffMinute);
  uint8_t *RelayPin;         ///<the Arduino pin controlling the AC relay
  uint8_t *DimmingPin;       ///<PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
  uint8_t *DimmingLimit;     ///<Sets the LED dimming limit (Usually around 5%)
  bool *Status;              ///<Lights ON or OFF
  bool *TimerEnabled;        ///<Enable timer controlling lights: true - Timer enabled, false - Timer disabled
  uint8_t *OnHour;           ///<Light ON time - hour
  uint8_t *OnMinute;         ///<Light ON time - minute
  uint8_t *OffHour;          ///<Light OFF time - hour
  uint8_t *OffMinute;        ///<Light OFF time - minute
  uint8_t *Brightness;       ///<Light intensity: 0 - 100 range for controlling led driver output
  bool *FadingEnabled;              ///<Enables/disables fading lights in when turning on, and fading lights out when turning off
  uint16_t *FadingStepInterval;  ///<How often should the brightness change during a fade in/out in Seconds
  uint8_t *FadingIncrements;    ///<How much to change the brightness during a fade in/out in Percentage
  uint8_t CurrentBrightness; ///<Tracks the brightness during a fadein/out period
};