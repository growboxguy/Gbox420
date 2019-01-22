void sendEmailAlert(const __FlashStringHelper *title){ //Title needs to be URL encoded: https://meyerweb.com/eric/tools/dencoder/
  if(MySettings.AlertEmails){
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable  
  strcat_P(WebMessage,(PGM_P)F("/pushingbox?devid=")); strcat(WebMessage,PushingBoxEmailRelayID); 
  strcat_P(WebMessage,(PGM_P)F("&Title=")); strcat_P(WebMessage,(PGM_P)title);
  strcat_P(WebMessage,(PGM_P)F("&Log=")); logToJSON(false,true);  
  RestAPI.get(WebMessage);
  if(MySettings.isDebugEnabled)LogToSerials(WebMessage,true);
  }
  else if(MySettings.isDebugEnabled){
    LogToSerials(F("Alert email suppressed: "),false);
    LogToSerials(title,true);
    }
}

void sendTestEmailAlert()
{
  sendEmailAlert(F("Test%20alert"));   //Encode text to be URL compatible: https://meyerweb.com/eric/tools/dencoder/ 
  addToLog(F("Test alert email sent")); 
}

void AlertEmailsOnOff(bool Status){  
  MySettings.AlertEmails = Status;
  if(Status){
   addToLog(F("Alert emails ON"));
   PlayOnSound=true;
  }
  else{
   addToLog(F("Alert emails OFF"));
   PlayOffSound=true; 
  }
}

void setHumidityAlertLow(int Humidity){
  MySettings.HumidityAlertLow = Humidity;
}
void setHumidityAlertHigh(int Humidity){
  MySettings.HumidityAlertHigh = Humidity;
  addToLog(F("Humidity alert range set"));
}

void setTempAlertLow(int Temp){
  MySettings.TempAlertLow = Temp;
}
void setTempAlertHigh(int Temp){
  MySettings.TempAlertHigh = Temp;
  addToLog(F("Temp alert range set"));
}

void setPressureAlertLow(float Pressure){
  MySettings.PressureAlertLow = Pressure;
}
void setPressureAlertHigh(float Pressure){
  MySettings.PressureAlertHigh = Pressure;
  addToLog(F("Pressure alert range set"));
}

void setPHAlertHigh(float PH){
  MySettings.PHAlertHigh = PH;
}
void setPHAlertLow(float PH){
  MySettings.PHAlertLow = PH;
  addToLog(F("PH alert range set"));
}
