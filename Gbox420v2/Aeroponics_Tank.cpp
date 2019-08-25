#include "Aeroponics_Tank.h"
#include "GrowBox.h"

Aeroponics_Tank::Aeroponics_Tank(GrowBox * GBox, byte SpraySolenoidPin, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_TankSpecific * TankSpecificSettings) : Aeroponics(GBox, BypassSolenoidPin, PumpPin, DefaultSettings){  //constructor
  this -> SpraySolenoidPin = SpraySolenoidPin;
  QuietFromHour = &TankSpecificSettings -> QuietFromHour;  //Quiet time to block pump - hour
  QuietFromMinute = &TankSpecificSettings -> QuietFromMinute; //Quiet time to block pump - minute
  QuietToHour = &TankSpecificSettings -> QuietToHour; //Quiet time end - hour
  QuietToMinute = &TankSpecificSettings -> QuietToMinute; //Quiet time end - minute
  QuietEnabled = &TankSpecificSettings -> QuietEnabled;  //Enable/disable quiet time then pump should not run
  RefillBeforeQuiet = &TankSpecificSettings -> RefillBeforeQuiet; //Enable/disable refill before quiet time
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("Aeroponics_Tank object created"),true);}
 }    

void Aeroponics_Tank::refresh(){ //when pressure tank is connected
if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("Aeroponics_Tank refreshing"),true);}  
  if(PumpOn){ //if pump is on
    if(AeroPressure >= *PressureHigh){ //refill complete, target pressure reached
     setPumpOff(false);
     logToSerials(F("Pressure tank recharged"),true);
    }
    else{
      if( millis() - PumpTimer >= (uint32_t)(*PumpTimeout * 1000)){ //If pump timeout reached
        setPumpOff(false);
        if(!BypassActive) {  //if bypass was not active and refill did not finish within timeout= pump must be broken
          PumpDisable();  //automatically disable pump if it is suspected to be broken
        //   sendEmailAlert(F("Aeroponics%20pump%20failed"));
        }
        logToSerials(F("Pump timeout reached"),true);       
      }
       if (!BypassActive && BypassSolenoidOn && millis() - PumpTimer >= (uint32_t)(*PrimingTime * 1000)){ //self priming timeout reached, time to start refilling
          if(MySettings.DebugEnabled)logToSerials(F("Starting refill"),true); 
          BypassSolenoidOn = false;      
          PumpTimer = millis(); //reset timer to start measuring spray time
        }      
    }
  }
  if( PumpOK && checkQuietTime() && AeroPressure <= *PressureLow){ //if pump is not disabled and quiet time not active and Pressure reached low limit: turn on pump 
        if(!PumpOn && !BypassSolenoidOn){ //start the bypass
          if(MySettings.DebugEnabled)logToSerials(F("Starting bypass"),true);
          BypassSolenoidOn = true; 
          PumpOn = true;
        }  
       
    }   
 if(SpraySolenoidOn){ //if spray is on
    if(millis() - SprayTimer >= (uint32_t)(*Duration * 1000)){  //if time to stop spraying (Duration in Seconds)
      SpraySolenoidOn = false;
      logToSerials(F("Stopping spray"),true);
      GBox -> Sound1 -> playOffSound();
      SprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(*SprayEnabled && millis() - SprayTimer >= (uint32_t)(*Interval * 60000)){ //if time to start spraying (AeroInterval in Minutes)
        SpraySolenoidOn = true;
        GBox -> Sound1 -> playOnSound();
        if(MySettings.DebugEnabled)logToSerials(F("Starting spray"),true);
        SprayTimer = millis();
    }    
  }  
}

// void Aeroponics_Tank::checkAeroPumpAlerts_WithPressureTank()
// {
//   if(MySettings.PressureAlertLow <= AeroPressure && AeroPressure <= MySettings.PressureAlertHigh){ //If pressure is between OK range
//     if(PreviousPressureRead != true){PressureTriggerCount = 0;}
//     else{ if(!PressureOK)PressureTriggerCount++; } 
//     PreviousPressureRead = true;     
     
