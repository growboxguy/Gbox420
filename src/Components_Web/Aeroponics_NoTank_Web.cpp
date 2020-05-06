#include "Aeroponics_NoTank_Web.h"

Aeroponics_NoTank_Web::Aeroponics_NoTank_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) :  Common(Name), Aeroponics_NoTank(Name, Parent, DefaultSettings, FeedbackPressureSensor, Pump), Common_Web(Name)
{
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);     ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
  Parent->addToWebsiteQueue_Load(this);    ///Subscribing to the Website load event: Calls the websiteEvent_Load() method
  Parent->addToWebsiteQueue_Refresh(this); ///Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  Parent->addToWebsiteQueue_Button(this);  ///Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  Parent->addToWebsiteQueue_Field(this);   ///Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
}

void Aeroponics_NoTank_Web::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgInt(getComponentName(F("Timeout")), Pump->getPumpTimeOut());
    WebServer.setArgInt(getComponentName(F("Priming")), Pump -> getPrimingTime());
    WebServer.setArgInt(getComponentName(F("Int")), *Interval);
    WebServer.setArgInt(getComponentName(F("Dur")), *Duration);
  }
}

void Aeroponics_NoTank_Web::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strncmp(url, "/G",2) == 0)
  {    
    WebServer.setArgString(getComponentName(F("Pres")), FeedbackPressureSensor->getPressureText(true, false));
    WebServer.setArgString(getComponentName(F("Spray")), sprayStateToText());
    WebServer.setArgString(getComponentName(F("Pump")), Pump->getStateText());
    WebServer.setArgString(getComponentName(F("LastSP")), toText_pressure(LastSprayPressure));
  }
}

void Aeroponics_NoTank_Web::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("BypassOn")) == 0){Pump-> turnBypassOn();}
    else if (strcmp_P(ShortMessage, (PGM_P)F("BypassOff")) == 0){Pump-> turnBypassOff();}    
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0){Pump->startPump(true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0){Pump->stopPump(true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0){Pump->disablePump();}
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0){mixReservoir();}
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEn")) == 0){setSprayOnOff(true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0){setSprayOnOff(false);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0){sprayNow(true);}
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0){sprayOff(true);}
  }
}

void Aeroponics_NoTank_Web::websiteEvent_Field(__attribute__((unused)) char *Field)
{ ///When a field is submitted using the Set button
  if (strcmp_P(ShortMessage, (PGM_P)F("Timeout")) == 0){Pump->setPumpTimeOut(WebServer.getArgInt());}
  else if (strcmp_P(ShortMessage, (PGM_P)F("Priming")) == 0){Pump->setPrimingTime(WebServer.getArgInt());}
  else if (strcmp_P(ShortMessage, (PGM_P)F("Dur")) == 0){setSprayDuration(WebServer.getArgInt());}
  else if (strcmp_P(ShortMessage, (PGM_P)F("Int")) == 0){setSprayInterval(WebServer.getArgInt());}
  else if (strcmp_P(ShortMessage, (PGM_P)F("Spray")) == 0){setSprayOnOff(WebServer.getArgBoolean());}
}