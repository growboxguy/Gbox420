/*! \file
 *  \brief     Main module for Mega2560 - Grow tent monitoring and controlling sketch.
 *  \details   To change the default pin layout / startup settings navigate to: Settings.h
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 *
 */

#include "Arduino.h"
#include "avr/wdt.h"                        // Watchdog timer for detecting a crash and automatically resetting the board
#include "avr/boot.h"                       // Watchdog timer related bug fix
#include "printf.h"                         // Printing the wireless status message from nRF24L01
#include "TimerThree.h"                     // Interrupt handling for webpage
#include "ELClient.h"                       // ESP-link
#include "ELClientWebServer.h"              // ESP-link - WebServer API
#include "ELClientCmd.h"                    // ESP-link - Get current time from the internet using NTP
#include "ELClientRest.h"                   // ESP-link - REST API
#include "ELClientMqtt.h"                   // ESP-link - MQTT protocol for sending and receiving IoT messages
#include "Thread.h"                         // Splitting functions to threads for timing
#include "StaticThreadController.h"         // Grouping threads
#include "SerialLog.h"                      // Logging to the Serial console and to ESP-link's console
#include "Settings.h"                       // EEPROM stored settings for every component
#include "src/Modules_Web/MainModule_Web.h" // Represents a complete box with all feautres
#include "SPI.h"                            // allows you to communicate with SPI devices, with the Arduino as the master device
#include "RF24.h"                           // https://github.com/maniacbug/RF24

// Global variable initialization
char LongMessage[MaxLongTextLength] = "";  ///< Temp storage for assembling long messages (REST API, MQTT reporting)
char ShortMessage[MaxShotTextLength] = ""; ///< Temp storage for assembling short text messages (Log entries, Error messages,etc)
char CurrentTime[MaxWordLength] = "";      ///< Buffer for storing current time in text format

// Component initialization
HardwareSerial &ArduinoSerial = Serial;      ///< Reference to the Arduino Serial output : Mega 2560 Rev3	Serial:0(RX) 1(TX) also showed on USB, Serial1:19(RX1) 18(TX1), Serial2:17(RX2), 16(TX2), Serial4:15(RX3) 14(TX3)
HardwareSerial &ESPSerial = Serial3;         ///< Reference to the ESP Link Serial output : Mega 2560 Rev3	Serial:0(RX) 1(TX), Serial1:19(RX1) 18(TX1), Serial2:17(RX2), 16(TX2), Serial4:15(RX3) 14(TX3)
ELClient ESPLink(&ESPSerial);                ///< ESP-link. Both SLIP and debug messages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink);       ///< ESP-link - WebServer API
ELClientCmd ESPCmd(&ESPLink);                ///< ESP-link - Helps getting the current time from the internet using NTP
ELClientRest PushingBoxRestAPI(&ESPLink);    ///< ESP-link - REST API for PushingBox
ELClientRest HomeAssistantRestAPI(&ESPLink); ///< ESP-link - REST API for HomeAssistant
ELClientMqtt MqttAPI(&ESPLink);              ///< ESP-link - MQTT protocol for sending and receiving messages
Settings *ModuleSettings;                    ///< This object will store the settings loaded from the EEPROM. Persistent between reboots.
bool &Debug = *new bool;                     ///< True - Turns on extra debug messages on the Serial Output
bool &Metric = *new bool;                    ///< True - Use metric units, False - Use imperial units
bool MqttConnected = false;                  ///< Track the connection state to the MQTT broker configured on the ESP-link's REST/MQTT tab
MainModule *Main1;                           ///< Represents a Grow Box with all components (Lights, DHT sensors, Power sensor, Aero/Hempy/Reservoir wireless modules,..etc)

RF24 Wireless(WirelessCEPin, WirelessCSNPin); ///< Wireless communication with Modules over nRF24L01+

// Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
StaticThreadController<3> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread);

