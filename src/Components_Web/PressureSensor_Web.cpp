#include "PressureSensor_Web.h"

PressureSensor_Web::PressureSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PressureSensorSettings *DefaultSettings) : Common(Name), PressureSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToCommandQueue(this);
}

void PressureSensor_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("Offset"),true), toText_floatDecimals(*Offset));
  WebServer.setArgString(getName(F("Ratio"),true), toText_floatDecimals(*Ratio));
}

bool PressureSensor_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Offset")) == 0)
    {
      readOffset();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Offset")) == 0)
    {
      setOffset(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Ratio")) == 0)
    {
      setRatio(WebServer.getArgFloat());
    }
    return true;
  }
}