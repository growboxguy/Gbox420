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

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
MainModule::MainModule(const __FlashStringHelper *Name, Settings::MainModuleSettings *DefaultSettings, RF24 *Wireless) : Common_Web(Name), Common(Name), Module_Web(Wireless)
{
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  MQTTReportingFrequency = &DefaultSettings->MQTTReportingFrequency;
  ReportToMQTT = &DefaultSettings->ReportToMQTT;
  JSONtoSerialMode = &DefaultSettings->JSONtoSerialMode;
  RealTimeMode = &DefaultSettings->RealTimeMode;
  ///< Enable/disable sending JSON formatted reports to the Serial output
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                   ///< Pointer for child objects to use sound feedback
  IFan = new Fan(F("IFan"), this, &ModuleSettings->IFan);         ///< passing parameters: 1. Component name; 2. MainModule object the component belongs to; 3. Persistent settings stored in EEPROM)
  EFan = new Fan(F("EFan"), this, &ModuleSettings->EFan);
  //FanI = new Fan_PWM(F("FanI"), this, &ModuleSettings->FanI);
  //FanE = new Fan_PWM(F("FanE"), this, &ModuleSettings->FanE);
  APump1 = new AirPump(F("APump1"), this, &ModuleSettings->APump1);
  Lt1 = new Lights(F("Lt1"), this, &ModuleSettings->Lt1);
  Lt2 = new Lights(F("Lt2"), this, &ModuleSettings->Lt2);
  LtSen1 = new LightSensor(F("LtSen1"), this, &ModuleSettings->LtSen1, Lt1); ///< Passing an extra Light object as parameter: Calibrates the light sensor against the passed Light object
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  //Pow1 = new PowerSensor(F("Pow1"), this, &Serial2); ///< For PZEM004T V1.0 or PZEM004T V2.0
  Pow1 = new PowerSensorV3(F("Pow1"), this, &Serial2);                                         ///< Only for PZEM004T V3.0
  HempyModule1 = new HempyModule_Web(F("Hemp1"), this, &ModuleSettings->HempyModule1);         ///< Module used to relay Settings/MQTT/Website commands to the Hempy module and receive sensor readings
  AeroModule1 = new AeroModule_Web(F("Aero1"), this, &ModuleSettings->AeroModule1);            ///< Module used to relay Settings/MQTT/Website commands to the Aeroponics module and receive sensor readings
  ReservoirModule1 = new ReservoirModule_Web(F("Res1"), this, &ModuleSettings->ReservoirMod1); ///< Module used to relay Settings/MQTT/Website commands to the Reservoir module and receive sensor readings
  addToReportQueue(this);                                                                      //< Attach to the report event: When triggered the module reports to the Serial Console or the MQTT
  addToRefreshQueue_FiveSec(this);                                                             //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);                                                              //< Attach to a trigger that fires every second and calls refresh_Sec()
  addToWebsiteQueue_Load(this);                                                                //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this);                                                             //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);                                                                     //< Attach to MQTT/Website commands: Calls commandEvent() with the command + data strings
  logToSerials(F("MainModule object created, refreshing"), true, 0);
  runAll();
  addToLog(F("MainModule initialized"), 0);
}

void MainModule::report(bool JSONReport)
{
  if (JSONReport) //Caller requested a JSON formatted report: Append it to the LogMessage buffer. Caller is responsible of clearing the LongMessage buffer
  {
    Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
    strcat(LongMessage, toText(*Metric));
    strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
    strcat(LongMessage, toText(*Debug));
    strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
  }
  else //Print a report to the Serial console
  {
    Common::report();
    memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    strcat_P(LongMessage, (PGM_P)F("Debug:"));
    strcat(LongMessage, toText_enabledDisabled(Debug));
    strcat_P(LongMessage, (PGM_P)F(" ; Metric mode:"));
    strcat(LongMessage, toText_enabledDisabled(Metric));
    logToSerials(&LongMessage, true, 1);
  }
}

