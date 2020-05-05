#include "WaterPump.h"

WaterPump::WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  PumpPin = &DefaultSettings->PumpPin;
  pinMode(*PumpPin, OUTPUT);
  digitalWrite(*PumpPin, HIGH);          ///initialize pump in OFF state
  PumpTimeOut= &DefaultSettings->PumpTimeOut;
  
  PumpEnabled = &DefaultSettings->PumpEnabled;
  if(*PumpEnabled) {State = IDLE; }
  else {State = DISABLED;}
    
  if(DefaultSettings->BypassSolenoidPin != 255)
  {
    BypassSolenoidPin = &DefaultSettings->BypassSolenoidPin;
    pinMode(*BypassSolenoidPin, OUTPUT);
    digitalWrite(*BypassSolenoidPin, HIGH); ///initialize bypass solenoid in OFF state
  }
  if(DefaultSettings->PrimeTime != -1) {PrimeTime= &DefaultSettings->PrimeTime;} 
  if(DefaultSettings->BlowOffTime != -1) {BlowOffTime= &DefaultSettings->BlowOffTime;}
    
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);    ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
  logToSerials(F("WaterPump object created"), true, 1);
}

void WaterPump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, getStateText());
  strcat_P(LongMessage, (PGM_P)F(" , TimeOut:"));
  strcat(LongMessage, toText(*PumpTimeOut));
  logToSerials(&LongMessage, true, 1);
}

void WaterPump::refresh_Sec()  
{
  if(Debug){
    Common::refresh_Sec();
  }

  UpdateState();

  if(PumpOn){
    digitalWrite(*PumpPin, LOW);   /// < Relay turns ON when output pin is pulled LOW (Inverted logic)
  }
  else{
    digitalWrite(*PumpPin, HIGH);  /// < Relay turns OFF when output pin is pulled HIGH (Inverted logic)
  }

  if(BypassOn){
  digitalWrite(*BypassSolenoidPin, LOW);   
  }
  else{
    digitalWrite(*BypassSolenoidPin, HIGH); 
  } 
}

void WaterPump::UpdateState(PumpState NewState)  ///< Without a parameter actualize the current State. When NewState parameter is passed it overwrites State 
{
  if(NewState >=0)  ///< if not the default value was passed
  {    
    State = NewState;
    PumpTimer = millis(); ///< Start measuring the time spent in the new State
  }

  switch (State)
  {
    case PRIME:
      PumpOn=true;
      BypassOn=true;
      if(millis() - PumpTimer > ((uint32_t)*PrimeTime * 1000)) ///< Is it time to disable the Bypass solenoid
      { 
        logToSerials(F("Priming complete, running..."), true, 3);
        UpdateState(RUNNING);
      }
      break;
    case RUNNING:
      PumpOn=true;
      BypassOn=false;
      if(RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000)){
        RunTime = 0;
        if(BlowOffTime != NULL && *BlowOffTime >0)        { 
          UpdateState(BLOWOFF);
        }
        else {   
          UpdateState(IDLE);
        }
      }
      if(millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)) ///< Safety feature, During normal operation this should never be reached. The caller that turned on the pump should stop it before timeout is reached
      { 
        Parent -> addToLog(F("ALERT: Pump timeout reached"), 3); ///< \todo send email alert 
        UpdateState(DISABLED);
      }
      break;
    case BLOWOFF:
      PumpOn=false;
      BypassOn=true;  
      if(millis() - PumpTimer > ((uint32_t)*BlowOffTime * 1000)) ///Is it time to disable the Bypass solenoid
      {  
        turnBypassOff();
        logToSerials(F("Pressure released"), true, 3);
      }
      break;
    case IDLE:
      PumpOn=false;
      BypassOn=false;      
      break;
    case MIXING:
      PumpOn=true;
      BypassOn=true;
      if((RunTime > 0 && millis() - PumpTimer > ((uint32_t)RunTime * 1000)) || millis() - PumpTimer > ((uint32_t)*PumpTimeOut * 1000)){
        RunTime = 0;
        logToSerials(F("Mixing finished"), true, 3);  
        UpdateState(IDLE);
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
    if(PrimeTime != NULL && *PrimeTime >0)
    {
      UpdateState(PRIME);       
    }
    else
    {
      UpdateState(RUNNING); 
    }
  }
  else
    logToSerials(F("disabled, cannot turn ON"), true, 1); 
}

void WaterPump::stopPump(bool ResetStatus)
{
  if(ResetStatus) {
    *PumpEnabled = true; 
  }
  logToSerials(Name, false, 3);
  if(*PumpEnabled){
    logToSerials(F("OFF"), true, 1);
    Parent->getSoundObject()->playOffSound();  
    if(BlowOffTime != NULL && *BlowOffTime >0)
    {
      UpdateState(BLOWOFF);       
    }
    else
    {
      UpdateState(IDLE);
    }    
  }
  else
    logToSerials(F("disabled, cannot turn OFF"), true, 1);
}

void WaterPump::disablePump()
{
  logToSerials(Name, false, 3);
  logToSerials(F("disabled"), true, 1);
  Parent->getSoundObject()->playOffSound();  
  UpdateState(DISABLED); 
}

void WaterPump::startMixing(int TimeOutSec)  ///< Mix the nutrient reservoir by turning on the bypass solenoid and the pump. Runs till the TimeOutSec parameter or the pump timeout
{
  if(TimeOutSec>0) RunTime = TimeOutSec;
  UpdateState(MIXING);
}

void WaterPump::turnBypassOn(){
  //BlowOffInProgress = true; ///no extra timer is needed, will use SprayTimer  \todo Move this under NoTank
  // SprayTimer = millis();  ///measures blowoff time  \todo Move this under NoTank
  
  Parent->addToLog(F("Bypass ON"));
  Parent->getSoundObject()->playOnSound();
  UpdateState(B)
  }
}

void WaterPump::turnBypassOff(bool addToLog){
  BypassOn = false;
  checkRelays();
  if(addToLog)
  {
    Parent->addToLog(F("Bypass OFF"));
    Parent->getSoundObject()->playOffSound();
  }
}

//////////////////////////////////////////////////////////////////////////////////

bool WaterPump::getOnState(){
  return PumpOn;
}

bool WaterPump::getEnabledState(){
  return *PumpEnabled;
}

char * WaterPump::getStateText(){
  return pumpStateToText(*PumpEnabled,PumpOn);
}

int WaterPump::getTimeOut(){
  return *PumpTimeOut;
}

void WaterPump::setTimeOut(int TimeOut){
  if(*this->PumpTimeOut != TimeOut && TimeOut > 0){
    *this->PumpTimeOut = TimeOut;
    logToSerials(Name, false, 1);
    logToSerials(F("timeout updated"), true, 1);
    Parent -> getSoundObject()->playOnSound();
  }  
}