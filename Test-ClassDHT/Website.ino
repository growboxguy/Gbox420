
//Website::Website(ELClient &ESPLink){
    //WebServer = ELClientWebServer(&ESPLink);
 //   if(Common::MySettings.DebugEnabled){logToSerials(F("Website object created"),true);}
 // }


void resetWebServer(void) {    // Callback made from esp-link to notify that it has just come out of a reset
  Common::logToSerials(F("WebServer (re)starting..."),false);
  while(!ESPLink.Sync())  {
    Common::logToSerials(F("."),false);
    delay(500);
    };
  //RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); //setup handling request from GrowBox.html
  GrowBoxHandler->loadCb.attach(&LoadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&RefreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&ButtonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&SetFieldCallback); //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&LoadCallback);  //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&RefreshCallback); //Called periodically to refresh website content 
  SettingsHandler->buttonCb.attach(&ButtonPressCallback); //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&SetFieldCallback); //Called when a field is changed on the website
//  addToLog(F("WebServer started"));
}

void LoadCallback(char * url) //called when website is loaded
{
  if (strcmp(url,"/GrowBox.html.json")==0){
  WebServer.setArgBoolean(F("AutoInternalFan"), Common::MySettings.AutomaticInternalFan);
  WebServer.setArgString(F("InternalFanSwitchTemp"), Common::toText(Common::MySettings.InternalFanSwitchTemp));
  WebServer.setArgBoolean(F("AutoExhaustFan"), Common::MySettings.AutomaticExhaustFan);
  WebServer.setArgString(F("ExhaustFanHighHumid"), Common::toText(Common::MySettings.ExhaustFanHighHumid));
  WebServer.setArgString(F("ExhaustFanLowHumid"), Common::toText(Common::MySettings.ExhaustFanLowHumid));
  WebServer.setArgString(F("ExhaustFanOffHumid"), Common::toText(Common::MySettings.ExhaustFanOffHumid));
    
  WebServer.setArgInt(F("Light1OnHour"), Common::MySettings.Light1OnHour); 
  WebServer.setArgInt(F("Light1OnMinute"), Common::MySettings.Light1OnMinute); 
  WebServer.setArgInt(F("Light1OffHour"), Common::MySettings.Light1OffHour); 
  WebServer.setArgInt(F("Light1OffMinute"),Common::MySettings.Light1OffMinute);
  WebServer.setArgInt(F("Light1Brightness"), Common::MySettings.Light1Brightness);
  WebServer.setArgInt(F("Light1BrightnessSlider"), Common::MySettings.Light1Brightness);

  WebServer.setArgInt(F("AeroPumpTimeout"), Common::MySettings.AeroPumpTimeout);
  WebServer.setArgInt(F("AeroPrimingTime"), Common::MySettings.AeroPrimingTime);
  WebServer.setArgInt(F("AeroInterval"), Common::MySettings.AeroInterval);
  WebServer.setArgInt(F("AeroDuration"), Common::MySettings.AeroDuration);  
  }
  
  if (strcmp(url,"/Settings.html.json")==0){  
  WebServer.setArgInt(F("SoundEnabled"), Common::MySettings.SoundEnabled);
  WebServer.setArgInt(F("DebugEnabled"), Common::MySettings.DebugEnabled);
  WebServer.setArgInt(F("MetricSystemEnabled"), Common::MySettings.MetricSystemEnabled);
  WebServer.setArgInt(F("DigitDisplayBrightness"), Common::MySettings.DigitDisplayBacklight);
  WebServer.setArgInt(F("DigitDisplayValue"), Common::MySettings.DigitDisplayValue);
  WebServer.setArgBoolean(F("MqttEnabled"), Common::MySettings.ReportToMqtt);
  WebServer.setArgBoolean(F("GoogleSheetsEnabled"), Common::MySettings.ReportToGoogleSheets);
  WebServer.setArgString(F("PushingBoxLogRelayID"), Common::MySettings.PushingBoxLogRelayID);

  WebServer.setArgBoolean(F("AlertEmails"), Common::MySettings.AlertEmails);
  WebServer.setArgString(F("PushingBoxEmailRelayID"), Common::MySettings.PushingBoxEmailRelayID);
  WebServer.setArgInt(F("ReadCountBeforeAlert"), Common::MySettings.ReadCountBeforeAlert);
  WebServer.setArgInt(F("TempAlertLow"), Common::MySettings.TempAlertLow);
  WebServer.setArgInt(F("TempAlertHigh"), Common::MySettings.TempAlertHigh);
  WebServer.setArgInt(F("HumidityAlertLow"), Common::MySettings.HumidityAlertLow);
  WebServer.setArgInt(F("HumidityAlertHigh"), Common::MySettings.HumidityAlertHigh);
  WebServer.setArgString(F("PressureAlertLow"), Common::toText(Common::MySettings.PressureAlertLow));
  WebServer.setArgString(F("PressureAlertHigh"), Common::toText(Common::MySettings.PressureAlertHigh));
  WebServer.setArgString(F("PHAlertLow"), Common::toText(Common::MySettings.PHAlertLow));
  WebServer.setArgString(F("PHAlertHigh"), Common::toText(Common::MySettings.PHAlertHigh));  

  WebServer.setArgString(F("PHCalibrationSlope"), Common::toPrecisionText(Common::MySettings.PHCalibrationSlope));
  WebServer.setArgString(F("PHCalibrationIntercept"), Common::toPrecisionText(Common::MySettings.PHCalibrationIntercept)); 
  WebServer.setArgString(F("PressureSensorOffset"), Common::toPrecisionText(Common::MySettings.PressureSensorOffset));
  WebServer.setArgString(F("PressureSensorRatio"), Common::toPrecisionText(Common::MySettings.PressureSensorRatio));  
  }
}

