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
  MyWebServer.setup();
  URLHandler *GrowBoxHandler = MyWebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = MyWebServer.createURLHandler("/Settings.html.json"); //setup handling request from GrowBox.html
  GrowBoxHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  SettingsHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
//  addToLog(F("WebServer started"));
}

void loadCallback(char * url) //called when website is loaded
{
  if (strcmp(url,"/GrowBox.html.json")==0){
  MyWebServer.setArgBoolean(F("AutoInternalFan"), Common::MySettings.AutomaticInternalFan);
  MyWebServer.setArgString(F("InternalFanSwitchTemp"), Common::toText(Common::MySettings.InternalFanSwitchTemp));
  MyWebServer.setArgBoolean(F("AutoExhaustFan"), Common::MySettings.AutomaticExhaustFan);
  MyWebServer.setArgString(F("ExhaustFanHighHumid"), Common::toText(Common::MySettings.ExhaustFanHighHumid));
  MyWebServer.setArgString(F("ExhaustFanLowHumid"), Common::toText(Common::MySettings.ExhaustFanLowHumid));
  MyWebServer.setArgString(F("ExhaustFanOffHumid"), Common::toText(Common::MySettings.ExhaustFanOffHumid));
    
  MyWebServer.setArgInt(F("Light1OnHour"), Common::MySettings.Light1OnHour); 
  MyWebServer.setArgInt(F("Light1OnMinute"), Common::MySettings.Light1OnMinute); 
  MyWebServer.setArgInt(F("Light1OffHour"), Common::MySettings.Light1OffHour); 
  MyWebServer.setArgInt(F("Light1OffMinute"),Common::MySettings.Light1OffMinute);
  MyWebServer.setArgInt(F("Light1Brightness"), Common::MySettings.Light1Brightness);
  MyWebServer.setArgInt(F("Light1BrightnessSlider"), Common::MySettings.Light1Brightness);

  MyWebServer.setArgInt(F("AeroPumpTimeout"), Common::MySettings.AeroPumpTimeout);
  MyWebServer.setArgInt(F("AeroPrimingTime"), Common::MySettings.AeroPrimingTime);
  MyWebServer.setArgInt(F("AeroInterval"), Common::MySettings.AeroInterval);
  MyWebServer.setArgInt(F("AeroDuration"), Common::MySettings.AeroDuration);  
  }
  
  if (strcmp(url,"/Settings.html.json")==0){  
  MyWebServer.setArgInt(F("SoundEnabled"), Common::MySettings.SoundEnabled);
  MyWebServer.setArgInt(F("DebugEnabled"), Common::MySettings.DebugEnabled);
  MyWebServer.setArgInt(F("MetricSystemEnabled"), Common::MySettings.MetricSystemEnabled);
  MyWebServer.setArgInt(F("DigitDisplayBrightness"), Common::MySettings.DigitDisplayBacklight);
  MyWebServer.setArgInt(F("DigitDisplayValue"), Common::MySettings.DigitDisplayValue);
  MyWebServer.setArgBoolean(F("MqttEnabled"), Common::MySettings.ReportToMqtt);
  MyWebServer.setArgBoolean(F("GoogleSheetsEnabled"), Common::MySettings.ReportToGoogleSheets);
  MyWebServer.setArgString(F("PushingBoxLogRelayID"), Common::MySettings.PushingBoxLogRelayID);

  MyWebServer.setArgBoolean(F("AlertEmails"), Common::MySettings.AlertEmails);
  MyWebServer.setArgString(F("PushingBoxEmailRelayID"), Common::MySettings.PushingBoxEmailRelayID);
  MyWebServer.setArgInt(F("ReadCountBeforeAlert"), Common::MySettings.ReadCountBeforeAlert);
  MyWebServer.setArgInt(F("TempAlertLow"), Common::MySettings.TempAlertLow);
  MyWebServer.setArgInt(F("TempAlertHigh"), Common::MySettings.TempAlertHigh);
  MyWebServer.setArgInt(F("HumidityAlertLow"), Common::MySettings.HumidityAlertLow);
  MyWebServer.setArgInt(F("HumidityAlertHigh"), Common::MySettings.HumidityAlertHigh);
  MyWebServer.setArgString(F("PressureAlertLow"), Common::toText(Common::MySettings.PressureAlertLow));
  MyWebServer.setArgString(F("PressureAlertHigh"), Common::toText(Common::MySettings.PressureAlertHigh));
  MyWebServer.setArgString(F("PHAlertLow"), Common::toText(Common::MySettings.PHAlertLow));
  MyWebServer.setArgString(F("PHAlertHigh"), Common::toText(Common::MySettings.PHAlertHigh));  

  MyWebServer.setArgString(F("PHCalibrationSlope"), Common::toPrecisionText(Common::MySettings.PHCalibrationSlope));
  MyWebServer.setArgString(F("PHCalibrationIntercept"), Common::toPrecisionText(Common::MySettings.PHCalibrationIntercept)); 
  MyWebServer.setArgString(F("PressureSensorOffset"), Common::toPrecisionText(Common::MySettings.PressureSensorOffset));
  MyWebServer.setArgString(F("PressureSensorRatio"), Common::toPrecisionText(Common::MySettings.PressureSensorRatio));  
  }
}

void refreshCallback(char * url) //called when website is refreshed
{ 
  MyWebServer.setArgString(F("Time"), Common::getFormattedTime()); 
    
  if (strcmp(url,"/GrowBox.html.json")==0){  
  MyWebServer.setArgString(F("tdLight1Status"),Light1 -> getStatusText());
  //MyWebServer.setArgString(F("tdLight1Reading"),toText(MySettings.LightBrightness, LightReading,"%-"));
  //MyWebServer.setArgString(F("tdBright"),yesNoToText());       
  MyWebServer.setArgString(F("tdTemp"),Common::toText(InternalDHTSensor -> getTemp(),ExternalDHTSensor -> getTemp()," / "));
  MyWebServer.setArgString(F("tdHumidity"),Common::toText(InternalDHTSensor -> getHumidity(),ExternalDHTSensor -> getHumidity()," / ")); 
  MyWebServer.setArgString(F("tdLight1TimerEnabled"),Light1 -> getTimerOnOffText());
  MyWebServer.setArgString(F("tdLight1On"), Light1 -> getOnTimeText());
  MyWebServer.setArgString(F("tdLight1Off"), Light1 -> getOffTimeText());  
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
  if(strcmp_P(field,(PGM_P)F("Brightness"))==0) {Light1 -> setBrightness(MyWebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("Light1OnHour"))==0) {Light1 -> setOnHour(MyWebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OnMinute"))==0) {Light1 -> setOnMinute(MyWebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffHour"))==0) {Light1 -> setOffHour(MyWebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1OffMinute"))==0) {Light1 -> setOffMinute(MyWebServer.getArgInt());} 
    
  Common::saveSettings(false);
} 
