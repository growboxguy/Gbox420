void checkAero(){
 checkAeroSprayTimer();
 checkAeroPump();
}

void checkAeroSprayTimer(){
 if(isAeroSprayOn)    { //if spray is on
    if(millis() - AeroSprayTimer >= MySettings.AeroDuration * 1000){  //if time to stop spraying (AeroDuration in Seconds)
      isAeroSprayOn = false;
      checkRelays();
      logToSerials(F("Stopping spray"),true);
      PlayOffSound = true;
      AeroSprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(millis() - AeroSprayTimer >= MySettings.AeroInterval * 60000){ //if time to start spraying (AeroInterval in Minutes)
      if(MySettings.isAeroSprayEnabled){
        isAeroSprayOn = true;
        checkRelays();
        logToSerials(F("Starting spray"),true);
        PlayOnSound = true;
        AeroSprayTimer = millis();
      }
    }
  }
}

void checkAeroPump(){
  if(isAeroPumpOn){
    readAeroPressure();   
    if(AeroPressure >= MySettings.AeroPressureHigh){ //If set high pressure is reached
      aeroPumpOff();
    }
    if (millis() - AeroPumpTimer >= MySettings.AeroPumpTimeout){ //have not reached high pressue within timeout limit
      aeroPumpDisable();
      sendEmailAlert(F("Aeroponics%20pump%20failed")); 
      addToLog(F("Pump failed"));    
    }
  }
  else{
    if( PumpOK && checkQuietTime()){ //if pump is not disabled and quiet time not active
      if(AeroPressure <= MySettings.AeroPressureLow) //Pressure reached low limit: turn on pump 
        aeroPumpOn();
    }    
  }
  CheckAeroPumpAlerts();
}

void CheckAeroPumpAlerts()
{
  if(MySettings.PressureAlertLow <= AeroPressure && AeroPressure <= MySettings.PressureAlertHigh){ //If pressure is between OK range
     PressureAlertCount = 0; 
       if(!PressureOK){ // pressure was not OK before
       PressureOK = true;
       sendEmailAlert(F("Aeroponics%20pressure%20OK"));
       } 
  }
  else{
    if(AeroPressure > MySettings.PressureAlertHigh){ //Pressure over limit - emergency spraying
          aeroPumpOff(); //force pump off
          aeroSprayNow(); //try to release pressure  
    }
    if(PressureOK){
      PressureAlertCount++;
      if(PressureAlertCount>=MySettings.ReadCountBeforeAlert){
        PressureOK = false;
        if(AeroPressure > MySettings.PressureAlertHigh){ //If high pressure alert level is reached   
          sendEmailAlert(F("Aeroponics%20pressure%20too%20high"));
          addToLog(F("High pressure warning"));
        }
        if(AeroPressure < MySettings.PressureAlertLow){ //If low pressure alert level is reached
          //if(PumpOK) aeroPumpOn(); //Uncomment this to turn pump on even under quiet time
          sendEmailAlert(F("Aeroponics%20pressure%20too%20low"));
          addToLog(F("Low pressure warning"));
        } 
      }
    }
  }   
}

void readAeroPressure(){
  float  Reading=0;
  for(byte i=0;i<50;i++) { 
   Reading+=analogRead(PressureSensorInPin);
   delay(20);
  }
  Reading = Reading /50;
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
  strncpy_P(LogMessage,(PGM_P)F("0 pressure AeroOffset: "),MaxTextLength);
  strcat(LogMessage,toText(AeroOffsetRecommendation));
  addToLog(LogMessage);
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

void setAeroSprayOnOff(bool State){
  MySettings.isAeroSprayEnabled=State;
  if(MySettings.isAeroSprayEnabled){ 
    addToLog(F("Aeroponics spray enabled"));
    PlayOnSound=true;} 
  else {
    addToLog(F("Aeroponics spray disabled"));
    PlayOffSound=true;}
}

void aeroSprayNow(){   
  if(MySettings.isAeroSprayEnabled){
    AeroSprayTimer = millis();
    isAeroSprayOn = true;
    PlayOnSound = true;
    checkRelays();
    addToLog(F("Aeroponics spraying"));
    }
}

void aeroSprayOff(){   
    isAeroSprayOn = false;    
    checkRelays();
    addToLog(F("Aeroponics spray OFF"));
}

void setAeroPressureLow(float PressureLow){
  MySettings.AeroPressureLow =  PressureLow;
}

void setAeroPressureHigh(float PressureHigh){
  MySettings.AeroPressureHigh = PressureHigh;
  addToLog(F("Pump settings updated"));
}

void aeroPumpOn(){
  PumpOK = true;
  isAeroPumpOn = true;
  checkRelays();
  AeroPumpTimer = millis();      
  PlayOnSound = true;
}

void aeroPumpOff(){
  isAeroPumpOn = false;
  checkRelays();
  PlayOffSound = true;
}

void aeroPumpRefill(){  
  addToLog(F("Refilling pressure tank"));
  aeroPumpOn();
}

void aeroPumpStop(){ 
  addToLog(F("Pump stopped"));
  PumpOK = true; 
  aeroPumpOff();
}

void aeroPumpDisable(){
  aeroPumpOff();
  PumpOK = false;
  addToLog(F("Pump disabled"));
}

const __FlashStringHelper * pumpStateToText(){
   if(!PumpOK) return F("DISABLED");
   else if(isAeroPumpOn) return F("ON");
   else return F("OFF");
}

//Quiet time section: Blocks running the pump in a pre-defined time range
unsigned long AeroLastRefill= 0;
bool checkQuietTime() {  
  if(MySettings.AeroQuietEnabled){
    time_t Now = now(); // Get the current time
    int CombinedFromTime = MySettings.AeroQuietFromHour * 100 + MySettings.AeroQuietFromMinute;
    int CombinedToTime = MySettings.AeroQuietToHour * 100 + MySettings.AeroQuietToMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if(MySettings.AeroRefillBeforeQuiet && PumpOK && CombinedFromTime == CombinedCurrentTime && millis() - AeroLastRefill > 120000 ){
      aeroPumpOn(); 
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
MySettings.AeroPumpTimeout = (unsigned long)Timeout * 1000;
addToLog(F("Aero pump timeout updated"));
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

void setQuietFromHour(int Hour){
  MySettings.AeroQuietFromHour = Hour; 
}

void setQuietFromMinute(int Minute){
  MySettings.AeroQuietFromMinute = Minute;
  addToLog(F("Pump quiet From time updated")); 
}

void setQuietToHour(int Hour){
  MySettings.AeroQuietToHour = Hour;
}

void setQuietToMinute(int Minute){
  MySettings.AeroQuietToMinute = Minute;
  addToLog(F("Pump quiet To time updated"));
}
