void checkAero(){
 checkAeroSprayTimer();
 checkAeroPump();
}

void checkAeroSprayTimer(){
 if(isAeroSprayOn)    { //if spray is on
    if(millis() - AeroSprayTimer >= MySettings.AeroDuration * 1000){  //if time to stop spraying (AeroDuration in Seconds)
      isAeroSprayOn = false;
      LogToSerials(F("Stopping spray"),true);
      PlayOffSound = true;
      AeroSprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(millis() - AeroSprayTimer >= MySettings.AeroInterval * 60000){ //if time to start spraying (AeroInterval in Minutes)
      if(MySettings.isAeroSprayEnabled){
      isAeroSprayOn = true;
      LogToSerials(F("Starting spray"),true);
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
    if (millis() - AeroPumpTimer >= AeroPumpTimeout){ //have not reached high pressue within limit (15min)
      aeroPumpDisable();
      sendEmailAlert(F("Aeroponics%20pump%20failed"),F("Aeroponics%20pump%20timed%20out%20during%20refill%20and%20got%20disabled.")); 
      addToLog(F("Pump failed"));    
    }
  }
  else{
    if( !isAeroPumpDisabled && checkQuietTime()){ //Pressure below low limit: turn on pump if its not disabled and quiet time not active
      if(AeroPressure <= MySettings.AeroPressureLow)aeroPumpOn();
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
  AeroPressure = (2.7*(Voltage-MySettings.AeroOffset)); // unit: bar / 100kPa
  AeroPressurePSI = AeroPressure * 14.5038; 
}

void calibratePressureSensor(){  //Should only be called when there is 0 pressure
  float sum = 0;
  for(byte i = 0; i<50;i++){
  sum += analogRead(PressureSensorInPin) ; 
  delay(10);
  }  
  MySettings.AeroOffset = (sum/50)*5/1024; //Reads voltage at 0 pressure
  strncpy_P(LogMessage,(PGM_P)F("Pressure sensor offset: "),LogLength);
  strcat(LogMessage,toText(MySettings.AeroOffset));
  addToLog(LogMessage);
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

void setAeroSprayOnOff(bool AeroState){
  MySettings.isAeroSprayEnabled=AeroState;
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

void setAeroOffset(float Offset){
  MySettings.AeroOffset = Offset;
  addToLog(F("Pressure sensor offset updated"));
}

void aeroPumpOn(){
  isAeroPumpDisabled = false;
  isAeroPumpOn = true;
  AeroPumpTimer = millis();      
  PlayOnSound = true;
}

void aeroPumpOff(){
  isAeroPumpOn = false;
  PlayOffSound = true;
}

void aeroPumpRefill(){  
  addToLog(F("Refilling pressure tank"));
  aeroPumpOn();
}

void aeroPumpStop(){ 
  addToLog(F("Pump stopped")); 
  aeroPumpOff();
}

void aeroPumpDisable(){
  isAeroPumpDisabled = true;
  aeroPumpOff();
  addToLog(F("Pump disabled"));
}

const __FlashStringHelper * pumpStateToText()
{
if(isAeroPumpOn) return F("ON"); else return F("OFF");
}

const __FlashStringHelper * pumpStatusToText()
{ 
  if(isAeroPumpDisabled) return F("DISABLED"); else return F("OK");
}

//Quiet time section: Blocks running the pump in a pre-defined time range
unsigned long AeroLastRefill= 0;
bool checkQuietTime() {  
  if(MySettings.isAeroQuietEnabled){
    time_t Now = now(); // Get the current time
    int CombinedFromTime = MySettings.AeroQuietFromHour * 100 + MySettings.AeroQuietFromMinute;
    int CombinedToTime = MySettings.AeroQuietToHour * 100 + MySettings.AeroQuietToMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if(MySettings.AeroRefillBeforeQuiet && !isAeroPumpDisabled && CombinedFromTime == CombinedCurrentTime && millis() - AeroLastRefill > 120000 ){
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

void setQuietOnOff(bool State){
  MySettings.isAeroQuietEnabled = State;
  if(MySettings.isAeroQuietEnabled){ 
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
