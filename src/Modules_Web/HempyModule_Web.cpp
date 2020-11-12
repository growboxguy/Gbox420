#include "HempyModule_Web.h"

struct HempyModuleCommand HempyModuleCommand1ToSend = {HempyMessages::HempyModuleCommand1};  ///Command to send will be stored here
struct HempyModuleResponse HempyModuleResponse1Received = {HempyMessages::HempyModuleResponse1};  /// Response will be stored here
struct HempyBucketCommand HempyBucketCommand1ToSend = {HempyMessages::HempyBucketCommand1}; ///Command to send will be stored here
struct HempyBucketResponse HempyBucketResponse1Received = {HempyMessages::HempyBucketResponse1};  /// Response will be stored here
struct HempyBucketCommand HempyBucketCommand2ToSend = {HempyMessages::HempyBucketCommand2}; ///Command to send will be stored here
struct HempyBucketResponse HempyBucketResponse2Received = {HempyMessages::HempyBucketResponse2};  /// Response will be stored here
struct HempyCommonTemplate HempyGetNextToSend = {HempyMessages::HempyGetNext};            //< Special command to fetch the next Response from the Receiver

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
  strcat(LongMessage, toText_weight(HempyBucketResponse1Received.Weight));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("/"));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("] ; Bucket2 Weight:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse2Received.Weight));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("/"));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("]"));
  logToSerials(&LongMessage, true, 1);
}

void HempyModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Stat\":\""));
  strcat(LongMessage, toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpB1\":\""));
  strcat(LongMessage, toText(HempyBucketResponse1Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpB1Spd\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.PumpSpeed_B1));
  strcat_P(LongMessage, (PGM_P)F("\",\"WeightB1\":\""));
  strcat(LongMessage, toText(HempyBucketResponse1Received.Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StartB1\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StopB1\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpB2\":\""));
  strcat(LongMessage, toText(HempyBucketResponse2Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PumpB2Spd\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.PumpSpeed_B2));
  strcat_P(LongMessage, (PGM_P)F("\",\"WeightB2\":\""));
  strcat(LongMessage, toText(HempyBucketResponse2Received.Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StartB2\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.StartWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"StopB2\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.StopWeight));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void HempyModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("B1WBW")), HempyBucketCommand1ToSend.WeightBasedWatering);
    WebServer.setArgString(getComponentName(F("B1Strt")), toText(HempyBucketCommand1ToSend.StartWeight));
    WebServer.setArgString(getComponentName(F("B1Stp")), toText(HempyBucketCommand1ToSend.StopWeight));
    WebServer.setArgBoolean(getComponentName(F("B1TBW")), HempyBucketCommand1ToSend.TimerBasedWatering);
    WebServer.setArgInt(getComponentName(F("B1Int")), HempyBucketCommand1ToSend.WateringInterval);
    WebServer.setArgInt(getComponentName(F("B1Dur")), HempyBucketCommand1ToSend.WateringDuration);  
    WebServer.setArgInt(getComponentName(F("B1PumpSp")), HempyBucketCommand1ToSend.PumpSpeed);
    WebServer.setArgInt(getComponentName(F("B1PumpSpS")), HempyBucketCommand1ToSend.PumpSpeed);  
    WebServer.setArgInt(getComponentName(F("B1Time")), HempyBucketCommand1ToSend.TimeOutPump);
    WebServer.setArgBoolean(getComponentName(F("B2WBW")), HempyBucketCommand2ToSend.WeightBasedWatering);
    WebServer.setArgString(getComponentName(F("B2Strt")), toText(HempyBucketCommand2ToSend.StartWeight));
    WebServer.setArgString(getComponentName(F("B2Stp")), toText(HempyBucketCommand2ToSend.StopWeight));
    WebServer.setArgBoolean(getComponentName(F("B2TBW")), HempyBucketCommand2ToSend.TimerBasedWatering);
    WebServer.setArgInt(getComponentName(F("B2Int")), HempyBucketCommand2ToSend.WateringInterval);
    WebServer.setArgInt(getComponentName(F("B2Dur")), HempyBucketCommand2ToSend.WateringDuration); 
    WebServer.setArgInt(getComponentName(F("B2PumpSp")), HempyBucketCommand2ToSend.PumpSpeed);
    WebServer.setArgInt(getComponentName(F("B2PumpSpS")), HempyBucketCommand2ToSend.PumpSpeed);
    WebServer.setArgInt(getComponentName(F("B2Time")), HempyBucketCommand2ToSend.TimeOutPump);
  }
}

void HempyModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("B1Weight")), toText_weight(HempyBucketResponse1Received.WeightB));
    WebServer.setArgString(getComponentName(F("B2Weight")), toText_weight(HempyBucketResponse2Received.WeightB));
    WebServer.setArgString(getComponentName(F("B1Waste")), toText_weight(HempyBucketResponse1Received.WeightWR));
    WebServer.setArgString(getComponentName(F("B2Waste")), toText_weight(HempyBucketResponse2Received.WeightWR));
    WebServer.setArgString(getComponentName(F("B1Pump")), toText_pumpState(HempyBucketResponse1Received.PumpState));
    WebServer.setArgString(getComponentName(F("B2Pump")), toText_pumpState(HempyBucketResponse2Received.PumpState));
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
      HempyBucketCommand1ToSend.TurnOnPump = true;
      Parent->addToLog(F("Watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpOn")) == 0)
    {
      HempyBucketCommand2ToSend.TurnOnPump = true;
      Parent->addToLog(F("Watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PumpOff")) == 0)
    {
      HempyBucketCommand1ToSend.TurnOffPump = true;
      Parent->addToLog(F("Stop watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpOff")) == 0)
    {
      HempyBucketCommand2ToSend.TurnOffPump = true;
      Parent->addToLog(F("Stop watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PumpDis")) == 0)
    {
      HempyBucketCommand1ToSend.DisablePump = true;
      Parent->addToLog(F("Disabled HempyBucket 1 pump"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpDis")) == 0)
    {
      HempyBucketCommand2ToSend.DisablePump = true;
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
    if (strcmp_P(ShortMessage, (PGM_P)F("B1WBW")) == 0)
    {
      DefaultSettings->WeightBasedWatering_B1 = WebServer.getArgBoolean();
      if(DefaultSettings->WeightBasedWatering_B1)
      {
        Parent->addToLog(F("Weight based watering ON"), false);
      }
      else
      {
       Parent->addToLog(F("Weight based watering OFF"), false);
      }      
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Strt")) == 0)
    {
      DefaultSettings->StartWeight_B1 = WebServer.getArgFloat();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Stp")) == 0)
    {
      DefaultSettings->StopWeight_B1 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 1 limits updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TBW")) == 0)
    {
      DefaultSettings->TimerBasedWatering_B1 = WebServer.getArgBoolean();
      if(DefaultSettings->TimerBasedWatering_B1)
      {
        Parent->addToLog(F("Timer based watering ON"), false);
      }
      else
      {
       Parent->addToLog(F("Timer based watering OFF"), false);
      }      
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Int")) == 0)
    {
      DefaultSettings->WateringInterval_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Watering interval updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Dur")) == 0)
    {
      DefaultSettings->WateringDuration_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Watering duration updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PumpSp")) == 0)
    {
      DefaultSettings->PumpSpeed_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 1 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Time")) == 0)
    {
      DefaultSettings->TimeOutPump_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 1 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2WBW")) == 0)
    {
      DefaultSettings->WeightBasedWatering_B2 = WebServer.getArgBoolean();
      if(DefaultSettings->WeightBasedWatering_B2)
      {
        Parent->addToLog(F("Weight based watering ON"), false);
      }
      else
      {
       Parent->addToLog(F("Weight based watering OFF"), false);
      }      
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
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TBW")) == 0)
    {
      DefaultSettings->TimerBasedWatering_B2 = WebServer.getArgBoolean();
      if(DefaultSettings->TimerBasedWatering_B2)
      {
        Parent->addToLog(F("Timer based watering ON"), false);
      }
      else
      {
       Parent->addToLog(F("Timer based watering OFF"), false);
      }      
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Int")) == 0)
    {
      DefaultSettings->WateringInterval_B2 = WebServer.getArgInt();
      Parent->addToLog(F("Watering interval updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Dur")) == 0)
    {
      DefaultSettings->WateringDuration_B2 = WebServer.getArgInt();
      Parent->addToLog(F("Watering duration updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PumpSp")) == 0)
    {
      DefaultSettings->PumpSpeed_B2 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 2 speed updated"), false);
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
    //syncModule(WirelessChannel,&Command,&Response);
    sendMessages();
  }
}

void HempyModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  //syncModule(WirelessChannel,&Command,&Response);
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
  sendCommand(&HempyModuleCommand1ToSend);                                                                                       //< Command - Response exchange
  sendCommand(&HempyBucketCommand1ToSend);                                                                                       //< Command - Response exchange
  sendCommand(&HempyBucketCommand2ToSend);                                                                                       //< Command - Response exchange
  while (sendCommand(&HempyGetNextToSend) < HempyMessages::HempyGetNext && millis() - LastResponseReceived < WirelessMessageTimeout) //< special Command, only exchange Response.
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
      case HempyMessages::HempyModuleResponse1:
        memcpy(&HempyModuleResponse1Received, ReceivedResponse, sizeof(struct HempyModuleResponse));
        if (*Debug)
        {
          logToSerials(F("Module1:"), false, 4);
          logToSerials(HempyModuleResponse1Received.Status, true, 1);
        }
        break;
      case HempyMessages::HempyBucketResponse1:
        memcpy(&HempyBucketResponse1Received, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (HempyBucketCommand1ToSend.DisablePump || HempyBucketCommand1ToSend.TurnOnPump || HempyBucketCommand1ToSend.TurnOffPump) ///Turn off command flags
        {
          SyncRequested = true; ///Force a second packet to actualize the response
          HempyBucketCommand1ToSend.DisablePump = false;
          HempyBucketCommand1ToSend.TurnOnPump = false;
          HempyBucketCommand1ToSend.TurnOffPump = false;
        }
        if (*Debug)
        {
          logToSerials(F("Bucket1:"), false, 4);
          logToSerials(HempyBucketResponse1Received.PumpState, false, 1);         
          logToSerials(F(","), false, 1);
          logToSerials(HempyBucketResponse1Received.Weight, true, 1);
        }
        break;
      case HempyMessages::HempyBucketResponse2:
        memcpy(&HempyBucketResponse2Received, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (HempyBucketCommand2ToSend.DisablePump || HempyBucketCommand2ToSend.TurnOnPump || HempyBucketCommand2ToSend.TurnOffPump) ///Turn off command flags
        {
          SyncRequested = true; ///Force a second message exchange to actualize the response
          HempyBucketCommand2ToSend.DisablePump = false;
          HempyBucketCommand2ToSend.TurnOnPump = false;
          HempyBucketCommand2ToSend.TurnOffPump = false;
        }
        if (*Debug)
        {
          logToSerials(F("Bucket2:"), false, 4);
          logToSerials(HempyBucketResponse2Received.PumpState, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(HempyBucketResponse2Received.Weight, true, 1);
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
  HempyModuleCommand1ToSend.Time = now();
  HempyModuleCommand1ToSend.Debug = *Debug;
  HempyModuleCommand1ToSend.Metric = *Metric;
  HempyBucketCommand1ToSend.WeightBasedWatering = DefaultSettings->WeightBasedWatering_B1;
  HempyBucketCommand1ToSend.StartWeight = DefaultSettings->StartWeight_B1;
  HempyBucketCommand1ToSend.StopWeight = DefaultSettings->StopWeight_B1;
  HempyBucketCommand1ToSend.TimerBasedWatering = DefaultSettings->TimerBasedWatering_B1;
  HempyBucketCommand1ToSend.WateringInterval = DefaultSettings->WateringInterval_B1;
  HempyBucketCommand1ToSend.WateringDuration = DefaultSettings->WateringDuration_B1;
  HempyBucketCommand1ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B1;
  HempyBucketCommand1ToSend.TimeOutPump = DefaultSettings->TimeOutPump_B1;
  HempyBucketCommand2ToSend.WeightBasedWatering = DefaultSettings->WeightBasedWatering_B2;
  HempyBucketCommand2ToSend.StartWeight = DefaultSettings->StartWeight_B2;
  HempyBucketCommand2ToSend.StopWeight = DefaultSettings->StopWeight_B2;
  HempyBucketCommand2ToSend.TimerBasedWatering = DefaultSettings->TimerBasedWatering_B2;
  HempyBucketCommand2ToSend.WateringInterval = DefaultSettings->WateringInterval_B2;
  HempyBucketCommand2ToSend.WateringDuration = DefaultSettings->WateringDuration_B2;
  HempyBucketCommand2ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B2;
  HempyBucketCommand2ToSend.TimeOutPump = DefaultSettings->TimeOutPump_B2;
}