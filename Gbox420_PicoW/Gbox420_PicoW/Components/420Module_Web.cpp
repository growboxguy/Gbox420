#pragma GCC diagnostic ignored "-Wstringop-truncation"
#include "420Module_Web.h"
#include "Sound_Web.h"

static char Logs[LogDepth][MaxWordLength]; ///< two dimensional array for storing log history displayed on the website (array of char arrays)

/**
 * @brief Constructor: creates an instance of the class
 */
Module_Web::Module_Web(const char *Name) : Common(Name), Module(Name)
{
  // printf("   Module_Web ready\n");
}

/**
 * @brief Subscribe to the ESP-link website load event
 */
void Module_Web::addToWebsiteQueue_Load(Common_Web *Subscriber)
{
  if (QueueDepth > WebsiteQueue_Load_Count)
    WebsiteQueue_Load[WebsiteQueue_Load_Count++] = Subscriber;
  else
    printf("WebsiteQueue_Load overflow!\n");
}

/**
 * @brief Subscribe to the ESP-link website refresh event
 */
void Module_Web::addToWebsiteQueue_Refresh(Common_Web *Subscriber)
{
  if (QueueDepth > WebsiteQueue_Refresh_Count)
    WebsiteQueue_Refresh[WebsiteQueue_Refresh_Count++] = Subscriber;
  else
    printf("WebsiteQueue_Refresh overflow!\n");
}

/**
 * @brief Notify subscribed components of a website load event
 */
void Module_Web::websiteLoadEventTrigger(char *Url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueue_Load_Count; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(Url);
  }
}

/**
 * @brief Notify subscribed components of a website refresh event
 */
void Module_Web::websiteRefreshEventTrigger(char *Url)
{ ///< called when website is refreshed.
  for (int i = 0; i < WebsiteQueue_Refresh_Count; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(Url);
  }
}

void Module_Web::refresh() // TODO: Rework module refreshing logic, move Google Sheets to its own Component
{
  if (RefreshCounter % 5 == 0)
  {
    Common::refresh();
    reportToSerialTrigger();
    // reportToMQTTTrigger();

    /*
    if (RefreshRequested)
    {
      RefreshRequested = false;
      run();
    }
    if (ReportToGoogleSheetsRequested)
    {
      ReportToGoogleSheetsRequested = false;
      reportToGoogleSheetsTrigger(true);
    }
    if (ConsoleReportRequested)
    {
      ConsoleReportRequested = false;
      runReport(true);
    }
    */
  }
  // reportToGoogleSheetsTrigger();
}

/**
 * @brief Adds a log entry to the top of the log and removes the oldest log entry
 */
void Module_Web::addToLog(const char *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///< adds a log entry that is displayed on the web interface
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///< Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///< clear variable
    strncpy(Logs[i], Logs[i - 1], MaxWordLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));         ///< clear variable
  strncpy(Logs[0], LongMessage, MaxWordLength); ///< instert new log to [0]
  printf("%s\n", Logs[0]);
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
    strcat(LongMessage, "{\"EventLog\":"); ///< Adds a curly bracket that needs to be closed at the end
  }
  strcat(LongMessage, "[");
  for (int i = LogDepth - 1; i >= 0; i--)
  {
    strcat(LongMessage, "\"");
    strcat(LongMessage, Logs[i]);
    strcat(LongMessage, "\"");
    if (i > 0)
      strcat(LongMessage, ",");
  }
  LongMessage[strlen(LongMessage)] = ']';
  if (IncludeKey)
  {
    strcat(LongMessage, "}"); ///< closing curly bracket
  }
  return LongMessage;
}

/**
 * @brief Converts the module settings into a JSON object and loads it into the LongMessage buffer
 */
