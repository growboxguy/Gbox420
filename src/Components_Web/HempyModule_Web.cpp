#include "HempyModule_Web.h"

HempyModule_Web::HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
{ ///Constructor
  this->Parent = Parent;
  this->Name = Name;
  memcpy_P(this->WirelessChannel,(PGM_P)Name,sizeof(this->WirelessChannel));
  Command.StartWeightBucket1= DefaultSettings->StartWeightBucket1;
  Command.StopWeightBucket1= DefaultSettings->StopWeightBucket1;
  Command.TimeOutPump1= DefaultSettings->TimeOutPump1;
  Command.StartWeightBucket2= DefaultSettings->StartWeightBucket2;
  Command.StopWeightBucket2= DefaultSettings->StopWeightBucket2;
  Command.TimeOutPump2= DefaultSettings->TimeOutPump2;

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
  
  WebServer.setArgString(getComponentName(F("B1Weight")), toText(Response.WeightBucket1));
  WebServer.setArgString(getComponentName(F("B2Weight")), toText(Response.WeightBucket2));
  WebServer.setArgString(getComponentName(F("B1Pump")),  pumpStateToText(Response.EnabledPump1,Response.OnPump1));
  WebServer.setArgString(getComponentName(F("B2Pump")), pumpStateToText(Response.EnabledPump2,Response.OnPump2));
  
          /*
          Serial.print(F(" ; DHT: "));
          Serial.print(Response -> Temp);
          Serial.print(F(", "));
          Serial.print(Response -> Humidity);
          Serial.println(); 
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
    if (strcmp_P(ShortMessage, (PGM_P)F("B1Wat")) == 0)
    {
      TriggerWateringBucket1();
      Parent -> addToLog(F("Watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Wat")) == 0)
    {
      TriggerWateringBucket2();
      Parent -> addToLog(F("Watering HempyBucket 2"), false);
    }     
  }
}

void HempyModule_Web::TriggerWateringBucket1()
{
  Command.TurnOnPump1 = true;
}

void HempyModule_Web::TriggerWateringBucket2()
{
  Command.TurnOnPump2 = true;
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
  
  syncModule(WirelessChannel,&Command,&Response);
}

void HempyModule_Web::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  
}

void HempyModule_Web::syncModule( const byte WirelessChannel[], hempyCommand *Command, hempyResponse *Response){
  updateCommand();
  logToSerials(F("Sending sync command..."),false,3);
  Parent -> Wireless -> openWritingPipe(WirelessChannel);
  if (Parent -> Wireless -> write(Command, sizeof(*Command) )) {
      if ( Parent -> Wireless -> isAckPayloadAvailable() ) {
          Parent -> Wireless -> read(Response, sizeof(*Response));
           logToSerials(F("Acknowledgement received ["),false,2);            
          Serial.print(sizeof(*Response)); /// \todo Use LogToSerial
          logToSerials(F("bytes]"),true,1);

          ///Turn of command flags after receiving it was confirmed
          Command -> DisablePump1 = false;
          Command -> TurnOnPump1 = false;
          Command -> TurnOffPump1 = false;
          Command -> DisablePump2 = false;
          Command -> TurnOnPump2 = false;
          Command -> TurnOffPump2 = false;
      }
      else {
          Serial.println(F(" Acknowledgement received without any data."));
      }        
  }
  else {
      Serial.println(F(" No response."));
  }
  }


void HempyModule_Web::updateCommand() {        // so you can see that new data is being sent
    //Command.TurnOnPump1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exclusive!
    //Command.TurnOnPump2 = random(0,2);
    Command.Time = now();
}