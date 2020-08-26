#include "ReservoirModule_Web.h"

struct ReservoirCommand ReservoirCommandToSend = {ReservoirMessages::ReservoirCommand1};         //< Command to send will be stored here
struct ReservoirCommonTemplate ReservoirGetNextResponse = {ReservoirMessages::ReservoirGetNext}; //< Special command to fetch the next Response from the Receiver
struct ReservoirResponse *ReservoirReceivedResponse = malloc(sizeof(struct ReservoirResponse));  //< Response will be stored here

ReservoirModule_Web::ReservoirModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
{
  this->Parent = Parent;
  this->DefaultSettings = DefaultSettings;
  updateCommands();
  memcpy_P(this->WirelessChannel, (PGM_P)Name, sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToWebsiteQueue_Refresh(this);
  logToSerials(F("ReservoirModule_Web object created"), true, 1);
}

void ReservoirModule_Web::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("PH:"));
  strcat(LongMessage, toText(ReservoirReceivedResponse->PH));
  strcat_P(LongMessage, (PGM_P)F(" ; Weight:"));
  strcat(LongMessage, toText_weight(ReservoirReceivedResponse->Weight));
  strcat_P(LongMessage, (PGM_P)F(" ; Water temp:"));
  strcat(LongMessage, toText_temp(ReservoirReceivedResponse->WaterTemperature));
  strcat_P(LongMessage, (PGM_P)F(" ; Air temp:"));
  strcat(LongMessage, toText_temp(ReservoirReceivedResponse->AirTemperature));
  strcat_P(LongMessage, (PGM_P)F(" ; Humidity:"));
  strcat(LongMessage, toText_percentage(ReservoirReceivedResponse->Humidity));
  logToSerials(&LongMessage, true, 1);
}

void ReservoirModule_Web::reportToJSON()
{
  Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"Status\":\""));
  strcat(LongMessage, toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"PH\":\""));
  strcat(LongMessage, toText(ReservoirReceivedResponse->PH));
  strcat_P(LongMessage, (PGM_P)F("\",\"Weight\":\""));
  strcat(LongMessage, toText(ReservoirReceivedResponse->Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"WaterTemp\":\""));
  strcat(LongMessage, toText(ReservoirReceivedResponse->WaterTemperature));
  strcat_P(LongMessage, (PGM_P)F("\",\"AirTemp\":\""));
  strcat(LongMessage, toText(ReservoirReceivedResponse->AirTemperature));
  strcat_P(LongMessage, (PGM_P)F("\",\"Humidity\":\""));
  strcat(LongMessage, toText(ReservoirReceivedResponse->Humidity));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void ReservoirModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("Status")), toText_onlineStatus(OnlineStatus));
    WebServer.setArgString(getComponentName(F("PH")), toText(ReservoirReceivedResponse->PH));
    WebServer.setArgString(getComponentName(F("Weight")), toText_weight(ReservoirReceivedResponse->Weight));
    WebServer.setArgString(getComponentName(F("WTemp")), toText_temp(ReservoirReceivedResponse->WaterTemperature));
    WebServer.setArgString(getComponentName(F("ATemp")), toText_temp(ReservoirReceivedResponse->AirTemperature));
    WebServer.setArgString(getComponentName(F("Humi")), toText_percentage(ReservoirReceivedResponse->Humidity));
  }
}

void ReservoirModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  sendMessages();
}

void ReservoirModule_Web::sendMessages()
{
  /**
  * @brief Exchange messages with the Reservoir module
  */
  updateCommands();
  sendCommand(&ReservoirCommandToSend);                                                                                           //< Command - Response exchange
  while (sendCommand(&ReservoirGetNextResponse) < ReservoirMessages::ReservoirGetNext && millis() - LastResponseReceived < WirelessMessageTimeout) //< special Command, only exchange Response.
    ;
  if (Debug)
    logToSerials(F("Message exchange finished"), true, 3);
}

ReservoirMessages ReservoirModule_Web::sendCommand(void *CommandToSend)
{
  /**
  * @brief Send a single command and process the Response received in the Acknowledgement package
  */
  ReservoirMessages SequenceIDToSend = ((ReservoirCommonTemplate *)CommandToSend)->SequenceID;
  ReservoirMessages ReceivedSequenceID = NULL;
  if (Debug)
  {
    logToSerials(F("Sending SequenceID:"), false, 3);
    logToSerials(SequenceIDToSend, false, 1);
    logToSerials(F("-"), false, 1);
    logToSerials(toText_reservoirSequenceID(SequenceIDToSend), false, 1);
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
      case ReservoirMessages::ReservoirResponse1:
        memcpy(ReservoirReceivedResponse, ReceivedResponse, sizeof(struct ReservoirResponse));
        if (*Debug)
        {
          logToSerials(F("Module1:"), false, 4);
          logToSerials(ReservoirReceivedResponse -> Status, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(ReservoirReceivedResponse -> PH, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(ReservoirReceivedResponse -> Weight, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(ReservoirReceivedResponse -> WaterTemperature, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(ReservoirReceivedResponse -> AirTemperature, false, 1);
          logToSerials(F(","), false, 1);
          logToSerials(ReservoirReceivedResponse -> Humidity, true, 1);
        }
        break;      
      case ReservoirMessages::ReservoirGetNext:
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
    OnlineStatus = false;
  }
  return ReceivedSequenceID;
}

void ReservoirModule_Web::updateCommands()
{
  /**
   * @brief Updates the command sent to the remote Reservoir Module wirelessly
  */
  ReservoirCommandToSend.Time = now();
  ReservoirCommandToSend.Debug = *Debug;
  ReservoirCommandToSend.Metric = *Metric;
}