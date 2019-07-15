void checkAero(bool Interrupt){
 checkAeroSprayTimer();
 if(!Interrupt) //Run some functions only when not called from an Interrupt. Within an Interroupt millis() counter doesn`t increase, so delay() never ends.
 {  
  if(AeroPumpOn & !AeroBypassSolenoidOn)readAeroPressure();  //When spraying is on: read the pressure
  checkAeroPumpAlerts(); 
 }
 setRelays(); //function from the Power tab
}

void checkAeroSprayTimer(){ //pump directly connected to aeroponics tote, with an electronically controlled bypass valve
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
      AeroBypassSolenoidOn = true;
      AeroBlowOffInProgress = true; //no extra time is needed, will use AeroSprayTimer
      setRelays();
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

void aeroSprayNow(bool CalledFromWebsite){ 
  if(CalledFromWebsite)MySettings.AeroSprayEnabled = true;
  if(MySettings.AeroSprayEnabled){
    AeroBypassActive = false;
    AeroSprayTimer = millis(); 
    PumpOK = true;
    AeroPumpOn = true;
    AeroBypassSolenoidOn = true;
    AeroPumpTimer = millis();      
    setRelays();
    PlayOnSound = true;
    addToLog(F("Aeroponics spraying"));
  }  
}

void aeroSprayOff(){    
    AeroPumpOn = false; 
    AeroBypassSolenoidOn = false;
    setRelays();
    addToLog(F("Aeroponics spray OFF"));
}

void aeroPumpOn(bool CalledFromWebsite){
  AeroBypassActive   = CalledFromWebsite; //If called from the Web interface keep the pump on
  AeroPumpOn = true;
  AeroPumpTimer = millis();
  setRelays();
  if(CalledFromWebsite){
    addToLog(F("Pump ON"));
    PlayOnSound = true;
    PumpOK = true; 
  }
}

void aeroPumpOff(bool CalledFromWebsite){  
  AeroBypassActive = false;
  AeroPumpOn = false;
  if(!AeroBlowOffInProgress)AeroBypassSolenoidOn = false; //Close bypass valve
  AeroPumpTimer = millis();         
  setRelays();
  if(CalledFromWebsite){ 
    addToLog(F("Pump OFF"));
    PlayOffSound = true;
    PumpOK = true; //re-enable pump 
  }
}

void aeroPumpDisable(){
  aeroPumpOff(false);
  PumpOK = false;
  addToLog(F("Pump disabled"));
}

void aeroMix(){
  AeroPumpOn = true;    
  AeroBypassActive = true; 
  PumpOK = true;
  AeroBypassSolenoidOn = true;
  AeroPumpTimer = millis();    
  setRelays();
  PlayOnSound = true;
  addToLog(F("Mixing nutrients"));
}

void setAeroSprayOnOff(bool State){
  MySettings.AeroSprayEnabled=State;
  if(MySettings.AeroSprayEnabled){ 
    addToLog(F("Aeroponics spray enabled"));
    PlayOnSound=true;} 
  else {
    addToLog(F("Aeroponics spray disabled"));
    PlayOffSound=true;}
}

bool PrevoiusPressureRead = true;
void checkAeroPumpAlerts()
{
  if(AeroPumpOn){ //Only if pump is running
    if(!AeroBypassSolenoidOn){ //when bypass is closed and should be spraying
      if( MySettings.PressureAlertLow <= AeroPressure && AeroPressure <= MySettings.PressureAlertHigh){ //If pressure is between OK range
        if(PrevoiusPressureRead != true){PressureTriggerCount = 0;}
        else{ if(!PressureOK)PressureTriggerCount++; } 
        PrevoiusPressureRead = true;     
         
        if(!PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ // pressure was not OK before
           PressureOK = true;
           sendEmailAlert(F("Aeroponics%20pressure%20OK"));
           } 
       }
       else{ //if pressure is not OK
          if(PrevoiusPressureRead != false){PressureTriggerCount = 0;}
          else{ if(PressureOK)PressureTriggerCount++; } 
          PrevoiusPressureRead = false;  
          
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
          if(PrevoiusPressureRead != true){PressureTriggerCount = 0;}
          else{ if(!PressureOK)PressureTriggerCount++; } 
          PrevoiusPressureRead = true;     
           
          if(!PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){ // pressure was not OK before
             PressureOK = true;
             sendEmailAlert(F("Aeroponics%20pressure%20OK"));
             } 
         }
         else{ //if pressure is not OK
            if(PrevoiusPressureRead != false){PressureTriggerCount = 0;}
            else{ if(PressureOK)PressureTriggerCount++; } 
            PrevoiusPressureRead = false;  
            
            if(PressureOK && PressureTriggerCount>=MySettings.ReadCountBeforeAlert){
                PressureOK = false;
                  sendEmailAlert(F("Aeroponics%20sensor%20malfunction"));
                  addToLog(F("Pressure sensor malfunction"));
              }
           }     
  }
}

RollingAvarage AeroPressureAverage;
void readAeroPressure(){ 
  int  Reading = AeroPressureAverage.updateAverage(analogRead(PressureSensorInPin)); //calculateRollingAverage(analogRead(PressureSensorInPin));
  if(MySettings.DebugEnabled){
   //logToSerials(AeroPressureAverage.GetDebug(),true);
  }  
  float Voltage = ((float)Reading) * 5 / 1024 ; 
  if(MySettings.MetricSystemEnabled) AeroPressure = MySettings.PressureSensorRatio*(Voltage-MySettings.PressureSensorOffset); // unit: bar / 100kPa
  else AeroPressure = MySettings.PressureSensorRatio*(Voltage-MySettings.PressureSensorOffset) * 14.5038;  //unit: PSI
}


void calibratePressureSensor(){  //Should only be called when there is 0 pressure
  float sum = 0;
  for(byte i = 0; i<50;i++){
  sum += analogRead(PressureSensorInPin) ; 
  delay(10);
  }  
  float AeroOffsetRecommendation = (sum/50)*5/1024; //Reads voltage at 0 pressure
  strncpy_P(Message,(PGM_P)F("0 pressure AeroOffset: "),MaxTextLength);
  strcat(Message,toText(AeroOffsetRecommendation));
  addToLog(Message);
}

void setPressureSensorOffset(float Value){
  MySettings.PressureSensorOffset = Value;
  addToLog(F("Pressure offset updated"));  
}

void setPressureSensorRatio(float Value){
  MySettings.PressureSensorRatio = Value;
  addToLog(F("Pressure/voltage ratio updated"));  
}

void setAeroInterval(int interval){  
  MySettings.AeroInterval = interval; 
  AeroSprayTimer = millis();
}

void setAeroDuration(int duration){  
  MySettings.AeroDuration = duration;
  AeroSprayTimer = millis(); 
  addToLog(F("Spray time updated"));  
}

const __FlashStringHelper * pumpOnOffToText(){
   if(!PumpOK) return F("DISABLED");
   else if(AeroPumpOn) return F("ON");
   else return F("OFF");
}

void setAeroPumpTimeout(int Timeout)
{
MySettings.AeroPumpTimeout = (uint32_t)Timeout;
addToLog(F("Aero pump timeout updated"));
}

void setAeroPrimingTime(int Timing)
{
MySettings.AeroPrimingTime = (uint32_t)Timing;
addToLog(F("Aero priming time updated"));
}