//     if(!PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ // pressure was not OK before
//        PressureOK = true;
//     //    sendEmailAlert(F("Aeroponics%20pressure%20OK"));
//        } 
//   }
//   else{
//     if(AeroPressure > MySettings.PressureAlertHigh){ //Pressure over limit - emergency spraying
//           setPumpOff(false); //force pump off
//          // aeroSprayNow(true); //try to release pressure  
//     }
//     if(PreviousPressureRead != false){PressureTriggerCount = 0;}
//     else{ if(PressureOK)PressureTriggerCount++; }  //count out of range readings while pressure is considered OK and an alert is not active.
//     PreviousPressureRead = false;  
    
//     if(PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ //trigger an alert if the out of range reading counter is above the limit
//         PressureOK = false;
//         if(AeroPressure > MySettings.PressureAlertHigh){ //If high pressure alert level is reached   
//         //   sendEmailAlert(F("Aeroponics%20pressure%20too%20high"));
//           GBox -> addToLog(F("High pressure warning"));
//         }
//         if(AeroPressure < MySettings.PressureAlertLow){ //If low pressure alert level is reached
//           //if(PumpOK) setPumpOn(false); //Uncomment this to turn pump on even under quiet time
//         //   sendEmailAlert(F("Aeroponics%20pressure%20too%20low"));
//           GBox -> addToLog(F("Low pressure warning"));
//         } 
//       }
//    }     
// }

void Aeroponics_Tank::setQuietOnOff(bool State){
  *QuietEnabled = State;
  if(*QuietEnabled){ 
    GBox -> addToLog(F("Quiet mode enabled"));
    GBox -> Sound1 -> playOnSound();
    }
  else {
    GBox -> addToLog(F("Quiet mode disabled"));
    GBox -> Sound1 -> playOffSound();
    }
}

//Quiet time section: Blocks running the pump in a pre-defined time range
bool Aeroponics_Tank::checkQuietTime() {  
  if(*QuietEnabled){
    time_t Now = now(); // Get the current time
    int CombinedFromTime = *QuietFromHour * 100 + *QuietFromMinute;
    int CombinedToTime = *QuietToHour * 100 + *QuietToMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if(*RefillBeforeQuiet && PumpOK && CombinedFromTime == CombinedCurrentTime && millis() - LastRefill > 120000 ){
      setPumpOn(false); 
      LastRefill = millis();
    }
    if(CombinedFromTime <= CombinedToTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if(CombinedFromTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedToTime){return false;} //do not allow running the pump
      else  return true;  //allow running   
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedFromTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedToTime){return false; } //do not allow running the pump
      else  return true;  //allow running    
    }
  }
  else return true; //always allow if quiet mode is not enabled
  return false;  //getting rid of  warning: control reaches end of non-void function
}

void Aeroponics_Tank::setQuietRefillOnOff(bool State){
  *RefillBeforeQuiet = State;
  if(*RefillBeforeQuiet){ 
    GBox -> addToLog(F("Refill before quiet enabled"));
    GBox -> Sound1 -> playOnSound();
    }
  else {
    GBox -> addToLog(F("Refill before quiet disabled"));
    GBox -> Sound1 -> playOffSound();
    }
}



void Aeroponics_Tank::aeroSprayNow(bool DueToHighPressure){   
  if(*SprayEnabled || DueToHighPressure){
    BypassActive = false;
    SprayTimer = millis();
    SpraySolenoidOn = true;     
    checkRelays();
    GBox -> Sound1 -> playOnSound();
    if(DueToHighPressure) GBox -> addToLog(F("Above pressure limit,spraying"));
    else GBox -> addToLog(F("Aeroponics spraying"));
    }
}

void Aeroponics_Tank::aeroSprayOff(){    
    SpraySolenoidOn = false; 
    checkRelays();
    GBox -> addToLog(F("Aeroponics spray OFF"));
}