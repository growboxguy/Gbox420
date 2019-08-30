void calibrateLights(){
  int LastBrightness = Brightness;
  bool LastLightStatus = isLightOn;
  MaxLightReading = 0;
  MinLightReading = 1023;
  PotGettingHigh = false;
  isLightOn=true;
  TurnLightOnOff();
  for (int steps = 0; steps < PotSteppingCount; steps++) 
    {StepOne();}  //Sets the digital potentiometer to low irregardless what the stored startup value is
  Brightness = 0;
  PotGettingHigh = true;  //set next step direction
  RunToEnd();
  RunToEnd();
  SetBrightness(LastBrightness);
  isLightOn=LastLightStatus;
  TurnLightOnOff();
  
  strncpy(LogMessage,"New min/max: ",LogLength);
  strcat(LogMessage,intToChar(MinLightReading));
  strcat(LogMessage,"/");
  strcat(LogMessage,intToChar(MaxLightReading));
  addToLog(LogMessage);
} 

void StepOne(){  //Adjust Potentiometer by one
digitalWrite(PotUDOutPin,PotGettingHigh);  //true=HIGH=increase brightness
digitalWrite(PotINCOutPin,HIGH);   //signal the change to the chip
tone(BuzzerOutPin,Brightness * 10);
delay(10);  //delay super oversized, https://www.intersil.com/content/dam/intersil/documents/x9c1/x9c102-103-104-503.pdf  
digitalWrite(PotINCOutPin,LOW);
delay(10);
noTone(BuzzerOutPin);
if(PotGettingHigh && Brightness < PotSteppingCount)  Brightness++;
else if(Brightness > 0)  Brightness--;
}

void SetBrightness(int NewBrightness){
strncpy(LogMessage,"Brightness: ",LogLength);  
strcat(LogMessage,intToChar(NewBrightness));
strcat(LogMessage,"%");
addToLog(LogMessage);
  
while(Brightness != NewBrightness){
  if(NewBrightness < Brightness)  PotGettingHigh = false;
  else PotGettingHigh = true;
  StepOne();
  }
}

void TurnLightON(){
   isLightOn = true;
   addToLog("Light ON"); 
   OnSound();
}

void TurnLightOFF(){
  isLightOn = false;
  addToLog("Light OFF"); 
  OffSound();
}

void TurnLightOnOff(){
if(isLightOn)digitalWrite(PowerLEDOutPin, HIGH); else digitalWrite(PowerLEDOutPin, LOW); //Turn on Power Led on PC case if light is on
if(isLightOn) digitalWrite(Relay8OutPin, LOW); else digitalWrite(Relay8OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay)
}

void CheckLightStatus() {  //fills the CurrentTime global variable
  if(isTimerEnabled){
  Time Now = rtc.time();  // Get the current time and date from the chip.
  int CombinedOnTime = LightOnHour * 100 + LightOnMinute;
  int CombinedOffTime = LightOffHour * 100 + LightOffMinute;
  int CombinedCurrentTime = Now.hr * 100 + Now.min;
  if(CombinedOnTime <= CombinedOffTime)  //no midnight turnover, Example: On 8:10, Off: 20:10
  {
    if(CombinedOnTime <= CombinedCurrentTime && CombinedCurrentTime < CombinedOffTime){
      if(isLightOn != true) TurnLightON();}
    else if(isLightOn != false) TurnLightOFF();     
  }
  else   //midnight turnover, Example: On 21:20, Off: 9:20
  {
    if(CombinedOnTime <= CombinedCurrentTime || CombinedCurrentTime < CombinedOffTime){
     if(isLightOn != true) TurnLightON();}
    else if(isLightOn != false) TurnLightOFF();    
  }
  }
}

void RunToEnd(){  //Goes to Minimum or Maximum dimming, measure light intensity on the way
int StepCounter = 0;
while(StepCounter < PotSteppingCount){ 
  StepOne();      //step in one direction (Initially upwards) 
  LightReading = 1023 - analogRead(LightSensorAnalogInPin);
  if(LightReading > MaxLightReading) MaxLightReading = LightReading;
  if(LightReading < MinLightReading) MinLightReading = LightReading;
  if(StepCounter % 10 == 0)  //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
    {  
     if(PotGettingHigh)Serial.print(StepCounter);
     else Serial.print(PotSteppingCount - StepCounter);
     Serial.print(F("% - ")); Serial.println(LightReading);
    }  
   StepCounter++;
}
PotGettingHigh= !PotGettingHigh;  // flip the direction
}


void Store(){  //store current potentiometer value in the X9C104 memory for the next power on. Write durability is only 100.000 writes, use with caution
digitalWrite(PotINCOutPin,HIGH);
digitalWrite(PotCSOutPin,HIGH);
delay(50);
digitalWrite(PotCSOutPin,LOW);
digitalWrite(PotINCOutPin,LOW);
}
