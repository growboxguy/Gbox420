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
struct HempyBucketResponse HempyBucket1ResponseToSend = {HempyMessages::HempyBucketResponse1};
struct HempyBucketResponse HempyBucket2ResponseToSend = {HempyMessages::HempyBucketResponse2};
struct HempyCommonTemplate HempyResetToSend = {HempyMessages::HempyReset}; ///< Special response signaling the end of a message exchange to the Transmitter

HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings &DefaultSettings) : Common(Name), Module(Name)
{
  SerialReportFrequency = DefaultSettings.SerialReportFrequency;
  SerialReportDate = DefaultSettings.SerialReportDate;
  SerialReportMemory = DefaultSettings.SerialReportMemory;
  SerialReportJSONFriendly = DefaultSettings.SerialReportJSONFriendly;
  SerialReportJSON = DefaultSettings.SerialReportJSON;
  SerialReportWireless = DefaultSettings.SerialReportWireless;
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
    //if (*Debug)
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
  reportToSerialTrigger();
  updateResponse();
}

void HempyModule::updateResponse()
{
  HempyBucket1ResponseToSend.HempyState = Bucket1->getState();
  HempyBucket1ResponseToSend.PumpState = Pump1->getState();
  HempyBucket1ResponseToSend.WeightB = WeightB1->getWeight(false);
  HempyBucket1ResponseToSend.WeightWR = WeightWR->getWeight(false);
  HempyBucket1ResponseToSend.DryWeight = Bucket1->getDryWeight();
  HempyBucket1ResponseToSend.WetWeight = Bucket1->getWetWeight();
  HempyBucket2ResponseToSend.HempyState = Bucket2->getState();
  HempyBucket2ResponseToSend.PumpState = Pump2->getState();
  HempyBucket2ResponseToSend.WeightB = WeightB2->getWeight(false);
  HempyBucket2ResponseToSend.DryWeight = Bucket2->getDryWeight();
  HempyBucket2ResponseToSend.WetWeight = Bucket2->getWetWeight();
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
  if (ReceivedSequenceID == HempyMessages::HempyBucketCommand2 && NextSequenceID == HempyMessages::HempyBucketResponse2) ///< Last real command-response exchange reached
  {
    LastMessageReached = true;
  }

  switch (ReceivedSequenceID)
  {
  case HempyMessages::HempyModuleCommand1:
    updateAckData(HempyMessages::HempyBucketResponse1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->Metric, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportFrequency, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportDate, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportMemory, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSON, false, 1);
      logToSerials(((HempyModuleCommand *)ReceivedCommand)->SerialReportWireless, true, 1);
    }
    setDebug(((HempyModuleCommand *)ReceivedCommand)->Debug);
    setMetric(((HempyModuleCommand *)ReceivedCommand)->Metric);
    setSerialReportingFrequency(((HempyModuleCommand *)ReceivedCommand)->SerialReportFrequency);
    setSerialReportDate(((HempyModuleCommand *)ReceivedCommand)->SerialReportDate);
    setSerialReportMemory(((HempyModuleCommand *)ReceivedCommand)->SerialReportMemory);
    setSerialReportJSONFriendly(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly);
    setSerialReportJSON(((HempyModuleCommand *)ReceivedCommand)->SerialReportJSON);
    setSerialReportWireless(((HempyModuleCommand *)ReceivedCommand)->SerialReportWireless);
    break;
  case HempyMessages::HempyBucketCommand1:
    updateAckData(HempyMessages::HempyBucketResponse2); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->Disable, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->StartWatering, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->StopWatering, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->TareWeightB, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->TareWeightDW, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->TareWeightWR, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->DryWeight, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->EvaporationTarget, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->DrainTargetWeight, false, 1);      
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->MaxWeight, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->DrainWaitTime, true, 1);
    }
    if (((HempyBucketCommand *)ReceivedCommand)->Disable && !HempyBucket1ResponseToSend.ConfirmDisable)
    {
      Bucket1->disable();
      HempyBucket1ResponseToSend.ConfirmDisable = true;
    }
    else
      HempyBucket1ResponseToSend.ConfirmDisable = false;
    if (((HempyBucketCommand *)ReceivedCommand)->StartWatering && !HempyBucket1ResponseToSend.ConfirmStartWatering)
    {
      Bucket1->startWatering();
      HempyBucket1ResponseToSend.ConfirmStartWatering = true;
    }
    else
      HempyBucket1ResponseToSend.ConfirmStartWatering = false;
    if (((HempyBucketCommand *)ReceivedCommand)->StopWatering && !HempyBucket1ResponseToSend.ConfirmStopWatering)
    {
      Bucket1->stopWatering();
      HempyBucket1ResponseToSend.ConfirmStopWatering = true;
    }
    else
      HempyBucket1ResponseToSend.ConfirmStopWatering = false;
    if (((HempyBucketCommand *)ReceivedCommand)->TareWeightB && !HempyBucket1ResponseToSend.ConfirmTareWeightB)
    {
      WeightB1->tareRequest();
      HempyBucket1ResponseToSend.ConfirmTareWeightB = true;
    }
    else
      HempyBucket1ResponseToSend.ConfirmTareWeightB = false;
    if (((HempyBucketCommand *)ReceivedCommand)->TareWeightDW && !HempyBucket1ResponseToSend.ConfirmTareWeightDW)
    {
      Bucket1->tareDryWetWeight();
      HempyBucket1ResponseToSend.ConfirmTareWeightDW = true;
    }
    else
      HempyBucket1ResponseToSend.ConfirmTareWeightDW = false;    

    Pump1->setSpeed(((HempyBucketCommand *)ReceivedCommand)->PumpSpeed);
    Pump1->setTimeOut(((HempyBucketCommand *)ReceivedCommand)->PumpTimeOut);
    Bucket1->setDryWeight(((HempyBucketCommand *)ReceivedCommand)->DryWeight);
    Bucket1->setMaxWeight(((HempyBucketCommand *)ReceivedCommand)->MaxWeight);
    Bucket1->setEvaporationTarget(((HempyBucketCommand *)ReceivedCommand)->EvaporationTarget);
    Bucket1->setDrainTargetWeight(((HempyBucketCommand *)ReceivedCommand)->DrainTargetWeight);
    Bucket1->setDrainWaitTime(((HempyBucketCommand *)ReceivedCommand)->DrainWaitTime);
    break;
  case HempyMessages::HempyBucketCommand2:
    updateAckData(HempyMessages::HempyModuleResponse1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->Disable, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->StartWatering, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->StopWatering, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->TareWeightB, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->TareWeightDW, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->DryWeight, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->EvaporationTarget, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->DrainTargetWeight, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->MaxWeight, false, 1);
      logToSerials(((HempyBucketCommand *)ReceivedCommand)->DrainWaitTime, true, 1);
    }
    if (((HempyBucketCommand *)ReceivedCommand)->Disable && !HempyBucket2ResponseToSend.ConfirmDisable)
    {
      Bucket2->disable();
      HempyBucket2ResponseToSend.ConfirmDisable = true;
    }
    else
    {
      HempyBucket2ResponseToSend.ConfirmDisable = false;
    }
    if (((HempyBucketCommand *)ReceivedCommand)->StartWatering && !HempyBucket2ResponseToSend.ConfirmStartWatering)
    {
      Bucket2->startWatering();
      HempyBucket2ResponseToSend.ConfirmStartWatering = true;
    }
    else
    {
      HempyBucket2ResponseToSend.ConfirmStartWatering = false;
    }
    if (((HempyBucketCommand *)ReceivedCommand)->StopWatering && !HempyBucket2ResponseToSend.ConfirmStopWatering)
    {
      Bucket2->stopWatering();
      HempyBucket2ResponseToSend.ConfirmStopWatering = true;
    }
    else
    {
      HempyBucket2ResponseToSend.ConfirmStopWatering = false;
    }
    if (((HempyBucketCommand *)ReceivedCommand)->TareWeightB && !HempyBucket2ResponseToSend.ConfirmTareWeightB)
    {
      WeightB2->tareRequest();
      HempyBucket2ResponseToSend.ConfirmTareWeightB = true;
    }
    else
    {
      HempyBucket2ResponseToSend.ConfirmTareWeightB = false;
    }
    if (((HempyBucketCommand *)ReceivedCommand)->TareWeightDW && !HempyBucket2ResponseToSend.ConfirmTareWeightDW)
    {
      Bucket2->tareDryWetWeight();
      HempyBucket2ResponseToSend.ConfirmTareWeightDW = true;
    }
    else
    {
      HempyBucket2ResponseToSend.ConfirmTareWeightDW = false;
    }
    Pump2->setTimeOut(((HempyBucketCommand *)ReceivedCommand)->PumpTimeOut);
    Pump2->setSpeed(((HempyBucketCommand *)ReceivedCommand)->PumpSpeed);
    Bucket2->setDryWeight(((HempyBucketCommand *)ReceivedCommand)->DryWeight);
    Bucket2->setMaxWeight(((HempyBucketCommand *)ReceivedCommand)->MaxWeight);
    Bucket2->setEvaporationTarget(((HempyBucketCommand *)ReceivedCommand)->EvaporationTarget);
    Bucket2->setDrainTargetWeight(((HempyBucketCommand *)ReceivedCommand)->DrainTargetWeight);
    Bucket2->setDrainWaitTime(((HempyBucketCommand *)ReceivedCommand)->DrainWaitTime);
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
  case HempyMessages::HempyBucketResponse1:
    Wireless.writeAckPayload(1, &HempyBucket1ResponseToSend, WirelessPayloadSize);
    break;
  case HempyMessages::HempyBucketResponse2:
    Wireless.writeAckPayload(1, &HempyBucket2ResponseToSend, WirelessPayloadSize);
    break;
  case HempyMessages::HempyReset: ///< HempyReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &HempyResetToSend, WirelessPayloadSize);
    break;
  default:
    Wireless.writeAckPayload(1, &HempyModuleResponse1ToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}