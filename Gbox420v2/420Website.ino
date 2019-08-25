#include "420Helpers.h"
#include "GrowBox.h"
#include "DHTSensor.h"
#include "Lights.h"
#include "Sound.h"
#include "PowerSensor.h"
#include "LightSensor.h"
#include "Aeroponics_NoTank.h"
#include "Aeroponics_Tank.h"

void resetWebServer(void) {    // Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("WebServer (re)starting..."),false,0);
  while(!ESPLink.Sync())  {
    logToSerials(F("."),false,0);
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
  WebServer.setArgBoolean(F("AutoInternalFan"),GBox -> BoxSettings -> AutomaticInternalFan);
  WebServer.setArgString(F("InternalFanSwitchTemp"), toText(GBox -> BoxSettings -> InternalFanSwitchTemp));
  WebServer.setArgBoolean(F("AutoExhaustFan"), GBox -> BoxSettings -> AutomaticExhaustFan);
  WebServer.setArgString(F("ExhaustFanHighHumid"), toText(GBox -> BoxSettings -> ExhaustFanHighHumid));
  WebServer.setArgString(F("ExhaustFanLowHumid"), toText(GBox -> BoxSettings -> ExhaustFanLowHumid));
  WebServer.setArgString(F("ExhaustFanOffHumid"), toText(GBox -> BoxSettings -> ExhaustFanOffHumid));
    
  //Light1 load
  WebServer.setArgInt(F("Light1_OnHour"), *(GBox -> Light1 -> OnHour)); 
  WebServer.setArgInt(F("Light1_OnMinute"), *(GBox -> Light1 -> OnMinute)); 
  WebServer.setArgInt(F("Light1_OffHour"), *(GBox -> Light1 -> OffHour)); 
  WebServer.setArgInt(F("Light1_OffMinute"),*(GBox -> Light1 -> OffMinute));
  WebServer.setArgInt(F("Light1_Brightness"), *(GBox -> Light1 -> Brightness));
  WebServer.setArgInt(F("Light1_BrightnessSlider"), *(GBox -> Light1 -> Brightness));

  GBox -> Aeroponics_Tank1 -> websiteLoadEvent();
  GBox -> Aeroponics_NoTank1 -> websiteLoadEvent();
  // //Aeroponics_Tank1 load
  // WebServer.setArgInt(F("Aeroponics_Tank1_PumpTimeout"), *(GBox -> Aeroponics_Tank1 -> PumpTimeout));
  // WebServer.setArgInt(F("Aeroponics_Tank1_PrimingTime"), *(GBox -> Aeroponics_Tank1 -> PrimingTime));
  // WebServer.setArgInt(F("Aeroponics_Tank1_Interval"), *(GBox -> Aeroponics_Tank1 -> Interval));
  // WebServer.setArgInt(F("Aeroponics_Tank1_Duration"), *(GBox -> Aeroponics_Tank1 -> Duration));

  // //Aeroponics_NoTank1 load
  // WebServer.setArgInt(F("Aeroponics_NoTank1_PumpTimeout"), *(GBox -> Aeroponics_NoTank1 -> PumpTimeout));
  // WebServer.setArgInt(F("Aeroponics_NoTank1_PrimingTime"), *(GBox -> Aeroponics_NoTank1 -> PrimingTime));
  // WebServer.setArgInt(F("Aeroponics_NoTank1_Interval"), *(GBox -> Aeroponics_NoTank1 -> Interval));
  // WebServer.setArgInt(F("Aeroponics_NoTank1_Duration"), *(GBox -> Aeroponics_NoTank1 -> Duration)); 
   
  }
  
  if (strcmp(url,"/Settings.html.json")==0){  
  WebServer.setArgInt(F("SoundEnabled"), GBox -> BoxSettings -> Sound1Enabled);
  WebServer.setArgInt(F("DebugEnabled"), GBox -> BoxSettings -> DebugEnabled);
  WebServer.setArgInt(F("MetricSystemEnabled"), GBox -> BoxSettings -> MetricSystemEnabled);
  WebServer.setArgInt(F("DigitDisplayBrightness"), GBox -> BoxSettings -> DigitDisplayBacklight);
  WebServer.setArgInt(F("DigitDisplayValue"), GBox -> BoxSettings -> DigitDisplayValue);
  WebServer.setArgBoolean(F("MqttEnabled"), GBox -> BoxSettings -> ReportToMqtt);
  WebServer.setArgBoolean(F("GoogleSheetsEnabled"), GBox -> BoxSettings -> ReportToGoogleSheets);
  WebServer.setArgString(F("PushingBoxLogRelayID"), GBox -> BoxSettings -> PushingBoxLogRelayID);

  WebServer.setArgBoolean(F("AlertEmails"), GBox -> BoxSettings -> AlertEmails);
  //WebServer.setArgString(F("PushingBoxEmailRelayID"), GBox -> BoxSettings -> PushingBoxEmailRelayID);
  //WebServer.setArgInt(F("TriggerCountBeforeAlert"), GBox -> BoxSettings -> TriggerCountBeforeAlert);
  //WebServer.setArgInt(F("TempAlertLow"), GBox -> BoxSettings -> TempAlertLow);
  //WebServer.setArgInt(F("TempAlertHigh"), GBox -> BoxSettings -> TempAlertHigh);
  //WebServer.setArgInt(F("HumidityAlertLow"), GBox -> BoxSettings -> HumidityAlertLow);
  //WebServer.setArgInt(F("HumidityAlertHigh"), GBox -> BoxSettings -> HumidityAlertHigh);
  //WebServer.setArgString(F("PressureAlertLow"), toText(GBox -> BoxSettings -> PressureAlertLow));
  //WebServer.setArgString(F("PressureAlertHigh"), toText(GBox -> BoxSettings -> PressureAlertHigh));
  //WebServer.setArgString(F("PHAlertLow"), toText(GBox -> Reservoir -> PHAlertLow));
  //WebServer.setArgString(F("PHAlertHigh"), toText(GBox -> BoxSettings -> PHAlertHigh));  

//  WebServer.setArgString(F("PHCalibrationSlope"), toPrecisionText(GBox -> BoxSettings -> PHCalibrationSlope));
//  WebServer.setArgString(F("PHCalibrationIntercept"), toPrecisionText(GBox -> BoxSettings -> PHCalibrationIntercept)); 
//  WebServer.setArgString(F("PressureSensorOffset"), toPrecisionText(GBox -> BoxSettings -> PressureSensorOffset));
//  WebServer.setArgString(F("PressureSensorRatio"), toPrecisionText(GBox -> BoxSettings -> PressureSensorRatio));  
  }
}

