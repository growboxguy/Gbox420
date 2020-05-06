#include "Aeroponics.h"

/// This is a virtual class (cannot be instanciated), parent of two classes:
/// -Aeroponics_NoTank: High pressure pump is directly connected to the aeroponics tote
/// -Aeroponics_Tank: A pressure tank is added between the high pressure pump and aeroponics tote, requires an extra solenoid for spraying

Aeroponics::Aeroponics(const __FlashStringHelper *Name, Module *Parent, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor, WaterPump *Pump) : Common(Name)
{ ///constructor
  this->Parent = Parent;
  this->Name = Name;
  SprayEnabled = &DefaultSettings->SprayEnabled; ///Enable/disable misting
  Interval = &DefaultSettings->Interval;         ///Aeroponics - Spray every 15 minutes
  Duration = &DefaultSettings->Duration;         ///Aeroponics - Spray time in seconds
  MaxPressure = &DefaultSettings->MaxPressure; ///Aeroponics - Turn off pump above this pressure (bar)
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  this->Pump = Pump;  
  Parent->addToReportQueue(this);          ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);     ///Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method  
}

void Aeroponics::report()
{                                                  ///report status to Serial output, runs after the child class`s report function
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F(" ; High Pressure:"));
  strcat(LongMessage, toText_pressure(*MaxPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, toText_yesNo(SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; Interval:"));
  strcat(LongMessage, toText_minute(*Interval));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText_second(*Duration));  
  logToSerials(&LongMessage, true, 0); ///Break line, No indentation needed: child class already printed it
}

void Aeroponics::setSprayInterval(int interval)
{
  if(*Interval != interval && interval > 0)
  {
    *Interval = interval;
   Parent -> getSoundObject() -> playOnSound();
  }
}

int Aeroponics::getSprayInterval()
{
  return *Interval;
}

char *Aeroponics::getSprayIntervalText()
{
  return toText(*Interval);
}

void Aeroponics::setSprayDuration(int duration)
{
  if(*Duration !=duration && duration > 0)
  {
    *Duration = duration;
    Parent->addToLog(F("Spray time updated"));
    Parent -> getSoundObject() -> playOnSound();
  }
}

void Aeroponics::setSprayOnOff(bool State)
{
  *SprayEnabled = State;
  if (*SprayEnabled)
  {
    Parent->addToLog(F("Aero spray enabled"));
    Parent->getSoundObject()->playOnSound();
  }
  else
  {
    Parent->addToLog(F("Aero spray disabled"));
    Parent->getSoundObject()->playOffSound();
  }
}

bool Aeroponics::getSprayEnabled()
{
  return *SprayEnabled;
}

int Aeroponics::getSprayDuration()
{
  return *Duration;
}

char *Aeroponics::getSprayDurationText()
{
  return toText(*Duration);
}

char *Aeroponics::sprayStateToText()
{
  return toText_onOff(*SprayEnabled);
}

float Aeroponics::getLastSprayPressure(){
  return LastSprayPressure;
}

char *Aeroponics::getLastSprayPressureText(bool IncludeCurrentPressure ){
  memset(&ShortMessage[0], 0, sizeof(ShortMessage)); ///clear variable

  toText_pressure(LastSprayPressure);  ///< loads the Last pressure measured during spraying
  if(IncludeCurrentPressure)
  {
    strcat_P(ShortMessage, (PGM_P)F(" ["));
    dtostrf(FeedbackPressureSensor->getPressure(), 4, 2, ShortMessage);
    strcat_P(ShortMessage, (PGM_P)F("]"));
  } 
  return ShortMessage;
}

////////////////////////////////////////
///Pump controls

void Aeroponics::mixReservoir()
{  
  Pump -> startMixing();
}

float Aeroponics::getPressure()
{
  return FeedbackPressureSensor -> getPressure();
}

void Aeroponics::setMinPressure(float Pressure)
{
  if(*MinPressure != Pressure && Pressure > 0){
    *MinPressure = Pressure;
  } 
}
  
void Aeroponics::setMaxPressure(float Pressure)
{ 
  if(*MaxPressure != Pressure && Pressure > 0){
    *MaxPressure = Pressure;
    Parent->addToLog(F("Tank limits updated"));
    Parent -> getSoundObject() -> playOnSound();
  }  
}

void Aeroponics::startPump(bool UserRequest)
{  
  Pump -> startPump(UserRequest);
}

void Aeroponics::stopPump(bool UserRequest)
{
  Pump -> stopPump(UserRequest);
}

void Aeroponics::setPumpDisable()
{
  Pump -> disablePump();
}
