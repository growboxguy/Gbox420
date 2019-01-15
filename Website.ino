// Callback made from esp-link to notify that it has just come out of a reset
void resetWebServer(void) {  
  LogToSerials(F("WebServer (re)starting..."),false);
  while(!ESPLink.Sync())  {
    LogToSerials(F("."),false);
    delay(500);
    };
  RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
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
  addToLog(F("WebServer started"));
}

void LoadCallback(char * url) //called when website is loaded
{
  if (strcmp(url,"/GrowBox.html.json")==0){
  WebServer.setArgBoolean(F("AutoInternalFan"), MySettings.automaticInternalFan);
  WebServer.setArgString(F("InternalFanSwitchTemp"), toText(MySettings.internalFanSwitchTemp));
  WebServer.setArgBoolean(F("AutoExhaustFan"), MySettings.automaticExhaustFan);
  WebServer.setArgString(F("ExhaustFanHighHumid"), toText(MySettings.exhaustFanHighHumid));
  WebServer.setArgString(F("ExhaustFanLowHumid"), toText(MySettings.exhaustFanLowHumid));
  WebServer.setArgString(F("ExhaustFanOffHumid"), toText(MySettings.exhaustFanOffHumid));
    
  WebServer.setArgBoolean(F("TimerEnabled"), MySettings.isTimerEnabled);
  WebServer.setArgInt(F("LightsOnHour"), MySettings.LightOnHour); 
  WebServer.setArgInt(F("LightsOnMinute"), MySettings.LightOnMinute); 
  WebServer.setArgInt(F("LightsOffHour"), MySettings.LightOffHour); 
  WebServer.setArgInt(F("LightsOffMinute"),MySettings.LightOffMinute);
  WebServer.setArgInt(F("Brightness"), MySettings.LightBrightness);
  WebServer.setArgInt(F("sliderBrightness"), MySettings.LightBrightness);

  WebServer.setArgInt(F("AeroInterval"), MySettings.AeroInterval);
  WebServer.setArgInt(F("AeroDuration"), MySettings.AeroDuration);
  WebServer.setArgString(F("AeroPressureLow"), toText(MySettings.AeroPressureLow));
  WebServer.setArgString(F("AeroPressureHigh"), toText(MySettings.AeroPressureHigh));
  WebServer.setArgBoolean(F("AeroSprayEnabled"), MySettings.isAeroSprayEnabled);
  WebServer.setArgBoolean(F("AeroQuietEnabled"), MySettings.isAeroQuietEnabled);
  WebServer.setArgBoolean(F("AeroRefillBeforeQuiet"), MySettings.AeroRefillBeforeQuiet);
  WebServer.setArgInt(F("AeroQuietFromHour"), MySettings.AeroQuietFromHour); 
  WebServer.setArgInt(F("AeroQuietFromMinute"), MySettings.AeroQuietFromMinute); 
  WebServer.setArgInt(F("AeroQuietToHour"), MySettings.AeroQuietToHour); 
  WebServer.setArgInt(F("AeroQuietToMinute"),MySettings.AeroQuietToMinute);
  }
  
  if (strcmp(url,"/Settings.html.json")==0){
  WebServer.setArgInt(F("DigitDisplayBrightness"), MySettings.DigitDisplayBacklight);
  WebServer.setArgInt(F("DigitDisplayValue"), MySettings.DigitDisplayValue);
  WebServer.setArgInt(F("SoundEnabled"), MySettings.isSoundEnabled);

  WebServer.setArgBoolean(F("GoogleSheetsEnabled"), MySettings.ReportToGoogleSheets);
  WebServer.setArgBoolean(F("MqttEnabled"), MySettings.ReportToMqtt);

  WebServer.setArgBoolean(F("AlertEmails"), MySettings.AlertEmails);
  WebServer.setArgInt(F("TempAlertLow"), MySettings.TempAlertLow);
  WebServer.setArgInt(F("TempAlertHigh"), MySettings.TempAlertHigh);
  WebServer.setArgInt(F("HumidityAlertLow"), MySettings.HumidityAlertLow);
  WebServer.setArgInt(F("HumidityAlertHigh"), MySettings.HumidityAlertHigh);
  WebServer.setArgString(F("PressureAlertLow"), toText(MySettings.PressureAlertLow));
  WebServer.setArgString(F("PressureAlertHigh"), toText(MySettings.PressureAlertHigh));
  WebServer.setArgString(F("PHAlertLow"), toText(MySettings.PHAlertLow));
  WebServer.setArgString(F("PHAlertHigh"), toText(MySettings.PHAlertHigh));  
  }
}

