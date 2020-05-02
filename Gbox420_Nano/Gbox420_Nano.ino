/**@file*/
///GrowBoxGuy - http:///sites.google.com/site/growboxguy/
///This is currently under development
///Gbox420 Nano is a stripped down version of Gbox420 without a web interface support
///Runs autonomously on an Arduino Nano RF and [WILL] support wireless connection towards the main module

#include "Arduino.h"
#include "avr/wdt.h"                ///Watchdog timer for detecting a crash and automatically resetting the board
#include "avr/boot.h"               ///Watchdog timer related bug fix
#include "printf.h"
#include "Thread.h"                 ///Splitting functions to threads for timing
#include "StaticThreadController.h" ///Grouping threads           
#include "SPI.h" ///allows you to communicate with SPI devices, with the Arduino as the master device
#include "nRF24L01.h"   ///https://forum.arduino.cc/index.php?topic=421081
#include "RF24.h"       ///https://github.com/maniacbug/RF24
#include "SerialLog.h"  ///Logging debug messages to Serial            
#include "Settings.h"  
#include "src/Modules/HempyModule.h"
#include "src/WirelessCommands_Hempy.h"   ///Structs for wireless communication via the nRF24L01 chip, defines the messages exchanged with the main modul 


///Global variable initialization
char LongMessage[MaxLongTextLength] = "";  ///temp storage for assembling long messages (REST API, MQTT API)
char ShortMessage[MaxShotTextLength] = ""; ///temp storage for assembling short messages (Log entries, Error messages)char CurrentTime[MaxTextLength] = "";      ///buffer for storing current time in text
char CurrentTime[MaxTextLength] = "";      ///buffer for storing current time in text
struct hempyCommand Command;  //Variable where the wireless command values will get stored
struct hempyResponse Response;  ///Response sent back in the Acknowledgement after receiving a command from the Transmitter

///Component initialization
HardwareSerial &ArduinoSerial = Serial;   ///Reference to the Arduino Serial
Settings * ModuleSettings;                ///settings loaded from the EEPROM. Persistent between reboots, defaults are in Settings.h
bool *Debug;
bool *Metric;
HempyModule *HempyMod1;                   ///Represents a Hempy bucket with weight sensors and pumps
RF24 Wireless(10, 9); /// Initialize the NRF24L01 wireless chip (CE, CSN pins are hard wired on the Arduino Nano RF)

///Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
Thread QuarterHourThread = Thread();
StaticThreadController<4> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread, &QuarterHourThread);

Settings *loadSettings(bool ResetEEPROM = false )   ///if the function contains arguments with default values, they must be declared strictly before they are called, otherwise there is a compilation error: '<function name> was not declared in this scope. https://forum.arduino.cc/index.php?topic=606678.0
{
  Settings *DefaultSettings = new Settings();                                    ///This is where settings are stored, first it takes the sketch default settings defined in Settings.h
  Settings EEPROMSettings;                                                       ///temporary storage with "Settings" type
  eeprom_read_block((void *)&EEPROMSettings, (void *)0, sizeof(Settings)); ///Load EEPROM stored settings into EEPROMSettings
  if (DefaultSettings->CompatibilityVersion != EEPROMSettings.CompatibilityVersion || ResetEEPROM)
  { ///Making sure the EEPROM loaded settings are compatible with the sketch
    logToSerials(F("Incompatible stored settings detected, updating EEPROM..."), false, 0);
    saveSettings(DefaultSettings); ///overwrites EEPROM stored settings with defaults from this sketch
  }
  else
  {
    logToSerials(F("Same settings version detected, applying EEPROM settings..."), false, 0);
    ///DefaultSettings = EEPROMSettings; ///overwrite sketch defaults with loaded settings
    memcpy(DefaultSettings, &EEPROMSettings, sizeof(Settings));
  }
  logToSerials(F("done"), true, 1);
  return DefaultSettings;
}

