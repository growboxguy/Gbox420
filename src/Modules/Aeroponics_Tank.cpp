#include "Aeroponics_Tank.h"
#include "../../GrowBox.h"

Aeroponics_Tank::Aeroponics_Tank(const __FlashStringHelper * Name, GrowBox * GBox, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_TankSpecific * TankSpecificSettings, PressureSensor * FeedbackPressureSensor) : Aeroponics(&(*Name), &(*GBox), &(*DefaultSettings), &(*FeedbackPressureSensor)){  //constructor
  SpraySolenoidPin = &TankSpecificSettings -> SpraySolenoidPin;
  PressureLow = &TankSpecificSettings -> PressureLow; //Aeroponics - Turn on pump below this pressure (bar)
  PressureHigh = &TankSpecificSettings -> PressureHigh; //Aeroponics - Turn off pump above this pressure (bar)
  QuietEnabled = &TankSpecificSettings -> QuietEnabled;  //Enable/disable quiet time then pump should not run
  RefillBeforeQuiet = &TankSpecificSettings -> RefillBeforeQuiet; //Enable/disable refill before quiet time
  QuietFromHour = &TankSpecificSettings -> QuietFromHour;  //Quiet time to block pump - hour
  QuietFromMinute = &TankSpecificSettings -> QuietFromMinute; //Quiet time to block pump - minute
  QuietToHour = &TankSpecificSettings -> QuietToHour; //Quiet time end - hour
  QuietToMinute = &TankSpecificSettings -> QuietToMinute; //Quiet time end - minute
  pinMode(*SpraySolenoidPin,OUTPUT);
  digitalWrite(*SpraySolenoidPin,HIGH);  //initialize off 
  logToSerials(F("Aeroponics_Tank object created"),true,1);
}   

void Aeroponics_Tank::websiteEvent_Field(char * Field){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Field)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name
    //if(strcmp_P(field,(PGM_P)F("QuietEnabled"))==0) {setQuietOnOff(WebServer.getArgBoolean());}
    //else if(strcmp_P(field,(PGM_P)F("RefillBeforeQuiet"))==0) {setQuietRefillOnOff(WebServer.getArgBoolean());}
    // else if(strcmp_P(field,(PGM_P)F("QuietFromHour"))==0) {setQuietFromHour(WebServer.getArgInt());}
    // else if(strcmp_P(field,(PGM_P)F("QuietFromMinute"))==0) {setQuietFromMinute(WebServer.getArgInt());}
    // else if(strcmp_P(field,(PGM_P)F("QuietToHour"))==0) {setQuietToHour(WebServer.getArgInt());}
    // else if(strcmp_P(field,(PGM_P)F("QuietToMinute"))==0) {setQuietToMinute(WebServer.getArgInt());}     
    Aeroponics::websiteEvent_Field(Field);
  }
}

