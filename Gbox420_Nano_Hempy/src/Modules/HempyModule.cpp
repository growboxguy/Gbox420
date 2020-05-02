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
  Pump1 = new WaterPump(F("Pump1"), this, &ModuleSettings->Pump1);
  Pump2 = new WaterPump(F("Pump2"), this, &ModuleSettings->Pump2);
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

void HempyModule::processCommand(hempyCommand *Command){
  setDebug(Command -> Debug);
  setMetric(Command -> Metric);
  if(Command -> DisablePump1)
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

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
      logToSerials(F(","),false,1);
      logToSerials(Command -> Metric,false,1);
      logToSerials(F(","),false,1);
    logToSerials(Command -> DisablePump1,false,1);
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
        logToSerials(Command -> StopWeightBucket2,true,1);
  }       
}

void HempyModule::updateResponse(){
  Response.OnPump1 = Pump1 -> getOnState();
  Response.EnabledPump1 = Pump1 -> getEnabledState();
  Response.WeightBucket1 = Weight1 -> getWeight();
  Response.OnPump2 = Pump2 -> getOnState();
  Response.EnabledPump2 = Pump2 -> getEnabledState();
  Response.WeightBucket2 = Weight2 -> getWeight(); 
  Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 
  Wireless.writeAckPayload(1, &Response, sizeof(Response)); ///< load the payload to send the next time
}

void HempyModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  if (RefreshAllRequested)
  {
    RefreshAllRequested = false;
    runAll();
  }  
  runReport(); 
  updateResponse(); 
}



//////////////////////////////////////////////////////////////////
//Settings
void HempyModule::setDebug(bool DebugEnabled)
{
   if (DebugEnabled != *Debug)
   {
    *Debug = DebugEnabled;
    if (*Debug)
    {
      addToLog(F("Debug enabled"));
      getSoundObject() -> playOnSound();
    }
    else
    {
      addToLog(F("Debug disabled"));
      getSoundObject() -> playOffSound();
    }
   }  
}

void HempyModule::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { //if there was a change
    *Metric = MetricEnabled;
    RefreshAllRequested = true;
    if (*Metric)
      addToLog(F("Using Metric units"));
    else
      addToLog(F("Using Imperial units"));
    getSoundObject() -> playOnSound();
  }
}
