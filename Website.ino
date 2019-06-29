// Callback made from esp-link to notify that it has just come out of a reset
void resetWebServer(void) {  
  logToSerials(F("WebServer (re)starting..."),false);
  while(!ESPLink.Sync())  {
    logToSerials(F("."),false);
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
  WebServer.setArgBoolean(F("AutoInternalFan"), MySettings.AutomaticInternalFan);
  WebServer.setArgString(F("InternalFanSwitchTemp"), toText(MySettings.InternalFanSwitchTemp));
  WebServer.setArgBoolean(F("AutoExhaustFan"), MySettings.AutomaticExhaustFan);
  WebServer.setArgString(F("ExhaustFanHighHumid"), toText(MySettings.ExhaustFanHighHumid));
  WebServer.setArgString(F("ExhaustFanLowHumid"), toText(MySettings.ExhaustFanLowHumid));
  WebServer.setArgString(F("ExhaustFanOffHumid"), toText(MySettings.ExhaustFanOffHumid));
    
  WebServer.setArgBoolean(F("TimerEnabled"), MySettings.TimerEnabled);
  WebServer.setArgInt(F("LightsOnHour"), MySettings.LightOnHour); 
  WebServer.setArgInt(F("LightsOnMinute"), MySettings.LightOnMinute); 
  WebServer.setArgInt(F("LightsOffHour"), MySettings.LightOffHour); 
  WebServer.setArgInt(F("LightsOffMinute"),MySettings.LightOffMinute);
  WebServer.setArgInt(F("Brightness"), MySettings.LightBrightness);
  WebServer.setArgInt(F("sliderBrightness"), MySettings.LightBrightness);

  WebServer.setArgInt(F("AeroInterval"), MySettings.AeroInterval);
  WebServer.setArgInt(F("AeroDuration"), MySettings.AeroDuration);
  WebServer.setArgBoolean(F("AeroSprayEnabled"), MySettings.AeroSprayEnabled);
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

  WebServer.setArgInt(F("AeroPumpTimeout"), MySettings.AeroPumpTimeout);
  WebServer.setArgInt(F("AeroPrimingTime"), MySettings.AeroPrimingTime);  
  WebServer.setArgBoolean(F("AeroBlowOff"), MySettings.AeroBlowOff);  
  }
}

void RefreshCallback(char * url) //called when website is refreshed
{ 
  WebServer.setArgString(F("Time"), getFormattedTime()); 
  WebServer.setArgString(F("tdLightOK"), statusToText(LightOK)); 
  WebServer.setArgString(F("tdPressureOK"),statusToText(PressureOK));
  WebServer.setArgString(F("tdPumpOK"),statusToText(PumpOK));
  WebServer.setArgString(F("tdACPowerOK"),statusToText(ACPowerOK));
  WebServer.setArgString(F("tdDCPowerOK"),statusToText(DCPowerOK));
  WebServer.setArgString(F("tdVentOK"),statusToText(VentOK));
  WebServer.setArgString(F("tdReservOK"),statusToText(ReservOK));
  WebServer.setArgString(F("tdPhOK"),statusToText(PhOK));
  WebServer.setArgJson(F("list_SerialLog"), eventLogToJSON(false)); //Last events that happened in JSON format
  
  if (strcmp(url,"/GrowBox.html.json")==0){      
  WebServer.setArgString(F("tdTemp"),toText(ExternalTemp,InternalTemp,"/"));
  WebServer.setArgString(F("tdHumidity"),toText(ExternalHumidity,InternalHumidity,"/"));
  WebServer.setArgString(F("tdInternalFanSpeed"),fanSpeedToText(true));
  WebServer.setArgString(F("tdExhaustFanSpeed"),fanSpeedToText(false));
 
  WebServer.setArgString(F("tdPower"),toText(Power));  
  WebServer.setArgString(F("tdEnergy"),toText(Energy));
  WebServer.setArgString(F("tdVoltage"),toText(Voltage));
  WebServer.setArgString(F("tdCurrent"),toText(Current));
  WebServer.setArgString(F("tdATXStatus"),stateToText(MySettings.ATXPowerSupplyOn)); 

  WebServer.setArgString(F("tdLightStatus"),stateToText(MySettings.LightStatus));
  WebServer.setArgString(F("tdLightReading"),toText(MySettings.LightBrightness, LightReading,"%-")); 
  WebServer.setArgString(F("tdLightMinMax"),toText(MinLightReading, MaxLightReading,"/"));

  WebServer.setArgString(F("tdBright"),BrightToText());
  WebServer.setArgString(F("tdLightOn"), timetoText(MySettings.LightOnHour,MySettings.LightOnMinute)); 
  WebServer.setArgString(F("tdLightOff"), timetoText(MySettings.LightOffHour,MySettings.LightOffMinute)); 
  
  WebServer.setArgString("tdAeroPressure",toText(AeroPressure));
  WebServer.setArgString(F("tdAeroPumpOn"),pumpStateToText());
      
  WebServer.setArgString(F("tdReservoir"),ReservoirText);
  WebServer.setArgString(F("tdPH"),toText(PH));
  WebServer.setArgString(F("tdReservoirTemp"),toText(ReservoirTemp));  
  }
}

