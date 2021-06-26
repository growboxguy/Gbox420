#include "WeightSensor_Web.h"

WeightSensor_Web::WeightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WeightSensorSettings *DefaultSettings) : Common(Name), WeightSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void WeightSensor_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  /* if (strncmp(url, "/S", 2) == 0) ///< When the /Settings.html page is refreshed - !Matches for any page that starts with /S
  {
    WebServer.setArgString(getComponentName(F("O")), toText(*Offset));
    WebServer.setArgString(getComponentName(F("S")), toText(*Scale));
  } */
}

void WeightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgFloat(getComponentName(F("W")), getWeight());
}

bool WeightSensor_Web::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
      triggerTare();
/*     else if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
      triggerCalibration(WebServer.getArgInt());
    else if (strcmp_P(ShortMessage, (PGM_P)F("Scale")) == 0)
      setScale(WebServer.getArgFloat()); */
    return true;
  }
}