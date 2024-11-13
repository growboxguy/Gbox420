/*! \file 
 *  \brief     AC motor module for Nano
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
#include "SPI.h"                    ///< communicate with SPI devices, with the Arduino as the master device
//WIRELESS DISBLED// #include "RF24.h"                   // https://github.com/maniacbug/RF24
#include "SerialLog.h" // Logging messages to Serial
#include "Settings.h"
#include "src/Modules/ACMotorModule.h"
//WIRELESS DISBLED// #include "src/WirelessCommands_ACMotor.h" // Structs for wireless communication via the nRF24L01 chip, defines the messages exchanged with the main modul

// Global variable initialization
bool &Debug = *new bool;   
bool &Metric = *new bool;
char LongMessage[MaxLongTextLength] = "";  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
char ShortMessage[MaxShotTextLength] = ""; // Temp storage for assembling short messages (Log entries, Error messages)
char CurrentTime[MaxWordLength] = "";      // Buffer for storing current time in text format
//WIRELESS DISBLED// void *ReceivedMessage = malloc(WirelessPayloadSize); // Stores a pointer to the latest received data. A void pointer is a pointer that has no associated data type with it. A void pointer can hold address of any type and can be typcasted to any type. Malloc allocates a fixed size memory section and returns the address of it.
//WIRELESS DISBLED// uint32_t ReceivedMessageTimestamp = millis();        // Stores the timestamp when the last wireless package was received

///< Component initialization
HardwareSerial &ArduinoSerial = Serial; // Reference to the Arduino Serial
Settings *ModuleSettings;               // settings loaded from the EEPROM. Persistent between reboots, defaults are in Settings.h
ACMotorModule *ACMotorMod1;             // Represents a AC Incuction Motor with direction and speed control
//WIRELESS DISBLED// RF24 Wireless(WirelessCEPin, WirelessCSNPin); // Initialize the NRF24L01 wireless chip (CE, CSN pins are hard wired on the Arduino Nano RF)

///< Thread initialization
Thread TimeCriticalThread = Thread();
Thread OneSecThread = Thread();
Thread FiveSecThread = Thread();
Thread MinuteThread = Thread();
StaticThreadController<4> ThreadControl(&TimeCriticalThread, &OneSecThread, &FiveSecThread, &MinuteThread);

void setup()
{                              // put your setup code here, to run once:
  ArduinoSerial.begin(115200); // Nano console output
  pinMode(LED_BUILTIN, OUTPUT);
  printf_begin();
  logToSerials(F(""), true, 0);                            ///< New line
  logToSerials(F("ACMotor module initializing"), true, 0); ///< logs to the Arduino serial, adds new line after the text (true), and uses no indentation (0). More on why texts are in F(""):  https://gist.github.com/sticilface/e54016485fcccd10950e93ddcd4461a3
  wdt_enable(WDTO_8S);                                     ///< Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds it assumes a lockup and resets the sketch
  boot_rww_enable();                                       ///< fix watchdog not loading sketch after a reset error on Mega2560
  //WIRELESS DISBLED// struct ACMotorModuleCommand BlankCommand = {ACMotorMessages::ACMotorModuleCommand1};
  //WIRELESS DISBLED// memcpy(ReceivedMessage, &BlankCommand, sizeof(struct ACMotorModuleCommand)); ///< Copy a blank command to the memory block pointed ReceivedMessage. Without this ReceivedMessage would contain random data
  //WIRELESS DISBLED// setSyncProvider(updateTime);
  //WIRELESS DISBLED// setSyncInterval(3600); //Sync time every hour with the main module

  ///< Loading settings from EEPROM
  ModuleSettings = loadSettings();
  Debug = ModuleSettings->Debug;
  Metric = &ModuleSettings->Metric;

  ///< Setting up wireless module
  //WIRELESS DISBLED// InitializeWireless();

  ///< Threads - Setting up how often threads should be triggered and what functions to call when the trigger fires
  TimeCriticalThread.setInterval(200); ///< 200ms, 0.2sec
  TimeCriticalThread.onRun(processTimeCriticalStuff);
  OneSecThread.setInterval(1000); ///< 1000ms
  OneSecThread.onRun(run1sec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(run5sec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(run1min);

  ///< Create the ACMotor object
  ACMotorMod1 = new ACMotorModule(F("ACM1"), &ModuleSettings->ACM1); ///< This is the object representing an AC Motor controller with all components in it. Receives its name and the settings loaded from the EEPROM as parameters

  // set up Timer1
  OCR1A = 100;   // initialize the comparator
  TIMSK1 = 0x03; // enable comparator A and overflow interrupts
  TCCR1A = 0x00; // timer control registers set for normal operation, timer disabled
  TCCR1B = 0x00; // timer control registers set for normal operation, timer disabled

  ///< Attach external interrupts
  attachInterrupt(digitalPinToInterrupt(*(ACMotorMod1->Motor1->ZeroCrossingPin)), zeroCrossingInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(*(ACMotorMod1->Motor1->ComparatorPin)), tachoTrigger, RISING);  

  logToSerials(F("Looping.."), true, 0);
}

//WIRELESS DISBLED//
/*
void InitializeWireless()
{
  if (Debug)
  {
   logToSerials(F("(re)Initializing wireless transceiver"), false, 0);
  }
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
  if (Debug)
  {
  logToSerials(F("done"), true, 3);
  }
  ReceivedMessageTimestamp = millis(); ///< Reset timeout counter
}
*/

