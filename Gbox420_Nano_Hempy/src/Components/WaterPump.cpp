#include "WaterPump.h"

WaterPump::WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  logToSerials(F(""),true,0);  //New line
  PumpSwitch = new Switch_PWM(F("SpraySolenoid"),DefaultSettings->PumpPin,&DefaultSettings->Speed,&DefaultSettings->SpeedLowLimit,DefaultSettings->PumpPinNegativeLogic);
  if(DefaultSettings->BypassSolenoidPin != 255)  //TODO: Split WaterPump into: WaterPump and PressurePump (inherits from WaterPump)
  {
   BypassSwitch = new Switch(F("BypassSolenoid"),DefaultSettings->BypassSolenoidPin,DefaultSettings->BypassSolenoidNegativeLogic);
  }
    
  PumpTimeOut= &DefaultSettings->PumpTimeOut;  
  PumpEnabled = &DefaultSettings->PumpEnabled;
  if(*PumpEnabled) {State = IDLE; }
  else {State = DISABLED;}  
  
  if(DefaultSettings->PrimingTime != -1) {PrimingTime= &DefaultSettings->PrimingTime;} 
  if(DefaultSettings->BlowOffTime != -1) {BlowOffTime= &DefaultSettings->BlowOffTime;}
    
  Parent->addToReportQueue(this);         
  Parent->addToRefreshQueue_Sec(this);      
  logToSerials(F("WaterPump object created"), true, 2);
}

void WaterPump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, getStateText());
  strcat_P(LongMessage, (PGM_P)F(" , TimeOut:"));
  strcat(LongMessage, toText_second(*PumpTimeOut));
  logToSerials(&LongMessage, true, 1);
}

void WaterPump::refresh_Sec()  
{
  if(*Debug){
    Common::refresh_Sec();
  }

  updateState();

  if(PumpOn){
    PumpSwitch -> turnOn();
  }
  else{
    PumpSwitch -> turnOff();
  }

  if(BypassOn){
    BypassSwitch -> turnOn();   
  }
  else{
    BypassSwitch -> turnOff(); 
  } 
}

void WaterPump::updateState(PumpStates NewState)  ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State 
{
  if(NewState >=0)  ///< if not the default value was passed
  {    
    State = NewState;
    PumpTimer = millis(); ///< Start measuring the time spent in the new State
  }

  switch (State)
  {
    case PRIMING:
      PumpOn=true;
      BypassOn=true;
      if(millis() - PumpTimer > ((uint32_t)*PrimingTime * 1000)) ///< Is it time to disable the Bypass solenoid
      { 
        logToSerials(F("Priming complete, running..."), true, 3);
        updateState(RUNNING);
      }
      break;
    case RUNNING:
      PumpOn=true;
      BypassOn=false;
      *PumpEnabled = true;
      if(RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000)){
        RunTime = 0;
        logToSerials(F("Running complete, stopping..."), true, 3);
        if(BlowOffTime != NULL && *BlowOffTime >0)        { 
          updateState(BLOWOFF);
        }
        else {   
          updateState(IDLE);
        }
      }
      if(millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
      { 
        Parent -> addToLog(F("ALERT: Pump timeout reached"), 3); ///< \todo send email alert 
        updateState(DISABLED);
      }
      break;
    case BLOWOFF:
      PumpOn=false;
      BypassOn=true;  
      *PumpEnabled = true;
      if(millis() - PumpTimer > ((uint32_t)*BlowOffTime * 1000)) ///Is it time to disable the Bypass solenoid
      {  
        logToSerials(F("Pressure released"), true, 3);
        updateState(IDLE);
      }
      break;
    case IDLE:
      PumpOn=false;
      BypassOn=false; 
      *PumpEnabled = true;    
      break;
    case MIXING:
      PumpOn=true;
      BypassOn=true;
      *PumpEnabled = true;
      if((RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000)) || millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)){
        RunTime = 0;
        logToSerials(F("Mixing finished"), true, 3);  
        updateState(IDLE);
      }      
      break;
    case DISABLED:
      PumpOn=false;
      BypassOn=false;
      *PumpEnabled = false;      
      break;
  }
}

void WaterPump::startPump(bool ResetStatus)
{
  if(ResetStatus)   {
    *PumpEnabled = true;
  }
  logToSerials(Name, false, 3);  
  if(*PumpEnabled){
    logToSerials(F("ON"), true, 1);
    Parent->getSoundObject()->playOnSound();
    if(PrimingTime != NULL && *PrimingTime >0)
    {
      updateState(PRIMING);       
    }
    else
    {
      updateState(RUNNING); 
    }
  }
  else
    logToSerials(F("disabled, cannot turn ON"), true, 1); 
}

void WaterPump::stopPump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("OFF"), true, 1);
  Parent->getSoundObject()->playOffSound();  
  if(BlowOffTime != NULL && *BlowOffTime >0)
  {
    updateState(BLOWOFF);       
  }
  else
  {
    updateState(IDLE);
  }  
}

void WaterPump::disablePump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("disabled"), true, 1);
  Parent->getSoundObject()->playOffSound();  
  updateState(DISABLED); 
}

void WaterPump::setSpeed(uint8_t DutyCycle) //Set PWM duty cycle
{
  PumpSwitch -> setDutyCycle(DutyCycle);
}

void WaterPump::startMixing(int TimeOutSec)  ///< Mix the nutrient reservoir by turning on the bypass solenoid and the pump. Runs till the TimeOutSec parameter or the pump timeout
{
  if(TimeOutSec>0) {
    RunTime = TimeOutSec;
  }
  else {
    RunTime = 0;  ///< if no mix timeout defined -> Run until pump timeout is reached
  }
  Parent->addToLog(F("Mixing nutrients"));
  Parent->getSoundObject()->playOnSound();
  updateState(MIXING);
}

void WaterPump::turnBypassOn(){
  Parent->addToLog(F("Bypass ON"));
  Parent->getSoundObject()->playOnSound();
  updateState(BLOWOFF);  
}

void WaterPump::turnBypassOff(){  
  Parent->addToLog(F("Bypass OFF"));
  Parent->getSoundObject()->playOffSound();
  updateState(IDLE); 
}

//////////////////////////////////////////////////////////////////////////////////

PumpStates WaterPump::getState(){
  return State;
}


char * WaterPump::getStateText(){
  return toText_pumpState(State);
}

bool WaterPump::getOnState(){
  return PumpOn;
}

bool WaterPump::getBypassOnState(){
  return BypassOn;
}

char *WaterPump::getBypassOnStateText(){
  return toText_onOff(BypassOn);
}

bool WaterPump::getEnabledState(){
  return *PumpEnabled;
}

int WaterPump::getPumpTimeOut(){
  return *PumpTimeOut;
}

void WaterPump::setPumpTimeOut(int TimeOut){
  if(*this->PumpTimeOut != TimeOut && TimeOut > 0){
    *this->PumpTimeOut = TimeOut;
    logToSerials(Name, false, 1);
    logToSerials(F("timeout updated"), true, 1);
    Parent -> getSoundObject()->playOnSound();
  }  
}

int WaterPump::getPrimingTime()
{
  return *PrimingTime;   
}

void WaterPump::setPrimingTime(int Timing)
{
  if(*PrimingTime != Timing && Timing > 0)
  {
    *PrimingTime = Timing;
    Parent->addToLog(F("Aero priming time updated"));
    Parent->getSoundObject()->playOnSound();
  }
}