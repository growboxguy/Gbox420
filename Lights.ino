void calibrateLights(){
  int LastBrightness = MySettings.LightBrightness;
  bool LastLightStatus = MySettings.isLightOn;
  MaxLightReading = 0;
  MinLightReading = 1023;
  isPotGettingHigh = false;
  MySettings.isLightOn=true;
  turnLightOnOff();
  for (int steps = 0; steps < PotStepping; steps++) 
    {stepOne();}  //Sets the digital potentiometer to low irregardless what the stored startup value is
  MySettings.LightBrightness = 0;
  isPotGettingHigh = true;  //set next step direction
  runToEnd();
  runToEnd();
  setBrightness(LastBrightness);
  MySettings.isLightOn=LastLightStatus;
  turnLightOnOff();  
  strncpy(LogMessage,"New min/max: ",LogLength);
  strcat(LogMessage,intToChar(MinLightReading));
  strcat(LogMessage,"/");
  strcat(LogMessage,intToChar(MaxLightReading));
  addToLog(LogMessage);
} 

void triggerCalibrateLights(){
  CalibrateLights = true;
  addToLog("Lights calibrating...");  
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

void setBrightness(int NewBrightness){
  strncpy(LogMessage,"Brightness: ",LogLength);  
  strcat(LogMessage,intToChar(NewBrightness));
  strcat(LogMessage,"%");
  addToLog(LogMessage);    
  while(MySettings.LightBrightness != NewBrightness){
    if(NewBrightness < MySettings.LightBrightness)  isPotGettingHigh = false;
    else isPotGettingHigh = true;
    stepOne();
  }
}

void turnLightON(){
   MySettings.isLightOn = true;
   addToLog("Light ON"); 
   PlayOnSound=true;
}

void turnLightOFF(){
  MySettings.isLightOn = false;
  addToLog("Light OFF"); 
  PlayOffSound=true; 
}

void turnLightOnOff(){
  if(CalibrateLights){CalibrateLights=false;calibrateLights();}
  if(MySettings.isLightOn){
    digitalWrite(PowerLEDOutPin, HIGH); //Turn on Power Led on PC case if light is on
    digitalWrite(Relay8OutPin, LOW); //True turns relay ON (LOW signal activates Relay)
  }
  else {
    digitalWrite(PowerLEDOutPin, LOW); 
    digitalWrite(Relay8OutPin, HIGH); //False turns relay OFF  
  }
}

void setTimerOnOff(bool TimerState){
  MySettings.isTimerEnabled = TimerState;
  if(MySettings.isTimerEnabled){ 
    checkLightStatus();
    addToLog("Timer enabled");
    PlayOnSound=true;
    }
  else {
    addToLog("Timer disabled");
    PlayOffSound=true;
    }
}

void setLightsOnHour(int OnHour){
  MySettings.LightOnHour = OnHour; 
}

void setLightsOnMinute(int OnMinute){
  MySettings.LightOnMinute = OnMinute;
  checkLightStatus();
  addToLog("Light ON time updated"); 
}

void setLightsOffHour(int OffHour){
  MySettings.LightOffHour = OffHour;
}

void setLightsOffMinute(int OffMinute){
  MySettings.LightOffMinute = OffMinute;
  checkLightStatus();
  addToLog("Light OFF time updated");
}

void checkLightStatus() {  //fills the CurrentTime global variable
  if(MySettings.isTimerEnabled){
    Time Now = Clock.time();  // Get the current time and date from the chip.
    int CombinedOnTime = MySettings.LightOnHour * 100 + MySettings.LightOnMinute;
    int CombinedOffTime = MySettings.LightOffHour * 100 + MySettings.LightOffMinute;
    int CombinedCurrentTime = Now.hr * 100 + Now.min;
    if(CombinedOnTime <= CombinedOffTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
    {
      if(CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime){
        if(MySettings.isLightOn != true) turnLightON();}
      else if(MySettings.isLightOn != false) turnLightOFF();     
    }
    else   //midnight turnover, Example: On 21:20, Off: 9:20
    {
      if(CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime){
       if(MySettings.isLightOn != true) turnLightON();}
      else if(MySettings.isLightOn != false) turnLightOFF();    
    }
  }
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
       if(isPotGettingHigh)Serial.print(StepCounter);
       else Serial.print(PotStepping - StepCounter);
       Serial.print(F("% - ")); Serial.println(LightReading);
      }  
     StepCounter++;
  }
  isPotGettingHigh= !isPotGettingHigh;  // flip the direction
}

void store(){  //store current potentiometer value in the X9C104 memory for the next power on. Write durability is only 100.000 writes, use with caution
  digitalWrite(PotINCOutPin,HIGH);
  digitalWrite(PotCSOutPin,HIGH);
  delay(50);
  digitalWrite(PotCSOutPin,LOW);
  digitalWrite(PotINCOutPin,LOW);
}
