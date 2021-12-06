/**@file*/
///< Supports speed and direction controlling an AC Induction Motor
///< Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "ACMotorModule.h"
#include "../Components/Sound.h"
#include "../Components/ACMotor.h"

///< Variables used during wireless communication
//WIRELESS DISBLED// 
/*
uint8_t NextSequenceID = ACMotorMessages::ACMotorModuleResponse1;
struct ACMotorModuleResponse ACMotorModuleResponse1ToSend = {ACMotorMessages::ACMotorModuleResponse1};
struct ACMotorResponse ACMotor1ResponseToSend = {ACMotorMessages::ACMotorResponse1};
struct ACMotorCommonTemplate ACMotorResetToSend = {ACMotorMessages::ACMotorReset}; ///< Special response signaling the end of a message exchange to the Transmitter
*/

ACMotorModule::ACMotorModule(const __FlashStringHelper *Name, Settings::ACMotorModuleSettings *DefaultSettings) : Common(Name), Module(Name)
{
  SerialReportFrequency = &DefaultSettings->SerialReportFrequency;
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  logToSerials(F(""), true, 0);                                   // line break
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  Motor1 = new ACMotor(F("Motor1"), this, &ModuleSettings->Motor1);
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);
  //addToRefreshQueue_Minute(this);
  logToSerials(F("Refreshing"), true, 0);
  runAll();
  addToLog(F("ACMotorModule initialized"), 0);
}

void ACMotorModule::processTimeCriticalStuff()
{
  Motor1->processTimeCriticalStuff();
}

void ACMotorModule::refresh_Sec()
{
  Common::refresh_Sec();
  //WIRELESS DISBLED// 
  /*
  if (NextSequenceID != ACMotorMessages::ACMotorModuleResponse1 && millis() - LastMessageReceived >= WirelessMessageTimeout)
  { ///< If there is a package exchange in progress, but a followup command was not received within the timeout
    //if (*Debug)
    //{
    // logToSerials(F("Message timeout"), true, 0);
    //}
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    updateAckData(ACMotorMessages::ACMotorModuleResponse1);
  }
  */
}

void ACMotorModule::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  reportToSerialTrigger();
  //WIRELESS DISBLED// updateResponse();
}

//WIRELESS DISBLED// 
/*

void ACMotorModule::updateResponse()
{
  ACMotor1ResponseToSend.RPM = Motor1->getRPM();
  ACMotor1ResponseToSend.ACMotorState = Motor1->getState();
}

bool ACMotorModule::processCommand(void *ReceivedCommand)
{
  ACMotorMessages ReceivedSequenceID = ((ACMotorCommonTemplate *)ReceivedCommand)->SequenceID;
  LastMessageReceived = millis(); ///< Store current time
  if (*SerialReportWireless)
  {
    logToSerials(F("Received:"), false, 1);
    logToSerials(toText_ACMotorSequenceID(ReceivedSequenceID), false, 1);
    logToSerials(F("; Sent:"), false, 1);
    logToSerials(toText_ACMotorSequenceID(NextSequenceID), false, 1); ///< This is the pre-buffered response that was instantly sent when a command was received
    logToSerials(F("; Data:"), false, 1);
  }
  bool LastMessageReached = false;
  if (ReceivedSequenceID == ACMotorMessages::ACMotorCommand1 && NextSequenceID == ACMotorMessages::ACMotorResponse1) ///< Last real command-response exchange reached
  {
    LastMessageReached = true;
  }

  switch (ReceivedSequenceID)
  {
  case ACMotorMessages::ACMotorModuleCommand1:
    updateAckData(ACMotorMessages::ACMotorResponse1); // update the next Message that will be copied to the buffer
    if (*SerialReportWireless)
    {
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->Metric, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportFrequency, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportDate, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportMemory, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportJSON, false, 1);
      logToSerials(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportWireless, true, 1);
    }
    setDebug(((ACMotorModuleCommand *)ReceivedCommand)->Debug);
    setMetric(((ACMotorModuleCommand *)ReceivedCommand)->Metric);
    setSerialReportingFrequency(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportFrequency);
    setSerialReportDate(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportDate);
    setSerialReportMemory(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportMemory);
    setSerialReportJSONFriendly(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly);
    setSerialReportJSON(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportJSON);
    setSerialReportWireless(((ACMotorModuleCommand *)ReceivedCommand)->SerialReportWireless);
    break;

  case ACMotorMessages::ACMotorCommand1:
    updateAckData(ACMotorMessages::ACMotorReset); // update the next Message that will be copied to the buffer
    if (*SerialReportWireless)
    {
      logToSerials(((ACMotorCommand *)ReceivedCommand)->Forward, false, 1);
      logToSerials(((ACMotorCommand *)ReceivedCommand)->Backward, false, 1);
      logToSerials(((ACMotorCommand *)ReceivedCommand)->Stop, false, 1);
      logToSerials(((ACMotorCommand *)ReceivedCommand)->Speed, true, 1);
    }

    if (((ACMotorCommand *)ReceivedCommand)->Forward && !ACMotor1ResponseToSend.ConfirmForward)
    {
      Motor1->forward();
      ACMotor1ResponseToSend.ConfirmForward = true;
    }
    else
      ACMotor1ResponseToSend.ConfirmForward = false;
    if (((ACMotorCommand *)ReceivedCommand)->Backward && !ACMotor1ResponseToSend.ConfirmBackward)
    {
      Motor1->backward();
      ACMotor1ResponseToSend.ConfirmBackward = true;
    }
    else
      ACMotor1ResponseToSend.ConfirmBackward = false;
    if (((ACMotorCommand *)ReceivedCommand)->Stop && !ACMotor1ResponseToSend.ConfirmStop)
    {
      Motor1->stop();
      ACMotor1ResponseToSend.ConfirmStop = true;
    }
    else
      ACMotor1ResponseToSend.ConfirmStop = false;

    Motor1->setSpeed(((ACMotorCommand *)ReceivedCommand)->Speed);
    break;

  case ACMotorMessages::ACMotorReset:                       ///< Used to get all Responses that do not have a corresponding Command
    updateAckData(ACMotorMessages::ACMotorModuleResponse1); ///< Load the first response for the next message exchange
    if (*SerialReportWireless)
    {
      logToSerials(F("-"), true, 1);
    }
    break;
  default:
    //if (*SerialReportWireless)
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

void ACMotorModule::updateAckData(ACMotorMessages NewSequenceID)
{
  //Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
  NextSequenceID = NewSequenceID; // update the next Message that will be copied to the buffer

  switch (NextSequenceID) // based on the NextSeqenceID load the next response into the Acknowledgement buffer
  {
  case ACMotorMessages::ACMotorModuleResponse1:
    Wireless.writeAckPayload(1, &ACMotorModuleResponse1ToSend, WirelessPayloadSize);
    break;
  case ACMotorMessages::ACMotorResponse1:
    Wireless.writeAckPayload(1, &ACMotor1ResponseToSend, WirelessPayloadSize);
    break;
  case ACMotorMessages::ACMotorReset: ///< ACMotorReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &ACMotorResetToSend, WirelessPayloadSize);
    break;
  default:
    Wireless.writeAckPayload(1, &ACMotorModuleResponse1ToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}
*/