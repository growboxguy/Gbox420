void relayCheck(){
  if(isAeroSprayOn) digitalWrite(Relay1OutPin, LOW); else digitalWrite(Relay1OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay) 
  if(isAeroPumpOn) digitalWrite(Relay2OutPin, LOW); else digitalWrite(Relay2OutPin, HIGH);  
  if(MySettings.isPCPowerSupplyOn) digitalWrite(Relay3OutPin, LOW); else digitalWrite(Relay3OutPin, HIGH);
  if(MySettings.isInternalFanOn) digitalWrite(Relay4OutPin, LOW); else digitalWrite(Relay4OutPin, HIGH);
  if(MySettings.isInternalFanHigh) digitalWrite(Relay5OutPin, LOW); else digitalWrite(Relay5OutPin, HIGH);
  if(MySettings.isExhaustFanOn) digitalWrite(Relay6OutPin, LOW); else digitalWrite(Relay6OutPin, HIGH);
  if(MySettings.isExhaustFanHigh) digitalWrite(Relay7OutPin, LOW); else digitalWrite(Relay7OutPin, HIGH);
  if(MySettings.isLightOn) digitalWrite(Relay8OutPin, LOW); else digitalWrite(Relay8OutPin, HIGH);
}

void powerSupplyOn(){
  MySettings.isPCPowerSupplyOn = true;
  addToLog("Power supply ON");
  PlayOnSound=true;
}

void powerSupplyOff(){
  MySettings.isPCPowerSupplyOn = false;
  addToLog("Power supply OFF");
  PlayOffSound=true;
}

void internalFanLow(){
  MySettings.isInternalFanHigh = false;
  MySettings.isInternalFanOn = true;
  addToLog("Internal fan Low"); 
  PlayOnSound=true;
}

void internalFanHigh(){
  MySettings.isInternalFanHigh = true;
  MySettings.isInternalFanOn = true;
  addToLog("Internal fan High");
  PlayOnSound=true;
}

void internalFanOff(){
   MySettings.isInternalFanOn = false;
   MySettings.isInternalFanHigh = false;
   addToLog("Internal fan OFF");
   PlayOffSound=true;
}

void exhaustFanLow(){
  MySettings.isExhaustFanHigh = false;
  MySettings.isExhaustFanOn = true;
  addToLog("Exhaust fan Low");
  PlayOnSound=true;
}

void exhaustFanHigh(){
  MySettings.isExhaustFanHigh = true;
  MySettings.isExhaustFanOn = true;
  addToLog("Exhaust fan High");
  PlayOnSound=true;
}

void exhaustFanOff(){
  MySettings.isExhaustFanOn = false;
  MySettings.isExhaustFanHigh = false;
  addToLog("Exhaust fan OFF");
  PlayOffSound=true;
}

