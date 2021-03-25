#include "ReservoirModule_Web.h"

struct ReservoirModuleCommand ReservoirModuleCommand1ToSend = {ReservoirMessages::ReservoirModuleCommand1};      ///< Command to send will be stored here
struct ReservoirModuleResponse ReservoirModuleResponse1Received = {ReservoirMessages::ReservoirModuleResponse1}; ///< Response will be stored here
struct ReservoirCommand ReservoirCommand1ToSend = {ReservoirMessages::ReservoirCommand1};                        ///< Command to send will be stored here
struct ReservoirResponse ReservoirResponse1Received = {ReservoirMessages::ReservoirResponse1};                   ///< Response will be stored here
struct ReservoirCommonTemplate ReservoirResetToSend = {ReservoirMessages::ReservoirReset};                       ///< Special command to fetch the next Response from the Receiver

/**
* @brief Constructor, creates an instance of the class, loads the EEPROM stored persistent settings and subscribes to events
*/
ReservoirModule_Web::ReservoirModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ReservoirModuleSettings *DefaultSettings) : Common_Web(Name), Common(Name)
{
  this->Parent = Parent;
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
void ReservoirModule_Web::report(bool JSONReport)
{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
    strcat(LongMessage, toText(OnlineStatus));
    strcat_P(LongMessage, (PGM_P)F("\",\"P\":\""));
    strcat(LongMessage, toText(ReservoirResponse1Received.PH));
    strcat_P(LongMessage, (PGM_P)F("\",\"T\":\""));
    strcat(LongMessage, toText(ReservoirResponse1Received.TDS));
    strcat_P(LongMessage, (PGM_P)F("\",\"W\":\""));
    strcat(LongMessage, toText(ReservoirResponse1Received.Weight));
    strcat_P(LongMessage, (PGM_P)F("\",\"WT\":\""));
    strcat(LongMessage, toText(ReservoirResponse1Received.WaterTemperature));
    strcat_P(LongMessage, (PGM_P)F("\",\"AT\":\""));
    strcat(LongMessage, toText(ReservoirResponse1Received.AirTemperature));
    strcat_P(LongMessage, (PGM_P)F("\",\"H\":\""));
    strcat(LongMessage, toText(ReservoirResponse1Received.Humidity));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    Common::report();
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("PH:"));
    strcat(LongMessage, toText(ReservoirResponse1Received.PH));
    strcat_P(LongMessage, (PGM_P)F(" ; TDS:"));
    strcat(LongMessage, toText(ReservoirResponse1Received.TDS));
    strcat_P(LongMessage, (PGM_P)F(" ; Weight:"));
    strcat(LongMessage, toText_weight(ReservoirResponse1Received.Weight));
    strcat_P(LongMessage, (PGM_P)F(" ; Water temp:"));
    strcat(LongMessage, toText_temp(ReservoirResponse1Received.WaterTemperature));
    strcat_P(LongMessage, (PGM_P)F(" ; Air temp:"));
    strcat(LongMessage, toText_temp(ReservoirResponse1Received.AirTemperature));
    strcat_P(LongMessage, (PGM_P)F(" ; Humidity:"));
    strcat(LongMessage, toText_percentage(ReservoirResponse1Received.Humidity));
    logToSerials(&LongMessage, true, 1);
  }
}

void ReservoirModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("S")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("PH")), toText(ReservoirResponse1Received.PH));
    WebServer.setArgString(getComponentName(F("TDS")), toText_TDS(ReservoirResponse1Received.TDS));
    WebServer.setArgString(getComponentName(F("W")), toText_weight(ReservoirResponse1Received.Weight));
    WebServer.setArgString(getComponentName(F("WT")), toText_temp(ReservoirResponse1Received.WaterTemperature));
    WebServer.setArgString(getComponentName(F("AT")), toText_temp(ReservoirResponse1Received.AirTemperature));
    WebServer.setArgString(getComponentName(F("H")), toText_percentage(ReservoirResponse1Received.Humidity));
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
void ReservoirModule_Web::commandEvent(char *Command, char *Data)
{ ///< When a button is pressed on the website
  if (!isThisMyComponent(Command))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
    {
      ReservoirCommand1ToSend.TareWeight = true;
      Parent->addToLog(F("Taring reservoir scale"), false);
    }
    SyncRequested = true;
  }
}

void ReservoirModule_Web::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    sendMessages();
  }
}

void ReservoirModule_Web::refresh_FiveSec()
{
  if (*Debug)
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
  sendCommand(&ReservoirResetToSend);          ///< special Command, resets communication to first message
  if (*Debug)
    logToSerials(F("Message exchange finished"), true, 3);
}

/**
* @brief Send a single command and process the Response received in the Acknowledgement package
*/
ReservoirMessages ReservoirModule_Web::sendCommand(void *CommandToSend)
{
  ReservoirMessages SequenceIDToSend = ((ReservoirCommonTemplate *)CommandToSend)->SequenceID;
  ReservoirMessages ReceivedSequenceID = NULL;
  if (*Debug)
  {
    logToSerials(F("Sending SequenceID:"), false, 3);
    logToSerials(SequenceIDToSend, false, 1);
    logToSerials(F("-"), false, 1);
    logToSerials(toText_reservoirSequenceID(SequenceIDToSend), false, 1);
    logToSerials(F("and waiting for Acknowledgment"), true, 1);
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
      if (*Debug)
      {
        logToSerials(F("Response SequenceID:"), false, 4);
        logToSerials(ReceivedSequenceID, false, 1);
        logToSerials(F("-"), false, 1);
        logToSerials(toText_reservoirSequenceID(ReceivedSequenceID), true, 1);
      }

      switch (ReceivedSequenceID)
      {
      case ReservoirMessages::ReservoirModuleResponse1:
        memcpy(&ReservoirModuleResponse1Received, ReceivedResponse, sizeof(struct ReservoirModuleResponse));
        if (*Debug)
        {
          logToSerials(F("Module:"), false, 4);
          logToSerials(ReservoirModuleResponse1Received.Status, true, 1);
        }
        break;
      case ReservoirMessages::ReservoirResponse1:
        memcpy(&ReservoirResponse1Received, ReceivedResponse, sizeof(struct ReservoirResponse));
        if (ReservoirCommand1ToSend.TareWeight)
        {
          SyncRequested = true; ///< Force a second message exchange to actualize the response
          ReservoirCommand1ToSend.TareWeight = false;
        }
        if (*Debug)
        {
          logToSerials(F("Reservoir:"), false, 4);
          logToSerials(ReservoirResponse1Received.PH, false, 1);
          logToSerials(ReservoirResponse1Received.TDS, false, 1);
          logToSerials(ReservoirResponse1Received.Weight, false, 1);
          logToSerials(ReservoirResponse1Received.WaterTemperature, false, 1);
          logToSerials(ReservoirResponse1Received.AirTemperature, false, 1);
          logToSerials(ReservoirResponse1Received.Humidity, true, 1);
        }
        break;
      case ReservoirMessages::ReservoirReset:
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

/**
 * @brief Updates the command sent to the remote Reservoir Module wirelessly
*/
void ReservoirModule_Web::updateCommands()
{
  ReservoirModuleCommand1ToSend.Time = now();
  ReservoirModuleCommand1ToSend.Debug = *Debug;
  ReservoirModuleCommand1ToSend.Metric = *Metric;
  ReservoirModuleCommand1ToSend.JSONToSerialMode = *(Parent->JSONToSerialMode);
}