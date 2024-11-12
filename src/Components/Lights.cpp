#include "Lights.h"
#include "Sound.h"

Lights::Lights(const __FlashStringHelper *Name, Module *Parent, Settings::LightsSettings *DefaultSettings) 
  : Common(Name),
    Parent(Parent),
    RelayPin(DefaultSettings->RelayPin),
    DimmingPin(DefaultSettings->DimmingPin),
    DimmingLimit(DefaultSettings->DimmingLimit), 
    DimmingDuration(DefaultSettings->DimmingDuration),
    Status(DefaultSettings->Status),
    Brightness(DefaultSettings->Brightness),
    FadingEnabled(DefaultSettings->FadingEnabled),
    FadingInterval(DefaultSettings->FadingInterval),
    FadingIncrements(DefaultSettings->FadingIncrements),
    TimerEnabled(DefaultSettings->TimerEnabled),
    OnHour(DefaultSettings->OnHour),
    OnMinute(DefaultSettings->OnMinute),
    OffHour(DefaultSettings->OffHour),
    OffMinute(DefaultSettings->OffMinute)
{
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH); ///< Turn relay off initially
  pinMode(DimmingPin, OUTPUT);
  if (FadingEnabled)
  {
    CurrentBrightness = 0; ///< start with low light intensity and slowly fade in to target Brightness
  }
  else
  {
    CurrentBrightness = Brightness; ///< instantly jump to target Brightness
  }  
  setBrightness(CurrentBrightness, false, false); ///< Set initial brightness
  setLightOnOff(Status, false);
  checkRelay();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("Lights ready"), true, 3);
}

void Lights::refresh_Sec()
{
  Common::refresh_Sec();
  checkDimming();
}

void Lights::refresh_Minute()
{ ///< makes the class non-virtual, by implementing the refresh function from Common (Else you get an error while trying to create a new Lights object: invalid new-expression of abstract class type 'Lights')
  Common::refresh_Minute();
  checkTimer();
  checkRelay();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Lights::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getStatusText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"CB\":\""));
  strcat(LongMessage, getCurrentBrightnessText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"B\":\""));
  strcat(LongMessage, getBrightnessText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"T\":\""));
  strcat(LongMessage, getTimerOnOffText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"On\":\""));
  strcat(LongMessage, getOnTimeText());
  strcat_P(LongMessage, (PGM_P)F("\",\"Of\":\""));
  strcat(LongMessage, getOffTimeText());
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Lights::checkRelay()
{
  if (CurrentStatus == LightStates::TURNEDOFF)
    digitalWrite(RelayPin, HIGH); ///< True turns relay OFF (HIGH signal de-activates Relay)
  else
    digitalWrite(RelayPin, LOW); ///< LOW turns relay ON
}

void Lights::dimLightsOnOff()
{
  if (CurrentStatus == LightStates::DIMMED)
  { ///< If temporary dimming is already ON -> Turn it OFF
    CurrentStatus = BeforeDimmingState;
    setBrightness(BeforeDimmingBrightness, false, false);
    Parent->addToLog(getName(F("dimming OFF")));
    Parent->getSoundObject()->playOffSound();
  }
  else ///< If temporary dimming is OFF -> Turn it ON
  {
    BeforeDimmingState = CurrentStatus;
    BeforeDimmingBrightness = CurrentBrightness;
    CurrentStatus = LightStates::DIMMED;
    setBrightness(1, false, false);
    DimmingTimer = millis();
    Parent->addToLog(getName(F("dimming ON")));
    Parent->getSoundObject()->playOnSound();
  }
  checkDimming();
}

