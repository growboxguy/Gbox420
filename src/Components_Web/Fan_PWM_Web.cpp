#include "Fan_PWM_Web.h"

Fan_PWM_Web::Fan_PWM_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::Fan_PWMSettings *DefaultSettings) : Common(Name), Fan_PWM(Name, Parent, DefaultSettings), Common_Web(Name), Parent(Parent)
{
  Parent->addToCommandQueue(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToCommandQueue(this);
}

void Fan_PWM_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgInt(getName(F("S")), *Speed);
}

void Fan_PWM_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("C")), getSpeedText(true, true));
}

void Fan_PWM_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Of")) == 0)
    {
      turnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("On")) == 0)
    {
      turnOn();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("S")) == 0)
    {
      setSpeed(WebServer.getArgInt());
    }
  }
}