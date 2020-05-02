#include "WaterPump.h"

WaterPump::WaterPump(const __FlashStringHelper *Name, Module *Parent, Settings::WaterPumpSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Pin = &DefaultSettings->Pin;
  PumpEnabled = &DefaultSettings->PumpEnabled;
  TimeOut= &DefaultSettings->TimeOut;
  pinMode(*Pin, OUTPUT);
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);    ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
  logToSerials(F("WaterPump object created"), true, 1);
}


void WaterPump::refresh_Sec()  
{
  if(PumpOn && millis() - PumpTimer > ((uint32_t)* TimeOut * 1000)) ///Check for pump timeout, during normal operation this should never be true
  {  
    disable();
  }
}

void WaterPump::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("State:"));
  strcat(LongMessage, getState());
  strcat_P(LongMessage, (PGM_P)F(" , TimeOut:"));
  strcat(LongMessage, toText(*TimeOut));
  logToSerials(&LongMessage, true, 1);
}

void WaterPump::turnOn(bool ResetStatus)
{
  if(ResetStatus) *PumpEnabled = true;
  logToSerials(Name, false, 3);
  if(*PumpEnabled){
    logToSerials(F("on"), true, 1);
    Parent->getSoundObject()->playOnSound();  
    PumpOn = true;
    PumpTimer = millis(); 
    checkRelay(); 
  }
  else
    logToSerials(F("disabled, cannot turnOn"), true, 1); 
}

void WaterPump::turnOff(bool ResetStatus)
{
  if(ResetStatus) *PumpEnabled = true; 
  logToSerials(Name, false, 3);
  if(*PumpEnabled){
    logToSerials(F("off"), true, 1);
    Parent->getSoundObject()->playOffSound();  
    PumpOn = false;   
    checkRelay();
  }
  else
    logToSerials(F("disabled, cannot turnOff"), true, 1);
}

void WaterPump::disable()
{
  logToSerials(Name, false, 3);
  logToSerials(F("disabled"), true, 1);
  Parent->getSoundObject()->playOffSound();  
  PumpOn = false;
  *PumpEnabled = false;
  checkRelay();
}

void WaterPump::checkRelay()
{
  if (PumpOn && PumpEnabled)
    digitalWrite(*Pin, LOW);
  else
    digitalWrite(*Pin, HIGH);  
}

char * WaterPump::getState(){
  return pumpStateToText(*PumpEnabled,PumpOn);
}

bool WaterPump::getOnState(){
  return PumpOn;
}

bool WaterPump::getEnabledState(){
  return *PumpEnabled;
}

int WaterPump::getTimeOut(){
  return *TimeOut;
}

void WaterPump::setTimeOut(int TimeOut){
  if(*this->TimeOut != TimeOut){
    *this->TimeOut = TimeOut;
    logToSerials(Name, false, 1);
    logToSerials(F("timeout updated"), true, 1);
    Parent -> getSoundObject()->playOnSound();
  }  
}