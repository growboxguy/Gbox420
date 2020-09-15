#include "AeroModule_Web.h"

struct AeroModuleCommand AeroModuleCommand1ToSend = {AeroMessages::AeroModuleCommand1};  ///Command to send will be stored here
struct AeroModuleResponse AeroModuleResponse1Received = {AeroMessages::AeroModuleResponse1};   /// Default startup values
struct AeroCommand AeroCommand1ToSend = {AeroMessages::AeroCommand1}; ///Command to send will be stored here
struct AeroResponse AeroResponse1Received = {AeroMessages::AeroResponse1};  /// Default startup values
struct AeroCommonTemplate AeroGetNextToSend = {AeroMessages::AeroGetNext};    //< Special command to fetch the next Response from the Receiver


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
  strcat(LongMessage, toText_pressure(AeroResponse1Received.Pressure));
  if (AeroResponse1Received.PressureTankPresent)
  {
    strcat_P(LongMessage, (PGM_P)F(" ["));
    strcat(LongMessage, toText(AeroCommand1ToSend.MinPressure));
    strcat_P(LongMessage, (PGM_P)F("/"));
    strcat(LongMessage, toText(AeroCommand1ToSend.MaxPressure));
    strcat_P(LongMessage, (PGM_P)F("]"));
  }
  strcat_P(LongMessage, (PGM_P)F(" ; LastSprayPressure:"));
  strcat(LongMessage, toText_pressure(AeroResponse1Received.LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, toText_yesNo(AeroResponse1Received.SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; DayMode:"));
  strcat(LongMessage, toText(AeroCommand1ToSend.DayMode));
  strcat_P(LongMessage, (PGM_P)F(" ; DayInterval:"));
  strcat(LongMessage, toText_minute(AeroCommand1ToSend.DayInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; DayDuration:"));
  strcat(LongMessage, toText_second(AeroCommand1ToSend.DayDuration));
  strcat_P(LongMessage, (PGM_P)F(" ; NightInterval:"));
  strcat(LongMessage, toText_minute(AeroCommand1ToSend.NightInterval));
  strcat_P(LongMessage, (PGM_P)F(" ; NightDuration:"));
  strcat(LongMessage, toText_second(AeroCommand1ToSend.NightDuration));
  logToSerials(&LongMessage, true, 1);
}

void AeroModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Status\":\""));
  strcat(LongMessage, toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"Pressure\":\""));
  strcat(LongMessage, toText(AeroResponse1Received.Pressure));
  if (AeroResponse1Received.PressureTankPresent)
  {
    strcat_P(LongMessage, (PGM_P)F("\",\"Min\":\""));
    strcat(LongMessage, toText(AeroCommand1ToSend.MinPressure));
    strcat_P(LongMessage, (PGM_P)F("\",\"Max\":\""));
    strcat(LongMessage, toText(AeroCommand1ToSend.MaxPressure));
  }
  strcat_P(LongMessage, (PGM_P)F("\",\"LastSpray\":\""));
  strcat(LongMessage, toText(AeroResponse1Received.LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpState\":\""));
  strcat(LongMessage, toText(AeroResponse1Received.State));
  strcat_P(LongMessage, (PGM_P)F("\",\"SprayEnabled\":\""));
  strcat(LongMessage, toText(AeroCommand1ToSend.SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F("\",\"DayInterval\":\""));
  strcat(LongMessage, toText(AeroCommand1ToSend.DayInterval));
  strcat_P(LongMessage, (PGM_P)F("\",\"DayDuration\":\""));
  strcat(LongMessage, toText(AeroCommand1ToSend.DayDuration));
  strcat_P(LongMessage, (PGM_P)F("\",\"NightInterval\":\""));
  strcat(LongMessage, toText(AeroCommand1ToSend.NightInterval));
  strcat_P(LongMessage, (PGM_P)F("\",\"NightDuration\":\""));
  strcat(LongMessage, toText(AeroCommand1ToSend.NightDuration));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void AeroModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("Tank")), AeroResponse1Received.PressureTankPresent);
    WebServer.setArgInt(getComponentName(F("Timeout")), AeroCommand1ToSend.PumpTimeOut);
    WebServer.setArgInt(getComponentName(F("Priming")), AeroCommand1ToSend.PumpPrimingTime);
    WebServer.setArgInt(getComponentName(F("DInt")), AeroCommand1ToSend.DayInterval);
    WebServer.setArgInt(getComponentName(F("DDur")), AeroCommand1ToSend.DayDuration);
    WebServer.setArgInt(getComponentName(F("NInt")), AeroCommand1ToSend.NightInterval);
    WebServer.setArgInt(getComponentName(F("NDur")), AeroCommand1ToSend.NightDuration);
    WebServer.setArgFloat(getComponentName(F("PresMax")), AeroCommand1ToSend.MaxPressure);
    WebServer.setArgFloat(getComponentName(F("PresMin")), AeroCommand1ToSend.MinPressure);
  }
}

void AeroModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("Spray")), toText_enabledDisabled(AeroResponse1Received.SprayEnabled));
    WebServer.setArgString(getComponentName(F("Pump")), toText_pumpState(AeroResponse1Received.State));
    WebServer.setArgString(getComponentName(F("Pres")), toText_pressure(AeroResponse1Received.Pressure));
    WebServer.setArgString(getComponentName(F("LastSP")), toText_pressure(AeroResponse1Received.LastSprayPressure));
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
      AeroCommand1ToSend.SprayEnabled = true;
      Parent->addToLog(F("Spray enabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0)
    {
      AeroCommand1ToSend.SprayDisabled = true;
      Parent->addToLog(F("Spray disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      AeroCommand1ToSend.SprayNow = true;
      Parent->addToLog(F("Starting spray"));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      AeroCommand1ToSend.SprayOff = true;
      Parent->addToLog(F("Stop spray"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      AeroCommand1ToSend.PumpOn = true;
      Parent->addToLog(F("Aero pump ON"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      AeroCommand1ToSend.PumpOff = true;
      Parent->addToLog(F("Aero pump OFF"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0)
    {
      AeroCommand1ToSend.PumpDisable = true;
      Parent->addToLog(F("Aero pump disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      AeroCommand1ToSend.MixReservoir = true;
      Parent->addToLog(F("Mixing reservoir"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refill")) == 0)
    {
      if (AeroResponse1Received.PressureTankPresent)
      {
        AeroCommand1ToSend.RefillPressureTank = true;
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
    if (strcmp_P(ShortMessage, (PGM_P)F("DInt")) == 0)
    {
      DefaultSettings->DayInterval = WebServer.getArgInt();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("DDur")) == 0)
    {
      DefaultSettings->DayDuration = WebServer.getArgInt();
      Parent->addToLog(F("Day spray updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("NInt")) == 0)
    {
      DefaultSettings->NightInterval = WebServer.getArgInt();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("NDur")) == 0)
    {
      DefaultSettings->NightDuration = WebServer.getArgInt();
      Parent->addToLog(F("Night spray updated"), false);
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
  sendCommand(&AeroModuleCommand1ToSend);                                                                                       //< Command - Response exchange
  sendCommand(&AeroCommand1ToSend);                                                                                         //< Command - Response exchange
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
        memcpy(&AeroModuleResponse1Received, ReceivedResponse, sizeof(struct AeroModuleResponse));
        if (*Debug)
        {
          logToSerials(F("Module1:"), false, 4);
          logToSerials(AeroModuleResponse1Received.Status, true, 1);
        }
        break;
      case AeroMessages::AeroResponse1:
        memcpy(&AeroResponse1Received, ReceivedResponse, sizeof(struct AeroResponse));
        if (AeroCommand1ToSend.SprayEnabled || AeroCommand1ToSend.SprayDisabled || AeroCommand1ToSend.SprayNow || AeroCommand1ToSend.SprayOff || AeroCommand1ToSend.PumpOn || AeroCommand1ToSend.PumpOff || AeroCommand1ToSend.PumpDisable || AeroCommand1ToSend.MixReservoir || AeroCommand1ToSend.RefillPressureTank)
        {
          SyncRequested = true; ///Force a second message exchange to actualize the response
          AeroCommand1ToSend.SprayEnabled = false;
          AeroCommand1ToSend.SprayDisabled = false;
          AeroCommand1ToSend.SprayNow = false;
          AeroCommand1ToSend.SprayOff = false;
          AeroCommand1ToSend.PumpOn = false;
          AeroCommand1ToSend.PumpOff = false;
          AeroCommand1ToSend.PumpDisable = false;
          AeroCommand1ToSend.MixReservoir = false;
          AeroCommand1ToSend.RefillPressureTank = false;
        }
        if (*Debug)
        {
          logToSerials(F("Aero1:"), false, 4);          
          logToSerials(AeroResponse1Received.PressureTankPresent, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(AeroResponse1Received.SprayEnabled, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(AeroResponse1Received.State, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(AeroResponse1Received.Pressure, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(AeroResponse1Received.LastSprayPressure, true, 1);
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
  AeroModuleCommand1ToSend.Time = now();
  AeroModuleCommand1ToSend.Debug = *Debug;
  AeroModuleCommand1ToSend.Metric = *Metric;
  AeroCommand1ToSend.DayMode = ((MainModule*)Parent) -> getDayMode();
  AeroCommand1ToSend.DayInterval = DefaultSettings->DayInterval;
  AeroCommand1ToSend.DayDuration = DefaultSettings->DayDuration;
  AeroCommand1ToSend.NightInterval = DefaultSettings->NightInterval;
  AeroCommand1ToSend.NightDuration = DefaultSettings->NightDuration;
  AeroCommand1ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut;
  AeroCommand1ToSend.PumpPrimingTime = DefaultSettings->PrimingTime;
  AeroCommand1ToSend.MinPressure = DefaultSettings->MinPressure;
  AeroCommand1ToSend.MaxPressure = DefaultSettings->MaxPressure;
}