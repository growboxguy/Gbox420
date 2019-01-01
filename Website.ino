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
  GrowBoxHandler->loadCb.attach(&LoadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&RefreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&ButtonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&SetFieldCallback); //Called when a field is changed on the website
  addToLog(F("WebServer started"));
}

void LoadCallback(char * url) //called when website is loaded
{
  //LogToSerials(F("LoadCB for URL: "),false); LogToSerials(url,true);
  if (strcmp(url,"/GrowBox.html.json")==0){  
  WebServer.setArgBoolean(F("check_TimerEnabled"), MySettings.isTimerEnabled);
  WebServer.setArgInt(F("num_LightsOnHour"), MySettings.LightOnHour); 
  WebServer.setArgInt(F("num_LightsOnMinute"), MySettings.LightOnMinute); 
  WebServer.setArgInt(F("num_LightsOffHour"), MySettings.LightOffHour); 
  WebServer.setArgInt(F("num_LightsOffMinute"),MySettings.LightOffMinute);
  WebServer.setArgInt(F("slider_Brightness"), MySettings.LightBrightness);
  WebServer.setArgInt(F("num_Brightness"), MySettings.LightBrightness);

  WebServer.setArgInt(F("num_AeroInterval"), MySettings.AeroInterval);
  WebServer.setArgInt(F("num_AeroDuration"), MySettings.AeroDuration);
  WebServer.setArgString(F("num_AeroPressureLow"), toText(MySettings.AeroPressureLow));
  WebServer.setArgString(F("num_AeroPressureHigh"), toText(MySettings.AeroPressureHigh));
  WebServer.setArgBoolean(F("check_AeroSprayEnabled"), MySettings.isAeroSprayEnabled);
  WebServer.setArgBoolean(F("check_AeroQuietEnabled"), MySettings.isAeroQuietEnabled);
  WebServer.setArgBoolean(F("check_AeroRefillBeforeQuiet"), MySettings.AeroRefillBeforeQuiet);
  WebServer.setArgInt(F("num_AeroQuietFromHour"), MySettings.AeroQuietFromHour); 
  WebServer.setArgInt(F("num_AeroQuietFromMinute"), MySettings.AeroQuietFromMinute); 
  WebServer.setArgInt(F("num_AeroQuietToHour"), MySettings.AeroQuietToHour); 
  WebServer.setArgInt(F("num_AeroQuietToMinute"),MySettings.AeroQuietToMinute);

  WebServer.setArgBoolean(F("check_GoogleSheetsEnabled"), MySettings.ReportToGoogleSheets);
  WebServer.setArgBoolean(F("check_MqttEnabled"), MySettings.ReportToMqtt);

  WebServer.setArgInt(F("slider_DigitDisplayBrightness"), MySettings.DigitDisplayBacklight);
  WebServer.setArgInt(F("check_SoundEnabled"), MySettings.isSoundEnabled);
  }
}

void RefreshCallback(char * url) //called when website is refreshed
{ 
  //LogToSerials(F("RefreshCB for URL: "),false); LogToSerials(url,true);
  if (strcmp(url,"/GrowBox.html.json")==0){   
  WebServer.setArgString(F("tdTime"), getFormattedTime()); 
  WebServer.setArgString(F("tdBoxTemp"),toText(BoxTempC,BoxTempF,"/"));
  WebServer.setArgString(F("tdHumidity"),toText(Humidity));
  WebServer.setArgString(F("tdInternalFanSpeed"),internalFanSpeedToText());
  WebServer.setArgString(F("tdExhaustFanSpeed"),exhaustFanSpeedToText());

  WebServer.setArgString(F("tdisPowersupplyOn"),powerSupplyToText()); 
  WebServer.setArgString(F("tdPower"),toText(Power));  
  WebServer.setArgString(F("tdEnergy"),toText(Energy));
  WebServer.setArgString(F("tdVoltage"),toText(Voltage));
  WebServer.setArgString(F("tdCurrent"),toText(Current)); 

  WebServer.setArgString(F("tdisLightOn"),lightStatusToText());
  WebServer.setArgString(F("tdLightReading"),toText(MySettings.LightBrightness, LightReading,"%-")); 
  WebServer.setArgString(F("tdLightMinMax"),toText(MinLightReading, MaxLightReading,"/"));

  WebServer.setArgString(F("tdisBright"),isBrightToText());
  WebServer.setArgString(F("tdLightOn"), timetoText(MySettings.LightOnHour,MySettings.LightOnMinute)); 
  WebServer.setArgString(F("tdLightOff"), timetoText(MySettings.LightOffHour,MySettings.LightOffMinute)); 
  
  WebServer.setArgString("tdAeroPressure",toText(AeroPressure,AeroPressurePSI,"/"));
  WebServer.setArgString(F("tdisAeroPumpOn"),pumpStateToText());
  WebServer.setArgString(F("tdisAeroPumpDisabled"),pumpStatusToText());
  WebServer.setArgFloat(F("num_AeroOffset"), MySettings.AeroOffset);
    
  WebServer.setArgString(F("tdReservoir"),reservoirText);
  WebServer.setArgString(F("tdPH"),toText(PH));  
  //WebServer.setArgString("tdMoisture",toText(Moisture)); 
 
  WebServer.setArgJson(F("list_SerialLog"), eventLogToJSON()); //Last events that happened in JSON format
  }
}

