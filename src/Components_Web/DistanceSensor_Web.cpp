#include "DistanceSensor_Web.h"

DistanceSensor_Web::DistanceSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::DistanceSensorSettings *DefaultSettings) : Common(Name), DistanceSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
}

void DistanceSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{                                                                   ///< When the website is refreshing
  WebServer.setArgString(getName(F("D"),true), getDistanceText(true)); ///< Distance reading
}

void commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    return true;
    //if (strcmp_P(ShortMessage, (PGM_P)F("Emp")) == 0){setEmptyDistance();}    ///< Read the distance when the reservoir is empty
  }
}