void readDHTSensor(){
  IntHumidity = InternalDHTSensor.readHumidity();
  ExtHumidity = ExternalDHTSensor.readHumidity();
  if(MySettings.MetricSystemEnabled){
    IntTemp= InternalDHTSensor.readTemperature();
    ExtTemp = ExternalDHTSensor.readTemperature();
  }
  else {
    IntTemp = InternalDHTSensor.readTemperature() *1.8 + 32;
    ExtTemp = ExternalDHTSensor.readTemperature() *1.8 + 32;
  }
  checkDHTAlerts();
  checkFanAutomation();
}

void checkDHTAlerts(){
  if(!isnan(IntTemp) && !isnan(IntHumidity) && MySettings.TempAlertLow <= IntTemp  && IntTemp <= MySettings.TempAlertHigh && MySettings.HumidityAlertLow <= IntHumidity  && IntHumidity <= MySettings.HumidityAlertHigh){ //readings are normal
    VentilationAlertCount = 0;
    if(!VentOK){ ////alert was active before
      sendEmailAlert(F("Temp%20and%20Humidity%20OK")); 
      VentOK = true; //everything OK
    }
  }
  else {
    if(VentOK){
      VentilationAlertCount++;
      if(VentilationAlertCount>=MySettings.ReadCountBeforeAlert){
        VentOK = false;
        if(isnan(IntTemp) || IntTemp < MySettings.TempAlertLow ||  MySettings.TempAlertHigh < IntTemp){ //if temp out of limits, isnan-Returns true if float is Not A Numeber: DHT library returns NAN if the value cannot be read
        sendEmailAlert(F("Temperature%20out%20of%20range"));     
        addToLog(F("Temperature alert triggered"));
        }
        if(isnan(IntHumidity) || IntHumidity < MySettings.HumidityAlertLow ||  MySettings.HumidityAlertHigh < IntHumidity){ //if temp out of limits
        sendEmailAlert(F("Humidity%20out%20of%20range")); 
        addToLog(F("Humidity alert triggered"));
        }    
      }
     }
  }
}

void checkFanAutomation(){
    if(MySettings.AutomaticInternalFan && !MySettings.InternalFanHigh && IntTemp > MySettings.InternalFanSwitchTemp){ //if temp is above the limit turn the fan to High
    internalFanHigh();
  }
  if(MySettings.AutomaticInternalFan && MySettings.InternalFanHigh && IntTemp < (MySettings.InternalFanSwitchTemp-3)){ //backward switch only if temp is 3 degrees below limit: avoids constant switching
    internalFanLow();
  }
  if(MySettings.AutomaticExhaustFan && MySettings.ExhaustFanOn && IntHumidity < MySettings.ExhaustFanOffHumid){ //Humidity below Off limit: turn exhaust off
    exhaustFanOff();
  }
  if(MySettings.AutomaticExhaustFan && !MySettings.ExhaustFanHigh && MySettings.ExhaustFanHighHumid < IntHumidity){ //Humidity above High limit: set exhaust High
    exhaustFanHigh();
  }
  if(MySettings.AutomaticExhaustFan && ((!MySettings.ExhaustFanOn && MySettings.ExhaustFanLowHumid < IntHumidity) || (MySettings.ExhaustFanHigh && IntHumidity < MySettings.ExhaustFanLowHumid))){ //if exhaust was OFF and humidity gets above LOW limit, or if exhaust was High and humidity gets below LOW limit: Turn the fan LOW
    exhaustFanLow();
  }
}

void internalFanLow(){
  MySettings.InternalFanHigh = false;
  MySettings.InternalFanOn = true;
  addToLog(F("Internal fan Low")); 
  PlayOnSound=true;
}

void internalFanHigh(){
  MySettings.InternalFanHigh = true;
  MySettings.InternalFanOn = true;
  addToLog(F("Internal fan High"));
  PlayOnSound=true;
}

void internalFanOff(){
   MySettings.InternalFanOn = false;
   MySettings.InternalFanHigh = false;
   addToLog(F("Internal fan OFF"));
   PlayOffSound=true;
}

void exhaustFanLow(){
  MySettings.ExhaustFanHigh = false;
  MySettings.ExhaustFanOn = true;
  addToLog(F("Exhaust fan Low"));
  PlayOnSound=true;
}

void exhaustFanHigh(){
  MySettings.ExhaustFanHigh = true;
  MySettings.ExhaustFanOn = true;
  addToLog(F("Exhaust fan High"));
  PlayOnSound=true;
}

void exhaustFanOff(){
  MySettings.ExhaustFanOn = false;
  MySettings.ExhaustFanHigh = false;
  addToLog(F("Exhaust fan OFF"));
  PlayOffSound=true;
}

void autoFanOnOff(bool Status,bool Internal){
  if(Internal){
    MySettings.AutomaticInternalFan = Status;
    if(MySettings.AutomaticInternalFan){
      addToLog(F("Internal fan automatic"));
      PlayOnSound=true;
      }
    else{
      addToLog(F("Internal fan manual"));
      PlayOffSound=true; 
      }
  }
  else{
    MySettings.AutomaticExhaustFan = Status;
    if(MySettings.AutomaticExhaustFan){
      addToLog(F("Exhaust fan automatic"));
      PlayOnSound=true;
      }
    else{
      addToLog(F("Exhaust fan manual"));
      PlayOffSound=true; 
      }
  }
}

void setInternalSwitchTemp(int Temp){
  MySettings.InternalFanSwitchTemp = Temp;
}

void setExhaustOffHumidity(int Humidity){
  MySettings.ExhaustFanOffHumid = Humidity;
}

void setExhaustLowHumidity(int Humidity){
  MySettings.ExhaustFanLowHumid = Humidity;
}

void setExhaustHighHumidity(int Humidity){
  MySettings.ExhaustFanHighHumid = Humidity;
  addToLog(F("Exhaust fan limits updated"));
}

const __FlashStringHelper * fanSpeedToText(bool Internal){
  if(Internal){
   if(!MySettings.InternalFanOn) return F("OFF");
   else if (MySettings.InternalFanHigh) return F("HIGH");
   else return F("LOW");
  }
  else{
   if(!MySettings.ExhaustFanOn) return F("OFF");
   else if (MySettings.ExhaustFanHigh) return F("HIGH");
   else return F("LOW");
  }
}

const __FlashStringHelper * fanSpeedToNumber(bool internal){
   if(internal){
   if(!MySettings.InternalFanOn) return F("0");
   else if (MySettings.InternalFanHigh) return F("2");
   else return F("1");
  }
  else{
   if(!MySettings.ExhaustFanOn) return F("0");
   else if (MySettings.ExhaustFanHigh) return F("2");
   else return F("1");
  }
}
