#include "420Helpers.h"
#include "GrowBox.h"
#include "DHTSensor.h"
#include "LightSensor.h"
#include "Lights.h"
#include "Buzzer.h"

void resetWebServer(void) {    // Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("WebServer (re)starting..."),false);
  while(!ESPLink.Sync())  {
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

void loadCallback(char * url) //called when website is loaded. Do not call logToSerials within any methods used here!
{
  if (strcmp(url,"/GrowBox.html.json")==0){
  WebServer.setArgBoolean(F("AutoInternalFan"), BoxSettings.AutomaticInternalFan);
  WebServer.setArgString(F("InternalFanSwitchTemp"), toText(BoxSettings.InternalFanSwitchTemp));
  WebServer.setArgBoolean(F("AutoExhaustFan"), BoxSettings.AutomaticExhaustFan);
  WebServer.setArgString(F("ExhaustFanHighHumid"), toText(BoxSettings.ExhaustFanHighHumid));
  WebServer.setArgString(F("ExhaustFanLowHumid"), toText(BoxSettings.ExhaustFanLowHumid));
  WebServer.setArgString(F("ExhaustFanOffHumid"), toText(BoxSettings.ExhaustFanOffHumid));
    
  WebServer.setArgInt(F("Light1OnHour"), BoxSettings.Light1OnHour); 
  WebServer.setArgInt(F("Light1OnMinute"), BoxSettings.Light1OnMinute); 
  WebServer.setArgInt(F("Light1OffHour"), BoxSettings.Light1OffHour); 
  WebServer.setArgInt(F("Light1OffMinute"),BoxSettings.Light1OffMinute);
  WebServer.setArgInt(F("Light1Brightness"), BoxSettings.Light1Brightness);
  WebServer.setArgInt(F("Light1BrightnessSlider"), BoxSettings.Light1Brightness);

  WebServer.setArgInt(F("AeroPumpTimeout"), BoxSettings.AeroPumpTimeout);
  WebServer.setArgInt(F("AeroPrimingTime"), BoxSettings.AeroPrimingTime);
  WebServer.setArgInt(F("AeroInterval"), BoxSettings.AeroInterval);
  WebServer.setArgInt(F("AeroDuration"), BoxSettings.AeroDuration);  
  }
  
  if (strcmp(url,"/Settings.html.json")==0){  
  WebServer.setArgInt(F("SoundEnabled"), BoxSettings.Buzzer1Enabled);
  WebServer.setArgInt(F("DebugEnabled"), BoxSettings.DebugEnabled);
  WebServer.setArgInt(F("MetricSystemEnabled"), BoxSettings.MetricSystemEnabled);
  WebServer.setArgInt(F("DigitDisplayBrightness"), BoxSettings.DigitDisplayBacklight);
  WebServer.setArgInt(F("DigitDisplayValue"), BoxSettings.DigitDisplayValue);
  WebServer.setArgBoolean(F("MqttEnabled"), BoxSettings.ReportToMqtt);
  WebServer.setArgBoolean(F("GoogleSheetsEnabled"), BoxSettings.ReportToGoogleSheets);
  WebServer.setArgString(F("PushingBoxLogRelayID"), BoxSettings.PushingBoxLogRelayID);

  WebServer.setArgBoolean(F("AlertEmails"), BoxSettings.AlertEmails);
  WebServer.setArgString(F("PushingBoxEmailRelayID"), BoxSettings.PushingBoxEmailRelayID);
  WebServer.setArgInt(F("ReadCountBeforeAlert"), BoxSettings.ReadCountBeforeAlert);
  WebServer.setArgInt(F("TempAlertLow"), BoxSettings.TempAlertLow);
  WebServer.setArgInt(F("TempAlertHigh"), BoxSettings.TempAlertHigh);
  WebServer.setArgInt(F("HumidityAlertLow"), BoxSettings.HumidityAlertLow);
  WebServer.setArgInt(F("HumidityAlertHigh"), BoxSettings.HumidityAlertHigh);
  WebServer.setArgString(F("PressureAlertLow"), toText(BoxSettings.PressureAlertLow));
  WebServer.setArgString(F("PressureAlertHigh"), toText(BoxSettings.PressureAlertHigh));
  WebServer.setArgString(F("PHAlertLow"), toText(BoxSettings.PHAlertLow));
  WebServer.setArgString(F("PHAlertHigh"), toText(BoxSettings.PHAlertHigh));  

  WebServer.setArgString(F("PHCalibrationSlope"), toPrecisionText(BoxSettings.PHCalibrationSlope));
  WebServer.setArgString(F("PHCalibrationIntercept"), toPrecisionText(BoxSettings.PHCalibrationIntercept)); 
  WebServer.setArgString(F("PressureSensorOffset"), toPrecisionText(BoxSettings.PressureSensorOffset));
  WebServer.setArgString(F("PressureSensorRatio"), toPrecisionText(BoxSettings.PressureSensorRatio));  
  }
}

void refreshCallback(char * url) //called when website is refreshed. Do not call logToSerials within any methods used here!
{ 
  WebServer.setArgString(F("Time"), getFormattedTime());
  WebServer.setArgJson(F("list_SerialLog"), GBox -> eventLogToJSON(false)); //Last events that happened in JSON format
    
  if (strcmp(url,"/GrowBox.html.json")==0){       
  WebServer.setArgString(F("tdInternalTemp"),GBox -> InternalDHTSensor -> getTempText());
  WebServer.setArgString(F("tdInternalHumidity"),GBox -> InternalDHTSensor -> getHumidityText());
  WebServer.setArgString(F("tdExternalTemp"),GBox -> ExternalDHTSensor -> getTempText());
  WebServer.setArgString(F("tdExternalHumidity"),GBox -> ExternalDHTSensor -> getHumidityText());
  WebServer.setArgString(F("tdLightSensor1IsDark"),GBox -> LightSensor1 -> getIsDarkText());
  WebServer.setArgString(F("tdLightSensor1Reading"),(PGM_P)GBox -> LightSensor1 -> getReadingPercentage());
  WebServer.setArgString(F("tdLightSensor1ReadingRaw"),GBox -> LightSensor1 -> getReadingText()); 
  WebServer.setArgString(F("tdLight1Status"),GBox -> Light1 -> getStatusText());
  WebServer.setArgString(F("tdLight1TimerEnabled"),GBox -> Light1 -> getTimerOnOffText());
  WebServer.setArgString(F("tdLight1On"), GBox -> Light1 -> getOnTimeText());
  WebServer.setArgString(F("tdLight1Off"), GBox -> Light1 -> getOffTimeText());  
  }
}

void buttonPressCallback(char *button)  //Called when any button on the website is pressed. Do not call logToSerials within any methods used here!
{
  if (strcmp_P(button,(PGM_P)F("btn_Light1On"))==0) {GBox -> Light1 -> setLightOnOff(true,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1Off"))==0) {GBox -> Light1 -> setLightOnOff(false,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1Calibrate"))==0) {GBox -> Light1 -> triggerCalibrateLights();}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1TimerEnable"))==0) {GBox -> Light1 -> setTimerOnOff(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1TimerDisable"))==0) {GBox -> Light1 -> setTimerOnOff(false);}
  else if (strcmp_P(button,(PGM_P)F("btn_Ee"))==0) { GBox -> Buzzer1 ->playEE(); }
  saveSettings(false); 
}

void setFieldCallback(char * field){  //Called when any field on the website is updated. Do not call logToSerials within any methods used here!
  if(strcmp_P(field,(PGM_P)F("Light1Brightness"))==0) {GBox -> Light1 -> setBrightness(WebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("Light1OnHour"))==0) {GBox -> Light1 -> setOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OnMinute"))==0) {GBox -> Light1 -> setOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffHour"))==0) {GBox -> Light1 -> setOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffMinute"))==0) {GBox -> Light1 -> setOffMinute(WebServer.getArgInt());} 


  else if(strcmp_P(field,(PGM_P)F("SoundEnabled"))==0) {GBox -> Buzzer1 -> setSoundOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("DebugEnabled"))==0) {setDebugOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MetricSystemEnabled"))==0) {GBox -> setMetricSystemEnabled(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}
 
    
  saveSettings(false);
} 
