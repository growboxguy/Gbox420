void LoadCallback(char * url) //called when website is loaded
{
  //LogToSerials("LoadCB for URL: ",false); LogToSerials(url,true);
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
  WebServer.setArgString(F("num_AeroPressureLow"), floatToChar(MySettings.AeroPressureLow));
  WebServer.setArgString(F("num_AeroPressureHigh"), floatToChar(MySettings.AeroPressureHigh));
  WebServer.setArgBoolean(F("check_AeroSprayEnabled"), MySettings.isAeroSprayEnabled);
  WebServer.setArgBoolean(F("check_AeroQuietEnabled"), MySettings.isAeroQuietEnabled);
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
  //LogToSerials("RefreshCB for URL: ",false); LogToSerials(url,true);
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
  WebServer.setArgFloat(F("num_AeroOffset"), MySettings.AeroOffset);
    
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
  WebServer.setArgJson(F("list_SerialLog"), WebMessage);
  }
}

//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  //LogToSerials(button,true);
  if (strcmp(button,"btn_LightOn")==0) { turnLightON(); }
  else if (strcmp(button,"btn_LightOff")==0) { turnLightOFF(); }
  else if (strcmp(button,"btn_LightCalibrate")==0) {triggerCalibrateLights();}
  else if (strcmp(button,"btn_PowersupplyOn")==0) { powerSupplyOn();}
  else if (strcmp(button,"btn_PowersupplyOff")==0) { powerSupplyOff();}
  else if (strcmp(button,"btn_Ee")==0) { playEE(); }
  else if (strcmp(button,"btn_InternalFanOff")==0) {internalFanOff();}
  else if (strcmp(button,"btn_InternalFanLow")==0) { internalFanLow();}
  else if (strcmp(button,"btn_InternalFanHigh")==0) {internalFanHigh(); }  
  else if (strcmp(button,"btn_ExhaustFanOff")==0) { exhaustFanOff();}
  else if (strcmp(button,"btn_ExhaustFanLow")==0) { exhaustFanLow();}
  else if (strcmp(button,"btn_ExhaustFanHigh")==0) { exhaustFanHigh();}
  else if (strcmp(button,"btn_GoogleSheets")==0) { ReportToGoogleSheets(true);} 
  else if (strcmp(button,"btn_Mqtt")==0) { mqttPublush(true);}
  else if (strcmp(button,"btn_SaveSettings")==0) { saveSettings(true);}
  else if (strcmp(button,"btn_AeroSprayNow")==0) { aeroSprayNow();}
  else if (strcmp(button,"btn_AeroSprayOff")==0) { aeroSprayOff();}  
  else if (strcmp(button,"btn_PumpRefill")==0) { aeroPumpRefill();}
  else if (strcmp(button,"btn_PumpDisable")==0) { aeroPumpDisable();}  
  else if (strcmp(button,"btn_PressureCalibrate")==0) { calibratePressureSensor();}
  else if (strcmp(button,"btn_NtpTime")==0) { UpdateNtpTime = true;}  
  saveSettings(false); 
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  //LogToSerials(field,true);
  if(strcmp(field,"slider_Brightness")==0) {setBrightness(WebServer.getArgInt());}
  else if(strcmp(field,"slider_DigitDisplayBrightness")==0) {setDigitDisplayBacklight(WebServer.getArgInt());}
  else if(strcmp(field,"check_TimerEnabled")==0) {setTimerOnOff(WebServer.getArgBoolean());}
  else if(strcmp(field,"check_SoundEnabled")==0) {setSoundOnOff(WebServer.getArgBoolean());}
  else if(strcmp(field,"num_LightsOnHour")==0) {setLightsOnHour(WebServer.getArgInt());}
  else if(strcmp(field,"num_LightsOnMinute")==0) {setLightsOnMinute(WebServer.getArgInt());}
  else if(strcmp(field,"num_LightsOffHour")==0) {setLightsOffHour(WebServer.getArgInt());}
  else if(strcmp(field,"num_LightsOffMinute")==0) {setLightsOffMinute(WebServer.getArgInt());}       
  else if(strcmp(field,"num_AeroInterval")==0) { setAeroInterval(WebServer.getArgInt());}
  else if(strcmp(field,"num_AeroDuration")==0) {setAeroDuration(WebServer.getArgInt());}
  else if(strcmp(field,"num_AeroPressureLow")==0) {setAeroPressureLow(WebServer.getArgFloat());}
  else if(strcmp(field,"num_AeroPressureHigh")==0) {setAeroPressureHigh(WebServer.getArgFloat());} 
  else if(strcmp(field,"num_AeroOffset")==0) {setAeroOffset(WebServer.getArgFloat());}
  else if(strcmp(field,"check_AeroQuietEnabled")==0) {setQuietOnOff(WebServer.getArgBoolean());}
  else if(strcmp(field,"num_AeroQuietFromHour")==0) {setQuietFromHour(WebServer.getArgInt());}
  else if(strcmp(field,"num_AeroQuietFromMinute")==0) {setQuietFromMinute(WebServer.getArgInt());}
  else if(strcmp(field,"num_AeroQuietToHour")==0) {setQuietToHour(WebServer.getArgInt());}
  else if(strcmp(field,"num_AeroQuietToMinute")==0) {setQuietToMinute(WebServer.getArgInt());}  
  else if(strcmp(field,"time_ToSet")==0) {setTime(WebServer.getArgString());}
  else if(strcmp(field,"check_AeroSprayEnabled")==0) {setAeroSprayOnOff(WebServer.getArgBoolean());}
  else if(strcmp(field,"check_GoogleSheetsEnabled")==0) {MySettings.ReportToGoogleSheets = WebServer.getArgBoolean();}
  else if(strcmp(field,"check_MqttEnabled")==0) {MySettings.ReportToMqtt = WebServer.getArgBoolean();}
  
  saveSettings(false);
} 

// Callback made from esp-link to notify that it has just come out of a reset
void resetWebServer(void) {  
  addToLog("WebServer (re)starting");
  while(!ESPLink.Sync())  {
    LogToSerials(".",false);
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
