#pragma once

/*! 
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.  
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 2; ///< Increment this after changing the stucture of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 64;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 128; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!
static const uint8_t MovingAverageDepth = 8;   ///< Number of previous readings to keep when calculating average. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 9;              ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
static const uint8_t WirelessCEPin = 10;              ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
static const uint8_t WirelessChannel[6] = {"Moto1"};  ///< This needs to be unique and match with the Name of the ACMotorModule_Web object in the MainModule_Web.cpp
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
static const uint16_t WirelessReceiveTimeout = 60000; ///< (ms) If no packages are received from the Main module over this limit, try reseting the nRF24L01+ wireless receiver

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct ACMotorModuleSettings
  {
    ACMotorModuleSettings(bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSONFriendly = true, bool SerialReportJSON = true, bool SerialReportWireless = true) : SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportJSON(SerialReportJSON), SerialReportWireless(SerialReportWireless) {}
    bool SerialReportDate;          ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;        ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSONFriendly;  ///< Enable/disable sending Text formatted reports to the Serial output
    bool SerialReportJSON;          ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportWireless;      ///< Enable/disable sending wireless package exchange reports to the Serial output
  };
  struct ACMotorModuleSettings ACM1 = {.SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct ACMotorSettings ///< ACMotor default settings
  {
    ACMotorSettings(uint8_t ZeroCrossingPin = 0, uint8_t ComparatorPin = 0, uint8_t BackwardPin = 0, uint8_t ForwardPin = 0, uint8_t TriacPin = 0, uint8_t TargetRPMPin = 0, bool PIDEnabled = false, double Kp = 0, double Ki = 0, double Kd = 0, uint8_t TachoPulsesPerRevolution = 0, uint16_t RPMLimitMin = 0, uint16_t RPMLimitMax = 0, uint8_t Prescale = 0, double TriacDelayMin = 0, double TriacDelayMax = 0, uint8_t TriacActiveTicks = 0, uint16_t SpinOffTime = 0, uint8_t DebounceDelay = 0) : ZeroCrossingPin(ZeroCrossingPin), ComparatorPin(ComparatorPin), BackwardPin(BackwardPin), ForwardPin(ForwardPin), TriacPin(TriacPin), TargetRPMPin(TargetRPMPin), PIDEnabled(PIDEnabled), Kp(Kp), Ki(Ki), Kd(Kd),  TachoPulsesPerRevolution(TachoPulsesPerRevolution), RPMLimitMin(RPMLimitMin), RPMLimitMax(RPMLimitMax), Prescale(Prescale), TriacDelayMin(TriacDelayMin), TriacDelayMax(TriacDelayMax), TriacActiveTicks(TriacActiveTicks), SpinOffTime(SpinOffTime), DebounceDelay(DebounceDelay) {}
    uint8_t ZeroCrossingPin;          ///< Zero Crossing interrupt - Only pin 2 or 3 allowed on Nano. https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
    uint8_t ComparatorPin;            ///< LM393 comparator interupt for measuring tacho pulses -> used for RPM counting. Connect the LM393 Output1 to pin 2 or 3, LM393 IN1- pin: Motor Tacho cable1, GND PIN: Motor tacho cable2 + Arduino GND, and 5V to IN1+
    uint8_t BackwardPin;              ///< Pysical Button - Direction control
    uint8_t ForwardPin;               ///< Pysical Button - Direction control
    uint8_t TriacPin;                 ///< AC dimmer - PWM pin
    uint8_t TargetRPMPin;             ///< Analog pin connected to the center pin of a 10kΩ potentiometer. Left leg: GND and Right leg: +5V
    bool PIDEnabled;           ///< Enable/disable motor speed stabilization under variable load based on RPM feedback
    double Kp;                        ///< PID controller - proportional gain
    double Ki;                        ///< PID controller - integral gain
    double Kd;                        ///< PID controller - derivative gain
    uint8_t TachoPulsesPerRevolution; ///< Shortest delay after a zero crossing before turning on the TRIAC: The lower the delay the High the power output
    uint16_t RPMLimitMin;             ///< Target speed when TargetRPMPin potentiometer is at the lowest position
    uint16_t RPMLimitMax;             ///< Target speed when TargetRPMPin potentiometer is at the highest position
    uint8_t Prescale;                 ///< Timer1 Prescaler accepts the following values: 0x00 - Stop timer, 0x01 - No prescale (max ~4ms before overflow), 0x02: /8 prescale (max ~32ms), 0x03: /64 prescale, 0x04: /256 prescale,0x05: /1024 prescale  https://maxembedded.com/2011/06/avr-timers-timer1/
    double TriacDelayMin;             ///< Shortest delay after a zero crossing before turning on the TRIAC: The lower the delay the High the power output
    double TriacDelayMax;             ///< Longest delay after a zero crossing before turning on the TRIAC: Low power output. 15000 ticks -> 7.5ms
    uint8_t TriacActiveTicks;      ///< Keep the HIGH signal on the TRIAC gate for 20 timer ticks -> 10μs
    uint16_t SpinOffTime;             ///< (sec) How long it takes for the motor to stop after cutting the power
    uint8_t DebounceDelay;            ///< Number of miliseconds to wait for the signal to stabilize after a button press
  };
  struct ACMotorSettings Motor1 = {.ZeroCrossingPin = 2, .ComparatorPin = 3, .BackwardPin = 4, .ForwardPin = 5, .TriacPin = 7, .TargetRPMPin = A0, .PIDEnabled = true, .Kp = 1, .Ki = 1, .Kd = 0.01, .TachoPulsesPerRevolution = 16, .RPMLimitMin = 2000, .RPMLimitMax = 10000, .Prescale = 2, .TriacDelayMin = 0, .TriacDelayMax = 16000, .TriacActiveTicks = 20, .SpinOffTime = 5, .DebounceDelay = 50};

  struct RelaySettings ///< ACMotor default settings
  {
    RelaySettings(bool NegativeLogic = false, uint16_t FlipDelay = 0, uint8_t OnOffRelayPin = 0, uint8_t BrushRelayPin = 0, uint8_t Coil1RelayPin = 0, uint8_t Coil2RelayPin = 0) : NegativeLogic(NegativeLogic), FlipDelay(FlipDelay), OnOffRelayPin(OnOffRelayPin), BrushRelayPin(BrushRelayPin), Coil1RelayPin(Coil1RelayPin), Coil2RelayPin(Coil2RelayPin) {}
    bool NegativeLogic;    ///< 4 port relay switching logic: true: HIGH turns port ON, false: LOW turns port ON
    uint16_t FlipDelay;  ///< Time in milliseconds needed by the relays to change state
    uint8_t OnOffRelayPin; ///< Power intake relay pin - ON/OFF control
    uint8_t BrushRelayPin; ///< Motor brush relay pin - Direction control
    uint8_t Coil1RelayPin; ///< Motor coil pole 1 relay pin - Direction control
    uint8_t Coil2RelayPin; ///< Motor coil pole 2 relay pin - Direction control
  };
  struct RelaySettings Relay1 = {.NegativeLogic = true, .FlipDelay = 300, .OnOffRelayPin = A1, .BrushRelayPin1 = A2, .Coil1RelayPin = A3, .Coil2RelayPin = A4};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0, bool Enabled = false) : Pin(Pin), Enabled(Enabled) {}
    uint8_t Pin;  ///< Piezo Buzzer red(+) cable
    bool Enabled; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 6, .Enabled = true};

  uint8_t CompatibilityVersion = Version; ///< Should always be the last value stored.
} Settings;

/**
  \brief Store settings in EEPROM - Only updates changed bits
  \attention Use cautiously, EEPROM has a write limit of 100.000 cycles 
*/
void saveSettings(Settings *ToSave);
/**
  \brief Load settings from EEPROM
  \param ResetEEPROM - Force loading the defaults from the sketch and overwriting the EEPROM with it
  \return Reference to Settings object
*/
Settings *loadSettings(bool ResetEEPROM = false);
void restoreDefaults();
