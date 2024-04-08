#pragma GCC diagnostic ignored "-Wstringop-truncation"
#include "420Module.h"
#include "Sound.h"
#include "MqttClient.h"

static char Logs[LogDepth][MaxWordLength]; ///< two dimensional array for storing log history displayed on the website (array of char arrays)

Module::Module(const char *Name) : Common(Name)
{ ///< Constructor
  printf("   Module ready\n");
}

/**
 * @brief Handles custom reporting frequency for Serial
 * @param ForceRun Send a report instantly, even when regular reports are disabled
 * @param ClearBuffer Flush the LongMessage buffer before starting to report
 * @param KeepBuffer Stores the full JSON report in the LongMessage buffer (up to 1024kB)
 * @param JSONToBufferOnly Do not print anything on the serial output, only fill the LongMessage buffer with the JSON report
 */
void Module::reportToSerialTrigger(bool ForceRun, bool ClearBuffer, bool KeepBuffer, bool JSONToBufferOnly)
{
  if ((SerialTriggerCounter++ % (*SerialReportFrequency / 5) == 0) || ForceRun)
  {
    runReport(ForceRun, ClearBuffer, KeepBuffer, JSONToBufferOnly);
  }
}

/**
 * @brief Notify subscribed components of a received MQTT/Website command
 */
void Module::commandEventTrigger(char *Command, char *Data)
{
  bool NameMatchFound = false;
  for (int i = 0; i < CommandQueueItemCount; i++)
  {
    NameMatchFound = CommandQueue[i]->commandEvent(Command, Data);
    if (NameMatchFound)
      break;
  }
  // if (!NameMatchFound) ///< None of the subscribed component Names matched the command. Try processing it as a Module settings command.
  //  settingsEvent_Command(Command, Data); // TODO: The module should subscribe to the commandEvent callback the same way as any other component
}

/**
 * @brief Set how often a report should be sent to stdout
 * @param Frequency Send a report every X seconds
 */
void Module::setSerialReportingFrequency(uint16_t Frequency)
{
  if (Frequency != *SerialReportFrequency)
  {
    *SerialReportFrequency = Frequency;
  }
  getSoundObject()->playOnSound();
}

/**
 * @brief Reports sensor readings to stdout or to the LongMessage buffer
 * @param ForceRun Send a report instantly, even when regular reports are disabled
 * @param ClearBuffer Flush the LongMessage buffer before starting to report
 * @param KeepBuffer Stores the full JSON report in the LongMessage buffer - Only use this on the Mega2560 where LongMessage is large enough to store a complete report (Can be up to 1024kB)
 * @param JSONToBufferOnly Do not print anything on the serial output, only fill the LongMessage buffer with the JSON report
 */
void Module::runReport(bool ForceRun, bool ClearBuffer, bool KeepBuffer, bool JSONToBufferOnly)
{
  if ((*SerialReportDate || ForceRun) && !JSONToBufferOnly)
  {
    rtcGetCurrentTime(true);
  }
  if ((*SerialReportMemory || ForceRun) && !JSONToBufferOnly)
  {
    // getFreeMemory();
  }
  if ((*SerialReportJSONFriendly || ForceRun) && !JSONToBufferOnly)
  {
    printf("%u items reporting:\n", ReportQueueItemCount); ///< Prints the number of items that will report
    for (int i = 0; i < ReportQueueItemCount; i++)
    {
      ReportQueue[i]->report(false);
    }
  }
  if (*SerialReportJSON || ForceRun || JSONToBufferOnly)
  {
    if (ClearBuffer)
    {
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    }
    strcat(LongMessage, "{\"Log\":{"); ///< Adds two curly brackets that needs to be closed at the end
    if (!KeepBuffer)
    {
      printf("%s\n", LongMessage);
      memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
    }
    for (int i = 0; i < ReportQueueItemCount;)
    {
      ReportQueue[i++]->report(JSONToBufferOnly || KeepBuffer ? false : *SerialReportJSONFriendly);
      if (i != ReportQueueItemCount)
        strcat(LongMessage, ","); ///< < Unless it was the last element add a , separator
      if (!KeepBuffer)
      {
        printf("%s\n", LongMessage);
        memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
      }
    }
    strcat(LongMessage, "}}"); ///< closing both curly bracket
    if (!JSONToBufferOnly)
      printf("%s\n", LongMessage);
  }
}

