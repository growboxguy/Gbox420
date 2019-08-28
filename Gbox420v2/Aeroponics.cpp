#include "Aeroponics.h"
#include "GrowBox.h"

// This is a virtual class (cannot be instanciated), parent of two classes:
// -Aeroponics_NoTank: High pressure pump is directly connected to the aeroponics tote
// -Aeroponics_Tank: A pressure tank is added between the high pressure pump and aeroponics tote, requires an extra solenoid (electric water valve)

Aeroponics::Aeroponics(const __FlashStringHelper * Name, GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings) : Common(Name){  //constructor
    this -> GBox = GBox;
    this -> BypassSolenoidPin = BypassSolenoidPin;
    this -> PumpPin = PumpPin;
    SprayEnabled = &DefaultSettings -> SprayEnabled;  //Enable/disable misting
    Interval = &DefaultSettings -> Interval; //Aeroponics - Spray every 15 minutes
    Duration = &DefaultSettings -> Duration; //Aeroponics - Spray time in seconds    
    PumpTimeout = &DefaultSettings -> PumpTimeout;  // Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
    PrimingTime = &DefaultSettings -> PrimingTime;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
}

void Aeroponics::websiteLoadEvent(){ //When the website is opened, load stuff once
  WebServer.setArgInt(getWebsiteComponentName(F("PumpTimeout")), *SprayEnabled);
  WebServer.setArgInt(getWebsiteComponentName(F("PrimingTime")), *Interval);
  WebServer.setArgInt(getWebsiteComponentName(F("Interval")), *Interval);
  WebServer.setArgInt(getWebsiteComponentName(F("Duration")), *Duration); 
} 

// void Aeroponics::websiteRefreshEvent(){ //When the website is opened, load stuff once
//   WebServer.setArgInt(getWebsiteComponentName(F("PumpTimeout")), *SprayEnabled);
//   WebServer.setArgInt(getWebsiteComponentName(F("PrimingTime")), *Interval);
//   WebServer.setArgInt(getWebsiteComponentName(F("Interval")), *Interval);
//   WebServer.setArgInt(getWebsiteComponentName(F("Duration")), *Duration); 
// }  

 void Aeroponics::report(){
  memset(&Message[0], 0, sizeof(Message));  //clear variable  
  strcat_P(Message,(PGM_P)F(" ; SprayEnabled:"));strcat(Message,yesNoToText(SprayEnabled));
  strcat_P(Message,(PGM_P)F(" ; Interval:"));strcat(Message,toText(*Interval));
  strcat_P(Message,(PGM_P)F(" ; Duration:"));strcat(Message,toText(*Duration));
  strcat_P(Message,(PGM_P)F(" ; PumpTimeout:"));strcat(Message,toText(*PumpTimeout));
  strcat_P(Message,(PGM_P)F(" ; PrimingTime:"));strcat(Message,toText(*PrimingTime));
  logToSerials( &Message, true, 0); //Break line, No indentation needed: child class already printed it 
 }

void Aeroponics::setPumpOn(bool UserRequest){
  BypassActive  = UserRequest; //If pump was turned on from the web interface first run an air bleeding cycle
  PumpOn = true;
  PumpTimer = millis();
  checkRelays();
  if(UserRequest){  //if the pump was turned on from the web interface, not by the automation
    GBox -> addToLog(F("Pump ON"));
    GBox -> Sound1 -> playOnSound();
    PumpOK = true; 
  }
}

void Aeroponics::setPumpOff(bool UserRequest){  
  BypassActive = false;
  PumpOn = false;
  if(!BlowOffInProgress)BypassSolenoidOn = false; //Close bypass valve
  PumpTimer = millis();        
  checkRelays();
  if(UserRequest){  //if the pump was turned off from the web interface, not by the automation
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


