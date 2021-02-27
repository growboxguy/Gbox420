#include "HempyModule_Web.h"

struct HempyModuleCommand HempyModuleCommand1ToSend = {HempyMessages::HempyModuleCommand1};      ///< Command to send will be stored here
struct HempyModuleResponse HempyModuleResponse1Received = {HempyMessages::HempyModuleResponse1}; ///< Response will be stored here
struct HempyBucketCommand HempyBucketCommand1ToSend = {HempyMessages::HempyBucketCommand1};      ///< Command to send will be stored here
struct HempyBucketResponse HempyBucketResponse1Received = {HempyMessages::HempyBucketResponse1}; ///< Response will be stored here
struct HempyBucketCommand HempyBucketCommand2ToSend = {HempyMessages::HempyBucketCommand2};      ///< Command to send will be stored here
struct HempyBucketResponse HempyBucketResponse2Received = {HempyMessages::HempyBucketResponse2}; ///< Response will be stored here
struct HempyCommonTemplate HempyResetToSend = {HempyMessages::HempyReset};                       ///< Special command to fetch the next Response from the Receiver

HempyModule_Web::HempyModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
{ ///< Constructor
  this->Parent = Parent;
  this->DefaultSettings = DefaultSettings;
  updateCommands();
  memcpy_P(this->WirelessChannel, (PGM_P)Name, sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToWebsiteQueue_Field(this);
  Parent->addToWebsiteQueue_Button(this);
  logToSerials(F("HempyModule_Web object created"), true, 3);
}

void HempyModule_Web::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Bucket1 Weight:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse1Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F(" [Dry:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse1Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F(", Wet:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse1Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("] ; EvaporationTarget:"));
  strcat(LongMessage, toText_weight(HempyBucketCommand1ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F(" ; OverFlowTarget:"));
  strcat(LongMessage, toText_weight(HempyBucketCommand1ToSend.OverflowTarget));
  strcat_P(LongMessage, (PGM_P)F(" ; Waste1 Weight:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse1Received.WeightWR));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText_weight(HempyBucketCommand1ToSend.WasteLimit));
  strcat_P(LongMessage, (PGM_P)F("] ; Bucket2 Weight:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse2Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F(" [Dry:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse2Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F(", Wet:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse2Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("] ; EvaporationTarget:"));
  strcat(LongMessage, toText_weight(HempyBucketCommand2ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F(" ; OverFlowTarget:"));
  strcat(LongMessage, toText_weight(HempyBucketCommand2ToSend.OverflowTarget));
  strcat_P(LongMessage, (PGM_P)F(" ; Waste2 Weight:"));
  strcat(LongMessage, toText_weight(HempyBucketResponse2Received.WeightWR));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText_weight(HempyBucketCommand2ToSend.WasteLimit));
  strcat_P(LongMessage, (PGM_P)F("]"));
  logToSerials(&LongMessage, true, 1);
}

void HempyModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"H1\":\""));
  strcat(LongMessage, toText((int)HempyBucketResponse1Received.HempyState));
  strcat_P(LongMessage, (PGM_P)F("\",\"P1\":\""));
  strcat(LongMessage, toText((int)HempyBucketResponse1Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS1\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"WB1\":\""));
  strcat(LongMessage, toText(HempyBucketResponse1Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F("\",\"WR1\":\""));
  strcat(LongMessage, toText(HempyBucketResponse1Received.WeightWR));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW1\":\""));
  strcat(LongMessage, toText(HempyBucketResponse1Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW1\":\""));
  strcat(LongMessage, toText(HempyBucketResponse1Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET1\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"OT1\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.OverflowTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"WL1\":\""));
  strcat(LongMessage, toText(HempyBucketCommand1ToSend.WasteLimit));
  strcat_P(LongMessage, (PGM_P)F("\",\"H2\":\""));
  strcat(LongMessage, toText((int)HempyBucketResponse2Received.HempyState));
  strcat_P(LongMessage, (PGM_P)F("\",\"P2\":\""));
  strcat(LongMessage, toText((int)HempyBucketResponse2Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS2\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"WB2\":\""));
  strcat(LongMessage, toText(HempyBucketResponse2Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F("\",\"WR2\":\""));
  strcat(LongMessage, toText(HempyBucketResponse2Received.WeightWR));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW2\":\""));
  strcat(LongMessage, toText(HempyBucketResponse2Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW2\":\""));
  strcat(LongMessage, toText(HempyBucketResponse2Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET2\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"OT2\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.OverflowTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"WL2\":\""));
  strcat(LongMessage, toText(HempyBucketCommand2ToSend.WasteLimit));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void HempyModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("B1ET")), toText(HempyBucketCommand1ToSend.EvaporationTarget));
    WebServer.setArgString(getComponentName(F("B1OF")), toText(HempyBucketCommand1ToSend.OverflowTarget));
    WebServer.setArgString(getComponentName(F("B1WL")), toText(HempyBucketCommand1ToSend.WasteLimit));
    WebServer.setArgInt(getComponentName(F("B1PS")), HempyBucketCommand1ToSend.PumpSpeed);
    WebServer.setArgInt(getComponentName(F("B1T")), HempyBucketCommand1ToSend.PumpTimeOut);
    WebServer.setArgInt(getComponentName(F("B1D")), HempyBucketCommand1ToSend.DrainWaitTime);
    WebServer.setArgInt(getComponentName(F("B1WT")), HempyBucketCommand1ToSend.WateringTimeOut);
    WebServer.setArgFloat(getComponentName(F("B1DW")), HempyBucketResponse1Received.DryWeight);
    WebServer.setArgString(getComponentName(F("B2ET")), toText(HempyBucketCommand2ToSend.EvaporationTarget));
    WebServer.setArgString(getComponentName(F("B2OF")), toText(HempyBucketCommand2ToSend.OverflowTarget));
    WebServer.setArgString(getComponentName(F("B2WL")), toText(HempyBucketCommand2ToSend.WasteLimit));
    WebServer.setArgInt(getComponentName(F("B2PS")), HempyBucketCommand2ToSend.PumpSpeed);
    WebServer.setArgInt(getComponentName(F("B2T")), HempyBucketCommand2ToSend.PumpTimeOut);
    WebServer.setArgInt(getComponentName(F("B2D")), HempyBucketCommand2ToSend.DrainWaitTime);
    WebServer.setArgInt(getComponentName(F("B2WT")), HempyBucketCommand2ToSend.WateringTimeOut);
    WebServer.setArgFloat(getComponentName(F("B2DW")), HempyBucketResponse2Received.DryWeight);
  }
}

void HempyModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("S")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("B1W")), toText_weight(HempyBucketResponse1Received.WeightB));
    WebServer.setArgString(getComponentName(F("B1WR")), toText_weight(HempyBucketResponse1Received.WeightWR));
    WebServer.setArgString(getComponentName(F("B1DWW")), toText(HempyBucketResponse1Received.DryWeight, HempyBucketResponse1Received.WetWeight, "/"));
    WebServer.setArgString(getComponentName(F("B1S")), toText_hempyState(HempyBucketResponse1Received.HempyState));
    WebServer.setArgString(getComponentName(F("B1P")), toText_waterPumpState(HempyBucketResponse1Received.PumpState));
    WebServer.setArgString(getComponentName(F("B2W")), toText_weight(HempyBucketResponse2Received.WeightB));
    WebServer.setArgString(getComponentName(F("B2WR")), toText_weight(HempyBucketResponse2Received.WeightWR));
    WebServer.setArgString(getComponentName(F("B2DWW")), toText(HempyBucketResponse2Received.DryWeight, HempyBucketResponse2Received.WetWeight, "/"));
    WebServer.setArgString(getComponentName(F("B2S")), toText_hempyState(HempyBucketResponse2Received.HempyState));
    WebServer.setArgString(getComponentName(F("B2P")), toText_waterPumpState(HempyBucketResponse2Received.PumpState));
  }
}