///< Refresh queues: Refresh components inside the Module

Sound *Module::getSoundObject()
{
  return DefaultSound;
}

///< Queue subscriptions: When a component needs to get refreshed at certain intervals it subscribes

/**
 * @brief Subscribe to report() calls
 */
void Module::addToReportQueue(Common *Component)
{
  if (QueueDepth > ReportQueueItemCount)
    ReportQueue[ReportQueueItemCount++] = Component;
  else
    printf("Report queue overflow\n"); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h , or shift components to a different queue
}

/**
 * @brief Subscribe to refresh() calls
 *
 * @param Component Pointer to the object subscribing
 */
void Module::addToRefreshQueue_1sec(Common *Component)
{
  if (QueueDepth > RefreshQueue_1sec_ItemCount)
    RefreshQueue_1sec[RefreshQueue_1sec_ItemCount++] = Component;
  else
    printf("1sec queue overflow\n"); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h
}

void Module::addToRefreshQueue_5sec(Common *Component)
{
  if (QueueDepth > RefreshQueue_5sec_ItemCount)
    RefreshQueue_5sec[RefreshQueue_5sec_ItemCount++] = Component;
  else
    printf("5sec queue overflow\n"); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h
}

void Module::addToRefreshQueue_1min(Common *Component)
{
  if (QueueDepth > RefreshQueue_1min_ItemCount)
    RefreshQueue_1min[RefreshQueue_1min_ItemCount++] = Component;
  else
    printf("1min queue overflow\n"); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h
}

void Module::addToRefreshQueue_30min(Common *Component)
{
  if (QueueDepth > RefreshQueue_30min_ItemCount)
    RefreshQueue_30min[RefreshQueue_30min_ItemCount++] = Component;
  else
    printf("30min queue overflow\n"); ///< Too many components are added to the queue, increase "QueueDepth" variable in Settings.h
}

/**
 * @brief Subscribe to commandEvent(char *Command, char *Data) calls  (MQTT command)
 */
void Module::addToCommandQueue(Common *Subscriber)
{
  if (QueueDepth > CommandQueueItemCount)
    CommandQueue[CommandQueueItemCount++] = Subscriber;
  else
    printf("CommandQueue overflow!\n");
}

/**
 * @brief Adds a log entry to the top of the log and removes the oldest log entry
 */
void Module::addToLog(const char *LongMessage, __attribute__((unused)) uint8_t Indent)
{ ///< adds a log entry that is displayed on the web interface
  for (uint8_t i = LogDepth - 1; i > 0; i--)
  {                                       ///< Shift every log entry one up, dropping the oldest
    memset(&Logs[i], 0, sizeof(Logs[i])); ///< clear variable
    strncpy(Logs[i], Logs[i - 1], MaxWordLength);
  }
  memset(&Logs[0], 0, sizeof(Logs[0]));         ///< clear variable
  strncpy(Logs[0], LongMessage, MaxWordLength); ///< insert new log to [0]
  printf("%s\n", Logs[0]);
}

///< Settings
void Module::setDebug(bool DebugEnabled)
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

void Module::setMetric(bool MetricEnabled)
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

void Module::setSerialReportDate(bool State)
{
  if (State != *SerialReportDate)
  { // if there was a change
    *SerialReportDate = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportDate);
}

void Module::setSerialReportMemory(bool State)
{
  if (State != *SerialReportMemory)
  { // if there was a change
    *SerialReportMemory = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportMemory);
}

void Module::setSerialReportJSONFriendly(bool State)
{
  if (State != *SerialReportJSONFriendly)
  { // if there was a change
    *SerialReportJSONFriendly = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportJSONFriendly);
}

void Module::setSerialReportJSON(bool State)
{
  if (State != *SerialReportJSON)
  { // if there was a change
    *SerialReportJSON = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportJSON);
}

void Module::setSerialReportWireless(bool State)
{
  if (State != *SerialReportWireless)
  { // if there was a change
    *SerialReportWireless = State;
  }
  getSoundObject()->playOnOffSound(*SerialReportWireless);
}

