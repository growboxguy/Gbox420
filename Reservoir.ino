void airPumpOn(){
  MySettings.isAirPumpOn = true;
  addToLog(F("Air pump ON"));
  PlayOnSound=true;
}

void airPumpOff(){
  MySettings.isAirPumpOn = false;
  addToLog(F("Air pump OFF"));
  PlayOffSound=true;
}

void checkReservoir(){
  bool isWaterAboveCritical = !digitalRead(WaterCriticalInPin);  //Water sensor, true if level reached
  bool isWaterAboveLow = !digitalRead(WaterLowInPin);
  bool isWaterAboveMedium = !digitalRead(WaterMediumInPin);
  bool isWaterAboveFull = !digitalRead(WaterFullInPin);

  //Get text representation of reservoir level: E[----]F / E[#---]F / E[##--]F / E[###-] / E[####]
  memset(&reservoirText, 0, sizeof(reservoirText));  //clear variable   
  strcpy_P(reservoirText,(PGM_P)F("E["));
  if(isWaterAboveCritical) strcat_P(reservoirText,(PGM_P)F("#")); else strcat_P(reservoirText,(PGM_P)F("-"));
  if(isWaterAboveLow) strcat_P(reservoirText,(PGM_P)F("#")); else strcat_P(reservoirText,(PGM_P)F("-"));
  if(isWaterAboveMedium) strcat_P(reservoirText,(PGM_P)F("#")); else strcat_P(reservoirText,(PGM_P)F("-"));
  if(isWaterAboveFull) strcat_P(reservoirText,(PGM_P)F("#")); else strcat_P(reservoirText,(PGM_P)F("-"));
  strcat_P(reservoirText,(PGM_P)F("]F"));

  //Get number representation of reservoir level and send out email alerts: From 0-empty to 4-full
  if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && isWaterAboveFull) {
    reservoirLevel= 4;    
  }
  else if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && !isWaterAboveFull) {
    reservoirLevel= 3;
  }
  else if(isWaterAboveCritical && isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {
    reservoirLevel= 2; 
  }
  else if(isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {
    reservoirLevel= 1;
  }
  else if(!isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {  
    reservoirLevel= 0;  
  }
  else {    //non-valid sensor combination was read like E[#--#]F
    reservoirLevel= -1;     
  }
  checkReservoirAlert();
}

void checkReservoirAlert(){
  if(reservoirLevel > 0){
      ReservoirAlertCount = 0;
      if(!ReservOK){
        sendEmailAlert(F("Reservoir%20OK"));
        ReservOK = true;
      }
  }
  if(ReservOK && reservoirLevel <= 0){
    ReservoirAlertCount++;
    if(ReservoirAlertCount>=MySettings.ReadCountBeforeAlert){
      ReservOK = false;
      if(reservoirLevel == 0){
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

//***PH METER***
void readPH(bool ShowRaw){
  float  Reading=0;
  for(byte i=0;i<40;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(25);
  }
  PHRaw = Reading /40; //Calculates average
  if(ShowRaw)
  {
  strncpy_P(LogMessage,(PGM_P)F("PH analog read: "),MaxTextLength);
  strcat(LogMessage,toText(PHRaw));
  addToLog(LogMessage);
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

void checkPHAlert(){
  if(MySettings.PHAlertLow <= PH && PH <= MySettings.PHAlertHigh ){
      PHAlertCount = 0;
      if(!PhOK){
        sendEmailAlert(F("PH%20OK"));
        PhOK = true;
      }
  } 
  else{
    if(PhOK){
      PHAlertCount++;
      if(PHAlertCount>=MySettings.ReadCountBeforeAlert){
        sendEmailAlert(F("Reservoir%20PH%20not%20optimal"));
        addToLog(F("Reservoir PH not optimal"));
        PhOK = false;
      }
    }    
  }    
}
