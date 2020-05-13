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
    Response.PressureTankPresent = true;
  }
  else
  {
    AeroNT1 = new Aeroponics_NoTank(F("AeroNT1"), this, &ModuleSettings->AeroNT1_Common, Pres1, Pump1);  ///< Use this without a pressure tank
    Response.PressureTankPresent = false;
  }  
  addToRefreshQueue_Sec(this);         
  addToRefreshQueue_FiveSec(this);     
  //addToRefreshQueue_Minute(this);    
  //addToRefreshQueue_QuarterHour(this); 
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
    if(Command -> SprayEnabled) AeroT1 ->  setSprayOnOff(true);
    if(Command -> SprayDisabled) AeroT1 -> setSprayOnOff(false);
    if(Command -> SprayNow) AeroT1 -> sprayNow(true);
    if(Command -> SprayOff) AeroT1 -> sprayOff();
    if(Command -> RefillPressureTank) AeroT1 -> refillTank();    
    AeroT1 -> setSprayInterval(Command -> SprayInterval);
    AeroT1 -> setSprayDuration(Command -> SprayDuration);
    if(Command -> PumpOn) AeroT1 -> Pump -> startPump();
    if(Command -> PumpOff) AeroT1 -> Pump -> stopPump();
    if(Command -> PumpDisable) AeroT1 -> Pump -> disablePump();
    AeroT1 -> Pump -> setPumpTimeOut(Command -> PumpTimeOut);
    AeroT1 -> Pump -> setPrimingTime(Command -> PumpPrimingTime);
    AeroT1 -> setMinPressure(Command -> MinPressure);
    AeroT1 -> setMaxPressure(Command -> MaxPressure);
    if(Command -> MixReservoir) AeroT1 -> Pump -> startMixing();
  }

  if(AeroNT1 != NULL)
  {
    if(Command -> SprayEnabled) AeroNT1 ->  setSprayOnOff(true);
    if(Command -> SprayDisabled) AeroNT1 -> setSprayOnOff(false);
    if(Command -> SprayNow) AeroNT1 -> sprayNow(true);
    if(Command -> SprayOff) AeroNT1 -> sprayOff();
    AeroNT1 -> setSprayInterval(Command -> SprayInterval);
    AeroNT1 -> setSprayDuration(Command -> SprayDuration);
    if(Command -> PumpOn) AeroNT1 -> lockPumpOn();
    if(Command -> PumpOff) AeroNT1 -> Pump -> stopPump();
    if(Command -> PumpDisable) AeroNT1 -> Pump -> disablePump();
    AeroNT1 -> Pump -> setPumpTimeOut(Command -> PumpTimeOut);
    AeroNT1 -> Pump -> setPrimingTime(Command -> PumpPrimingTime);
    AeroNT1 -> setMaxPressure(Command -> MaxPressure);
    if(Command -> MixReservoir) AeroNT1 -> Pump -> startMixing();
    //if(Command -> BypassOn) AeroNT1 -> Pump-> turnBypassOn();
    //if(Command -> BypassOff) AeroNT1 -> Pump-> turnBypassOff();
  }  

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
      logToSerials(F(","),false,1);
      logToSerials(Command -> Metric,false,1);
      logToSerials(F(";"),false,1);
    logToSerials(Command -> SprayEnabled,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> SprayDisabled,false,1);
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
        logToSerials(Command -> PumpPrimingTime,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> MaxPressure,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> MinPressure,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> MixReservoir,true,1);
      //logToSerials(F(";"),false,1);
      //logToSerials(Command -> BypassOn,false,1);
      //logToSerials(F(","),false,1);
      //logToSerials(Command -> BypassOff,true,1);
  }       
}

void AeroModule::updateResponse(){
  if(AeroT1 != NULL)
  {    
    Response.SprayEnabled = AeroT1 -> getSprayEnabled();
    Response.Pressure = AeroT1 -> getPressure();
    Response.State = AeroT1 -> Pump -> getState();
    Response.LastSprayPressure = AeroT1 -> getLastSprayPressure();
  }
  if(AeroNT1 != NULL)
  {
    Response.SprayEnabled = AeroNT1 -> getSprayEnabled();
    Response.Pressure = AeroNT1 -> getPressure();
    Response.State = AeroNT1 -> Pump -> getState();
    Response.LastSprayPressure = AeroNT1 -> getLastSprayPressure();
  }
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