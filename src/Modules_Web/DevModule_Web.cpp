#include "DevModule_Web.h"
#include "../Components/Sound.h"
#include "../Components/DHTSensor.h"

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
*/
DevModule_Web::DevModule_Web(const __FlashStringHelper *Name, Settings::DevModule_WebSettings *DefaultSettings, RF24 *Wireless) : Module_Web(Name), Module(Name), Common_Web(Name), Common(Name)
{
  SerialReportFrequency = &DefaultSettings->SerialReportFrequency;
  SerialReportDate = &DefaultSettings->SerialReportDate;
  SerialReportMemory = &DefaultSettings->SerialReportMemory;
  SerialReportJSONFriendly = &DefaultSettings->SerialReportJSONFriendly;
  SerialReportJSON = &DefaultSettings->SerialReportJSON;
  SerialReportWireless = &DefaultSettings->SerialReportWireless;
  ReportToGoogleSheets = &DefaultSettings->ReportToGoogleSheets;
  SheetsReportingFrequency = &DefaultSettings->SheetsReportingFrequency;
  ReportToMQTT = &DefaultSettings->ReportToMQTT;
  MQTTReportFrequency = &DefaultSettings->MQTTReportFrequency;

  logToSerials(F(""), true, 0);                                   //<Line break
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///< Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this->SoundFeedback = Sound1;                                   ///< Pointer for child objects to use sound feedback
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  addToReportQueue(this);          //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  addToRefreshQueue_FiveSec(this); //< Attach to a trigger that fires every five seconds and calls refresh_FiveSec()
  addToRefreshQueue_Minute(this);  //< Attach to a trigger that fires every second and calls refresh_Minute()
  addToWebsiteQueue_Load(this);    //< Attach to the ESP-link website load event: Calls websiteEvent_Load() when an ESP-link webpage is opened
  addToWebsiteQueue_Refresh(this); //< Attach to the ESP-link website refresh event: Calls websiteEvent_Refresh() when an ESP-link webpage is refreshing
  addToCommandQueue(this);
  logToSerials(Name, false, 0);
  logToSerials(F("refreshing"), true, 1);
  runAll();
  addToLog(F("DevModule_Web initialized"), 0);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void DevModule_Web::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"M\":\""));
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void DevModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //WebServer.setArgInt(getName(F("FIS")), FanI->getSpeed()); ///< Internal PWM Fan speed
    //WebServer.setArgInt(getName(F("FES")), FanE->getSpeed()); ///< Exhaust PWM Fan speed
  }
  else if (strncmp(url, "/S", 2) == 0) //Settings tab
  {
    WebServer.setArgInt(getName(F("Debug")), *Debug);
    WebServer.setArgInt(getName(F("Metric")), *Metric);
    WebServer.setArgInt(getName(F("SerialF")), *SerialReportFrequency);
    WebServer.setArgInt(getName(F("Date")), *SerialReportDate);
    WebServer.setArgInt(getName(F("Mem")), *SerialReportMemory);
    WebServer.setArgInt(getName(F("JSON")), *SerialReportJSON);
    WebServer.setArgInt(getName(F("FJSON")), *SerialReportJSONFriendly);
    WebServer.setArgInt(getName(F("Wire")), *SerialReportWireless);
    WebServer.setArgBoolean(getName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getName(F("Relay")), ModuleSettings->PushingBoxLogRelayID);
    WebServer.setArgBoolean(getName(F("MQTT")), *ReportToMQTT);
    WebServer.setArgInt(getName(F("MQTTF")), *MQTTReportFrequency);
    WebServer.setArgString(getName(F("MPT")), ModuleSettings->MqttPubTopic);
    WebServer.setArgString(getName(F("MST")), ModuleSettings->MqttSubTopic);
    WebServer.setArgString(getName(F("MLT")), ModuleSettings->MqttLwtTopic);
    WebServer.setArgString(getName(F("MLM")), ModuleSettings->MqttLwtMessage);
  }
}

void DevModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  //All tabs
  WebServer.setArgString(getName(F("Time")), getFormattedTime(false));
  WebServer.setArgJson(getName(F("Log")), eventLogToJSON(false, true)); ///< Last events that happened in JSON format

  if (strncmp(url, "/G", 2) == 0) //GrowBox tab
  {
    //DHT1
    WebServer.setArgString(getName(F("DT")), DHT1->getTempText(true)); ///< Shows the latest reading
    WebServer.setArgString(getName(F("DH")), DHT1->getHumidityText(true));
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool DevModule_Web::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {   
    //Report triggers
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
    //Settings
    else if (strcmp_P(ShortMessage, (PGM_P)F("Debug")) == 0)
    {
      setDebug(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Metric")) == 0)
    {
      setMetric(toBool(Data));
    }
    //Settings - Serial reporting
    else if (strcmp_P(ShortMessage, (PGM_P)F("SerialF")) == 0)
    {
      setSerialReportingFrequency(toInt(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Date")) == 0)
    {
      setSerialReportDate(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mem")) == 0)
    {
      setSerialReportMemory(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("JSON")) == 0)
    {
      setSerialReportJSON(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("FJSON")) == 0)
    {
      setSerialReportJSONFriendly(toBool(Data));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Wire")) == 0)
    {
      setSerialReportWireless(toBool(Data));
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
    else
    {
      return false;
    }
    return true;
  }
}

void DevModule_Web::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  reportToSerialTrigger();
  reportToMQTTTrigger();
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

void DevModule_Web::refresh_Minute()
{
  Common::refresh_Minute();
  reportToGoogleSheetsTrigger();
}

///< Settings
void DevModule_Web::setDebug(bool DebugEnabled)
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

char *DevModule_Web::getDebugText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(*Debug);
  }
  else
  {
    return toText(*Debug);
  }
}

void DevModule_Web::setMetric(bool MetricEnabled)
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

char *DevModule_Web::getMetricText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(*Metric);
  }
  else
  {
    return toText(*Metric);
  }
}

///< Google Sheets reporting

void DevModule_Web::setSheetsReportingOnOff(bool State)
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

void DevModule_Web::setSheetsReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("Sheets freqency updated"));
  getSoundObject()->playOnSound();
}

void DevModule_Web::setPushingBoxLogRelayID(const char *ID)
{
  strncpy(ModuleSettings->PushingBoxLogRelayID, ID, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("Sheets log relay ID updated"));
}

void DevModule_Web::reportToGoogleSheetsTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for Google Sheets
  if ((*ReportToGoogleSheets && SheetsTriggerCounter++ % (*SheetsReportingFrequency) == 0) || ForceRun)
  {
    addPushingBoxLogRelayID();         //Loads Pushingbox relay ID into LongMessage
    runReport(false, true, true);      //Append the sensor readings in a JSON format to LongMessage buffer
    relayToGoogleSheets(&LongMessage); //Sends it to Google Sheets
  }
}
///< This is how a sent out message looks like:
///< {parameter={Log={"Report":{"InternalTemp":"20.84","ExternalTemp":"20.87","InternalHumidity":"38.54","ExternalHumidity":"41.87","InternalFan":"0","ExhaustFan":"0","Lt1_Status":"0","Lt1_Brightness":"15","LightReading":"454","Dark":"1","WaterLevel":"0","WaterTemp":"20.56","PH":"17.73","Pressure":"-0.18","Power":"-1.00","Energy":"-0.00","Voltage":"-1.00","Current":"-1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"},"Settings":{"Metric":"1"}}}, contextPath=, contentLength=499, queryString=, parameters={Log=[Ljava.lang.Object;@60efa46b}, postData=FileUpload}

void DevModule_Web::setMQTTReportingOnOff(bool State)
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

void DevModule_Web::setMQTTReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("MQTT freqency updated"));
  getSoundObject()->playOnSound();
}

void DevModule_Web::setMqttPublishTopic(const char *Topic)
{
  strncpy(ModuleSettings->MqttPubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT publish updated"));
}

void DevModule_Web::setMqttSubscribeTopic(const char *Topic)
{
  strncpy(ModuleSettings->MqttSubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT subscribe updated"));
}

void DevModule_Web::setMQTTLWTTopic(const char *LWTTopic)
{
  strncpy(ModuleSettings->MqttLwtTopic, LWTTopic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("LWT topic updated"));
}

void DevModule_Web::setMQTTLWTMessage(const char *LWTMessage)
{
  strncpy(ModuleSettings->MqttLwtMessage, LWTMessage, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("LWT message updated"));
}

void DevModule_Web::reportToMQTTTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for MQTT
  if ((*ReportToMQTT && MQTTTriggerCounter++ % (*MQTTReportFrequency / 5) == 0) || ForceRun)
  {
    runReport(true, true, true); //< Loads a JSON Log to LongMessage buffer
    mqttPublish(&LongMessage);   //< and publish readings via ESP MQTT API
    eventLogToJSON(true, true);  //< Loads the EventLog as a JSON with EventLog key
    mqttPublish(&LongMessage);   //< Load the event log in JSON format to LongMessage and publish the log via ESP MQTT API
  }
}
///< This is how the two sent out messages looks like:
///< Gbox420/{"Log":{"IFan":{"S":"1"},"EFan":{"S":"1"},"APump1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"92","B":"92","T":"1","On":"10:20","Of":"02:20"},"LtSen1":{"R":"955","D":"0"},"DHT1":{"T":"25.60","H":"45.20"},"Pow1":{"P":"569.30","E":"636.74","V":"227.50","C":"2.62","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"19.12","WR1":"6.23","DW1":"18.60","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"21.13","WR2":"3.65","DW2":"19.19","WW2":"21.19","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"6.12","W":"17.53","Mi":"5.00","Ma":"7.00","AS":"1","LS":"6.22","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"1.84","T":"1110.70","W":"24.37","WT":"18.56","AT":"26.20","H":"39.40"},"Main1":{"M":"1","D":"1"}}}
///< Gbox420/{"EventLog":["Event log entry 1","Event log entry 2","Event log entry 3","Event log entry 4"]}
