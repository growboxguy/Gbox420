void checkLightStatus(){
  if(!digitalRead(PowerButtonInPin)){ //If the power button is kept pressed
    if(MySettings.isLightOn) turnLightOFF(true);
    else turnLightON(true);  
    }
  if(CalibrateLights){calibrateLights();}
  if(MySettings.isLightOn){
    digitalWrite(PowerLEDOutPin, HIGH); //Turn on Power Led on PC case if light is on
    digitalWrite(Relay8OutPin, LOW); //True turns relay ON (LOW signal activates Relay)
  }
  else {
    digitalWrite(PowerLEDOutPin, LOW); 
    digitalWrite(Relay8OutPin, HIGH); //False turns relay OFF  
  }
}

void checkLightTimer() {
  if(MySettings.isTimerEnabled){
    time_t Now = now();  // Get the current time
    int CombinedOnTime = MySettings.LightOnHour * 100 + MySettings.LightOnMinute; //convert time to number, Example: 8:10=810, 20:10=2010
    int CombinedOffTime = MySettings.LightOffHour * 100 + MySettings.LightOffMinute;
    int CombinedCurrentTime = hour(Now) * 100 + minute(Now);
    if(CombinedOnTime <= CombinedOffTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if(CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime){
        if(MySettings.isLightOn != true) turnLightON(false);}
      else if(MySettings.isLightOn != false) turnLightOFF(false);     
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime){
       if(MySettings.isLightOn != true) turnLightON(false);}
      else if(MySettings.isLightOn != false) turnLightOFF(false);    
    }
  }
}

void setBrightness(int NewBrightness, bool AddToLog){
  MySettings.LightBrightness = NewBrightness;
  if(AddToLog){
    strncpy_P(LogMessage,(PGM_P)F("Brightness: "),LogLength);  
    strcat(LogMessage,toText(MySettings.LightBrightness));
    strcat_P(LogMessage,(PGM_P)F("%"));
    addToLog(LogMessage);
  }    
  analogWrite(DimmingOutPin, map(MySettings.LightBrightness,0,100,MaxDimming,0) ); //mapping 0% brightness to MaxDimming(92%) duty cycle, and 100% brighness to 0% dimming duty cycle
}

void checkLightSensor(){
  isBright = !digitalRead(LightSensorInPin);     // read the input pin: 0- light detected , 1 - no light detected. Had to invert signal from the sensor to make more sense.
  LightReading = 1023 - analogRead(LightSensorAnalogInPin);
  
  if((MySettings.isLightOn && isBright) || (!MySettings.isLightOn && !isBright)){ //All OK: lights on&bright OR lights off&dark
    LightsAlertCount=0;
    if(!LightOK) {
      sendEmailAlert(F("Lights%20OK")); 
      LightOK = true;
    }
  }
  else{
    if(LightOK){
      LightsAlertCount++;
      if(LightsAlertCount>=ReadCountBeforeAlert){
       LightOK = false;
       if(MySettings.isLightOn && !isBright){ //if light should be ON but no light is detected
        sendEmailAlert(F("No%20light%20detected"));        
        addToLog(F("Lights ON, no light detected"));
       }
       if(!MySettings.isLightOn && isBright){ //if light should be OFf but light is detected
        sendEmailAlert(F("Dark%20period%20interrupted")); 
        addToLog(F("Dark period interrupted"));
       }
      }
    }
  }
}

void triggerCalibrateLights(){ //website signals to calibrate lights when checkLightStatus runs next
  CalibrateLights = true; 
}

void calibrateLights(){
  CalibrateLights=false;  
  bool LastLightStatus = MySettings.isLightOn;
  byte LastLightBrightness = MySettings.LightBrightness;
  MySettings.isLightOn=true;
  checkLightStatus();  //apply turning the lights on
  setBrightness(0,false);
  delay(2000); //wait for light output change
  MinLightReading = 1023 - analogRead(LightSensorAnalogInPin);
  setBrightness(100,false);
  delay(2000); //wait for light output change
  MaxLightReading = 1023 - analogRead(LightSensorAnalogInPin);
  
  if(MySettings.isDebugEnabled){
         LogToSerials(F("0% - "),false); LogToSerials(MinLightReading,false);
         LogToSerials(F(", 100% - "),false); LogToSerials(MaxLightReading,true);
  }
  setBrightness(LastLightBrightness,false); //restore original brightness
  MySettings.isLightOn=LastLightStatus; //restore original state
  checkLightStatus();
  addToLog(F("Lights calibrated"));
}

void turnLightON(bool AddToLog){
   MySettings.isLightOn = true;
   if(AddToLog)addToLog(F("Light ON")); 
   PlayOnSound=true;
}

void turnLightOFF(bool AddToLog){
  MySettings.isLightOn = false;
  if(AddToLog)addToLog(F("Light OFF")); 
  PlayOffSound=true; 
}

void setTimerOnOff(bool TimerState){
  MySettings.isTimerEnabled = TimerState;
  if(MySettings.isTimerEnabled){ 
    checkLightTimer();
    addToLog(F("Timer enabled"));
    PlayOnSound=true;
    }
  else {
    addToLog(F("Timer disabled"));
    PlayOffSound=true;
    }
}

void setLightsOnHour(int OnHour){
  MySettings.LightOnHour = OnHour; 
}

void setLightsOnMinute(int OnMinute){
  MySettings.LightOnMinute = OnMinute;
  checkLightTimer();
  addToLog(F("Light ON time updated")); 
}

void setLightsOffHour(int OffHour){
  MySettings.LightOffHour = OffHour;
}

void setLightsOffMinute(int OffMinute){
  MySettings.LightOffMinute = OffMinute;
  checkLightTimer();
  addToLog(F("Light OFF time updated"));
}

const __FlashStringHelper * isBrightToText(){
   if(isBright) return F("YES");
   else return F("NO");
}
