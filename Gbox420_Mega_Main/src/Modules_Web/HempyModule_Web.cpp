#include "HempyModule_Web.h"

struct HempyModuleCommand HempyModuleCommand1ToSend = {HempyMessages::HempyModuleCommand1};      ///< Command to send will be stored here
struct HempyModuleResponse HempyModuleResponse1Received = {HempyMessages::HempyModuleResponse1}; ///< Response will be stored here
struct HempyBucketCommand HempyBucketCommand1ToSend = {HempyMessages::HempyBucketCommand1};      ///< Command to send will be stored here
struct HempyBucketResponse HempyBucketResponse1Received = {HempyMessages::HempyBucketResponse1}; ///< Response will be stored here
struct HempyBucketCommand HempyBucketCommand2ToSend = {HempyMessages::HempyBucketCommand2};      ///< Command to send will be stored here
struct HempyBucketResponse HempyBucketResponse2Received = {HempyMessages::HempyBucketResponse2}; ///< Response will be stored here
struct HempyCommonTemplate HempyResetToSend = {HempyMessages::HempyReset};                       ///< Special command to fetch the next Response from the Receiver

/**
 * @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings and subscribes to events
 */
HempyModule_Web::HempyModule_Web(const __FlashStringHelper *Name, MainModule *Parent, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name), Parent(Parent)
{ 
  this->DefaultSettings = DefaultSettings;
  updateCommands();
  memcpy_P(this->WirelessChannel, (PGM_P)Name, sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
  logToSerials(F("HempyModule_Web ready"), true, 3);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void HempyModule_Web::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_onlineStatus(OnlineStatus) : toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"H1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_hempyState(HempyBucketResponse1Received.HempyState) : toText((int)HempyBucketResponse1Received.HempyState));
  strcat_P(LongMessage, (PGM_P)F("\",\"P1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_waterPumpState(HempyBucketResponse1Received.PumpState) : toText((int)HempyBucketResponse1Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(HempyBucketCommand1ToSend.PumpSpeed) : toText(HempyBucketCommand1ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"PT1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucketCommand1ToSend.PumpTimeOut) : toText(HempyBucketCommand1ToSend.PumpTimeOut));
  strcat_P(LongMessage, (PGM_P)F("\",\"DT1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucketCommand1ToSend.DrainWaitTime) : toText(HempyBucketCommand1ToSend.DrainWaitTime));
  strcat_P(LongMessage, (PGM_P)F("\",\"WB1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketResponse1Received.WeightB) : toText(HempyBucketResponse1Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketResponse1Received.DryWeight) : toText(HempyBucketResponse1Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketResponse1Received.WetWeight) : toText(HempyBucketResponse1Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"MW1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketCommand1ToSend.MaxWeight) : toText(HempyBucketCommand1ToSend.MaxWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketCommand1ToSend.EvaporationTarget) : toText(HempyBucketCommand1ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"OT1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketCommand1ToSend.DrainTargetWeight) : toText(HempyBucketCommand1ToSend.DrainTargetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"H2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_hempyState(HempyBucketResponse2Received.HempyState) : toText((int)HempyBucketResponse2Received.HempyState));
  strcat_P(LongMessage, (PGM_P)F("\",\"P2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_waterPumpState(HempyBucketResponse2Received.PumpState) : toText((int)HempyBucketResponse2Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(HempyBucketCommand2ToSend.PumpSpeed) : toText(HempyBucketCommand2ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"PT2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucketCommand2ToSend.PumpTimeOut) : toText(HempyBucketCommand2ToSend.PumpTimeOut));
  strcat_P(LongMessage, (PGM_P)F("\",\"DT2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucketCommand2ToSend.DrainWaitTime) : toText(HempyBucketCommand2ToSend.DrainWaitTime));
  strcat_P(LongMessage, (PGM_P)F("\",\"WB2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketResponse2Received.WeightB) : toText(HempyBucketResponse2Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketResponse2Received.DryWeight) : toText(HempyBucketResponse2Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketResponse2Received.WetWeight) : toText(HempyBucketResponse2Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"MW2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketCommand2ToSend.MaxWeight) : toText(HempyBucketCommand2ToSend.MaxWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketCommand2ToSend.EvaporationTarget) : toText(HempyBucketCommand2ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"OT2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucketCommand2ToSend.DrainTargetWeight) : toText(HempyBucketCommand2ToSend.DrainTargetWeight));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void HempyModule_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgString(getName(F("B1ET"), true), toText(HempyBucketCommand1ToSend.EvaporationTarget));
  WebServer.setArgString(getName(F("B1OF"), true), toText(HempyBucketCommand1ToSend.DrainTargetWeight));
  WebServer.setArgString(getName(F("B1MW"), true), toText(HempyBucketCommand1ToSend.MaxWeight));
  WebServer.setArgInt(getName(F("B1PS"), true), HempyBucketCommand1ToSend.PumpSpeed);
  WebServer.setArgInt(getName(F("B1T"), true), HempyBucketCommand1ToSend.PumpTimeOut);
  WebServer.setArgInt(getName(F("B1D"), true), HempyBucketCommand1ToSend.DrainWaitTime);
  WebServer.setArgString(getName(F("B1DW"), true), toText(HempyBucketResponse1Received.DryWeight));
  WebServer.setArgString(getName(F("B2ET"), true), toText(HempyBucketCommand2ToSend.EvaporationTarget));
  WebServer.setArgString(getName(F("B2OF"), true), toText(HempyBucketCommand2ToSend.DrainTargetWeight));
  WebServer.setArgString(getName(F("B2MW"), true), toText(HempyBucketCommand2ToSend.MaxWeight));
  WebServer.setArgInt(getName(F("B2PS"), true), HempyBucketCommand2ToSend.PumpSpeed);
  WebServer.setArgInt(getName(F("B2T"), true), HempyBucketCommand2ToSend.PumpTimeOut);
  WebServer.setArgInt(getName(F("B2D"), true), HempyBucketCommand2ToSend.DrainWaitTime);
  WebServer.setArgString(getName(F("B2DW"), true), toText(HempyBucketResponse2Received.DryWeight));
}

void HempyModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  WebServer.setArgString(getName(F("S"), true), toText_onlineStatus(OnlineStatus));
  WebServer.setArgString(getName(F("B1W"), true), toText_weight(HempyBucketResponse1Received.WeightB));
  WebServer.setArgString(getName(F("B1DWW"), true), toText(HempyBucketResponse1Received.DryWeight, "/", HempyBucketResponse1Received.WetWeight));
  WebServer.setArgString(getName(F("B1S"), true), toText_hempyState(HempyBucketResponse1Received.HempyState));
  WebServer.setArgString(getName(F("B1P"), true), toText_waterPumpState(HempyBucketResponse1Received.PumpState));
  WebServer.setArgString(getName(F("B2W"), true), toText_weight(HempyBucketResponse2Received.WeightB));
  WebServer.setArgString(getName(F("B2DWW"), true), toText(HempyBucketResponse2Received.DryWeight, "/", HempyBucketResponse2Received.WetWeight));
  WebServer.setArgString(getName(F("B2S"), true), toText_hempyState(HempyBucketResponse2Received.HempyState));
  WebServer.setArgString(getName(F("B2P"), true), toText_waterPumpState(HempyBucketResponse2Received.PumpState));
}

bool HempyModule_Web::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    // Bucket1
    if (strcmp_P(ShortMessage, (PGM_P)F("B1On")) == 0)
    {
      HempyBucketCommand1ToSend.StartWatering = true;
      Parent->addToLog(F("Watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Off")) == 0)
    {
      HempyBucketCommand1ToSend.StopWatering = true;
      Parent->addToLog(F("Stop watering HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Dis")) == 0)
    {
      HempyBucketCommand1ToSend.Disable = true;
      Parent->addToLog(F("Disabled HempyBucket 1"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareB")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightB = true;
      Parent->addToLog(F("Taring Bucket 1 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareDW")) == 0)
    {
      HempyBucketCommand1ToSend.TareWeightDW = true;
      Parent->addToLog(F("Taring Bucket 1 Dry/Wet"), false);
    }    
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B1 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1OF")) == 0)
    {
      DefaultSettings->DrainTargetWeight_B1 = toFloat(Data);
      Parent->addToLog(F("Bucket 1 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1WL")) == 0)
    {
      DefaultSettings->MaxWeight_B1 = toFloat(Data);
      Parent->addToLog(F("Bucket 1 Max weight updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B1 = toInt(Data);
      Parent->addToLog(F("Pump 1 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B1 = toInt(Data);
      Parent->addToLog(F("Pump 1 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B1 = toInt(Data);
      Parent->addToLog(F("B1 Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1DW")) == 0)
    {
      HempyBucketCommand1ToSend.DryWeight = toFloat(Data);
      Parent->addToLog(F("B1 dry weight updated"), false);
    }
    // Bucket
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2On")) == 0)
    {
      HempyBucketCommand2ToSend.StartWatering = true;
      Parent->addToLog(F("Watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Off")) == 0)
    {
      HempyBucketCommand2ToSend.StopWatering = true;
      Parent->addToLog(F("Stop watering HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Dis")) == 0)
    {
      HempyBucketCommand2ToSend.Disable = true;
      Parent->addToLog(F("Disabled HempyBucket 2"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareB")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightB = true;
      Parent->addToLog(F("Taring Bucket 2 scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareDW")) == 0)
    {
      HempyBucketCommand2ToSend.TareWeightDW = true;
      Parent->addToLog(F("Taring Bucket 2 Dry/Wet"), false);
    }    
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B2 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2OF")) == 0)
    {
      DefaultSettings->DrainTargetWeight_B2 = toFloat(Data);
      Parent->addToLog(F("Bucket 2 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2WL")) == 0)
    {
      DefaultSettings->MaxWeight_B2 = toFloat(Data);
      Parent->addToLog(F("Bucket 2 Max weight updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B2 = toInt(Data);
      Parent->addToLog(F("Pump 2 speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B2 = toInt(Data);
      Parent->addToLog(F("Pump 2 timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B2 = toInt(Data);
      Parent->addToLog(F("B2 Drain wait updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2DW")) == 0)
    {
      HempyBucketCommand2ToSend.DryWeight = toFloat(Data);
      Parent->addToLog(F("B2 dry weight updated"), false);
    }
    else
    {
      return false;
    }
    SyncRequested = true;
    return true;
  }
}

void HempyModule_Web::refresh_Sec()
{
  Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    // syncModule(WirelessChannel,&Command,&Response);
    sendMessages();
  }
}

void HempyModule_Web::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  // syncModule(WirelessChannel,&Command,&Response);
  sendMessages();
}

void HempyModule_Web::refresh_Minute()
{
  Common::refresh_Minute();
}

void HempyModule_Web::sendMessages()
{
  updateCommands();
  sendCommand(&HempyResetToSend);          ///< special Command, resets communication to first message
  sendCommand(&HempyModuleCommand1ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucketCommand1ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucketCommand2ToSend); ///< Command - Response exchange
  if (Parent->SerialReportWireless && Debug)
  {
    logToSerials(F("Message exchange finished"), true, 1);
  }
}

HempyMessages HempyModule_Web::sendCommand(void *CommandToSend)
{
  HempyMessages SequenceIDToSend = ((HempyCommonTemplate *)CommandToSend)->SequenceID;
  HempyMessages ReceivedSequenceID = NULL;
  if (Parent->SerialReportWireless)
  {
    logToSerials(F("Sending:"), false, 1);
    logToSerials(toText_hempySequenceID(SequenceIDToSend), false, 1);
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
      if (Parent->SerialReportWireless)
      {
        logToSerials(F("; Response:"), false, 1);
        logToSerials(toText_hempySequenceID(ReceivedSequenceID), false, 1);
        logToSerials(F("; Data:"), false, 1);
      }

      switch (ReceivedSequenceID)
      {
      case HempyMessages::HempyModuleResponse1:
        memcpy(&HempyModuleResponse1Received, ReceivedResponse, sizeof(struct HempyModuleResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(HempyModuleResponse1Received.Status, true, 1);
        }
        break;
      case HempyMessages::HempyBucketResponse1:
        memcpy(&HempyBucketResponse1Received, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(HempyBucketResponse1Received.ConfirmDisable, false, 1);
          logToSerials(HempyBucketResponse1Received.ConfirmStartWatering, false, 1);
          logToSerials(HempyBucketResponse1Received.ConfirmStopWatering, false, 1);
          logToSerials(HempyBucketResponse1Received.ConfirmTareWeightB, false, 1);
          logToSerials(HempyBucketResponse1Received.ConfirmTareWeightDW, false, 1);
          logToSerials(toText((int)HempyBucketResponse1Received.HempyState), false, 1);
          logToSerials(toText((int)HempyBucketResponse1Received.PumpState), false, 1);
          logToSerials(HempyBucketResponse1Received.WeightB, false, 1);
          logToSerials(HempyBucketResponse1Received.DryWeight, false, 1);
          logToSerials(HempyBucketResponse1Received.WetWeight, true, 1);
        }
        if (HempyBucketCommand1ToSend.Disable || HempyBucketCommand1ToSend.StartWatering || HempyBucketCommand1ToSend.StopWatering || HempyBucketCommand1ToSend.TareWeightB || HempyBucketCommand1ToSend.TareWeightDW ) ///< Turn off command flags
        {
          SyncRequested = true; ///< Force a second packet to actualize the response
        }
        if (HempyBucketResponse1Received.ConfirmDisable)
          HempyBucketCommand1ToSend.Disable = false; // Turn off the Flag once the Receiver confirms processing it
        if (HempyBucketResponse1Received.ConfirmStartWatering)
          HempyBucketCommand1ToSend.StartWatering = false;
        if (HempyBucketResponse1Received.ConfirmStopWatering)
          HempyBucketCommand1ToSend.StopWatering = false;
        if (HempyBucketResponse1Received.ConfirmTareWeightB)
          HempyBucketCommand1ToSend.TareWeightB = false;
        if (HempyBucketResponse1Received.ConfirmTareWeightDW)
          HempyBucketCommand1ToSend.TareWeightDW = false;

        if (!isnan(HempyBucketCommand1ToSend.DryWeight) && HempyBucketResponse1Received.DryWeight == HempyBucketCommand1ToSend.DryWeight)
        {
          HempyBucketCommand1ToSend.DryWeight = NAN;
        }
        break;
      case HempyMessages::HempyBucketResponse2:
        memcpy(&HempyBucketResponse2Received, ReceivedResponse, sizeof(struct HempyBucketResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(HempyBucketResponse2Received.ConfirmDisable, false, 1);
          logToSerials(HempyBucketResponse2Received.ConfirmStartWatering, false, 1);
          logToSerials(HempyBucketResponse2Received.ConfirmStopWatering, false, 1);
          logToSerials(HempyBucketResponse2Received.ConfirmTareWeightB, false, 1);
          logToSerials(HempyBucketResponse2Received.ConfirmTareWeightDW, false, 1);
          logToSerials(toText((int)HempyBucketResponse2Received.HempyState), false, 1);
          logToSerials(toText((int)HempyBucketResponse2Received.PumpState), false, 1);
          logToSerials(HempyBucketResponse2Received.WeightB, false, 1);
          logToSerials(HempyBucketResponse2Received.DryWeight, false, 1);
          logToSerials(HempyBucketResponse2Received.WetWeight, true, 1);
        }
        if (HempyBucketCommand2ToSend.Disable || HempyBucketCommand2ToSend.StartWatering || HempyBucketCommand2ToSend.StopWatering || HempyBucketCommand2ToSend.TareWeightB || HempyBucketCommand2ToSend.TareWeightDW ) ///< Turn off command flags
        {
          SyncRequested = true; ///< Force another message exchange when a command is active
        }
        if (HempyBucketResponse2Received.ConfirmDisable)
          HempyBucketCommand2ToSend.Disable = false; // Turn off the Flag once the Receiver confirms processing it
        if (HempyBucketResponse2Received.ConfirmStartWatering)
          HempyBucketCommand2ToSend.StartWatering = false;
        if (HempyBucketResponse2Received.ConfirmStopWatering)
          HempyBucketCommand2ToSend.StopWatering = false;
        if (HempyBucketResponse2Received.ConfirmTareWeightB)
          HempyBucketCommand2ToSend.TareWeightB = false;
        if (HempyBucketResponse2Received.ConfirmTareWeightDW)
          HempyBucketCommand2ToSend.TareWeightDW = false;

        if (!isnan(HempyBucketCommand2ToSend.DryWeight) && HempyBucketResponse2Received.DryWeight == HempyBucketCommand2ToSend.DryWeight)
        {
          HempyBucketCommand2ToSend.DryWeight = NAN;
        }
        break;
      case HempyMessages::HempyReset:
        if (Parent->SerialReportWireless)
        {
          logToSerials(F("-"), true, 1); ///< Reset messages does not have any data
        }
        break;
      default:
        if (Parent->SerialReportWireless)
        {
          logToSerials(F("SequenceID unknown"), true, 1);
        }
        break;
      }
      LastResponseReceived = millis();
    }
    else
    {
      if (Parent->SerialReportWireless)
        logToSerials(F("; Ack received without data"), true, 1); ///< Indicates a communication problem - Make sure to have bypass capacitors across the 3.3V power line and ground powering the nRF24L01+
    }
  }
  else
  {
    if (Parent->SerialReportWireless)
      logToSerials(F("; No response"), true, 1);
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
  HempyModuleCommand1ToSend.Debug = Debug;
  HempyModuleCommand1ToSend.Metric = Metric;
  HempyModuleCommand1ToSend.SerialReportDate = Parent->SerialReportDate;
  HempyModuleCommand1ToSend.SerialReportMemory = Parent->SerialReportMemory;
  HempyModuleCommand1ToSend.SerialReportJSONFriendly = Parent->SerialReportJSONFriendly;
  HempyModuleCommand1ToSend.SerialReportJSON = Parent->SerialReportJSON;
  HempyModuleCommand1ToSend.SerialReportWireless = Parent->SerialReportWireless;
  HempyBucketCommand1ToSend.EvaporationTarget = DefaultSettings->EvaporationTarget_B1;
  HempyBucketCommand1ToSend.DrainTargetWeight = DefaultSettings->DrainTargetWeight_B1;
  HempyBucketCommand1ToSend.MaxWeight = DefaultSettings->MaxWeight_B1;
  HempyBucketCommand1ToSend.DrainWaitTime = DefaultSettings->DrainWaitTime_B1;
  HempyBucketCommand1ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B1;
  HempyBucketCommand1ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut_B1;
  HempyBucketCommand2ToSend.EvaporationTarget = DefaultSettings->EvaporationTarget_B2;
  HempyBucketCommand2ToSend.DrainTargetWeight = DefaultSettings->DrainTargetWeight_B2;
  HempyBucketCommand2ToSend.MaxWeight = DefaultSettings->MaxWeight_B2;
  HempyBucketCommand2ToSend.DrainWaitTime = DefaultSettings->DrainWaitTime_B2;
  HempyBucketCommand2ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B2;
  HempyBucketCommand2ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut_B2;
}