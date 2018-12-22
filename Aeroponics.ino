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
  strncpy(LogMessage,"Pressure sensor offset: ",LogLength);
  strcat(LogMessage,floatToChar(MySettings.AeroOffset));
  addToLog(LogMessage);
}

void setAeroInterval(int interval){  
  MySettings.AeroInterval = interval; 
  AeroSprayTimer = millis(); 
}

void setAeroDuration(int duration){  
  MySettings.AeroDuration = duration;
  AeroSprayTimer = millis(); 
  addToLog("Spray time updated");  
}

void setAeroSprayOnOff(bool AeroState){
  MySettings.isAeroSprayEnabled=AeroState;
  if(MySettings.isAeroSprayEnabled){ 
    addToLog("Aeroponics spray enabled");
    PlayOnSound=true;} 
  else {
    addToLog("Aeroponics spray disabled");
    PlayOffSound=true;}
}

void aeroSprayNow(){   
  if(MySettings.isAeroSprayEnabled){
    AeroSprayTimer = millis();
    isAeroSprayOn = true;
    PlayOnSound = true;
    relayCheck();
    addToLog("Aeroponics spraying");
    }
}

void aeroSprayOff(){   
    isAeroSprayOn = false;    
    relayCheck();
    addToLog("Aeroponics spray OFF");
}

void aeroCheck(){
 checkAeroSprayTimer();
 checkAeroPump();
}

void checkAeroSprayTimer(){
 if(isAeroSprayOn)    { //if spray is on
    if(millis() - AeroSprayTimer >= MySettings.AeroDuration * 1000){  //if time to stop spraying (AeroDuration in Seconds)
      isAeroSprayOn = false;
      LogToSerials("Stopping spray",true);
      PlayOffSound = true;
      AeroSprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(millis() - AeroSprayTimer >= MySettings.AeroInterval * 60000){ //if time to start spraying (AeroInterval in Minutes)
      if(MySettings.isAeroSprayEnabled){
      isAeroSprayOn = true;
      LogToSerials("Starting spray",true);
      PlayOnSound = true;
      AeroSprayTimer = millis();
      }
    }
  }
}

void setAeroPressureLow(float PressureLow){
  MySettings.AeroPressureLow =  PressureLow;
}

void setAeroPressureHigh(float PressureHigh){
  MySettings.AeroPressureHigh = PressureHigh;
  addToLog("Pump settings updated");
}

void setAeroOffset(float Offset){
  MySettings.AeroOffset = Offset;
  addToLog("Pressure sensor offset updated");
}

void checkAeroPump(){
  if(isAeroPumpOn){
    readAeroPressure();   
    if(AeroPressure >= MySettings.AeroPressureHigh){ //If set high pressure is reached
      aeroPumpOff();
    }
    if (millis() - AeroPumpTimer >= AeroPumpTimeout){ //have not reached high pressue within limit (15min)
      aeroPumpDisable();
      SendEmailAlert(PumpAlertDeviceID);
      addToLog("Pump failed, alert sent");      
    }
  }
  else{
    if(AeroPressure <= MySettings.AeroPressureLow && !isAeroPumpDisabled && checkQuietTime()){ //Pressure below low limit: turn on pump, pump not disabled and quiet time not active
      aeroPumpOn();
    }
  }
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
  addToLog("Refilling pressure tank");
  aeroPumpOn();
}

void aeroPumpDisable(){
  isAeroPumpDisabled = true;
  aeroPumpOff();
  addToLog("Pump disabled");
}

//Quiet time section
bool checkQuietTime() {  
  if(MySettings.isAeroQuietEnabled){
    Time Now = Clock.time();  // Get the current time and date from the chip.
    int CombinedFromTime = MySettings.AeroQuietFromHour * 100 + MySettings.AeroQuietFromMinute;
    int CombinedToTime = MySettings.AeroQuietToHour * 100 + MySettings.AeroQuietToMinute;
    int CombinedCurrentTime = Now.hr * 100 + Now.min;
    if(CombinedFromTime <= CombinedToTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if(CombinedFromTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedToTime){
        return false;} //does not allow runnign the pump
      else  return true;  //allow running   
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedFromTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedToTime){
       return false; } //does not allow runnign the pump
      else  return true;  //allow running    
    }
  }
  else return true; //always allow if quiet mode is not enabled
}

void setQuietOnOff(bool State){
  MySettings.isAeroQuietEnabled = State;
  if(MySettings.isAeroQuietEnabled){ 
    addToLog("Quiet mode enabled");
    PlayOnSound=true;
    }
  else {
    addToLog("Quiet mode disabled");
    PlayOffSound=true;
    }
}

void setQuietFromHour(int Hour){
  MySettings.AeroQuietFromHour = Hour; 
}

void setQuietFromMinute(int Minute){
  MySettings.AeroQuietFromMinute = Minute;
  addToLog("Pump quiet From time updated"); 
}

void setQuietToHour(int Hour){
  MySettings.AeroQuietToHour = Hour;
}

void setQuietToMinute(int Minute){
  MySettings.AeroQuietToMinute = Minute;
  addToLog("Pump quiet To time updated");
}