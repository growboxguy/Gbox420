#include "Common.h"

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
  Common::logToSerials(F("WebServer (re)starting..."),false);
  while(!MyESPLink.Sync())  {
    Common::logToSerials(F("."),false);
    delay(500);
    };
  //RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); //setup handling request from GrowBox.html
  GrowBoxHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  SettingsHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  Common::addToLog(F("WebServer started"));
}

void loadCallback(char * url) //called when website is loaded
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

void refreshCallback(char * url) //called when website is refreshed
{ 
  WebServer.setArgString(F("Time"), Common::getFormattedTime());
  WebServer.setArgJson(F("list_SerialLog"), Common::eventLogToJSON(false)); //Last events that happened in JSON format
   
    
  if (strcmp(url,"/GrowBox.html.json")==0){  
  WebServer.setArgString(F("tdLight1Status"),Light1 -> getStatusText());
  //WebServer.setArgString(F("tdLight1Reading"),toText(MySettings.LightBrightness, LightReading,"%-"));
  //WebServer.setArgString(F("tdBright"),yesNoToText());       
  WebServer.setArgString(F("tdTemp"),Common::toText(InternalDHTSensor -> getTemp(),ExternalDHTSensor -> getTemp()," / "));
  WebServer.setArgString(F("tdHumidity"),Common::toText(InternalDHTSensor -> getHumidity(),ExternalDHTSensor -> getHumidity()," / ")); 
  WebServer.setArgString(F("tdLight1TimerEnabled"),Light1 -> getTimerOnOffText());
  WebServer.setArgString(F("tdLight1On"), Light1 -> getOnTimeText());
  WebServer.setArgString(F("tdLight1Off"), Light1 -> getOffTimeText());  
  }
}

//Called when any button on the website is pressed
void buttonPressCallback(char *button)
{
  if (strcmp_P(button,(PGM_P)F("btn_Light1On"))==0) { Light1 -> setLightOnOff(true,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1Off"))==0) { Light1 -> setLightOnOff(false,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1Calibrate"))==0) {Light1 -> triggerCalibrateLights();}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1TimerEnable"))==0) {Light1 -> setTimerOnOff(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1TimerDisable"))==0) {Light1 -> setTimerOnOff(false);}
  
  Common::saveSettings(false); 
}

//Called when any field on the website is updated
void setFieldCallback(char * field){
  if(strcmp_P(field,(PGM_P)F("Light1Brightness"))==0) {Light1 -> setBrightness(WebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("Light1OnHour"))==0) {Light1 -> setOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OnMinute"))==0) {Light1 -> setOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffHour"))==0) {Light1 -> setOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffMinute"))==0) {Light1 -> setOffMinute(WebServer.getArgInt());} 


  //else if(strcmp_P(field,(PGM_P)F("SoundEnabled"))==0) {setSoundOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("DebugEnabled"))==0) {setDebugOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MetricSystemEnabled"))==0) {Common::setMetricSystemEnabled(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}
 
    
  Common::saveSettings(false);
} 
