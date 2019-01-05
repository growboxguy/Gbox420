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

  //Get percentage representation of reservoir level and send out email alerts
  if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && isWaterAboveFull) {
    reservoirPercent= 100;
    if(!ReservOK){
      sendEmailAlert(F("Reservoir%20recovered"),F("Reservoir%20level%20reading%20recovered."));
      ReservOK = true;
    } 
  }
  else if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && !isWaterAboveFull) {
    reservoirPercent= 75;
    if(!ReservOK){
      sendEmailAlert(F("Reservoir%20recovered"),F("Reservoir%20level%20reading%20recovered."));
      ReservOK = true;
    } 
  }
  else if(isWaterAboveCritical && isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {
    reservoirPercent= 50;
    if(!ReservOK){
      sendEmailAlert(F("Reservoir%20recovered"),F("Reservoir%20level%20reading%20recovered."));
      ReservOK = true;
    } 
  }
  else if(isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {
    reservoirPercent= 25;
    if(!ReservOK){
      sendEmailAlert(F("Reservoir%20recovered"),F("Reservoir%20level%20reading%20recovered."));
      ReservOK = true;
    }  
  }
  else if(!isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {  
    reservoirPercent= 0;
    if(ReservOK){
      sendEmailAlert(F("Reservoir%20empty"),F("Reservoir%20is%20empty%20and%20needs%20to%20be%20refilled."));
      addToLog(F("Reservoir empty"));
      ReservOK = false;
    }    
  }
  else {    //non-valid sensor combination was read like E[#--#]F
    reservoirPercent= 0;
    if(ReservOK){
      sendEmailAlert(F("Reservoir%20sensor%20failed"),F("Unexpected%20reservoir%20level%20reading%2C%20check%20the%20water%20sensors."));
      addToLog(F("Water sensor failed"));
      ReservOK = false;
    }    
  }
}

//***PH METER***
void readPH(){
  float  Reading=0;
  for(byte i=0;i<20;i++) { 
   Reading+=analogRead(PHMeterInPin);
   delay(50);
  }
  Reading = Reading /20; //Calculates average
  //Serial.print("Analog reading: "); Serial.println(Reading);
  PH = -0.031223*Reading + 23.376812;  //equation of the line

  if(!PhOK && ReservoirPHLowAlert <= PH && PH <= ReservoirPHHighAlert ){
      sendEmailAlert(F("PH%20recovered"),F("Reservoir%20PH%20optimal."));
      PhOK = true;
  } 
  if(PhOK && (PH < ReservoirPHLowAlert || ReservoirPHHighAlert < PH )){
      sendEmailAlert(F("PH%20out%20of%20range"),F("Reservoir%20PH%20not%20optimal."));
      addToLog(F("Reservoir PH not optimal"));
      PhOK = false;
  }  
}
