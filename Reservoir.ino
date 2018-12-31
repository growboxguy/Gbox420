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
    reservoirOK = true;
  }
  else if(isWaterAboveCritical && isWaterAboveLow && isWaterAboveMedium && !isWaterAboveFull) {
    reservoirPercent= 75;
    reservoirOK = true;
  }
  else if(isWaterAboveCritical && isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {
    reservoirPercent= 50;
    reservoirOK = true;
  }
  else if(isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {
    reservoirPercent= 25;
    reservoirOK = true;
  }
  else if(!isWaterAboveCritical && !isWaterAboveLow && !isWaterAboveMedium && !isWaterAboveFull) {  
    reservoirPercent= 0;
    if(reservoirOK){
      sendEmailAlert(F("Reservoir%20empty"),F("Reservoir%20is%20empty%20and%20needs%20to%20be%20refilled."));
      addToLog(F("Reservoir empty,alert sent"));
    }
    reservoirOK = false;
  }
  else {    //non-valid sensor combination was read like E[#--#]F
    reservoirPercent= 0;
    if(reservoirOK){
      sendEmailAlert(F("Reservoir%20sensor%20failed"),F("Unexpected%20reservoir%20level%20reading%2C%20check%20the%20water%20sensors."));
      addToLog(F("Water sensor failed,alert sent"));
    }
    reservoirOK = false;
  }
}
