#include "Fan_PWM.h"
#include "Sound.h"

///< PWM control an AC device using a Robodyn AC Light Dimming Module with Zero-cross detection

Fan_PWM::Fan_PWM(const __FlashStringHelper *Name, Module *Parent, Settings::Fan_PWMSettings *DefaultSettings)
    : Common(Name),
      Parent(Parent),
      MinSpeed(DefaultSettings->MinSpeed),
      State(DefaultSettings->State),
      Speed(DefaultSettings->Speed),
      ZeroCrossingPin(DefaultSettings->ZeroCrossingPin),
      PWMPin(DefaultSettings->PWMPin)
{
  PWMDimmer = new dimmerLamp(PWMPin);
  PWMDimmer->begin(NORMAL_MODE, OFF);
  PWMDimmer->setPower(Speed);
  checkState();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("Fan_PWM ready"), true, 3);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void Fan_PWM::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, getSpeedText(true, FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void Fan_PWM::refresh_Minute()
{
  Common::refresh_Minute();
  checkState();
}

void Fan_PWM::checkState()
{
  if (State) // Device should be ON
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
  State = true;
  checkState();
  Parent->addToLog(getName(getSpeedText(true)));
  Parent->getSoundObject()->playOnSound();
}

void Fan_PWM::turnOff()
{
  State = false;
  checkState();
  Parent->addToLog(getName(getSpeedText(true)));
  Parent->getSoundObject()->playOffSound();
}

void Fan_PWM::setSpeed(uint8_t NewSpeed)
{
  *Speed = NewSpeed;
  PWMDimmer->setPower(map(NewSpeed, *MinSpeed, 100, 0, 100));
  Parent->addToLog(getName(getSpeedText(true)));
  Parent->getSoundObject()->playOnSound();
}

uint8_t Fan_PWM::getSpeed(bool CurrentSpeed)
{
  if (CurrentSpeed && !State)
  {
    return 0;
  }
  else
  {
    return *Speed;
  }
}

char *Fan_PWM::getSpeedText(bool CurrentSpeed, bool FriendlyFormat)
{
  if (CurrentSpeed && !State)
  {
    if (FriendlyFormat)
    {
      return toText_onOff(State);
    }
    else
    {
      return toText('0');
    }
  }
  else
  {
    if (FriendlyFormat)
    {
      return toText_percentage(*Speed);
    }
    else
    {
      return toText(*Speed);
    }
  }
}