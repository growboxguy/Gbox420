/**@file*/
///< Supports monitoring an Aeroponics tote  - Without a pressure tank
///< Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "AeroModule_NoTank.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PressureSensor.h"
#include "../Components/PressurePump.h"
#include "../Components/Aeroponics_NoTank.h"
#include "../Components/WeightSensor.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = AeroMessages::AeroModuleResponse1;
struct AeroModuleResponse AeroModule1ResponseToSend = {AeroMessages::AeroModuleResponse1};
struct AeroResponse_P1 Aero1Response1ToSend = {AeroMessages::AeroResponse1};
struct AeroResponse_P2 Aero1Response2ToSend = {AeroMessages::AeroResponse2};
struct AeroCommonTemplate AeroResetToSend = {AeroMessages::AeroReset}; ///< Special response signaling the end of a message exchange to the Transmitter
unsigned long LastMessageSent = 0;                                     //When was the last message sent

AeroModule::AeroModule(const __FlashStringHelper *Name, Settings::AeroponicsModuleSettings *DefaultSettings) : Common(Name), Module(Name)
{
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  logToSerials(F(""), true, 0);                                   //<Line break
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  Pres1 = new PressureSensor(F("Pres1"), this, &ModuleSettings->Pres1);
  Pump1 = new PressurePump(F("Pump1"), this, &ModuleSettings->AeroPump1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  AeroNT1 = new Aeroponics_NoTank(F("AeroNT1"), this, &ModuleSettings->AeroNT1, Pres1, Pump1); ///< Use this without a pressure tank
  Aero1Response1ToSend.PressureTankPresent = false;
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);
  //addToRefreshQueue_Minute(this);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
  addToLog(F("AeroModule initialized"), 0);
}

void AeroModule::refresh_Sec()
{
  Common::refresh_Sec();
  if (NextSequenceID != AeroMessages::AeroModuleResponse1 && millis() - LastMessageReceived >= WirelessMessageTimeout)
  { ///< If there is a package exchange in progress
    if (Debug)
    {
      logToSerials(F("Message timeout"), true, 0);
    }
    Wireless.flush_tx();                              ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    updateAckData(AeroMessages::AeroModuleResponse1); ///< Reset back to the first response
  }
}

void AeroModule::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  runReport();
  updateResponse();
}

void AeroModule::processTimeCriticalStuff()
{
  AeroNT1->processTimeCriticalStuff();
}