//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  //LogToSerials(button,true);  //for debugging: prints the button ID received from the website
  if (strcmp_P(button,(PGM_P)F("btn_LightOn"))==0) { turnLightON(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_LightOff"))==0) { turnLightOFF(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_LightCalibrate"))==0) {triggerCalibrateLights();}
  else if (strcmp_P(button,(PGM_P)F("btn_PowersupplyOn"))==0) { powerSupplyOn();}
  else if (strcmp_P(button,(PGM_P)F("btn_PowersupplyOff"))==0) { powerSupplyOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_Ee"))==0) { playEE(); }
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanOff"))==0) {internalFanOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanLow"))==0) { internalFanLow();}
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanHigh"))==0) {internalFanHigh(); }  
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanOff"))==0) { exhaustFanOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanLow"))==0) { exhaustFanLow();}
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanHigh"))==0) { exhaustFanHigh();}
  else if (strcmp_P(button,(PGM_P)F("btn_GoogleSheets"))==0) { ReportToGoogleSheets(true);} 
  else if (strcmp_P(button,(PGM_P)F("btn_Mqtt"))==0) { mqttPublush(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_SaveSettings"))==0) { saveSettings(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroSprayNow"))==0) { aeroSprayNow();}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroSprayOff"))==0) { aeroSprayOff();}  
  else if (strcmp_P(button,(PGM_P)F("btn_PumpRefill"))==0) { aeroPumpRefill();}
  else if (strcmp_P(button,(PGM_P)F("btn_PumpStop"))==0) { aeroPumpStop();}
  else if (strcmp_P(button,(PGM_P)F("btn_PumpDisable"))==0) { aeroPumpDisable();}  
  else if (strcmp_P(button,(PGM_P)F("btn_PressureCalibrate"))==0) { calibratePressureSensor();}
  saveSettings(false); 
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  //LogToSerials(field,true);  //for debugging: prints the filed ID received from the website
  if(strcmp_P(field,(PGM_P)F("slider_Brightness"))==0) {setBrightness(WebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("slider_DigitDisplayBrightness"))==0) {setDigitDisplayBacklight(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("check_TimerEnabled"))==0) {setTimerOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("check_SoundEnabled"))==0) {setSoundOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("num_LightsOnHour"))==0) {setLightsOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_LightsOnMinute"))==0) {setLightsOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_LightsOffHour"))==0) {setLightsOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_LightsOffMinute"))==0) {setLightsOffMinute(WebServer.getArgInt());}       
  else if(strcmp_P(field,(PGM_P)F("num_AeroInterval"))==0) { setAeroInterval(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroDuration"))==0) {setAeroDuration(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroPressureLow"))==0) {setAeroPressureLow(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroPressureHigh"))==0) {setAeroPressureHigh(WebServer.getArgFloat());} 
  else if(strcmp_P(field,(PGM_P)F("num_AeroOffset"))==0) {setAeroOffset(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("check_AeroQuietEnabled"))==0) {setQuietOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("check_AeroRefillBeforeQuiet"))==0) {setQuietRefillOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroQuietFromHour"))==0) {setQuietFromHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroQuietFromMinute"))==0) {setQuietFromMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroQuietToHour"))==0) {setQuietToHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("num_AeroQuietToMinute"))==0) {setQuietToMinute(WebServer.getArgInt());}  
  else if(strcmp_P(field,(PGM_P)F("check_AeroSprayEnabled"))==0) {setAeroSprayOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("check_GoogleSheetsEnabled"))==0) {MySettings.ReportToGoogleSheets = WebServer.getArgBoolean();}
  else if(strcmp_P(field,(PGM_P)F("check_MqttEnabled"))==0) {MySettings.ReportToMqtt = WebServer.getArgBoolean();}  
  saveSettings(false);
} 
