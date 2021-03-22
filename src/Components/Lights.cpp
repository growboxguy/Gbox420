#include "Lights.h"
#include "Sound.h"

Lights::Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  RelayPin = &DefaultSettings->RelayPin;
  DimmingPin = &DefaultSettings->DimmingPin;
  DimmingLimit = &DefaultSettings->DimmingLimit; ///< Blocks dimming below a certain percentage (default 8%), Most LED drivers cannot fully dim, check the specification and adjust accordingly, only set 0 if it supports dimming fully! (Usually not the case..)
  DimmingDuration = &DefaultSettings->DimmingDuration;
  Status = &DefaultSettings->Status;
  Brightness = &DefaultSettings->Brightness;
  FadingEnabled = &DefaultSettings->FadingEnabled;
  FadingInterval = &DefaultSettings->FadingInterval;
  FadingIncrements = &DefaultSettings->FadingIncrements;
  TimerEnabled = &DefaultSettings->TimerEnabled;
  OnHour = &DefaultSettings->OnHour;
  OnMinute = &DefaultSettings->OnMinute;
  OffHour = &DefaultSettings->OffHour;
  OffMinute = &DefaultSettings->OffMinute;
  pinMode(*RelayPin, OUTPUT);
  digitalWrite(*RelayPin, HIGH); ///< Turn relay off initially
  pinMode(*DimmingPin, OUTPUT);
  if (*FadingEnabled)
  {
    CurrentBrightness = 0; ///< start with low light intensity and slowly fade in to target Brightness
  }
  else
  {
    CurrentBrightness = *Brightness; ///< instantly jump to target Brightness
  }
  setBrightness(CurrentBrightness, false, false); ///< Set initial brightness
  setLightOnOff(*Status, false);
  checkRelay();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("Lights object created"), true, 3);
}

void Lights::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  checkDimming();
}

void Lights::refresh_Minute()
{ ///< makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
  if (*Debug)
    Common::refresh_Minute();
  checkTimer();
  checkRelay();
}

void Lights::report(bool JSONReport)
{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, getStatusText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"CB\":\""));
    strcat(LongMessage, getCurrentBrightnessText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"B\":\""));
    strcat(LongMessage, getBrightnessText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"T\":\""));
    strcat(LongMessage, getTimerOnOffText(false));
    strcat_P(LongMessage, (PGM_P)F("\",\"On\":\""));
    strcat(LongMessage, getOnTimeText());
    strcat_P(LongMessage, (PGM_P)F("\",\"Of\":\""));
    strcat(LongMessage, getOffTimeText());
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
  }
  else //Print a report to the Serial console
  {
    Common::report();
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("State:"));
    strcat(LongMessage, getStateText());
    strcat_P(LongMessage, (PGM_P)F(" ; Brightness:"));
    strcat(LongMessage, toText_percentage(*Brightness));
    if (*Debug || CurrentStatus == LightStates::FADEIN || CurrentStatus == LightStates::FADEOUT)
    {
      strcat_P(LongMessage, (PGM_P)F(" ("));
      strcat(LongMessage, toText_percentage(CurrentBrightness));
      strcat_P(LongMessage, (PGM_P)F(")"));
    }
    strcat_P(LongMessage, (PGM_P)F(" ; LightON:"));
    strcat(LongMessage, getOnTimeText());
    strcat_P(LongMessage, (PGM_P)F(" ; LightOFF:"));
    strcat(LongMessage, getOffTimeText());
    logToSerials(&LongMessage, true, 1);
  }
}

void Lights::checkRelay()
{
  if (CurrentStatus == LightStates::TURNEDOFF)
    digitalWrite(*RelayPin, HIGH); ///< True turns relay OFF (HIGH signal de-activates Relay)
  else
    digitalWrite(*RelayPin, LOW); ///< LOW turns relay ON
}

void Lights::dimLightsOnOff()
{
  appendName(true);
  if (CurrentStatus == LightStates::DIMMED)
  { ///< If temporary dimming is already ON -> Turn it OFF
    CurrentStatus = BeforeDimmingState;
    setBrightness(BeforeDimmingBrightness, false, false);
    strcat_P(ShortMessage, (PGM_P)F("dimming OFF"));
    Parent->getSoundObject()->playOffSound();
  }
  else ///< If temporary dimming is OFF -> Turn it ON
  {
    BeforeDimmingState = CurrentStatus;
    BeforeDimmingBrightness = CurrentBrightness;
    CurrentStatus = LightStates::DIMMED;
    setBrightness(1, false, false);
    DimmingTimer = millis();
    strcat_P(ShortMessage, (PGM_P)F("dimming ON"));
    Parent->getSoundObject()->playOnSound();
  }
  checkDimming();
  Parent->addToLog(ShortMessage);
}

