#pragma once

/*! 
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.  
 *  \warning   EEPROM has a write limit of 100,000 cycles. Constantly updating variables inside a loop will wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 3; ///< Increment this after changing the structure of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char* buffer length for storing a single word + null terminator. Memory intense!
static const uint8_t MaxShortTextLength = 64;   ///< Default char* buffer length for storing multiple words. Memory intense!
static const uint16_t MaxLongTextLength = 128; ///< Default char* buffer length for storing long text strings. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!
static const uint8_t MovingAverageDepth = 8;   ///< Number of historical readings retained for calculating moving averages. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temporary storage array for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShortTextLength]; // Temporary storage array for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing the current time string in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 9;              ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
static const uint8_t WirelessCEPin = 10;              ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
static const uint8_t WirelessChannel[6] = {"Moto1"};  ///< Unique channel name matching the corresponding ACMotorModule_Web object definition inside MainModule_Web.cpp
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) Complete single package exchange cycle time limit window (including retries and delays).
static const uint16_t WirelessReceiveTimeout = 60000; ///< (ms) If no wireless packages arrive from the Main module within this limit, trigger an nRF24L01+ receiver reset sequence.

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct __attribute__((packed))
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial and Metric units. If changed, manually update the default temperature and pressure values below.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct ACMotorModuleSettings
  {
    ACMotorModuleSettings(bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSONFriendly = true, bool SerialReportJSON = true, bool SerialReportWireless = true) : SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportJSON(SerialReportJSON), SerialReportWireless(SerialReportWireless) {}
    bool SerialReportDate;         ///< Enable/disable printing timestamps to the Serial monitor output
    bool SerialReportMemory;       ///< Enable/disable printing available dynamic free memory status to the Serial monitor output
    bool SerialReportJSONFriendly; ///< Enable/disable sending text formatted data streams to the Serial monitor output
    bool SerialReportJSON;         ///< Enable/disable streaming raw JSON formatted data strings to the Serial monitor output
    bool SerialReportWireless;     ///< Enable/disable outputting RF transceiver package exchange statistics to the Serial monitor
  };
  struct ACMotorModuleSettings ACM1 = {.SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct ACMotorSettings ///< ACMotor default settings
  {
    ACMotorSettings(uint8_t ZeroCrossingPin = 0, uint8_t ComparatorPin = 0, uint8_t BackwardPin = 0, uint8_t ForwardPin = 0, uint8_t TriacPin = 0, uint8_t TargetRPMPin = 0, bool PIDEnabled = false, double Kp = 0, double Ki = 0, double Kd = 0, uint8_t TachoPulsesPerRevolution = 0, uint16_t RPMLimitMin = 0, uint16_t RPMLimitMax = 0, uint8_t Prescale = 0, double TriacDelayMin = 0, double TriacDelayMax = 0, uint8_t TriacActiveTicks = 0, uint16_t SpinOffTime = 0, uint8_t DebounceDelay = 0) : Kp(Kp), Ki(Ki), Kd(Kd), TriacDelayMin(TriacDelayMin), TriacDelayMax(TriacDelayMax), RPMLimitMin(RPMLimitMin), RPMLimitMax(RPMLimitMax), SpinOffTime(SpinOffTime), ZeroCrossingPin(ZeroCrossingPin), ComparatorPin(ComparatorPin), BackwardPin(BackwardPin), ForwardPin(ForwardPin), TriacPin(TriacPin), TargetRPMPin(TargetRPMPin), TachoPulsesPerRevolution(TachoPulsesPerRevolution), Prescale(Prescale), TriacActiveTicks(TriacActiveTicks), DebounceDelay(DebounceDelay), PIDEnabled(PIDEnabled) {}
    double Kp;                        ///< PID controller - proportional gain configuration value
    double Ki;                        ///< PID controller - integral gain configuration value
    double Kd;                        ///< PID controller - derivative gain configuration value
    double TriacDelayMin;             ///< Shortest delay after a zero crossing before turning on the TRIAC: The lower the delay, the higher the power output.
    double TriacDelayMax;             ///< Longest delay after a zero crossing before turning on the TRIAC: Corresponds to lower power output. 15,000 ticks = 7.5ms.
    uint16_t RPMLimitMin;             ///< Target speed threshold mapped when TargetRPMPin potentiometer reaches its absolute minimum bound.
    uint16_t RPMLimitMax;             ///< Target speed threshold mapped when TargetRPMPin potentiometer reaches its absolute maximum bound.
    uint16_t SpinOffTime;             ///< (sec) Total settling duration required for the motor assembly to safely coast down to a stop after isolating power lines.
    uint8_t ZeroCrossingPin;          ///< Zero Crossing interrupt loop pin tracking line phase shifts. Only digital pins 2 or 3 are permitted on the ATmega328P Nano.
    uint8_t ComparatorPin;            ///< LM393 comparator hardware external interrupt tracking tachometer input signal pulses to calculate current RPM levels. Connect Output 1 of the LM393 module to pin 2 or 3. Connect IN1- to Motor Tachometer line 1. Connect ground line to Motor Tachometer line 2 and Arduino GND. Supply 5V to the IN1+ channel.
    uint8_t BackwardPin;              ///< Digital input interface pin wired to the physical direction control button (Reverse).
    uint8_t ForwardPin;               ///< Digital input interface pin wired to the physical direction control button (Forward).
    uint8_t TriacPin;                 ///< AC phase dimmer firing pin mapped to a timer-capable digital output channel.
    uint8_t TargetRPMPin;             ///< Analog input interface pin wired to the adjustable center sweep leg of an external 10kΩ control potentiometer.
    uint8_t TachoPulsesPerRevolution; ///< Count parameter identifying how many discrete pulses the embedded tachometer generates during one mechanical rotation.
    uint8_t Prescale;                 ///< Timer1 Clock Prescaler configuration bitmask configuration flags: 0x00 stops the timer, 0x01 maps no prescaling (max ~4ms window before overflow triggers), 0x02 sets a /8 divisor (max ~32ms window), 0x03 maps a /64 divisor, 0x04 maps a /256 divisor, 0x05 enforces a /1024 divisor factor.
    uint8_t TriacActiveTicks;         ///< The pulse duration holding window where the logic HIGH gate drive signal remains applied to the active TRIAC terminal (20 timer ticks = 10μs).
    uint8_t DebounceDelay;            ///< Mechanical switch signal settling window in milliseconds to ignore button contact bounce noise.
    bool PIDEnabled;                  ///< Master switch enabling closed-loop speed stabilization behaviors adjusting dynamically to varying shaft loads using tachometer feedback vectors.
  };
  struct ACMotorSettings Motor1 = {.Kp = 1, .Ki = 1, .Kd = 0.01, .TriacDelayMin = 0, .TriacDelayMax = 16000, .RPMLimitMin = 2000, .RPMLimitMax = 10000, .SpinOffTime = 5, .ZeroCrossingPin = 2, .ComparatorPin = 3, .BackwardPin = 4, .ForwardPin = 5, .TriacPin = 7, .TargetRPMPin = A0, .TachoPulsesPerRevolution = 16, .Prescale = 2, .TriacActiveTicks = 20, .DebounceDelay = 50, .PIDEnabled = true};

  struct RelaySettings ///< ACMotor default settings
  {
    RelaySettings(bool NegativeLogic = false, uint16_t FlipDelay = 0, uint8_t OnOffRelayPin = 0, uint8_t BrushRelayPin = 0, uint8_t Coil1RelayPin = 0, uint8_t Coil2RelayPin = 0) : FlipDelay(FlipDelay), OnOffRelayPin(OnOffRelayPin), BrushRelayPin(BrushRelayPin), Coil1RelayPin(Coil1RelayPin), Coil2RelayPin(Coil2RelayPin), NegativeLogic(NegativeLogic) {}
    uint16_t FlipDelay;    ///< Protective interlocking propagation delay in milliseconds separating dynamic relay coil transitions.
    uint8_t OnOffRelayPin; ///< Master alternating current supply routing relay tracking execution control.
    uint8_t BrushRelayPin; ///< Direction configuration relay mapping the armature winding pathways.
    uint8_t Coil1RelayPin; ///< Direction configuration relay mapping the field excitation coil pole 1 line.
    uint8_t Coil2RelayPin; ///< Direction configuration relay mapping the field excitation coil pole 2 line.
    bool NegativeLogic;    ///< 4-port relay board drive orientation tracker: set true if low states energize standard coils; set false if high logic commands channels.
  };
  struct RelaySettings Relay1 = {.FlipDelay = 300, .OnOffRelayPin = A1, .BrushRelayPin = A2, .Coil1RelayPin = A3, .Coil2RelayPin = A4, .NegativeLogic = true};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0, bool Enabled = false) : Pin(Pin), Enabled(Enabled) {}
    uint8_t Pin;  ///< Hardware connection pin for Piezo Buzzer red (+) positive line
    bool Enabled; ///< Enable/Disable audible signaling alerts
  };
  struct SoundSettings Sound1 = {.Pin = 6, .Enabled = true};

  uint8_t CompatibilityVersion = Version; ///< Structural layout validation indicator. This must strictly remain the terminal variable definition located inside the struct memory footprint.
} Settings;

/**
  \brief Store settings in EEPROM - Only updates changed bits
  \attention Use cautiously, EEPROM has a write limit of 100,000 cycles
*/
void saveSettings(Settings *ToSave);
/**
  \brief Load settings from EEPROM
  \param ResetEEPROM - Force loading the defaults from the sketch and overwriting the EEPROM with it
  \return Reference to Settings object
*/
Settings *loadSettings(bool ResetEEPROM = false);
void restoreDefaults();