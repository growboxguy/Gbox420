#include "420Module_Web.h"
#include "../Components/Sound.h"

static char Logs[LogDepth][MaxWordLength]; ///< two dimensional array for storing log histroy displayed on the website (array of char arrays)

/**
* @brief Constructor: creates an instance of the class, and stores wireless transmitter object used to communicate with other modules
*/
Module_Web::Module_Web(const __FlashStringHelper *Name) : Common(Name), Module()
{  
  //logToSerials(F("Module_Web ready"), true, 3);
}

/**
* @brief Subscribe to the ESP-link website load event
*/
void Module_Web::addToWebsiteQueue_Load(Module_Web *Subscriber)
{
  if (QueueDepth > WebsiteQueue_Load_Count)
    WebsiteQueue_Load[WebsiteQueue_Load_Count++] = Subscriber;
  else
    logToSerials(F("WebsiteQueue_Load overflow!"), true, 0);
}

/**
* @brief Subscribe to the ESP-link website refresh event
*/
void Module_Web::addToWebsiteQueue_Refresh(Module_Web *Subscriber)
{
  if (QueueDepth > WebsiteQueue_Refresh_Count)
    WebsiteQueue_Refresh[WebsiteQueue_Refresh_Count++] = Subscriber;
  else
    logToSerials(F("WebsiteQueue_Refresh overflow!"), true, 0);
}

/**
* @brief Subscribe to MQTT or ESP-link website commands  (Pressing a button, setting a value)
*/
void Module_Web::addToCommandQueue(Module_Web *Subscriber)
{
  if (QueueDepth > CommandQueue_Count)
    CommandQueue[CommandQueue_Count++] = Subscriber;
  else
    logToSerials(F("CommandQueue overflow!"), true, 0);
}

/**
* @brief Notify subscribed modules of a website load event
*/
void Module_Web::websiteLoadEventTrigger(char *url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueue_Load_Count; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(url);
  }
}

/**
* @brief Notify subscribed modules of a website refresh event
*/
void Module_Web::websiteRefreshEventTrigger(char *url)
{ ///< called when website is refreshed.
  for (int i = 0; i < WebsiteQueue_Refresh_Count; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(url);
  }
}

/**
* @brief Notify subscribed modules of a received MQTT/Website command
*/
void Module_Web::commandEventTrigger(char *command, char *data)
{
  logToSerials(&command, false, 1);
  logToSerials(&data, true, 2);
  for (int i = 0; i < CommandQueue_Count; i++)
  {
    CommandQueue[i]->commandEvent(command, data);
  }
}

/**
* @brief Adds a log entry to the top of the log and removes the oldest log entry
*/
void Module_Web::addToLog(const char *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///< adds a log entry that is displayed on the web interface
  logToSerials(&LongMessage, true, Indent);
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///< Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///< clear variable
    strncpy(Logs[i], Logs[i - 1], MaxWordLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));         ///< clear variable
  strncpy(Logs[0], LongMessage, MaxWordLength); ///< instert new log to [0]
}

/**
* @brief Adds a log entry to the top of the log and removes the oldest log entry
*/
void Module_Web::addToLog(const __FlashStringHelper *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///< function overloading: same function name, different parameter type
  logToSerials(&LongMessage, true, Indent);
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///< Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///< clear variable
    strncpy(Logs[i], Logs[i - 1], MaxWordLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));                  ///< clear variable
  strncpy_P(Logs[0], (PGM_P)LongMessage, MaxWordLength); ///< instert new log to [0]
}