void refreshCallback(char * url) //called when website is refreshed. Do not call logToSerials within any methods used here!
{ 
  WebServer.setArgString(F("Time"), getFormattedTime());
  WebServer.setArgJson(F("list_SerialLog"), GBox -> eventLogToJSON(false)); //Last events that happened in JSON format
  // WebServer.setArgString(F("td_LightOK"), statusToText(LightOK)); 
  // WebServer.setArgString(F("td_PressureOK"),statusToText(PressureOK));
  // WebServer.setArgString(F("td_PumpOK"),statusToText(PumpOK));
  // WebServer.setArgString(F("td_ACPowerOK"),statusToText(ACPowerOK));
  // WebServer.setArgString(F("td_DCPowerOK"),statusToText(DCPowerOK));
  // WebServer.setArgString(F("td_VentOK"),statusToText(VentOK));
  //WebServer.setArgString(F("td_ReservOK"),statusToText(ReservOK));
    
  if (strcmp(url,"/GrowBox.html.json")==0){       
  GBox -> InternalDHTSensor -> websiteRefreshEvent();
  GBox -> ExternalDHTSensor -> websiteRefreshEvent();
  GBox -> LightSensor1 -> websiteRefreshEvent();

 
  
  //Light1
  WebServer.setArgString(F("td_Light1_Status"),GBox -> Light1 -> getStatusText());
  WebServer.setArgString(F("td_Light1_TimerEnabled"),GBox -> Light1 -> getTimerOnOffText());
  WebServer.setArgString(F("td_Light1_On"), GBox -> Light1 -> getOnTimeText());
  WebServer.setArgString(F("td_Light1_Off"), GBox -> Light1 -> getOffTimeText());  

  //PowerSensor1
  WebServer.setArgString(F("td_Power"),GBox -> PowerSensor1 -> getPowerText());  
  WebServer.setArgString(F("td_Energy"),GBox -> PowerSensor1 -> getEnergyText());  
  WebServer.setArgString(F("td_Voltage"),GBox -> PowerSensor1 -> getVoltageText());  
  WebServer.setArgString(F("td_Current"),GBox -> PowerSensor1 -> getCurrentText());  
  WebServer.setArgString(F("td_Frequency"),GBox -> PowerSensor1 -> getFrequencyText());  
  WebServer.setArgString(F("td_PowerFactor"),GBox -> PowerSensor1 -> getPowerFactorText());  
  }
}

