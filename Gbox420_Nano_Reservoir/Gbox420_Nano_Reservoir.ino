/*! \file 
 *  \brief     Reservoir module for Nano
 *  \details   To change the default pin layout / startup settings navigate to: Settings.h
 *  \details   Runs autonomously on an Arduino RF-Nano or Arduino Nano with an nRF24L01+ wireless transceiver.
 *  \details   Supports wireless data exchange with the Main module
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

#include "Arduino.h"
#include "avr/wdt.h"  // Watchdog timer for detecting a crash and automatically resetting the board
#include "avr/boot.h" // Watchdog timer related bug fix
#include "printf.h"
#include "Thread.h"                 // Splitting functions to threads for timing
#include "StaticThreadController.h" // Grouping threads
#include "SPI.h"                    // allows you to communicate with SPI devices, with the Arduino as the master device
#include "RF24.h"                   // https://github.com/maniacbug/RF24
#include "SerialLog.h"              // Logging debug messages to Serial
#include "Settings.h"
#include "src/Modules/ReservoirModule.h"
#include "src/WirelessCommands_Reservoir.h" // Structs for wireless communication via the nRF24L01 chip, defines the messages exchanged with the main modul

// Global variable initialization
bool &Debug = *new bool;   
bool &Metric = *new bool;
char LongMessage[MaxLongTextLength] = "";            // Temp storage for assembling long messages (REST API - Google Sheets reporting)
char ShortMessage[MaxShotTextLength] = "";           // Temp storage for assembling short messages (Log entries, Error messages)
char CurrentTime[MaxWordLength] = "";                // Buffer for storing current time in text format
void *ReceivedMessage = malloc(WirelessPayloadSize); // Stores a pointer to the latest received data. A void pointer is a pointer that has no associated data type with it. A void pointer can hold address of any type and can be typcasted to any type. Malloc allocates a fixed size memory section and returns the address of it.
uint32_t ReceivedMessageTimestamp = millis();        // Stores the timestamp when the last wireless package was received

// Component initialization
HardwareSerial &ArduinoSerial = Serial;       // Reference to the Arduino Serial
Settings *ModuleSettings;                     // settings loaded from the EEPROM. Persistent between reboots, defaults are in Settings.h
ReservoirModule *ReservoirMod1;               // Represents a Reservoir tote with temp,PH,water level sensors
RF24 Wireless(WirelessCEPin, WirelessCSNPin); // Initialize the NRF24L01 wireless chip (CE, CSN pins are hard wired on the Arduino Nano RF)

// Thread initialization
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
StaticThreadController<3> ThreadControl(&OneSecThread, &FiveSecThread, &MinuteThread);

void setup()
{                              // put your setup code here, to run once:
  ArduinoSerial.begin(115200); // Nano console output
  printf_begin();
  logToSerials(F(""), true, 0);                              // New line
  logToSerials(F("Reservoir module initializing"), true, 0); // logs to the Arduino serial, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https:// gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                       // Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                         // fix watchdog not loading sketch after a reset error on Mega2560
  struct ReservoirModuleCommand BlankCommand = {ReservoirMessages::ReservoirModuleCommand1};
  memcpy(ReceivedMessage, &BlankCommand, sizeof(struct ReservoirModuleCommand)); // Copy a blank command to the memory block pointed ReceivedMessage. Without this ReceivedMessage would contain random data
  setSyncProvider(updateTime);
  setSyncInterval(3600); // Sync time every hour with the main module

  // Loading settings from EEPROM
  ModuleSettings = loadSettings();
  Debug = ModuleSettings->Debug;
  Metric = &ModuleSettings->Metric;

  // Setting up wireless module
  InitializeWireless();

  // Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  OneSecThread.setInterval(1000); // 1000ms
  OneSecThread.onRun(run1sec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(run5sec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(run1min);

  // Create the Reservoir Module object
  ReservoirMod1 = new ReservoirModule(F("Res1"), &ModuleSettings->Res1); // This is the main object representing an entire Grow Box with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  logToSerials(F("Setup ready, starting loops:"), true, 0);
}

void InitializeWireless()
{
  logToSerials(F("(re)Initializing wireless transceiver"), false, 0);
  pinMode(WirelessCSNPin, OUTPUT);
  digitalWrite(WirelessCSNPin, HIGH);
  pinMode(WirelessCEPin, OUTPUT);
  digitalWrite(WirelessCEPin, HIGH);
  Wireless.begin();
  Wireless.powerDown();
  Wireless.setDataRate(RF24_250KBPS);           ///< Set the speed to slow - has longer range + No need for faster transmission, Other options: RF24_2MBPS, RF24_1MBPS
  Wireless.setCRCLength(RF24_CRC_16);           ///< RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
  Wireless.setPALevel(RF24_PA_MAX);             ///< RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, and RF24_PA_MAX=0dBm.
  Wireless.setPayloadSize(WirelessPayloadSize); ///< The number of bytes in the payload. This implementation uses a fixed payload size for all transmissions
  Wireless.enableDynamicPayloads();
  Wireless.enableAckPayload();
  Wireless.openReadingPipe(1, WirelessChannel);
  Wireless.startListening();
  Wireless.powerUp();  ///< Not necessary, startListening should switch back to normal power mode
  Wireless.flush_tx(); ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved)
  Wireless.flush_rx(); ///< Dump all previously received messages from the RX FIFO buffer (Max 3 are saved)
  logToSerials(F("done"), true, 3);
  ReceivedMessageTimestamp = millis(); ///< Reset timeout counter
}

void loop()
{                      // put your main code here, to run repeatedly:
  ThreadControl.run(); // loop only checks if it's time to trigger one of the threads (run1sec(), run5sec(),run1min()..etc)
  // If a control package is received from the main module
  getWirelessData();
}

// Threads

void run1sec()
{
  wdt_reset();
  ReservoirMod1->run1sec(); // Calls the run1sec() method in GrowBox.cpp
}

void run5sec()
{
  wdt_reset();
  ReservoirMod1->run5sec();
}

void run1min()
{
  wdt_reset();
  ReservoirMod1->run1min();
  //getWirelessStatus();   //USES TOO MUCH MEMORY!
}

// Wireless communication

void getWirelessData()
{
  if (Wireless.available())
  {
    Wireless.read(ReceivedMessage, WirelessPayloadSize);
    if (timeStatus() != timeSet && ((ReservoirCommonTemplate *)ReceivedMessage)->SequenceID == ReservoirMessages::ReservoirModuleCommand1)
    {
      updateTime(); // Updating internal timer
    }
    if (ReservoirMod1->processCommand(ReceivedMessage))
    {
      ReceivedMessageTimestamp = millis(); //< Reset the timer after the last message was exchanged
    }
  }
  if (millis() - ReceivedMessageTimestamp > WirelessReceiveTimeout)
  {
    InitializeWireless();
  }
}

void getWirelessStatus()
{
  if (Debug)
  {
    logToSerials(F("Wireless report:"), true, 0);
    Wireless.printPrettyDetails();
    logToSerials(F(""), true, 0); // Just a line break after the report
  }
}

time_t updateTime()
{
  time_t ReceivedTime = ((ReservoirModuleCommand *)ReceivedMessage)->Time;
  if (ReceivedTime > 0)
  {
    setTime(ReceivedTime);
    logToSerials(F("Clock synced"), true, 0);
  }
  else
  {
    logToSerials(F("Clock out of sync"), true, 0);
  }
  return ReceivedTime;
}