void MainModule::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //WebServer.setArgInt(getComponentName(F("FIS")), FanI->getSpeed()); ///< Internal PWM Fan speed
    //WebServer.setArgInt(getComponentName(F("FES")), FanE->getSpeed()); ///< Exhaust PWM Fan speed
    //Light1
    WebServer.setArgInt(getComponentName(F("L1OnH")), *(Lt1->OnHour));    ///< On hour
    WebServer.setArgInt(getComponentName(F("L1OnM")), *(Lt1->OnMinute));  ///< On minute
    WebServer.setArgInt(getComponentName(F("L1OfH")), *(Lt1->OffHour));   ///< Off hour
    WebServer.setArgInt(getComponentName(F("L1OfM")), *(Lt1->OffMinute)); ///< Off minute
    WebServer.setArgInt(getComponentName(F("L1B")), *(Lt1->Brightness));  ///< Brightness percentage
    //Light2
    WebServer.setArgInt(getComponentName(F("L2OnH")), *(Lt2->OnHour));    ///< On hour
    WebServer.setArgInt(getComponentName(F("L2OnM")), *(Lt2->OnMinute));  ///< On minute
    WebServer.setArgInt(getComponentName(F("L2OfH")), *(Lt2->OffHour));   ///< Off hour
    WebServer.setArgInt(getComponentName(F("L2OfM")), *(Lt2->OffMinute)); ///< Off minute
    WebServer.setArgInt(getComponentName(F("L2B")), *(Lt2->Brightness));  ///< Brightness percentage
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
    WebServer.setArgString(getComponentName(F("MPT")), ModuleSettings->MqttPubTopic);
    WebServer.setArgString(getComponentName(F("MST")), ModuleSettings->MqttSubTopic);
    WebServer.setArgString(getComponentName(F("MLT")), ModuleSettings->MqttLwtTopic);
    WebServer.setArgString(getComponentName(F("MLM")), ModuleSettings->MqttLwtMessage);
    WebServer.setArgBoolean(getComponentName(F("Sound")), Sound1->getEnabledState());
  }
}