void buttonPressCallback(char *button)  //Called when any button on the website is pressed. Do not call logToSerials within any methods used here!
{
  //GrowBox page
  if (strcmp_P(button,(PGM_P)F("btn_Light1_On"))==0) {GBox -> Light1 -> setLightOnOff(true,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1_Off"))==0) {GBox -> Light1 -> setLightOnOff(false,true); }
  else if (strcmp_P(button,(PGM_P)F("btn_Light1_TimerEnable"))==0) {GBox -> Light1 -> setTimerOnOff(true);}
  else if (strcmp_P(button,(PGM_P)F("btn_Light1_TimerDisable"))==0) {GBox -> Light1 -> setTimerOnOff(false);}
  else if (strcmp_P(button,(PGM_P)F("btn_LightSensor1Calibrate"))==0) {GBox -> LightSensor1 -> triggerCalibration();}
  //Settings page
  else if (strcmp_P(button,(PGM_P)F("btn_restoreDefaults"))==0) { restoreDefaults(GBox -> BoxSettings); }
  else if (strcmp_P(button,(PGM_P)F("btn_Ee"))==0) { GBox -> Sound1 ->playEE(); }
  
  saveSettings(false,GBox -> BoxSettings); 
}

void setFieldCallback(char * field){  //Called when any field on the website is updated. Do not call logToSerials within any methods used here!
  if(strcmp_P(field,(PGM_P)F("Light1_Brightness"))==0) {GBox -> Light1 -> setBrightness(WebServer.getArgInt(),true);}
  else if(strcmp_P(field,(PGM_P)F("Light1_OnHour"))==0) {GBox -> Light1 -> setOnHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1_OnMinute"))==0) {GBox -> Light1 -> setOnMinute(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1_OffHour"))==0) {GBox -> Light1 -> setOffHour(WebServer.getArgInt());}
  else if(strcmp_P(field,(PGM_P)F("Light1_OffMinute"))==0) {GBox -> Light1 -> setOffMinute(WebServer.getArgInt());} 


  else if(strcmp_P(field,(PGM_P)F("SoundEnabled"))==0) {GBox -> Sound1 -> setSoundOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("DebugEnabled"))==0) {GBox -> setDebugOnOff(WebServer.getArgBoolean());}
  else if(strcmp_P(field,(PGM_P)F("MetricSystemEnabled"))==0) {GBox -> setMetricSystemEnabled(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("MqttEnabled"))==0) {setReportToMqttOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("GoogleSheetsEnabled"))==0) {setReportToGoogleSheetsOnOff(WebServer.getArgBoolean());}
  //else if(strcmp_P(field,(PGM_P)F("PushingBoxLogRelayID"))==0) {setPushingBoxLogRelayID(WebServer.getArgString());}
 
    
  saveSettings(false,GBox -> BoxSettings);
} 