char *Module_Web::settingsToJSON()
{
  memset(&LongMessage[0], 0, MaxLongTextLength);
  strcat(LongMessage, "{\"Settings\":{"); ///< Adds a curly bracket that needs to be closed at the end
  strcat(LongMessage, "\"Debug\":\"");
  strcat(LongMessage, toText(*Debug));
  strcat(LongMessage, "\",\"Metric\":\"");
  strcat(LongMessage, toText(*Metric));
  strcat(LongMessage, "\",\"SerialF\":\"");
  strcat(LongMessage, toText(*SerialReportFrequency));
  strcat(LongMessage, "\",\"Date\":\"");
  strcat(LongMessage, toText(*SerialReportDate));
  strcat(LongMessage, "\",\"Mem\":\"");
  strcat(LongMessage, toText(*SerialReportMemory));
  strcat(LongMessage, "\",\"JSON\":\"");
  strcat(LongMessage, toText(*SerialReportJSON));
  strcat(LongMessage, "\",\"JSONF\":\"");
  strcat(LongMessage, toText(*SerialReportJSONFriendly));
  strcat(LongMessage, "\",\"Wire\":\"");
  strcat(LongMessage, toText(*SerialReportWireless));
  strcat(LongMessage, "\",\"Sheets\":\"");
  strcat(LongMessage, toText(*ReportToGoogleSheets));
  strcat(LongMessage, "\",\"SheetsF\":\"");
  strcat(LongMessage, toText(*SheetsReportingFrequency));
  strcat(LongMessage, "\",\"Relay\":\"");
  strcat(LongMessage, ModuleSettings->PushingBoxLogRelayID);
  /*
  strcat(LongMessage, "\",\"MQTT\":\"");
  strcat(LongMessage, toText(*ReportToMQTT));
  strcat(LongMessage, "\",\"MQTTF\":\"");
  strcat(LongMessage, toText(*MQTTReportFrequency));
  strcat(LongMessage, "\",\"MPT\":\"");
  strcat(LongMessage, ModuleSettings->PubTopic);
  strcat(LongMessage, "\",\"MST\":\"");
  strcat(LongMessage, ModuleSettings->SubTopic);
  strcat(LongMessage, "\",\"MLT\":\"");
  strcat(LongMessage, ModuleSettings->LwtTopic);
  strcat(LongMessage, "\",\"MLM\":\"");
  strcat(LongMessage, ModuleSettings->LwtMessage);
  */
  strcat(LongMessage, "\"}}"); ///< closing the curly brackets at the end of the JSON
  return LongMessage;
}

///< ESP-link web interface functions
void Module_Web::settingsEvent_Load(__attribute__((unused)) char *Url)
{
  /*
  WebServer.setArgInt("Debug", *Debug);
  WebServer.setArgInt("Metric", *Metric);
  WebServer.setArgInt("SerialF", *SerialReportFrequency);
  WebServer.setArgInt("Date", *SerialReportDate);
  WebServer.setArgInt("Mem", *SerialReportMemory);
  WebServer.setArgInt("JSON", *SerialReportJSON);
  WebServer.setArgInt("JSONF", *SerialReportJSONFriendly);
  WebServer.setArgInt("Wire", *SerialReportWireless);
  WebServer.setArgBoolean("Sheets", *ReportToGoogleSheets);
  WebServer.setArgInt("SheetsF", *SheetsReportingFrequency);
  WebServer.setArgString("Relay", ModuleSettings->PushingBoxLogRelayID);
  WebServer.setArgBoolean("MQTT", *ReportToMQTT);
  WebServer.setArgInt("MQTTF", *MQTTReportFrequency);
  WebServer.setArgString("MPT", ModuleSettings->PubTopic);
  WebServer.setArgString("MST", ModuleSettings->SubTopic);
  WebServer.setArgString("MLT", ModuleSettings->LwtTopic);
  WebServer.setArgString("MLM", ModuleSettings->LwtMessage);
  WebServer.setArgInt(getSoundObject()->getName("E", true), getSoundObject()->getEnabledState());
  */
}

void Module_Web::settingsEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  /*
  WebServer.setArgString("Time", getFormattedTime(false));  ///< Current time
  WebServer.setArgJson("Log", eventLogToJSON(false, true)); ///< Last events that happened in JSON format
  */
}

/**
 * @brief Process commands received from the ESP-link /Settings.html website
 */

