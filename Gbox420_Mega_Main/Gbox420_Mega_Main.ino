/**@file*/
// GrowBoxGuy - http:///< sites.google.com/site/growboxguy/
// Sketch for grow box monitoring and controlling

///< HELLO, You are probably here looking for the following tab:
///< Settings.h : Pin assignment, First time setup, Default settings

///< \todo Proper doxygen documentation
///< \todo Add pump cutout during aero spray in case pressure limit is reached
///< \todo Light sensor is not linear, need a better way to estimate brightness percentage. Readings[10] and calibrate to every 10% , lookup closest 2 calibration rating (TopRange,BottomRange) and do a mapping between them?
///< \todo Display module online/offline status, timeout to offline if no response is received in X minutes

#include "Arduino.h"
#include "avr/wdt.h"                          ///< Watchdog timer for detecting a crash and automatically resetting the board
#include "avr/boot.h"                         ///< Watchdog timer related bug fix
#include "printf.h"                           ///< Printing the wireless status message from nRF24L01
#include "TimerThree.h"                       ///< Interrupt handling for webpage
#include "ELClient.h"                         ///< ESP-link
#include "ELClientWebServer.h"                ///< ESP-link - WebServer API
#include "ELClientCmd.h"                      ///< ESP-link - Get current time from the internet using NTP
#include "ELClientRest.h"                     ///< ESP-link - REST API
#include "Thread.h"                           ///< Splitting functions to threads for timing
#include "StaticThreadController.h"           ///< Grouping threads
#include "SerialLog.h"                        ///< Logging to the Serial console and to ESP-link's console
#include "src/Components_Web/420Common_Web.h" ///< Base class where all web components inherits from
#include "Settings.h"                         ///< EEPROM stored settings for every component
#include "src/Modules_Web/MainModule_Web.h"   ///< Represents a complete box with all feautres
#include "SPI.h"                              ///< allows you to communicate with SPI devices, with the Arduino as the master device
#include "nRF24L01.h"                         ///< https://forum.arduino.cc/index.php?topic=421081
#include "RF24.h"                             ///< https://github.com/maniacbug/RF24

//Global variable initialization
char LongMessage[MaxLongTextLength] = "";  ///< Temp storage for assembling long messages (REST API - Google Sheets reporting)
char ShortMessage[MaxShotTextLength] = ""; ///< Temp storage for assembling short messages (Log entries, Error messages)
char CurrentTime[MaxWordLength] = "";      ///< Buffer for storing current time in text format

//Component initialization
HardwareSerial &ArduinoSerial = Serial;   ///< Reference to the Arduino Serial output
HardwareSerial &ESPSerial = Serial3;      ///< Reference to the ESP Link Serial output
ELClient ESPLink(&ESPSerial);             ///< ESP-link. Both SLIP and debug messages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink);    ///< ESP-link WebServer API
ELClientCmd ESPCmd(&ESPLink);             ///< ESP-link - Helps getting the current time from the internet using NTP
ELClientRest PushingBoxRestAPI(&ESPLink); ///< ESP-link REST API
Settings *ModuleSettings;                 ///< This object will store the settings loaded from the EEPROM. Persistent between reboots.
bool *Debug;
bool *Metric;
MainModule *Main1; ///< Represents a Grow Box with all components (Lights, DHT sensors, Power sensor..etc)

RF24 Wireless(WirelessCEPin, WirelessCSNPin); ///< Wireless communication with Modules over nRF24L01+

// Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
Thread QuarterHourThread = Thread();
StaticThreadController<4> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread, &QuarterHourThread);

void setup()
{                               ///<  put your setup code here, to run once:
  ArduinoSerial.begin(115200);  ///< 2560mega console output
  ESPSerial.begin(115200);      ///< ESP WiFi console output
  pinMode(LED_BUILTIN, OUTPUT); ///< onboard LED - Heartbeat every second to confirm code is running
  printf_begin();
  logToSerials(F(""), true, 0);                            ///< New line
  logToSerials(F("Main module initializing..."), true, 0); ///< logs to both Arduino and ESP serials, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https:///< gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                     ///< Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                       ///< fix watchdog not loading sketch after a reset error on Mega2560

  // Loading settings from EEPROM
  logToSerials(F("Loading settings..."), true, 0);
  ModuleSettings = loadSettings();
  Debug = &ModuleSettings->Debug;
  Metric = &ModuleSettings->Metric;

  logToSerials(F("Setting up ESP-link connection..."), true, 0);
  ESPLink.resetCb = &resetWebServer; ///< Callback subscription: What to do when WiFi reconnects
  resetWebServer();                  ///< reset the WebServer
  setSyncProvider(getNtpTime);       ///< Points to method for updating time from NTP server
  setSyncInterval(86400);            ///< Sync time every day

  // Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  logToSerials(F("Setting up refresh threads..."), false, 0);
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  QuarterHourThread.setInterval(900000);
  QuarterHourThread.onRun(runQuarterHour);
  logToSerials(F("done"), true, 1);

  // Start interrupts to handle request from ESP-Link firmware
  logToSerials(F("Setting up interrupt handler..."), false, 0);
  Timer3.initialize(500); ///< check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();
  logToSerials(F("done"), true, 1);

  //Initialize wireless communication with Modules
  logToSerials(F("Setting up wireless transceiver..."), false, 0);
  Wireless.begin();                                  ///< Initialize the nRF24L01+ wireless chip for talking to Modules
  Wireless.setDataRate(RF24_250KBPS);                ///< Set the speed to slow - has longer range + No need for faster transmission, Other options: RF24_2MBPS, RF24_1MBPS
  Wireless.setCRCLength(RF24_CRC_8);                 ///< RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
  Wireless.setPALevel(RF24_PA_MAX);                  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
  Wireless.setPayloadSize(WirelessPayloadSize);      ///< The number of bytes in the payload. This implementation uses a fixed payload size for all transmissions
  Wireless.enableAckPayload();                       ///< When sending a wireless package, expect a response confirming the package was received in a custom Acknowledgement package
  Wireless.setRetries(WirelessDelay, WirelessRetry); ///< Defined in Settings.h. How many retries before giving up sending a single package and How long to wait between each retry
  logToSerials(F("done"), true, 1);

  // Create the Module objects
  logToSerials(F("Creating main module..."), true, 0);
  Main1 = new MainModule(F("Main1"), &ModuleSettings->Main1, &Wireless); ///< This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  //   sendEmailAlert(F("Grow%20box%20(re)started"));
  logToSerials(F("Setup ready, starting loops:"), true, 0);
}