void RefreshCallback(char * url) //called when website is refreshed
{ 
  WebServer.setArgString(F("Time"), getFormattedTime()); 
  WebServer.setArgString(F("tdLightOK"), statusToText(LightOK)); 
  WebServer.setArgString(F("tdPressureOK"),statusToText(PressureOK));
  WebServer.setArgString(F("tdPumpOK"),statusToText(PumpOK));
  WebServer.setArgString(F("tdPowerOK"),statusToText(PowerOK));
  WebServer.setArgString(F("tdVentOK"),statusToText(VentOK));
  WebServer.setArgString(F("tdReservOK"),statusToText(ReservOK));
  WebServer.setArgString(F("tdPhOK"),statusToText(PhOK));
  WebServer.setArgJson(F("list_SerialLog"), eventLogToJSON(false)); //Last events that happened in JSON format
  
  if (strcmp(url,"/GrowBox.html.json")==0){      
  WebServer.setArgString(F("tdBoxTemp"),toText(BoxTempC,BoxTempF,"/"));
  WebServer.setArgString(F("tdHumidity"),toText(Humidity));
  WebServer.setArgString(F("tdInternalFanSpeed"),fanSpeedToText(true));
  WebServer.setArgString(F("tdExhaustFanSpeed"),fanSpeedToText(false));
 
  WebServer.setArgString(F("tdPower"),toText(Power));  
  WebServer.setArgString(F("tdEnergy"),toText(Energy));
  WebServer.setArgString(F("tdVoltage"),toText(Voltage));
  WebServer.setArgString(F("tdCurrent"),toText(Current)); 

  WebServer.setArgString(F("tdisLightOn"),stateToText(MySettings.isLightOn));
  WebServer.setArgString(F("tdLightReading"),toText(MySettings.LightBrightness, LightReading,"%-")); 
  WebServer.setArgString(F("tdLightMinMax"),toText(MinLightReading, MaxLightReading,"/"));

  WebServer.setArgString(F("tdisBright"),isBrightToText());
  WebServer.setArgString(F("tdLightOn"), timetoText(MySettings.LightOnHour,MySettings.LightOnMinute)); 
  WebServer.setArgString(F("tdLightOff"), timetoText(MySettings.LightOffHour,MySettings.LightOffMinute)); 
  
  WebServer.setArgString("tdAeroPressure",toText(AeroPressure,AeroPressurePSI,"/"));
  WebServer.setArgString(F("tdisAeroPumpOn"),pumpStateToText());
  WebServer.setArgString(F("tdisAirPumpOn"),stateToText(MySettings.isAirPumpOn));
    
  WebServer.setArgString(F("tdReservoir"),reservoirText);
  WebServer.setArgString(F("tdPH"),toText(PH));  
  }
}

