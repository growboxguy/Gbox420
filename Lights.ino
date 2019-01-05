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
  if(AddToLog){
    strncpy_P(LogMessage,(PGM_P)F("Brightness: "),LogLength);  
    strcat(LogMessage,toText(NewBrightness));
    strcat_P(LogMessage,(PGM_P)F("%"));
    addToLog(LogMessage);
  }    
  while(MySettings.LightBrightness != NewBrightness){
    if(NewBrightness < MySettings.LightBrightness)  isPotGettingHigh = false;
    else isPotGettingHigh = true;
    stepOne();
  }
}

void checkLightSensor(){
  isBright = !digitalRead(LightSensorInPin);     // read the input pin: 0- light detected , 1 - no light detected. Had to invert signal from the sensor to make more sense.
  LightReading = 1023 - analogRead(LightSensorAnalogInPin);
  LightReadingPercent = map(LightReading,MinLightReading,MaxLightReading,0,100);
  if((MySettings.isLightOn && isBright) || (!MySettings.isLightOn && !isBright)){
    if(!LightOK) sendEmailAlert(F("Lights OK"),F("Lights%20status%20recovered."));  //https://meyerweb.com/eric/tools/dencoder/  
    LightOK = true; //everything OK: if lights are ON light is detected, when OFF no light is detected
  }
  if(MySettings.isLightOn && !isBright && LightOK){ //if light should be ON but no light is detected and the light was OK before
    sendEmailAlert(F("No%20light%20detected"),F("No%20light%20was%20detected%20when%20lights%20should%20be%20ON."));  //https://meyerweb.com/eric/tools/dencoder/  
    LightOK = false;
    addToLog(F("Lights ON, no light detected"));
  }
  if(!MySettings.isLightOn && isBright && LightOK){ //if light should be ON but no light is detected and the light was OK before
    sendEmailAlert(F("Dark%20period%20interrupted"),F("Light%20was%20detected%20when%20lights%20should%20be%20OFF."));  //https://meyerweb.com/eric/tools/dencoder/  
    LightOK = false;
    addToLog(F("Dark period interrupted"));
  }
}

void storeBrightness(){  //store current potentiometer value in the X9C104 memory for the next power on. Write durability is only 100.000 writes, use with caution
  digitalWrite(PotINCOutPin,HIGH);
  digitalWrite(PotCSOutPin,HIGH);
  delay(50);
  digitalWrite(PotCSOutPin,LOW);
  digitalWrite(PotINCOutPin,LOW);
}

void triggerCalibrateLights(){ //website signals to calibrate lights when checkLightStatus runs next
  CalibrateLights = true; 
}

void calibrateLights(){
  CalibrateLights=false;
  int LastBrightness = MySettings.LightBrightness;
  bool LastLightStatus = MySettings.isLightOn;
  MaxLightReading = 0;
  MinLightReading = 1023;
  isPotGettingHigh = false;
  MySettings.isLightOn=true;
  checkLightStatus();
  for (int steps = 0; steps < PotStepping; steps++) 
    {stepOne();}  //Sets the digital potentiometer to low irregardless what the stored startup value is
  MySettings.LightBrightness = 0;
  isPotGettingHigh = true;  //set next step direction
  runToEnd(); //runs to highest value
  runToEnd(); //runs to lowest value (same function)
  setBrightness(LastBrightness,false);
  MySettings.isLightOn=LastLightStatus;
  checkLightStatus();  
  addToLog(F("Lights calibrated"));
}

void stepOne(){  //Adjust Potentiometer by one
  digitalWrite(PotUDOutPin,isPotGettingHigh);  //true=HIGH=increase brightness
  digitalWrite(PotINCOutPin,HIGH);   //signal the change to the chip
  if(MySettings.isSoundEnabled){ tone(BuzzerOutPin,MySettings.LightBrightness * 10); }
  delay(10);  //delay super oversized, https://www.intersil.com/content/dam/intersil/documents/x9c1/x9c102-103-104-503.pdf  
  digitalWrite(PotINCOutPin,LOW);
  delay(10);
  noTone(BuzzerOutPin);
  if(isPotGettingHigh && (MySettings.LightBrightness < PotStepping))  MySettings.LightBrightness++;
  else if(MySettings.LightBrightness > 0)  MySettings.LightBrightness--;
}

void runToEnd(){  //Goes to Minimum or Maximum dimming, measure light intensity on the way
  int StepCounter = 0;
  while(StepCounter < PotStepping){ 
    stepOne();      //step in one direction (Initially upwards) 
    LightReading = 1023 - analogRead(LightSensorAnalogInPin);
    if(LightReading > MaxLightReading) MaxLightReading = LightReading;
    if(LightReading < MinLightReading) MinLightReading = LightReading;
    if(StepCounter % 10 == 0)  //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
      {  
       if(isPotGettingHigh)LogToSerials(StepCounter,false);
       else LogToSerials(PotStepping - StepCounter,false);
       LogToSerials(F("% - "),false); LogToSerials(LightReading,true);
      }  
     StepCounter++;
  }
  isPotGettingHigh= !isPotGettingHigh;  // flip the direction for he next run
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
