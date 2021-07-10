#include "WasteReservoir_Web.h"

WasteReservoir_Web::WasteReservoir_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::WasteReservoirSettings *DefaultSettings, WeightSensor *WasteWeightSensor) : Common(Name), WasteReservoir(Name, Parent, DefaultSettings, WasteWeightSensor), Common_Web(Name)
{
  this->Parent = Parent;
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
}

void WasteReservoir_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("L"), true), getWasteLimitText());
}

void WasteReservoir_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("S"), true), getStateText(true));
}

bool WasteReservoir_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("L")) == 0) ///TODO: Handle waste limit from multiple buckets to a single waste reservoir
    {
      setWasteLimit(toFloat(Data));
      Parent->addToLog(F("Waste limit updated"), false);
    }
    return true;
  }
}