void Aeroponics_Tank::refresh_Sec(){ 
  if(GBox -> BoxSettings -> DebugEnabled) Common::refresh_Sec();
  if(PumpOn){ //if pump is on
    if(Aeroponics::FeedbackPressureSensor -> getPressure() >= *PressureHigh){ //refill complete, target pressure reached
     setPumpOff(false);
     logToSerials(F("Pressure tank recharged"),true);
    }
    else{
      if( millis() - PumpTimer >= ((uint32_t)*PumpTimeout * 60000)){ //If pump timeout reached
        setPumpOff(false); //turn of pump, 
        if(!MixInProgress) {  //if mixing was not active and refill did not finish within timeout= pump must be broken
          PumpDisable();  //automatically disable pump if it is suspected to be broken
        //   sendEmailAlert(F("Aeroponics%20pump%20failed"));
        }
        logToSerials(F("Pump timeout reached"),true);       
      }
       if (!MixInProgress && BypassSolenoidOn && millis() - PumpTimer >= ((uint32_t)*PrimingTime * 1000)){ //self priming timeout reached, time to start refilling
          if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Starting refill"),true); 
          BypassSolenoidOn = false;      
          PumpTimer = millis(); //reset timer to start measuring refill time
        }      
    }
  }
  if( PumpOK && checkQuietTime() && Aeroponics::FeedbackPressureSensor -> getPressure() <= *PressureLow){ //if pump is not disabled and quiet time not active and Pressure reached low limit: turn on pump 
        if(!PumpOn && !BypassSolenoidOn){ //start the bypass
          if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Starting bypass"),true);
          BypassSolenoidOn = true; 
          PumpOn = true;
        }  
       
    }   
  if(SpraySolenoidOn){ //if spray is on
    if(millis() - SprayTimer >= ((uint32_t)*Duration * 1000)){  //if time to stop spraying (Duration in Seconds)
      SpraySolenoidOn = false;
      logToSerials(F("Stopping spray"),true);
      GBox -> Sound1 -> playOffSound();
      SprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(*SprayEnabled && millis() - SprayTimer >= ((uint32_t)*Interval * 60000)){ //if time to start spraying (AeroInterval in Minutes)
        SpraySolenoidOn = true;
        GBox -> Sound1 -> playOnSound();
        if(GBox -> BoxSettings -> DebugEnabled)logToSerials(F("Starting spray"),true);
        SprayTimer = millis();
    }    
  } 
  checkRelays();  
}

void Aeroponics_Tank::report(){
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable  
  //strcat_P(LongMessage,(PGM_P)F("Pressure:"));strcat(LongMessage,pressureToText(AeroPressure));
  //strcat_P(LongMessage,(PGM_P)F(" ["));strcat(LongMessage,toText(*PressureLow,*PressureHigh,"/"));
  //strcat_P(LongMessage,(PGM_P)F("]"));
  logToSerials( &LongMessage, false,1); //first print Aeroponics_Tank specific report, without a line break  
  Aeroponics::report();  //then print parent class report  
  memset(&LongMessage[0], 0, sizeof(LongMessage));    
  strcat_P(LongMessage,(PGM_P)F("QuietEnabled:"));strcat(LongMessage,yesNoToText(*QuietEnabled));
  strcat_P(LongMessage,(PGM_P)F(" ; RefillBeforeQuiet:"));strcat(LongMessage,yesNoToText(*RefillBeforeQuiet));
  strcat_P(LongMessage,(PGM_P)F(" ; QuietFrom:"));strcat(LongMessage,timeToText(*QuietFromHour,*QuietFromMinute));
  strcat_P(LongMessage,(PGM_P)F(" ; QuietTo:"));strcat(LongMessage,timeToText(*QuietToHour,*QuietToMinute));
  logToSerials( &LongMessage, true,1); //Print rarely used settings last  
 }

void Aeroponics_Tank::checkRelays(){
  //logToSerials(F("Aeroponics_Tank checking relays:"),false,0);
  //logToSerials(SpraySolenoidOn,true,0);
  if(SpraySolenoidOn) digitalWrite(*SpraySolenoidPin, LOW); else digitalWrite(*SpraySolenoidPin, HIGH); 
  Aeroponics::checkRelays();
}

void Aeroponics_Tank::setPressureLow(float PressureLow){
  *(this -> PressureLow) =  PressureLow;
}

void Aeroponics_Tank::setPressureHigh(float PressureHigh){
  *(this -> PressureHigh) = PressureHigh;
  GBox -> addToLog(F("Pump settings updated"));
}

// void Aeroponics_Tank::checkAeroPumpAlerts_WithPressureTank()
// {
//   if(GBox -> BoxSettings -> PressureAlertLow <= Aeroponics::FeedbackPressureSensor -> getPressure() && Aeroponics::FeedbackPressureSensor -> getPressure() <= GBox -> BoxSettings -> PressureAlertHigh){ //If pressure is between OK range
//     if(PreviousPressureRead != true){PressureTriggerCount = 0;}
//     else{ if(!PressureOK)PressureTriggerCount++; } 
//     PreviousPressureRead = true;     
     
//     if(!PressureOK && PressureTriggerCount>=GBox -> BoxSettings -> ReadCountBeforeAlert){ // pressure was not OK before
//        PressureOK = true;
//     //    sendEmailAlert(F("Aeroponics%20pressure%20OK"));
//        } 
//   }
//   else{
//     if(Aeroponics::FeedbackPressureSensor -> getPressure() > GBox -> BoxSettings -> PressureAlertHigh){ //Pressure over limit - emergency spraying
//           setPumpOff(false); //force pump off
//          // sprayNow(true); //try to release pressure  
//     }
//     if(PreviousPressureRead != false){PressureTriggerCount = 0;}
//     else{ if(PressureOK)PressureTriggerCount++; }  //count out of range readings while pressure is considered OK and an alert is not active.
//     PreviousPressureRead = false;  
    
//     if(PressureOK && PressureTriggerCount>=GBox -> BoxSettings -> ReadCountBeforeAlert){ //trigger an alert if the out of range reading counter is above the limit
//         PressureOK = false;
//         if(Aeroponics::FeedbackPressureSensor -> getPressure() > GBox -> BoxSettings -> PressureAlertHigh){ //If high pressure alert level is reached   
//         //   sendEmailAlert(F("Aeroponics%20pressure%20too%20high"));
//           GBox -> addToLog(F("High pressure warning"));
//         }
//         if(Aeroponics::FeedbackPressureSensor -> getPressure() < GBox -> BoxSettings -> PressureAlertLow){ //If low pressure alert level is reached
//           //if(PumpOK) setPumpOn(false); //Uncomment this to turn pump on even under quiet time
//         //   sendEmailAlert(F("Aeroponics%20pressure%20too%20low"));
//           GBox -> addToLog(F("Low pressure warning"));
//         } 
//       }
//    }     
// }

//Quiet time section: Blocks running the pump in a pre-defined time range

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

void Aeroponics_Tank::sprayNow(bool DueToHighPressure){   
  if(*SprayEnabled || DueToHighPressure){
    MixInProgress = false;
    SprayTimer = millis();
    SpraySolenoidOn = true;     
    checkRelays();
    GBox -> Sound1 -> playOnSound();
    if(DueToHighPressure) GBox -> addToLog(F("Above pressure limit,spraying"));
    else GBox -> addToLog(F("Aeroponics spraying"));
    }
}

void Aeroponics_Tank::sprayOff(){    
    SpraySolenoidOn = false; 
    checkRelays();
    GBox -> addToLog(F("Aeroponics spray OFF"));
}