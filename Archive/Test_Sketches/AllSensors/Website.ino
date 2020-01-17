// Callback made from esp-link to notify that it has just come out of a reset
void ResetWebServer(void) {  
  addToLog("WebServer (re)starting");
  while(!esp.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  rest.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  webServer.setup();
  URLHandler *GrowBoxHandler = webServer.createURLHandler(F("/GrowBox.html.json")); //setup handling request from GrowBox.html
  GrowBoxHandler->buttonCb.attach(ButtonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(SetFieldCallback); //Called when a field is changed on the website
  GrowBoxHandler->loadCb.attach(LoadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(RefreshCallback); //Called periodically to refresh website content 
  addToLog("WebServer started"); 
}


//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  Serial.println(button);
  if (strcmp(button,"btn_lightOn")==0) { TurnLightON(); }
  else if (strcmp(button,"btn_lightOff")==0) { TurnLightOFF(); }
  else if (strcmp(button,"btn_lightCalibrate")==0) {addToLog("Lights calibrated"); calibrateLights();  }
  else if (strcmp(button,"btn_powersupplyOn")==0) { addToLog("Power supply ON"); isPCPowerSupplyOn = true; OnSound();}
  else if (strcmp(button,"btn_powersupplyOff")==0) { addToLog("Power supply OFF"); isPCPowerSupplyOn = false;  OffSound(); }
  else if (strcmp(button,"btn_ee")==0) {   addToLog("♬Easter egg♬");mario(); }
  else if (strcmp(button,"btn_fanLow")==0) {   addToLog("Fan Low"); }
  else if (strcmp(button,"btn_fanMedium")==0) { addToLog("Fan Medium"); }
  else if (strcmp(button,"btn_fanHigh")==0) { addToLog("Fan High"); }    
  SaveToSD(); 
}

void LoadCallback(char * url) //called when website is refreshed
{
Serial.print("LoadCB for URL: ");  Serial.println(url);
webServer.setArgInt(F("num_lightsOnHour"), LightOnHour); 
webServer.setArgInt(F("num_lightsOnMinute"), LightOnMinute); 
webServer.setArgInt(F("num_lightsOffHour"), LightOffHour); 
webServer.setArgInt(F("num_lightsOffMinute"),LightOffMinute);
webServer.setArgInt(F("slider_brightness"), Brightness);
webServer.setArgInt(F("num_brightness"), Brightness);
webServer.setArgBoolean(F("check_timerEnabled"), isTimerEnabled);
RefreshCallback(url);
}

void RefreshCallback(char * url) //called when website is refreshed
{ 
  webServer.setArgString(F("tdWattage"),floatToChar(Wattage));  
  webServer.setArgString(F("tdEnergy"),floatToChar(Energy));
  //utilization calc here
  
  webServer.setArgString(F("tdBoxTemp"),floatsToChar(BoxTempC,BoxTempF,"-"));
  webServer.setArgString(F("tdHumidity"),floatToChar(Humidity));
  webServer.setArgString(F("tdPH"),floatToChar(PH));

  if(isLightOn) webServer.setArgString(F("tdisLightOn"),F("ON")); else webServer.setArgString(F("tdisLightOn"),F("OFF"));
  webServer.setArgString(F("tdLightReading"),intsToChar(Brightness, LightReading,"%-")); 
  webServer.setArgString(F("tdLightMinMax"),intsToChar(MinLightReading, MaxLightReading,"/"));
  
  if(isBright) webServer.setArgString(F("tdisBright"),F("Bright")); else webServer.setArgString(F("tdisBright"),F("Dark"));
  if(isPCPowerSupplyOn) webServer.setArgString(F("tdisPowersupplyOn"), F("ON"));else webServer.setArgString(F("tdisPowersupplyOn"),F("OFF"));  
  //fan speed here

  webServer.setArgString(F("tdTime"), CurrentTime); 
  webServer.setArgString(F("tdLightOn"), intsToChar(LightOnHour,LightOnMinute,":")); 
  webServer.setArgString(F("tdLightOff"), intsToChar(LightOffHour,LightOffMinute,":")); 
  
  //webServer.setArgString("tdMoisture",floatToChar(Moisture));
  //webServer.setArgString("tdVoltage",floatToChar(Voltage));
  //webServer.setArgString("tdCurrent",floatToChar(Current));    

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
  WebMessage[strlen(WebMessage)] = ']';  //replace the last , to ]
  webServer.setArgJson(F("list_serialLog"), WebMessage);
}

//Called when any field on the website is updated
void SetFieldCallback(char * field){
  if(strcmp(field,"slider_brightness")==0) {SetBrightness(webServer.getArgInt()); }
  else if(strcmp(field,"check_timerEnabled")==0) {isTimerEnabled = webServer.getArgBoolean(); CheckLightStatus(); if(isTimerEnabled){ addToLog("Timer enabled"); OnSound();} else {addToLog("Timer disabled");OffSound();}}
  else if(strcmp(field,"num_lightsOnHour")==0) {LightOnHour = webServer.getArgInt();  }
  else if(strcmp(field,"num_lightsOnMinute")==0) {LightOnMinute = webServer.getArgInt(); CheckLightStatus(); addToLog("Light ON time updated"); }
  else if(strcmp(field,"num_lightsOffHour")==0) {LightOffHour = webServer.getArgInt(); }
  else if(strcmp(field,"num_lightsOffMinute")==0) {LightOffMinute = webServer.getArgInt(); CheckLightStatus(); addToLog("Light OFF time updated"); }       
  SaveToSD();
} 