void Module_Web::settingsEvent_Command(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
{
  // Report triggers
  if (strcmp(Command, "SheetsRep") == 0)
  {
    ReportToGoogleSheetsRequested = true; ///< just signal that a report should be sent, do not actually run it: Takes too long from an interrupt
    addToLog("Reporting to Sheets", false);
    getSoundObject()->playOnSound();
  }
  else if (strcmp(Command, "SerialRep") == 0)
  {
    ConsoleReportRequested = true;
    addToLog("Reporting to Serial", false);
    getSoundObject()->playOnSound();
  }
  else if (strcmp(Command, "MQTTRep") == 0)
  {
    MQTTReportRequested = true;
    addToLog("Reporting to MQTT", false);
    getSoundObject()->playOnSound();
  }
  else if (strcmp(Command, "Refresh") == 0) ///< Website signals to refresh all sensor readings
  {
    RefreshRequested = true;
    addToLog("Refreshing", false);
    getSoundObject()->playOnSound();
  }
  // Settings
  else if (strcmp(Command, "Debug") == 0)
  {
    setDebug(toBool(Data));
  }
  else if (strcmp(Command, "Metric") == 0)
  {
    setMetric(toBool(Data));
  }
  // Settings - Serial reporting
  else if (strcmp(Command, "SerialF") == 0)
  {
    setSerialReportingFrequency(toInt(Data));
  }
  else if (strcmp(Command, "Date") == 0)
  {
    setSerialReportDate(toBool(Data));
  }
  else if (strcmp(Command, "Mem") == 0)
  {
    setSerialReportMemory(toBool(Data));
  }
  else if (strcmp(Command, "JSON") == 0)
  {
    setSerialReportJSON(toBool(Data));
  }
  else if (strcmp(Command, "JSONF") == 0)
  {
    setSerialReportJSONFriendly(toBool(Data));
  }
  else if (strcmp(Command, "Wire") == 0)
  {
    setSerialReportWireless(toBool(Data));
  }
  // Settings - Google Sheets
  else if (strcmp(Command, "Sheets") == 0)
  {
    setSheetsReportingOnOff(toBool(Data));
  }
  else if (strcmp(Command, "SheetsF") == 0)
  {
    setSheetsReportingFrequency(toInt(Data));
  }
  else if (strcmp(Command, "Relay") == 0)
  {
    // setPushingBoxLogRelayID(WebServer.getArgString());
  }
  // Settings - MQTT
  else if (strcmp(Command, "MQTT") == 0)
  {
    // setMQTTReportingOnOff(toBool(Data));
  }
  else if (strcmp(Command, "MQTTF") == 0)
  {
    // setMQTTReportingFrequency(toInt(Data));
  }
  else if (strcmp(Command, "MPT") == 0)
  {
    // setMqttPublishTopic(WebServer.getArgString());
  }
  else if (strcmp(Command, "MST") == 0)
  {
    // setMqttSubscribeTopic(WebServer.getArgString());
  }
  else if (strcmp(Command, "MLT") == 0)
  {
    // setLwtTopic(WebServer.getArgString());
  }
  else if (strcmp(Command, "MLM") == 0)
  {
    // setLwtMessage(WebServer.getArgString());
  }
  else if (strcmp(Command, getSoundObject()->getName((char *)"E", true)) == 0)
  {
    getSoundObject()->setSoundOnOff(toBool(Data));
  }
  else if (strcmp(Command, getSoundObject()->getName((char *)"Ee", true)) == 0)
  {
    getSoundObject()->playEE();
  }
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
  strcat(LongMessage, "/pushingbox?devid=");
  strcat(LongMessage, ModuleSettings->PushingBoxLogRelayID);
  strcat(LongMessage, "&BoxData=");
}

/**
 * @brief Send a JSON formatted report to Google Sheets
 * Example
 * REST API reporting: api.pushingbox.com/pushingbox?devid=v755877CF53383E1&BoxData={"Log":{"FanI":{"S":"1"},"FanE":{"S":"1"},"Ap1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"Ls1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"44.50"},"Pow1":{"P":"572.20","E":"665.26","V":"228.00","C":"2.62","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.44","WR1":"6.85","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.71","WR2":"5.28","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.68","W":"23.31","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.77","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.28","T":"1082.70","W":"14.63","WT":"19.13","AT":"27.20","H":"37.90"},"Main1":{"M":"1","D":"0","RD":"0","RM":"0","RT":"0","RJ":"0"}}}
 */
void Module_Web::relayToGoogleSheets(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  {
    printf("  REST API reporting: api.pushingbox.com");
    printf("%s\n", JSONData);
  }
  // PushingBoxRestAPI.get(*JSONData); ///< PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
}

/**
 * @brief Publish an MQTT message containing a JSON formatted report
 * Examples:
 * MQTT reporting: Gbox420/Hempy/{"Log":{"FanI":{"S":"1"},"FanE":{"S":"1"},"Ap1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"Ls1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"45.20"},"Pow1":{"P":"573.60","E":"665.47","V":"227.20","C":"2.64","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.45","WR1":"6.77","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.69","WR2":"5.31","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.41","W":"23.57","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.50","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.41","T":"1071.30","W":"14.64","WT":"19.13","AT":"27.30","H":"38.50"},"Main1":{"M":"1","D":"0","RD":"0","RM":"1","RT":"0","RJ":"1"}}}
 * MQTT reporting: char PubTopic[MaxShotTextLength]Hempy/{"EventLog":["","","Module initialized","Lt2 brightness updated"]}
 */
void Module_Web::mqttPublish(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  {
    printf("  MQTT reporting: ");
    // printf(&(ModuleSettings->PubTopic));
    printf("%s\n", JSONData);
  }
  /*
  if (MqttConnected)
  {
    MqttAPI.publish(ModuleSettings->PubTopic, *JSONData, 0, 1); //(topic,message,qos (Only level 0 supported),retain )
  }

  else
  */
  {
    if (*Debug)
    {
      printf("  MQTT broker not connected\n");
    }
  }
}

///< Settings
void Module_Web::setDebug(bool DebugEnabled)
{
  *Debug = DebugEnabled;
  if (*Debug)
  {
    addToLog("Debug ON");
  }
  else
  {
    addToLog("Debug OFF");
  }
  getSoundObject()->playOnOffSound(*Debug);
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
    RefreshRequested = true;
  }
  if (*Metric)
    addToLog("Using Metric units");
  else
    addToLog("Using Imperial units");
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
  if (*ReportToGoogleSheets)
  {
    addToLog("Sheets ON");
  }
  else
  {
    addToLog("Sheets OFF");
  }
  getSoundObject()->playOnOffSound(*ReportToGoogleSheets);
}

