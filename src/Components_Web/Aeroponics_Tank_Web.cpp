#include "Aeroponics_Tank_Web.h"

Aeroponics_Tank_Web::Aeroponics_Tank_Web(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_TankSpecific *TankSpecificSettings, PressureSensor *FeedbackPressureSensor) : Aeroponics(Name, GBox, DefaultSettings, FeedbackPressureSensor)
{ //constructor
  Parent->AddToWebsiteQueue_Load(this);    //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->AddToWebsiteQueue_Field(this);   //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
}

void Aeroponics_Tank_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgFloat(getWebsiteComponentName(F("PresLow")), *PressureLow);
    WebServer.setArgFloat(getWebsiteComponentName(F("PresHigh")), *PressureHigh);
    WebServer.setArgInt(getWebsiteComponentName(F("PumpTimeout")), *PumpTimeout);
    WebServer.setArgInt(getWebsiteComponentName(F("PrimingTime")), *PrimingTime);
    WebServer.setArgInt(getWebsiteComponentName(F("Interval")), *Interval);
    WebServer.setArgInt(getWebsiteComponentName(F("Duration")), *Duration);
  }
}

void Aeroponics_Tank_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Pressure")), FeedbackPressureSensor->getPressureText(true, false));
    WebServer.setArgString(getWebsiteComponentName(F("SprayEnabled")), sprayStateToText());
    WebServer.setArgString(getWebsiteComponentName(F("PumpState")), pumpStateToText());
    WebServer.setArgString(getWebsiteComponentName(F("BypassState")), onOffToText(BypassSolenoidOn));
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
      setPumpOn(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      setPumpOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDisable")) == 0)
    {
      PumpDisable();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      Mix();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEnable")) == 0)
    {
      setSprayOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDisable")) == 0)
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
    if (strcmp_P(ShortMessage, (PGM_P)F("PresLow")) == 0)
    {
      setPressureLow(WebServer.getArgFloat());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresHigh")) == 0)
    {
      setPressureHigh(WebServer.getArgFloat());
    } 
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpTimeout")) == 0)
    {
      setPumpTimeout(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PrimingTime")) == 0)
    {
      setPrimingTime(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Duration")) == 0)
    {
      setDuration(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Interval")) == 0)
    {
      setInterval(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEnabled")) == 0)
    {
      setSprayOnOff(WebServer.getArgBoolean());
    }
  }
}