/**
 * @brief Subscribe to the ESP-link website load event
 */
void Module::addToWebsiteQueue_Load(Common *Subscriber)
{
  if (QueueDepth > WebsiteQueue_Load_Count)
    WebsiteQueue_Load[WebsiteQueue_Load_Count++] = Subscriber;
  else
    printf("WebsiteQueue_Load overflow!\n");
}

/**
 * @brief Subscribe to the ESP-link website refresh event
 */
void Module::addToWebsiteQueue_Refresh(Common *Subscriber)
{
  if (QueueDepth > WebsiteQueue_Refresh_Count)
    WebsiteQueue_Refresh[WebsiteQueue_Refresh_Count++] = Subscriber;
  else
    printf("WebsiteQueue_Refresh overflow!\n");
}

/**
 * @brief Notify subscribed components of a website load event
 */
void Module::websiteLoadEventTrigger(char *Url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  for (int i = 0; i < WebsiteQueue_Load_Count; i++)
  {
    WebsiteQueue_Load[i]->websiteEvent_Load(Url);
  }
}

/**
 * @brief Notify subscribed components of a website refresh event
 */
void Module::websiteRefreshEventTrigger(char *Url)
{ ///< called when website is refreshed.
  for (int i = 0; i < WebsiteQueue_Refresh_Count; i++)
  {
    WebsiteQueue_Refresh[i]->websiteEvent_Refresh(Url);
  }
}

void Module::run1sec()
{
  Common::run1sec();
  if (RunAllRequested)
  {
    RunAllRequested = false;
    runAll();
  }
  else
  {
    for (int i = 0; i < RefreshQueue_1sec_ItemCount; i++)
    {
      RefreshQueue_1sec[i]->run1sec();
    }
  }
}

void Module::run5sec()
{
  Common::run5sec();
  reportToSerialTrigger();
  reportToMqttTrigger();

  for (int i = 0; i < RefreshQueue_5sec_ItemCount; i++)
  {
    RefreshQueue_5sec[i]->run5sec();
  }

  /*
  if (ReportToGoogleSheetsRequested)   // TODO: move Google Sheets to its own Component
  {
    ReportToGoogleSheetsRequested = false;
    reportToGoogleSheetsTrigger(true);
  }

  if (ConsoleReportRequested)
  {
    ConsoleReportRequested = false;
    runReport(true);
  }
   reportToGoogleSheetsTrigger();
   */
}

void Module::run1min()
{
  Common::run1min();
  for (int i = 0; i < RefreshQueue_1min_ItemCount; i++)
  {
    RefreshQueue_1min[i]->run1min();
  }
}

void Module::run30min()
{
  Common::run30min();
  for (int i = 0; i < RefreshQueue_30min_ItemCount; i++)
  {
    RefreshQueue_30min[i]->run30min();
  }
}

void Module::runAll()
{
  run1sec();
  run5sec();
  run1min();
  run30min();
}

/**
 * @brief Called when an MQTT command is received
 */
void Module::mqttDataReceived(char *Topic, char *Data)
{
  //printf("Topic: %s\n", Topic); // Print the topic the subscribed message was received in, minus the pre-known part of the Topic. Example: Subscribed topic: Gbox420CMD/#  Received: Gbox420CMD/Light1_ON --> Light1_ON 
  //printf("Data: %s\n", Data);   // Print the message received on the subscribed topic
  commandEventTrigger(Topic, Data);
  reportToMqttTrigger(true); //send out a fresh report
}

/**
 * @brief Converts the log entries to a JSON object
 * IncludeKey false: ["Log1","Log2","Log3",...,"LogN"],
 * IncludeKey true:  {"EventLog"\["Log1","Log2","Log3",...,"LogN"]}
 * Append false: Clear the LongMessage buffer before
 */
