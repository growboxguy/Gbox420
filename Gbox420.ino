//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for grow box monitoring and controlling

//HELLO, You are probably here looking for the following tab:
//Gbox420Settings.h : Pin assignment, First time setup, Default settings

//TODO:
//Add pump cutout during aero spray in case pressure limit is reached
//HempyBucket controls
//Move metric/imperial selection to Gbox420Settings.h, remove it from the Settings Webpage
//Wireless module, mini Gbox420 on Ardino Nano V3, with  for sensor boxes
//Light sensor is not linear, need a better way to estimate brightness percentage. Readings[10] and calibrate to every 10% , lookup closest 2 calibration rating (TopRange,BottomRange) and do a mapping between them?

#include "Arduino.h"
#include "avr/wdt.h"                //Watchdog timer for detecting a crash and automatically resetting the board
#include "avr/boot.h"               //Watchdog timer related bug fix
#include "TimerThree.h"             //Interrupt handling for webpage
#include "ELClient.h"               //ESP-link
#include "ELClientWebServer.h"      //ESP-link - WebServer API
#include "ELClientCmd.h"            //ESP-link - Get current time from the internet using NTP
#include "ELClientRest.h"           //ESP-link - REST API
#include "Thread.h"                 //Splitting functions to threads for timing
#include "StaticThreadController.h" //Grouping threads
#include "src/Modules/420Common.h"              //Base class where all components inherits from
#include "Gbox420Settings.h"
#include "GrowBox.h"                //Represents the complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

//Global variable initialization
char LongMessage[MaxLongTextLength] = "";  //temp storage for assembling long messages (REST API, MQTT API)
char ShortMessage[MaxShotTextLength] = ""; //temp storage for assembling short messages (Log entries, Error messages)
char CurrentTime[MaxTextLength] = "";      //buffer for storing current time in text

//Component initialization
HardwareSerial &ArduinoSerial = Serial;   //Reference to the Arduino Serial
HardwareSerial &ESPSerial = Serial3;      //Reference to the ESP Link Serial
ELClient ESPLink(&ESPSerial);             //ESP-link. Both SLIP and debug messages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink);    //ESP-link WebServer API
ELClientCmd ESPCmd(&ESPLink);             //ESP-link - Helps getting the current time from the internet using NTP
ELClientRest PushingBoxRestAPI(&ESPLink); //ESP-link REST API
Settings * EEPROMSettings;                //This object will store the settings loaded from the EEPROM. Persistent between reboots.
GrowBox *GBox;                            //Represents a Grow Box with all components (Lights, DHT sensors, Power sensor..etc)

//Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
Thread QuarterHourThread = Thread();
StaticThreadController<4> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread, &QuarterHourThread);

void setup()
{                                                      // put your setup code here, to run once:
  ArduinoSerial.begin(115200);                         //2560mega console output
  ESPSerial.begin(115200);                             //ESP WiFi console output
  pinMode(13, OUTPUT);                                 //onboard LED - Heartbeat every second to confirm code is running
  logToSerials(F(""), true, 0);                         //New line
  logToSerials(F("GrowBox initializing..."), true, 0); //logs to both Arduino and ESP serials, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https://gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                 //Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                   //fix watchdog not loading sketch after a reset error on Mega2560

  EEPROMSettings = loadSettings();

  ESPLink.resetCb = &resetWebServer; //Callback subscription: What to do when WiFi reconnects
  resetWebServer();                  //reset the WebServer
  setTime(getNtpTime());             //Get the current time over the internet using NTP

  //Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  QuarterHourThread.setInterval(900000);
  QuarterHourThread.onRun(runQuarterHour);

  //Start interrupts to handle request from ESP-Link firmware
  Timer3.initialize(500); //check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();

  //Create the GrowBox object
  GBox = new GrowBox(F("GBox1"), EEPROMSettings); //This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  //  sendEmailAlert(F("Grow%20box%20(re)started"));
  logToSerials(F("Setup ready, starting loops:"), true, 0);
}

void loop()
{                      // put your main code here, to run repeatedly:
  ThreadControl.run(); //loop only checks if it's time to trigger one of the threads (runSec(), runFiveSec(),runMinute()..etc)
}

void processTimeCriticalStuff()
{
  ESPLink.Process(); //Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage
}

//////////////////////////////////////////
//Threads

void runSec()
{
  wdt_reset();    //reset watchdog timeout
  HeartBeat();    //Blinks built-in led
  GBox->runSec(); //Calls the runSec() method in GrowBox.cpp
}

