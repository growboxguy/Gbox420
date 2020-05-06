#include "Lights.h"
#include "Sound.h"

Lights::Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  RelayPin = &DefaultSettings->RelayPin;
  DimmingPin = &DefaultSettings->DimmingPin;
  DimmingLimit = &DefaultSettings->DimmingLimit; ///Blocks dimming below a certain percentage (default 8%), Most LED drivers cannot fully dim, check the specification and adjust accordingly, only set 0 if it supports dimming fully! (Usually not the case..)
  Status = &DefaultSettings->Status;
  Brightness = &DefaultSettings->Brightness;
  TimerEnabled = &DefaultSettings->TimerEnabled;
  OnHour = &DefaultSettings->OnHour;
  OnMinute = &DefaultSettings->OnMinute;
  OffHour = &DefaultSettings->OffHour;
  OffMinute = &DefaultSettings->OffMinute;
  pinMode(*RelayPin, OUTPUT);
  digitalWrite(*RelayPin, HIGH); ///Turn relay off initially
  pinMode(*DimmingPin, OUTPUT);
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Minute(this);  ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  logToSerials(F("Lights object created"), true, 1);
}

void Lights::refresh_Minute()
{ ///makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
  if (*Debug)
    Common::refresh_Minute();
  checkLightTimer();
  checkLightStatus();
}

void Lights::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Status:"));
  strcat(LongMessage, getStatusText(true));
  strcat_P(LongMessage, (PGM_P)F(" ; Brightness:"));
  strcat(LongMessage, toText(*Brightness));
  strcat_P(LongMessage, (PGM_P)F(" ; LightON:"));
  strcat(LongMessage, getOnTimeText());
  strcat_P(LongMessage, (PGM_P)F(" ; LightOFF:"));
  strcat(LongMessage, getOffTimeText());
  logToSerials(&LongMessage, true, 1);
}

void Lights::checkLightStatus()
{
  if (*Status)
    digitalWrite(*RelayPin, LOW); ///True turns relay ON (LOW signal activates Relay)
  else
    digitalWrite(*RelayPin, HIGH); ///HIGH turns relay OFF
}

void Lights::checkLightTimer()
{
  if (*TimerEnabled)
  {
    time_t Now = now();                             /// Get the current time from TimeLib
    int CombinedOnTime = *OnHour * 100 + *OnMinute; ///convert time to number, Example: 8:10=810, 20:10=2010
    int CombinedOffTime = *OffHour * 100 + *OffMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if (CombinedOnTime <= CombinedOffTime) ///no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if (CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime)
      { ///True: Light should be on
        if (!*Status)
        {
          setLightOnOff(true, false); ///If status is OFF: Turn ON the lights (First bool), and do not add it to the log (Second bool)
          if (*Debug)
            logToSerials(F("Timer:Light ON"), true, 4);
        }
      }
      else ///False: Light should be off
          if (*Status)
      {                              ///If status is ON
        setLightOnOff(false, false); ///Turn OFF the lights (First bool), and do not add it to the log (Second bool)
        if (*Debug)
          logToSerials(F("Timer:Light OFF"), true, 4);
      }
    }
    else ///midnight turnover, Example: On 21:20, Off: 9:20
    {
      if (CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime)
      {
        if (!*Status)
        {
          setLightOnOff(true, false);
          if (*Debug)
            logToSerials(F("Timer:Light ON"), true, 4);
        }
      }
      else if (*Status)
      {
        setLightOnOff(false, false);
        if (*Debug)
          logToSerials(F("Timer:Light OFF"), true, 4);
      }
    }
  }
}

void Lights::setBrightness(uint8_t Brightness, bool LogThis)
{
  *(this->Brightness) = Brightness;
  analogWrite(*DimmingPin, map(Brightness, 0, 100, int(255 * (100 - *DimmingLimit) / 100.0f), 0)); ///mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brightness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https:///www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  if (LogThis)
  {
    strncpy_P(LongMessage, (PGM_P)F("Brightness: "), MaxTextLength);
    strcat(LongMessage, toText(Brightness));
    strcat_P(LongMessage, (PGM_P)F("%"));
    Parent->addToLog(LongMessage);
  }
}

void Lights::setLightOnOff(bool Status, bool LogThis)
{
  *(this->Status) = Status;
  if (LogThis)
  {
    if (Status)
    {
      Parent->addToLog(F("Light ON"));
      Parent->getSoundObject()->playOnSound();
    }
    else
    {
      Parent->addToLog(F("Light OFF"));
      Parent->getSoundObject()->playOffSound();
    }
  }
  checkLightStatus();
}

char *Lights::getTimerOnOffText(bool UseWords)
{
  if (UseWords)
    return toText_enabledDisabled(*TimerEnabled); ///Returns ENABLED or DISABLED
  else
    return toText(*TimerEnabled); ///Returns '1' or '0'
}

bool Lights::getStatus()
{
  return *Status;
}

int Lights::getBrightness()
{
  return *Brightness;
}

char *Lights::getBrightnessText()
{
  return toText(*Brightness);
}

char *Lights::getStatusText(bool UseWords)
{
  if (UseWords)
    return toText_onOff(*Status); ///Returns ON or OFF
  else
    return toText(*Status); ///Returns '1' or '0'
}

char *Lights::getOnTimeText()
{
  return toText_time(*OnHour, *OnMinute);
}
char *Lights::getOffTimeText()
{
  return toText_time(*OffHour, *OffMinute);
}

void Lights::setTimerOnOff(bool TimerState)
{
  *(this->TimerEnabled) = TimerState;
  if (*TimerEnabled)
  {
    checkLightTimer();
    Parent->addToLog(F("Timer enabled"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(F("Timer disabled"));
    Parent->getSoundObject()->playOffSound();
  }
}

void Lights::setOnHour(uint8_t OnHour)
{
  *(this->OnHour) = OnHour;
}

void Lights::setOnMinute(uint8_t OnMinute)
{
  *(this->OnMinute) = OnMinute;
  Parent->addToLog(F("Light ON updated"));
}

void Lights::setOffHour(uint8_t OffHour)
{
  *(this->OffHour) = OffHour;
}

void Lights::setOffMinute(uint8_t OffMinute)
{
  *(this->OffMinute) = OffMinute;
  Parent->addToLog(F("Light OFF updated"));
}