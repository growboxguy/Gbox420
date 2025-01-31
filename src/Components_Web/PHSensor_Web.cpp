#include "PHSensor_Web.h"

PHSensor_Web::PHSensor_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::PHSensorSettings *DefaultSettings) : Common(Name), PHSensor(Name, Parent, DefaultSettings), Common_Web(Name), Parent(Parent)
{ 
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void PHSensor_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  ///< WebServer.setArgString(F("PHAlertLow"), toText(GBox -> Reservoir -> PHAlertLow));
  ///< WebServer.setArgString(F("PHAlertHigh"), toText(GBox -> ModuleSettings -> PHAlertHigh));
  WebServer.setArgString(getName(F("Slope"), true), toText_floatDecimals(Slope));
  WebServer.setArgString(getName(F("Intercept"), true), toText_floatDecimals(Intercept));
}

void PHSensor_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("PH"), true), getPHText(false));
}

bool PHSensor_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{ ///< When a button is pressed on the website
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("ReadRaw")) == 0)
    {
      updatePH(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Slope")) == 0)
    {
      setSlope(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Intercept")) == 0)
    {
      setIntercept(WebServer.getArgFloat());
    }
    return true;
  }
}