void checkSwitches(){
  //Relay array
  if(AeroSprayOn) digitalWrite(Relay1OutPin, LOW); else digitalWrite(Relay1OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay) 
  if(AeroPumpOn) digitalWrite(Relay2OutPin, LOW); else digitalWrite(Relay2OutPin, HIGH); 
  if(MySettings.AirPumpOn) digitalWrite(Relay3OutPin, LOW); else digitalWrite(Relay3OutPin, HIGH);
  if(MySettings.InternalFanOn) digitalWrite(Relay4OutPin, LOW); else digitalWrite(Relay4OutPin, HIGH);
  if(MySettings.InternalFanHigh) digitalWrite(Relay5OutPin, LOW); else digitalWrite(Relay5OutPin, HIGH);
  if(MySettings.ExhaustFanOn) digitalWrite(Relay6OutPin, LOW); else digitalWrite(Relay6OutPin, HIGH);
  if(MySettings.ExhaustFanHigh) digitalWrite(Relay7OutPin, LOW); else digitalWrite(Relay7OutPin, HIGH);
  if(MySettings.LightOn) digitalWrite(Relay8OutPin, LOW); else digitalWrite(Relay8OutPin, HIGH);

  //Optocoupler
  if(ATXPowerSupplyOn) digitalWrite(ATXPowerONOutPin, HIGH); else digitalWrite(ATXPowerONOutPin, LOW); 
}

void readPowerSensor(){
  Voltage = PowerSensor.voltage(PowerSensorIP);
  Current = PowerSensor.current(PowerSensorIP);
  Power = PowerSensor.power(PowerSensorIP);
  Energy = PowerSensor.energy(PowerSensorIP) / 1000;  //total power consumption in kWh
  if(ACPowerOK && Voltage < 0) {
    ACPowerAlertCount++;
    if(ACPowerAlertCount>=MySettings.ReadCountBeforeAlert){
      sendEmailAlert(F("AC%20input%20lost")); 
      ACPowerOK = false;
      addToLog(F("AC Power lost"));
    }
  }
  if(Voltage > 0){
    ACPowerAlertCount = 0;
    if(!ACPowerOK){
      sendEmailAlert(F("AC%20input%20recovered"));
      ACPowerOK = true;
      addToLog(F("AC Power recovered")); 
    }    
  }
}

void TurnATXOn(){
  ATXPowerSupplyOn = true;
  addToLog(F("ATX power supply ON")); 
}

void TurnATXOff(){
  ATXPowerSupplyOn = false;
  addToLog(F("ATX power supply OFF"));
}

void readATXPowerGood(){   
  bool ATXStateOK = !digitalRead(ATXPowerGoodInPin); //inverting the reading to compensate the inverting optocoupler. True:DC power OK, False:DC power not OK
   
  if(DCPowerOK && !ATXStateOK) {  //ATX Power Good pin is not at expected 5V, signaling a problem with the power supply or the voltage divider
    DCPowerAlertCount++;
    if(DCPowerAlertCount>=MySettings.ReadCountBeforeAlert){
      sendEmailAlert(F("DC%20input%20lost")); 
      DCPowerOK = false;
      addToLog(F("DC Power lost"));
    }
  }
  if(ATXStateOK){  //ATX Power Good pin is at ~5V, signaling DC power output is OK
    DCPowerAlertCount = 0;
    if(!DCPowerOK){ //if it was in failed state before
      sendEmailAlert(F("DC%20input%20recovered"));
      DCPowerOK = true;
      addToLog(F("DC Power recovered")); 
    }    
  }
}