char *Module::eventLogToJSON(bool IncludeKey, bool ClearBuffer)
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
char *Module::settingsToJSON()
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
  strcat(LongMessage, GboxSettings->PushingBoxLogRelayID);
  /*
  strcat(LongMessage, "\",\"MQTT\":\"");
  strcat(LongMessage, toText(*ReportToMqtt));
  strcat(LongMessage, "\",\"MQTTF\":\"");
  strcat(LongMessage, toText(*MQTTReportFrequency));
  strcat(LongMessage, "\",\"MPT\":\"");
  strcat(LongMessage, GboxSettings->PubTopic);
  strcat(LongMessage, "\",\"MST\":\"");
  strcat(LongMessage, GboxSettings->SubTopic);
  strcat(LongMessage, "\",\"MLT\":\"");
  strcat(LongMessage, GboxSettings->LwtTopic);
  strcat(LongMessage, "\",\"MLM\":\"");
  strcat(LongMessage, GboxSettings->LwtMessage);
  */
  strcat(LongMessage, "\"}}"); ///< closing the curly brackets at the end of the JSON
  return LongMessage;
}

///< ESP-link web interface functions
void Module::settingsEvent_Load(__attribute__((unused)) char *Url)
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
  WebServer.setArgString("Relay", GboxSettings->PushingBoxLogRelayID);
  WebServer.setArgBoolean("MQTT", *ReportToMqtt);
  WebServer.setArgInt("MQTTF", *MQTTReportFrequency);
  WebServer.setArgString("MPT", GboxSettings->PubTopic);
  WebServer.setArgString("MST", GboxSettings->SubTopic);
  WebServer.setArgString("MLT", GboxSettings->LwtTopic);
  WebServer.setArgString("MLM", GboxSettings->LwtMessage);
  WebServer.setArgInt(getSoundObject()->getName("E", true), getSoundObject()->getEnabledState());
  */
}

void Module::settingsEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  /*
  WebServer.setArgString("Time", rtcGetCurrentTime(false));  ///< Current time
  WebServer.setArgJson("Log", eventLogToJSON(false, true)); ///< Last events that happened in JSON format
  */
}

/**
 * @brief Process commands received from the ESP-link /Settings.html website
 */

void Module::settingsEvent_Command(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data)
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
}

///< Google Sheets functions - https://sites.google.com/site/growboxguy/esp-link/logging

/**
 * @brief Loads the PushingBox device ID to the LongMessage buffer. Pushingbox is a relay service that is used to send reports to Google Sheets: https://sites.google.com/site/growboxguy/esp-link/logging
 * Example
 * /pushingbox?devid=v755877CF53383E1&BoxData=
 */
void Module::addPushingBoxLogRelayID()
{
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat(LongMessage, "/pushingbox?devid=");
  strcat(LongMessage, GboxSettings->PushingBoxLogRelayID);
  strcat(LongMessage, "&BoxData=");
}

/**
 * @brief Send a JSON formatted report to Google Sheets
 * Example
 * REST API reporting: api.pushingbox.com/pushingbox?devid=v755877CF53383E1&BoxData={"Log":{"FanI":{"S":"1"},"FanE":{"S":"1"},"Ap1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"Ls1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"44.50"},"Pow1":{"P":"572.20","E":"665.26","V":"228.00","C":"2.62","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.44","WR1":"6.85","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.71","WR2":"5.28","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.68","W":"23.31","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.77","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.28","T":"1082.70","W":"14.63","WT":"19.13","AT":"27.20","H":"37.90"},"Main1":{"M":"1","D":"0","RD":"0","RM":"0","RT":"0","RJ":"0"}}}
 */
void Module::relayToGoogleSheets(char (*JSONData)[MaxLongTextLength])
{
  if (*Debug)
  {
    printf("  REST API reporting: api.pushingbox.com");
    printf("%s\n", *(char(*)[MaxLongTextLength])JSONData);
  }
  // PushingBoxRestAPI.get(*JSONData); ///< PushingBoxRestAPI will append http://api.pushingbox.com/ in front of the command
}

