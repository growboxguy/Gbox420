void sendEmailAlert(const __FlashStringHelper *title){ //Title needs to be URL encoded: https://meyerweb.com/eric/tools/dencoder/
  if(MySettings.AlertEmails){
  memset(&Message[0], 0, sizeof(Message));  //clear variable  
  strcat_P(Message,(PGM_P)F("/pushingbox?devid=")); strcat(Message,MySettings.PushingBoxEmailRelayID); 
  strcat_P(Message,(PGM_P)F("&Title=")); strcat_P(Message,(PGM_P)title);
  strcat_P(Message,(PGM_P)F("&Log=")); logToJSON(false,true);  
  RestAPI.get(Message);
  if(MySettings.DebugEnabled)logToSerials(Message,true);
  }
  else if(MySettings.DebugEnabled){
    logToSerials(F("Alert email suppressed: "),false);
    logToSerials(title,true);
    }
}

void setPushingBoxEmailRelayID(char * ID)
{
  strncpy(MySettings.PushingBoxEmailRelayID,ID,MaxTextLength);
  addToLog(F("Email relay ID updated")); 
  logToSerials(F("New PushingBox DeviceID: "),false);logToSerials(MySettings.PushingBoxEmailRelayID,true); 
}

void sendTestEmailAlert()
{
  sendEmailAlert(F("Test%20alert"));   //Encode text to be URL compatible: https://meyerweb.com/eric/tools/dencoder/ 
  addToLog(F("Test alert email sent")); 
}

void setReadCountBeforeAlert(byte Count)
{
  MySettings.ReadCountBeforeAlert = Count;
  addToLog(F("Alert trigger updated"));  
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
