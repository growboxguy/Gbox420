void readAeroPressure(){
  float Reading = analogRead(PressureSensorInPin) ;
  AeroPressure = (Reading - MySettings.PressureSensorOffset) * 12 / (921 - MySettings.PressureSensorOffset);  //1.2mPa = 12 bar
}

void calibratePressureSensor(){
  MySettings.PressureSensorOffset = 1023;
  for(byte i = 0; i<100;i++){
  int Reading = analogRead(PressureSensorInPin) ;  
  if(Reading<MySettings.PressureSensorOffset) MySettings.PressureSensorOffset = Reading;
  delay(10);
  }
  strncpy(LogMessage,"Pressure sensor offset: ",LogLength);
  strcat(LogMessage,intToChar(MySettings.PressureSensorOffset));
  addToLog(LogMessage);
}

void setAeroDuration(int duration){  
  MySettings.AeroDuration = WebServer.getArgInt() * 1000;
  AeroSprayTimer = millis();  
}

void setAeroInterval(int interval){  
  MySettings.AeroInterval = interval * 60000; 
  AeroSprayTimer = millis(); 
}

void aeroSprayNow(){  
  AeroSprayTimer = millis(); 
  isAeroSprayOn = true;
  PlayOnSound = true;
  relayCheck();
  addToLog("Aeroponics Spraying");
}

void checkAeroStatus(){
 checkAeroSprayTimer();
 checkAeroPump();
}

void checkAeroSprayTimer(){
 if(isAeroSprayOn)    { //if spray is on
    if(millis() - AeroSprayTimer >= MySettings.AeroDuration){  //if time to stop spraying
      isAeroSprayOn = false;
      Serial.println("Stopping spray");
      PlayOffSound = true;
      AeroSprayTimer = millis();
    }
  }
  else{ //if spray is off
    if(millis() - AeroSprayTimer >= MySettings.AeroInterval){ //if time to start spraying
      isAeroSprayOn = true;
      Serial.println("Starting spray");
      PlayOnSound = true;
      AeroSprayTimer = millis(); 
    }
  }
}

void checkAeroPump(){
  if(isAeroPumpOn){
    readAeroPressure();   
    if(AeroPressure >= MySettings.AeroPressureHigh){
      isAeroPumpOn = false;
      addToLog("Turning off pump");
      PlayOffSound = true;
    }
    else if (millis() - AeroPumpTimer >= AeroPumpTimeout){
      isAeroPumpOn = false;      
      isAeroPumpBroken = true;
      addToLog("Pump failed, alerting");
      PlayOffSound = true;
      memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
      strcat(WebMessage,"/pushingbox?devid="); strcat(WebMessage,PumpAlertDeviceID);  
      Serial.println(WebMessage);   
      RestAPI.get(WebMessage);
    }
  }
  else{
    if(AeroPressure <= MySettings.AeroPressureLow && !isAeroPumpBroken){
      AeroPumpTimer = millis();
      isAeroPumpOn = true;
      addToLog("Turning on pump");
      PlayOnSound = true;
    }
  }
}
