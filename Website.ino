void LoadCallback(char * url) //called when website is loaded
{
  Serial.print("LoadCB for URL: "); Serial.println(url);
  if (strcmp(url,"/GrowBox.html.json")==0){  
  WebServer.setArgBoolean(F("check_timerEnabled"), MySettings.isTimerEnabled);
  WebServer.setArgInt(F("num_lightsOnHour"), MySettings.LightOnHour); 
  WebServer.setArgInt(F("num_lightsOnMinute"), MySettings.LightOnMinute); 
  WebServer.setArgInt(F("num_lightsOffHour"), MySettings.LightOffHour); 
  WebServer.setArgInt(F("num_lightsOffMinute"),MySettings.LightOffMinute);
  WebServer.setArgInt(F("slider_brightness"), MySettings.LightBrightness);
  WebServer.setArgInt(F("num_brightness"), MySettings.LightBrightness);

  WebServer.setArgInt(F("num_aeroInterval"), MySettings.AeroInterval/60000);
  WebServer.setArgInt(F("num_aeroDuration"), MySettings.AeroDuration/1000);
  WebServer.setArgString(F("num_aeroPressureLow"), floatToChar(MySettings.AeroPressureLow));
  WebServer.setArgString(F("num_aeroPressureHigh"), floatToChar(MySettings.AeroPressureHigh));
  WebServer.setArgBoolean(F("check_aeroSprayEnabled"), MySettings.isAeroSprayEnabled);

  WebServer.setArgInt(F("slider_digitDisplayBrightness"), MySettings.DigitDisplayBacklight);
  WebServer.setArgInt(F("check_soundEnabled"), MySettings.isSoundEnabled);
  }
}

void RefreshCallback(char * url) //called when website is refreshed
{ 
  Serial.print("RefreshCB for URL: "); Serial.println(url);
  if (strcmp(url,"/GrowBox.html.json")==0){   
  WebServer.setArgString(F("tdTime"), CurrentTime); 
  WebServer.setArgString(F("tdBoxTemp"),floatsToChar(BoxTempC,BoxTempF,"/"));
  WebServer.setArgString(F("tdHumidity"),floatToChar(Humidity));
  if(!MySettings.isInternalFanOn) WebServer.setArgString(F("tdInternalFanSpeed"),F("OFF")); else if(MySettings.isInternalFanHigh) WebServer.setArgString(F("tdInternalFanSpeed"),F("High"));else WebServer.setArgString(F("tdInternalFanSpeed"),F("Low"));
  if(!MySettings.isExhaustFanOn) WebServer.setArgString(F("tdExhaustFanSpeed"),F("OFF")); else if(MySettings.isExhaustFanHigh) WebServer.setArgString(F("tdExhaustFanSpeed"),F("High"));else WebServer.setArgString(F("tdExhaustFanSpeed"),F("Low"));
  
  if(MySettings.isPCPowerSupplyOn) WebServer.setArgString(F("tdisPowersupplyOn"), F("ON"));else WebServer.setArgString(F("tdisPowersupplyOn"),F("OFF"));  
  WebServer.setArgString(F("tdPower"),floatToChar(Power));  
  WebServer.setArgString(F("tdEnergy"),floatToChar(Energy));
  WebServer.setArgString(F("tdVoltage"),floatToChar(Voltage));
  WebServer.setArgString(F("tdCurrent"),floatToChar(Current)); 

  if(MySettings.isLightOn) WebServer.setArgString(F("tdisLightOn"),F("ON")); else WebServer.setArgString(F("tdisLightOn"),F("OFF"));
  WebServer.setArgString(F("tdLightReading"),intsToChar(MySettings.LightBrightness, LightReading,"%-")); 
  WebServer.setArgString(F("tdLightMinMax"),intsToChar(MinLightReading, MaxLightReading,"/"));
  if(isBright) WebServer.setArgString(F("tdisBright"),F("Bright")); else WebServer.setArgString(F("tdisBright"),F("Dark"));
  WebServer.setArgString(F("tdLightOn"), intsToChar(MySettings.LightOnHour,MySettings.LightOnMinute,":")); 
  WebServer.setArgString(F("tdLightOff"), intsToChar(MySettings.LightOffHour,MySettings.LightOffMinute,":")); 
  
  WebServer.setArgString("tdAeroPressure",floatsToChar(AeroPressure,AeroPressurePSI,"/"));
  if(isAeroPumpOn) WebServer.setArgString(F("tdisAeroPumpOn"),F("ON")); else WebServer.setArgString(F("tdisAeroPumpOn"),F("OFF"));
  if(isAeroPumpDisabled) WebServer.setArgString(F("tdisAeroPumpDisabled"),F("DISABLED")); else WebServer.setArgString(F("tdisAeroPumpDisabled"),F("OK"));
  WebServer.setArgFloat(F("num_aeroOffset"), MySettings.AeroOffset);
    
  WebServer.setArgString(F("tdReservoir"),reservoirToText(false));
  WebServer.setArgString(F("tdPH"),floatToChar(PH));  
  //WebServer.setArgString("tdMoisture",floatToChar(Moisture)); 

  //Log output  
  memset(&WebMessage[0], 0, sizeof(WebMessage));
  strcat(WebMessage,"[");
  for(int i=LogDepth-1;i >=0 ; i--)
  {
   strcat(WebMessage,"\"");
   strcat(WebMessage,Logs[i]);
   strcat(WebMessage,"\"");
   if(i > 0 ) strcat(WebMessage,",");
  }
  WebMessage[strlen(WebMessage)] = ']';
  WebServer.setArgJson(F("list_serialLog"), WebMessage);
  }
}