void loop()
{
  ThreadControl.run(); ///< loop only checks if it's time to trigger one of the threads (runSec(), runFiveSec(),runMinute()..etc)
}

void processTimeCriticalStuff()
{
  ESPLink.Process(); ///< Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage
}

// Threads

void runSec()
{
  wdt_reset();     ///< reset watchdog timeout
  HeartBeat();     ///< Blinks built-in led
  Main1->runSec(); ///< Calls the runSec() method in GrowBox.cpp
}

void runFiveSec()
{
  wdt_reset();
  Main1->runFiveSec();
}

void runMinute()
{
  wdt_reset();
  Main1->runMinute();
  getWirelessStatus();
}

void runQuarterHour()
{
  wdt_reset();
  Main1->runQuarterHour();
}

void HeartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite(LED_BUILTIN, ledStatus);
}

// Website related functions

void resetWebServer(void)
{ ///<  Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("(re)Connecting ESP-link..."), false, 1);
  while (!ESPLink.Sync())
  {
    logToSerials(F("."), false, 0);
    delay(500);
  };
  logToSerials(F(""), true, 0);                           ///< line break
  if (PushingBoxRestAPI.begin("api.pushingbox.com") == 0) ///< Pre-setup relay to Google Sheets
  {
    logToSerials(F("PushingBox RestAPI ready"), true, 2);
  }
  else
    logToSerials(F("PushingBox RestAPI failed"), true, 2); ///< If begin returns a negative number the initialization failed
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json");   ///< setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); ///< setup handling request from Settings.html
  URLHandler *TestHandler = WebServer.createURLHandler("/Test.html.json");         ///< setup handling request from Test.html
  GrowBoxHandler->loadCb.attach(&loadCallback);                                    ///< GrowBox tab - Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback);                              ///< GrowBox tab - Called periodically to refresh website content
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback);                           ///< GrowBox tab - Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback);                            ///< GrowBox tab - Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);                                   ///< Settings tab - Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback);                             ///< Settings tab - Called periodically to refresh website content
  SettingsHandler->buttonCb.attach(&buttonPressCallback);                          ///< Settings tab - Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback);                           ///< Settings tab - Called when a field is changed on the website
  TestHandler->loadCb.attach(&loadCallback);                                       ///< Test tab - Called then the website loads initially
  TestHandler->refreshCb.attach(&refreshCallback);                                 ///< Test tab - Called periodically to refresh website content
  TestHandler->buttonCb.attach(&buttonPressCallback);                              ///< Test tab - Called when a button is pressed on the website
  TestHandler->setFieldCb.attach(&setFieldCallback);                               ///< Test tab - Called when a field is changed on the website
  logToSerials(F("ESP-link ready"), true, 2);
}

// Time

static bool SyncInProgress = false;
time_t getNtpTime()
{
  time_t NTPResponse = 0;
  if (!SyncInProgress)
  { ///< blocking calling the sync again in an interrupt
    SyncInProgress = true;
    uint32_t LastRefresh = millis();
    logToSerials(F("Waiting for NTP time..."), false, 0);
    while (NTPResponse == 0 && millis() - LastRefresh < 15000)
    {
      NTPResponse = ESPCmd.GetTime();
      delay(500);
      logToSerials(F("."), false, 0);
      wdt_reset(); ///< reset watchdog timeout
    }
    SyncInProgress = false;
    if (NTPResponse == 0)
    {
      logToSerials(F("NTP time sync failed"), true, 1);
      ///< sendEmailAlert(F("NTP%20time%20sync%20failed"));
    }
    else
      logToSerials(F("time synchronized"), true, 1);
  }
  return NTPResponse;
}

void loadCallback(__attribute__((unused)) char *Url)
{ ///< called when website is loaded. Runs through all components that subscribed for this event
  Main1->loadEvent(Url);
}

void refreshCallback(__attribute__((unused)) char *Url)
{ ///< called when website is refreshed.
  Main1->refreshEvent(Url);
}

void buttonPressCallback(char *Button)
{ ///< Called when any button on the website is pressed.
  if (strcmp_P(Button, (PGM_P)F("RestoreDefaults")) == 0)
  {
    restoreDefaults();
  }
  else
  {
    Main1->buttonEvent(Button);
  }
  saveSettings(ModuleSettings);
}

void setFieldCallback(char *Field)
{ ///< Called when any field on the website is updated.
  Main1->setFieldEvent(Field);
  saveSettings(ModuleSettings);
}

void getWirelessStatus()
{
  if (*Debug)
  {
    logToSerials(F("Wireless status report:"), true, 0);
    Wireless.printDetails();
    logToSerials(F(""), true, 0);
  }
}