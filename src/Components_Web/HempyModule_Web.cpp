#include "HempyModule_Web.h"

HempyModule_Web::HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent) : Common(Name), Common_Web(Name)
{ ///Constructor
  this->Parent = Parent;
  this->Name = Name;
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_FiveSec(this);     ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  Parent->addToRefreshQueue_Minute(this);      ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  //addToRefreshQueue_QuarterHour(this); ///Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
  Parent->addToWebsiteQueue_Load(this);        ///Subscribing to the Website load event
  Parent->addToWebsiteQueue_Refresh(this);     ///Subscribing to the Website refresh event
  Parent->addToWebsiteQueue_Field(this);       ///Subscribing to the Website field submit event
  Parent->addToWebsiteQueue_Button(this);      ///Subscribing to the Website button press event
  logToSerials(F("HempyModule_Web object created"), true, 1);
}

void HempyModule_Web::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  //strcat(LongMessage, getTempText(true)); ///Shows the average reading
  strcat_P(LongMessage, (PGM_P)F(" ; Pump:"));
  //strcat(LongMessage, getHumidityText(true));
  logToSerials(&LongMessage, true, 1);
}

void HempyModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/S",2) == 0)
  {
    /*
    WebServer.setArgInt(getComponentName(F("Debug")), *Debug);
    WebServer.setArgInt(getComponentName(F("Metric")), *Metric);
    WebServer.setArgBoolean(getComponentName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getComponentName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getComponentName(F("Relay")), ModuleSettings -> PushingBoxLogRelayID);
    */
  }
}

void HempyModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  /*
  WebServer.setArgString(F("Time"), getFormattedTime(false));
  WebServer.setArgJson(F("Log"), eventLogToJSON()); ///Last events that happened in JSON format
  */
}

void HempyModule_Web::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    /*
    if (strcmp_P(ShortMessage, (PGM_P)F("SheetsRep")) == 0)
    {
      ReportToGoogleSheetsRequested = true;  ///just signal that a report should be sent, do not actually run it: Takes too long from an interrupt
      addToLog(F("Reporting to Sheets"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SerialRep")) == 0)
    {
      ConsoleReportRequested = true;
      addToLog(F("Reporting to Serial"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refresh")) == 0) ///Website signals to refresh all sensor readings
    {        
      RefreshAllRequested = true;
      addToLog(F("Refresh triggered"), false);
    } 
    */
  }
}

void HempyModule_Web::websiteEvent_Field(char *Field)
{ ///When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    /*
    if (strcmp_P(ShortMessage, (PGM_P)F("Debug")) == 0){setDebugOnOff(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("Metric")) == 0){setMetric(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sheets")) == 0){setSheetsReportingOnOff(WebServer.getArgBoolean());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("SheetsF")) == 0){setSheetsReportingFrequency(WebServer.getArgInt());}
    else if (strcmp_P(ShortMessage, (PGM_P)F("PushingBoxLogRelayID")) == 0){setPushingBoxLogRelayID(WebServer.getArgString());}
    */
  }
}

void HempyModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  //syncModule();
}

void HempyModule_Web::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  
}


void HempyModule_Web::syncModule(){  
  
  
}

void HempyModule_Web::updateMessage() {        // so you can see that new data is being sent
    //FakeCommand.TurnOnPump1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exlusive!
    //FakeCommand.TurnOnPump2 = random(0,2);
    FakeCommand.StopWeightBucket1 = random(400, 500) / 100.0;
    FakeCommand.StopWeightBucket2 = random(400, 500) / 100.0;
}