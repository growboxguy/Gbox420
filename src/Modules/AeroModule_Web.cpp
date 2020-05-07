#include "AeroModule_Web.h"

AeroModule_Web::AeroModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::AeroModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
{ ///Constructor
  this->Parent = Parent;
  this->DefaultSettings = DefaultSettings;
  updateCommand();
  memcpy_P(this->WirelessChannel,(PGM_P)Name,sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);          
  Parent->addToRefreshQueue_Sec(this);     
  Parent->addToRefreshQueue_FiveSec(this);     
  Parent->addToRefreshQueue_Minute(this);    
  //addToRefreshQueue_QuarterHour(this); 
  Parent->addToWebsiteQueue_Load(this);        
  Parent->addToWebsiteQueue_Refresh(this);     
  Parent->addToWebsiteQueue_Field(this);       
  Parent->addToWebsiteQueue_Button(this);      
  logToSerials(F("AeroModule_Web object created"), true, 1);
}

void AeroModule_Web::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Pressure:"));
  strcat(LongMessage, toText(Response.Pressure));
  if(Response.PressureTankPresent)
  {
    strcat_P(LongMessage, (PGM_P)F(" ["));
    strcat(LongMessage, toText(Command.MinPressure));
    strcat_P(LongMessage, (PGM_P)F("/"));
    strcat(LongMessage, toText(Command.MaxPressure));
    strcat_P(LongMessage, (PGM_P)F("]"));
  }
  else
  {
    strcat_P(LongMessage, (PGM_P)F(" ; LastSprayPressure:"));
    strcat(LongMessage, toText(Response.LastSprayPressure));
  }
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, toText_yesNo(Response.SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; Interval:"));
  strcat(LongMessage, toText(Command.SprayInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText(Command.SprayDuration));  
  logToSerials(&LongMessage, true, 1);
}

void AeroModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G",2) == 0)
  { 
    WebServer.setArgBoolean(getComponentName(F("Tank")), Response.PressureTankPresent);
    WebServer.setArgInt(getComponentName(F("Timeout")), Command.PumpTimeOut);
    WebServer.setArgInt(getComponentName(F("Priming")), Command.PumpPrimingTime);
    WebServer.setArgInt(getComponentName(F("Int")), Command.SprayInterval);
    WebServer.setArgInt(getComponentName(F("Dur")), Command.SprayDuration);
    WebServer.setArgFloat(getComponentName(F("PresMax")), Command.MaxPressure);
    WebServer.setArgFloat(getComponentName(F("PresMin")), Command.MinPressure);      
  }
}

void AeroModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Spray")), toText_enabledDisabled(Response.SprayEnabled));
    WebServer.setArgString(getComponentName(F("Pump")), toText_pumpState(Response.State));
    WebServer.setArgString(getComponentName(F("Pres")), toText_pressure(Response.Pressure));
    WebServer.setArgString(getComponentName(F("LastSP")), toText_pressure(Response.LastSprayPressure));    
  }
}

