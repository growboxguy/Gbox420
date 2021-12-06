#pragma once

/*! 
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.  
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 3; ///< Increment this after changing the stucture of the SAVED TO EEPROM secton to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 64;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 128; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!
static const uint8_t MovingAverageDepth = 8;   ///< Limits the maximum number of active modules. Memory intense!

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
    ACMotorModuleSettings(uint16_t SerialReportFrequency = 15, bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSONFriendly = true, bool SerialReportJSON = true, bool SerialReportWireless = true) : SerialReportFrequency(SerialReportFrequency), SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportJSON(SerialReportJSON), SerialReportWireless(SerialReportWireless) {}
    uint16_t SerialReportFrequency; ///< How often to report to Serial console. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
    bool SerialReportDate;          ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;        ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSONFriendly;  ///< Enable/disable sending Text formatted reports to the Serial output
    bool SerialReportJSON;          ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportWireless;      ///< Enable/disable sending wireless package exchange reports to the Serial output
  };
  struct ACMotorModuleSettings ACM1 = {.SerialReportFrequency = 15, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct ACMotorSettings ///< ACMotor default settings
  {
    ACMotorSettings(double Speed = 0, uint8_t RPMTargetPin = 0, uint16_t RPMTargetMin = 0, uint16_t RPMTargetMax = 0, uint16_t ACDimmerZCPin = 0, uint16_t ACDimmerPWMPin = 0, uint8_t ACDimmerLimitMin = 0, uint8_t ACDimmerLimitMax = 0, uint8_t TachoPulsesPerRevolution = 0, double Kp = 0, double Ki = 0, double Kd = 0, uint16_t SpinOffTime = 0, bool RelayNegativeLogic = false, uint8_t OnOffPin = 0, uint8_t BrushPin = 0, uint8_t Coil1Pin = 0, uint8_t Coil2Pin = 0, uint8_t ForwardPin = 0, uint8_t BackwardPin = 0, uint8_t ComparatorPin = 0) : Speed(Speed), RPMTargetPin(RPMTargetPin), RPMTargetMin(RPMTargetMin), RPMTargetMax(RPMTargetMax), ACDimmerZCPin(ACDimmerZCPin), ACDimmerPWMPin(ACDimmerPWMPin), ACDimmerLimitMin(ACDimmerLimitMin), ACDimmerLimitMax(ACDimmerLimitMax), TachoPulsesPerRevolution(TachoPulsesPerRevolution), Kp(Kp), Ki(Ki), Kd(Kd), SpinOffTime(SpinOffTime), RelayNegativeLogic(RelayNegativeLogic), OnOffPin(OnOffPin), BrushPin(BrushPin), Coil1Pin(Coil1Pin), Coil2Pin(Coil2Pin), ForwardPin(ForwardPin), BackwardPin(BackwardPin), ComparatorPin(ComparatorPin) {}
    double Speed;                     ///< Motor speed at startup (0% - 100%)
    uint8_t RPMTargetPin;             ///< Analog pin connected to the center pin of a 10kOhm potentiometer. Left leg: GND and Right leg: +5V
    uint16_t RPMTargetMin;            ///< Target speed when RPMTargetPin potentiometer is at the lowest position
    uint16_t RPMTargetMax;            ///< Target speed when RPMTargetPin potentiometer is at the highest position
    uint8_t ACDimmerZCPin;            ///< AC dimmer - Zero Corssing pin, FIXED to port D2 for interrupt handling
    uint8_t ACDimmerPWMPin;           ///< AC dimmer - PWM pin
    uint8_t ACDimmerLimitMin;         ///< Lowest % allowed PWM speed
    uint8_t ACDimmerLimitMax;         ///< Highest % allowed PWM speed
    uint8_t TachoPulsesPerRevolution; ///< How many Tachometer pulses are generated during a full turn of the motor shaft. Used during RPM calculations
    double Kp;                        ///< PID controller - proportional gain
    double Ki;                        ///< PID controller - integral gain
    double Kd;                        ///< PID controller - derivative gain
    uint16_t SpinOffTime;             ///< (sec) How long it takes for the motor to stop after cutting the power
    bool RelayNegativeLogic;          ///< 4 port relay switching logic: true: HIGH turns port ON, false: LOW turns port ON
    uint8_t OnOffPin;                 ///< Power intake relay pin - ON/OFF control
    uint8_t BrushPin;                 ///< Motor brush relay pin - Direction control
    uint8_t Coil1Pin;                 ///< Motor coil pole 1 relay pin - Direction control
    uint8_t Coil2Pin;                 ///< Motor coil pole 2 relay pin - Direction control
    uint8_t ForwardPin;               ///< Pysical Button - Direction control
    uint8_t BackwardPin;              ///< Pysical Button - Direction control
    uint8_t ComparatorPin;            ///< External comparator output -> triggers an interoupt (Nano supports this on D2 or D3 ports)
  };
  struct ACMotorSettings Motor1 = {.Speed = 32, .RPMTargetPin = A0, .RPMTargetMin = 2000, .RPMTargetMax = 10000, .ACDimmerZCPin = 2, .ACDimmerPWMPin = 10, .ACDimmerLimitMin = 27, .ACDimmerLimitMax = 80, .TachoPulsesPerRevolution = 16, .Kp = 0.05, .Ki = 0.05, .Kd = 0, .SpinOffTime = 5, .RelayNegativeLogic = true, .OnOffPin = A4, .BrushPin1 = A3, .Coil1Pin = A2, .Coil2Pin = A1, .ForwardPin = 4, .BackwardPin = 5, .ComparatorPin=3};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0, bool Enabled = false) : Pin(Pin), Enabled(Enabled) {}
    uint8_t Pin;  ///< Piezo Buzzer red(+) cable
    bool Enabled; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 9, .Enabled = true};

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
