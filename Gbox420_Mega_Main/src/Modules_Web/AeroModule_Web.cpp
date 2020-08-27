#include "AeroModule_Web.h"

struct AeroModuleCommand AeroModule1CommandToSend = {AeroMessages::AeroModuleCommand1};  ///Command to send will be stored here
struct AeroCommand Aero1CommandToSend = {AeroMessages::AeroCommand1}; ///Command to send will be stored here
struct AeroCommonTemplate AeroGetNextToSend = {AeroMessages::AeroGetNext};            //< Special command to fetch the next Response from the Receiver
struct AeroModuleResponse * AeroModule1ReceivedResponse = malloc(sizeof(struct HempyModuleResponse));  /// Response will be stored here
struct AeroResponse * Aero1ReceivedResponse = malloc(sizeof(struct AeroResponse));  /// Response will be stored here

AeroModule_Web::AeroModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
{ ///Constructor
  this->Parent = Parent;
  this->DefaultSettings = DefaultSettings;
  updateCommands();
  memcpy_P(this->WirelessChannel, (PGM_P)Name, sizeof(this->WirelessChannel));
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
  strcat(LongMessage, toText_pressure(Aero1ReceivedResponse->Pressure));
  if (Aero1ReceivedResponse->PressureTankPresent)
  {
    strcat_P(LongMessage, (PGM_P)F(" ["));
    strcat(LongMessage, toText(Aero1CommandToSend.MinPressure));
    strcat_P(LongMessage, (PGM_P)F("/"));
    strcat(LongMessage, toText(Aero1CommandToSend.MaxPressure));
    strcat_P(LongMessage, (PGM_P)F("]"));
  }
  strcat_P(LongMessage, (PGM_P)F(" ; LastSprayPressure:"));
  strcat(LongMessage, toText_pressure(Aero1ReceivedResponse->LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, toText_yesNo(Aero1ReceivedResponse->SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; Interval:"));
  strcat(LongMessage, toText_minute(Aero1CommandToSend.SprayInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText_second(Aero1CommandToSend.SprayDuration));
  logToSerials(&LongMessage, true, 1);
}

void AeroModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Status\":\""));
  strcat(LongMessage, toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"Pressure\":\""));
  strcat(LongMessage, toText(Aero1ReceivedResponse->Pressure));
  if (Aero1ReceivedResponse->PressureTankPresent)
  {
    strcat_P(LongMessage, (PGM_P)F("\",\"Min\":\""));
    strcat(LongMessage, toText(Aero1CommandToSend.MinPressure));
    strcat_P(LongMessage, (PGM_P)F("\",\"Max\":\""));
    strcat(LongMessage, toText(Aero1CommandToSend.MaxPressure));
  }
  strcat_P(LongMessage, (PGM_P)F("\",\"LastSpray\":\""));
  strcat(LongMessage, toText(Aero1ReceivedResponse->LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpState\":\""));
  strcat(LongMessage, toText(Aero1ReceivedResponse->State));
  strcat_P(LongMessage, (PGM_P)F("\",\"SprayEnabled\":\""));
  strcat(LongMessage, toText(Aero1CommandToSend.SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F("\",\"Interval\":\""));
  strcat(LongMessage, toText(Aero1CommandToSend.SprayInterval));
  strcat_P(LongMessage, (PGM_P)F("\",\"Duration\":\""));
  strcat(LongMessage, toText(Aero1CommandToSend.SprayDuration));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void AeroModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("Tank")), Aero1ReceivedResponse->PressureTankPresent);
    WebServer.setArgInt(getComponentName(F("Timeout")), Aero1CommandToSend.PumpTimeOut);
    WebServer.setArgInt(getComponentName(F("Priming")), Aero1CommandToSend.PumpPrimingTime);
    WebServer.setArgInt(getComponentName(F("Int")), Aero1CommandToSend.SprayInterval);
    WebServer.setArgInt(getComponentName(F("Dur")), Aero1CommandToSend.SprayDuration);
    WebServer.setArgFloat(getComponentName(F("PresMax")), Aero1CommandToSend.MaxPressure);
    WebServer.setArgFloat(getComponentName(F("PresMin")), Aero1CommandToSend.MinPressure);
  }
}

void AeroModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("Spray")), toText_enabledDisabled(Aero1ReceivedResponse->SprayEnabled));
    WebServer.setArgString(getComponentName(F("Pump")), toText_pumpState(Aero1ReceivedResponse->State));
    WebServer.setArgString(getComponentName(F("Pres")), toText_pressure(Aero1ReceivedResponse->Pressure));
    WebServer.setArgString(getComponentName(F("LastSP")), toText_pressure(Aero1ReceivedResponse->LastSprayPressure));
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
      Aero1CommandToSend.SprayEnabled = true;
      Parent->addToLog(F("Spray enabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0)
    {
      Aero1CommandToSend.SprayDisabled = true;
      Parent->addToLog(F("Spray disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      Aero1CommandToSend.SprayNow = true;
      Parent->addToLog(F("Starting spray"));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      Aero1CommandToSend.SprayOff = true;
      Parent->addToLog(F("Stop spray"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      Aero1CommandToSend.PumpOn = true;
      Parent->addToLog(F("Aero pump ON"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      Aero1CommandToSend.PumpOff = true;
      Parent->addToLog(F("Aero pump OFF"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0)
    {
      Aero1CommandToSend.PumpDisable = true;
      Parent->addToLog(F("Aero pump disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      Aero1CommandToSend.MixReservoir = true;
      Parent->addToLog(F("Mixing reservoir"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refill")) == 0)
    {
      if (Aero1ReceivedResponse->PressureTankPresent)
      {
        Aero1CommandToSend.RefillPressureTank = true;
        Parent->addToLog(F("Refilling pressure tank"), false);
      }
      else
      {
        Parent->addToLog(F("Pressure tank not available"), false);
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
      Parent->addToLog(F("Spray timer updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Timeout")) == 0)
    {
      DefaultSettings->PumpTimeOut = WebServer.getArgInt();
      Parent->addToLog(F("Pump timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Priming")) == 0)
    {
      DefaultSettings->PrimingTime = WebServer.getArgInt();
      Parent->addToLog(F("Priming time updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresMin")) == 0)
    {
      DefaultSettings->MinPressure = WebServer.getArgFloat();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PresMax")) == 0)
    {
      DefaultSettings->MaxPressure = WebServer.getArgFloat();
      Parent->addToLog(F("Pressure limits updated"), false);
    }
    SyncRequested = true;
  }
}

void AeroModule_Web::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    sendMessages();
  }
}

void AeroModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  sendMessages();
}

void AeroModule_Web::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
}

void AeroModule_Web::sendMessages()
{
  updateCommands();
  sendCommand(&AeroModule1CommandToSend);                                                                                       //< Command - Response exchange
  sendCommand(&Aero1CommandToSend);                                                                                         //< Command - Response exchange
  while (sendCommand(&AeroGetNextToSend) < AeroMessages::AeroGetNext && millis() - LastResponseReceived < WirelessMessageTimeout) //< special Command, only exchange Response.
    ;
  if (Debug)
    logToSerials(F("Message exchange finished"), true, 3);
}

AeroMessages AeroModule_Web::sendCommand(void *CommandToSend)
{
  AeroMessages SequenceIDToSend = ((AeroCommonTemplate *)CommandToSend)->SequenceID;
  AeroMessages ReceivedSequenceID = NULL;
  if (Debug)
  {
    logToSerials(F("Sending SequenceID:"), false, 3);
    logToSerials(SequenceIDToSend, false, 1);
    logToSerials(F("-"), false, 1);
    logToSerials(toText_aeroSequenceID(SequenceIDToSend), false, 1);
    logToSerials(F("and waiting for Acknowledgment..."), true, 1);
  }
  Parent->Wireless->openWritingPipe(WirelessChannel);
  Parent->Wireless->flush_rx(); ///< Dump all previously received but unprocessed messages
  if (Parent->Wireless->write(CommandToSend, WirelessPayloadSize))
  {
    delay(50); //< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    if (Parent->Wireless->isAckPayloadAvailable())
    {
      OnlineStatus = true;
      Parent->Wireless->read(ReceivedResponse, WirelessPayloadSize);
      ReceivedSequenceID = ((AeroCommonTemplate *)ReceivedResponse)->SequenceID;
      if (*Debug)
      {
        logToSerials(F("Response SequenceID:"), false, 4);
        logToSerials(ReceivedSequenceID, false, 1);
        logToSerials(F("-"), false, 1);
        logToSerials(toText_aeroSequenceID(ReceivedSequenceID), true, 1);
      }

      switch (ReceivedSequenceID)
      {
      case AeroMessages::AeroModuleResponse1:
        memcpy(AeroModule1ReceivedResponse, ReceivedResponse, sizeof(struct HempyModuleResponse));
        if (*Debug)
        {
          logToSerials(F("Module1:"), false, 4);
          logToSerials(AeroModule1ReceivedResponse -> Status, true, 1);
        }
        break;
      case AeroMessages::AeroResponse1:
        memcpy(Aero1ReceivedResponse, ReceivedResponse, sizeof(struct AeroResponse));
        if (Aero1CommandToSend.SprayEnabled || Aero1CommandToSend.SprayDisabled || Aero1CommandToSend.SprayNow || Aero1CommandToSend.SprayOff || Aero1CommandToSend.PumpOn || Aero1CommandToSend.PumpOff || Aero1CommandToSend.PumpDisable || Aero1CommandToSend.MixReservoir || Aero1CommandToSend.RefillPressureTank)
        {
          SyncRequested = true; ///Force a second message exchange to actualize the response
          Aero1CommandToSend.SprayEnabled = false;
          Aero1CommandToSend.SprayDisabled = false;
          Aero1CommandToSend.SprayNow = false;
          Aero1CommandToSend.SprayOff = false;
          Aero1CommandToSend.PumpOn = false;
          Aero1CommandToSend.PumpOff = false;
          Aero1CommandToSend.PumpDisable = false;
          Aero1CommandToSend.MixReservoir = false;
          Aero1CommandToSend.RefillPressureTank = false;
        }
        if (*Debug)
        {
          logToSerials(F("Aero1:"), false, 4);          
          logToSerials(Aero1ReceivedResponse -> PressureTankPresent, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(Aero1ReceivedResponse -> SprayEnabled, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(Aero1ReceivedResponse -> State, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(Aero1ReceivedResponse -> Pressure, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(Aero1ReceivedResponse -> LastSprayPressure, true, 1);
        }
        break;      
      case AeroMessages::AeroGetNext:
        if (*Debug)
        {
          logToSerials(F("Last message received"), true, 4);
        }
        break;
      default:
        logToSerials(F("SequenceID not known, ignoring package"), true, 4);
        break;
      }
      LastResponseReceived = millis();
    }
    else
    {
      if (*Debug)
        logToSerials(F("Acknowledgement received without any data"), true, 4);
      OnlineStatus = false; /// Comment this out if you have modules that do not return any data
    }
  }
  else
  {
    if (*Debug)
      logToSerials(F("No response"), true, 3);
    OnlineStatus = false;
  }
  return ReceivedSequenceID;
}

void AeroModule_Web::updateCommands()
{ // so you can see that new data is being sent
  AeroModule1CommandToSend.Time = now();
  AeroModule1CommandToSend.Debug = *Debug;
  AeroModule1CommandToSend.Metric = *Metric;
  Aero1CommandToSend.SprayInterval = DefaultSettings->Interval;
  Aero1CommandToSend.SprayDuration = DefaultSettings->Duration;
  Aero1CommandToSend.PumpTimeOut = DefaultSettings->PumpTimeOut;
  Aero1CommandToSend.PumpPrimingTime = DefaultSettings->PrimingTime;
  Aero1CommandToSend.MinPressure = DefaultSettings->MinPressure;
  Aero1CommandToSend.MaxPressure = DefaultSettings->MaxPressure;
}