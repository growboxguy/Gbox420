/**@file*/
///Supports monitoring an Aeroponics tote
///Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "AeroModule.h"
//#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PressureSensor.h"
#include "../Components/WaterPump.h"
#include "../Components/Aeroponics_NoTank.h"
#include "../Components/Aeroponics_Tank.h"


AeroModule::AeroModule(const __FlashStringHelper *Name, Settings::AeroModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  Pres1 = new PressureSensor(F("Pres1"), this, &ModuleSettings->Pres1);
  Pump1 = new WaterPump(F("Pump1"),this,&ModuleSettings->AeroPump1);
  if(DefaultSettings->PressureTankPresent)
  {
    AeroT1 = new Aeroponics_Tank(F("AeroT1"), this, &ModuleSettings->AeroT1_Common, &ModuleSettings->AeroT1_Specific, Pres1, Pump1);  ///< Use this with a pressure tank
  }
  else
  {
    AeroNT1 = new Aeroponics_NoTank(F("AeroNT1"), this, &ModuleSettings->AeroNT1_Common, Pres1, Pump1);  ///< Use this without a pressure tank
  }  
  addToRefreshQueue_Sec(this);         ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  addToRefreshQueue_FiveSec(this);     ///Subscribing to the 5 sec refresh queue: Calls the refresh_FiveSec() method
  //addToRefreshQueue_Minute(this);      ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  //addToRefreshQueue_QuarterHour(this); ///Subscribing to the 30 minutes refresh queue: Calls the refresh_QuarterHour() method
  logToSerials(Name, false, 0);
  logToSerials(F("- AeroModule object created, refreshing..."), true, 1);
  if(AeroT1 != NULL && AeroNT1 != NULL)  /// \todo Remove this
  {
    logToSerials(F("DEV MODE, please comment out AeroT1 or AeroNT1 depending if you use a pressure tank or not"), true, 0);
  }
  runAll();
  addToLog(F("AeroModule initialized"), 0);
}

void AeroModule::processCommand(aeroCommand *Command){
  setDebug(Command -> Debug);
  setMetric(Command -> Metric);

  if(AeroT1 != NULL)
  {
    if(Command -> SprayEnable) AeroT1 ->  setSprayOnOff(true);
    if(Command -> SprayDisable) AeroT1 -> setSprayOnOff(false);
    if(Command -> SprayNow) AeroT1 -> sprayNow(true);
    if(Command -> SprayOff) AeroT1 -> sprayOff();
    AeroT1 -> setSprayInterval(Command -> SprayInterval);
    AeroT1 -> setSprayDuration(Command -> SprayDuration);
    if(Command -> PumpOn) AeroT1 -> startPump(true);
    if(Command -> PumpOff) AeroT1 -> stopPump(true);
    if(Command -> PumpDisable) AeroT1 -> setPumpDisable();
    AeroT1 -> Pump -> setPumpTimeOut(Command -> PumpTimeOut);
    AeroT1 -> Pump -> setPrimingTime(Command -> PumpPriming);
    if(Command -> MixReservoir) AeroT1 -> mixReservoir();
  }

  if(AeroNT1 != NULL)
  {
    if(Command -> SprayEnable) AeroNT1 ->  setSprayOnOff(true);
    if(Command -> SprayDisable) AeroNT1 -> setSprayOnOff(false);
    if(Command -> SprayNow) AeroNT1 -> sprayNow(true);
    if(Command -> SprayOff) AeroNT1 -> sprayOff();
    AeroNT1 -> setSprayInterval(Command -> SprayInterval);
    AeroNT1 -> setSprayDuration(Command -> SprayDuration);
    if(Command -> PumpOn) AeroNT1 -> startPump(true);
    if(Command -> PumpOff) AeroNT1 -> stopPump(true);
    if(Command -> PumpDisable) AeroNT1 -> setPumpDisable();
    AeroNT1 -> Pump -> setPumpTimeOut(Command -> PumpTimeOut);
    AeroNT1 -> Pump -> setPrimingTime(Command -> PumpPriming);
    if(Command -> MixReservoir) AeroNT1 -> mixReservoir();
    if(Command -> BypassOn) AeroNT1 -> Pump-> turnBypassOn();
    if(Command -> BypassOff) AeroNT1 -> Pump-> turnBypassOff();
  }  

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
      logToSerials(F(","),false,1);
      logToSerials(Command -> Metric,false,1);
      logToSerials(F(";"),false,1);
    logToSerials(Command -> SprayEnable,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> SprayDisable,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> SprayNow,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> SprayOff,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> SprayInterval,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> SprayDuration,false,1);
        logToSerials(F(";"),false,1);
      logToSerials(Command -> PumpOn,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> PumpOff,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> PumpDisable,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> PumpTimeOut,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> PumpPriming,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> MixReservoir,false,1);
        logToSerials(F(";"),false,1);
      logToSerials(Command -> BypassOn,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> BypassOff,true,1);
  }       
}

void AeroModule::updateResponse(){
  if(AeroT1 != NULL)
  {
    Response.SprayEnabled = AeroT1 -> getSprayEnabled();
    Response.Pressure = AeroT1 -> getPressure();
    //Response.PumpOn = AeroT1 -> PumpOK;
    //Response.PumpEnabled = AeroT1 -> getPumpOnState();
    //Response.BypassOn = AeroT1 -> getEnabledState();
    //Response.LastSP = AeroT1 -> getWeight();
    Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 
    Wireless.writeAckPayload(1, &Response, sizeof(Response)); ///< load the payload to send the next time
  }
  if(AeroNT1 != NULL)
  {

  }
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