void AeroModule_Web::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SprayEn")) == 0)
    {
      Command.SprayEnabled = true;
      Parent -> addToLog(F("Spray enabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0)
    {
      Command.SprayDisabled = true;
      Parent -> addToLog(F("Spray disabled"), false);
    } 
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      Command.SprayNow = true;
      Parent -> addToLog(F("Starting spray"));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      Command.SprayOff = true;
      Parent -> addToLog(F("Stop spray"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      Command.PumpOn = true;
      Parent -> addToLog(F("Aero pump ON"), false);
    } 
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      Command.PumpOff = true;
      Parent -> addToLog(F("Aero pump OFF"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0)
    {
      Command.PumpDisable = true;
      Parent -> addToLog(F("Aero pump disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      Command.MixReservoir = true;
      Parent -> addToLog(F("Mixing reservoir"), false);
    } 
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refill")) == 0)
    {
      if(Response.PressureTankPresent)
      {
        Command.RefillPressureTank = true;
        Parent -> addToLog(F("Refilling pressure tank"), false);
      }
      else{
        Parent -> addToLog(F("Pressure tank not available"), false);
      }
    }     
    SyncRequested = true;  
  }
}


void AeroModule_Web::websiteEvent_Field(char *Field)
{ ///When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Int")) == 0)
    {
      DefaultSettings->Interval = WebServer.getArgInt();  
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Dur")) == 0)
    {
      DefaultSettings->Duration = WebServer.getArgInt();
      Parent -> addToLog(F("Spray timer updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Timeout")) == 0)
    {
      DefaultSettings->PumpTimeOut = WebServer.getArgInt();
      Parent -> addToLog(F("Pump timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Priming")) == 0)
    {
      DefaultSettings->PrimingTime = WebServer.getArgInt();
      Parent -> addToLog(F("Priming time updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresMin")) == 0)
    {
      DefaultSettings->MinPressure = WebServer.getArgFloat();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresMax")) == 0)
    {
      DefaultSettings->MaxPressure = WebServer.getArgFloat();
      Parent -> addToLog(F("Pressure limits updated"), false);
    }
    SyncRequested = true;
  }
}

void AeroModule_Web::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();  
  if(SyncRequested) {
    SyncRequested = false;
    syncModule(WirelessChannel,&Command,&Response);    
  }
}

void AeroModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();  
  syncModule(WirelessChannel,&Command,&Response);
}

void AeroModule_Web::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();  
}

void AeroModule_Web::syncModule( const byte WirelessChannel[], aeroCommand *Command, aeroResponse *Response){
  updateCommand();
  logToSerials(F("Sending sync command..."),false,3);
  Parent -> Wireless -> openWritingPipe(WirelessChannel);
  if (Parent -> Wireless -> write(Command, sizeof(*Command) )) {
      if ( Parent -> Wireless -> isAckPayloadAvailable() ) {
          Parent -> Wireless -> read(Response, sizeof(*Response));
           logToSerials(F("Acknowledgement received ["),false,2);            
          Serial.print(sizeof(*Response)); /// \todo Use LogToSerial
          logToSerials(F("bytes]"),true,1);

          if(*Debug){
          logToSerials(Response -> PressureTankPresent,false,3);
          logToSerials(F(","),false,1);
          logToSerials(Response -> SprayEnabled,false,1);;
          logToSerials(F(","),false,1);
          logToSerials(Response -> State,false,1);;
          logToSerials(F(","),false,1);
          logToSerials(Response -> Pressure,false,1);
          logToSerials(F(";"),false,1);
          logToSerials(Response -> LastSprayPressure,true,1); 
          }            

          ///Turn off command flags          
          if(Command -> SprayEnabled || Command -> SprayDisabled || Command -> SprayNow || Command -> SprayOff || Command -> PumpOn || Command -> PumpOff || Command -> PumpDisable || Command -> MixReservoir || Command -> RefillPressureTank )
          {
            SyncRequested = true;  ///Force a second packet to actualize the response
            Command -> SprayEnabled = false;
            Command -> SprayDisabled = false;
            Command -> SprayNow = false;
            Command -> SprayOff = false;
            Command -> PumpOn = false;
            Command -> PumpOff = false;
            Command -> PumpDisable = false;
            Command -> MixReservoir = false;
            Command -> RefillPressureTank = false;
          }
      }
      else {
          Serial.println(F(" Acknowledgement received without any data."));
      }        
  }
  else {
      Serial.println(F(" No response."));
  }
  }


void AeroModule_Web::updateCommand() {        // so you can see that new data is being sent
    //Command.TurnOnPump_B1 = random(0,2);  //Generate random bool: 0 or 1. The max limit is exclusive!
    //Command.TurnOnPump_B2 = random(0,2);    
    Command.Time = now();
    Command.Debug = *Debug;
    Command.Metric = *Metric;
    
    Command.SprayInterval= DefaultSettings->Interval;
    Command.SprayDuration= DefaultSettings->Duration;
    Command.PumpTimeOut= DefaultSettings->PumpTimeOut;
    Command.PumpPrimingTime= DefaultSettings->PrimingTime;
    Command.MinPressure= DefaultSettings->MinPressure;
    Command.MaxPressure= DefaultSettings->MaxPressure;
}