void loop()
{                      ///< put your main code here, to run repeatedly:
  ThreadControl.run(); ///< loop only checks if it's time to trigger one of the threads (run1sec(), run5sec(),run1min()..etc)
  ///< If a control package is received from the main module
  //getWirelessData();
}

///< Threads

void processTimeCriticalStuff() ///< Process things that need precise timing
{
  ACMotorMod1->processTimeCriticalStuff();
}

void run1sec()
{
  wdt_reset(); ///< reset watchdog timeout
  heartBeat(); ///< Blinks built-in led
  ACMotorMod1->run1sec();
}

void run5sec()
{
  wdt_reset();
  ACMotorMod1->run5sec();
}

void run1min()
{
  wdt_reset();
  ACMotorMod1->run1min();
  //WIRELESS DISBLED// getWirelessStatus();
}

void heartBeat()
{
  static bool ledStatus;
  ledStatus = !ledStatus;
  digitalWrite(LED_BUILTIN, ledStatus);
}

///< Wireless communication
//WIRELESS DISBLED//
/*
void getWirelessData()
{
  if (Wireless.available())
  {
    Wireless.read(ReceivedMessage, WirelessPayloadSize);
    if (timeStatus() != timeSet && ((ACMotorCommonTemplate *)ReceivedMessage)->SequenceID == ACMotorMessages::ACMotorModuleCommand1)
    {
      updateTime(); ///< Updating internal timer
    }
    if (ACMotorMod1->processCommand(ReceivedMessage))
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
    logToSerials(F(""), true, 0);
  }
}

time_t updateTime()
{
  time_t ReceivedTime = ((ACMotorModuleCommand *)ReceivedMessage)->Time;
  if (ReceivedTime > 0)
  {
    setTime(ReceivedTime);
    logToSerials(F("Clock synced"), true, 0);
  }
  return ReceivedTime;
}
*/


// Interrupt Service Routines - Keep these as simple as possible (interupts normal operation)


void zeroCrossingInterrupt()  //called every 50Hz AC : 10ms, 60Hz AC : 8.3335ms
{                    //AC signal crossed zero: start the delay before turning on the TRIAC
  if (ACMotorMod1->Motor1->MotorState) ///< If the motor should be running
  {
  TCCR1B = *ACMotorMod1->Motor1->Prescale; // prescale the
  TCNT1 = 0;         // reset timer - count from zero
  OCR1A = ACMotorMod1->Motor1->PID_TriacDelay;     // set the compare register: triggers TIMER1_COMPA_vect when the tick counter reaches the delay calculated by the PID controller
  }
}

ISR(TIMER1_COMPA_vect)  //called with a delay after each zeroCrossingInterrupt 
{                 // comparator match: TRIAC delay reached after a zero crossing
  if (ACMotorMod1->Motor1->MotorState) ///< If the motor should be running
  {
    digitalWrite(*(ACMotorMod1->Motor1->TriacPin), HIGH);        // turn on TRIAC gate
    TCNT1 = 65536 - *(ACMotorMod1->Motor1->TriacActiveTicks); // trigger pulse width
  }
}

ISR(TIMER1_OVF_vect)
{                              // timer1 overflow
  digitalWrite(*(ACMotorMod1->Motor1->TriacPin), LOW); // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
}


void tachoTrigger()
{
 ACMotorMod1->Motor1->TachoPulseCounter++;
}
