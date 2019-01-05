void readDHTSensor(){
  Humidity = TempSensor.readHumidity();
  BoxTempC= TempSensor.readTemperature();
  BoxTempF = BoxTempC *1.8 + 32;
}

void internalFanLow(){
  MySettings.isInternalFanHigh = false;
  MySettings.isInternalFanOn = true;
  addToLog(F("Internal fan Low")); 
  PlayOnSound=true;
}

void internalFanHigh(){
  MySettings.isInternalFanHigh = true;
  MySettings.isInternalFanOn = true;
  addToLog(F("Internal fan High"));
  PlayOnSound=true;
}

void internalFanOff(){
   MySettings.isInternalFanOn = false;
   MySettings.isInternalFanHigh = false;
   addToLog(F("Internal fan OFF"));
   PlayOffSound=true;
}

void exhaustFanLow(){
  MySettings.isExhaustFanHigh = false;
  MySettings.isExhaustFanOn = true;
  addToLog(F("Exhaust fan Low"));
  PlayOnSound=true;
}

void exhaustFanHigh(){
  MySettings.isExhaustFanHigh = true;
  MySettings.isExhaustFanOn = true;
  addToLog(F("Exhaust fan High"));
  PlayOnSound=true;
}

void exhaustFanOff(){
  MySettings.isExhaustFanOn = false;
  MySettings.isExhaustFanHigh = false;
  addToLog(F("Exhaust fan OFF"));
  PlayOffSound=true;
}

const __FlashStringHelper * fanSpeedToText(bool internal){
  if(internal){
   if(!MySettings.isInternalFanOn) return F("OFF");
   else if (MySettings.isInternalFanHigh) return F("HIGH");
   else return F("LOW");
  }
  else{
   if(!MySettings.isExhaustFanOn) return F("OFF");
   else if (MySettings.isExhaustFanHigh) return F("HIGH");
   else return F("LOW");
  }
}

const __FlashStringHelper * fanSpeedToNumber(bool internal){
   if(internal){
   if(!MySettings.isInternalFanOn) return F("0");
   else if (MySettings.isInternalFanHigh) return F("2");
   else return F("1");
  }
  else{
   if(!MySettings.isExhaustFanOn) return F("0");
   else if (MySettings.isExhaustFanHigh) return F("2");
   else return F("1");
  }
}
