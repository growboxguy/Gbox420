#include "PressureSensor_Web.h"

PressureSensor_Web::PressureSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PressureSensorSettings *DefaultSettings) : Common(Name), PressureSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToCommandQueue(this);
  Parent->addToWebsiteQueue_Field(this);
}

void PressureSensor_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/S", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Offset")), toText_floatDecimals(*Offset));
    WebServer.setArgString(getComponentName(F("Ratio")), toText_floatDecimals(*Ratio));
  }
}

void PressureSensor_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMine(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Offset")) == 0)
    {
      readOffset();
    }
  }
}

void PressureSensor_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMine(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Offset")) == 0)
    {
      setOffset(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Ratio")) == 0)
    {
      setRatio(WebServer.getArgFloat());
    }
  }
}