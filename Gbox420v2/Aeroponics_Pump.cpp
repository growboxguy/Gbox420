#include "Aeroponics_Pump.h"
#include "GrowBox.h"

Aeroponics_Pump::Aeroponics_Pump(GrowBox * GBox, byte BypassSolenoidPin, byte PumpPin, Settings::AeroponicsSettings * DefaultSettings) : Aeroponics(&(*GBox), BypassSolenoidPin, PumpPin, &(*DefaultSettings)) {
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("Aeroponics_Pump object created"),true);}
}

 void Aeroponics_Pump::report(){
   ;
 }

void Aeroponics_Pump::refresh(){ //pump directly connected to aeroponics tote, with an electronically controlled bypass valve
  if(GBox -> BoxSettings -> DebugEnabled){logToSerials(F("Aeroponics_Pump refreshing"),true);}  
 if (BlowOffInProgress && millis() - SprayTimer >= (uint32_t)(BlowOffTime * 1000)){   //checking pressure blow-off timeout
      BypassSolenoidOn = false; //Close bypass valve
      BlowOffInProgress = false;
      logToSerials(F("Stopping blow-off"),true);
 }
 if(PumpOn)    { //if pump is on
    if (millis() - PumpTimer >= (uint32_t)(*PumpTimeout * 1000)){   //checking pump timeout
      setPumpOff(false);
      logToSerials(F("Pump timeout reached"),true);
    }
    
    if(!BypassActive && !BypassSolenoidOn && PumpOn && millis() - SprayTimer >= (uint32_t)(*Duration * 1000)){  //bypass valve is closed and time to stop spraying (Duration in Seconds)
      BypassSolenoidOn = true;
      BlowOffInProgress = true; //no extra time is needed, will use SprayTimer
      checkRelays();
      setPumpOff(false);    
      logToSerials(F("Stopping spray"),true);
      SprayTimer = millis();
    }
    else{
      if (!BypassActive && BypassSolenoidOn && millis() - PumpTimer >= (uint32_t)(*PrimingTime * 1000)){ //self priming timeout reached, time to start spraying
        BypassSolenoidOn = false; //Close bypass valve
        logToSerials(F("Closing bypass, starting spray"),true);
        SprayTimer = millis();
        }
       }
  }
  else{ //pump is off
    if (!BlowOffInProgress)BypassSolenoidOn = false; //Should not leave the solenoid turned on
    if(PumpOK && *SprayEnabled  && millis() - SprayTimer >= (uint32_t)(*Interval * 60000)){ //if time to start spraying (AeroInterval in Minutes)
      aeroSprayNow(false);                     
    }    
  } 
}


void Aeroponics_Pump::aeroSprayNow(bool DueToHighPressure){   
  if(*SprayEnabled || DueToHighPressure){
    BypassActive = false;
    SprayTimer = millis();
      PumpOK = true;
      PumpOn = true;
      BypassSolenoidOn = true;
      PumpTimer = millis();
    checkRelays();
    GBox -> Sound1 -> playOnSound();
    if(DueToHighPressure) GBox -> addToLog(F("Above pressure limit,spraying"));
    else GBox -> addToLog(F("Aeroponics spraying"));
    }
}



// void Aeroponics_Pump::checkAeroPumpAlerts_WithoutPressureTank()
// {
//   if(PumpOn){ //Only if pump is running
//     if(!BypassSolenoidOn){ //when bypass is closed and should be spraying
//       if( GBox -> BoxSettings -> PressureAlertLow <= AeroPressure && AeroPressure <= GBox -> BoxSettings -> PressureAlertHigh){ //If pressure is between OK range
//         if(PreviousPressureRead != true){PressureTriggerCount = 0;}
//         else{ if(!PressureOK)PressureTriggerCount++; } 
//         PreviousPressureRead = true;     
         
//         if(!PressureOK && PressureTriggerCount>=GBox -> BoxSettings -> ReadCountBeforeAlert){ // pressure was not OK before
//            PressureOK = true;
//         //    sendEmailAlert(F("Aeroponics%20pressure%20OK"));
//            } 
//        }
//        else{ //if pressure is not OK
//           if(PreviousPressureRead != false){PressureTriggerCount = 0;}
//           else{ if(PressureOK)PressureTriggerCount++; } 
//           PreviousPressureRead = false;  
          
//           if(PressureOK && PressureTriggerCount>=GBox -> BoxSettings -> ReadCountBeforeAlert){
//               PressureOK = false;
//               if(AeroPressure > GBox -> BoxSettings -> PressureAlertHigh){ //If high pressure alert level is reached  
//                 setPumpOff(false); //force pump off 
//                 // sendEmailAlert(F("Aeroponics%20pressure%20too%20high"));
//                 GBox -> addToLog(F("High pressure warning"));
//               }
//               if(AeroPressure < GBox -> BoxSettings -> PressureAlertLow){ //If low pressure alert level is reached
//                 // sendEmailAlert(F("Aeroponics%20pressure%20too%20low"));
//                 GBox -> addToLog(F("Low pressure warning"));
//               } 
//             }
//          }
//     }     
//   }
//   else  {//if pump is off: should be no pressure
//         if((GBox -> BoxSettings -> MetricSystemEnabled && AeroPressure <= 0.3 )  || (!GBox -> BoxSettings -> MetricSystemEnabled && AeroPressure <= 5)){ //Checking if readings are close to 0
//           if(PreviousPressureRead != true){PressureTriggerCount = 0;}
//           else{ if(!PressureOK)PressureTriggerCount++; } 
//           PreviousPressureRead = true;     
           
//           if(!PressureOK && PressureTriggerCount>=GBox -> BoxSettings -> ReadCountBeforeAlert){ // pressure was not OK before
//              PressureOK = true;
//             //  sendEmailAlert(F("Aeroponics%20pressure%20OK"));
//              } 
//          }
//          else{ //if pressure is not OK
//             if(PreviousPressureRead != false){PressureTriggerCount = 0;}
//             else{ if(PressureOK)PressureTriggerCount++; } 
//             PreviousPressureRead = false;  
            
//             if(PressureOK && PressureTriggerCount>=GBox -> BoxSettings -> ReadCountBeforeAlert){
//                 PressureOK = false;
//                 //   sendEmailAlert(F("Aeroponics%20sensor%20malfunction"));
//                   GBox -> addToLog(F("Pressure sensor malfunction"));
//               }
//            }     
//   }
// }


void Aeroponics_Pump::aeroSprayOff(){    
    PumpOn = false; 
    BypassSolenoidOn = false; 
    checkRelays();
    GBox -> addToLog(F("Aeroponics spray OFF"));
}