void setup()
{                                                      /// put your setup code here, to run once:
  ArduinoSerial.begin(115200);                         ///Nano console output
  printf_begin();
  logToSerials(F(""), true, 0);                         ///New line
  logToSerials(F("Arduino Nano RF initializing..."), true, 0); ///logs to the Arduino serial, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https:///gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                 ///Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                   ///fix watchdog not loading sketch after a reset error on Mega2560
  setSyncProvider(updateTime);
  setSyncInterval(3600);                               //Sync time every hour with the main module
  

  ///Loading settings from EEPROM
  ModuleSettings = loadSettings();
  Debug = &ModuleSettings ->  Debug;
  Metric = &ModuleSettings ->  Metric;

  ///Setting up wireless module
  Wireless.begin();
  Wireless.setDataRate( RF24_250KBPS );
  //Wireless.setPALevel(RF24_PA_MIN);
  Wireless.enableAckPayload();
  Wireless.openReadingPipe(1, WirelessChannel);  
  Wireless.writeAckPayload(1, &Response, sizeof(Response));
  Wireless.startListening();

  /// Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires 
  OneSecThread.setInterval(1000);  ///1000ms
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  QuarterHourThread.setInterval(900000);
  QuarterHourThread.onRun(runQuarterHour);
 
  ///Create the Hempy bucket object
  HempyMod1 = new HempyModule(F("Hempy1"), &ModuleSettings->HempyMod1); ///This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  logToSerials(F("Setup ready, starting loops:"), true, 0);
}

void loop()
{                      /// put your main code here, to run repeatedly:
  ThreadControl.run(); ///loop only checks if it's time to trigger one of the threads (runSec(), runFiveSec(),runMinute()..etc)
  ///If a control package is received from the main module
  getWirelessData();  
}

///////////////////////////////////////////////////////////////
///Threads

void runSec()
{
  wdt_reset();
  HempyMod1->runSec(); ///Calls the runSec() method in GrowBox.cpp  
}

void runFiveSec()
{
  wdt_reset();
  HempyMod1->runFiveSec();
}

void runMinute()
{
  wdt_reset();
  HempyMod1->runMinute(); 
  getWirelessStatus(); 
}

void runQuarterHour()
{
  wdt_reset();
  HempyMod1->runQuarterHour();
}

///////////////////////////////////////////////////////////////
///Wireless communication

void getWirelessData() {
    if ( Wireless.available() ) { 
        Wireless.read( &Command, sizeof(Command) );
        logToSerials(F("Command received ["),false,0);
        Serial.print(sizeof(Command));  /// \todo print this with logToSerials: Need support for unsigned long
        logToSerials(F("bytes], Response sent"),true,1); 
        
        if(timeStatus() != timeSet)  
        {
          updateTime(); ///Updating internal timer
        }
        HempyMod1 -> processCommand(&Command);       /// \todo: Support Aeroponics Module  
    }
}

void getWirelessStatus(){
  if(Debug){
    logToSerials(F("Wireless status report:"),true,0);
    Wireless.printDetails();
    logToSerials(F(""),true,0);
  }
}

time_t updateTime()
{
  if(Command.Time > 0)
  {
  setTime(Command.Time);
  logToSerials(F("Clock synced with main module"),true,0); 
  }
  else {
  logToSerials(F("Clock out of sync"),true,0); 
  }
  return Command.Time;
}

///////////////////////////////////////////////////////////////
///EEPROM stored Settings related functions

void saveSettings(Settings *SettingsToSave)
{                                                                                 ///do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void *)SettingsToSave, (void *)0, sizeof(Settings)); ///update_block only writes the bytes that changed
}



void restoreDefaults(Settings *SettingsToOverwrite)
{
  logToSerials(F("Restoring settings from sketch defaults..."), false, 0);
  Settings *DefaultSettings = new Settings; ///new "Settings" type objects with sketch defaults
  memcpy(&SettingsToOverwrite, &DefaultSettings, sizeof(SettingsToOverwrite));
  saveSettings(SettingsToOverwrite);
///  GBox->addToLog(F("Default settings restored"), 1);
}