/**
* @brief Converts the log entries to a JSON object
* IncludeKey false: ["Log1","Log2","Log3",...,"LogN"], 
* IncludeKey true:  {"EventLog"\["Log1","Log2","Log3",...,"LogN"]}
* Append false: Clear the LongMessage buffer before
*/
char *Module_Web::eventLogToJSON(bool IncludeKey, bool ClearBuffer)
{
  if (ClearBuffer)
  {
    memset(&LongMessage[0], 0, MaxLongTextLength);
  }
  if (IncludeKey)
  {
    strcat_P(LongMessage, (PGM_P)F("{\"EventLog\":")); ///< Adds a curly bracket that needs to be closed at the end
  }
  strcat_P(LongMessage, (PGM_P)F("["));
  for (int i = LogDepth - 1; i >= 0; i--)
  {
    strcat_P(LongMessage, (PGM_P)F("\""));
    strcat(LongMessage, Logs[i]);
    strcat_P(LongMessage, (PGM_P)F("\""));
    if (i > 0)
      strcat_P(LongMessage, (PGM_P)F(","));
  }
  LongMessage[strlen(LongMessage)] = ']';
  if (IncludeKey)
  {
    strcat_P(LongMessage, (PGM_P)F("}")); ///< closing curly bracket
  }
  return LongMessage;
}

///< ESP-link web interface functions
void Module_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/S", 2) == 0) //Settings tab
  {
    WebServer.setArgInt(getComponentName(F("Debug")), *Debug);
    WebServer.setArgInt(getComponentName(F("Metric")), *Metric);
    WebServer.setArgInt(getComponentName(F("SerialF")), *SerialReportFrequency);
    WebServer.setArgInt(getComponentName(F("Date")), *SerialReportDate);
    WebServer.setArgInt(getComponentName(F("Mem")), *SerialReportMemory);
    WebServer.setArgInt(getComponentName(F("JSON")), *SerialReportJSON);
    WebServer.setArgInt(getComponentName(F("FJSON")), *SerialReportJSONFriendly);
    WebServer.setArgInt(getComponentName(F("Wire")), *SerialReportWireless);
    WebServer.setArgBoolean(getComponentName(F("Sheets")), *ReportToGoogleSheets);
    WebServer.setArgInt(getComponentName(F("SheetsF")), *SheetsReportingFrequency);
    WebServer.setArgString(getComponentName(F("Relay")), ModuleSettings->PushingBoxLogRelayID);
    WebServer.setArgBoolean(getComponentName(F("MQTT")), *ReportToMQTT);
    WebServer.setArgInt(getComponentName(F("MQTTF")), *MQTTReportFrequency);
    WebServer.setArgString(getComponentName(F("MPT")), ModuleSettings->MqttPubTopic);
    WebServer.setArgString(getComponentName(F("MST")), ModuleSettings->MqttSubTopic);
    WebServer.setArgString(getComponentName(F("MLT")), ModuleSettings->MqttLwtTopic);
    WebServer.setArgString(getComponentName(F("MLM")), ModuleSettings->MqttLwtMessage);
    WebServer.setArgBoolean(getComponentName(F("Sound")), getSoundObject()->getEnabledState());
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
bool Module_Web::commandEvent(char *Command, char *Data)
{  
    //Sound1
    if (strcmp_P(ShortMessage, (PGM_P)F("Sound")) == 0)
    {
      getSoundObject()->setSoundOnOff(toBool(Data));
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
      return false;  //Nothing matched
    }
    return true;  //Match found   
}

///< Google Sheets functions - https://sites.google.com/site/growboxguy/esp-link/logging

/**
* @brief Loads the PushingBox device ID to the LongMessage buffer. Pushingbox is a relay service that is used to send reports to Google Sheets: https://sites.google.com/site/growboxguy/esp-link/logging
* Example
* /pushingbox?devid=v755877CF53383E1&BoxData=
*/
void Module_Web::addPushingBoxLogRelayID()
{
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("/pushingbox?devid="));
  strcat(LongMessage, ModuleSettings->PushingBoxLogRelayID);
  strcat_P(LongMessage, (PGM_P)F("&BoxData="));
}

