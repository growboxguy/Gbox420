/**@file*/
///Supports monitoring an Aeroponics tote
///Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "AeroModule.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PressureSensor.h"
#include "../Components/Aeroponics_NoTank.h"
#include "../Components/Aeroponics_Tank.h"


AeroModule::AeroModule(const __FlashStringHelper *Name, Settings::AeroModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  Pres1 = new PressureSensor(F("Pres1"), this, &ModuleSettings->Pres1);
  AeroT1 = new Aeroponics_Tank(F("AeroT1"), this, &ModuleSettings->AeroT1_Common, &ModuleSettings->AeroT1_Specific, Pres1); ///Passing the pressure sensor object that monitors the pressure inside the Aeroponics system
  AeroNT1 = new Aeroponics_NoTank(F("AeroNT1"), this, &ModuleSettings->AeroNT1_Common, &ModuleSettings->AeroNT1_Specific, Pres1);
  addToRefreshQueue_Sec(this);         ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  addToRefreshQueue_FiveSec(this);     ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  //addToRefreshQueue_Minute(this);      ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  //addToRefreshQueue_QuarterHour(this); ///Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
  logToSerials(Name, false, 0);
  logToSerials(F("- AeroModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("AeroModule initialized"), 0);
}

void AeroModule::processCommand(aeroCommand *Command){
  setDebug(Command -> Debug);
  setMetric(Command -> Metric);
  /*if(Command -> DisablePump1)
  if(Command -> DisablePump1) Pump1 -> disable();
  if(Command -> TurnOnPump1) Pump1 -> turnOn(true);
  if(Command -> TurnOffPump1) Pump1 -> turnOff(true);
  Pump1 -> setTimeOut(Command -> TimeOutPump1);
  Bucket1 -> setStartWeight(Command -> StartWeightBucket1);
  Bucket1 -> setStopWeight(Command -> StopWeightBucket1);

  if(Command -> DisablePump2) Pump2 -> disable();
  if(Command -> TurnOnPump2) Pump2 -> turnOn(true);
  if(Command -> TurnOffPump2) Pump2 -> turnOff(true);
  Pump2 -> setTimeOut(Command -> TimeOutPump2);
  Bucket2 -> setStartWeight(Command -> StartWeightBucket2);
  Bucket2 -> setStopWeight(Command -> StopWeightBucket2);
  */

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
      logToSerials(F(","),false,1);
      logToSerials(Command -> Metric,false,1);
      logToSerials(F(","),false,1);
    /*logToSerials(Command -> DisablePump1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOnPump1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOffPump1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TimeOutPump1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StartWeightBucket1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StopWeightBucket1,false,1);
        logToSerials(F(";"),false,1);
  logToSerials(Command -> DisablePump2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOnPump2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOffPump2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TimeOutPump2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StartWeightBucket2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StopWeightBucket2,true,1);*/
  }       
}

void AeroModule::updateResponse(){
  /*Response.OnPump1 = Pump1 -> getOnState();
  Response.EnabledPump1 = Pump1 -> getEnabledState();
  Response.WeightBucket1 = Weight1 -> getWeight();
  Response.OnPump2 = Pump2 -> getOnState();
  Response.EnabledPump2 = Pump2 -> getEnabledState();
  Response.WeightBucket2 = Weight2 -> getWeight(); */
  Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 
  Wireless.writeAckPayload(1, &Response, sizeof(Response)); ///< load the payload to send the next time
}

void AeroModule::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();  
}

void AeroModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  runReport(); 
  updateResponse(); 
}