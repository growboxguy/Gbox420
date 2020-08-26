#include "HempyModule_Web.h"

HempyModule_Web::HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
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
  logToSerials(F("HempyModule_Web object created"), true, 1);
}

void HempyModule_Web::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Bucket1 Weight:"));
  strcat(LongMessage, toText_weight(Bucket1ReceivedResponse -> Weight));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText(Bucket1CommandToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("/"));
  strcat(LongMessage, toText(Bucket1CommandToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("] ; Bucket2 Weight:"));
  strcat(LongMessage, toText_weight(Bucket2ReceivedResponse -> Weight));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText(Bucket2CommandToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("/"));
  strcat(LongMessage, toText(Bucket2CommandToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("]"));
  logToSerials(&LongMessage, true, 1);
}

void HempyModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Status\":\""));
  strcat(LongMessage, toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpB1\":\""));
  strcat(LongMessage, toText(Bucket1ReceivedResponse -> PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"WeightB1\":\""));
  strcat(LongMessage, toText(Bucket1ReceivedResponse -> Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StartB1\":\""));
  strcat(LongMessage, toText(Bucket1CommandToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StopB1\":\""));
  strcat(LongMessage, toText(Bucket1CommandToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpB2\":\""));
  strcat(LongMessage, toText(Bucket2ReceivedResponse -> PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"WeightB2\":\""));
  strcat(LongMessage, toText(Bucket2ReceivedResponse -> Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StartB2\":\""));
  strcat(LongMessage, toText(Bucket2CommandToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StopB2\":\""));
  strcat(LongMessage, toText(Bucket2CommandToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void HempyModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgFloat(getComponentName(F("B1Strt")), Bucket1CommandToSend.StartWeight);
    WebServer.setArgFloat(getComponentName(F("B1Stp")), Bucket1CommandToSend.StopWeight);
    WebServer.setArgInt(getComponentName(F("B1Time")), Bucket1CommandToSend.TimeOutPump);
    WebServer.setArgFloat(getComponentName(F("B2Strt")), Bucket2CommandToSend.StartWeight);
    WebServer.setArgFloat(getComponentName(F("B2Stp")), Bucket2CommandToSend.StopWeight);
    WebServer.setArgInt(getComponentName(F("B2Time")), Bucket2CommandToSend.TimeOutPump);
  }
}

void HempyModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("B1Weight")), toText_weight(Bucket1ReceivedResponse -> Weight));
    WebServer.setArgString(getComponentName(F("B2Weight")), toText_weight(Bucket2ReceivedResponse -> Weight));
    WebServer.setArgString(getComponentName(F("B1Pump")), toText_pumpState(Bucket1ReceivedResponse -> PumpState));
    WebServer.setArgString(getComponentName(F("B2Pump")), toText_pumpState(Bucket2ReceivedResponse -> PumpState));
  }
}

