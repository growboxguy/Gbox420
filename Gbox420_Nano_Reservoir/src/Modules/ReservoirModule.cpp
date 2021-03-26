#include "ReservoirModule.h"
#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PHSensor.h"
#include "../Components/TDSSensor.h"
#include "../Components/WaterTempSensor.h"
#include "../Components/WeightSensor.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = ReservoirMessages::ReservoirModuleResponse1;
struct ReservoirModuleResponse ReservoirModuleResponse1ToSend = {ReservoirMessages::ReservoirModuleResponse1};
struct ReservoirResponse ReservoirResponse1ToSend = {ReservoirMessages::ReservoirResponse1};
struct ReservoirCommonTemplate ReservoirResetToSend = {ReservoirMessages::ReservoirReset}; ///< Special response signaling the end of a message exchange to the Transmitter

ReservoirModule::ReservoirModule(const __FlashStringHelper *Name, Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Module()
{
  ReportDate = &DefaultSettings->ReportDate;
  ReportMemory = &DefaultSettings->ReportMemory;
  ReportToText = &DefaultSettings->ReportToText;
  ReportToJSON = &DefaultSettings->ReportToJSON;
  logToSerials(F(""), true, 0);  //<Line break
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  WTemp1 = new WaterTempSensor(F("WTemp1"), this, &ModuleSettings->WTemp1);
  PHSen1 = new PHSensor(F("PHSen1"), this, &ModuleSettings->PHSen1);
  TDS1 = new TDSSensor(F("TDS1"), this, &ModuleSettings->TDS1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);
  //addToRefreshQueue_Minute(this);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
  addToLog(F("ReservoirModule initialized"), 0);
}

void ReservoirModule::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (NextSequenceID != ReservoirMessages::ReservoirModuleResponse1 && millis() - LastMessageReceived >= WirelessMessageTimeout)
  {                                                               ///< If there is a package exchange in progress
    NextSequenceID = ReservoirMessages::ReservoirModuleResponse1; ///< Reset back to the first response
    logToSerials(F("Message timeout"), true, 0);
    updateAckData();
  }
}

void ReservoirModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  runReport(*ReportToJSON,true, false);
  updateResponse();
}

void ReservoirModule::updateResponse()
{ ///< Updates the response sent to the Main module
  ReservoirResponse1ToSend.PH = PHSen1->getPH();
  ReservoirResponse1ToSend.TDS = TDS1->getTDS(true);
  ReservoirResponse1ToSend.Weight = Weight1->getWeight();
  ReservoirResponse1ToSend.WaterTemperature = WTemp1->getTemp();
  ReservoirResponse1ToSend.AirTemperature = DHT1->getTemp();
  ReservoirResponse1ToSend.Humidity = DHT1->getHumidity();
  updateAckData();
}

bool ReservoirModule::processCommand(void *ReceivedCommand)
{
  ReservoirMessages ReceivedSequenceID = ((ReservoirCommonTemplate *)ReceivedCommand)->SequenceID;
  LastMessageReceived = millis(); ///< Store current time
  //if (*Debug)
  {
    logToSerials(F("Received:"), false, 1);
    logToSerials(toText_reservoirSequenceID(ReceivedSequenceID), false, 1);
    logToSerials(F("- Sent:"), false, 1);
    logToSerials(toText_reservoirSequenceID(NextSequenceID), false, 1); ///< This is the pre-buffered response that was instantly sent when a command was received
    logToSerials(F(". Data:"), false, 1);
  }

  bool LastMessageReached = false;
  if (ReceivedSequenceID == ReservoirMessages::ReservoirCommand1 && NextSequenceID == ReservoirMessages::ReservoirResponse1) ///< Last real command-response exchange reached
  {
    LastMessageReached = true;
  }

  switch (ReceivedSequenceID)
  {
  case ReservoirMessages::ReservoirModuleCommand1:
    setDebug(((ReservoirModuleCommand *)ReceivedCommand)->Debug);
    setMetric(((ReservoirModuleCommand *)ReceivedCommand)->Metric);
    setReportDate(((ReservoirModuleCommand *)ReceivedCommand)->ReportDate);
    setReportMemory(((ReservoirModuleCommand *)ReceivedCommand)->ReportMemory);
    setReportToText(((ReservoirModuleCommand *)ReceivedCommand)->ReportToText);
    setReportToJSON(((ReservoirModuleCommand *)ReceivedCommand)->ReportToJSON);
    NextSequenceID = ReservoirMessages::ReservoirResponse1; // update the next Message that will be copied to the buffer
    //if (*Debug)
    {
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->Metric, false, 1);
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->ReportDate, false, 1);
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->ReportMemory, false, 1);
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->ReportToText, false, 1);
      logToSerials(((ReservoirModuleCommand *)ReceivedCommand)->ReportToJSON, true, 1);
    }
    break;
  case ReservoirMessages::ReservoirCommand1:
    if (((ReservoirCommand *)ReceivedCommand)->TareWeight)
      Weight1->triggerTare();
    NextSequenceID = ReservoirMessages::ReservoirReset; // update the next Message that will be copied to the buffer
    //if (*Debug)
    {
      logToSerials(((ReservoirCommand *)ReceivedCommand)->TareWeight, true, 1);
    }
    break;
  case ReservoirMessages::ReservoirReset:                         ///< Used to get all Responses that do not have a corresponding Command
    NextSequenceID = ReservoirMessages::ReservoirModuleResponse1; ///< Load the first response for the next message exchange
    logToSerials(F("-"), true, 0);
    break;
  default:
    //logToSerials(F("SequenceID unknown"), true, 2);
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    Wireless.flush_rx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    break;
  }
  updateAckData();              ///< Loads the next ACK that will be sent out
  saveSettings(ModuleSettings); ///< Store changes in EEPROM
  return LastMessageReached;
}

void ReservoirModule::updateAckData()
{
  // if (*Debug)
  // {
  //   logToSerials(F("Ack:"), false, 2);
  //   logToSerials(toText_reservoirSequenceID(NextSequenceID), true, 1);
  // }
  Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)

  switch (NextSequenceID) // based on the NextSeqenceID load the next response into the Acknowledgement buffer
  {
  case ReservoirMessages::ReservoirModuleResponse1:
    Wireless.writeAckPayload(1, &ReservoirModuleResponse1ToSend, WirelessPayloadSize);
    break;
  case ReservoirMessages::ReservoirResponse1:
    Wireless.writeAckPayload(1, &ReservoirResponse1ToSend, WirelessPayloadSize);
    break;
  case ReservoirMessages::ReservoirReset: ///< ReservoirReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &ReservoirResetToSend, WirelessPayloadSize);
    break;
  default:
    //logToSerials(F("Ack defaults loaded"), true, 3);
    Wireless.writeAckPayload(1, &ReservoirModuleResponse1ToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}