void setup()
{                                                       ///<  put your setup code here, to run once:
  ArduinoSerial.begin(115200);                          ///< 2560mega console output
  ESPSerial.begin(115200);                              ///< ESP WiFi console output
  pinMode(LED_BUILTIN, OUTPUT);                         ///< onboard LED - Heartbeat every second to confirm code is running
  printf_begin();                                       ///< Needed to print wireless status to Serial
  logToSerials(F(""), true, 0);                         ///< New line
  logToSerials(F("Main module initializing"), true, 0); ///< logs to both Arduino and ESP serials, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https://gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                  ///< Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                    ///< fix watchdog not loading sketch after a reset error on Mega2560

  // Loading settings from EEPROM
  logToSerials(F("Loading settings"), true, 0);
  ModuleSettings = loadSettings();
  Debug = ModuleSettings->Debug;
  Metric = &ModuleSettings->Metric;

  logToSerials(F("Setting up ESP-link connection"), true, 0);
  ESPLink.resetCb = &resetWebServer; ///< Callback subscription: What to do when WiFi reconnects
  resetWebServer();                  ///< reset the WebServer
  setSyncProvider(getNtpTime);       ///< Points to method for updating time from NTP server
  setSyncInterval(86400);            ///< Sync time every day
  if ((ModuleSettings->Main1).ReportToMqtt)
  {
    setupMqtt(); // MQTT message relay setup. Logs "ConnectedCB is XXXX" to serial if successful
  }
  // Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  logToSerials(F("Setting up refresh threads"), false, 0);
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(run1sec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(run5sec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(run1min);
  logToSerials(F("done"), true, 3);

  // Start interrupts to handle request from ESP-link firmware
  logToSerials(F("Setting up interrupt handler"), false, 0);
  Timer3.initialize(500); ///< check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();
  logToSerials(F("done"), true, 3);

  // Initialize wireless communication with remote Modules
  logToSerials(F("Setting up wireless transceiver"), false, 0);
  Wireless.begin();                                  ///< Initialize the nRF24L01+ wireless chip for talking to Modules
  Wireless.setDataRate(RF24_250KBPS);                ///< Set the speed to slow - has longer range + No need for faster transmission, Other options: RF24_2MBPS, RF24_1MBPS
  Wireless.setCRCLength(RF24_CRC_16);                ///< RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
  Wireless.setPALevel(RF24_PA_MAX);                  ///< RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
  Wireless.setPayloadSize(WirelessPayloadSize);      ///< The number of bytes in the payload. This implementation uses a fixed payload size for all transmissions
  Wireless.enableDynamicPayloads();                  ///< Required for ACK messages
  Wireless.enableAckPayload();                       ///< When sending a wireless package, expect a response confirming the package was received in a custom Acknowledgement package
  Wireless.setRetries(WirelessDelay, WirelessRetry); ///< Defined in Settings.h. How many retries before giving up sending a single package and How long to wait between each retry
  Wireless.stopListening();
  logToSerials(F("done"), true, 3);

  // Create the Module objects
  logToSerials(F("Creating Main module"), true, 0);
  Main1 = new MainModule(F("Main1"), &ModuleSettings->Main1, &Wireless); ///< This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  //   sendEmailAlert(F("Grow%20box%20(re)started"));
  logToSerials(F("Setup ready, starting loops:"), true, 0);
}

void loop()
{
  ThreadControl.run(); ///< loop only checks if it's time to trigger one of the threads (run1sec(), run5sec(),run1min()..etc)
}

void processTimeCriticalStuff()
{
  ESPLink.Process(); ///< Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage. Uses Serial Line Internet Protocol (SLIP)
}

// Threads

void run1sec()
{
  wdt_reset(); ///< reset watchdog timeout
  heartBeat(); ///< Blinks built-in led
  Main1->run1sec();
}

void run5sec()
{
  wdt_reset();
  Main1->run5sec();
}

void run1min()
{
  wdt_reset();
  Main1->run1min();
  getWirelessStatus();
}

/**
  \brief Turns the integrated LED on the Arduino board ON/OFF
*/
void heartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite(LED_BUILTIN, ledStatus);
}

// Website related functions

/**
  \brief (re-)Initialize the ESP-link connection
*/
void resetWebServer()
{ ///<  Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("(re)Connecting ESP-link"), false, 1);
  while (!ESPLink.Sync())
  {
    logToSerials(F("."), false, 0);
    delay(1000);
  };
  logToSerials(F(""), true, 0);                           ///< line break
  if (PushingBoxRestAPI.begin("api.pushingbox.com") == 0) ///< Pre-setup PushingBox relay to Google Sheets (Not actual connection)
  {
    logToSerials(F("PushingBox RestAPI ready"), true, 2);
  }
  else
    logToSerials(F("PushingBox RestAPI failed"), true, 2); ///< If begin returns a negative number the initialization failed
  /*
  if (HomeAssistantRestAPI.begin(HomeAssistantServerIP, HomeAssistantServerPort, true) == 0) ///< Pre-setup RestAPI with Home Assistant (Not actual connection)
  {
    HomeAssistantRestAPI.setContentType("application/json");
    HomeAssistantRestAPI.setHeader(HomeAssistantServerToken);

    logToSerials(F("HomeAssistant RestAPI ready"), true, 2);
  }
  else
    logToSerials(F("HomeAssistant RestAPI failed"), true, 2); ///< If begin returns a negative number the initialization failed
  */
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/Main.html.json");      ///< setup handling request from GrowBox.html
  GrowBoxHandler->loadCb.attach(&loadCallback);                                    ///< GrowBox tab - Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback);                              ///< GrowBox tab - Called periodically to refresh website content
  GrowBoxHandler->buttonCb.attach(&buttonCallback);                                ///< GrowBox tab - Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&fieldCallback);                               ///< GrowBox tab - Called when a field is changed on the website
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); ///< setup handling request from Settings.html
  SettingsHandler->loadCb.attach(&settingsLoadCallback);                           ///< Settings tab - Called then the website loads initially
  SettingsHandler->refreshCb.attach(&settingsRefreshCallback);                     ///< Settings tab - Called periodically to refresh website content
  SettingsHandler->buttonCb.attach(&settingsButtonCallback);                       ///< Settings tab - Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&settingsFieldCallback);                      ///< Settings tab - Called when a field is changed on the website
  URLHandler *HempyHandler = WebServer.createURLHandler("/Hempy.html.json");       ///< setup handling request from Hempy.html (embeds the Stand-alone Hempy module's web interface)
  HempyHandler->loadCb.attach(&ignoreCallback);                                    ///< Ignore event, handled by the Stand-alone Hempy module
  HempyHandler->refreshCb.attach(&ignoreCallback);                                 ///< Ignore event, handled by the Stand-alone Hempy module

  logToSerials(F("ESP-link ready"), true, 1);
}