//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  //Serial.println(button);
  if (strcmp(button,"btn_lightOn")==0) { turnLightON(); PlayOnSound=true;}
  else if (strcmp(button,"btn_lightOff")==0) { turnLightOFF();PlayOffSound=true; }
  else if (strcmp(button,"btn_lightCalibrate")==0) {addToLog("Lights calibrated"); CalibrateLights = true;  }
  else if (strcmp(button,"btn_powersupplyOn")==0) { addToLog("Power supply ON"); MySettings.isPCPowerSupplyOn = true; PlayOnSound=true;}
  else if (strcmp(button,"btn_powersupplyOff")==0) { addToLog("Power supply OFF"); MySettings.isPCPowerSupplyOn = false;  PlayOffSound=true; }
  else if (strcmp(button,"btn_ee")==0) { addToLog("♬Easter egg♬");PlayEE = true; }
  else if (strcmp(button,"btn_InternalFanOff")==0) { addToLog("Internal fan OFF"); MySettings.isInternalFanOn = false;MySettings.isInternalFanHigh = false;PlayOffSound=true;}
  else if (strcmp(button,"btn_InternalFanLow")==0) { addToLog("Internal fan Low"); MySettings.isInternalFanHigh = false; MySettings.isInternalFanOn = true;PlayOnSound=true;}
  else if (strcmp(button,"btn_InternalFanHigh")==0) { addToLog("Internal fan High"); MySettings.isInternalFanHigh = true; MySettings.isInternalFanOn = true;PlayOnSound=true;}  
  else if (strcmp(button,"btn_ExhaustFanOff")==0) { addToLog("Exhaust fan OFF"); MySettings.isExhaustFanOn = false;MySettings.isExhaustFanHigh = false;PlayOffSound=true;}
  else if (strcmp(button,"btn_ExhaustFanLow")==0) { addToLog("Exhaust fan Low"); MySettings.isExhaustFanHigh = false; MySettings.isExhaustFanOn = true;PlayOnSound=true;}
  else if (strcmp(button,"btn_ExhaustFanHigh")==0) { addToLog("Exhaust fan High"); MySettings.isExhaustFanHigh = true; MySettings.isExhaustFanOn = true;PlayOnSound=true;}
  else if (strcmp(button,"btn_googleSheets")==0) { reportToGoogleSheets();} 
  else if (strcmp(button,"btn_saveSettings")==0) { saveSettings();addToLog("Settings saved to EEPROM");}
  else if (strcmp(button,"btn_aeroSprayNow")==0) { aeroSprayNow();}
  else if (strcmp(button,"btn_aeroSprayOff")==0) { aeroSprayOff();}  
  else if (strcmp(button,"btn_pumpRefill")==0) { aeroPumpRefill();}
  else if (strcmp(button,"btn_pumpOff")==0) { aeroPumpOff();}  
  else if (strcmp(button,"btn_pumpReset")==0) { aeroPumpReset();}
  else if (strcmp(button,"btn_pressureCalibrate")==0) { calibratePressureSensor();}
  saveSettings(); 
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  Serial.println(field);
  if(strcmp(field,"slider_brightness")==0) {setBrightness(WebServer.getArgInt()); }
  else if(strcmp(field,"slider_digitDisplayBrightness")==0) {MySettings.DigitDisplayBacklight=WebServer.getArgInt(); DigitDisplay.setBacklight(MySettings.DigitDisplayBacklight); }
  else if(strcmp(field,"check_timerEnabled")==0) {MySettings.isTimerEnabled = WebServer.getArgBoolean(); checkLightStatus(); if(MySettings.isTimerEnabled){ addToLog("Timer enabled"); PlayOnSound=true;} else {addToLog("Timer disabled");PlayOffSound=true;}}
  else if(strcmp(field,"check_soundEnabled")==0) {MySettings.isSoundEnabled = WebServer.getArgBoolean(); if(MySettings.isSoundEnabled){ addToLog("Sound enabled"); PlayOnSound=true;} else {addToLog("Sound disabled");}}
  else if(strcmp(field,"num_lightsOnHour")==0) {MySettings.LightOnHour = WebServer.getArgInt();  }
  else if(strcmp(field,"num_lightsOnMinute")==0) {MySettings.LightOnMinute = WebServer.getArgInt(); checkLightStatus(); addToLog("Light ON time updated"); }
  else if(strcmp(field,"num_lightsOffHour")==0) {MySettings.LightOffHour = WebServer.getArgInt(); }
  else if(strcmp(field,"num_lightsOffMinute")==0) {MySettings.LightOffMinute = WebServer.getArgInt(); checkLightStatus(); addToLog("Light OFF time updated"); }       
  else if(strcmp(field,"num_aeroInterval")==0) { setAeroInterval(WebServer.getArgInt());}
  else if(strcmp(field,"num_aeroDuration")==0) {setAeroDuration(WebServer.getArgInt()); addToLog("Spray time updated"); }
  else if(strcmp(field,"num_aeroPressureLow")==0) {MySettings.AeroPressureLow =  WebServer.getArgFloat(); }
  else if(strcmp(field,"num_aeroPressureHigh")==0) {MySettings.AeroPressureHigh = WebServer.getArgFloat(); addToLog("Pump settings updated");} 
  else if(strcmp(field,"num_aeroOffset")==0) {MySettings.AeroOffset = WebServer.getArgFloat(); addToLog("Pressure sensor offset updated");} 
  else if(strcmp(field,"time_toSet")==0) {setTime(WebServer.getArgString()); }
  else if(strcmp(field,"check_aeroSprayEnabled")==0) {MySettings.isAeroSprayEnabled=WebServer.getArgBoolean();if(MySettings.isAeroSprayEnabled){ addToLog("Aeroponics Spray enabled"); PlayOnSound=true;} else {addToLog("Aeroponics Spray disabled");PlayOffSound=true;} }
  saveSettings();
} 

void processWebsite(){
  ESPLink.Process();
}

// Callback made from esp-link to notify that it has just come out of a reset
void resetWebServer(void) {  
  addToLog("WebServer (re)starting");
  while(!ESPLink.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  GrowBoxHandler->loadCb.attach(&LoadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&RefreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&ButtonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&SetFieldCallback); //Called when a field is changed on the website
  addToLog("WebServer started"); 
}