/**
 * @brief Publish an MQTT message containing a JSON formatted report
 * Examples:
 * MQTT reporting: Gbox420/Hempy/{"Log":{"FanI":{"S":"1"},"FanE":{"S":"1"},"Ap1":{"S":"1"},"Lt1":{"S":"1","CB":"85","B":"85","T":"1","On":"14:20","Of":"02:20"},"Lt2":{"S":"1","CB":"95","B":"95","T":"1","On":"10:20","Of":"02:20"},"Ls1":{"R":"959","D":"0"},"DHT1":{"T":"26.70","H":"45.20"},"Pow1":{"P":"573.60","E":"665.47","V":"227.20","C":"2.64","F":"50.00","PF":"0.96"},"Hemp1":{"S":"1","H1":"1","P1":"1","PS1":"100","PT1":"120","DT1":"300","WB1":"21.45","WR1":"6.77","DW1":"19.00","WW1":"0.00","ET1":"2.00","OT1":"0.20","WL1":"13.00","H2":"1","P2":"1","PS2":"100","PT2":"120","DT2":"300","WB2":"19.69","WR2":"5.31","DW2":"19.30","WW2":"0.00","ET2":"2.00","OT2":"0.20","WL2":"13.00"},"Aero1":{"S":"1","P":"5.41","W":"23.57","Mi":"5.00","Ma":"7.00","AS":"1","LS":"5.50","PSt":"1","PS":"100","PT":"420","PP":"10","SE":"1","D":"3.00","DI":"6","NI":"10"},"Res1":{"S":"1","P":"2.41","T":"1071.30","W":"14.64","WT":"19.13","AT":"27.30","H":"38.50"},"Main1":{"M":"1","D":"0","RD":"0","RM":"1","RT":"0","RJ":"1"}}}
 * MQTT reporting: char PubTopic[MaxShotTextLength]Hempy/{"EventLog":["","","Module initialized","Lt2 brightness updated"]}
 */
void Module::mqttPublish(MqttClient *Client, char* JSONData)
{   
  if (Client != nullptr && Client->mqttIsConnected())
  {
    if (*Debug)
    {
      printf("  MQTT reporting to %s - %s\n", Client->PubTopic, JSONData);
    }
    Client->mqttPublish(MqttClientDefault->PubTopic, JSONData); //(topic,message,qos (Only level 0 supported),retain )
  }

  else
  {
    if (*Debug)
    {
      printf("  MQTT broker not connected\n");
    }
  }
}

///< Settings

char *Module::getDebugText(bool FriendlyFormat)
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

char *Module::getMetricText(bool FriendlyFormat)
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

void Module::setSheetsReportingOnOff(bool State)
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

void Module::setSheetsReportingFrequency(uint16_t Frequency)
{
  *SheetsReportingFrequency = Frequency;
  addToLog("Sheets frequency updated");
  getSoundObject()->playOnSound();
}

void Module::setPushingBoxLogRelayID(const char *ID)
{
  // strncpy(GboxSettings->PushingBoxLogRelayID, ID, MaxWordLength);
  getSoundObject()->playOnSound();
  addToLog("Sheets log relay ID updated");
}

void Module::reportToGoogleSheetsTrigger(bool ForceRun)
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
  Gbox420/Hempy/{"EventLog":["","","","HempyModule ready"]}
  Gbox420/Hempy/{"Settings":{"Debug":"1","Metric":"1","SerialF":"15","Date":"1","Mem":"1","JSON":"1","JSONF":"1","Wire":"1","Sheets":"1","SheetsF":"30","Relay":"v755877CF53383E1","MQTT":"1","MQTTF":"5","MPT":"Gbox420/Hempy","MST":"Gbox420CMD/Hempy/#","MLT":"Gbox420LWT/Hempy/","MLM":"Hempy Offline"}}
*/
void Module::reportToMqttTrigger(bool ForceRun)
{ ///< Handles custom reporting frequency for MQTT
  if (*ReportToMqtt || ForceRun)
  {
    runReport(false, true, true, true); //< Loads a JSON Log to LongMessage buffer  \TODO: Should call this Readings instead of Log
    mqttPublish(MqttClientDefault, LongMessage); //< Publish Log via ESP MQTT API
    eventLogToJSON(true, true);                    //< Loads the EventLog as a JSON
    mqttPublish(MqttClientDefault, LongMessage); //< Publish the EventLog via ESP MQTT API
    settingsToJSON(); //< Loads the module settings as a JSON to the LongMessage buffer
    mqttPublish(MqttClientDefault, LongMessage); //< Publish the Settings via ESP MQTT API
  }
}