/*
Website::Website(HardwareSerial *SerialPort){
    //Initialize web connections
  MyESPLink -> resetCb = &resetWebServer;  //Callback subscription: When wifi reconnects, restart the WebServer
  resetWebServer();  //reset the WebServer 
  if(MySettings.DebugEnabled){logToSerials(F("Website object created"),true);}
}
*/

void refresh(){
  MyESPLink.Process();  //Process any command from ESP-Link
}

void resetWebServer(void) {    // Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("WebServer (re)starting..."),false);
  while(!MyESPLink.Sync())  {
    logToSerials(F("."),false);
    delay(500); 
    };
  //RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); //setup handling request from Settings.html
  GrowBoxHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  SettingsHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  logToSerials(F("WebServer started"),true);
}

void loadCallback(char * url) //called when website is loaded
{
  if (strcmp(url,"/GrowBox.html.json")==0){
  WebServer.setArgBoolean(F("AutoInternalFan"), MySettings.AutomaticInternalFan);
  WebServer.setArgString(F("InternalFanSwitchTemp"), toText(MySettings.InternalFanSwitchTemp));
  WebServer.setArgBoolean(F("AutoExhaustFan"), MySettings.AutomaticExhaustFan);
  WebServer.setArgString(F("ExhaustFanHighHumid"), toText(MySettings.ExhaustFanHighHumid));
  WebServer.setArgString(F("ExhaustFanLowHumid"), toText(MySettings.ExhaustFanLowHumid));
  WebServer.setArgString(F("ExhaustFanOffHumid"), toText(MySettings.ExhaustFanOffHumid));
    
  WebServer.setArgInt(F("Light1OnHour"), MySettings.Light1OnHour); 
  WebServer.setArgInt(F("Light1OnMinute"), MySettings.Light1OnMinute); 
  WebServer.setArgInt(F("Light1OffHour"), MySettings.Light1OffHour); 
  WebServer.setArgInt(F("Light1OffMinute"),MySettings.Light1OffMinute);
  WebServer.setArgInt(F("Light1Brightness"), MySettings.Light1Brightness);
  WebServer.setArgInt(F("Light1BrightnessSlider"), MySettings.Light1Brightness);

  WebServer.setArgInt(F("AeroPumpTimeout"), MySettings.AeroPumpTimeout);
  WebServer.setArgInt(F("AeroPrimingTime"), MySettings.AeroPrimingTime);
  WebServer.setArgInt(F("AeroInterval"), MySettings.AeroInterval);
  WebServer.setArgInt(F("AeroDuration"), MySettings.AeroDuration);  
  }
  
  if (strcmp(url,"/Settings.html.json")==0){  
  WebServer.setArgInt(F("SoundEnabled"), MySettings.SoundEnabled);
  WebServer.setArgInt(F("DebugEnabled"), MySettings.DebugEnabled);
  WebServer.setArgInt(F("MetricSystemEnabled"), MySettings.MetricSystemEnabled);
  WebServer.setArgInt(F("DigitDisplayBrightness"), MySettings.DigitDisplayBacklight);
  WebServer.setArgInt(F("DigitDisplayValue"), MySettings.DigitDisplayValue);
  WebServer.setArgBoolean(F("MqttEnabled"), MySettings.ReportToMqtt);
  WebServer.setArgBoolean(F("GoogleSheetsEnabled"), MySettings.ReportToGoogleSheets);
  WebServer.setArgString(F("PushingBoxLogRelayID"), MySettings.PushingBoxLogRelayID);

  WebServer.setArgBoolean(F("AlertEmails"), MySettings.AlertEmails);
  WebServer.setArgString(F("PushingBoxEmailRelayID"), MySettings.PushingBoxEmailRelayID);
  WebServer.setArgInt(F("ReadCountBeforeAlert"), MySettings.ReadCountBeforeAlert);
  WebServer.setArgInt(F("TempAlertLow"), MySettings.TempAlertLow);
  WebServer.setArgInt(F("TempAlertHigh"), MySettings.TempAlertHigh);
  WebServer.setArgInt(F("HumidityAlertLow"), MySettings.HumidityAlertLow);
  WebServer.setArgInt(F("HumidityAlertHigh"), MySettings.HumidityAlertHigh);
  WebServer.setArgString(F("PressureAlertLow"), toText(MySettings.PressureAlertLow));
  WebServer.setArgString(F("PressureAlertHigh"), toText(MySettings.PressureAlertHigh));
  WebServer.setArgString(F("PHAlertLow"), toText(MySettings.PHAlertLow));
  WebServer.setArgString(F("PHAlertHigh"), toText(MySettings.PHAlertHigh));  

  WebServer.setArgString(F("PHCalibrationSlope"), toPrecisionText(MySettings.PHCalibrationSlope));
  WebServer.setArgString(F("PHCalibrationIntercept"), toPrecisionText(MySettings.PHCalibrationIntercept)); 
  WebServer.setArgString(F("PressureSensorOffset"), toPrecisionText(MySettings.PressureSensorOffset));
  WebServer.setArgString(F("PressureSensorRatio"), toPrecisionText(MySettings.PressureSensorRatio));  
  }
}

