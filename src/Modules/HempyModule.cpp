/**@file*/
///< Supports monitoring two Hempy buckets
///< Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "HempyModule.h"
#include "../Components/Sound.h"
#include "../Components/WeightSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/HempyBucket.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = HempyMessages::HempyModuleResponse1;
struct HempyModuleResponse HempyModuleResponse1ToSend = {HempyMessages::HempyModuleResponse1};
struct HempyBucketResponse1 HempyBucket1Response1ToSend = {HempyMessages::HempyBucket1Response1};
struct HempyBucketResponse1 HempyBucket2Response1ToSend = {HempyMessages::HempyBucket2Response1};
struct HempyCommonTemplate HempyResetToSend = {HempyMessages::HempyReset}; ///< Special response signaling the end of a message exchange to the Transmitter

HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings &DefaultSettings) : Common(Name), Module(Name)
{
  SerialReportDate = &DefaultSettings.SerialReportDate;
  SerialReportMemory = &DefaultSettings.SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings.SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings.SerialReportJSON;
  SerialReportWireless = &DefaultSettings.SerialReportWireless;
  logToSerials(F(""), true, 0);                                   // line break
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  WeightB1 = new WeightSensor(F("WeightB1"), this, &ModuleSettings->WeightB1);
  WeightB2 = new WeightSensor(F("WeightB2"), this, &ModuleSettings->WeightB2);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->HempyPump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->HempyPump2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, ModuleSettings->Bucket1, *WeightB1,*Pump1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, ModuleSettings->Bucket2, *WeightB2,*Pump2);
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);
  //addToRefreshQueue_Minute(this);
  logToSerials(F("Refreshing"), true, 0);
  runAll();
  addToLog(F("HempyModule initialized"), 0);
}

void HempyModule::refresh_Sec()
{
  Common::refresh_Sec();
  if (NextSequenceID != HempyMessages::HempyModuleResponse1 && millis() - LastMessageReceived >= WirelessMessageTimeout)
  { ///< If there is a package exchange in progress, but a followup command was not received within the timeout
    //if (Debug)
    //{
    // logToSerials(F("Message timeout"), true, 0);
    //}
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    updateAckData(HempyMessages::HempyModuleResponse1);
  }
}

void HempyModule::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  runReport();
  updateResponse();
}

void HempyModule::updateResponse()
{
  HempyBucket1Response1ToSend.HempyState = Bucket1->getState();
  HempyBucket1Response1ToSend.PumpState = Pump1->getState();
  HempyBucket1Response1ToSend.WeightB = WeightB1->getWeight();
  HempyBucket1Response1ToSend.DryWeight = Bucket1->getDryWeight();
  HempyBucket1Response1ToSend.WetWeight = Bucket1->getWetWeight();
  HempyBucket2Response1ToSend.HempyState = Bucket2->getState();
  HempyBucket2Response1ToSend.PumpState = Pump2->getState();
  HempyBucket2Response1ToSend.WeightB = WeightB2->getWeight();
  HempyBucket2Response1ToSend.DryWeight = Bucket2->getDryWeight();
  HempyBucket2Response1ToSend.WetWeight = Bucket2->getWetWeight();
}