/**
  \brief Sets up the MQTT relay
*/
void setupMqtt()
{

  MqttAPI.connectedCb.attach(mqttConnected);
  MqttAPI.disconnectedCb.attach(mqttDisconnected);
  MqttAPI.publishedCb.attach(mqttPublished);
  MqttAPI.dataCb.attach(mqttReceived);

  memset(&ShortMessage[0], 0, MaxShotTextLength); // reset variable to store the Publish to path
  strcat(ShortMessage, MqttLwtTopic);
  MqttAPI.lwt(ShortMessage, MqttLwtMessage, 0, 1); //(topic,message,qos,retain) declares what message should be sent on it's behalf by the broker after Gbox420 has gone offline.
  MqttAPI.setup();
}

void mqttConnected(__attribute__((unused)) void *response)
{
  MqttAPI.subscribe(MqttSubTopic);
  MqttConnected = true;
  // if(Debug) logToSerials(F("MQTT connected"), true);
}

void mqttDisconnected(__attribute__((unused)) void *response)
{
  // if(Debug) logToSerials(F("MQTT disconnected"), true);
  MqttConnected = false;
}

void mqttPublished(__attribute__((unused)) void *response)
{
  // if(Debug) logToSerials(F("MQTT published"), true);
}

void mqttReceived(void *response)
{
  static uint8_t MqttSubTopicLength = strlen(MqttSubTopic) - 1; // Get length of the command topic
  static char command[MaxShotTextLength];
  static char data[MaxShotTextLength];
  ELClientResponse *res = (ELClientResponse *)response;
  String mqttTopic = (*res).popString();
  String mqttData = (*res).popString();
  logToSerials(F("MQTT"), false, 0);
  logToSerials(&mqttTopic, false, 1);
  mqttTopic.remove(0, MqttSubTopicLength); // Cut the known command topic from the arrived topic
  mqttTopic.toCharArray(command, MaxShotTextLength);
  mqttData.toCharArray(data, MaxShotTextLength);
  Main1->commandEventTrigger(command, data);
  Main1->reportToMqttTrigger(true); // send out a fresh report
}