void Lights::checkDimming()
{
  if (CurrentStatus == LightStates::FADEIN)
  {
    if (millis() - FadingTimer >= *FadingInterval * 1000)
    {
      CurrentBrightness = CurrentBrightness + *FadingIncrements; //Increase current brightness
      if (CurrentBrightness >= *Brightness)                      ///< Check if the target brightness is reached
      {
        CurrentBrightness = *Brightness;
        CurrentStatus = LightStates::TURNEDON;
      }
      setBrightness(CurrentBrightness, false, false); ///< Set new brightness
      FadingTimer = millis();                         ///< Store the timestamp of the last change
    }
  }
  else if (CurrentStatus == LightStates::FADEOUT)
  {
    if (millis() - FadingTimer >= *FadingInterval * 1000)
    {
      CurrentBrightness = CurrentBrightness - *FadingIncrements; //Decrease current brightness
      if (CurrentBrightness <= 0)                                ///< Check if zero brightness is reached
      {
        CurrentBrightness = 0;
        CurrentStatus = LightStates::TURNEDOFF;
      }
      setBrightness(CurrentBrightness, false, false); ///< Set new brightness
      FadingTimer = millis();                         ///< Store the timestamp of the last change
    }
  }
  else if (CurrentStatus == LightStates::DIMMED)
  {
    if (millis() - DimmingTimer >= *DimmingDuration * 60000)
    {
      dimLightsOnOff();
    }
  }
  checkRelay();
}

void Lights::checkTimer()
{
  if (*TimerEnabled)
  {
    time_t Now = now();                             // Get the current time from TimeLib
    int CombinedOnTime = *OnHour * 100 + *OnMinute; // convert time to number, Example: 8:10=810, 20:10=2010
    int CombinedOffTime = *OffHour * 100 + *OffMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if (CombinedOnTime <= CombinedOffTime) // no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if (CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime)
      { // True: Light should be on
        if (!*Status)
        {
          setLightOnOff(true, false); // If status is OFF: Turn ON the lights (First bool), and do not add it to the log (Second bool)
          if (*Debug)
          {
            appendName(true);
            strcat_P(ShortMessage, (PGM_P)F("timer: ON"));
            logToSerials(&ShortMessage);
          }
        }
      }
      else // False: Light should be off
      {
        if (*Status)
        {                              // If status is ON
          setLightOnOff(false, false); // Turn OFF the lights (First bool), and do not add it to the log (Second bool)
          if (*Debug)
          {
            appendName(true);
            strcat_P(ShortMessage, (PGM_P)F("timer: OFF"));
            logToSerials(&ShortMessage);
          }
        }
      }
    }
    else // midnight turnover, Example: On 21:20, Off: 9:20
    {
      if (CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime)
      {
        if (!*Status)
        {
          setLightOnOff(true, false);
          if (*Debug)
          {
            appendName(true);
            strcat_P(ShortMessage, (PGM_P)F("timer ON"));
            logToSerials(&ShortMessage);
          }
        }
      }
      else if (*Status)
      {
        setLightOnOff(false, false);
        if (*Debug)
        {
          appendName(true);
          strcat_P(ShortMessage, (PGM_P)F("timer OFF"));
          logToSerials(&ShortMessage);
        }
      }
    }
  }
}

void Lights::setBrightness(uint8_t Brightness, bool LogThis, bool StoreSetting)
{
  CurrentBrightness = Brightness; // Sets the dimming duty cycle (0-100%)
  if (StoreSetting)
  {
    *(this->Brightness) = Brightness; // Store to EEPROM as a startup value
  }
  analogWrite(*DimmingPin, map(CurrentBrightness, 0, 100, int(255 * (100 - *DimmingLimit) / 100.0f), 0)); // mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brightness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  if (LogThis)
  {
    appendName(true);
    strcat_P(ShortMessage, (PGM_P)F("brightness updated"));
    Parent->addToLog(ShortMessage);
  }
}

void Lights::setDimDuration(int DimmingDuration)
{
  *(this->DimmingDuration) = DimmingDuration;
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("dimming updated"));
  Parent->addToLog(ShortMessage);
}

void Lights::setLightOnOff(bool Status, bool LogThis)
{
  if (Status)
  {
    if (LogThis)
    {
      appendName(true);
      strcat_P(ShortMessage, (PGM_P)F("ON"));
      Parent->addToLog(ShortMessage);
    }
    Parent->getSoundObject()->playOnSound();
    if (*FadingEnabled && CurrentStatus != LightStates::FADEIN && CurrentStatus != LightStates::TURNEDON)
    {
      CurrentStatus = LightStates::FADEIN;
      CurrentBrightness = 0; // Start fading in from 0%
      setBrightness(CurrentBrightness, false, false);
    }
    else
    {
      CurrentStatus = LightStates::TURNEDON;
      CurrentBrightness = *Brightness; // Instantly set the target Brightness
      setBrightness(CurrentBrightness, false, false);
    }
  }
  else
  {
    if (LogThis)
    {
      appendName(true);
      strcat_P(ShortMessage, (PGM_P)F("OFF"));
      Parent->addToLog(ShortMessage);
    }
    Parent->getSoundObject()->playOffSound();

    if (*FadingEnabled && CurrentStatus != LightStates::TURNEDOFF && CurrentStatus != LightStates::FADEOUT)
    {
      CurrentStatus = LightStates::FADEOUT;
      //CurrentBrightness = *Brightness; // Start fading out from the target brightness
      //setBrightness(CurrentBrightness, false, false);
    }
    else
    {
      CurrentStatus = LightStates::TURNEDOFF;
    }
  }
  *(this->Status) = Status;
  checkRelay();
}

