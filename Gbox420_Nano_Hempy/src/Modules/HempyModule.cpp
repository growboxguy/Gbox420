/**@file*/
///Supports monitoring two Hempy buckets
///Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "HempyModule.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/WeightSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/HempyBucket.h"

HempyModule::HempyModule(const __FlashStringHelper *Name, Settings::HempyModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  Weight1 = new WeightSensor(F("Weight1"), this, &ModuleSettings->Weight1);
  Weight2 = new WeightSensor(F("Weight2"), this, &ModuleSettings->Weight2);
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->HempyPump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->HempyPump2);
  Bucket1 = new HempyBucket(F("Bucket1"), this, &ModuleSettings->Bucket1,Weight1,Pump1);
  Bucket2 = new HempyBucket(F("Bucket2"), this, &ModuleSettings->Bucket2,Weight2,Pump2);
  //addToRefreshQueue_Sec(this);         ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  addToRefreshQueue_FiveSec(this);     ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  //addToRefreshQueue_Minute(this);      ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  //addToRefreshQueue_QuarterHour(this); ///Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
  logToSerials(Name, false, 0);
  logToSerials(F("- HempyModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("HempyModule initialized"), 0);
}

void HempyModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec(); 
  runReport(); 
  updateResponse(); 
}

void HempyModule::updateResponse(){
  Response.PumpState_B1 = Pump1 -> getState();;
  Response.Weight_B1 = Weight1 -> getWeight();
  Response.PumpState_B2 = Pump2 -> getState();
  Response.Weight_B2 = Weight2 -> getWeight(); 
  Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 
  Wireless.writeAckPayload(1, &Response, sizeof(Response)); ///< load the payload to send the next time
}

void HempyModule::processCommand(hempyCommand *Command){
  setDebug(Command -> Debug);
  setMetric(Command -> Metric);
  if(Command -> DisablePump_B1) Pump1 -> disablePump();
  if(Command -> TurnOnPump_B1) Pump1 -> startPump(true);
  if(Command -> TurnOffPump_B1) Pump1 -> stopPump();
  Pump1 -> setPumpTimeOut(Command -> TimeOutPump_B1);
  Bucket1 -> setStartWeight(Command -> StartWeightBucket_B1);
  Bucket1 -> setStopWeight(Command -> StopWeightBucket_B1);

  if(Command -> DisablePump_B2) Pump2 -> disablePump();
  if(Command -> TurnOnPump_B2) Pump2 -> startPump(true);
  if(Command -> TurnOffPump_B2) Pump2 -> stopPump();
  Pump2 -> setPumpTimeOut(Command -> TimeOutPump_B2);
  Bucket2 -> setStartWeight(Command -> StartWeightBucket_B2);
  Bucket2 -> setStopWeight(Command -> StopWeightBucket_B2);

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
      logToSerials(F(","),false,1);
      logToSerials(Command -> Metric,false,1);
      logToSerials(F(";"),false,1);
    logToSerials(Command -> DisablePump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOnPump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOffPump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TimeOutPump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StartWeightBucket_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StopWeightBucket_B1,false,1);
        logToSerials(F(";"),false,1);
  logToSerials(Command -> DisablePump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOnPump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOffPump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TimeOutPump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StartWeightBucket_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StopWeightBucket_B2,true,1);
  }       
}