void Module_Web::setSheetsReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog("Sheets freqency updated");
  getSoundObject()->playOnSound();
}

void Module_Web::setPushingBoxLogRelayID(const char *ID)
{
  strncpy(ModuleSettings->PushingBoxLogRelayID, ID, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog("Sheets log relay ID updated");
}

void Module_Web::reportToGoogleSheetsTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for Google Sheets
  if ((*ReportToGoogleSheets && SheetsTriggerCounter++ % (*SheetsReportingFrequency) == 0) || ForceRun)
  {
    addPushingBoxLogRelayID();           // Loads Pushingbox relay ID into LongMessage
    runReport(false, false, true, true); // Append the sensor readings in a JSON format to LongMessage buffer
    relayToGoogleSheets(&LongMessage);   // Sends it to Google Sheets
  }
}
///< This is how a sent out message looks like:
///< {parameter={Log={"Report":{"InternalTemp":"20.84","ExternalTemp":"20.87","InternalHumidity":"38.54","ExternalHumidity":"41.87","InternalFan":"0","ExhaustFan":"0","Lt1_Status":"0","Lt1_Brightness":"15","LightReading":"454","Dark":"1","WaterLevel":"0","WaterTemp":"20.56","PH":"17.73","Pressure":"-0.18","Power":"-1.00","Energy":"-0.00","Voltage":"-1.00","Current":"-1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"},"Settings":{"Metric":"1"}}}, contextPath=, contentLength=499, queryString=, parameters={Log=[Ljava.lang.Object;@60efa46b}, postData=FileUpload}

/**
  \brief Triggers sending out a sequence of MQTT messages to the PubTopic specified in Settings.h.
  Sample messages:
  Gbox420/Hempy/{"Log":{"DHT1":{"T":"29.00","H":"52.00"},"B1W":{"W":"19.35"},"B2W":{"W":"19.75"},"NRW":{"W":"43.30"},"WRW":{"W":"1.87"},"WR1":{"S":"1","L":"13.00"},"B1P":{"S":"1","T":"120"},"B2P":{"S":"1","T":"120"},"B1":{"S":"1","DW":"18.00","WW":"19.70","ET":"2.00","OF":"0.30","DT":"180"},"B2":{"S":"1","DW":"18.00","WW":"19.70","ET":"2.00","OF":"0.30","DT":"180"},"Hemp1":{"M":"1","D":"1"}}}
  Gbox420/Hempy/{"EventLog":["","","","Hempy_Standalone ready"]}
  Gbox420/Hempy/{"Settings":{"Debug":"1","Metric":"1","SerialF":"15","Date":"1","Mem":"1","JSON":"1","JSONF":"1","Wire":"1","Sheets":"1","SheetsF":"30","Relay":"v755877CF53383E1","MQTT":"1","MQTTF":"5","MPT":"Gbox420/Hempy","MST":"Gbox420CMD/Hempy/#","MLT":"Gbox420LWT/Hempy/","MLM":"Hempy Offline"}}
*/
void Module_Web::reportToMQTTTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for MQTT
  if ((*ReportToMQTT && MQTTTriggerCounter++ % (*MQTTReportFrequency / 5) == 0) || ForceRun)
  {
    runReport(false, true, true, true); //< Loads a JSON Log to LongMessage buffer  \TODO: Should call this Readings instead of Log
    /*
    mqttPublish(&LongMessage);          //< Publish Log via ESP MQTT API
    eventLogToJSON(true, true);         //< Loads the EventLog as a JSON
    mqttPublish(&LongMessage);          //< Publish the EventLog via ESP MQTT API
    settingsToJSON();                   //< Loads the module settings as a JSON to the LongMessage buffer
    mqttPublish(&LongMessage);          //< Publish the Settings via ESP MQTT API
    */
  }
}

Sound_Web *Module_Web::getSoundObject()
{
  return SoundFeedback;
}