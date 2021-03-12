#include "MainModule_Web.h"
#include "HempyModule_Web.h"
#include "AeroModule_Web.h"
#include "ReservoirModule_Web.h"
#include "../Components/Sound.h"
#include "../Components/DHTSensor.h"
#include "../Components/Lights.h"
#include "../Components/LightSensor.h"
#include "../Components/Fan.h"
#include "../Components/Fan_PWM.h"
#include "../Components/AirPump.h"
//#include "../Components/PowerSensor.h"  ///< For PZEM004T V1.0 or PZEM004T V2.0
#include "../Components/PowerSensorV3.h" ///< Only for PZEM004T V3.0

MainModule::MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless) : Common_Web(Name), Module_Web(Wireless)
{ ///< Constructor
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  MQTTReportingFrequency = &DefaultSettings->MQTTReportingFrequency;
  ReportToMQTT = &DefaultSettings->ReportToMQTT;
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;
  IFan = new Fan(F("IFan"), this, &ModuleSettings->IFan); ///< passing: Component name, MainModule object the component belongs to, Default settings)
  EFan = new Fan(F("EFan"), this, &ModuleSettings->EFan);
  //FanI = new Fan_PWM(F("FanI"), this, &ModuleSettings->FanI);
  //FanE = new Fan_PWM(F("FanE"), this, &ModuleSettings->FanE);
  APump1 = new AirPump(F("APump1"), this, &ModuleSettings->APump1);
  Lt1 = new Lights(F("Lt1"), this, &ModuleSettings->Lt1);
  Lt2 = new Lights(F("Lt2"), this, &ModuleSettings->Lt2);
  LtSen1 = new LightSensor(F("LtSen1"), this, &ModuleSettings->LtSen1, Lt1); ///< Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  //Pow1 = new PowerSensor(F("Pow1"), this, &Serial2); ///< For PZEM004T V1.0 or PZEM004T V2.0
  Pow1 = new PowerSensorV3(F("Pow1"), this, &Serial2); ///< Only for PZEM004T V3.0
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this, &ModuleSettings->HempyModule1);
  AeroModule1 = new AeroModule_Web(F("Aero1"), this, &ModuleSettings->AeroModule1);
  ReservoirModule1 = new ReservoirModule_Web(F("Res1"), this, &ModuleSettings->ReservoirMod1);
  addToReportQueue(this);
  addToRefreshQueue_FiveSec(this);
  addToRefreshQueue_Minute(this);
  addToWebsiteQueue_Load(this);
  addToWebsiteQueue_Refresh(this);
  addToWebsiteQueue_Field(this);
  addToWebsiteQueue_Button(this);
  logToSerials(F("MainModule object created, refreshing"), true, 0);
  runAll();
  addToLog(F("MainModule initialized"), 0);
}

void MainModule::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Debug:"));
  strcat(LongMessage, toText_enabledDisabled(Debug));
  strcat_P(LongMessage, (PGM_P)F(" ; Metric mode:"));
  strcat(LongMessage, toText_enabledDisabled(Metric));
  logToSerials(&LongMessage, true, 1);
}

void MainModule::reportToJSON()
{
  Common::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
  strcat(LongMessage, toText(*Metric));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, toText(*Debug));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket
}

