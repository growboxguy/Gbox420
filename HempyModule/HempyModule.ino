//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Hempy bucket module for Arduino Nano


#include "Arduino.h"
#include "Thread.h"                 //Splitting functions to threads for timing
#include "StaticThreadController.h" //Grouping threads
#include "src/420Common.h"              //Base class where all components inherits from

//Global variable initialization
char LongMessage[MaxLongTextLength] = "";  //temp storage for assembling long messages (REST API, MQTT API)
char ShortMessage[MaxShotTextLength] = ""; //temp storage for assembling short messages (Log entries, Error messages)
char CurrentTime[MaxTextLength] = "";      //buffer for storing current time in text

//Component initialization
HardwareSerial &ArduinoSerial = Serial;   //Reference to the Arduino Serial

//Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
StaticThreadController<3> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread);

void setup()
{                                                      // put your setup code here, to run once:
  Serial.begin(115200);                         //2560mega console output
  //pinMode(13, OUTPUT);                                 //onboard LED - Heartbeat every second to confirm code is running
 // logToSerials(F(""), true, 0);                         //New line
  Serial.println("Hempy bucket module initializing..."); //logs to both Arduino and ESP serials, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https://gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  //wdt_enable(WDTO_8S);                                 //Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  //boot_rww_enable();                                   //fix watchdog not loading sketch after a reset error on Mega2560

 // Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
 
  //Create the GrowBox object
  //GBox = new GrowBox(F("GBox1"), loadSettings()); //This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  //  sendEmailAlert(F("Grow%20box%20(re)started"));
  Serial.println("Setup ready, starting loops:");
}

void loop()
{                      // put your main code here, to run repeatedly:
  ThreadControl.run(); //loop only checks if it's time to trigger one of the threads (runSec(), runFiveSec(),runMinute()..etc)
}

void processTimeCriticalStuff()
{

}

//////////////////////////////////////////
//Threads

void runSec()
{
  // HeartBeat();    //Blinks built-in led
  //GBox->runSec(); //Calls the runSec() method in GrowBox.cpp
  Serial.println("sec");
}

void runFiveSec()
{
  //GBox->runFiveSec();
    Serial.println("5sec");
}

void runMinute()
{
  //GBox->runMinute();
    Serial.println("min");
}

void HeartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite(13, ledStatus);
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
//  GBox->addToLog(F("Default settings restored"), 1);
}
