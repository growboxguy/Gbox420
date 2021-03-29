#include "Fan_PWM.h"
#include "Sound.h"

///< PWM control an AC device using a Robodyn AC Light Dimming Module with Zero-cross detection

Fan_PWM::Fan_PWM(const __FlashStringHelper *Name, Module *Parent, Settings::Fan_PWMSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  ZeroCrossingPin = &DefaultSettings->ZeroCrossingPin;
  PWMPin = &DefaultSettings->PWMPin;
  State = &DefaultSettings->State;
  MinSpeed = &DefaultSettings->MinSpeed;
  Speed = &DefaultSettings->Speed;
  PWMDimmer = new dimmerLamp(*PWMPin);
  PWMDimmer->begin(NORMAL_MODE, OFF);
  PWMDimmer->setPower(*Speed);
  checkState();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("Fan_PWM ready"), true, 3);
}

void Fan_PWM::report(bool IncludeUnits)
{
  if (IncludeUnits) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, toText(getSpeed()));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    Common::report();
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("State:"));
    strcat(LongMessage, getSpeedText(true));
    logToSerials(&LongMessage, true, 1);
  }
}

void Fan_PWM::refresh_Minute()
{
  Common::refresh_Minute();
  checkState();
}

void Fan_PWM::checkState()
{
  if (*State) // Device should be ON
  {
    PWMDimmer->setState(ON);
  }
  else // Device should be OFF
  {
    PWMDimmer->setState(OFF);
  }
}

void Fan_PWM::turnOn()
{
  *State = true;
  checkState();
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("ON"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

void Fan_PWM::turnOff()
{
  *State = false;
  checkState();
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("OFF"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOffSound();
}

void Fan_PWM::setSpeed(uint8_t NewSpeed)
{
  *Speed = NewSpeed;
  PWMDimmer->setPower(map(NewSpeed, *MinSpeed, 100, 0, 100));
  appendName(true);
  strcat_P(ShortMessage, (PGM_P)F("speed updated"));
  Parent->addToLog(ShortMessage);
  Parent->getSoundObject()->playOnSound();
}

uint8_t Fan_PWM::getSpeed(bool CurrentSpeed)
{
  if (CurrentSpeed && !*State)
  {
    return 0;
  }
  else
  {
    return *Speed;
  }
}

char *Fan_PWM::getSpeedText(bool CurrentSpeed, bool IncludeUnits)
{
  if (CurrentSpeed && !*State)
  {
    if (IncludeUnits)
    {
      return toText_onOff(*State);
    }
    else
    {
      return toText('0');
    }
  }
  else
  {
    if (IncludeUnits)
    {
      return toText_percentage(*Speed);
    }
    else
    {
      return toText(*Speed);
    }
  }
}