void refreshCallback(char * url) //called when website is refreshed
{ 
  WebServer.setArgString(F("Time"), getFormattedTime());
  WebServer.setArgJson(F("list_SerialLog"), Common::eventLogToJSON(false)); //Last events that happened in JSON format
    
  if (strcmp(url,"/GrowBox.html.json")==0){  
  WebServer.setArgString(F("tdLight1Status"),GBox -> Light1 -> getStatusText());
  //WebServer.setArgString(F("tdLight1Reading"),toText(MySettings.LightBrightness, LightReading,"%-"));
  //WebServer.setArgString(F("tdBright"),yesNoToText());       
  WebServer.setArgString(F("tdTemp"),toText(GBox -> InternalDHTSensor -> getTemp(),GBox -> ExternalDHTSensor -> getTemp()," / "));
  WebServer.setArgString(F("tdHumidity"),toText(GBox -> InternalDHTSensor -> getHumidity(),GBox -> ExternalDHTSensor -> getHumidity()," / ")); 
  WebServer.setArgString(F("tdLight1TimerEnabled"),GBox -> Light1 -> getTimerOnOffText());
  WebServer.setArgString(F("tdLight1On"), GBox -> Light1 -> getOnTimeText());
  WebServer.setArgString(F("tdLight1Off"), GBox -> Light1 -> getOffTimeText());  
  }
}

//Called when any button on the website is pressed
void buttonPressCallback(char *button)
{
  if (strcmp_P(button,(PGM_P)F("btn_Light1On"))==0) {GBox -> Light1 -> setLightOnOff(true,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1Off"))==0) {GBox -> Light1 -> setLightOnOff(false,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1Calibrate"))==0) {GBox -> Light1 -> triggerCalibrateLights();}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1TimerEnable"))==0) {GBox -> Light1 -> setTimerOnOff(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1TimerDisable"))==0) {GBox -> Light1 -> setTimerOnOff(false);}
  
  saveSettings(false); 
}

//Called when any field on the website is updated
void setFieldCallback(char * field){
  if(strcmp_P(field,(PGM_P)F("Light1Brightness"))==0) {GBox -> Light1 -> setBrightness(WebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("Light1OnHour"))==0) {GBox -> Light1 -> setOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OnMinute"))==0) {GBox -> Light1 -> setOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffHour"))==0) {GBox -> Light1 -> setOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffMinute"))==0) {GBox -> Light1 -> setOffMinute(WebServer.getArgInt());} 


  //else if(strcmp_P(field,(PGM_P)F("SoundEnabled"))==0) {setSoundOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("DebugEnabled"))==0) {setDebugOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MetricSystemEnabled"))==0) {Common::setMetricSystemEnabled(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}
 
    
  saveSettings(false);
} 
