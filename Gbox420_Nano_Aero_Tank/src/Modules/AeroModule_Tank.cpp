/**@file*/
///Supports monitoring an Aeroponics tote - With a pressure tank
///Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "AeroModule_Tank.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PressureSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/Aeroponics_Tank.h"
#include "../Components/WeightSensor.h"

///< Variables used during wireless communication
uint8_t NextSequenceID = AeroMessages::AeroModuleResponse1;
struct AeroModuleResponse AeroModule1ResponseToSend = {AeroMessages::AeroModuleResponse1};
struct AeroResponse Aero1ResponseToSend = {AeroMessages::AeroResponse1};
struct AeroCommonTemplate AeroResetToSend = {AeroMessages::AeroReset}; //< Special response signaling the end of a message exchange to the Transmitter
unsigned long LastMessageSent = 0;                                            //When was the last message sent

AeroModule::AeroModule(const __FlashStringHelper *Name, Settings::AeroModuleSettings *DefaultSettings) : Common(Name), Module()
{
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  Pres1 = new PressureSensor(F("Pres1"), this, &ModuleSettings->Pres1);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->AeroPump1);
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  AeroT1 = new Aeroponics_Tank(F("AeroT1"), this, &ModuleSettings->AeroT1_Common, &ModuleSettings->AeroT1_Specific, Pres1, Pump1); ///< Use this with a pressure tank
  Aero1ResponseToSend.PressureTankPresent = true;

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
  {                                                     //< If there is a package exchange in progress
    NextSequenceID = AeroMessages::AeroModuleResponse1; //< Reset back to the first response
    logToSerials(F("Timeout during message exchange, reseting to first response"), true, 0);
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

void AeroModule::updateResponse()
{
  Aero1ResponseToSend.SprayEnabled = AeroT1->getSprayEnabled();
  Aero1ResponseToSend.Pressure = AeroT1->getPressure();
  Aero1ResponseToSend.State = AeroT1->Pump->getState();
  Aero1ResponseToSend.LastSprayPressure = AeroT1->getLastSprayPressure();
  Aero1ResponseToSend.Weight = Weight1->getWeight();
}

void AeroModule::processCommand(void *ReceivedCommand)
{
  AeroMessages ReceivedSequenceID = ((AeroCommonTemplate *)ReceivedCommand)->SequenceID;
  LastMessageReceived = millis(); ///< Store current time
  logToSerials(F("Received:"), false, 1);
  logToSerials(toText_aeroSequenceID(ReceivedSequenceID), false, 1);
  logToSerials(F("- Sent:"), false, 1);
  logToSerials(toText_aeroSequenceID(NextSequenceID), true, 1);

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
      if (((AeroCommand *)ReceivedCommand)->SprayEnabled)
        AeroT1->setSprayOnOff(true);
      if (((AeroCommand *)ReceivedCommand)->SprayDisabled)
        AeroT1->setSprayOnOff(false);
      if (((AeroCommand *)ReceivedCommand)->SprayNow)
        AeroT1->sprayNow(true);
      if (((AeroCommand *)ReceivedCommand)->SprayOff)
        AeroT1->sprayOff();
      if (((AeroCommand *)ReceivedCommand)->RefillPressureTank)
        AeroT1->refillTank();
      if (((AeroCommand *)ReceivedCommand)->TareWeight)
        Weight1->triggerTare();
      AeroT1->setDayMode(((AeroCommand *)ReceivedCommand)->DayMode);
      AeroT1->setDayInterval(((AeroCommand *)ReceivedCommand)->DayInterval);
      AeroT1->setDayDuration(((AeroCommand *)ReceivedCommand)->DayDuration);
      AeroT1->setNightInterval(((AeroCommand *)ReceivedCommand)->NightInterval);
      AeroT1->setNightDuration(((AeroCommand *)ReceivedCommand)->NightDuration);
      if (((AeroCommand *)ReceivedCommand)->PumpOn)
        AeroT1->Pump->startPump(true);
      if (((AeroCommand *)ReceivedCommand)->PumpOff)
        AeroT1->Pump->stopPump();
      if (((AeroCommand *)ReceivedCommand)->PumpDisable)
        AeroT1->Pump->disablePump();
      AeroT1->Pump->setSpeed(((AeroCommand *)ReceivedCommand)->PumpSpeed);
      AeroT1->Pump->setPumpTimeOut(((AeroCommand *)ReceivedCommand)->PumpTimeOut);
      AeroT1->Pump->setPrimingTime(((AeroCommand *)ReceivedCommand)->PumpPrimingTime);
      AeroT1->setMinPressure(((AeroCommand *)ReceivedCommand)->MinPressure);
      AeroT1->setMaxPressure(((AeroCommand *)ReceivedCommand)->MaxPressure);
      if (((AeroCommand *)ReceivedCommand)->MixReservoir)
        AeroT1->Pump->startMixing();
   
    NextSequenceID = AeroMessages::AeroReset; // update the next Message that will be copied to the buffer
    if (*Debug)
    {
      logToSerials(F("Aero1:"), false, 2);
      logToSerials(((AeroCommand *)ReceivedCommand)->SprayEnabled, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->SprayDisabled, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->SprayNow, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->SprayOff, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->DayMode, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->DayInterval, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->DayDuration, false, 1);
      logToSerials(F(";"), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->NightInterval, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->NightDuration, false, 1);
      logToSerials(F(";"), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->PumpSpeed, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->PumpOn, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->PumpOff, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->PumpDisable, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->PumpTimeOut, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->PumpPrimingTime, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->MinPressure, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->MaxPressure, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->MixReservoir, false, 1);
      logToSerials(F(","), false, 1);
      logToSerials(((AeroCommand *)ReceivedCommand)->TareWeight, true, 1);
      
    }
    break;
  case AeroMessages::AeroReset:                         //< Used to get all Responses that do not have a corresponding Command
    NextSequenceID = AeroMessages::AeroModuleResponse1; //< Load the first response for the next message exchange
    break;
  default:
    logToSerials(F("SequenceID unknown, ignoring message"), true, 2);
    break;
  }
  updateAckData();
  saveSettings(ModuleSettings);
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
    Wireless.writeAckPayload(1, &Aero1ResponseToSend, WirelessPayloadSize);
    break;
  case AeroMessages::AeroReset: //< AeroReset should always be the last element in the enum: Signals to stop the message exchange
    Wireless.writeAckPayload(1, &AeroResetToSend, WirelessPayloadSize);
    break;
  default:
    logToSerials(F("Unknown next Sequence number, Ack defaults loaded"), true, 3);
    Wireless.writeAckPayload(1, &AeroModule1ResponseToSend, WirelessPayloadSize); // load the first Response into the buffer
    break;
  }
}