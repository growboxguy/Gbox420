#include "WeightSensor_Web.h"

WeightSensor_Web::WeightSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WeightSensorSettings *DefaultSettings) : Common(Name), WeightSensor(Name, Parent, DefaultSettings), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void WeightSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  WebServer.setArgString(getName(F("W"), true), getWeightText(false, true));
}

bool WeightSensor_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
      tareRequest();
    /*     else if (strcmp_P(ShortMessage, (PGM_P)F("Calibrate")) == 0)
      triggerCalibration(WebServer.getArgInt());
    else if (strcmp_P(ShortMessage, (PGM_P)F("Scale")) == 0)
      setScale(WebServer.getArgFloat()); */
    return true;
  }
}