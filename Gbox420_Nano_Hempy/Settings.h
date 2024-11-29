#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

constexpr uint8_t Version = 15; ///< Increment this after changing the stucture of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
constexpr uint8_t MaxWordLength = 32;       ///< Default char * buffer length for storing a word + null terminator. Memory intense!
constexpr uint8_t MaxShotTextLength = 64;   ///< Default char * buffer length for storing mutiple words. Memory intense!
constexpr uint16_t MaxLongTextLength = 168; ///< Default char * buffer length for storing a long text. Memory intense!
constexpr uint8_t QueueDepth = 6;           ///< Limits the maximum number of active modules. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
constexpr uint8_t WirelessCSNPin = 9;              ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
constexpr uint8_t WirelessCEPin = 10;              ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
constexpr uint8_t WirelessChannel[6] = {"Hemp1"};  ///< This needs to be unique and match with the Name of the HempyModule_Web object in the MainModule_Web.cpp
constexpr uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
constexpr uint16_t WirelessMessageTimeout = 500;   ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
constexpr uint16_t WirelessReceiveTimeout = 60000; ///< (ms) If no packages are received from the Main module over this limit, try reseting the nRF24L01+ wireless receiver

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct HempyModuleSettings
  {
    bool SerialReportDate;          ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;        ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSONFriendly;  ///< Enable/disable sending Text formatted reports to the Serial output
    bool SerialReportJSON;          ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportWireless;      ///< Enable/disable sending wireless package exchange reports to the Serial output
  };
  struct HempyModuleSettings Hemp1 = {.SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSONFriendly = true, .SerialReportJSON = true, .SerialReportWireless = true};

  struct HempyBucketSettings ///< HempyBucket default settings
  {
    float EvaporationTarget;  //< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float MaxWeight;         ///< Waste reservoir full weight -> Pump gets disabled if reached
    float StartWeight;   ///< (kg/lbs) When the module starts up start watering if Bucket weight is below this. Set to 0 to instantly start watering until DrainTargetWeight is reached.
    float WateringIncrement; ///< How much water to pump in one cycle, then wait for DrainWaitTime seconds before either starting a new pump cycle (DrainTargetWeight not reached) or considering the watering done (DrainTargetWeight reached)
    float DrainTargetWeight;  ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    uint16_t DrainWaitTime;   ///< (sec) How long to wait after watering for the water to drain
  };
  struct HempyBucketSettings Bucket1 = {.EvaporationTarget = 2.0, .MaxWeight = 20.0, .StartWeight = 18.0, .WateringIncrement = 0.3, .DrainTargetWeight = 0.1, .DrainWaitTime = 180};
  struct HempyBucketSettings Bucket2 = {.EvaporationTarget = 2.0, .MaxWeight = 20.0, .StartWeight = 18.0, .WateringIncrement = 0.3, .DrainTargetWeight = 0.1, .DrainWaitTime = 180};

  struct SoundSettings ///< Sound default settings
  {
    uint8_t Pin;  ///< Piezo Buzzer red(+) cable
    bool Enabled; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

  struct WaterPumpSettings ///< WaterPump default settings
  {
    uint8_t PumpPin;           ///< Pump relay pin
    bool PumpPinNegativeLogic; ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    bool PumpEnabled;          ///< Enable/disable pump. false= Block running the pump
    uint16_t PumpTimeOut;      ///< (Sec) Max pump run time
    uint8_t Speed;             ///< Duty cycle of the PWM Motor speed
    uint8_t SpeedLimitLow;     ///< Duty cycle limit, does not allow lowering the speed too much. Avoids stalling the motor
    uint8_t SpeedLimitHigh;    ///< Duty cycle limit, does not allow raising the speed too high
  };
  struct WaterPumpSettings HempyPump1 = {.PumpPin = 3, .PumpPinNegativeLogic = false, .PumpEnabled = true, .PumpTimeOut = 120, .Speed = 100, .SpeedLimitLow = 30, .SpeedLimitHigh = 100};
  struct WaterPumpSettings HempyPump2 = {.PumpPin = 5, .PumpPinNegativeLogic = false, .PumpEnabled = true, .PumpTimeOut = 120, .Speed = 100, .SpeedLimitLow = 30, .SpeedLimitHigh = 100};

  struct WeightSensorSettings ///< WeightSensor default settings
  {
    uint8_t DTPin;  ///< Weight sensor DT pin
    uint8_t SCKPin; ///< Weight sensor SCK pin
    long Offset;    ///< Reading at 0 weight on the scale
    float Scale;    ///< Scale factor
  };
  struct WeightSensorSettings WeightB1 = {.DTPin = 4, .SCKPin = 6, .Offset = -176647, .Scale = -20810.75};  ///< Bucket 1 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyBucketPlatforms/Test-WeightSensor_HempyBucketPlatforms.ino
  struct WeightSensorSettings WeightB2 = {.DTPin = 7, .SCKPin = 8, .Offset = 387148, .Scale = -22333.25};   ///< Bucket 2 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyBucketPlatforms/Test-WeightSensor_HempyBucketPlatforms.ino

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