void runFiveSec()
{
  wdt_reset();
  GBox->runFiveSec();
}

void runMinute()
{
  wdt_reset();
  GBox->runMinute();
}

void runQuarterHour()
{
  wdt_reset();
  GBox->runQuarterHour();
}

void HeartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite(13, ledStatus);
}

//////////////////////////////////////////
//Website related functions

void resetWebServer(void)
{ // Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("(re)Connecting ESP-link.."), false, 0);
  while (!ESPLink.Sync())
  {
    logToSerials(F("."), false, 0);
    delay(500);
  };
  logToSerials(F(""), true, 0); //line break
  if (PushingBoxRestAPI.begin("api.pushingbox.com") == 0)
  {
    logToSerials(F("PushingBox RestAPI ready"), true, 2);
  } //Pre-setup relay to Google Sheets
  else
    logToSerials(F("PushingBox RestAPI failed"), true, 2); //If begin returns a negative number the initialization failed
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json");   //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); //setup handling request from Settings.html
  URLHandler *TestHandler = WebServer.createURLHandler("/Test.html.json");         //setup handling request from Test.html
  GrowBoxHandler->loadCb.attach(&loadCallback);                                    //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback);                              //Called periodically to refresh website content
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback);                           //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback);                            //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);                                   //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback);                             //Called periodically to refresh website content
  SettingsHandler->buttonCb.attach(&buttonPressCallback);                          //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback);                           //Called when a field is changed on the website
  TestHandler->loadCb.attach(&loadCallback);                                       //Called then the website loads initially
  TestHandler->refreshCb.attach(&refreshCallback);                                 //Called periodically to refresh website content
  TestHandler->buttonCb.attach(&buttonPressCallback);                              //Called when a button is pressed on the website
  TestHandler->setFieldCb.attach(&setFieldCallback);                               //Called when a field is changed on the website
  logToSerials(F("ESP-link connected"), true, 0);
}

void loadCallback(__attribute__((unused)) char *Url)
{ //called when website is loaded. Runs through all components that subscribed for this event
  GBox->loadEvent(Url);
}

void refreshCallback(__attribute__((unused)) char *Url)
{ //called when website is refreshed.
  GBox->refreshEvent(Url);
}

void buttonPressCallback(char *Button)
{ //Called when any button on the website is pressed.
  if (strcmp_P(ShortMessage, (PGM_P)F("RestoreDefaults")) == 0)
  {
    restoreDefaults(GBox->BoxSettings);
  }
  else
  {
    GBox->buttonEvent(Button);
  }
  saveSettings(GBox->BoxSettings);
}

void setFieldCallback(char *Field)
{ //Called when any field on the website is updated.
  GBox->setFieldEvent(Field);
  saveSettings(GBox->BoxSettings);
}

//////////////////////////////////////////
//EEPROM stored Settings related functions

void saveSettings(Settings *SettingsToSave)
{                                                                                 //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void *)SettingsToSave, (void *)0, sizeof(Settings)); //update_block only writes the bytes that changed
}

Settings *loadSettings()
{
  Settings *DefaultSettings = new Settings();                                    //This is where settings are stored, first it takes the sketch default settings defined in Gbox420Settings.h
  Settings EEPROMSettings;                                                       //temporary storage with "Settings" type
  eeprom_read_block((void *)&EEPROMSettings, (void *)0, sizeof(EEPROMSettings)); //Load EEPROM stored settings into EEPROMSettings
  if (DefaultSettings->CompatibilityVersion != EEPROMSettings.CompatibilityVersion)
  { //Making sure the EEPROM loaded settings are compatible with the sketch
    logToSerials(F("Incompatible stored settings detected, updating EEPROM..."), false, 0);
    saveSettings(DefaultSettings); //overwrites EEPROM stored settings with defaults from this sketch
  }
  else
  {
    logToSerials(F("Same settings version detected, applying EEPROM settings..."), false, 0);
    //DefaultSettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
    memcpy(DefaultSettings, &EEPROMSettings, sizeof(Settings));
  }
  logToSerials(F("done"), true, 1);
  return DefaultSettings;
}

void restoreDefaults(Settings *SettingsToOverwrite)
{
  logToSerials(F("Restoring settings from sketch defaults..."), false, 0);
  Settings DefaultSettings; //new "Settings" type objects with sketch defaults
  memcpy(&SettingsToOverwrite, &DefaultSettings, sizeof(SettingsToOverwrite));
  saveSettings(SettingsToOverwrite);
  GBox->addToLog(F("Default settings restored"), 1);
}