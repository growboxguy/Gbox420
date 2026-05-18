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
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!
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
    bool SerialReportJSONFriendly; ///< Enable/disable sending text formatted data streams to the Serial monitor output
    bool SerialReportJSON;         ///< Enable/disable streaming raw JSON formatted data strings to the Serial monitor output
    bool SerialReportWireless;     ///< Enable/disable outputting RF transceiver package exchange statistics to the Serial monitor
  };
  struct AeroponicsModuleSettings Aero1 = {.SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct AeroponicsSettings ///< Aeroponics_Tank default settings
  {
    float Duration;                     ///< Spray time in seconds (Actual duration is ~0.5sec longer due to thread + solenoid delay)
    float MinPressure;                  ///< Turn on pump below this pressure
    float MaxPressure;                  ///< Turn off pump above this pressure
    uint16_t SpraySolenoidClosingDelay; ///< (ms) Time required for the solenoid to close. To avoid draining the tank the bypass valve is not allowed to open until the Spray solenoid is closed
    uint16_t TankDrainingTimeout;       ///< (sec) How long does it take to fully drain the pressure tank by opening the spray and bypass solenoid at the same time
    uint16_t DayInterval;               ///< Spray every X minutes - When the lights are ON
    uint16_t NightInterval;             ///< Spray every X minutes - When the lights are OFF
    uint8_t SpraySolenoidPin;           ///< Relay controlling DC power to the solenoid
    bool SpraySolenoidNegativeLogic;    ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    bool SprayEnabled;                  ///< Enable/disable spraying cycle
  };
  struct AeroponicsSettings AeroT1 = {.Duration = 1.0, .MinPressure = 5.0, .MaxPressure = 7.0, .SpraySolenoidClosingDelay = 600, .TankDrainingTimeout = 180, .DayInterval = 6, .NightInterval = 10, .SpraySolenoidPin = 5, .SpraySolenoidNegativeLogic = true, .SprayEnabled = true};

  struct PressureSensorSettings ///< PressureSensor default settings
  {
    float Offset; ///< Pressure sensor calibration: voltage reading at 0 pressure
    float Ratio;  ///< Pressure sensor voltage to pressure ratio
    uint8_t Pin;  ///< Pressure sensor Pin: Signal(yellow)
  };
  struct PressureSensorSettings Pres1 = {.Offset = 0.57, .Ratio = 2.7, .Pin = A7};

  struct SoundSettings ///< Sound default settings
  {
    uint8_t Pin;  ///< Piezo Buzzer red(+) cable
    bool Enabled; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

  struct PressurePumpSettings ///< PressurePump default settings
  {
    uint16_t BypassSolenoidMaxOpenTime;  ///< (sec) Max time the bypass can stay open
    uint16_t BypassSolenoidClosingDelay; ///< (ms) How long it takes for the solenoid to close
    uint16_t PumpTimeOut;                ///< (Sec) Max pump run time
    uint16_t PrimingTime;                ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
    uint16_t BlowOffTime;                ///< (Sec) For how long to open the bypass solenoid on after turning the pump off - Release pressure from pump discharge side
    uint8_t PumpPin;                     ///< Pump relay pin
    uint8_t BypassSolenoidPin;           ///< Bypass solenoid relay pin
    uint8_t Speed;                       ///< Duty cycle of the PWM Motor speed
    uint8_t SpeedLimitLow;               ///< Duty cycle limit, does not allow lowering the speed too much. Avoids stalling the motor
    uint8_t SpeedLimitHigh;              ///< Maximum allowed speed to prevent over-revving
    bool PumpPinNegativeLogic;           ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    bool BypassSolenoidNegativeLogic;    ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    bool PumpEnabled;                    ///< Enable/disable pump. false = Block running the pump
  };
  struct PressurePumpSettings AeroPump1 = {.BypassSolenoidMaxOpenTime = 180, .BypassSolenoidClosingDelay = 600, .PumpTimeOut = 420, .PrimingTime = 10, .BlowOffTime = 3, .PumpPin = 3, .BypassSolenoidPin = 4, .Speed = 70, .SpeedLimitLow = 30, .SpeedLimitHigh = 100, .PumpPinNegativeLogic = false, .BypassSolenoidNegativeLogic = true, .PumpEnabled = true};

  struct WeightSensorSettings ///< WeightSensor default settings
  {
    long Offset;    ///< Reading at 0 weight on the scale
    float Scale;    ///< Scale factor
    uint8_t DTPin;  ///< Weight sensor DT pin
    uint8_t SCKPin; ///< Weight sensor SCK pin
  };
  struct WeightSensorSettings Weight1 = {.Offset = -288069, .Scale = -22280.70, .DTPin = A0, .SCKPin = A1}; ///< Waste Reservoir 1 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/tree/master/Test_Sketches/Test-WeightSensor_Aero

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