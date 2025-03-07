#include "ReservoirModule_Web.h"

struct ReservoirModuleCommand ReservoirModuleCommand1ToSend = {ReservoirMessages::ReservoirModuleCommand1};      ///< Command to send will be stored here
struct ReservoirModuleResponse ReservoirModuleResponse1Received = {ReservoirMessages::ReservoirModuleResponse1}; ///< Response will be stored here
struct ReservoirCommand ReservoirCommand1ToSend = {ReservoirMessages::ReservoirCommand1};                        ///< Command to send will be stored here
struct ReservoirResponse ReservoirResponse1Received = {ReservoirMessages::ReservoirResponse1};                   ///< Response will be stored here
struct ReservoirCommonTemplate ReservoirResetToSend = {ReservoirMessages::ReservoirReset};                       ///< Special command to fetch the next Response from the Receiver

/**
 * @brief Constructor, creates an instance of the class, loads the EEPROM stored persistent settings and subscribes to events
 */
ReservoirModule_Web::ReservoirModule_Web(const __FlashStringHelper *Name, MainModule *Parent, Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name), Parent(Parent)
{
  this->DefaultSettings = DefaultSettings;
  updateCommands();
  memcpy_P(this->WirelessChannel, (PGM_P)Name, sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
  logToSerials(F("ReservoirModule_Web ready"), true, 3);
}

/**
 * @brief Report the current state to the serial console
 */
void ReservoirModule_Web::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_onlineStatus(OnlineStatus) : toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"P\":\""));
  strcat(LongMessage, toText(ReservoirResponse1Received.PH));
  strcat_P(LongMessage, (PGM_P)F("\",\"T\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_TDS(ReservoirResponse1Received.TDS) : toText(ReservoirResponse1Received.TDS));
  strcat_P(LongMessage, (PGM_P)F("\",\"W\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(ReservoirResponse1Received.Weight) : toText(ReservoirResponse1Received.Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WR\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(ReservoirResponse1Received.WeightWR) : toText(ReservoirResponse1Received.WeightWR));
  strcat_P(LongMessage, (PGM_P)F("\",\"WT\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_temp(ReservoirResponse1Received.WaterTemperature) : toText(ReservoirResponse1Received.WaterTemperature));
  strcat_P(LongMessage, (PGM_P)F("\",\"AT\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_temp(ReservoirResponse1Received.AirTemperature) : toText(ReservoirResponse1Received.AirTemperature));
  strcat_P(LongMessage, (PGM_P)F("\",\"H\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(ReservoirResponse1Received.Humidity) : toText(ReservoirResponse1Received.Humidity));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void ReservoirModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  WebServer.setArgString(getName(F("S"), true), toText_onlineStatus(OnlineStatus));
  WebServer.setArgString(getName(F("PH"), true), toText(ReservoirResponse1Received.PH));
  WebServer.setArgString(getName(F("TDS"), true), toText_TDS(ReservoirResponse1Received.TDS));
  WebServer.setArgString(getName(F("W"), true), toText_weight(ReservoirResponse1Received.Weight));
  WebServer.setArgString(getName(F("WR"), true), toText_weight(ReservoirResponse1Received.WeightWR));
  WebServer.setArgString(getName(F("WT"), true), toText_temp(ReservoirResponse1Received.WaterTemperature));
  WebServer.setArgString(getName(F("AT"), true), toText_temp(ReservoirResponse1Received.AirTemperature));
  WebServer.setArgString(getName(F("H"), true), toText_percentage(ReservoirResponse1Received.Humidity));
}

/**
 * @brief Process commands received from MQTT subscription or from the ESP-link website
 */
bool ReservoirModule_Web::commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{ ///< When a button is pressed on the website
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
    {
      ReservoirCommand1ToSend.TareWeight = true;
      Parent->addToLog(F("Taring reservoir scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("TW")) == 0)
    {
      ReservoirCommand1ToSend.TareWeightWR = true;
      Parent->addToLog(F("Taring waste scale"), false);
    }
    else
    {
      return false;
    }
    SyncRequested = true;
    return true;
  }
}

void ReservoirModule_Web::refresh_Sec()
{
  Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    sendMessages();
  }
}

void ReservoirModule_Web::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  sendMessages();
}

/**
 * @brief Exchange messages with the Reservoir module
 */
void ReservoirModule_Web::sendMessages()
{
  updateCommands();
  sendCommand(&ReservoirResetToSend);          ///< special Command, resets communication to first message
  sendCommand(&ReservoirModuleCommand1ToSend); ///< Module specific Command - Response exchange
  sendCommand(&ReservoirCommand1ToSend);       ///< Command - Response exchange
  if (Parent->SerialReportWireless && Debug)
  {
    logToSerials(F("Message exchange finished"), true, 1);
  }
}

/**
 * @brief Send a single command and process the Response received in the Acknowledgement package
 */
ReservoirMessages ReservoirModule_Web::sendCommand(void *CommandToSend)
{
  ReservoirMessages SequenceIDToSend = ((ReservoirCommonTemplate *)CommandToSend)->SequenceID;
  ReservoirMessages ReceivedSequenceID = NULL;
  if (Parent->SerialReportWireless)
  {
    logToSerials(F("Sending:"), false, 1);
    logToSerials(toText_reservoirSequenceID(SequenceIDToSend), false, 1);
  }
  Parent->Wireless->openWritingPipe(WirelessChannel);
  Parent->Wireless->flush_rx(); ///< Dump all previously received but unprocessed messages
  if (Parent->Wireless->write(CommandToSend, WirelessPayloadSize))
  {
    delay(50); ///< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    if (Parent->Wireless->isAckPayloadAvailable())
    {
      OnlineStatus = true;
      Parent->Wireless->read(ReceivedResponse, WirelessPayloadSize);
      ReceivedSequenceID = ((ReservoirCommonTemplate *)ReceivedResponse)->SequenceID;
      if (Parent->SerialReportWireless)
      {
        logToSerials(F("; Response:"), false, 1);
        logToSerials(toText_reservoirSequenceID(ReceivedSequenceID), false, 1);
        logToSerials(F("; Data:"), false, 1);
      }

      switch (ReceivedSequenceID)
      {
      case ReservoirMessages::ReservoirModuleResponse1:
        memcpy(&ReservoirModuleResponse1Received, ReceivedResponse, sizeof(struct ReservoirModuleResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(ReservoirModuleResponse1Received.Status, true, 1);
        }
        break;
      case ReservoirMessages::ReservoirResponse1:
        memcpy(&ReservoirResponse1Received, ReceivedResponse, sizeof(struct ReservoirResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(ReservoirResponse1Received.ConfirmTareWeight, false, 1);
          logToSerials(ReservoirResponse1Received.PH, false, 1);
          logToSerials(ReservoirResponse1Received.TDS, false, 1);
          logToSerials(ReservoirResponse1Received.Weight, false, 1);
          logToSerials(ReservoirResponse1Received.ConfirmTareWeightWR, false, 1);
          logToSerials(ReservoirResponse1Received.WaterTemperature, false, 1);
          logToSerials(ReservoirResponse1Received.AirTemperature, false, 1);
          logToSerials(ReservoirResponse1Received.Humidity, true, 1);
        }
        if (ReservoirCommand1ToSend.TareWeight || ReservoirCommand1ToSend.TareWeightWR)
        {
          SyncRequested = true; ///< Force another message exchange when a command is active
        }
        if (ReservoirResponse1Received.ConfirmTareWeight)
          ReservoirCommand1ToSend.TareWeight = false; // Turn off the Flag once the Receiver confirms processing it
        if (ReservoirResponse1Received.ConfirmTareWeightWR)
          ReservoirCommand1ToSend.TareWeightWR = false;
        break;
      case ReservoirMessages::ReservoirReset:
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
        logToSerials(F("; Ack received without data"), true, 1);
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

/**
 * @brief Updates the command sent to the remote Reservoir Module wirelessly
 */
void ReservoirModule_Web::updateCommands()
{
  ReservoirModuleCommand1ToSend.Time = now();
  ReservoirModuleCommand1ToSend.Debug = Debug;
  ReservoirModuleCommand1ToSend.Metric = Metric;
  ReservoirModuleCommand1ToSend.SerialReportDate = Parent->SerialReportDate;
  ReservoirModuleCommand1ToSend.SerialReportMemory = Parent->SerialReportMemory;
  ReservoirModuleCommand1ToSend.SerialReportJSONFriendly = Parent->SerialReportJSONFriendly;
  ReservoirModuleCommand1ToSend.SerialReportJSON = Parent->SerialReportJSON;
  ReservoirModuleCommand1ToSend.SerialReportWireless = Parent->SerialReportWireless;
}