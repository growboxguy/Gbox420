//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for grow box monitoring and controlling
//UNDER DEVELOPMENT - This is the next generation of Gbox420 rewritten to depend fully on the web interface for control and feedback. 

//HELLO, You are probably here looking for the following tab:
// 420Settings.h : Pin assignment, First time setup, Default settings 


//TODO:
//Make alerting a class, every object will have a "bool StatusOK" variable inherited from Common, Alerting will go throught all subscribed classes and check status
//sendEmailAlert implementation
//HempyBucket controls
//Aeroponics
//PH sensor
//Water temp sensor
//Water level sensor
//Google Sheets reporting
//MQTT reporting
//Support for storing multiple Settings objects in EEPROM (?Support for controlling more grow boxes from a single arduino? )
//Add DebugEnabled to all classes, sets debug mode on-off per component
//Add debug message levels: 0-no debug, 1-Extra info, 2-Message, 3-Verbose
//Find a better name for Aeroponics NoTank: DirectPump?
//Aeroponics_Tank1 -> websiteEvent_Field and Aeroponics_Tank1 -> websiteButtonPress events cause a crash
//Remove WaterTempSensor and Reservoir classes from PHSensor


#include "Arduino.h" 
#include "avr/wdt.h" //Watchdog timer
#include "avr/boot.h" //Watchdog timer related bug fix
#include "TimerThree.h"  //Interrupt handling for webpage
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "Thread.h" //Splitting functions to threads for timing
#include "StaticThreadController.h"  //Grouping threads
#include "420Common.h"  //Base class where plugins inherits from
#include "GrowBox.h" //Represents a complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

//Global variables
//Settings BoxSettings;   //Storeses every setting of the grow box. Written to EEPROM to keep settings between reboots 
char LongMessage[512];   //temp storage for assembling log LongMessages, buffer for REST and MQTT API LongMessages
char ShortMessage[MaxTextLength];
char CurrentTime[20]; //buffer for storing current time

//Component initialization
HardwareSerial& ArduinoSerial = Serial;  //Reference to the Arduino Serial
HardwareSerial& ESPSerial = Serial3;    //Reference to the ESP Link Serial
ELClient ESPLink(&ESPSerial);  //ESP-link. Both SLIP and debug LongMessages are sent to ESP over the ESP Serial link
ELClientWebServer WebServer(&ESPLink); //ESP-link WebServer API
ELClientCmd ESPCmd(&ESPLink);//ESP-link - Get current time from the internet using NTP
GrowBox * GBox;  //Represents a Grow Box with all components (Lights, DHT sensors, Power sensor..etc)

//Threading to time tasks
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
Thread HalfHourThread = Thread();
StaticThreadController<4> ThreadControl (&OneSecThread,&FiveSecThread,&MinuteThread,&HalfHourThread);

void setup() {  // put your setup code here, to run once:
  ArduinoSerial.begin(115200);    //2560mega console output
  ESPSerial.begin(115200);  //esp WiFi console output
  pinMode(13, OUTPUT); //onboard LED - Heartbeat every second to confirm code is running
  logToSerials(F("GrowBox initializing..."),true,0); //logs to both Arduino and ESP serials, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https://gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S); //Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable(); //fix watchdog not loading sketch after a reset error on Mega2560  
  GBox = new GrowBox(F("GBox1"), loadSettings());  //This is the main object representing an entire Grow Box with all components in it.  
  ESPLink.resetCb = &resetWebServer;  //Callback subscription: When WiFi reconnects, restart the WebServer
  resetWebServer();  //reset the WebServer 
  setTime(getNtpTime()); 
  
  //Threading - Setting up threads and setting how often they should be called
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  HalfHourThread.setInterval(1800000);
  HalfHourThread.onRun(runHalfHour);
  
   //Start interrupts to handle request from ESP-Link firmware
  Timer3.initialize(500);  //check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();

  GBox -> refreshAll();
  //  sendEmailAlert(F("Grow%20box%20(re)started"));
  logToSerials(F("Setup ready, starting loops:"),true,0);
}

void loop() { // put your main code here, to run repeatedly:
 ThreadControl.run();    //loop only checks if it's time to trigger one of the threads (runSec(), runFiveSec(),runMinute()..etc)
}

void processTimeCriticalStuff(){
  ESPLink.Process();  //Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage  
}

//////////////////////////////////////////
//Threads

void runSec(){
  wdt_reset(); //reset watchdog timeout    
  HeartBeat(); //Blinks built-in led
  GBox -> runSec();
}

void runFiveSec(){
  wdt_reset();      
  GBox -> runFiveSec();
  getFreeMemory();
}

void runMinute(){
  wdt_reset(); 
  GBox -> runMinute();
}

void runHalfHour(){
  wdt_reset();   
  GBox -> runHalfHour();
}

void HeartBeat(){
  static bool pulseHigh;
  pulseHigh = !pulseHigh;
  digitalWrite(13, pulseHigh);
}

//////////////////////////////////////////
//Settings related functions

void saveSettings(bool LogThis, Settings * SettingsToSave){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)SettingsToSave, (void*)0, sizeof(Settings)); //update_block only writes the bytes that changed
  if(LogThis) logToSerials(F("Settings saved to EEPROM"),true);
}

Settings* loadSettings(){
  Settings* DefaultSettings = new Settings();  //This is where settings get are stored, first it takes the sketch default settings defined in 420Settings.h
  Settings EEPROMSettings; //temporary storage with "Settings" type
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));   //Load EEPROM stored settings into EEPROMSettings
  if(DefaultSettings -> CompatibilityVersion != EEPROMSettings.CompatibilityVersion){  //Making sure the EEPROM loaded settings are compatible with the sketch
    logToSerials(F("Incompatible stored settings detected, updating EEPROM..."),false);
    saveSettings(false,DefaultSettings);  //overwrites stored settings with defaults from this sketch
  }
  else {
    logToSerials(F("Same settings version detected, applying EEPROM settings..."),false,1);
    //DefaultSettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
    memcpy(DefaultSettings,&EEPROMSettings,sizeof(Settings)); 
  }
  logToSerials(F("done"),true);
  return DefaultSettings;
}

void restoreDefaults(Settings* SettingsToOverwrite){
  logToSerials(F("Restoring settings from sketch defaults..."),false,0);
  Settings DefaultSettings; //new "Settings" type objects with sketch defaults
  memcpy(&SettingsToOverwrite,&DefaultSettings,sizeof(Settings));  
  saveSettings(false,SettingsToOverwrite );
  GBox -> addToLog(F("Default settings restored"));
}