#include "Aeroponics.h"
#include "GrowBox.h"

Aeroponics::Aeroponics(GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings){  //constructor
    this -> GBox = GBox;
    this -> BypassSolenoidPin = BypassSolenoidPin;
    this -> PumpPin = PumpPin;
    SprayEnabled = &DefaultSettings -> SprayEnabled;  //Enable/disable misting
    Interval = &DefaultSettings -> Interval; //Aeroponics - Spray every 15 minutes
    Duration = &DefaultSettings -> Duration; //Aeroponics - Spray time in seconds
    PressureLow = &DefaultSettings -> PressureLow; //Aeroponics - Turn on pump below this pressure (bar)
    PressureHigh = &DefaultSettings -> PressureHigh; //Aeroponics - Turn off pump above this pressure (bar)
    PumpTimeout = &DefaultSettings -> PumpTimeout;  // Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
    PrimingTime = &DefaultSettings -> PrimingTime;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
}


void Aeroponics::setPumpOn(bool CalledFromWebsite){
  BypassActive  = CalledFromWebsite; //If called from the Web interface keep the pump on
  PumpOn = true;
  PumpTimer = millis();
  checkRelays();
  if(CalledFromWebsite){
    GBox -> addToLog(F("Pump ON"));
    GBox -> Sound1 -> playOnSound();
    PumpOK = true; 
  }
}

void Aeroponics::setPumpOff(bool CalledFromWebsite){  
  BypassActive = false;
  PumpOn = false;
  if(!BlowOffInProgress)BypassSolenoidOn = false; //Close bypass valve
  PumpTimer = millis();        
  checkRelays();
  if(CalledFromWebsite){ 
    GBox -> addToLog(F("Pump OFF"));
    GBox -> Sound1 -> playOffSound();
    PumpOK = true; //re-enable pump 
  }
}

void Aeroponics::PumpDisable(){
  setPumpOff(false);
  PumpOK = false;
  GBox -> addToLog(F("Pump disabled"));
}

void Aeroponics::Mix(){
  PumpOn = true;    
  BypassActive = true; 
  PumpOK = true;
  BypassSolenoidOn = true;
  PumpTimer = millis();    
  checkRelays();
  GBox -> Sound1 -> playOnSound();
  GBox -> addToLog(F("Mixing nutrients"));
}

void Aeroponics::setSprayOnOff(bool State){
  *SprayEnabled=State;
  if(*SprayEnabled){ 
    GBox -> addToLog(F("Aeroponics spray enabled"));
    GBox -> Sound1 -> playOnSound();
    } 
  else {
    GBox -> addToLog(F("Aeroponics spray disabled"));
    GBox -> Sound1 -> playOffSound();
    }
}

void Aeroponics::setInterval(int interval){  
  *Interval = interval; 
  SprayTimer = millis();
}

void Aeroponics::setDuration(int duration){  
  *Duration = duration;
  SprayTimer = millis(); 
  GBox -> addToLog(F("Spray time updated"));  
}


void Aeroponics::setPressureLow(float PressureLow){
  *(this -> PressureLow) =  PressureLow;
}

void Aeroponics::setPressureHigh(float PressureHigh){
  *(this -> PressureHigh) = PressureHigh;
  GBox -> addToLog(F("Pump settings updated"));
}

const __FlashStringHelper * Aeroponics::pumpStateToText(){
   if(!PumpOK) return F("DISABLED");
   else if(PumpOn) return F("ON");
   return F("OFF");
}


void Aeroponics::setAeroPumpTimeout(int Timeout)
{
*PumpTimeout = (uint32_t)Timeout;
GBox -> addToLog(F("Aero pump timeout updated"));
}

void Aeroponics::setAeroPrimingTime(uint32_t Timing)
{
*PrimingTime = (uint32_t)Timing;
GBox -> addToLog(F("Aero priming time updated"));
}


