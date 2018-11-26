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
  if (strcmp(button,"btn_lightOn")==0) { turnLightON(); }
  else if (strcmp(button,"btn_lightOff")==0) { turnLightOFF(); }
  else if (strcmp(button,"btn_lightCalibrate")==0) {triggerCalibrateLights();}
  else if (strcmp(button,"btn_powersupplyOn")==0) { powerSupplyOn();}
  else if (strcmp(button,"btn_powersupplyOff")==0) { powerSupplyOff();}
  else if (strcmp(button,"btn_ee")==0) { playEE(); }
  else if (strcmp(button,"btn_InternalFanOff")==0) {internalFanOff();}
  else if (strcmp(button,"btn_InternalFanLow")==0) { internalFanLow();}
  else if (strcmp(button,"btn_InternalFanHigh")==0) {internalFanHigh(); }  
  else if (strcmp(button,"btn_ExhaustFanOff")==0) { exhaustFanOff();}
  else if (strcmp(button,"btn_ExhaustFanLow")==0) { exhaustFanLow();}
  else if (strcmp(button,"btn_ExhaustFanHigh")==0) { exhaustFanHigh();}
  else if (strcmp(button,"btn_googleSheets")==0) { reportToGoogleSheets();} 
  else if (strcmp(button,"btn_saveSettings")==0) { saveSettings(true);}
  else if (strcmp(button,"btn_aeroSprayNow")==0) { aeroSprayNow();}
  else if (strcmp(button,"btn_aeroSprayOff")==0) { aeroSprayOff();}  
  else if (strcmp(button,"btn_pumpRefill")==0) { aeroPumpRefill();}
  else if (strcmp(button,"btn_pumpOff")==0) { aeroPumpOff();}  
  else if (strcmp(button,"btn_pumpReset")==0) { aeroPumpReset();}
  else if (strcmp(button,"btn_pressureCalibrate")==0) { calibratePressureSensor();}
  else if (strcmp(button,"btn_ntpTime")==0) { Serial.print(EspCmd.GetTime());} //takes too long to process
  saveSettings(false); 
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  Serial.println(field);
  if(strcmp(field,"slider_brightness")==0) {setBrightness(WebServer.getArgInt());}
  else if(strcmp(field,"slider_digitDisplayBrightness")==0) {setDigitDisplayBacklight(WebServer.getArgInt());}
  else if(strcmp(field,"check_timerEnabled")==0) {setTimerOnOff(WebServer.getArgBoolean());}
  else if(strcmp(field,"check_soundEnabled")==0) {setSoundOnOff(WebServer.getArgBoolean());}
  else if(strcmp(field,"num_lightsOnHour")==0) {setLightsOnHour(WebServer.getArgInt());}
  else if(strcmp(field,"num_lightsOnMinute")==0) {setLightsOnMinute(WebServer.getArgInt());}
  else if(strcmp(field,"num_lightsOffHour")==0) {setLightsOffHour(WebServer.getArgInt());}
  else if(strcmp(field,"num_lightsOffMinute")==0) {setLightsOffMinute(WebServer.getArgInt());}       
  else if(strcmp(field,"num_aeroInterval")==0) { setAeroInterval(WebServer.getArgInt());}
  else if(strcmp(field,"num_aeroDuration")==0) {setAeroDuration(WebServer.getArgInt());}
  else if(strcmp(field,"num_aeroPressureLow")==0) {setAeroPressureLow(WebServer.getArgFloat());}
  else if(strcmp(field,"num_aeroPressureHigh")==0) {setAeroPressureHigh(WebServer.getArgFloat());} 
  else if(strcmp(field,"num_aeroOffset")==0) {setAeroOffset(WebServer.getArgFloat());} 
  else if(strcmp(field,"time_toSet")==0) {setTime(WebServer.getArgString());}
  else if(strcmp(field,"check_aeroSprayEnabled")==0) {setAeroSprayOnOff(WebServer.getArgBoolean());}
  saveSettings(false);
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