void MainModule::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //WebServer.setArgInt(getComponentName(F("FIS")), FanI->getSpeed()); ///< Internal PWM Fan speed
    //WebServer.setArgInt(getComponentName(F("FES")), FanE->getSpeed()); ///< Exhaust PWM Fan speed
    //Light1
    WebServer.setArgInt(getComponentName(F("L1OnH")), *(Lt1 -> OnHour));            ///< On hour
    WebServer.setArgInt(getComponentName(F("L1OnM")), *(Lt1 -> OnMinute));          ///< On minute
    WebServer.setArgInt(getComponentName(F("L1OfH")), *(Lt1 -> OffHour));           ///< Off hour
    WebServer.setArgInt(getComponentName(F("L1OfM")), *(Lt1 -> OffMinute));         ///< Off minute
    WebServer.setArgInt(getComponentName(F("L1B")), *(Lt1 -> Brightness));          ///< Brightness percentage
    //Light2
    WebServer.setArgInt(getComponentName(F("L2OnH")), *(Lt2 -> OnHour));            ///< On hour
    WebServer.setArgInt(getComponentName(F("L2OnM")), *(Lt2 -> OnMinute));          ///< On minute
    WebServer.setArgInt(getComponentName(F("L2OfH")), *(Lt2 -> OffHour));           ///< Off hour
    WebServer.setArgInt(getComponentName(F("L2OfM")), *(Lt2 -> OffMinute));         ///< Off minute
    WebServer.setArgInt(getComponentName(F("L2B")), *(Lt2 -> Brightness));          ///< Brightness percentage
  }
  else if (strncmp(url, "/S", 2) == 0) //Settings tab
  {
    WebServer.setArgInt(getComponentName(F("Debug")), *Debug);
    WebServer.setArgInt(getComponentName(F("Metric")), *Metric);
    WebServer.setArgBoolean(getComponentName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getComponentName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getComponentName(F("Relay")), ModuleSettings->PushingBoxLogRelayID);
    WebServer.setArgBoolean(getComponentName(F("MQTT")), *ReportToMQTT);
    WebServer.setArgInt(getComponentName(F("MQTTF")), *MQTTReportingFrequency);
    WebServer.setArgString(getComponentName(F("MT")), ModuleSettings->MqttTopic);
    WebServer.setArgString(getComponentName(F("MLT")), ModuleSettings->MqttLwtTopic);
    WebServer.setArgString(getComponentName(F("MLM")), ModuleSettings->MqttLwtMessage);
    WebServer.setArgBoolean(getComponentName(F("Sound")), Sound1 -> getEnabledState());
  }
}

void MainModule::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  //All tabs
  WebServer.setArgString(getComponentName(F("Time")), getFormattedTime(false));
  WebServer.setArgJson(getComponentName(F("Log")), eventLogToJSON()); ///< Last events that happened in JSON format

  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    WebServer.setArgString(getComponentName(F("AP")), APump1->getStateToText());
    WebServer.setArgString(getComponentName(F("DT")), DHT1->getTempText(true)); ///< Shows the latest reading
    WebServer.setArgString(getComponentName(F("DH")), DHT1->getHumidityText(true));
    //WebServer.setArgString(getComponentName(F("FIST")), getSpeedText(true,true));  ///PWM Fan speed text
    //WebServer.setArgString(getComponentName(F("FEST")), getSpeedText(true,true));  ///PWM Fan speed text
    WebServer.setArgString(getComponentName(F("IFS")), IFan -> fanSpeedToText());
    WebServer.setArgString(getComponentName(F("EFS")), EFan -> fanSpeedToText());
    //Light1
    WebServer.setArgString(getComponentName(F("L1S")), Lt1 -> getStateText());                  ///< State
    WebServer.setArgString(getComponentName(F("L1Br")), Lt1 -> getCurrentBrightnessText(true)); ///< Timer on or off
    WebServer.setArgString(getComponentName(F("L1T")), Lt1 -> getTimerOnOffText(true));         ///< Timer on or off
     //Light2
    WebServer.setArgString(getComponentName(F("L2S")), Lt2 -> getStateText());                  ///< State
    WebServer.setArgString(getComponentName(F("L2Br")), Lt2 -> getCurrentBrightnessText(true)); ///< Timer on or off
    WebServer.setArgString(getComponentName(F("L2T")), Lt2 -> getTimerOnOffText(true));         ///< Timer on or off
    //LightSensor1
    WebServer.setArgString(getComponentName(F("LSD")), LtSen1 -> getDarkText(true));
    WebServer.setArgString(getComponentName(F("LSR")), LtSen1 -> getReadingText(true));
    //PowerSensor
    WebServer.setArgString(getComponentName(F("PP")), Pow1 -> getPowerText(true));
    WebServer.setArgString(getComponentName(F("PE")), Pow1 -> getEnergyText(true));
    WebServer.setArgString(getComponentName(F("PV")), Pow1 -> getVoltageText(true));
    WebServer.setArgString(getComponentName(F("PC")), Pow1 -> getCurrentText(true));
    //PowerSensor V3 
    //WebServer.setArgString(getComponentName(F("PF")), Pow1 -> getFrequencyText(true));
    //WebServer.setArgString(getComponentName(F("PPF")), Pow1 -> getPowerFactorText());
  }
}

