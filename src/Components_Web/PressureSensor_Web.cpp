#include "PressureSensor_Web.h"

PressureSensor_Web::PressureSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PressureSensorSettings *DefaultSettings) : Common(Name), PressureSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToCommandQueue(this);
}

void PressureSensor_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("Offset")), toText_floatDecimals(*Offset));
  WebServer.setArgString(getName(F("Ratio")), toText_floatDecimals(*Ratio));
}

void PressureSensor_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return;
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
  }
}