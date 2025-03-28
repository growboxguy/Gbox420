#include "LightSensor_Web.h"

LightSensor_Web::LightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource) : Common(Name), Common_Web(Name), LightSensor(Name, Parent, DefaultSettings, LightSource), Parent(Parent)
{
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void LightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("D"), true), getDarkText(true));
  WebServer.setArgString(getName(F("R"), true), getReadingText(true));
}

bool LightSensor_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("C")) == 0)
    {
      triggerCalibration();
    }
    return true;
  }
}