void HempyModule_Web::websiteEvent_Button(char *Button)
{ ///< When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("B1On")) == 0)
    {
      HempyBucketCommand1ToSend.StartWatering = true;
      Parent->addToLog(F("Watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2On")) == 0)
    {
      HempyBucketCommand2ToSend.StartWatering = true;
      Parent->addToLog(F("Watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Off")) == 0)
    {
      HempyBucketCommand1ToSend.StopWatering = true;
      Parent->addToLog(F("Stop watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Off")) == 0)
    {
      HempyBucketCommand2ToSend.StopWatering = true;
      Parent->addToLog(F("Stop watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Dis")) == 0)
    {
      HempyBucketCommand1ToSend.Disable = true;
      Parent->addToLog(F("Disabled HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Dis")) == 0)
    {
      HempyBucketCommand2ToSend.Disable = true;
      Parent->addToLog(F("Disabled HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareB")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightB = true;
      Parent->addToLog(F("Taring Bucket 1 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareWR")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightWR = true;
      Parent->addToLog(F("Taring Bucket 1 waste scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareB")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightB = true;
      Parent->addToLog(F("Taring Bucket 2 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareWR")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightWR = true;
      Parent->addToLog(F("Taring Bucket 2 waste scale"), false);
    }
    SyncRequested = true;
  }
}

void HempyModule_Web::websiteEvent_Field(char *Field)
{ ///< When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("B1ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B1 = WebServer.getArgFloat();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1OF")) == 0)
    {
      DefaultSettings->OverflowTarget_B1 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 1 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1WL")) == 0)
    {
      DefaultSettings->WasteLimit_B1 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 1 waste limit updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 1 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B1 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 1 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B1 = WebServer.getArgInt();
      Parent->addToLog(F("B1 Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1WT")) == 0)
    {
      DefaultSettings->WateringTimeOut_B1 = WebServer.getArgInt();
      Parent->addToLog(F("B1 Watering timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1DW")) == 0)
    {
      HempyBucketCommand1ToSend.DryWeight = WebServer.getArgFloat();
      Parent->addToLog(F("B1 DryWeight updated"), false);
    }    
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B2 = WebServer.getArgFloat();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2OF")) == 0)
    {
      DefaultSettings->OverflowTarget_B2 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 2 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2WL")) == 0)
    {
      DefaultSettings->WasteLimit_B2 = WebServer.getArgFloat();
      Parent->addToLog(F("Bucket 2 waste limit updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B2 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 2 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B2 = WebServer.getArgInt();
      Parent->addToLog(F("Pump 2 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B2 = WebServer.getArgInt();
      Parent->addToLog(F("B2 Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2WT")) == 0)
    {
      DefaultSettings->WateringTimeOut_B2 = WebServer.getArgInt();
      Parent->addToLog(F("B2 Watering timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2DW")) == 0)
    {
      HempyBucketCommand2ToSend.DryWeight = WebServer.getArgFloat();
      Parent->addToLog(F("B2 DryWeight updated"), false);
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
  sendCommand(&HempyResetToSend);          ///< special Command, resets communication to first message
  sendCommand(&HempyModuleCommand1ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucketCommand1ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucketCommand2ToSend); ///< Command - Response exchange
  sendCommand(&HempyResetToSend);          ///< special Command, resets communication to first message
  if (*Debug)
    logToSerials(F("Message exchange finished"), true, 3);
}

HempyMessages HempyModule_Web::sendCommand(void *CommandToSend)
{
  HempyMessages SequenceIDToSend = ((HempyCommonTemplate *)CommandToSend)->SequenceID;
  HempyMessages ReceivedSequenceID = NULL;
  if (*Debug)
  {
    logToSerials(F("Sending SequenceID:"), false, 3);
    logToSerials(SequenceIDToSend, false, 1);
    logToSerials(F("-"), false, 1);
    logToSerials(toText_hempySequenceID(SequenceIDToSend), false, 1);
    logToSerials(F("and waiting for Acknowledgment"), true, 1);
  }
  Parent->Wireless->openWritingPipe(WirelessChannel);
  Parent->Wireless->flush_rx(); ///< Dump all previously received but unprocessed messages
  if (Parent->Wireless->write(CommandToSend, WirelessPayloadSize))
  {
    delay(50); ///< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
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
          logToSerials(F("Module:"), false, 4);
          logToSerials(HempyModuleResponse1Received.Status, true, 1);
        }
        break;
      case HempyMessages::HempyBucketResponse1:
        memcpy(&HempyBucketResponse1Received, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (HempyBucketCommand1ToSend.Disable || HempyBucketCommand1ToSend.StartWatering || HempyBucketCommand1ToSend.StopWatering || HempyBucketCommand1ToSend.TareWeightB || HempyBucketCommand1ToSend.TareWeightWR) ///< Turn off command flags
        {
          SyncRequested = true; ///< Force a second packet to actualize the response
          HempyBucketCommand1ToSend.Disable = false;
          HempyBucketCommand1ToSend.StartWatering = false;
          HempyBucketCommand1ToSend.StopWatering = false;
          HempyBucketCommand1ToSend.TareWeightB = false;
          HempyBucketCommand1ToSend.TareWeightWR = false;
        }
        if(!isnan(HempyBucketCommand1ToSend.DryWeight) && HempyBucketResponse1Received.DryWeight ==  HempyBucketCommand1ToSend.DryWeight)
        {
          HempyBucketCommand1ToSend.DryWeight = NAN;
        }
        if (*Debug)
        {
          logToSerials(F("Bucket1:"), false, 4);
          logToSerials(toText((int)HempyBucketResponse1Received.HempyState), false, 1);
          logToSerials(toText((int)HempyBucketResponse1Received.PumpState), false, 1);
          logToSerials(HempyBucketResponse1Received.WeightB, false, 1);
          logToSerials(HempyBucketResponse1Received.WeightWR, false, 1);
          logToSerials(HempyBucketResponse1Received.DryWeight, false, 1);
          logToSerials(HempyBucketResponse1Received.WetWeight, true, 1);
        }
        break;
      case HempyMessages::HempyBucketResponse2:
        memcpy(&HempyBucketResponse2Received, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (HempyBucketCommand2ToSend.Disable || HempyBucketCommand2ToSend.StartWatering || HempyBucketCommand2ToSend.StopWatering || HempyBucketCommand2ToSend.TareWeightB || HempyBucketCommand2ToSend.TareWeightWR) ///< Turn off command flags
        {
          SyncRequested = true; ///< Force a second message exchange to actualize the response
          HempyBucketCommand2ToSend.Disable = false;
          HempyBucketCommand2ToSend.StartWatering = false;
          HempyBucketCommand2ToSend.StopWatering = false;
          HempyBucketCommand2ToSend.TareWeightB = false;
          HempyBucketCommand2ToSend.TareWeightWR = false;
        }
        if(!isnan(HempyBucketCommand2ToSend.DryWeight) && HempyBucketResponse2Received.DryWeight ==  HempyBucketCommand2ToSend.DryWeight)
        {
          HempyBucketCommand2ToSend.DryWeight = NAN;
        }
        if (*Debug)
        {
          logToSerials(F("Bucket2:"), false, 4);
          logToSerials(toText((int)HempyBucketResponse2Received.HempyState), false, 1);
          logToSerials(toText((int)HempyBucketResponse2Received.PumpState), false, 1);
          logToSerials(HempyBucketResponse2Received.WeightB, false, 1);
          logToSerials(HempyBucketResponse2Received.WeightWR, false, 1);
          logToSerials(HempyBucketResponse2Received.DryWeight, false, 1);
          logToSerials(HempyBucketResponse2Received.WetWeight, true, 1);
        }
        break;
      case HempyMessages::HempyReset:
        if (*Debug)
        {
          logToSerials(F("Last message received"), true, 4);
        }
        break;
      default:
        if (*Debug)
        {
          logToSerials(F("SequenceID not known, ignoring package"), true, 4);
        }
        break;
      }
      LastResponseReceived = millis();
    }
    else
    {
      if (*Debug)
        logToSerials(F("Acknowledgement received without any data"), true, 4); ///< Indicates a communication problem - Make sure to have bypass capacitors across the 3.3V power line and ground powering the nRF24L01+
    }
  }
  else
  {
    if (*Debug)
      logToSerials(F("No response"), true, 3);
    if (millis() - LastResponseReceived > WirelessReceiveTimeout)
    {
      OnlineStatus = false; ///< Comment this out if you have modules that do not return any data
    }
  }
  return ReceivedSequenceID;
}

void HempyModule_Web::updateCommands()
{
  HempyModuleCommand1ToSend.Time = now();
  HempyModuleCommand1ToSend.Debug = *Debug;
  HempyModuleCommand1ToSend.Metric = *Metric;
  HempyBucketCommand1ToSend.EvaporationTarget = DefaultSettings->EvaporationTarget_B1;
  HempyBucketCommand1ToSend.OverflowTarget = DefaultSettings->OverflowTarget_B1;
  HempyBucketCommand1ToSend.WasteLimit = DefaultSettings->WasteLimit_B1;
  HempyBucketCommand1ToSend.DrainWaitTime = DefaultSettings->DrainWaitTime_B1;
  HempyBucketCommand1ToSend.WateringTimeOut = DefaultSettings->WateringTimeOut_B1;
  HempyBucketCommand1ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B1;
  HempyBucketCommand1ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut_B1;
  HempyBucketCommand2ToSend.EvaporationTarget = DefaultSettings->EvaporationTarget_B2;
  HempyBucketCommand2ToSend.OverflowTarget = DefaultSettings->OverflowTarget_B2;
  HempyBucketCommand2ToSend.WasteLimit = DefaultSettings->WasteLimit_B2;
  HempyBucketCommand2ToSend.DrainWaitTime = DefaultSettings->DrainWaitTime_B2;
  HempyBucketCommand2ToSend.WateringTimeOut = DefaultSettings->WateringTimeOut_B2;
  HempyBucketCommand2ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B2;
  HempyBucketCommand2ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut_B2;
}