static bool SyncInProgress = false; ///< True if an time sync is in progress

/**
  \brief Update the time over ESP-link using NTP (Network Time Protocol)
*/
time_t getNtpTime()
{
  time_t NTPResponse = 0;
  if (!SyncInProgress)
  { // block calling the sync again inside an interrupt while already waiting for a sync to finish
    SyncInProgress = true;
    uint32_t LastRefresh = millis();
    logToSerials(F("Waiting for NTP time"), false, 0);
    while (NTPResponse == 0 && millis() - LastRefresh < 15000)
    {
      NTPResponse = ESPCmd.GetTime();
      delay(1000);
      logToSerials(F("."), false, 0);
      wdt_reset(); /// reset watchdog timeout
    }
    SyncInProgress = false;
    if (NTPResponse == 0)
    {
      logToSerials(F("sync failed"), true, 3);
    }
    else
      logToSerials(F("synchronized"), true, 3);
  }
  return NTPResponse;
}

/**
  \brief Called when a website is loading on the ESP-link webserver
  \param Url - HTML filename that is getting loaded
*/
void loadCallback(__attribute__((unused)) char *Url)
{
  Main1->websiteLoadEventTrigger(Url); // Runs through all components that are subscribed to this event
}

/**
  \brief Called when a website is refreshing on the ESP-link webserver
  \param Url - HTML filename that is refreshinging
*/
void refreshCallback(__attribute__((unused)) char *Url)
{
  Main1->websiteRefreshEventTrigger(Url);
}

/**
  \brief Called when a button is pressed.
  \param Button - ID of the button HTML element
*/
void buttonCallback(char *Button)
{
  logToSerials(F("ESP button:"), false, 0);
  if (strcmp_P(Button, (PGM_P)F("RestoreDef")) == 0)
  {
    restoreDefaults();
  }
  else
  {
    Main1->commandEventTrigger(Button, NULL);
  }
  saveSettings(ModuleSettings);
}

/**
  \brief Called when a field on the website is submitted
  \param Field - Name of the input HTML element
*/
void fieldCallback(char *Field)
{ ///< Called when any field on the website is updated.
  logToSerials(F("ESP field:"), false, 0);
  Main1->commandEventTrigger(Field, WebServer.getArgString());
  saveSettings(ModuleSettings);
}

/**
  \brief Called when the /Settings.html website is loading on the ESP-link webserver
*/
void settingsLoadCallback(__attribute__((unused)) char *Url)
{
  Main1->settingsEvent_Load(Url); // Runs through all components that are subscribed to this event
}

/**
  \brief Called when the /Settings.html website is refreshing on the ESP-link webserver
*/
void settingsRefreshCallback(__attribute__((unused)) char *Url)
{
  Main1->settingsEvent_Refresh(Url);
}

/**
  \brief Called when a button is pressed on the /Settings.html web page.
  \param Button - ID of the button HTML element
*/
void settingsButtonCallback(char *Button)
{
  if (Debug)
  {
    logToSerials(F("Settings button:"), false, 0);
    logToSerials(&Button, true, 1);
  }
  if (strcmp_P(Button, (PGM_P)F("RestoreDef")) == 0)
  {
    restoreDefaults();
  }
  else
  {
    Main1->settingsEvent_Command(Button, NULL);
  }
  saveSettings(ModuleSettings);
}

/**
  \brief Called when a field on the /Settings.html website is submitted
  \param Field - Name of the input HTML element
*/
void settingsFieldCallback(char *Field)
{
  if (Debug)
  {
    logToSerials(F("Settings field:"), false, 0);
    logToSerials(&Field, true, 1);
  }
  Main1->settingsEvent_Command(Field, WebServer.getArgString());
  saveSettings(ModuleSettings);
}

/**
  \brief Prints the nRF24L01+ wireless transceiver's status to the Serial log
*/
void getWirelessStatus()
{
  if (Debug)
  {
    logToSerials(F("Wireless report:"), true, 0);
    Wireless.printPrettyDetails();
    logToSerials(F(""), true, 0);
  }
}

/**
  \brief Ignores the incoming load/refresh event. Used when embedding another module's web interface that already handles the event
*/
void ignoreCallback(__attribute__((unused)) char *Url)
{
}