//Called when any button on the website is pressed
void ButtonPressCallback(char *button)
{
  if (strcmp_P(button,(PGM_P)F("btn_LightOn"))==0) { turnLightON(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_LightOff"))==0) { turnLightOFF(true); }
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanOff"))==0) {internalFanOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanLow"))==0) { internalFanLow();}
  else if (strcmp_P(button,(PGM_P)F("btn_InternalFanHigh"))==0) {internalFanHigh(); }  
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanOff"))==0) { exhaustFanOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanLow"))==0) { exhaustFanLow();}
  else if (strcmp_P(button,(PGM_P)F("btn_ExhaustFanHigh"))==0) { exhaustFanHigh();}
  else if (strcmp_P(button,(PGM_P)F("btn_ATXOn"))==0) { TurnATXOn();}
  else if (strcmp_P(button,(PGM_P)F("btn_ATXOff"))==0) { TurnATXOff();}
  else if (strcmp_P(button,(PGM_P)F("btn_LightCalibrate"))==0) {triggerCalibrateLights();}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroSprayNow"))==0) { aeroSprayNow(false);}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroSprayOff"))==0) { aeroSprayOff();}  
  else if (strcmp_P(button,(PGM_P)F("btn_PumpOn"))==0) { aeroPumpOn(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_PumpOff"))==0) { aeroPumpOff(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_PumpDisable"))==0) { aeroPumpDisable();}
  else if (strcmp_P(button,(PGM_P)F("btn_AeroMix"))==0) { aeroMix();}    
  else if (strcmp_P(button,(PGM_P)F("btn_GoogleSheets"))==0) { ReportToGoogleSheets(true);} 
  else if (strcmp_P(button,(PGM_P)F("btn_Mqtt"))==0) { mqttPublush(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_Ee"))==0) { playEE(); }
  else if (strcmp_P(button,(PGM_P)F("btn_AlertTest"))==0) { sendTestEmailAlert();}
  else if (strcmp_P(button,(PGM_P)F("btn_DigitDisplayLoop"))==0) { setDigitDisplayLoop();}
  else if (strcmp_P(button,(PGM_P)F("btn_DigitDisplayLockTemp"))==0) { setDigitDisplayLockTemp();}
  else if (strcmp_P(button,(PGM_P)F("btn_DigitDisplayLockPH"))==0) { setDigitDisplayLockPH(); }
  else if (strcmp_P(button,(PGM_P)F("btn_DigitDisplayLockPressure"))==0) { setDigitDisplayLockPressure();}
  else if (strcmp_P(button,(PGM_P)F("btn_DigitDisplayLockWattage"))==0) { setDigitDisplayLockWattage();}
  else if (strcmp_P(button,(PGM_P)F("btn_PressureCalibrate"))==0) { calibratePressureSensor();}
  else if (strcmp_P(button,(PGM_P)F("btn_PHCalibrate"))==0) { readReservoirPH(true);}
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
  else if(strcmp_P(field,(PGM_P)F("LightsOnHour"))==0) {setLightsOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("LightsOnMinute"))==0) {setLightsOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("LightsOffHour"))==0) {setLightsOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("LightsOffMinute"))==0) {setLightsOffMinute(WebServer.getArgInt());}       
  else if(strcmp_P(field,(PGM_P)F("AeroInterval"))==0) { setAeroInterval(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroDuration"))==0) {setAeroDuration(WebServer.getArgInt());} 

  else if(strcmp_P(field,(PGM_P)F("SoundEnabled"))==0) {setSoundOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("DebugEnabled"))==0) {setDebugOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MetricSystemEnabled"))==0) {setMetricSystemEnabled(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}
  
  else if(strcmp_P(field,(PGM_P)F("DigitDisplayBrightness"))==0) {setDigitDisplayBacklight(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("DigitDisplayValue"))==0) {setDigitDisplayValue(WebServer.getArgInt());}
  
  else if(strcmp_P(field,(PGM_P)F("AlertEmails"))==0) {AlertEmailsOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("PushingBoxEmailRelayID"))==0) {setPushingBoxEmailRelayID(WebServer.getArgString());}
  else if(strcmp_P(field,(PGM_P)F("ReadCountBeforeAlert"))==0) {setReadCountBeforeAlert(WebServer.getArgInt());}   
  else if(strcmp_P(field,(PGM_P)F("TempAlertLow"))==0) {setTempAlertLow(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("TempAlertHigh"))==0) {setTempAlertHigh(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("HumidityAlertLow"))==0) {setHumidityAlertLow(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("HumidityAlertHigh"))==0) {setHumidityAlertHigh(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("PressureAlertLow"))==0) {setPressureAlertLow(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PressureAlertHigh"))==0) {setPressureAlertHigh(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PHAlertLow"))==0) {setPHAlertLow(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PHAlertHigh"))==0) {setPHAlertHigh(WebServer.getArgFloat());}

  else if(strcmp_P(field,(PGM_P)F("PHCalibrationSlope"))==0) {setPHCalibrationSlope(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PHCalibrationIntercept"))==0) {setPHCalibrationIntercept(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PressureSensorOffset"))==0) {setPressureSensorOffset(WebServer.getArgFloat());}
  else if(strcmp_P(field,(PGM_P)F("PressureSensorRatio"))==0) {setPressureSensorRatio(WebServer.getArgFloat());}

  else if(strcmp_P(field,(PGM_P)F("AeroPumpTimeout"))==0) {setAeroPumpTimeout(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("AeroPrimingTime"))==0) {setAeroPrimingTime(WebServer.getArgInt());}  
  else if(strcmp_P(field,(PGM_P)F("AeroBlowOff"))==0) {setAeroBlowOnOff(WebServer.getArgBoolean());}  
  else if(strcmp_P(field,(PGM_P)F("AeroSprayEnabled"))==0) {setAeroSprayOnOff(WebServer.getArgBoolean());} 
    
  saveSettings(false);
} 