void Lights::checkDimming()
{
  if (CurrentStatus == LightStates::FADEIN)
  {
    if (millis() - FadingTimer >= FadingInterval * 1000)
    {
      CurrentBrightness = CurrentBrightness + FadingIncrements; //Increase current brightness
      if (CurrentBrightness >= Brightness)                      ///< Check if the target brightness is reached
      {
        CurrentBrightness = Brightness;
        CurrentStatus = LightStates::TURNEDON;
      }
      setBrightness(CurrentBrightness, false, false); ///< Set new brightness
      FadingTimer = millis();                         ///< Store the timestamp of the last change
    }
  }
  else if (CurrentStatus == LightStates::FADEOUT)
  {
    if (millis() - FadingTimer >= FadingInterval * 1000)
    {
      CurrentBrightness = CurrentBrightness - FadingIncrements; //Decrease current brightness
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
    if (millis() - DimmingTimer >= DimmingDuration * 60000)
    {
      dimLightsOnOff();
    }
  }
  checkRelay();
}

void Lights::checkTimer()
{
  if (TimerEnabled)
  {
    time_t Now = now();                             // Get the current time from TimeLib
    int CombinedOnTime = OnHour * 100 + OnMinute; // convert time to number, Example: 8:10=810, 20:10=2010
    int CombinedOffTime = OffHour * 100 + OffMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if (CombinedOnTime <= CombinedOffTime) // no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if (CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime)
      { // True: Light should be on
        if (!Status)
        {
          setLightOnOff(true, true); // If status is OFF: Turn ON the lights (First bool), and do not add it to the log (Second bool)
        }
      }
      else // False: Light should be off
      {
        if (Status)
        {                             // If status is ON
          setLightOnOff(false, true); // Turn OFF the lights (First bool), and do not add it to the log (Second bool)
        }
      }
    }
    else // midnight turnover, Example: On 21:20, Off: 9:20
    {
      if (CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime)
      {
        if (!Status)
        {
          setLightOnOff(true, true);
        }
      }
      else if (Status)
      {
        setLightOnOff(false, true);
      }
    }
  }
}

void Lights::setBrightness(uint8_t Brightness, bool LogThis, bool StoreSetting)
{
  CurrentBrightness = Brightness; // Sets the dimming duty cycle (0-100%)
  if (StoreSetting)
  {
    this->Brightness = Brightness; // Store to EEPROM as a startup value
  }
  analogWrite(DimmingPin, map(CurrentBrightness, 0, 100, int(255 * (100 - DimmingLimit) / 100.0f), 0)); // mapping brightness to duty cycle. Example 1: Mapping Brightness 100 -> PWM duty cycle will be 0% on Arduino side, 100% on LED driver side. Example2: Mapping Brightness 0 with Dimming limit 8% ->  int(255*((100-8)/100)) ~= 234 AnalogWrite (92% duty cycle on Arduino Side, 8% in Driver dimming side) https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  if (LogThis)
  {
    Parent->addToLog(getName(F("brightness updated")));
  }
}

void Lights::setDimDuration(int DimmingDuration)
{
  this->DimmingDuration = DimmingDuration;
  Parent->addToLog(getName(F("dimming updated")));
}

void Lights::setLightOnOff(bool Status, bool LogThis)
{
  if (Status)
  {
    if (LogThis)
    {
      Parent->addToLog(getName(getStatusText(true)));
    }
    Parent->getSoundObject()->playOnSound();
    if (FadingEnabled && CurrentStatus != LightStates::FADEIN && CurrentStatus != LightStates::TURNEDON)
    {
      CurrentStatus = LightStates::FADEIN;
      CurrentBrightness = 0; // Start fading in from 0%
      setBrightness(CurrentBrightness, false, false);
    }
    else
    {
      CurrentStatus = LightStates::TURNEDON;
      CurrentBrightness = Brightness; // Instantly set the target Brightness
      setBrightness(CurrentBrightness, false, false);
    }
  }
  else
  {
    if (LogThis)
    {
      Parent->addToLog(getName(getStatusText(true)));
    }
    Parent->getSoundObject()->playOffSound();

    if (FadingEnabled && CurrentStatus != LightStates::TURNEDOFF && CurrentStatus != LightStates::FADEOUT)
    {
      CurrentStatus = LightStates::FADEOUT;
      //CurrentBrightness = Brightness; // Start fading out from the target brightness
      //setBrightness(CurrentBrightness, false, false);
    }
    else
    {
      CurrentStatus = LightStates::TURNEDOFF;
    }
  }
  this->Status = Status;
  checkRelay();
}

