#include "ReservoirModule.h"
#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PHSensor.h"
#include "../Components/WaterTempSensor.h"
#include "../Components/WeightSensor.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = ReservoirMessages::ReservoirResponse1;
struct ReservoirResponse ReservoirResponse1ToSend = {ReservoirMessages::ReservoirResponse1};
struct ReservoirCommonTemplate ReservoirLastResponseToSend = {ReservoirMessages::ReservoirReset}; //< Special response signaling the end of a message exchange to the Transmitter

ReservoirModule::ReservoirModule(const __FlashStringHelper *Name, Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Module()
{
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  WTemp1 = new WaterTempSensor(F("WTemp1"), this, &ModuleSettings->WTemp1);
  PHSen1 = new PHSensor(F("PHSen1"), this, &ModuleSettings->PHSen1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);
  //addToRefreshQueue_Minute(this);
  //addToRefreshQueue_QuarterHour(this);
  logToSerials(Name, false, 0);
  logToSerials(F("- ReservoirModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("ReservoirModule initialized"), 0);
}

void ReservoirModule::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (NextSequenceID != ReservoirMessages::ReservoirResponse1 && millis() - LastMessageReceived >= WirelessMessageTimeout)
  {                                                         //< If there is a package exchange in progress
    NextSequenceID = ReservoirMessages::ReservoirResponse1; //< Reset back to the first response
    logToSerials(F("Timeout during message exchange, reseting to first response"), true, 0);
    updateAckData();
  }
}

void ReservoirModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  runReport();
  updateResponse();
}

void ReservoirModule::updateResponse()
{ ///<Updates the response sent to the Main module
  ReservoirResponse1ToSend.PH = PHSen1->getPH();
  ReservoirResponse1ToSend.Weight = Weight1->getWeight();
  ReservoirResponse1ToSend.WaterTemperature = WTemp1->getTemp();
  ReservoirResponse1ToSend.AirTemperature = DHT1->getTemp();
  ReservoirResponse1ToSend.Humidity = DHT1->getHumidity();
  updateAckData();
}

void ReservoirModule::processCommand(void *ReceivedCommand)
{
  ReservoirMessages ReceivedSequenceID = ((ReservoirCommonTemplate *)ReceivedCommand)->SequenceID;
  LastMessageReceived = millis(); ///< Store current time
  logToSerials(F("Received:"), false, 1);
  logToSerials(toText_reservoirSequenceID(ReceivedSequenceID), false, 1);
  logToSerials(F("- Sent:"), false, 1);
  logToSerials(toText_reservoirSequenceID(NextSequenceID), true, 1);

  switch (ReceivedSequenceID)
  {
  case ReservoirMessages::ReservoirCommand1:
    setDebug(((ReservoirCommand *)ReceivedCommand)->Debug);
    setMetric(((ReservoirCommand *)ReceivedCommand)->Metric);
    if (((ReservoirCommand *)ReceivedCommand)->TareWeight)
      Weight1->triggerTare();
    NextSequenceID = ReservoirMessages::ReservoirReset; // update the next Message that will be copied to the buffer
    if (*Debug)
    {
      logToSerials(F("Module:"), false, 2);
      logToSerials(((ReservoirCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((ReservoirCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((ReservoirCommand *)ReceivedCommand)->Metric, true, 1);
    }
    break;
  case ReservoirMessages::ReservoirReset:                   //< Used to get all Responses that do not have a corresponding Command
    NextSequenceID = ReservoirMessages::ReservoirResponse1; //< Load the first response for the next message exchange
    break;
  default:
    logToSerials(F("SequenceID unknown, ignoring message"), true, 2);
    break;
  }
  updateAckData();              //< Loads the next ACK that will be sent out
  saveSettings(ModuleSettings); //< Store changes in EEPROM
}

void ReservoirModule::updateAckData()
{ // so you can see that new data is being sent
  if (*Debug)
  {
    logToSerials(F("Updating Acknowledgement to:"), false, 2);
    logToSerials(toText_reservoirSequenceID(NextSequenceID), true, 1);
  }
  Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)

  switch (NextSequenceID) // based on the NextSeqenceID load the next response into the Acknowledgement buffer
  {
  case ReservoirMessages::ReservoirResponse1:
    Wireless.writeAckPayload(1, &ReservoirResponse1ToSend, WirelessPayloadSize);
    break;
  case ReservoirMessages::ReservoirReset: //< ReservoirReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &ReservoirLastResponseToSend, WirelessPayloadSize);
    break;
  default:
    logToSerials(F("Unknown next Sequence number, Ack defaults loaded"), true, 3);
    Wireless.writeAckPayload(1, &ReservoirResponse1ToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}