//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  if (strcmp_P(button,(PGM_P)F("btn_LightOn"))==0) { turnLightON(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanOff"))==0) {internalFanOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanLow"))==0) { internalFanLow();}
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanHigh"))==0) {internalFanHigh(); }  
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanOff"))==0) { exhaustFanOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanLow"))==0) { exhaustFanLow();}
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanHigh"))==0) { exhaustFanHigh();}
  else if (strcmp_P(button,(PGM_P)F("btn_AirPumpOn"))==0) { airPumpOn();}
  else if (strcmp_P(button,(PGM_P)F("btn_AirPumpOff"))==0) { airPumpOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_LightOff"))==0) { turnLightOFF(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_LightCalibrate"))==0) {triggerCalibrateLights();}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroSprayNow"))==0) { aeroSprayNow();}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroSprayOff"))==0) { aeroSprayOff();}  
  else if (strcmp_P(button,(PGM_P)F("btn_PumpRefill"))==0) { aeroPumpRefill();}
  else if (strcmp_P(button,(PGM_P)F("btn_PumpStop"))==0) { aeroPumpStop();}
  else if (strcmp_P(button,(PGM_P)F("btn_PumpDisable"))==0) { aeroPumpDisable();}  
  else if (strcmp_P(button,(PGM_P)F("btn_PressureCalibrate"))==0) { calibratePressureSensor();}
  else if (strcmp_P(button,(PGM_P)F("btn_GoogleSheets"))==0) { ReportToGoogleSheets(true);} 
  else if (strcmp_P(button,(PGM_P)F("btn_Mqtt"))==0) { mqttPublush(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_Ee"))==0) { playEE(); }
  else if (strcmp_P(button,(PGM_P)F("btn_SaveSettings"))==0) { saveSettings(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_AlertTest"))==0) { sendTestEmailAlert();}
  saveSettings(false); 
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  if(strcmp_P(field,(PGM_P)F("Brightness"))==0) {setBrightness(WebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("AutoInternalFan"))==0) {autoFanOnOff(WebServer.getArgBoolean(),true);}
  else if(strcmp_P(field,(PGM_P)F("InternalFanSwitchTemp"))==0) {setInternalSwitchTemp(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AutoExhaustFan"))==0) {autoFanOnOff(WebServer.getArgBoolean(),false);}
  else if(strcmp_P(field,(PGM_P)F("ExhaustFanHighHumid"))==0) {setExhaustHighHumidity(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("ExhaustFanLowHumid"))==0) {setExhaustLowHumidity(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("ExhaustFanOffHumid"))==0) {setExhaustOffHumidity(WebServer.getArgInt());}
  
  else if(strcmp_P(field,(PGM_P)F("TimerEnabled"))==0) {setTimerOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("SoundEnabled"))==0) {setSoundOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("LightsOnHour"))==0) {setLightsOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("LightsOnMinute"))==0) {setLightsOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("LightsOffHour"))==0) {setLightsOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("LightsOffMinute"))==0) {setLightsOffMinute(WebServer.getArgInt());}       
  else if(strcmp_P(field,(PGM_P)F("AeroInterval"))==0) { setAeroInterval(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroDuration"))==0) {setAeroDuration(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroPressureLow"))==0) {setAeroPressureLow(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("AeroPressureHigh"))==0) {setAeroPressureHigh(WebServer.getArgFloat());} 
  else if(strcmp_P(field,(PGM_P)F("AeroQuietEnabled"))==0) {setQuietOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("AeroRefillBeforeQuiet"))==0) {setQuietRefillOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("AeroQuietFromHour"))==0) {setQuietFromHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroQuietFromMinute"))==0) {setQuietFromMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroQuietToHour"))==0) {setQuietToHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroQuietToMinute"))==0) {setQuietToMinute(WebServer.getArgInt());}  
  else if(strcmp_P(field,(PGM_P)F("AeroSprayEnabled"))==0) {setAeroSprayOnOff(WebServer.getArgBoolean());}
  
  else if(strcmp_P(field,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("DigitDisplayBrightness"))==0) {setDigitDisplayBacklight(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("DigitDisplayValue"))==0) {setDigitDisplayValue(WebServer.getArgInt());}
  
  else if(strcmp_P(field,(PGM_P)F("AlertEmails"))==0) {AlertEmailsOnOff(WebServer.getArgBoolean());}  
  else if(strcmp_P(field,(PGM_P)F("TempAlertLow"))==0) {setTempAlertLow(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("TempAlertHigh"))==0) {setTempAlertHigh(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("HumidityAlertLow"))==0) {setHumidityAlertLow(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("HumidityAlertHigh"))==0) {setHumidityAlertHigh(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("PressureAlertLow"))==0) {setPressureAlertLow(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PressureAlertHigh"))==0) {setPressureAlertHigh(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PHAlertLow"))==0) {setPHAlertLow(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PHAlertHigh"))==0) {setPHAlertHigh(WebServer.getArgFloat());}
  
  saveSettings(false);
} 
