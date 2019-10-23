#include "Aeroponics_NoTank.h"
#include "GrowBox.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper * Name, GrowBox * GBox, Settings::AeroponicsSettings * DefaultSettings, Settings::AeroponicsSettings_NoTankSpecific * NoTankSpecificSettings) : Aeroponics(&(*Name), &(*GBox), &(*DefaultSettings)) {
  BlowOffTime = &NoTankSpecificSettings -> BlowOffTime; //Aeroponics - Turn on pump below this pressure (bar)
  logToSerials(F("Aeroponics_NoTank object created"),true);
}

void Aeroponics_NoTank::websiteLoadEvent(char * url){ //When the website is opened, load stuff once
  WebServer.setArgInt(getWebsiteComponentName(F("BlowOffTime")), *BlowOffTime);
  Aeroponics::websiteLoadEvent(url); 
}

void Aeroponics_NoTank::websiteFieldSubmitEvent(char * Field){ //When the website is opened, load stuff once
  if(!isThisMyComponent(Field)) {  //check if component name matches class. If it matches: fills ShortMessage global variable with the button function 
    return;  //If did not match:return control to caller fuction
  }
  else{ //if the component name matches with the object name     
    if(strcmp_P(ShortMessage,(PGM_P)F("BlowOffTime"))==0) {setBlowOffTime(WebServer.getArgInt());}
    else Aeroponics::websiteFieldSubmitEvent(Field);
  }
}  

void Aeroponics_NoTank::report(){
  memset(&LongMessage[0], 0, sizeof(LongMessage));  //clear variable  
  strcat_P(LongMessage,(PGM_P)F("BlowOffTime:"));strcat(LongMessage,toText(*BlowOffTime));
  logToSerials(&LongMessage, false,4); //first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();  //then print parent class report
 }

void Aeroponics_NoTank::refresh(){ //pump directly connected to aeroponics tote, with an electronically controlled bypass valve
  Common::refresh();
  // Serial.print("SprayTimer: "); 
  // Serial.print(millis() - SprayTimer);  
  // Serial.print("/"); 
  // Serial.println((uint32_t)*Interval * 60000); 

  // Serial.print("PumpTimer: "); 
  // Serial.print(millis() - PumpTimer);  
  // Serial.print("/"); 
  // Serial.println((uint32_t)*PumpTimeout * 60000); 

  // Serial.print("DurationTimer: "); 
  // Serial.print(millis() - SprayTimer);  
  // Serial.print("/"); 
  // Serial.println((uint32_t)*Duration * 1000); 


  if (BlowOffInProgress && millis() - SprayTimer >= ((uint32_t)*BlowOffTime * 1000)){   //checking pressure blow-off timeout
      BypassSolenoidOn = false; //Close bypass valve
      BlowOffInProgress = false;
      logToSerials(F("Stopping blow-off"),true);
 }
 if(PumpOn)    { //if pump is on
    if (millis() - PumpTimer >= ((uint32_t)*PumpTimeout * 60000)){   //checking pump timeout
      setPumpOff(false);
      logToSerials(F("Pump timeout reached"),true);
    }
    
    if(!MixInProgress && !BypassSolenoidOn && PumpOn && millis() - SprayTimer >= ((uint32_t)*Duration * 1000)){  //bypass valve is closed and time to stop spraying (Duration in Seconds)
      BypassSolenoidOn = true;
      BlowOffInProgress = true; //no extra time is needed, will use SprayTimer
      checkRelays();
      setPumpOff(false);
      SprayTimer = millis();    
      logToSerials(F("Stopping spray"),true);      
    }
    else{
      if (!MixInProgress && BypassSolenoidOn && millis() - PumpTimer >= ((uint32_t)*PrimingTime * 1000)){ //self priming timeout reached, time to start spraying
        BypassSolenoidOn = false; //Close bypass valve
        checkRelays();
        SprayTimer = millis();
        logToSerials(F("Closing bypass, starting spray"),true);        
        }
       }
  }
  else{ //pump is off
    if (!BlowOffInProgress)BypassSolenoidOn = false; //Should not leave the solenoid turned on
    if(PumpOK && *SprayEnabled  && ((millis() - SprayTimer) >= ((uint32_t)*Interval * 60000))){ //if time to start spraying (AeroInterval in Minutes)
      sprayNow(false);                     
    }    
  } 
  checkRelays();
  if(RefreshCounter++%60 == 0) report();  //Report only every 60 seconds - Refresh() function is called every second in the Aeroponics component
}


void Aeroponics_NoTank::sprayNow(bool DueToHighPressure){   
  if(*SprayEnabled || DueToHighPressure){
    MixInProgress = false;    
    PumpOK = true;
    PumpOn = true;
    BypassSolenoidOn = true;
    checkRelays();
    PumpTimer = millis();
    SprayTimer = millis();    
    GBox -> Sound1 -> playOnSound();
    logToSerials(F("Aeroponics spraying"),true,3);
    }
}


// void Aeroponics_NoTank::checkAlerts()
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


void Aeroponics_NoTank::setBlowOffTime(int _BlowOffTime){
  *BlowOffTime = _BlowOffTime;
  GBox -> addToLog(F("Blowoff time updated"));
}

void Aeroponics_NoTank::sprayOff(){    
    PumpOn = false; 
    BypassSolenoidOn = false; 
    checkRelays();
    GBox -> addToLog(F("Aeroponics spray OFF"));
}