void HempyModule_Web::websiteEvent_Button(char *Button)
{ ///When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("B1PumpOn")) == 0)
    {
      Bucket1CommandToSend.TurnOnPump = true;
      Parent->addToLog(F("Watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpOn")) == 0)
    {
      Bucket2CommandToSend.TurnOnPump = true;
      Parent->addToLog(F("Watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PumpOff")) == 0)
    {
      Bucket1CommandToSend.TurnOffPump = true;
      Parent->addToLog(F("Stop watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpOff")) == 0)
    {
      Bucket2CommandToSend.TurnOffPump = true;
      Parent->addToLog(F("Stop watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PumpDis")) == 0)
    {
      Bucket1CommandToSend.DisablePump = true;
      Parent->addToLog(F("Disabled HempyBucket 1 pump"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpDis")) == 0)
    {
      Bucket2CommandToSend.DisablePump = true;
      Parent->addToLog(F("Disabled HempyBucket 2 pump"), false);
    }
    SyncRequested = true;
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
    if (strcmp_P(ShortMessage, (PGM_P)F("B1Strt")) == 0)
    {
      DefaultSettings->StartWeight_B1 = WebServer.getArgFloat();
    } /// \todo Write back to EEPROM
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Stp")) == 0)
    {
      DefaultSettings->StopWeight_B1 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 1 limits updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Time")) == 0)
    {
      DefaultSettings->TimeOutPump_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 1 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Strt")) == 0)
    {
      DefaultSettings->StartWeight_B2 = WebServer.getArgFloat();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Stp")) == 0)
    {
      DefaultSettings->StopWeight_B2 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 2 limits updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Time")) == 0)
    {
      DefaultSettings->TimeOutPump_B2 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 2 timeout updated"), false);
    }
    SyncRequested = true;
  }
}

void HempyModule_Web::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    sendMessages();
  }
}

void HempyModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  sendMessages();
}

void HempyModule_Web::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
}

void HempyModule_Web::sendMessages()
{
  updateCommands();
  sendCommand(&Module1CommandToSend);                                                                                       //< Command - Response exchange
  sendCommand(&Bucket1CommandToSend);                                                                                       //< Command - Response exchange
  sendCommand(&Bucket2CommandToSend);                                                                                       //< Command - Response exchange
  while (sendCommand(&GetNextResponse) < HempyMessages::HempyGetNext && millis() - LastResponseReceived < WirelessMessageTimeout) //< special Command, only exchange Response.
    ;
  if (Debug)
    logToSerials(F("Message exchange finished"), true, 3);
}

HempyMessages HempyModule_Web::sendCommand(void *CommandToSend)
{
  HempyMessages SequenceIDToSend = ((HempyCommonTemplate *)CommandToSend)->SequenceID;
  HempyMessages ReceivedSequenceID = NULL;
  if (Debug)
  {
    logToSerials(F("Sending SequenceID:"), false, 3);
    logToSerials(SequenceIDToSend, false, 1);
    logToSerials(F("-"), false, 1);
    logToSerials(toText_hempySequenceID(SequenceIDToSend), false, 1);
    logToSerials(F("and waiting for Acknowledgment..."), true, 1);
  }
  Parent->Wireless->openWritingPipe(WirelessChannel);
  Parent->Wireless->flush_rx(); ///< Dump all previously received but unprocessed messages
  if (Parent->Wireless->write(CommandToSend, WirelessPayloadSize))
  {
    delay(50); //< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    if (Parent->Wireless->isAckPayloadAvailable())
    {
      OnlineStatus = true; ///< Mark that the module responded
      Parent->Wireless->read(ReceivedResponse, WirelessPayloadSize);
      ReceivedSequenceID = ((HempyCommonTemplate *)ReceivedResponse)->SequenceID;
      if (*Debug)
      {
        logToSerials(F("Response SequenceID:"), false, 4);
        logToSerials(ReceivedSequenceID, false, 1);
        logToSerials(F("-"), false, 1);
        logToSerials(toText_hempySequenceID(ReceivedSequenceID), true, 1);
      }

      switch (ReceivedSequenceID)
      {
      case HempyMessages::HempyModule1Response:
        memcpy(Module1ReceivedResponse, ReceivedResponse, sizeof(struct HempyModuleResponse));
        if (*Debug)
        {
          logToSerials(F("Module1:"), false, 4);
          logToSerials(Module1ReceivedResponse -> Status, true, 1);
        }
        break;
      case HempyMessages::HempyBucket1Response:
        memcpy(Bucket1ReceivedResponse, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (Bucket1CommandToSend.DisablePump || Bucket1CommandToSend.TurnOnPump || Bucket1CommandToSend.TurnOffPump) ///Turn off command flags
        {
          SyncRequested = true; ///Force a second packet to actualize the response
          Bucket1CommandToSend.DisablePump = false;
          Bucket1CommandToSend.TurnOnPump = false;
          Bucket1CommandToSend.TurnOffPump = false;
        }
        if (*Debug)
        {
          logToSerials(F("Bucket1:"), false, 4);
          logToSerials(Bucket1ReceivedResponse -> PumpState, false, 1);         
          logToSerials(F(","), false, 1);
          logToSerials(Bucket1ReceivedResponse -> Weight, true, 1);
        }
        break;
      case HempyMessages::HempyBucket2Response:
        memcpy(Bucket2ReceivedResponse, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (Bucket2CommandToSend.DisablePump || Bucket2CommandToSend.TurnOnPump || Bucket2CommandToSend.TurnOffPump) ///Turn off command flags
        {
          SyncRequested = true; ///Force a second message exchange to actualize the response
          Bucket2CommandToSend.DisablePump = false;
          Bucket2CommandToSend.TurnOnPump = false;
          Bucket2CommandToSend.TurnOffPump = false;
        }
        if (*Debug)
        {
          logToSerials(F("Bucket2:"), false, 4);
          logToSerials(Bucket2ReceivedResponse -> PumpState, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(Bucket2ReceivedResponse -> Weight, true, 1);
        }
        break;
      case HempyMessages::HempyGetNext:
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
        logToSerials(F("Acknowledgement received without any data"), true, 4);  //< Indicates a communication problem - Make sure to have bypass capacitors across the 3.3V power line and ground powering the nRF24L01+        
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

void HempyModule_Web::updateCommands()
{
  Module1CommandToSend.Time = now();
  Module1CommandToSend.Debug = *Debug;
  Module1CommandToSend.Metric = *Metric;
  Bucket1CommandToSend.StartWeight = DefaultSettings->StartWeight_B1;
  Bucket1CommandToSend.StopWeight = DefaultSettings->StopWeight_B1;
  Bucket1CommandToSend.TimeOutPump = DefaultSettings->TimeOutPump_B1;
  Bucket2CommandToSend.StartWeight = DefaultSettings->StartWeight_B2;
  Bucket2CommandToSend.StopWeight = DefaultSettings->StopWeight_B2;
  Bucket2CommandToSend.TimeOutPump = DefaultSettings->TimeOutPump_B2;
}