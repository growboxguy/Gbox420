void checkRelays(){
  if(isAeroSprayOn) digitalWrite(Relay1OutPin, LOW); else digitalWrite(Relay1OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay) 
  if(isAeroPumpOn) digitalWrite(Relay2OutPin, LOW); else digitalWrite(Relay2OutPin, HIGH);  
  if(MySettings.isAirPumpOn) digitalWrite(Relay3OutPin, LOW); else digitalWrite(Relay3OutPin, HIGH);
  if(MySettings.isInternalFanOn) digitalWrite(Relay4OutPin, LOW); else digitalWrite(Relay4OutPin, HIGH);
  if(MySettings.isInternalFanHigh) digitalWrite(Relay5OutPin, LOW); else digitalWrite(Relay5OutPin, HIGH);
  if(MySettings.isExhaustFanOn) digitalWrite(Relay6OutPin, LOW); else digitalWrite(Relay6OutPin, HIGH);
  if(MySettings.isExhaustFanHigh) digitalWrite(Relay7OutPin, LOW); else digitalWrite(Relay7OutPin, HIGH);
  if(MySettings.isLightOn) digitalWrite(Relay8OutPin, LOW); else digitalWrite(Relay8OutPin, HIGH);
}

void readPowerSensor(){
  Voltage = PowerSensor.voltage(PowerSensorIP);
  Current = PowerSensor.current(PowerSensorIP);
  Power = PowerSensor.power(PowerSensorIP);
  Energy = PowerSensor.energy(PowerSensorIP) / 1000;  //total power consumption in kWh
  if(ACPowerOK && Voltage < 0) {
    ACPowerAlertCount++;
    if(ACPowerAlertCount>=ReadCountBeforeAlert){
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

void readATXPowerGood(){   
  float Reading=0;
  for (byte i=0;i<10;i++) {
        Reading += analogRead(ATXPowerGoodInPin);
        delay(10);
  }   
  Reading = (Reading / 10 * 5) / 1024.0 * DividingFactor;  //using a voltage divider circuit to protect from voltage spikes up to ~50V
  if(MySettings.isDebugEnabled){
    LogToSerials(F("ATXPowerGood voltage: "),false);
    LogToSerials(Reading,true);
    }
  
  if(DCPowerOK && ( Reading < 4 || Reading > 6)) {  //ATX Power Good pin is not at expected 5V, signaling a problem with the power supply or the voltage divider
    DCPowerAlertCount++;
    if(DCPowerAlertCount>=ReadCountBeforeAlert){
      sendEmailAlert(F("DC%20input%20lost")); 
      DCPowerOK = false;
      addToLog(F("DC Power lost"));
    }
  }
  if(Reading > 4 && Reading < 6){  //ATX Power Good pin is at ~5V, signaling DC power output is OK
    DCPowerAlertCount = 0;
    if(!DCPowerOK){
      sendEmailAlert(F("DC%20input%20recovered"));
      DCPowerOK = true;
      addToLog(F("DC Power recovered")); 
    }    
  }
}
