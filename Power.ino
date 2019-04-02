void checkRelays(){
  //Relay array
  if(AeroSolenoidOn) digitalWrite(Relay1OutPin, LOW); else digitalWrite(Relay1OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay) 
  if(AeroPumpOn) digitalWrite(Relay2OutPin, LOW); else digitalWrite(Relay2OutPin, HIGH); 
  if(MySettings.AirPumpOn) digitalWrite(Relay3OutPin, LOW); else digitalWrite(Relay3OutPin, HIGH);
  if(MySettings.InternalFanOn) digitalWrite(Relay4OutPin, LOW); else digitalWrite(Relay4OutPin, HIGH);
  if(MySettings.InternalFanHigh) digitalWrite(Relay5OutPin, LOW); else digitalWrite(Relay5OutPin, HIGH);
  if(MySettings.ExhaustFanOn) digitalWrite(Relay6OutPin, LOW); else digitalWrite(Relay6OutPin, HIGH);
  if(MySettings.ExhaustFanHigh) digitalWrite(Relay7OutPin, LOW); else digitalWrite(Relay7OutPin, HIGH);
  if(MySettings.LightStatus) digitalWrite(Relay8OutPin, LOW); else digitalWrite(Relay8OutPin, HIGH);

  //Optocoupler
  if(MySettings.ATXPowerSupplyOn) digitalWrite(ATXPowerONOutPin, HIGH); else digitalWrite(ATXPowerONOutPin, LOW); 
}

bool PreviousACRead = true;
void readPowerSensor(){
  Voltage = PowerSensor.voltage(PowerSensorIP);
  Current = PowerSensor.current(PowerSensorIP);
  Power = PowerSensor.power(PowerSensorIP);
  Energy = PowerSensor.energy(PowerSensorIP) / 1000;  //total power consumption in kWh

  if(Voltage > 0){
    if(PreviousACRead != true){ACPowerTriggerCount = 0;}
    else {if(!ACPowerOK) {ACPowerTriggerCount++;}}
    PreviousACRead = true;
    
    if(!ACPowerOK && ACPowerTriggerCount>=MySettings.ReadCountBeforeAlert){
      sendEmailAlert(F("AC%20input%20recovered"));      
      addToLog(F("AC Power recovered"));
      ACPowerOK = true;
    }    
  }
  else{
    if(PreviousACRead != false){ACPowerTriggerCount = 0;}
    else {if(ACPowerOK) {ACPowerTriggerCount++;}}
    PreviousACRead = false;

    if(ACPowerOK && ACPowerTriggerCount>=MySettings.ReadCountBeforeAlert){
      sendEmailAlert(F("AC%20input%20lost"));      
      addToLog(F("AC Power lost"));
      ACPowerOK = false;
    }
  }  
}

void TurnATXOn(){
  MySettings.ATXPowerSupplyOn = true;
  addToLog(F("ATX power supply ON")); 
}

void TurnATXOff(){
  MySettings.ATXPowerSupplyOn = false;
  addToLog(F("ATX power supply OFF"));
}

bool PreviousDCRead = true;
void readATXPowerGood(){   
  bool ATXStateOK = !digitalRead(ATXPowerGoodInPin); //inverting the reading to compensate the inverting optocoupler. True:DC power OK, False:DC power not OK

  if(ATXStateOK){  //ATX Power Good pin is at ~5V, signaling DC power output is OK
    if(PreviousDCRead != true){DCPowerTriggerCount = 0;}
    else {if(!DCPowerOK) {DCPowerTriggerCount++;}}
    PreviousDCRead = true;

    if(!DCPowerOK && DCPowerTriggerCount>=MySettings.ReadCountBeforeAlert){ //if DC power was in failed state before
      sendEmailAlert(F("DC%20input%20recovered"));
      addToLog(F("DC Power recovered"));
      DCPowerOK = true; 
    }    
  }
  else{      
    if(PreviousDCRead != false){DCPowerTriggerCount = 0;}
    else {if(DCPowerOK) {DCPowerTriggerCount++;}}
    PreviousDCRead = false;
    
    if(DCPowerOK && DCPowerTriggerCount>=MySettings.ReadCountBeforeAlert) {  
      sendEmailAlert(F("DC%20input%20lost")); 
      DCPowerOK = false;
      addToLog(F("DC Power lost"));
      }
    }  
}
