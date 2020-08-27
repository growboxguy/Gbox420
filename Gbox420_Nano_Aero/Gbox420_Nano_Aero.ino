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
#include "src/Modules/AeroModule.h"
#include "src/WirelessCommands_Aero.h"   ///Structs for wireless communication via the nRF24L01 chip, defines the messages exchanged with the main modul 

///Global variable initialization
char LongMessage[MaxLongTextLength] = "";  ///temp storage for assembling long messages (REST API, MQTT API)
char ShortMessage[MaxShotTextLength] = ""; ///temp storage for assembling short messages (Log entries, Error messages)char CurrentTime[MaxTextLength] = "";      ///buffer for storing current time in text
char CurrentTime[MaxTextLength] = "";      ///buffer for storing current time in text
void* ReceivedMessage = malloc(WirelessPayloadSize); ///< Stores a pointer to the latest received data. A void pointer is a pointer that has no associated data type with it. A void pointer can hold address of any type and can be typcasted to any type. Malloc allocates a fixed size memory section and returns the address of it.

///Component initialization
HardwareSerial &ArduinoSerial = Serial;   ///Reference to the Arduino Serial
Settings * ModuleSettings;                ///settings loaded from the EEPROM. Persistent between reboots, defaults are in Settings.h
bool *Debug;
bool *Metric;
AeroModule *AeroMod1;                   ///Represents a Aeroponics tote with solenoid,pressure pump..etc
RF24 Wireless(WirelessCEPin, WirelessCSNPin); /// Initialize the NRF24L01 wireless chip (CE, CSN pins are hard wired on the Arduino Nano RF)

///Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
Thread QuarterHourThread = Thread();
StaticThreadController<4> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread, &QuarterHourThread);

void setup()
{                                                      /// put your setup code here, to run once:
  ArduinoSerial.begin(115200);                         ///Nano console output
  pinMode(13, OUTPUT);                        ///< onboard LED - Heartbeat every second to confirm code is running
  printf_begin();
  logToSerials(F(""), true, 0);                         ///New line
  logToSerials(F("Aeroponics module initializing..."), true, 0); ///logs to the Arduino serial, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https:///gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                 ///Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                   ///fix watchdog not loading sketch after a reset error on Mega2560
  struct AeroModuleCommand BlankCommand = {AeroMessages::AeroModuleCommand1};
  memcpy(ReceivedMessage, &BlankCommand, sizeof(struct AeroModuleCommand)); //< Copy a blank command to the memory block pointed ReceivedMessage. Without this ReceivedMessage would contain random data 
  setSyncProvider(updateTime);
  setSyncInterval(3600);                               //Sync time every hour with the main module
  
  ///Loading settings from EEPROM
  ModuleSettings = loadSettings();
  Debug = &ModuleSettings ->  Debug;
  Metric = &ModuleSettings ->  Metric;

  ///Setting up wireless module
  Wireless.begin();
  Wireless.setDataRate( RF24_250KBPS );  ///< Set the speed to slow - has longer range + No need for faster transmission, Other options: RF24_2MBPS, RF24_1MBPS
  Wireless.setCRCLength(RF24_CRC_8);  /// RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
  Wireless.setPALevel(RF24_PA_MAX);  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
  Wireless.setPayloadSize(WirelessPayloadSize);  ///The number of bytes in the payload. This implementation uses a fixed payload size for all transmissions
  Wireless.enableAckPayload();
  Wireless.openReadingPipe(1, WirelessChannel);    
  Wireless.startListening();
  //Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
  //Wireless.flush_rx();  ///< Dump all previously received messages from the RX FIFO buffer (Max 3 are saved)

  /// Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires 
  OneSecThread.setInterval(1000);  ///1000ms
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  QuarterHourThread.setInterval(900000);
  QuarterHourThread.onRun(runQuarterHour);
 
  ///Create the Aeroponics object
  AeroMod1 = new AeroModule(F("Aero1"), &ModuleSettings->AeroModule1); ///This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

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
  HeartBeat();    ///< Blinks built-in led
  AeroMod1->runSec(); ///Calls the runSec() method in GrowBox.cpp  
}

void runFiveSec()
{
  wdt_reset();  
  AeroMod1->runFiveSec();
}

void runMinute()
{
  wdt_reset();
  AeroMod1->runMinute(); 
  getWirelessStatus(); 
}

void runQuarterHour()
{
  wdt_reset();
  AeroMod1->runQuarterHour();
}


void HeartBeat()
{  
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite(LED_BUILTIN, ledStatus);
}

///////////////////////////////////////////////////////////////
///Wireless communication

void getWirelessData() {
    if ( Wireless.available() ) { 
        if(*Debug)logToSerials(F("Wireless Command received"),true,0);
        Wireless.read( ReceivedMessage, WirelessPayloadSize );        
        if(timeStatus() != timeSet && ((AeroCommonTemplate*)ReceivedMessage) -> SequenceID == AeroMessages::AeroModuleCommand1)  
        {
          updateTime(); ///Updating internal timer
        }
        AeroMod1 -> processCommand(ReceivedMessage); 
    }
}

void getWirelessStatus(){
  if(*Debug){
    logToSerials(F("Wireless status report:"),true,0);
    Wireless.printDetails();
    logToSerials(F(""),true,0);
  }
}

time_t updateTime()
{
  time_t ReceivedTime = ((AeroModuleCommand*)ReceivedMessage) -> Time;
  if(ReceivedTime > 0)
  {
    setTime(ReceivedTime);
    logToSerials(F("Clock synced with Main module"),true,0); 
  }
  else {
    logToSerials(F("Clock out of sync"),true,0); 
  }
  return ReceivedTime;
}