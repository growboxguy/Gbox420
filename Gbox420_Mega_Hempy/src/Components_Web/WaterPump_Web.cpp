#include "WaterPump_Web.h"

WaterPump_Web::WaterPump_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WaterPumpSettings *DefaultSettings) : Common(Name), Common_Web(Name), WaterPump(Name, Parent, DefaultSettings), Parent(Parent)
{
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void WaterPump_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgInt(getName(F("PS"), true), getSpeed());
  WebServer.setArgInt(getName(F("T"), true), getTimeOut());
}

void WaterPump_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("S"), true), getStateText(true));
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/

bool WaterPump_Web::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("ON")) == 0)
    {
      startPumpRequest();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("OF")) == 0)
    {
      stopPumpRequest();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("D")) == 0)
    {
      disablePumpRequest();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PS")) == 0)
    {
      setSpeed(toInt(Data));
      Parent->addToLog(getName(F("speed updated")));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
    {
      setTimeOut(toInt(Data));
      Parent->addToLog(getName(F("timeout updated")));
    }
    return true;
  }
}