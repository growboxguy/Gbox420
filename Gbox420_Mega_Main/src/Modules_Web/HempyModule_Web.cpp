#include "HempyModule_Web.h"

struct HempyCommonTemplate HempyResetToSend = {HempyMessages::HempyReset};                       ///< Special command to fetch the next Response from the Receiver
struct HempyModuleCommand HempyModuleCommand1ToSend = {HempyMessages::HempyModuleCommand1};      ///< Command to send will be stored here
struct HempyModuleResponse HempyModuleResponse1Received = {HempyMessages::HempyModuleResponse1}; ///< Response will be stored here
// Bucket1
struct HempyBucketCommand1 HempyBucket1Command1ToSend = {HempyMessages::HempyBucket1Command1};      ///< Command to send will be stored here
struct HempyBucketResponse1 HempyBucket1Response1Received = {HempyMessages::HempyBucket1Response1}; ///< Response will be stored here
struct HempyBucketCommand2 HempyBucket1Command2ToSend = {HempyMessages::HempyBucket1Command2};      ///< Command to send will be stored here
struct HempyBucketResponse2 HempyBucket1Response2Received = {HempyMessages::HempyBucket1Response2}; ///< Response will be stored here
// Bucket2
struct HempyBucketCommand1 HempyBucket2Command1ToSend = {HempyMessages::HempyBucket2Command1};      ///< Command to send will be stored here
struct HempyBucketResponse1 HempyBucket2Response1Received = {HempyMessages::HempyBucket2Response1}; ///< Response will be stored here
struct HempyBucketCommand2 HempyBucket2Command2ToSend = {HempyMessages::HempyBucket2Command2};      ///< Command to send will be stored here
struct HempyBucketResponse2 HempyBucket2Response2Received = {HempyMessages::HempyBucket2Response2}; ///< Response will be stored here

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
  strcat(LongMessage, FriendlyFormat ? toText_hempyState(HempyBucket1Response1Received.HempyState) : toText((int)HempyBucket1Response1Received.HempyState));
  strcat_P(LongMessage, (PGM_P)F("\",\"P1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_waterPumpState(HempyBucket1Response1Received.PumpState) : toText((int)HempyBucket1Response1Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(HempyBucket1Command1ToSend.PumpSpeed) : toText(HempyBucket1Command1ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"PT1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucket1Command1ToSend.PumpTimeOut) : toText(HempyBucket1Command1ToSend.PumpTimeOut));
  strcat_P(LongMessage, (PGM_P)F("\",\"D1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucket1Command1ToSend.DrainWaitTime) : toText(HempyBucket1Command1ToSend.DrainWaitTime));
  strcat_P(LongMessage, (PGM_P)F("\",\"WB1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Response1Received.WeightB) : toText(HempyBucket1Response1Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Response1Received.DryWeight) : toText(HempyBucket1Response1Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Response1Received.WetWeight) : toText(HempyBucket1Response1Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WI1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Command2ToSend.WateringIncrement) : toText(HempyBucket1Command2ToSend.WateringIncrement));
  strcat_P(LongMessage, (PGM_P)F("\",\"MW1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Command2ToSend.MaxWeight) : toText(HempyBucket1Command2ToSend.MaxWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Command2ToSend.EvaporationTarget) : toText(HempyBucket1Command2ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"OT1\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket1Command2ToSend.DrainTargetWeight) : toText(HempyBucket1Command2ToSend.DrainTargetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"H2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_hempyState(HempyBucket2Response1Received.HempyState) : toText((int)HempyBucket2Response1Received.HempyState));
  strcat_P(LongMessage, (PGM_P)F("\",\"P2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_waterPumpState(HempyBucket2Response1Received.PumpState) : toText((int)HempyBucket2Response1Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(HempyBucket2Command1ToSend.PumpSpeed) : toText(HempyBucket2Command1ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"PT2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucket2Command1ToSend.PumpTimeOut) : toText(HempyBucket2Command1ToSend.PumpTimeOut));
  strcat_P(LongMessage, (PGM_P)F("\",\"D2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(HempyBucket2Command1ToSend.DrainWaitTime) : toText(HempyBucket2Command1ToSend.DrainWaitTime));
  strcat_P(LongMessage, (PGM_P)F("\",\"WB2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Response1Received.WeightB) : toText(HempyBucket2Response1Received.WeightB));
  strcat_P(LongMessage, (PGM_P)F("\",\"DW2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Response1Received.DryWeight) : toText(HempyBucket2Response1Received.DryWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WW2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Response1Received.WetWeight) : toText(HempyBucket2Response1Received.WetWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WI2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Command2ToSend.WateringIncrement) : toText(HempyBucket2Command2ToSend.WateringIncrement));
  strcat_P(LongMessage, (PGM_P)F("\",\"MW2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Command2ToSend.MaxWeight) : toText(HempyBucket2Command2ToSend.MaxWeight));
  strcat_P(LongMessage, (PGM_P)F("\",\"ET2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Command2ToSend.EvaporationTarget) : toText(HempyBucket2Command2ToSend.EvaporationTarget));
  strcat_P(LongMessage, (PGM_P)F("\",\"OT2\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(HempyBucket2Command2ToSend.DrainTargetWeight) : toText(HempyBucket2Command2ToSend.DrainTargetWeight));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void HempyModule_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  // Bucket1
  WebServer.setArgString(getName(F("B1SW"), true), toText(HempyBucket1Command2ToSend.StartWeight));
  WebServer.setArgString(getName(F("B1WI"), true), toText(HempyBucket1Command2ToSend.WateringIncrement));
  WebServer.setArgString(getName(F("B1ET"), true), toText(HempyBucket1Command2ToSend.EvaporationTarget));
  WebServer.setArgString(getName(F("B1DT"), true), toText(HempyBucket1Command2ToSend.DrainTargetWeight));
  WebServer.setArgString(getName(F("B1MW"), true), toText(HempyBucket1Command2ToSend.MaxWeight));
  WebServer.setArgInt(getName(F("B1PS"), true), HempyBucket1Command1ToSend.PumpSpeed);
  WebServer.setArgInt(getName(F("B1T"), true), HempyBucket1Command1ToSend.PumpTimeOut);
  WebServer.setArgInt(getName(F("B1D"), true), HempyBucket1Command1ToSend.DrainWaitTime);
  WebServer.setArgString(getName(F("B1DW"), true), toText(HempyBucket1Response1Received.DryWeight));
  // Bucket2
  WebServer.setArgString(getName(F("B2SW"), true), toText(HempyBucket2Command2ToSend.StartWeight));
  WebServer.setArgString(getName(F("B2WI"), true), toText(HempyBucket2Command2ToSend.WateringIncrement));
  WebServer.setArgString(getName(F("B2ET"), true), toText(HempyBucket2Command2ToSend.EvaporationTarget));
  WebServer.setArgString(getName(F("B2DT"), true), toText(HempyBucket2Command2ToSend.DrainTargetWeight));
  WebServer.setArgString(getName(F("B2MW"), true), toText(HempyBucket2Command2ToSend.MaxWeight));
  WebServer.setArgInt(getName(F("B2PS"), true), HempyBucket2Command1ToSend.PumpSpeed);
  WebServer.setArgInt(getName(F("B2T"), true), HempyBucket2Command1ToSend.PumpTimeOut);
  WebServer.setArgInt(getName(F("B2D"), true), HempyBucket2Command1ToSend.DrainWaitTime);
  WebServer.setArgString(getName(F("B2DW"), true), toText(HempyBucket2Response1Received.DryWeight));
}

void HempyModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  WebServer.setArgString(getName(F("S"), true), toText_onlineStatus(OnlineStatus));
  WebServer.setArgString(getName(F("B1W"), true), toText_weight(HempyBucket1Response1Received.WeightB));
  WebServer.setArgString(getName(F("B1DWW"), true), toText(HempyBucket1Response1Received.DryWeight, "/", HempyBucket1Response1Received.WetWeight));
  WebServer.setArgString(getName(F("B1S"), true), toText_hempyState(HempyBucket1Response1Received.HempyState));
  WebServer.setArgString(getName(F("B1P"), true), toText_waterPumpState(HempyBucket1Response1Received.PumpState));
  WebServer.setArgString(getName(F("B2W"), true), toText_weight(HempyBucket2Response1Received.WeightB));
  WebServer.setArgString(getName(F("B2DWW"), true), toText(HempyBucket2Response1Received.DryWeight, "/", HempyBucket2Response1Received.WetWeight));
  WebServer.setArgString(getName(F("B2S"), true), toText_hempyState(HempyBucket2Response1Received.HempyState));
  WebServer.setArgString(getName(F("B2P"), true), toText_waterPumpState(HempyBucket2Response1Received.PumpState));
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
      HempyBucket1Command1ToSend.StartWatering = true;
      Parent->addToLog(F("B1 watering"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Off")) == 0)
    {
      HempyBucket1Command1ToSend.StopWatering = true;
      Parent->addToLog(F("B1 stop watering"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1Dis")) == 0)
    {
      HempyBucket1Command1ToSend.Disable = true;
      Parent->addToLog(F("B1 disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareB")) == 0)
    {
      HempyBucket1Command1ToSend.TareWeightB = true;
      Parent->addToLog(F("B1 scale tare"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1TareDW")) == 0)
    {
      HempyBucket1Command1ToSend.TareWeightDW = true;
      Parent->addToLog(F("B1 Dry/Wet tare"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1WI")) == 0)
    {
      DefaultSettings->WateringIncrement_B1 = toFloat(Data);
      Parent->addToLog(F("B1 watering increment set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1SW")) == 0)
    {
      DefaultSettings->StartWeight_B1 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1MW")) == 0)
    {
      DefaultSettings->MaxWeight_B1 = toFloat(Data);
      Parent->addToLog(F("B1 weight limits set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1DT")) == 0)
    {
      DefaultSettings->DrainTargetWeight_B1 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B1 = toFloat(Data);
      Parent->addToLog(F("B1 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B1 = toInt(Data);
      Parent->addToLog(F("B1 pump speed set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B1 = toInt(Data);
      Parent->addToLog(F("B1 pump timeout set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B1D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B1 = toInt(Data);
      Parent->addToLog(F("B1 drain wait set"), false);
    }
    // Bucket2
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2On")) == 0)
    {
      HempyBucket2Command1ToSend.StartWatering = true;
      Parent->addToLog(F("B2 watering"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Off")) == 0)
    {
      HempyBucket2Command1ToSend.StopWatering = true;
      Parent->addToLog(F("B2 stop watering"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2Dis")) == 0)
    {
      HempyBucket2Command1ToSend.Disable = true;
      Parent->addToLog(F("B2 disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareB")) == 0)
    {
      HempyBucket2Command1ToSend.TareWeightB = true;
      Parent->addToLog(F("B2 scale tare"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2TareDW")) == 0)
    {
      HempyBucket2Command1ToSend.TareWeightDW = true;
      Parent->addToLog(F("B2 Dry/Wet tare"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2WI")) == 0)
    {
      DefaultSettings->WateringIncrement_B2 = toFloat(Data);
      Parent->addToLog(F("B2 watering increment set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2SW")) == 0)
    {
      DefaultSettings->StartWeight_B2 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2MW")) == 0)
    {
      DefaultSettings->MaxWeight_B2 = toFloat(Data);
      Parent->addToLog(F("B2 weight limits set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2DT")) == 0)
    {
      DefaultSettings->DrainTargetWeight_B2 = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2ET")) == 0)
    {
      DefaultSettings->EvaporationTarget_B2 = toFloat(Data);
      Parent->addToLog(F("B2 targets updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2PS")) == 0)
    {
      DefaultSettings->PumpSpeed_B2 = toInt(Data);
      Parent->addToLog(F("B2 pump speed set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2T")) == 0)
    {
      DefaultSettings->PumpTimeOut_B2 = toInt(Data);
      Parent->addToLog(F("B2 pump timeout set"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B2D")) == 0)
    {
      DefaultSettings->DrainWaitTime_B2 = toInt(Data);
      Parent->addToLog(F("B2 drain wait set"), false);
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
  sendCommand(&HempyResetToSend);           ///< special Command, resets communication to first message
  sendCommand(&HempyModuleCommand1ToSend);  ///< Command - Response exchange
  sendCommand(&HempyBucket1Command1ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucket1Command2ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucket2Command1ToSend); ///< Command - Response exchange
  sendCommand(&HempyBucket2Command2ToSend); ///< Command - Response exchange
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
      case HempyMessages::HempyBucket1Response1:
        memcpy(&HempyBucket1Response1Received, ReceivedResponse, sizeof(struct HempyBucketResponse1));
        if (Parent->SerialReportWireless)
        {
          logToSerials(HempyBucket1Response1Received.ConfirmDisable, false, 1);
          logToSerials(HempyBucket1Response1Received.ConfirmStartWatering, false, 1);
          logToSerials(HempyBucket1Response1Received.ConfirmStopWatering, false, 1);
          logToSerials(HempyBucket1Response1Received.ConfirmTareWeightB, false, 1);
          logToSerials(HempyBucket1Response1Received.ConfirmTareWeightDW, false, 1);
          logToSerials(toText((int)HempyBucket1Response1Received.HempyState), false, 1);
          logToSerials(toText((int)HempyBucket1Response1Received.PumpState), false, 1);
          logToSerials(HempyBucket1Response1Received.WeightB, false, 1);
          logToSerials(HempyBucket1Response1Received.DryWeight, false, 1);
          logToSerials(HempyBucket1Response1Received.WetWeight, true, 1);
        }
        if (HempyBucket1Command1ToSend.Disable || HempyBucket1Command1ToSend.StartWatering || HempyBucket1Command1ToSend.StopWatering || HempyBucket1Command1ToSend.TareWeightB || HempyBucket1Command1ToSend.TareWeightDW) ///< Turn off command flags
        {
          SyncRequested = true; ///< Force a second packet to actualize the response
        }
        if (HempyBucket1Response1Received.ConfirmDisable)
          HempyBucket1Command1ToSend.Disable = false; // Turn off the Flag once the Receiver confirms processing it
        if (HempyBucket1Response1Received.ConfirmStartWatering)
          HempyBucket1Command1ToSend.StartWatering = false;
        if (HempyBucket1Response1Received.ConfirmStopWatering)
          HempyBucket1Command1ToSend.StopWatering = false;
        if (HempyBucket1Response1Received.ConfirmTareWeightB)
          HempyBucket1Command1ToSend.TareWeightB = false;
        if (HempyBucket1Response1Received.ConfirmTareWeightDW)
          HempyBucket1Command1ToSend.TareWeightDW = false;
        break;
      case HempyMessages::HempyBucket1Response2:
        memcpy(&HempyBucket1Response2Received, ReceivedResponse, sizeof(struct HempyBucketResponse2));
        if (Parent->SerialReportWireless)
        {
          logToSerials(F("-"), true, 1); ///< HempyBucket1Response2 messages does not have any data
        }
        break;
      case HempyMessages::HempyBucket2Response1:
        memcpy(&HempyBucket2Response1Received, ReceivedResponse, sizeof(struct HempyBucketResponse1));
        if (Parent->SerialReportWireless)
        {
          logToSerials(HempyBucket2Response1Received.ConfirmDisable, false, 1);
          logToSerials(HempyBucket2Response1Received.ConfirmStartWatering, false, 1);
          logToSerials(HempyBucket2Response1Received.ConfirmStopWatering, false, 1);
          logToSerials(HempyBucket2Response1Received.ConfirmTareWeightB, false, 1);
          logToSerials(HempyBucket2Response1Received.ConfirmTareWeightDW, false, 1);
          logToSerials(toText((int)HempyBucket2Response1Received.HempyState), false, 1);
          logToSerials(toText((int)HempyBucket2Response1Received.PumpState), false, 1);
          logToSerials(HempyBucket2Response1Received.WeightB, false, 1);
          logToSerials(HempyBucket2Response1Received.DryWeight, false, 1);
          logToSerials(HempyBucket2Response1Received.WetWeight, true, 1);
        }
        if (HempyBucket2Command1ToSend.Disable || HempyBucket2Command1ToSend.StartWatering || HempyBucket2Command1ToSend.StopWatering || HempyBucket2Command1ToSend.TareWeightB || HempyBucket2Command1ToSend.TareWeightDW) ///< Turn off command flags
        {
          SyncRequested = true; ///< Force another message exchange when a command is active
        }
        if (HempyBucket2Response1Received.ConfirmDisable)
          HempyBucket2Command1ToSend.Disable = false; // Turn off the Flag once the Receiver confirms processing it
        if (HempyBucket2Response1Received.ConfirmStartWatering)
          HempyBucket2Command1ToSend.StartWatering = false;
        if (HempyBucket2Response1Received.ConfirmStopWatering)
          HempyBucket2Command1ToSend.StopWatering = false;
        if (HempyBucket2Response1Received.ConfirmTareWeightB)
          HempyBucket2Command1ToSend.TareWeightB = false;
        if (HempyBucket2Response1Received.ConfirmTareWeightDW)
          HempyBucket2Command1ToSend.TareWeightDW = false;
        break;
      case HempyMessages::HempyBucket2Response2:
        memcpy(&HempyBucket2Response2Received, ReceivedResponse, sizeof(struct HempyBucketResponse2));
        if (Parent->SerialReportWireless)
        {
          logToSerials(F("-"), true, 1); ///< HempyBucket2Response2 messages does not have any data
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
  // Bucket1
  HempyBucket1Command1ToSend.DrainWaitTime = DefaultSettings->DrainWaitTime_B1;
  HempyBucket1Command1ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B1;
  HempyBucket1Command1ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut_B1;
  HempyBucket1Command2ToSend.StartWeight = DefaultSettings->StartWeight_B1;
  HempyBucket1Command2ToSend.WateringIncrement = DefaultSettings->WateringIncrement_B1;
  HempyBucket1Command2ToSend.EvaporationTarget = DefaultSettings->EvaporationTarget_B1;
  HempyBucket1Command2ToSend.DrainTargetWeight = DefaultSettings->DrainTargetWeight_B1;
  HempyBucket1Command2ToSend.MaxWeight = DefaultSettings->MaxWeight_B1;
  // Bucket2
  HempyBucket2Command1ToSend.DrainWaitTime = DefaultSettings->DrainWaitTime_B2;
  HempyBucket2Command1ToSend.PumpSpeed = DefaultSettings->PumpSpeed_B2;
  HempyBucket2Command1ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut_B2;
  HempyBucket2Command2ToSend.WateringIncrement = DefaultSettings->WateringIncrement_B2;
  HempyBucket2Command2ToSend.StartWeight = DefaultSettings->StartWeight_B2;
  HempyBucket2Command2ToSend.EvaporationTarget = DefaultSettings->EvaporationTarget_B2;
  HempyBucket2Command2ToSend.DrainTargetWeight = DefaultSettings->DrainTargetWeight_B2;
  HempyBucket2Command2ToSend.MaxWeight = DefaultSettings->MaxWeight_B2;
}