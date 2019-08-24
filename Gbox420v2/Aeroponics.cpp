volatile unsigned int FlowPulseCount = 0; //volatile variables are stored in RAM: https://www.arduino.cc/reference/en/language/variables/variable-scope--qualifiers/volatile/

void checkAero(bool Interrupt){
 if(Interrupt) //Stuff to do only when called from an interrupt (When Solenoid or Pump is on)
  {
    if(millis() - FlowMeterTimer >= 1000) //Log after every second the pulse count
      {
        if(MySettings.DebugEnabled){
          memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable  
          strcat_P(WebMessage,(PGM_P)F("FlowMeter: "));
          strcat(WebMessage,toText((uint32_t)FlowPulseCount)); 
          strcat_P(WebMessage,(PGM_P)F(" pulse/sec"));
          logToSerials(WebMessage,true);
        }
        LastPulseCount = FlowPulseCount;
        FlowMeterTimer = millis();
        FlowPulseCount = 0;
      }
  }
 else{
  readAeroPressure(); //skip reading the pressure when called from an interrupt. (Within an interrupt millis() counter doesn`t increase, so delay() never ends)
 }
 if(MySettings.AeroPressureTankPresent)  {
    checkAeroSprayTimer_WithPressureTank();
    if(!Interrupt)checkAeroPumpAlerts_WithPressureTank();
  }
 else {
  checkAeroSprayTimer_WithoutPressureTank();
  if(!Interrupt)checkAeroPumpAlerts_WithoutPressureTank();
 }
 checkRelays();
}





void aeroSprayNow(bool DueToHighPressure){   
  if(MySettings.AeroSprayEnabled || DueToHighPressure){
    AeroBypassActive = false;
    AeroSprayTimer = millis();
    if(MySettings.AeroPressureTankPresent){
      AeroSpraySolenoidOn = true; 
    }
    else{
      PumpOK = true;
      AeroPumpOn = true;
      AeroBypassSolenoidOn = true;
      startFlowMeter();
      AeroPumpTimer = millis();
    }    
    checkRelays();
    PlayOnSound = true;
    if(DueToHighPressure) addToLog(F("Above pressure limit,spraying"));
    else addToLog(F("Aeroponics spraying"));
    }
}

void aeroSprayOff(){    
    if(MySettings.AeroPressureTankPresent) {
       AeroSpraySolenoidOn = false; 
    }
    else{
      AeroPumpOn = false; 
      AeroBypassSolenoidOn = false; 
      stopFlowMeter(); 
    }
    checkRelays();
    addToLog(F("Aeroponics spray OFF"));
}

void aeroPumpOn(bool CalledFromWebsite){
  AeroBypassActive   = CalledFromWebsite; //If called from the Web interface keep the pump on
  AeroPumpOn = true;
  AeroPumpTimer = millis();
  startFlowMeter(); 
  checkRelays();
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
  stopFlowMeter();           
  checkRelays();
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
  startFlowMeter();      
  checkRelays();
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

void setAeroInterval(int interval){  
  MySettings.AeroInterval = interval; 
  AeroSprayTimer = millis();
}

void setAeroDuration(int duration){  
  MySettings.AeroDuration = duration;
  AeroSprayTimer = millis(); 
  addToLog(F("Spray time updated"));  
}


void setAeroPressureLow(float PressureLow){
  MySettings.AeroPressureLow =  PressureLow;
}

void setAeroPressureHigh(float PressureHigh){
  MySettings.AeroPressureHigh = PressureHigh;
  addToLog(F("Pump settings updated"));
}

const __FlashStringHelper * pumpStateToText(){
   if(!PumpOK) return F("DISABLED");
   else if(AeroPumpOn) return F("ON");
   else return F("OFF");
}

//Quiet time section: Blocks running the pump in a pre-defined time range
uint32_t AeroLastRefill= 0;
bool checkQuietTime() {  
  if(MySettings.AeroQuietEnabled){
    time_t Now = now(); // Get the current time
    int CombinedFromTime = MySettings.AeroQuietFromHour * 100 + MySettings.AeroQuietFromMinute;
    int CombinedToTime = MySettings.AeroQuietToHour * 100 + MySettings.AeroQuietToMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if(MySettings.AeroRefillBeforeQuiet && PumpOK && CombinedFromTime == CombinedCurrentTime && millis() - AeroLastRefill > 120000 ){
      aeroPumpOn(false); 
      AeroLastRefill = millis();
    }
    if(CombinedFromTime <= CombinedToTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if(CombinedFromTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedToTime){
        return false;} //do not allow running the pump
      else  return true;  //allow running   
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedFromTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedToTime){
       return false; } //do not allow running the pump
      else  return true;  //allow running    
    }
  }
  else return true; //always allow if quiet mode is not enabled
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



void setAeroBlowOnOff(bool State){ //this change requires a different aeroponics setup
  MySettings.AeroBlowOff = State;  
  if(MySettings.AeroBlowOff){ 
    addToLog(F("Blow-off enabled"));
    PlayOnSound=true;
    }
  else {
    addToLog(F("Blow-off disabled"));
    PlayOffSound=true;
    }
}