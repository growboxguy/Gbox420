#pragma once

#include "420Common.h"
#include "420Module.h"

class Lights : virtual public Common
{
public:
  Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings); ///< constructor
  void refresh_Sec();
  void refresh_Minute();
  void report(bool FriendlyFormat = false);
  void setBrightness(uint8_t Brightness, bool AddToLog, bool StoreSetting);
  void setLightOnOff(bool State, bool AddToLog);
  void setTimerOnOff(bool State);
  void setOnTime(char *OnTime); // Format: hhmm (24h). Example: 0420 (4:20AM), 1620 (4:20PM)
  void setOnHour(uint8_t OnHour);
  void setOnMinute(uint8_t OnMinute);
  void setOffTime(char *OffTime); // Format: hhmm (24h). Example: 0420 (4:20AM), 1620 (4:20PM)
  void setOffHour(uint8_t OffHour);
  void setOffMinute(uint8_t OffMinute);
  bool getStatus();
  int getBrightness();
  char *getBrightnessText(bool FriendlyFormat);
  char *getCurrentBrightnessText(bool FriendlyFormat); // Takes into account the ON/OFF state of the light too
  char *getTimerOnOffText(bool FriendlyFormat);
  char *getStatusText(bool FriendlyFormat);
  char *getStateText();
  char *getOnTimeText();
  char *getOffTimeText();
  void dimLightsOnOff();                     ///< Temporary dim the lights while working on the plants
  void setDimDuration(int DimDuration);      ///< Set the duration of the temporary dimming in Seconds
  void setFadeOnOff(bool State);             ///< Enable/disable slowly fadeing in and out the lights when the Status changes
  void setFadeInterval(uint16_t Interval);   ///< Set how often to increase/decrease the Brightness
  void setFadeIncrements(uint8_t Increment); ///< Set the Percentage of change at one increment/decrement
  uint8_t &OnHour;                           ///< Light ON time - hour
  uint8_t &OnMinute;                         ///< Light ON time - minute
  uint8_t &OffHour;                          ///< Light OFF time - hour
  uint8_t &OffMinute;                        ///< Light OFF time - minute
  uint8_t &Brightness;                       ///< Light intensity target: 0% - 100% range for controlling led driver output

private:
protected:
  Module *Parent;
  void checkTimer();                                       ///< Based on ON/OFF time decide if AC Relays should be ON or OFF
  void checkDimming();                                     ///< Currently unused - Can fade-on our fade out
  uint8_t &RelayPin;                                       ///< the Arduino pin controlling the AC relay
  uint8_t &DimmingPin;                                     ///< PWM based dimming, connected to optocoupler's base over 1k ohm resistor
  uint8_t &DimmingLimit;                                   ///< Sets the LED dimming LOW limit (Usually around 5%)
  uint8_t &DimmingDuration;                                ///< Timeout of temporary dimming in Minutes
  LightStates BeforeDimmingState = LightStates::TURNEDOFF; ///< Stores the state before the temporary dimming was activated
  uint8_t BeforeDimmingBrightness;                         ///< Stores the brightness before the temporary dimming was activated
  uint32_t DimmingTimer = 0;                               ///< Stores the time when the temporary dimming got enabled
  bool &Status;                                            ///< Lights ON or OFF
  LightStates CurrentStatus = LightStates::TURNEDOFF;      ///< Tracks light state during fading in/out
  bool &TimerEnabled;                                      ///< Enable timer controlling lights: true - Timer enabled, false - Timer disabled
  uint8_t CurrentBrightness;                               ///< Tracks light intensity while fading in/out
  bool &FadingEnabled;                                     ///< Enables/disables fading lights in when turning on, and fading lights out when turning off
  uint32_t FadingTimer = 0;                                ///< Stores the last time the Fade in/out increased/decreased an increment
  uint16_t &FadingInterval;                                ///< How often should the brightness change during a fade in/out (Seconds)
  uint8_t &FadingIncrements;                               ///< How much to change the brightness during a fade in/out (Percentage)
};