/**
* @brief Send a JSON formatted report to Google Sheets
* Example
* REST API reporting: api.pushingbox.com/pushingbox?devid=v755877CF53383E1&BoxData={"Log":{"IFan":{"S":"1"},"EFan":{"S":"1"},"APump1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"LtSen1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"44.50"},"Pow1":{"P":"572.20","E":"665.26","V":"228.00","C":"2.62","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.44","WR1":"6.85","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.71","WR2":"5.28","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.68","W":"23.31","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.77","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.28","T":"1082.70","W":"14.63","WT":"19.13","AT":"27.20","H":"37.90"},"Main1":{"M":"1","D":"0","RD":"0","RM":"0","RT":"0","RJ":"0"}}}
*/
void Module_Web::relayToGoogleSheets(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  {
    logToSerials(F("REST API reporting: api.pushingbox.com"), false, 2);
    logToSerials(JSONData, true, 0);
  }
  PushingBoxRestAPI.get(*JSONData); ///< PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
}

/**
* @brief Publish an MQTT message containing a JSON formatted report
* Examples:
* MQTT reporting: Gbox420/{"Log":{"IFan":{"S":"1"},"EFan":{"S":"1"},"APump1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"LtSen1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"45.20"},"Pow1":{"P":"573.60","E":"665.47","V":"227.20","C":"2.64","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.45","WR1":"6.77","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.69","WR2":"5.31","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.41","W":"23.57","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.50","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.41","T":"1071.30","W":"14.64","WT":"19.13","AT":"27.30","H":"38.50"},"Main1":{"M":"1","D":"0","RD":"0","RM":"1","RT":"0","RJ":"1"}}}
* MQTT reporting: Gbox420/{"EventLog":["","","Module initialized","Lt2 brightness updated"]}
*/
void Module_Web::mqttPublish(char (*JSONData)[MaxLongTextLength])
{
  if (MqttConnected)
  {
    if (*Debug)
    {
      logToSerials(F("MQTT reporting:"), false, 2);
      logToSerials(&(ModuleSettings->MqttPubTopic), false, 1);
      logToSerials(JSONData, true, 0);
    }
    MqttAPI.publish(ModuleSettings->MqttPubTopic, *JSONData, 0, 1); //(topic,message,qos (Only level 0 supported),retain )
  }
  else
  {
    if (*Debug)
    {
      logToSerials(F("MQTT broker not connected"), true, 2);
    }
  }
}


///< Settings
void Module_Web::setDebug(bool DebugEnabled)
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

char *Module_Web::getDebugText(bool FriendlyFormat)
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

void Module_Web::setMetric(bool MetricEnabled)
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

char *Module_Web::getMetricText(bool FriendlyFormat)
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

void Module_Web::setSheetsReportingOnOff(bool State)
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

void Module_Web::setSheetsReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("Sheets freqency updated"));
  getSoundObject()->playOnSound();
}

void Module_Web::setPushingBoxLogRelayID(const char *ID)
{
  strncpy(ModuleSettings->PushingBoxLogRelayID, ID, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("Sheets log relay ID updated"));
}

void Module_Web::reportToGoogleSheetsTrigger(bool ForceRun)
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

void Module_Web::setMQTTReportingOnOff(bool State)
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

void Module_Web::setMQTTReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog(F("MQTT freqency updated"));
  getSoundObject()->playOnSound();
}

void Module_Web::setMqttPublishTopic(const char *Topic)
{
  strncpy(ModuleSettings->MqttPubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT publish updated"));
}

void Module_Web::setMqttSubscribeTopic(const char *Topic)
{
  strncpy(ModuleSettings->MqttSubTopic, Topic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("MQTT subscribe updated"));
}

void Module_Web::setMQTTLWTTopic(const char *LWTTopic)
{
  strncpy(ModuleSettings->MqttLwtTopic, LWTTopic, MaxShotTextLength);
  getSoundObject()->playOnSound();
  addToLog(F("LWT topic updated"));
}

void Module_Web::setMQTTLWTMessage(const char *LWTMessage)
{
  strncpy(ModuleSettings->MqttLwtMessage, LWTMessage, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog(F("LWT message updated"));
}

void Module_Web::reportToMQTTTrigger(bool ForceRun)
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