char *Lights::getTimerOnOffText(bool UseWords)
{
  if (UseWords)
    return toText_enabledDisabled(TimerEnabled); // Returns ENABLED or DISABLED
  else
    return toText(TimerEnabled); // Returns '1' or '0'
}

bool Lights::getStatus()
{
  return Status;
}

int Lights::getBrightness()
{
  return Brightness;
}

char *Lights::getBrightnessText(bool FriendlyFormat)
{
  itoa(Brightness, ShortMessage, 10);
  if (FriendlyFormat)
  {
    strcat_P(ShortMessage, (PGM_P)F("%"));
  }
  return ShortMessage;
}

char *Lights::getCurrentBrightnessText(bool FriendlyFormat)
{
  if (CurrentStatus == LightStates::FADEIN || CurrentStatus == LightStates::FADEOUT)
  {
    itoa(CurrentBrightness, ShortMessage, 10);
  }
  else if (!Status) //When the light is off
  {
    itoa(0, ShortMessage, 10);
  }
  else
  {
    itoa(Brightness, ShortMessage, 10);
  }

  if (FriendlyFormat)
  {
    strcat_P(ShortMessage, (PGM_P)F("%"));
  }
  return ShortMessage;
}

char *Lights::getStatusText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_onOff(Status); // Returns ON or OFF
  else
    return toText(Status); // Returns '1' or '0'
}

char *Lights::getStateText()
{
  return toText_lightState(CurrentStatus);
}

char *Lights::getOnTimeText()
{
  return toText_time(OnHour, OnMinute);
}
char *Lights::getOffTimeText()
{
  return toText_time(OffHour, OffMinute);
}

void Lights::setTimerOnOff(bool TimerState)
{
  this->TimerEnabled = TimerState;
  if (TimerEnabled)
  {
    Parent->addToLog(getName(F("timer ON")));
    checkTimer();
  }
  else
  {
    Parent->addToLog(getName(F("timer OFF")));
  }
  Parent->getSoundObject()->playOnOffSound(TimerEnabled);
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
    Minute[2] = '\0';
    setOnMinute(toInt(Minute));
  }
}

void Lights::setOnHour(uint8_t OnHour)
{
  this->OnHour = OnHour;
}

void Lights::setOnMinute(uint8_t OnMinute)
{
  this->OnMinute = OnMinute;
  Parent->addToLog(getName(F("ON time updated")));
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
    Minute[2] = '\0';
    setOffMinute(toInt(Minute));
  }
}

void Lights::setOffHour(uint8_t OffHour)
{
  this->OffHour = OffHour;
}

void Lights::setOffMinute(uint8_t OffMinute)
{
  this->OffMinute = OffMinute;
  Parent->addToLog(getName(F("OFF time updated")));
  Parent->getSoundObject()->playOnSound();
}

void Lights::setFadeOnOff(bool State)
{
  FadingEnabled = State;
  if (FadingEnabled)
  {
    Parent->addToLog(getName(F("fading ON")));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(getName(F("fading OFF")));
    Parent->getSoundObject()->playOffSound();
  }
  Parent->addToLog(ShortMessage);
}

void Lights::setFadeInterval(uint16_t Interval)
{
  FadingInterval = Interval;
}

void Lights::setFadeIncrements(uint8_t Increment)
{
  FadingIncrements = Increment;
  Parent->addToLog(getName(F("fading updated")));
  Parent->getSoundObject()->playOnSound();
}