void checkAeroSprayTimer_WithPressureTank(){ //when pressure tank is connected
  if(AeroPumpOn){ //if pump is on
    if(AeroPressure >= MySettings.AeroPressureHigh){ //refill complete, target pressure reached
     aeroPumpOff(false);
     logToSerials(F("Pressure tank recharged"),true);
    }
    else{
      if( millis() - AeroPumpTimer >= MySettings.AeroPumpTimeout * 1000){ //If pump timeout reached
        aeroPumpOff(false);
        if(!AeroBypassActive) {  //if bypass was not active and refill did not finish within timeout= pump must be broken
          aeroPumpDisable();  //automatically disable pump if it is suspected to be broken
          sendEmailAlert(F("Aeroponics%20pump%20failed"));
        }
        logToSerials(F("Pump timeout reached"),true);       
      }
       if (!AeroBypassActive && AeroBypassSolenoidOn && millis() - AeroPumpTimer >= MySettings.AeroPrimingTime * 1000){ //self priming timeout reached, time to start refilling
          if(MySettings.DebugEnabled)logToSerials(F("Starting refill"),true); 
          AeroBypassSolenoidOn = false;      
          AeroPumpTimer = millis(); //reset timer to start measuring spray time
        }      
    }
  }
  if( PumpOK && checkQuietTime() && AeroPressure <= MySettings.AeroPressureLow){ //if pump is not disabled and quiet time not active and Pressure reached low limit: turn on pump 
        if(!AeroPumpOn && !AeroBypassSolenoidOn){ //start the bypass
          if(MySettings.DebugEnabled)logToSerials(F("Starting bypass"),true);
          AeroBypassSolenoidOn = true; 
          AeroPumpOn = true;
        }  
       
    }   
 if(AeroSpraySolenoidOn){ //if spray is on
    if(millis() - AeroSprayTimer >= MySettings.AeroDuration * 1000){  //if time to stop spraying (AeroDuration in Seconds)
      AeroSpraySolenoidOn = false;
      logToSerials(F("Stopping spray"),true);
      PlayOffSound = true;
      AeroSprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(MySettings.AeroSprayEnabled && millis() - AeroSprayTimer >= MySettings.AeroInterval * 60000){ //if time to start spraying (AeroInterval in Minutes)
        AeroSpraySolenoidOn = true;
        PlayOnSound = true;
        if(MySettings.DebugEnabled)logToSerials(F("Starting spray"),true);
        AeroSprayTimer = millis();
    }    
  }  
}

bool PreviousPressureRead = true;
void checkAeroPumpAlerts_WithPressureTank()
{
  if(MySettings.PressureAlertLow <= AeroPressure && AeroPressure <= MySettings.PressureAlertHigh){ //If pressure is between OK range
    if(PreviousPressureRead != true){PressureTriggerCount = 0;}
    else{ if(!PressureOK)PressureTriggerCount++; } 
    PreviousPressureRead = true;     
     
    if(!PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ // pressure was not OK before
       PressureOK = true;
       sendEmailAlert(F("Aeroponics%20pressure%20OK"));
       } 
  }
  else{
    if(AeroPressure > MySettings.PressureAlertHigh){ //Pressure over limit - emergency spraying
          aeroPumpOff(false); //force pump off
         // aeroSprayNow(true); //try to release pressure  
    }
    if(PreviousPressureRead != false){PressureTriggerCount = 0;}
    else{ if(PressureOK)PressureTriggerCount++; }  //count out of range readings while pressure is considered OK and an alert is not active.
    PreviousPressureRead = false;  
    
    if(PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ //trigger an alert if the out of range reading counter is above the limit
        PressureOK = false;
        if(AeroPressure > MySettings.PressureAlertHigh){ //If high pressure alert level is reached   
          sendEmailAlert(F("Aeroponics%20pressure%20too%20high"));
          addToLog(F("High pressure warning"));
        }
        if(AeroPressure < MySettings.PressureAlertLow){ //If low pressure alert level is reached
          //if(PumpOK) aeroPumpOn(false); //Uncomment this to turn pump on even under quiet time
          sendEmailAlert(F("Aeroponics%20pressure%20too%20low"));
          addToLog(F("Low pressure warning"));
        } 
      }
   }     
}
void setQuietOnOff(bool State){
  MySettings.AeroQuietEnabled = State;
  if(MySettings.AeroQuietEnabled){ 
    addToLog(F("Quiet mode enabled"));
    PlayOnSound=true;
    }
  else {
    addToLog(F("Quiet mode disabled"));
    PlayOffSound=true;
    }
}

void setQuietRefillOnOff(bool State){
  MySettings.AeroRefillBeforeQuiet = State;
  if(MySettings.AeroRefillBeforeQuiet){ 
    addToLog(F("Refill before quiet enabled"));
    PlayOnSound=true;
    }
  else {
    addToLog(F("Refill before quiet disabled"));
    PlayOffSound=true;
    }
}

