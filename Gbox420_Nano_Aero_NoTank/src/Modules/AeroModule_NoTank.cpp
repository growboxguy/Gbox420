/**@file*/
///< Supports monitoring an Aeroponics tote  - Without a pressure tank
///< Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "AeroModule_NoTank.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PressureSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/Aeroponics_NoTank.h"
#include "../Components/WeightSensor.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = AeroMessages::AeroModuleResponse1;
struct AeroModuleResponse AeroModule1ResponseToSend = {AeroMessages::AeroModuleResponse1};
struct AeroResponse_P1 Aero1Response1ToSend = {AeroMessages::AeroResponse1};
struct AeroResponse_P2 Aero1Response2ToSend = {AeroMessages::AeroResponse2};
struct AeroCommonTemplate AeroResetToSend = {AeroMessages::AeroReset}; ///< Special response signaling the end of a message exchange to the Transmitter
unsigned long LastMessageSent = 0;                                     //When was the last message sent

AeroModule::AeroModule(const __FlashStringHelper *Name) : Common(Name), Module()
{
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  Pres1 = new PressureSensor(F("Pres1"), this, &ModuleSettings->Pres1);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->AeroPump1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  AeroNT1 = new Aeroponics_NoTank(F("AeroNT1"), this, &ModuleSettings->AeroNT1_Common, Pres1, Pump1); ///< Use this without a pressure tank
  Aero1Response1ToSend.PressureTankPresent = false;
  addToRefreshQueue_Sec(this);
  addToRefreshQueue_FiveSec(this);
  //addToRefreshQueue_Minute(this);
  //addToRefreshQueue_QuarterHour(this);
  logToSerials(Name, false, 0);
  logToSerials(F("- AeroModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("AeroModule initialized"), 0);
}

void AeroModule::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
  if (NextSequenceID != AeroMessages::AeroModuleResponse1 && millis() - LastMessageReceived >= WirelessMessageTimeout)
  {                                                     ///< If there is a package exchange in progress
    NextSequenceID = AeroMessages::AeroModuleResponse1; ///< Reset back to the first response
    logToSerials(F("Message exchange timeout"), true, 0);
    updateAckData();
  }
}

void AeroModule::refresh_FiveSec()
{
  if (*Debug)
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
  logToSerials(F("Received:"), false, 1);
  logToSerials(toText_aeroSequenceID(ReceivedSequenceID), false, 1);
  logToSerials(F("- Sent:"), false, 1);
  logToSerials(toText_aeroSequenceID(NextSequenceID), true, 1); ///< This is the pre-buffered response that was instantly sent when a command was received

  bool LastMessageReached = false;
  if (ReceivedSequenceID == AeroMessages::AeroCommand2 && NextSequenceID == AeroMessages::AeroResponse2) ///< Last real command-response exchange reached
  {
    LastMessageReached = true;
  }

  switch (ReceivedSequenceID)
  {
  case AeroMessages::AeroModuleCommand1:
    setDebug(((AeroModuleCommand *)ReceivedCommand)->Debug);
    setMetric(((AeroModuleCommand *)ReceivedCommand)->Metric);
    NextSequenceID = AeroMessages::AeroResponse1; // update the next Message that will be copied to the buffer
    if (*Debug)
    {
      logToSerials(F("Module:"), false, 2);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->Time, false, 1);
      logToSerials(F(","), false, 0);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->Debug, false, 1);
      logToSerials(F(","), false, 0);
      logToSerials(((AeroModuleCommand *)ReceivedCommand)->Metric, true, 1);
    }
    break;
  case AeroMessages::AeroCommand1:
    NextSequenceID = AeroMessages::AeroResponse2; // update the next Message that will be copied to the buffer
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayEnabled)
      AeroNT1->setSprayOnOff(true);
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayDisabled)
      AeroNT1->setSprayOnOff(false);
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayNow)
      AeroNT1->sprayNow(true);
    if (((AeroCommand_P1 *)ReceivedCommand)->SprayOff)
      AeroNT1->sprayOff();
    AeroNT1->setDayMode(((AeroCommand_P1 *)ReceivedCommand)->DayMode);
    AeroNT1->setDuration(((AeroCommand_P1 *)ReceivedCommand)->Duration);
    AeroNT1->setDayInterval(((AeroCommand_P1 *)ReceivedCommand)->DayInterval);
    AeroNT1->setNightInterval(((AeroCommand_P1 *)ReceivedCommand)->NightInterval);
    AeroNT1->setMaxPressure(((AeroCommand_P1 *)ReceivedCommand)->MaxPressure);

    if (*Debug)
    {
      logToSerials(F("Aero1:"), false, 2);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayEnabled, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayDisabled, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayNow, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->SprayOff, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->DayMode, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->Duration, false, 1);
      logToSerials(F(";"), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->DayInterval, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->NightInterval, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->MinPressure, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P1 *)ReceivedCommand)->MaxPressure, true, 1);
    }
    break;
  case AeroMessages::AeroCommand2:
    NextSequenceID = AeroMessages::AeroReset; // update the next Message that will be copied to the buffer
    if (((AeroCommand_P2 *)ReceivedCommand)->PumpOn)
      AeroNT1->lockPumpOn();
    if (((AeroCommand_P2 *)ReceivedCommand)->PumpOff)
      AeroNT1->Pump->stopPump();
    if (((AeroCommand_P2 *)ReceivedCommand)->PumpDisable)
      AeroNT1->Pump->disablePump();
    AeroNT1->Pump->setSpeed(((AeroCommand_P2 *)ReceivedCommand)->PumpSpeed);
    AeroNT1->Pump->setPumpTimeOut(((AeroCommand_P2 *)ReceivedCommand)->PumpTimeOut);
    AeroNT1->Pump->setPrimingTime(((AeroCommand_P2 *)ReceivedCommand)->PumpPrimingTime);
    if (((AeroCommand_P2 *)ReceivedCommand)->MixReservoir)
      AeroNT1->Pump->startMixing();
    if (((AeroCommand_P2 *)ReceivedCommand)->TareWeight)
      Weight1->triggerTare();

    if (*Debug)
    {
      logToSerials(F("Aero1:"), false, 2);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpOn, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpOff, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpDisable, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->PumpPrimingTime, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->MixReservoir, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand_P2 *)ReceivedCommand)->TareWeight, true, 1);
    }
    break;
  case AeroMessages::AeroReset:                         ///< Used to get all Responses that do not have a corresponding Command
    NextSequenceID = AeroMessages::AeroModuleResponse1; ///< Load the first response for the next message exchange
    break;
  default:
    logToSerials(F("SequenceID unknown"), true, 2);
    Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    Wireless.flush_rx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
    break;
  }
  updateAckData();
  saveSettings(ModuleSettings);
  return LastMessageReached;
}

void AeroModule::updateResponse()
{
  Aero1Response1ToSend.SprayEnabled = AeroNT1->getSprayEnabled();
  Aero1Response1ToSend.Pressure = AeroNT1->getPressure();
  Aero1Response1ToSend.State = AeroNT1->Pump->getState();
  Aero1Response1ToSend.LastSprayPressure = AeroNT1->getLastSprayPressure();
  Aero1Response1ToSend.Weight = Weight1->getWeight();
}

void AeroModule::updateAckData()
{ // so you can see that new data is being sent
  if (*Debug)
  {
    logToSerials(F("Updating Acknowledgement to:"), false, 2);
    logToSerials(toText_aeroSequenceID(NextSequenceID), true, 1);
  }
  Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)

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
    logToSerials(F("Ack defaults loaded"), true, 3);
    Wireless.writeAckPayload(1, &AeroModule1ResponseToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}