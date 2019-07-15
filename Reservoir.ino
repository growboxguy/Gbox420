void checkReservoir(){
  readReservoirTemp(); //Check water temperature
  
  bool isWaterAboveCritical = !digitalRead(WaterCriticalInPin);  //Water sensor, true if level reached
  bool isWaterAboveLow = !digitalRead(WaterLowInPin);
  bool isWaterAboveMedium = !digitalRead(WaterMediumInPin);
  bool isWaterAboveFull = !digitalRead(WaterFullInPin);

  //Get text representation of reservoir level: E[----]F / E[#---]F / E[##--]F / E[###-] / E[####]
  memset(&ReservoirText, 0, sizeof(ReservoirText));  //clear variable   
  strcpy_P(ReservoirText,(PGM_P)F("E["));
  if(isWaterAboveCritical) strcat_P(ReservoirText,(PGM_P)F("#")); else strcat_P(ReservoirText,(PGM_P)F("-"));
  if(isWaterAboveLow) strcat_P(ReservoirText,(PGM_P)F("#")); else strcat_P(ReservoirText,(PGM_P)F("-"));
  if(isWaterAboveMedium) strcat_P(ReservoirText,(PGM_P)F("#")); else strcat_P(ReservoirText,(PGM_P)F("-"));
  if(isWaterAboveFull) strcat_P(ReservoirText,(PGM_P)F("#")); else strcat_P(ReservoirText,(PGM_P)F("-"));
  strcat_P(ReservoirText,(PGM_P)F("]F"));

  //Get number representation of reservoir level and send out email alerts: From 0-empty to 4-full
  if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && isWaterAboveFull) ReservoirLevel= 4;
  else if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && !isWaterAboveFull) ReservoirLevel= 3;
  else if(isWaterAboveCritical && isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) ReservoirLevel= 2;
  else if(isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) ReservoirLevel= 1;
  else if(!isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) ReservoirLevel= 0;
  else ReservoirLevel= -1; //non-valid sensor combination, like E[#--#]F
  checkReservoirAlert();
}

bool PrevoiusReservoirRead = true;
void checkReservoirAlert(){
  if(ReservoirLevel > 0){
    if(PrevoiusReservoirRead != true){ReservoirTriggerCount = 0;}
    else{ if(!ReservOK)ReservoirTriggerCount++; } 
    PrevoiusReservoirRead = true;
    
    if(!ReservOK && ReservoirTriggerCount>=MySettings.ReadCountBeforeAlert){
        sendEmailAlert(F("Reservoir%20OK"));
        ReservOK = true;
     }
  }
  else{
    if(PrevoiusReservoirRead != false){ReservoirTriggerCount = 0;}
    else{ if(ReservOK)ReservoirTriggerCount++; } 
    PrevoiusReservoirRead = false;

    if(ReservOK && ReservoirTriggerCount>=MySettings.ReadCountBeforeAlert){
      ReservOK = false;
      if(ReservoirLevel == 0){
        sendEmailAlert(F("Reservoir%20is%20empty"));
        addToLog(F("Reservoir is empty"));
        }
      else{
        sendEmailAlert(F("Reservoir%20sensor%20failed"));
        addToLog(F("Water sensor failed"));
        }
     }
   }  
}

//***Temperature sensor***
void readReservoirTemp(){
 ReservoirTempSensor.requestTemperatures(); 
 if(MySettings.MetricSystemEnabled) ReservoirTemp = ReservoirTempSensor.getTempCByIndex(0);
 else ReservoirTemp = ReservoirTempSensor.getTempFByIndex(0); 
}

//***PH meter***
RollingAvarage PHAverage;
void readReservoirPH(bool ShowRaw){
  float PHRaw = PHAverage.updateAverage(analogRead(PHMeterInPin));
  if(ShowRaw)
  {
  strncpy_P(Message,(PGM_P)F("PH analog read: "),MaxTextLength);
  strcat(Message,toText(PHRaw));
  addToLog(Message);
  }
  PH = MySettings.PHCalibrationSlope*PHRaw + MySettings.PHCalibrationIntercept;  //equation of the line  
  checkPHAlert();
}


void setPHCalibrationSlope(float Value){
  MySettings.PHCalibrationSlope = Value;
  addToLog(F("PH slope updated"));
}

void setPHCalibrationIntercept(float Value){
  MySettings.PHCalibrationIntercept = Value;
  addToLog(F("PH intercept updated"));
}

bool PreviousPhRead = true;
void checkPHAlert(){
  if(MySettings.PHAlertLow <= PH && PH <= MySettings.PHAlertHigh ){ //PH within range
    if(PreviousPhRead != true){PHTriggerCount = 0;}
    else {if(!PhOK) {PHTriggerCount++;}}
    PreviousPhRead = true;
      
    if(!PhOK && PHTriggerCount>=MySettings.ReadCountBeforeAlert){
        sendEmailAlert(F("PH%20OK"));
        PhOK = true;
      }
  } 
  else{  //PH out of range
    if(PreviousPhRead != false){PHTriggerCount = 0;}
    else {if(PhOK) {PHTriggerCount++;}}
    PreviousPhRead = false;
    
    if(PhOK && PHTriggerCount>=MySettings.ReadCountBeforeAlert){
        sendEmailAlert(F("Reservoir%20PH%20not%20optimal"));
        addToLog(F("Reservoir PH not optimal"));
        PhOK = false;
        }
     }    
 }