bool AeroModule::processCommand(void *ReceivedCommand)
{
  AeroMessages ReceivedSequenceID = ((AeroCommonTemplate *)ReceivedCommand)->SequenceID;
  LastMessageReceived = millis(); ///< Store current time
  if (SerialReportWireless)
  {
    logToSerials(F("Received:"), false, 1);
    logToSerials(toText_aeroSequenceID(ReceivedSequenceID), false, 1);
    logToSerials(F("; Sent:"), false, 1);
    logToSerials(toText_aeroSequenceID(NextSequenceID), false, 1); ///< This is the pre-buffered response that was instantly sent when a command was received
    logToSerials(F("; Data:"), false, 1);
  }
  bool LastMessageReached = false;
  if (ReceivedSequenceID == AeroMessages::AeroCommand2 && NextSequenceID == AeroMessages::AeroResponse2) ///< Last real command-response exchange reached
  {
    LastMessageReached = true;
  }

  switch (ReceivedSequenceID)
  {
  case AeroMessages::AeroModuleCommand1:
    updateAckData(AeroMessages::AeroResponse1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->Metric, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->SerialReportDate, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->SerialReportMemory, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->SerialReportJSON, false, 1);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->SerialReportWireless, true, 1);
    }
    setDebug(((AeroModuleCommand *)ReceivedCommand)->Debug);
    setMetric(((AeroModuleCommand *)ReceivedCommand)->Metric);
    setSerialReportDate(((AeroModuleCommand *)ReceivedCommand)->SerialReportDate);
    setSerialReportMemory(((AeroModuleCommand *)ReceivedCommand)->SerialReportMemory);
    setSerialReportJSONFriendly(((AeroModuleCommand *)ReceivedCommand)->SerialReportJSONFriendly);
    setSerialReportJSON(((AeroModuleCommand *)ReceivedCommand)->SerialReportJSON);
    setSerialReportWireless(((AeroModuleCommand *)ReceivedCommand)->SerialReportWireless);
    break;
  case AeroMessages::AeroCommand1:
    updateAckData(AeroMessages::AeroResponse2); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayEnabled, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayDisabled, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayNow, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayOff, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->DayMode, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->Duration, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->DayInterval, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->NightInterval, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->MinPressure, false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->MaxPressure, true, 1);
    }
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayEnabled && !Aero1Response1ToSend.ConfirmSprayEnabled)
    {
      AeroNT1->setSprayOnOff(true);
      Aero1Response1ToSend.ConfirmSprayEnabled = true;
    }
    else
      Aero1Response1ToSend.ConfirmSprayEnabled = false;
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayDisabled && !Aero1Response1ToSend.ConfirmSprayDisabled)
    {
      AeroNT1->setSprayOnOff(false);
      Aero1Response1ToSend.ConfirmSprayDisabled = true;
    }
    else
      Aero1Response1ToSend.ConfirmSprayDisabled = false;
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayNow && !Aero1Response1ToSend.ConfirmSprayNow)
    {
      AeroNT1->sprayNow(true);
      Aero1Response1ToSend.ConfirmSprayNow = true;
    }
    else
      Aero1Response1ToSend.ConfirmSprayNow = false;
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayOff && !Aero1Response1ToSend.ConfirmSprayOff)
    {
      AeroNT1->sprayOff();
      Aero1Response1ToSend.ConfirmSprayOff = true;
    }
    else
      Aero1Response1ToSend.ConfirmSprayOff = false;

    AeroNT1->setDayMode(((AeroCommand_P1 *)ReceivedCommand)->DayMode);
    AeroNT1->setDuration(((AeroCommand_P1 *)ReceivedCommand)->Duration);
    AeroNT1->setDayInterval(((AeroCommand_P1 *)ReceivedCommand)->DayInterval);
    AeroNT1->setNightInterval(((AeroCommand_P1 *)ReceivedCommand)->NightInterval);
    AeroNT1->setMaxPressure(((AeroCommand_P1 *)ReceivedCommand)->MaxPressure);
    break;
  case AeroMessages::AeroCommand2:
    updateAckData(AeroMessages::AeroModuleResponse1); // update the next Message that will be copied to the buffer
    if (SerialReportWireless)
    {
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->MixReservoir, false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->TareWeight, true, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpOn, false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpOff, false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpDisable, false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpPrimingTime, false, 1);
    }
    if (((AeroCommand_P2 *)ReceivedCommand)->MixReservoir && !Aero1Response2ToSend.ConfirmMixReservoir)
    {
      AeroNT1->Pump->startMixing();
      Aero1Response2ToSend.ConfirmMixReservoir = true;
    }
    else
      Aero1Response2ToSend.ConfirmMixReservoir = false;
    if (((AeroCommand_P2 *)ReceivedCommand)->TareWeight && !Aero1Response2ToSend.ConfirmTareWeight)
    {
      Weight1->tareRequest();
      Aero1Response2ToSend.ConfirmTareWeight = true;
    }
    else
      Aero1Response2ToSend.ConfirmTareWeight = false;
    if (((AeroCommand_P2 *)ReceivedCommand)->PumpOn && !Aero1Response2ToSend.ConfirmPumpOn)
    {
      AeroNT1->sprayNow(true);
      Aero1Response2ToSend.ConfirmPumpOn = true;
    }
    else
      Aero1Response2ToSend.ConfirmPumpOn = false;
    if (((AeroCommand_P2 *)ReceivedCommand)->PumpOff && !Aero1Response2ToSend.ConfirmPumpOff)
    {
      AeroNT1->Pump->stopPump();
      Aero1Response2ToSend.ConfirmPumpOff = true;
    }
    else
      Aero1Response2ToSend.ConfirmPumpOff = false;
    if (((AeroCommand_P2 *)ReceivedCommand)->PumpDisable && !Aero1Response2ToSend.ConfirmPumpDisable)
    {
      AeroNT1->Pump->disablePump();
      Aero1Response2ToSend.ConfirmPumpDisable = true;
    }
    else
      Aero1Response2ToSend.ConfirmPumpDisable = false;

    AeroNT1->Pump->setSpeed(((AeroCommand_P2 *)ReceivedCommand)->PumpSpeed);
    AeroNT1->Pump->setTimeOut(((AeroCommand_P2 *)ReceivedCommand)->PumpTimeOut);
    AeroNT1->Pump->setPrimingTime(((AeroCommand_P2 *)ReceivedCommand)->PumpPrimingTime);
    break;
  case AeroMessages::AeroReset:                       ///< Used to get all Responses that do not have a corresponding Command
    updateAckData(AeroMessages::AeroModuleResponse1); ///< Load the first response for the next message exchange
    if (SerialReportWireless)
    {
      logToSerials(F("-"), true, 1);
    }
    break;
  default:
    if (SerialReportWireless)
    {
      logToSerials(F("SequenceID unknown"), true, 1);
    }
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    Wireless.flush_rx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    break;
  }
  saveSettings(ModuleSettings);
  return LastMessageReached;
}

void AeroModule::updateResponse()
{
  Aero1Response1ToSend.AeroState = (int)AeroNT1->getState();
  Aero1Response1ToSend.SprayEnabled = AeroNT1->getSprayEnabled();
  Aero1Response1ToSend.Pressure = AeroNT1->getPressure();
  Aero1Response1ToSend.PumpState = AeroNT1->Pump->getState();
  Aero1Response1ToSend.LastSprayPressure = AeroNT1->getLastSprayPressure();
  Aero1Response1ToSend.Weight = Weight1->getWeight();
}

void AeroModule::updateAckData(AeroMessages NewSequenceID)
{
  //Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
  NextSequenceID = NewSequenceID; // update the next Message that will be copied to the buffer

  switch (NextSequenceID) // based on the NextSeqenceID load the next response into the Acknowledgement buffer
  {
  case AeroMessages::AeroModuleResponse1:
    Wireless.writeAckPayload(1, &AeroModule1ResponseToSend, WirelessPayloadSize);
    break;
  case AeroMessages::AeroResponse1:
    Wireless.writeAckPayload(1, &Aero1Response1ToSend, WirelessPayloadSize);
    break;
  case AeroMessages::AeroResponse2:
    Wireless.writeAckPayload(1, &Aero1Response2ToSend, WirelessPayloadSize);
    break;
  case AeroMessages::AeroReset: ///< AeroReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &AeroResetToSend, WirelessPayloadSize);
    break;
  default:
    Wireless.writeAckPayload(1, &AeroModule1ResponseToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}