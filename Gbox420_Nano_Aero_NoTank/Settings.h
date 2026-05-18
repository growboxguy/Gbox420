#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100,000 cycles. Constantly updating variables inside a loop will wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 10; ///< Increment this after changing the structure of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char* buffer length for storing a single word + null terminator. Memory intense!
static const uint8_t MaxShortTextLength = 64;   ///< Default char* buffer length for storing multiple words. Memory intense!
static const uint16_t MaxLongTextLength = 256; ///< Default char* buffer length for storing long text strings. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active components allowed within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;  ///< Number of historical readings retained for calculating moving averages. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temporary storage array for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShortTextLength]; // Temporary storage array for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing the current time string in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 9;              ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
static const uint8_t WirelessCEPin = 10;              ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
static const uint8_t WirelessChannel[6] = {"Aero1"};  ///< Unique channel name matching the corresponding AeroModule_Web object definition inside MainModule_Web.cpp
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
  struct AeroponicsModuleSettings
  {
    bool SerialReportDate;         ///< Enable/disable printing timestamps to the Serial monitor output
    bool SerialReportMemory;       ///< Enable/disable printing available dynamic free memory status to the Serial monitor output
    bool SerialReportJSONFriendly; ///< Enable/disable appending human-readable standard units (Sec, %, Min, kg/lbs, etc.) to the Serial JSON output streams
    bool SerialReportJSON;         ///< Enable/disable streaming raw JSON formatted data strings to the Serial monitor output
    bool SerialReportWireless;     ///< Enable/disable outputting RF transceiver package exchange statistics to the Serial monitor
  };
  struct AeroponicsModuleSettings Aero1 = {.SerialReportDate = false, .SerialReportMemory = false, .SerialReportJSONFriendly = false, .SerialReportJSON = false, .SerialReportWireless = true};

  struct AeroponicsSettings
  {
    float Duration;           ///< Spray execution runtime duration scaled across seconds units boundaries
    float MaxPressure;        ///< Upper critical pressure ceiling point where the accumulator pump disengages
    uint16_t DayInterval;     ///< Spray repeat interval timing in minutes maintained when daytime lighting profiles run
    uint16_t NightInterval;   ///< Spray repeat interval timing in minutes maintained when nighttime lighting profiles run
    bool SprayEnabled;        ///< Master execution flag enabling or halting automated aeroponics misting routines
  };
  struct AeroponicsSettings AeroNT1 = {.Duration = 3.0, .MaxPressure = 7.0, .DayInterval = 6, .NightInterval = 10, .SprayEnabled = true};

  struct PressureSensorSettings
  {
    float Offset; ///< Pressure sensor interface calibration reference: raw voltage signature observed under 0 pressure states
    float Ratio;  ///< Core linear operational mapping variable converting raw output voltage into accurate pressure units
    uint8_t Pin;  ///< Analog input pin reading the raw electronic signature line (yellow) from the pressure sensor transducer
  };
  struct PressureSensorSettings Pres1 = {.Offset = 0.57, .Ratio = 2.7, .Pin = A7};

  struct SoundSettings
  {
    uint8_t Pin;  ///< Hardware connection pin for Piezo Buzzer red (+) positive line
    bool Enabled; ///< Enable/Disable audible signaling alerts
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

  struct PressurePumpSettings
  {
    uint16_t BypassSolenoidMaxOpenTime;  ///< (sec) Failsafe upper constraint limit limiting the continuous runtime window allowed for the bypass solenoid to sit open
    uint16_t BypassSolenoidClosingDelay; ///< (ms) Electromechanical inductive settling delay window needed for the solenoid coil to physically seal
    uint16_t PumpTimeOut;                ///< (Sec) Safe continuous running window constraint checking pump operations before emergency lockout routines engage
    uint16_t PrimingTime;                ///< (Sec) Allocation window keeping the bypass solenoid energized on initial loops to clear out trapped intake gas locks
    uint16_t BlowOffTime;                ///< (Sec) Post-run pressure relief delay opening the bypass solenoid immediately upon cutting motor drive lines
    uint8_t PumpPin;                     ///< Logic interface output control line mapped to the primary pump activation relay channel
    uint8_t BypassSolenoidPin;           ///< Logic interface output control line mapped to the bypass solenoid gas bleed relay channel
    uint8_t Speed;                       ///< Standard baseline speed configuration parameter driving the motor through variable PWM duty cycles (0-100%)
    uint8_t SpeedLimitLow;               ///< Dynamic low speed floor threshold limiting PWM modulation adjustments to prevent electrical motor stalls
    uint8_t SpeedLimitHigh;              ///< Upper safety limit ceiling clamping PWM speed outputs to protect against over-revving mechanical damage
    bool PumpPinNegativeLogic;           ///< Drive orientation flag: true maps to relays running on active LOW signals; false means tracking active HIGH states
    bool BypassSolenoidNegativeLogic;    ///< Drive orientation flag: true maps to relays running on active LOW signals; false means tracking active HIGH states
    bool PumpEnabled;                    ///< Master software isolation switch override enabling or entirely locking out pump channel executions
  };
  struct PressurePumpSettings AeroPump1 = {.BypassSolenoidMaxOpenTime = 180, .BypassSolenoidClosingDelay = 600, .PumpTimeOut = 420, .PrimingTime = 10, .BlowOffTime = 3, .PumpPin = 3, .BypassSolenoidPin = 4, .Speed = 100, .SpeedLimitLow = 30, .SpeedLimitHigh = 100, .PumpPinNegativeLogic = false, .BypassSolenoidNegativeLogic = true, .PumpEnabled = true};

  struct WeightSensorSettings
  {
    long Offset;    ///< Raw analog-to-digital converter signal reading observed under structural zero-weight scale deck conditions
    float Scale;    ///< Linear adjustment scale multi-factor tracking variable used to accurately weight object volumes
    uint8_t DTPin;  ///< Hardware instrumentation data (DT) interface line pin mapped to the load cell amplifier breakout board
    uint8_t SCKPin; ///< Hardware instrumentation clock (SCK) interface line pin mapped to the load cell amplifier breakout board
  };
  struct WeightSensorSettings Weight1 = {.Offset = 19793, .Scale = -22280.70, .DTPin = A0, .SCKPin = A1}; ///< Generate appropriate calibration values using validation scripts located at: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_Aero/Test-WeightSensor_Aero.ino

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