void MainModule::websiteEvent_Button(char *Button)
{ ///< When a button is pressed on the website
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SheetsRep")) == 0)
    {
      ReportToGoogleSheetsRequested = true; ///< just signal that a report should be sent, do not actually run it: Takes too long from an interrupt
      addToLog(F("Reporting to Sheets"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SerialRep")) == 0)
    {
      ConsoleReportRequested = true;
      addToLog(F("Reporting to Serial"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MQTTRep")) == 0)
    {
      MQTTReportRequested = true;
      addToLog(F("Reporting to MQTT"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refresh")) == 0) ///< Website signals to refresh all sensor readings
    {
      RefreshAllRequested = true;
      addToLog(F("Refresh triggered"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("APOn")) == 0)
    {
      APump1->TurnOn();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("APOff")) == 0)
    {
      APump1->TurnOff();
    }
    /*
    else if (strcmp_P(ShortMessage, (PGM_P)F("FIOf")) == 0)
    {
      FanI -> turnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("FIOn")) == 0)
    {
     FanI -> turnOn();
    }  
    else if (strcmp_P(ShortMessage, (PGM_P)F("FEOf")) == 0)
    {
      FanE -> turnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("FEOn")) == 0)
    {
     FanE -> turnOn();
    }  
    */
    //Internal Fan
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFO")) == 0)
    {
      IFan -> TurnOff();
      WebServer.setArgString(getComponentName(F("IFS")), IFan -> fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFL")) == 0)
    {
      IFan -> SetLowSpeed();
      WebServer.setArgString(getComponentName(F("IFS")), IFan -> fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFH")) == 0)
    {
      IFan -> SetHighSpeed();
      WebServer.setArgString(getComponentName(F("IFS")), IFan -> fanSpeedToText());
    }
    //Exhaust Fan
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFO")) == 0)
    {
      EFan -> TurnOff();
      WebServer.setArgString(getComponentName(F("EFS")), EFan ->fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFL")) == 0)
    {
      EFan -> SetLowSpeed();
      WebServer.setArgString(getComponentName(F("EFS")), EFan ->fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFH")) == 0)
    {
      EFan -> SetHighSpeed();
      WebServer.setArgString(getComponentName(F("EFS")), EFan ->fanSpeedToText());
    }
    //Light1
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1On")) == 0)
    {
      Lt1 -> setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1Of")) == 0)
    {
      Lt1 -> setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1TOn")) == 0)
    {
      Lt1 -> setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1TOff")) == 0)
    {
      Lt1 -> setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1D")) == 0)
    {
      Lt1 -> dimLightsOnOff();
    }
     //Light2
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2On")) == 0)
    {
      Lt2 -> setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2Of")) == 0)
    {
      Lt2 -> setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2TOn")) == 0)
    {
      Lt2 -> setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2TOff")) == 0)
    {
      Lt2 -> setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2D")) == 0)
    {
      Lt2 -> dimLightsOnOff();
    }
  }
}

void MainModule::websiteEvent_Field(char *Field)
{ ///< When the website field is submitted
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("Debug")) == 0)
    {
      setDebug(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Metric")) == 0)
    {
      setMetric(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sheets")) == 0)
    {
      setSheetsReportingOnOff(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SheetsF")) == 0)
    {
      setSheetsReportingFrequency(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Relay")) == 0)
    {
      setPushingBoxLogRelayID(WebServer.getArgString());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MQTT")) == 0)
    {
      setMQTTReportingOnOff(WebServer.getArgBoolean());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MQTTF")) == 0)
    {
      setMQTTReportingFrequency(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MT")) == 0)
    {
      setMQTTTopic(WebServer.getArgString());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MLT")) == 0)
    {
      setMQTTLWTTopic(WebServer.getArgString());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MLM")) == 0)
    {
      setMQTTLWTMessage(WebServer.getArgString());
    }
    /*
    else if (strcmp_P(ShortMessage, (PGM_P)F("FIS")) == 0)
    {
      FanI - > setSpeed(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("FES")) == 0)
    {
      FanE - > setSpeed(WebServer.getArgInt());
    }
    */
   //Light1
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1B")) == 0)
    {
      Lt1 -> setBrightness(WebServer.getArgInt(), true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnH")) == 0)
    {
      Lt1 -> setOnHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnM")) == 0)
    {
      Lt1 -> setOnMinute(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfH")) == 0)
    {
      Lt1 -> setOffHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfM")) == 0)
    {
      Lt1 -> setOffMinute(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1DD")) == 0)
    {
      Lt1 -> setDimDuration(WebServer.getArgInt());
    }
    //Light2
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2B")) == 0)
    {
      Lt2 -> setBrightness(WebServer.getArgInt(), true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnH")) == 0)
    {
      Lt2 -> setOnHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnM")) == 0)
    {
      Lt2 -> setOnMinute(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfH")) == 0)
    {
      Lt2 -> setOffHour(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfM")) == 0)
    {
      Lt2 -> setOffMinute(WebServer.getArgInt());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2DD")) == 0)
    {
      Lt2 -> setDimDuration(WebServer.getArgInt());
    }
    //Sound1
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sound")) == 0)
    {
     Sound1 -> setSoundOnOff(WebServer.getArgBoolean());
    }
  }
}

void MainModule::refresh_FiveSec()
{
  if (*Debug)
  {
    Common::refresh_FiveSec();
    runReport();
  }
  if (RefreshAllRequested)
  {
    RefreshAllRequested = false;
    runAll();
  }
  if (ReportToGoogleSheetsRequested)
  {
    ReportToGoogleSheetsRequested = false;
    reportToGoogleSheetsTrigger(true);
  }
  if (ConsoleReportRequested)
  {
    ConsoleReportRequested = false;
    runReport();
  }
  if (MQTTReportRequested)
  {
    MQTTReportRequested = false;
    reportToMQTTTrigger(true);
  }
}

void MainModule::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  runReport();
  reportToGoogleSheetsTrigger();
  reportToMQTTTrigger();
}

bool MainModule::getDayMode()
{
  if (Lt1->getStatus() || Lt2->getStatus() || !(LtSen1->getDark()))
  {
    return true; ///< Return true if any of the lights are on OR the light sensor is detecting daylight
  }
  else
  {
    return false;
  }
}

///< Settings
void MainModule::setDebug(bool DebugEnabled)
{
  *Debug = DebugEnabled;
  if (*Debug)
  {
    addToLog(F("Debug enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("Debug disabled"));
    getSoundObject()->playOffSound();
  }
}

void MainModule::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { ///< if there was a change
    *Metric = MetricEnabled;
    RefreshAllRequested = true;
  }
  if (*Metric)
    addToLog(F("Using Metric units"));
  else
    addToLog(F("Using Imperial units"));
  getSoundObject()->playOnSound();
}

///< Google Sheets reporting

void MainModule::setSheetsReportingOnOff(bool State)
{
  *ReportToGoogleSheets = State;
  if (State)
  {
    addToLog(F("Google Sheets enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("Google Sheets disabled"));
    getSoundObject()->playOffSound();
  }
}

void MainModule::setSheetsReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("Sheets freqency updated"));
  getSoundObject()->playOnSound();
}

void MainModule::setPushingBoxLogRelayID(const char *ID)
{
  strncpy(ModuleSettings->PushingBoxLogRelayID, ID, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("Sheets log relay ID updated"));
}

void MainModule::reportToGoogleSheetsTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for Google Sheets
  if (SheetsRefreshCounter++ % (*SheetsReportingFrequency) == 0 || ForceRun)
  {
    addPushingBoxLogRelayID(); //Loads Pushingbox relay ID into LongMessage
    getJSONReport(false);      //Adds the JSON report to LongMessage
    relayToGoogleSheets(&LongMessage);
  }
}
///< This is how a sent out message looks like:
///< {parameter={Log={"Report":{"InternalTemp":"20.84","ExternalTemp":"20.87","InternalHumidity":"38.54","ExternalHumidity":"41.87","InternalFan":"0","ExhaustFan":"0","Lt1_Status":"0","Lt1_Brightness":"15","LightReading":"454","Dark":"1","WaterLevel":"0","WaterTemp":"20.56","PH":"17.73","Pressure":"-0.18","Power":"-1.00","Energy":"-0.00","Voltage":"-1.00","Current":"-1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"},"Settings":{"Metric":"1"}}}, contextPath=, contentLength=499, queryString=, parameters={Log=[Ljava.lang.Object;@60efa46b}, postData=FileUpload}

void MainModule::setMQTTReportingOnOff(bool State)
{
  *ReportToMQTT = State;
  if (State)
  {
    addToLog(F("MQTT enabled"));
    getSoundObject()->playOnSound();
  }
  else
  {
    addToLog(F("MQTT disabled"));
    getSoundObject()->playOffSound();
  }
}

void MainModule::setMQTTReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("MQTT freqency updated"));
  getSoundObject()->playOnSound();
}

void MainModule::setMQTTTopic(const char *RootTopic)
{
  strncpy(ModuleSettings->MqttTopic, RootTopic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT topic updated"));
}

void MainModule::setMQTTLWTTopic(const char *LWTTopic)
{
  strncpy(ModuleSettings->MqttLwtTopic, LWTTopic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("LWT topic updated"));
}

void MainModule::setMQTTLWTMessage(const char *LWTMessage)
{
  strncpy(ModuleSettings->MqttLwtMessage, LWTMessage, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("LWT message updated"));
}

void MainModule::reportToMQTTTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for MQTT
  if (MQTTRefreshCounter++ % (*MQTTReportingFrequency) == 0 || ForceRun)
  {
    getJSONReport(true);       //Load the JSON report to LongMessage
    mqttPublish(&LongMessage); //publish readings via ESP MQTT API
  }
}
///< This is how a sent out message looks like:
///< /growboxguy@gmail.com/Gbox420/{"Log":{"IFan":{"S":"1"},"EFan":{"S":"0"},"APump1":{"S":"1"},"Lt1":{"S":"1","B":"75","T":"1","On":"08:20","Of":"02:20"},"Lt2":{"S":"0","B":"0","T":"0","On":"08:20","Of":"16:20"},"LtSen1":{"R":"967","D":"0"},"DHT1":{"T":"25.10","H":"43.10"},"Pow1":{"P":"34.40","E":"510.93","V":"231.90","C":"0.34","F":"50.00","PF":"0.44"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","WB1":"18.29","WR1":"3.96","DW1":"18.10","WW1":"0.00","ET1":"2.00","OT1":"0.30","WL1":"13.00","H2":"1","P2":"1","PS2":"100","WB2":"20.17","WR2":"12.21","DW2":"18.49","WW2":"20.49","ET2":"2.00","OT2":"0.30","WL2":"13.00"},"Aero1":{"S":"1","P":"6.43","W":"19.68","Mi":"5.00","Ma":"7.00","AS":"1","LS":"6.47","PSt":"1","PS":"100","SE":"1","D":"1.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"1.85","T":"1126.00","W":"24.16","WT":"17.06","AT":"24.70","H":"27.60"},"Main1":{"M":"1","D":"1"}}}