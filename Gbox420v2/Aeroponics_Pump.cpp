void checkAeroSprayTimer_WithoutPressureTank(){ //pump directly connected to aeroponics tote, with an electronically controlled bypass valve
 if (AeroBlowOffInProgress && millis() - AeroSprayTimer >= AeroBlowOffTime * 1000){   //checking pressure blow-off timeout
      AeroBypassSolenoidOn = false; //Close bypass valve
      AeroBlowOffInProgress = false;
      logToSerials(F("Stopping blow-off"),true);
 }
 if(AeroPumpOn)    { //if pump is on
    if (millis() - AeroPumpTimer >= MySettings.AeroPumpTimeout * 1000){   //checking pump timeout
      aeroPumpOff(false);
      logToSerials(F("Pump timeout reached"),true);
    }
    
    if(!AeroBypassActive && !AeroBypassSolenoidOn && AeroPumpOn && millis() - AeroSprayTimer >= MySettings.AeroDuration * 1000){  //bypass valve is closed and time to stop spraying (AeroDuration in Seconds)
      if(MySettings.AeroBlowOff){
          AeroBypassSolenoidOn = true;
          AeroBlowOffInProgress = true; //no extra time is needed, will use AeroSprayTimer
          checkRelays();
        }
      aeroPumpOff(false);    
      logToSerials(F("Stopping spray"),true);
      AeroSprayTimer = millis();
    }
    else{
      if (!AeroBypassActive && AeroBypassSolenoidOn && millis() - AeroPumpTimer >= MySettings.AeroPrimingTime * 1000){ //self priming timeout reached, time to start spraying
        AeroBypassSolenoidOn = false; //Close bypass valve
        logToSerials(F("Closing bypass, starting spray"),true);
        AeroSprayTimer = millis();
        }
       }
  }
  else{ //pump is off
    if (!AeroBlowOffInProgress)AeroBypassSolenoidOn = false; //Should not leave the solenoid turned on
    if(PumpOK && MySettings.AeroSprayEnabled  && millis() - AeroSprayTimer >= MySettings.AeroInterval * 60000){ //if time to start spraying (AeroInterval in Minutes)
      aeroSprayNow(false);                     
    }    
  } 
}


void checkAeroPumpAlerts_WithoutPressureTank()
{
  if(AeroPumpOn){ //Only if pump is running
    if(!AeroBypassSolenoidOn){ //when bypass is closed and should be spraying
      if( MySettings.PressureAlertLow <= AeroPressure && AeroPressure <= MySettings.PressureAlertHigh){ //If pressure is between OK range
        if(PreviousPressureRead != true){PressureTriggerCount = 0;}
        else{ if(!PressureOK)PressureTriggerCount++; } 
        PreviousPressureRead = true;     
         
        if(!PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ // pressure was not OK before
           PressureOK = true;
           sendEmailAlert(F("Aeroponics%20pressure%20OK"));
           } 
       }
       else{ //if pressure is not OK
          if(PreviousPressureRead != false){PressureTriggerCount = 0;}
          else{ if(PressureOK)PressureTriggerCount++; } 
          PreviousPressureRead = false;  
          
          if(PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){
              PressureOK = false;
              if(AeroPressure > MySettings.PressureAlertHigh){ //If high pressure alert level is reached  
                aeroPumpOff(false); //force pump off 
                sendEmailAlert(F("Aeroponics%20pressure%20too%20high"));
                addToLog(F("High pressure warning"));
              }
              if(AeroPressure < MySettings.PressureAlertLow){ //If low pressure alert level is reached
                sendEmailAlert(F("Aeroponics%20pressure%20too%20low"));
                addToLog(F("Low pressure warning"));
              } 
            }
         }
    }     
  }
  else  {//if pump is off: should be no pressure
        if((MySettings.MetricSystemEnabled && AeroPressure <= 0.3 )  || (!MySettings.MetricSystemEnabled && AeroPressure <= 5)){ //Checking if readings are close to 0
          if(PreviousPressureRead != true){PressureTriggerCount = 0;}
          else{ if(!PressureOK)PressureTriggerCount++; } 
          PreviousPressureRead = true;     
           
          if(!PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ // pressure was not OK before
             PressureOK = true;
             sendEmailAlert(F("Aeroponics%20pressure%20OK"));
             } 
         }
         else{ //if pressure is not OK
            if(PreviousPressureRead != false){PressureTriggerCount = 0;}
            else{ if(PressureOK)PressureTriggerCount++; } 
            PreviousPressureRead = false;  
            
            if(PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){
                PressureOK = false;
                  sendEmailAlert(F("Aeroponics%20sensor%20malfunction"));
                  addToLog(F("Pressure sensor malfunction"));
              }
           }     
  }
}