bool HempyModule::processCommand(void *ReceivedCommand)
{
  HempyMessages ReceivedSequenceID = ((HempyCommonTemplate *)ReceivedCommand)->SequenceID;
  LastMessageReceived = millis(); ///< Store current time
  if (SerialReportWireless)
  {
    logToSerials(F("Received:"), false, 1);
    logToSerials(toText_hempySequenceID(ReceivedSequenceID), false, 1);
    logToSerials(F("; Sent:"), false, 1);
    logToSerials(toText_hempySequenceID(NextSequenceID), false, 1); ///< This is the pre-buffered response that was instantly sent when a command was received
    logToSerials(F("; Data:"), false, 1);
  }
  bool LastMessageReached = false;
  if (ReceivedSequenceID == HempyMessages::HempyBucket2Command1 && NextSequenceID == HempyMessages::HempyBucket2Response1) ///< Last real command-response exchange reached
  {
    LastMessageReached = true;
  }

  switch (ReceivedSequenceID)
  {
  case HempyMessages::HempyModuleCommand1:
    updateAckData(HempyMessages::HempyBucket1Response1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->Metric, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportDate, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportMemory, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSON, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportWireless, true, 1);
    }
    setDebug(((HempyModuleCommand *)ReceivedCommand)->Debug);
    setMetric(((HempyModuleCommand *)ReceivedCommand)->Metric);
    setSerialReportDate(((HempyModuleCommand *)ReceivedCommand)->SerialReportDate);
    setSerialReportMemory(((HempyModuleCommand *)ReceivedCommand)->SerialReportMemory);
    setSerialReportJSONFriendly(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly);
    setSerialReportJSON(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSON);
    setSerialReportWireless(((HempyModuleCommand *)ReceivedCommand)->SerialReportWireless);
    break;
  case HempyMessages::HempyBucket1Command1:
    updateAckData(HempyMessages::HempyBucket2Response1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->Disable, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->StartWatering, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->StopWatering, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->TareWeightB, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->TareWeightDW, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->StartWeight, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->EvaporationTarget, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->DrainTargetWeight, false, 1);      
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->MaxWeight, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->DrainWaitTime, true, 1);
    }
    if (((HempyBucketCommand1 *)ReceivedCommand)->Disable && !HempyBucket1Response1ToSend.ConfirmDisable)
    {
      Bucket1->disable();
      HempyBucket1Response1ToSend.ConfirmDisable = true;
    }
    else
      HempyBucket1Response1ToSend.ConfirmDisable = false;
    if (((HempyBucketCommand1 *)ReceivedCommand)->StartWatering && !HempyBucket1Response1ToSend.ConfirmStartWatering)
    {
      Bucket1->startWatering();
      HempyBucket1Response1ToSend.ConfirmStartWatering = true;
    }
    else
      HempyBucket1Response1ToSend.ConfirmStartWatering = false;
    if (((HempyBucketCommand1 *)ReceivedCommand)->StopWatering && !HempyBucket1Response1ToSend.ConfirmStopWatering)
    {
      Bucket1->stopWatering();
      HempyBucket1Response1ToSend.ConfirmStopWatering = true;
    }
    else
      HempyBucket1Response1ToSend.ConfirmStopWatering = false;
    if (((HempyBucketCommand1 *)ReceivedCommand)->TareWeightB && !HempyBucket1Response1ToSend.ConfirmTareWeightB)
    {
      WeightB1->tareRequest();
      HempyBucket1Response1ToSend.ConfirmTareWeightB = true;
    }
    else
      HempyBucket1Response1ToSend.ConfirmTareWeightB = false;
    if (((HempyBucketCommand1 *)ReceivedCommand)->TareWeightDW && !HempyBucket1Response1ToSend.ConfirmTareWeightDW)
    {
      Bucket1->tareDryWetWeight();
      HempyBucket1Response1ToSend.ConfirmTareWeightDW = true;
    }
    else
      HempyBucket1Response1ToSend.ConfirmTareWeightDW = false;    

    Pump1->setSpeed(((HempyBucketCommand1 *)ReceivedCommand)->PumpSpeed);
    Pump1->setTimeOut(((HempyBucketCommand1 *)ReceivedCommand)->PumpTimeOut);
    Bucket1->setStartWeight(((HempyBucketCommand1 *)ReceivedCommand)->StartWeight);
    Bucket1->setMaxWeight(((HempyBucketCommand1 *)ReceivedCommand)->MaxWeight);
    Bucket1->setEvaporationTarget(((HempyBucketCommand1 *)ReceivedCommand)->EvaporationTarget);
    Bucket1->setDrainTargetWeight(((HempyBucketCommand1 *)ReceivedCommand)->DrainTargetWeight);
    Bucket1->setDrainWaitTime(((HempyBucketCommand1 *)ReceivedCommand)->DrainWaitTime);
    break;
  case HempyMessages::HempyBucket2Command1:
    updateAckData(HempyMessages::HempyModuleResponse1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->Disable, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->StartWatering, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->StopWatering, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->TareWeightB, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->TareWeightDW, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->StartWeight, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->EvaporationTarget, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->DrainTargetWeight, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->MaxWeight, false, 1);
      logToSerials(((HempyBucketCommand1 *)ReceivedCommand)->DrainWaitTime, true, 1);
    }
    if (((HempyBucketCommand1 *)ReceivedCommand)->Disable && !HempyBucket2Response1ToSend.ConfirmDisable)
    {
      Bucket2->disable();
      HempyBucket2Response1ToSend.ConfirmDisable = true;
    }
    else
    {
      HempyBucket2Response1ToSend.ConfirmDisable = false;
    }
    if (((HempyBucketCommand1 *)ReceivedCommand)->StartWatering && !HempyBucket2Response1ToSend.ConfirmStartWatering)
    {
      Bucket2->startWatering();
      HempyBucket2Response1ToSend.ConfirmStartWatering = true;
    }
    else
    {
      HempyBucket2Response1ToSend.ConfirmStartWatering = false;
    }
    if (((HempyBucketCommand1 *)ReceivedCommand)->StopWatering && !HempyBucket2Response1ToSend.ConfirmStopWatering)
    {
      Bucket2->stopWatering();
      HempyBucket2Response1ToSend.ConfirmStopWatering = true;
    }
    else
    {
      HempyBucket2Response1ToSend.ConfirmStopWatering = false;
    }
    if (((HempyBucketCommand1 *)ReceivedCommand)->TareWeightB && !HempyBucket2Response1ToSend.ConfirmTareWeightB)
    {
      WeightB2->tareRequest();
      HempyBucket2Response1ToSend.ConfirmTareWeightB = true;
    }
    else
    {
      HempyBucket2Response1ToSend.ConfirmTareWeightB = false;
    }
    if (((HempyBucketCommand1 *)ReceivedCommand)->TareWeightDW && !HempyBucket2Response1ToSend.ConfirmTareWeightDW)
    {
      Bucket2->tareDryWetWeight();
      HempyBucket2Response1ToSend.ConfirmTareWeightDW = true;
    }
    else
    {
      HempyBucket2Response1ToSend.ConfirmTareWeightDW = false;
    }
    Pump2->setTimeOut(((HempyBucketCommand1 *)ReceivedCommand)->PumpTimeOut);
    Pump2->setSpeed(((HempyBucketCommand1 *)ReceivedCommand)->PumpSpeed);
    Bucket2->setStartWeight(((HempyBucketCommand1 *)ReceivedCommand)->StartWeight);
    Bucket2->setMaxWeight(((HempyBucketCommand1 *)ReceivedCommand)->MaxWeight);
    Bucket2->setEvaporationTarget(((HempyBucketCommand1 *)ReceivedCommand)->EvaporationTarget);
    Bucket2->setDrainTargetWeight(((HempyBucketCommand1 *)ReceivedCommand)->DrainTargetWeight);
    Bucket2->setDrainWaitTime(((HempyBucketCommand1 *)ReceivedCommand)->DrainWaitTime);
    break;
  case HempyMessages::HempyReset:                       ///< Used to get all Responses that do not have a corresponding Command
    updateAckData(HempyMessages::HempyModuleResponse1); ///< Load the first response for the next message exchange
    if (SerialReportWireless)
    {
      logToSerials(F("-"), true, 1);
    }
    break;
  default:
    //if (SerialReportWireless)
    //{
    // logToSerials(F("SequenceID unknown"), true, 1);
    //}
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    Wireless.flush_rx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    break;
  }
  saveSettings(ModuleSettings); ///< Store changes in EEPROM
  return LastMessageReached;
}

void HempyModule::updateAckData(HempyMessages NewSequenceID)
{
  //Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
  NextSequenceID = NewSequenceID; // update the next Message that will be copied to the buffer

  switch (NextSequenceID) // based on the NextSeqenceID load the next response into the Acknowledgement buffer
  {
  case HempyMessages::HempyModuleResponse1:
    Wireless.writeAckPayload(1, &HempyModuleResponse1ToSend, WirelessPayloadSize);
    break;
  case HempyMessages::HempyBucket1Response1:
    Wireless.writeAckPayload(1, &HempyBucket1Response1ToSend, WirelessPayloadSize);
    break;
  case HempyMessages::HempyBucket2Response1:
    Wireless.writeAckPayload(1, &HempyBucket2Response1ToSend, WirelessPayloadSize);
    break;
  case HempyMessages::HempyReset: ///< HempyReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &HempyResetToSend, WirelessPayloadSize);
    break;
  default:
    Wireless.writeAckPayload(1, &HempyModuleResponse1ToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}