void RefreshCallback(char * url) //called when website is refreshed
{ 
  WebServer.setArgString(F("Time"), Common::getFormattedTime()); 
    
  if (strcmp(url,"/GrowBox.html.json")==0){      
  WebServer.setArgString(F("tdTemp"),InternalDHTSensor -> getTempText());
  WebServer.setArgString(F("tdHumidity"),InternalDHTSensor -> getHumidityText()); 
  WebServer.setArgString(F("tdTimerEnabled"),Common::enabledToText(Common::MySettings.TimerEnabled));
  WebServer.setArgString(F("tdLightOn"), Common::timeToText(Common::MySettings.LightOnHour,Common::MySettings.LightOnMinute)); 
  WebServer.setArgString(F("tdLightOff"), Common::timeToText(Common::MySettings.LightOffHour,Common::MySettings.LightOffMinute));  
  }
}

//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  if (strcmp_P(button,(PGM_P)F("btn_LightOn"))==0) { GrowLights -> turnLightON(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_LightOff"))==0) { GrowLights -> turnLightOFF(true); }
  
  saveSettings(false); 
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  if(strcmp_P(field,(PGM_P)F("Light1OnHour"))==0) {GrowLights -> setLightsOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OnMinute"))==0) {GrowLights -> setLightsOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffHour"))==0) {GrowLights -> setLightsOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffMinute"))==0) {GrowLights -> setLightsOffMinute(WebServer.getArgInt());} 
    
  saveSettings(false);
} 

void saveSettings(bool LogThis){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&Common::MySettings, (void*)0, sizeof(Common::MySettings)); //update_block only writes the bytes that changed
  if(LogThis) Common::addToLog(F("Settings saved to EEPROM"));
}

void loadSettings(){
  Settings EEPROMSettings; //temporary storage with "settings" type
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));  
  if(EEPROMSettings.Version != Common::MySettings.Version){
    Common::logToSerials(F("Change detected, updating EEPROM..."),false);
    saveSettings(false);  //overwrites stored settings with defaults from this sketch
  }
  else {
    Common::logToSerials(F("Same structure version detected, applying restored settings..."),false);
    Common::MySettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
  }
  Common::logToSerials(F("done"),true);
}