void MainModule::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  //All tabs
  WebServer.setArgString(getComponentName(F("Time")), getFormattedTime(false));
  WebServer.setArgJson(getComponentName(F("Log")), eventLogToJSON(false, true)); ///< Last events that happened in JSON format

  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //Air pump
    WebServer.setArgString(getComponentName(F("AP")), APump1->getStateToText());
    //DHT1
    WebServer.setArgString(getComponentName(F("DT")), DHT1->getTempText(true)); ///< Shows the latest reading
    WebServer.setArgString(getComponentName(F("DH")), DHT1->getHumidityText(true));
    //PWM FAN
    //WebServer.setArgString(getComponentName(F("FIST")), getSpeedText(true,true));  ///PWM Fan speed text
    //WebServer.setArgString(getComponentName(F("FEST")), getSpeedText(true,true));  ///PWM Fan speed text
    //Internal Fan
    WebServer.setArgString(getComponentName(F("IFS")), IFan->fanSpeedToText());
    //Exhaust Fan
    WebServer.setArgString(getComponentName(F("EFS")), EFan->fanSpeedToText());
    //Light1
    WebServer.setArgString(getComponentName(F("L1S")), Lt1->getStateText());                  ///< State
    WebServer.setArgString(getComponentName(F("L1Br")), Lt1->getCurrentBrightnessText(true)); ///< Timer on or off
    WebServer.setArgString(getComponentName(F("L1T")), Lt1->getTimerOnOffText(true));         ///< Timer on or off
    //Light2
    WebServer.setArgString(getComponentName(F("L2S")), Lt2->getStateText());                  ///< State
    WebServer.setArgString(getComponentName(F("L2Br")), Lt2->getCurrentBrightnessText(true)); ///< Timer on or off
    WebServer.setArgString(getComponentName(F("L2T")), Lt2->getTimerOnOffText(true));         ///< Timer on or off
    //LightSensor1
    WebServer.setArgString(getComponentName(F("LSD")), LtSen1->getDarkText(true));
    WebServer.setArgString(getComponentName(F("LSR")), LtSen1->getReadingText(true));
    //PowerSensor
    WebServer.setArgString(getComponentName(F("PP")), Pow1->getPowerText(true));
    WebServer.setArgString(getComponentName(F("PE")), Pow1->getEnergyText(true));
    WebServer.setArgString(getComponentName(F("PV")), Pow1->getVoltageText(true));
    WebServer.setArgString(getComponentName(F("PC")), Pow1->getCurrentText(true));
    //PowerSensor V3
    //WebServer.setArgString(getComponentName(F("PF")), Pow1 -> getFrequencyText(true));
    //WebServer.setArgString(getComponentName(F("PPF")), Pow1 -> getPowerFactorText());
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
void MainModule::commandEvent(char *Command, char *Data)
{
  if (!isThisMyComponent(Command))
  {
    return;
  }
  else
  {
    //Air pump
    if (strcmp_P(ShortMessage, (PGM_P)F("AP")) == 0)
    {
      APump1->setState(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("APOn")) == 0)
    {
      APump1->setState(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("APOff")) == 0)
    {
      APump1->setState(false);
    }
    //Internal Fan
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFO")) == 0)
    {
      IFan->TurnOff();
      WebServer.setArgString(getComponentName(F("IFS")), IFan->fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFL")) == 0)
    {
      IFan->SetLowSpeed();
      WebServer.setArgString(getComponentName(F("IFS")), IFan->fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("IFH")) == 0)
    {
      IFan->SetHighSpeed();
      WebServer.setArgString(getComponentName(F("IFS")), IFan->fanSpeedToText());
    }
    //Exhaust Fan
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFO")) == 0)
    {
      EFan->TurnOff();
      WebServer.setArgString(getComponentName(F("EFS")), EFan->fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFL")) == 0)
    {
      EFan->SetLowSpeed();
      WebServer.setArgString(getComponentName(F("EFS")), EFan->fanSpeedToText());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("EFH")) == 0)
    {
      EFan->SetHighSpeed();
      WebServer.setArgString(getComponentName(F("EFS")), EFan->fanSpeedToText());
    }
    //Light1
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1")) == 0)
    {
      Lt1->setLightOnOff(toBool(Data), true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1On")) == 0)
    {
      Lt1->setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1Of")) == 0)
    {
      Lt1->setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1T")) == 0)
    {
      Lt1->setTimerOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1TOn")) == 0)
    {
      Lt1->setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1TOff")) == 0)
    {
      Lt1->setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1D")) == 0)
    {
      Lt1->dimLightsOnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1B")) == 0)
    {
      Lt1->setBrightness(toInt(Data), false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnT")) == 0)
    {
      Lt1->setOnTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnH")) == 0)
    {
      Lt1->setOnHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OnM")) == 0)
    {
      Lt1->setOnMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfT")) == 0)
    {
      Lt1->setOffTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfH")) == 0)
    {
      Lt1->setOffHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1OfM")) == 0)
    {
      Lt1->setOffMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L1DD")) == 0)
    {
      Lt1->setDimDuration(toInt(Data));
    }
    //Light2
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2")) == 0)
    {
      Lt2->setLightOnOff(toBool(Data), true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2On")) == 0)
    {
      Lt2->setLightOnOff(true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2Of")) == 0)
    {
      Lt2->setLightOnOff(false, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2T")) == 0)
    {
      Lt2->setTimerOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2TOn")) == 0)
    {
      Lt2->setTimerOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2TOff")) == 0)
    {
      Lt2->setTimerOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2D")) == 0)
    {
      Lt2->dimLightsOnOff();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2B")) == 0)
    {
      Lt2->setBrightness(toInt(Data), true, true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnT")) == 0)
    {
      Lt2->setOnTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnH")) == 0)
    {
      Lt2->setOnHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OnM")) == 0)
    {
      Lt2->setOnMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfT")) == 0)
    {
      Lt2->setOffTime(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfH")) == 0)
    {
      Lt2->setOffHour(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2OfM")) == 0)
    {
      Lt2->setOffMinute(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("L2DD")) == 0)
    {
      Lt2->setDimDuration(toInt(Data));
    }
    //Sound1
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sound")) == 0)
    {
      Sound1->setSoundOnOff(toBool(Data));
    }
    //Report triggers
    else if (strcmp_P(ShortMessage, (PGM_P)F("SheetsRep")) == 0)
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
    //Settings
    else if (strcmp_P(ShortMessage, (PGM_P)F("Debug")) == 0)
    {
      setDebug(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Metric")) == 0)
    {
      setMetric(toBool(Data));
    }
    //Settings - Google Sheets
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sheets")) == 0)
    {
      setSheetsReportingOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SheetsF")) == 0)
    {
      setSheetsReportingFrequency(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Relay")) == 0)
    {
      setPushingBoxLogRelayID(WebServer.getArgString());
    }
    //Settings - MQTT
    else if (strcmp_P(ShortMessage, (PGM_P)F("MQTT")) == 0)
    {
      setMQTTReportingOnOff(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MQTTF")) == 0)
    {
      setMQTTReportingFrequency(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MPT")) == 0)
    {
      setMqttPublishTopic(WebServer.getArgString());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MST")) == 0)
    {
      setMqttSubscribeTopic(WebServer.getArgString());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MLT")) == 0)
    {
      setMQTTLWTTopic(WebServer.getArgString());
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("MLM")) == 0)
    {
      setMQTTLWTMessage(WebServer.getArgString());
    }
  }
}

void MainModule::refresh_FiveSec()
{
  if (*Debug)
  {
    Common::refresh_FiveSec();
    runReport(*JSONtoSerialMode, true, true);
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
    runReport(*JSONtoSerialMode, true, true);
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
  runReport(*JSONtoSerialMode, true, true);
  reportToGoogleSheetsTrigger();
  reportToMQTTTrigger();
}

bool MainModule::getDayMode()
{
  if (Lt1->getStatus() || Lt2->getStatus() || !(LtSen1->getDark()))
  {
    return true; ///< Return true if any of the lights are on OR the light sensor is detecting light
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
  if ((*ReportToGoogleSheets && SheetsRefreshCounter++ % (*SheetsReportingFrequency) == 0) || ForceRun)
  {
    addPushingBoxLogRelayID();         //Loads Pushingbox relay ID into LongMessage
    runReport(true, false, true);      //Append the sensor readings in a JSON format to LongMessage buffer
    relayToGoogleSheets(&LongMessage); //Sends it to Google Sheets
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

void MainModule::setMqttPublishTopic(const char *Topic)
{
  strncpy(ModuleSettings->MqttPubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT publish updated"));
}

void MainModule::setMqttSubscribeTopic(const char *Topic)
{
  strncpy(ModuleSettings->MqttSubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT subscribe updated"));
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
  if ((*ReportToMQTT && MQTTRefreshCounter++ % (*MQTTReportingFrequency) == 0) || ForceRun)
  {
    runReport(true, true, true);       //< Loads a JSON Log to LongMessage buffer
    mqttPublish(&LongMessage);   //  and publish readings via ESP MQTT API
    eventLogToJSON(true, true); //<Loads the EventLog as a JSON with EventLog key
    mqttPublish(&LongMessage);   //Load the event log in JSON format to LongMessage and publish the log via ESP MQTT API
  }
}
///< This is how the two sent out messages looks like:
///< Gbox420/{"Log":{"IFan":{"S":"1"},"EFan":{"S":"1"},"APump1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"92","B":"92","T":"1","On":"10:20","Of":"02:20"},"LtSen1":{"R":"955","D":"0"},"DHT1":{"T":"25.60","H":"45.20"},"Pow1":{"P":"569.30","E":"636.74","V":"227.50","C":"2.62","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"19.12","WR1":"6.23","DW1":"18.60","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"21.13","WR2":"3.65","DW2":"19.19","WW2":"21.19","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"6.12","W":"17.53","Mi":"5.00","Ma":"7.00","AS":"1","LS":"6.22","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"1.84","T":"1110.70","W":"24.37","WT":"18.56","AT":"26.20","H":"39.40"},"Main1":{"M":"1","D":"1"}}}
///< Gbox420/{"EventLog":["Event log entry 1","Event log entry 2","Event log entry 3","Event log entry 4"]}
