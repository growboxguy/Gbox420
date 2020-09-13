#pragma once

#include "420Common.h"
#include "420Module.h"

///<Light controller with PWM dimming
enum LightStates
{
  OFF,
  ON,
  FADEIN,
  FADEOUT
};

class Lights : virtual public Common
{
public:
  Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings); ///<constructor
  void refresh_Sec();
  void refresh_Minute();
  void report();
  void setBrightness(uint8_t Brightness, bool AddToLog, bool StoreSetting);
  void setLightOnOff(bool State, bool AddToLog);
  bool getStatus();
  int getBrightness();
  char *getBrightnessText();
  char *getTimerOnOffText(bool UseText);
  char *getStatusText(bool UseText);
  char *getStateText();

  char *getOnTimeText();
  char *getOffTimeText();
  void setFadeOnOff(bool State);
  void setFadeInterval(uint16_t Interval);
  void setFadeIncrements(uint8_t Increment);

private:
protected:
  Module *Parent;
  void checkRelay();
  void checkTimer();
  void checkDimming();
  void setTimerOnOff(bool State);
  void setOnHour(uint8_t OnHour);
  void setOnMinute(uint8_t OnMinute);
  void setOffHour(uint8_t OffHour);
  void setOffMinute(uint8_t OffMinute);
  uint8_t *RelayPin;         ///<the Arduino pin controlling the AC relay
  uint8_t *DimmingPin;       ///<PWM based dimming, connected to optocoupler`s base over 1k ohm resistor0
  uint8_t *DimmingLimit;     ///<Sets the LED dimming limit (Usually around 5%)
  bool *Status;              ///<Lights ON or OFF
  LightStates CurrentState = LightStates::OFF;  ///<Tracks light state during fading in/out
  bool *TimerEnabled;        ///<Enable timer controlling lights: true - Timer enabled, false - Timer disabled
  uint8_t *OnHour;           ///<Light ON time - hour
  uint8_t *OnMinute;         ///<Light ON time - minute
  uint8_t *OffHour;          ///<Light OFF time - hour
  uint8_t *OffMinute;        ///<Light OFF time - minute
  uint8_t *Brightness;       ///<Light intensity target: 0% - 100% range for controlling led driver output
  int16_t CurrentBrightness; ///<Tracks light intensity while fading in/out
  bool *FadingEnabled;       ///<Enables/disables fading lights in when turning on, and fading lights out when turning off
  uint32_t FadingTimer = 0;  ///Stores the last time the Fadein/out increased/decreased a increment
  uint16_t *FadingInterval;  ///<How often should the brightness change during a fade in/out (Seconds)
  uint8_t *FadingIncrements; ///<How much to change the brightness during a fade in/out (Percentage)
};