char *Lights::getTimerOnOffText(bool UseWords)
{
  if (UseWords)
    return toText_enabledDisabled(*TimerEnabled); // Returns ENABLED or DISABLED
  else
    return toText(*TimerEnabled); // Returns '1' or '0'
}

bool Lights::getStatus()
{
  return *Status;
}

int Lights::getBrightness()
{
  return *Brightness;
}

char *Lights::getBrightnessText(bool UseText)
{
  itoa(*Brightness, ShortMessage, 10);
  if (UseText)
  {
    strcat_P(ShortMessage, (PGM_P)F("%"));
  }
  return ShortMessage;
}

char *Lights::getCurrentBrightnessText(bool UseText)
{
  if (CurrentStatus == LightStates::FADEIN || CurrentStatus == LightStates::FADEOUT)
  {
    itoa(CurrentBrightness, ShortMessage, 10);
  }
  else if (!*Status) //When the light is off
  {
    itoa(0, ShortMessage, 10);
  }
  else
  {
    itoa(*Brightness, ShortMessage, 10);
  }

  if (UseText)
  {
    strcat_P(ShortMessage, (PGM_P)F("%"));
  }
  return ShortMessage;
}

char *Lights::getStatusText(bool UseWords)
{
  if (UseWords)
    return toText_onOff(*Status); // Returns ON or OFF
  else
    return toText(*Status); // Returns '1' or '0'
}

char *Lights::getStateText()
{
  switch (CurrentStatus)
  {
  case LightStates::TURNEDOFF:
    return toText(F("OFF"));
    break;
  case LightStates::TURNEDON:
    return toText(F("ON"));
    break;
  case LightStates::FADEIN:
    return toText(F("FADEIN"));
    break;
  case LightStates::FADEOUT:
    return toText(F("FADEOUT"));
    break;
  case LightStates::DIMMED:
    return toText(F("DIMMED"));
    break;
  default:
    return toText(F("UNKNOWN"));
    break;
  }
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
  appendName(true);
  if (*TimerEnabled)
  {
    checkTimer();
    strcat_P(ShortMessage, (PGM_P)F("timer ON"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("timer OFF"));
    Parent->getSoundObject()->playOffSound();
  }
  Parent->addToLog(ShortMessage);
}

void Lights::setOnTime(char *OnTime)
{
  if (strlen(OnTime) == 4) //strlen does not include the null terminator in the size like sizeof() does
  {
    char Hour[3]; //null terminator included
    memcpy(Hour, &OnTime[0], 2);
    Hour[2] = '\0';
    setOnHour(toInt(Hour));

    char Minute[3];
    memcpy(Minute, &OnTime[2], 2);
    Hour[2] = '\0';
    setOnMinute(toInt(Minute));
  }
}

void Lights::setOnHour(uint8_t OnHour)
{
  *(this->OnHour) = OnHour;
}

void Lights::setOnMinute(uint8_t OnMinute)
{
  *(this->OnMinute) = OnMinute;
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("ON time updated"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

void Lights::setOffTime(char *OffTime)
{
  if (strlen(OffTime) == 4) //strlen does not include the null terminator in the size like sizeof() does
  {
    char Hour[3]; //null terminator included
    memcpy(Hour, &OffTime[0], 2);
    Hour[2] = '\0';
    setOffHour(toInt(Hour));

    char Minute[3];
    memcpy(Minute, &OffTime[2], 2);
    Hour[2] = '\0';
    setOffMinute(toInt(Minute));
  }
}

void Lights::setOffHour(uint8_t OffHour)
{
  *(this->OffHour) = OffHour;
}

void Lights::setOffMinute(uint8_t OffMinute)
{
  *(this->OffMinute) = OffMinute;
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("OFF time updated"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

void Lights::setFadeOnOff(bool State)
{
  *FadingEnabled = State;
  appendName(true);
  if (*FadingEnabled)
  {
    strcat_P(ShortMessage, (PGM_P)F("fading ON"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    strcat_P(ShortMessage, (PGM_P)F("fading OFF"));
    Parent->getSoundObject()->playOffSound();
  }
  Parent->addToLog(ShortMessage);
}

void Lights::setFadeInterval(uint16_t Interval)
{
  *FadingInterval = Interval;
}

void Lights::setFadeIncrements(uint8_t Increment)
{
  *FadingIncrements = Increment;
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("fading updated"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}