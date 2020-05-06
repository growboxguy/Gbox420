#include "Aeroponics_Tank_Web.h"

Aeroponics_Tank_Web::Aeroponics_Tank_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Common(Name), Aeroponics_Tank(Name, Parent, DefaultSettings, TankSpecificSettings, FeedbackPressureSensor, Pump), Common_Web(Name)
{ ///constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);     ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
  Parent->addToWebsiteQueue_Load(this);    ///Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->addToWebsiteQueue_Refresh(this); ///Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Button(this);  ///Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->addToWebsiteQueue_Field(this);   ///Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
}

void Aeroponics_Tank_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgFloat(getComponentName(F("PresMin")), *MinPressure);
    WebServer.setArgFloat(getComponentName(F("PresMax")), *MaxPressure);
    WebServer.setArgInt(getComponentName(F("Timeout")), Pump->getPumpTimeOut());
    WebServer.setArgInt(getComponentName(F("Priming")), Pump->getPrimingTime());
    WebServer.setArgInt(getComponentName(F("Int")), *Interval);
    WebServer.setArgInt(getComponentName(F("Dur")), *Duration);
  }
}

void Aeroponics_Tank_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Pres")), FeedbackPressureSensor->getPressureText(true, false));
    WebServer.setArgString(getComponentName(F("Spray")), sprayStateToText());
    WebServer.setArgString(getComponentName(F("Pump")), Pump->getStateText());
    WebServer.setArgString(getComponentName(F("LastSP")), getLastSprayPressureText(false));
  }
}

void Aeroponics_Tank_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Refill")) == 0)
    {
      refillTank();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      startPump(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      stopPump(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0)
    {
      setPumpDisable();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      mixReservoir();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEn")) == 0)
    {
      setSprayOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0)
    {
      setSprayOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      sprayNow(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      sprayOff();
    }
  }
}

void Aeroponics_Tank_Web::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("PresMin")) == 0)
    {
      setMinPressure(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresMax")) == 0)
    {
      setMaxPressure(WebServer.getArgFloat());
    } 
    else if (strcmp_P(ShortMessage, (PGM_P)F("Timeout")) == 0)
    {
      setPumpTimeout(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Priming")) == 0)
    {
      Pump -> setPrimingTime(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Dur")) == 0)
    {
      setSprayDuration(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Int")) == 0)
    {
      setSprayInterval(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Spray")) == 0)
    {
      setSprayOnOff(WebServer.getArgBoolean());
    }
  }
}