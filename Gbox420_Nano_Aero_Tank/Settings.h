#pragma once

/*! 
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.  
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 9; ///< Increment this after changing the stucture of the SAVED TO EEPROM secton to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 64;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 256; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!
static const uint8_t MovingAverageDepth = 10;  ///< Limits the maximum number of active modules. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 9;              ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
static const uint8_t WirelessCEPin = 10;              ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
static const uint8_t WirelessChannel[6] = {"Aero1"};  ///< This needs to be unique and match with the Name of the AeroModule_Web object in the MainModule_Web.cpp
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
static const uint16_t WirelessReceiveTimeout = 60000; ///< (ms) If no packages are received from the Main module over this limit, try reseting the nRF24L01+ wireless receiver

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  //<initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct AeroponicsModuleSettings
  {
    AeroponicsModuleSettings(uint16_t SerialReportFrequency = 0, bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSONFriendly = true, bool SerialReportJSON = true, bool SerialReportWireless = true) : SerialReportFrequency(SerialReportFrequency), SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportJSON(SerialReportJSON), SerialReportWireless(SerialReportWireless) {}
    uint16_t SerialReportFrequency; ///< How often to report to Serial console. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
    bool SerialReportDate;          ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;        ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSONFriendly;  ///< Enable/disable sending Text formatted reports to the Serial output
    bool SerialReportJSON;          ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportWireless;      ///< Enable/disable sending wireless package exchange reports to the Serial output
  };
  struct AeroponicsModuleSettings Aero1 = {.SerialReportFrequency = 15, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct AeroponicsSettings ///< Aeroponics_Tank default settings
  {
    AeroponicsSettings(uint8_t SpraySolenoidPin = 0, bool SpraySolenoidNegativeLogic = false, uint16_t SpraySolenoidClosingDelay = 0, uint16_t TankDrainingTimeout = 0, bool SprayEnabled = true, float Duration = 0.0, uint16_t DayInterval = 0, uint16_t NightInterval = 0, float MinPressure = 0.0, float MaxPressure = 0.0) : SpraySolenoidPin(SpraySolenoidPin), SpraySolenoidNegativeLogic(SpraySolenoidNegativeLogic), SpraySolenoidClosingDelay(SpraySolenoidClosingDelay), TankDrainingTimeout(TankDrainingTimeout), SprayEnabled(SprayEnabled), Duration(Duration), DayInterval(DayInterval), NightInterval(NightInterval), MinPressure(MinPressure), MaxPressure(MaxPressure) {}
    uint8_t SpraySolenoidPin;           ///< Relay controlling DC power to the solenoid
    bool SpraySolenoidNegativeLogic;    ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    uint16_t SpraySolenoidClosingDelay; ///< (ms) Time required for the solenoid to close. To avoid draining the tank the bypass valve is not allowed to open until the Spray solenoid is not closed
    uint16_t TankDrainingTimeout;       ///< (sec) How long does it take to fully drain the pressure tank by opening the spray and bypass solenoid at the same time
    bool SprayEnabled;                  ///< Enable/disable spraying cycle
    float Duration;                     ///< Spray time in seconds (Actual duration is ~0.5sec longer due to thread + solenoid delay)
    uint16_t DayInterval;               ///< Spray every X minutes - When the lights are ON
    uint16_t NightInterval;             ///< Spray every X minutes - When the lights are OFF
    float MinPressure;                  ///< Turn on pump below this pressure
    float MaxPressure;                  ///< Turn off pump above this pressure
  };
  struct AeroponicsSettings AeroT1 = {.SpraySolenoidPin = 5, .SpraySolenoidNegativeLogic = true, .SpraySolenoidClosingDelay = 600, .TankDrainingTimeout = 180, .SprayEnabled = true, .Duration = 1.0, .DayInterval = 6, .NightInterval = 10, .MinPressure = 5.0, .MaxPressure = 7.0};

  struct PressureSensorSettings ///< PressureSensor default settings
  {
    PressureSensorSettings(uint8_t Pin = 0, float Offset = 0.0, float Ratio = 0.0) : Pin(Pin), Offset(Offset), Ratio(Ratio) {}
    uint8_t Pin;  ///< Pressure sensor Pin: Signal(yellow)
    float Offset; ///< Pressure sensor calibration: voltage reading at 0 pressure
    float Ratio;  ///< Pressure sensor voltage to pressure ratio
  };
  struct PressureSensorSettings Pres1 = {.Pin = A7, .Offset = 0.57, .Ratio = 2.7};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin;         ///< Piezo Buzzer red(+) cable
    bool Enabled = true; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2};

  struct PressurePumpSettings ///< PressurePump default settings
  {
    PressurePumpSettings(uint8_t PumpPin = 0, bool PumpPinNegativeLogic = false, uint8_t BypassSolenoidPin = 0, bool BypassSolenoidNegativeLogic = false, uint16_t BypassSolenoidMaxOpenTime = 0, uint16_t BypassSolenoidClosingDelay = 0, bool PumpEnabled = false, uint8_t Speed = 100, uint8_t SpeedLowLimit = 0, uint16_t PumpTimeOut = 0, uint16_t PrimingTime = 0, uint16_t BlowOffTime = 0) : PumpPin(PumpPin), PumpPinNegativeLogic(PumpPinNegativeLogic), BypassSolenoidPin(BypassSolenoidPin), BypassSolenoidNegativeLogic(BypassSolenoidNegativeLogic), BypassSolenoidMaxOpenTime(BypassSolenoidMaxOpenTime), BypassSolenoidClosingDelay(BypassSolenoidClosingDelay), PumpEnabled(PumpEnabled), Speed(Speed), SpeedLowLimit(SpeedLowLimit), PumpTimeOut(PumpTimeOut), PrimingTime(PrimingTime), BlowOffTime(BlowOffTime) {}
    uint8_t PumpPin;                     ///< Pump relay pin
    bool PumpPinNegativeLogic;           ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    uint8_t BypassSolenoidPin;           ///< Bypass solenoid relay pin
    bool BypassSolenoidNegativeLogic;    ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    uint16_t BypassSolenoidMaxOpenTime;  ///< (sec) Max time the bypass can stay open
    uint16_t BypassSolenoidClosingDelay; ///< (ms) How long it takes for the solenoid to close
    bool PumpEnabled;                    ///< Enable/disable pump. false= Block running the pump
    uint8_t Speed;                       ///< Duty cycle of the PWM Motor speed
    uint8_t SpeedLowLimit;               ///< Duty cycle limit, does not allow lowering the speed too much. Avoids stalling the motor
    uint16_t PumpTimeOut;                ///< (Sec) Max pump run time
    uint16_t PrimingTime;                ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
    uint16_t BlowOffTime;                ///< (Sec) For how long to open the bypass solenoid on after turning the pump off - Release pressure from pump discharge side
  };
  struct PressurePumpSettings AeroPump1 = {.PumpPin = 3, .PumpPinNegativeLogic = false, .BypassSolenoidPin = 4, .BypassSolenoidNegativeLogic = true, .BypassSolenoidMaxOpenTime = 180, .BypassSolenoidClosingDelay = 600, .PumpEnabled = true, .Speed = 70, .SpeedLowLimit = 30, .PumpTimeOut = 420, .PrimingTime = 10, .BlowOffTime = 3};

/*
  struct WeightSensorSettings ///< WeightSensor default settings
  {
    WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, long Offset = 0, float Scale = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Offset(Offset), Scale(Scale) {}
    uint8_t DTPin;  ///< Weight sensor DT pin
    uint8_t SCKPin; ///< Weight sensor SCK pin
    long Offset;    ///< Reading at 0 weight on the scale
    float Scale;    ///< Scale factor
  };
  struct WeightSensorSettings Weight1 = {.DTPin = A0, .SCKPin = A1, .Offset = -288069, .Scale = -22280.70}; ///< Waste Reservoir 1 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/tree/master/Test_Sketches/Test-WeightSensor_Aero
*/

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