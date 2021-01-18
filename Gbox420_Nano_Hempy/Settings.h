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

///< State machine - Defining possible states
enum PumpStates
{
  DISABLED,
  IDLE,
  PRIMING,
  RUNNING,
  BLOWOFF,
  MIXING
};

///< Global constants
static const uint8_t MaxWordLength = 32;       ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 64;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 192; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t QueueDepth = 8;           ///< Limits the maximum number of active modules. Memory intense!
static const uint8_t RollingAverageDepth = 10; ///< Limits the maximum number of active modules. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 9;             ///< nRF24l01+ wireless transmitter CSN pin - Pre-connected on RF-Nano
static const uint8_t WirelessCEPin = 10;             ///< nRF24l01+ wireless transmitter CE pin - Pre-connected on RF-Nano
static const uint8_t WirelessChannel[6] = {"Hemp1"}; ///< This needs to be unique and match with the Name of the HempyModule_Web object in the MainModule_Web.cpp
static const uint8_t WirelessPayloadSize = 32;       ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;  ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
static const uint16_t WirelessReceiveTimeout = 60000;  ///< (ms) If no packages are received from the Main module over this limit, try reseting the nRF24L01+ wireless receiver

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct HempyBucketSettings ///< HempyBucket default settings
  {
    HempyBucketSettings(bool WeightBasedWatering = false, float StartWeight = 0.0, float StopWeight = 0.0, float WasteLimit = 0.0, bool TimerBasedWatering = false, uint16_t WateringInterval = 0, uint16_t WateringDuration = 0) : WeightBasedWatering(WeightBasedWatering), StartWeight(StartWeight), StopWeight(StopWeight), WasteLimit(WasteLimit), TimerBasedWatering(TimerBasedWatering), WateringInterval(WateringInterval), WateringDuration(WateringDuration) {}
    bool WeightBasedWatering;  //Enable/Disable weight based watering
    float StartWeight;         ///< Start watering below this weight
    float StopWeight;          ///< Stop watering above this weight
    float WasteLimit;          ///< Waste reservoir full weight -> Pump gets disabled if reached
    bool TimerBasedWatering;   //Enable/Disable timer based watering
    uint16_t WateringInterval; //Water every X minutes
    uint16_t WateringDuration; //Water for X seconds
  };
  struct HempyBucketSettings Bucket1 = {.WeightBasedWatering = true, .StartWeight = 16.0, .StopWeight = 19.0, .WasteLimit = 13.0, .TimerBasedWatering = false, .WateringInterval = 1440, .WateringDuration = 30};
  struct HempyBucketSettings Bucket2 = {.WeightBasedWatering = true, .StartWeight = 16.0, .StopWeight = 19.0, .WasteLimit = 13.0, .TimerBasedWatering = false, .WateringInterval = 1440, .WateringDuration = 30};

  struct HempyModuleSettings  ///< HempyModule default settings
  {
    //HempyModuleSettings() :  {}
  };
  struct HempyModuleSettings HempyMod1 = {}; ///< Default settings for the Hempy Module

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0, bool Enabled = false) : Pin(Pin), Enabled(Enabled) {}
    uint8_t Pin;  ///< Piezo Buzzer red(+) cable
    bool Enabled; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

  struct WaterPumpSettings ///< WaterPump default settings
  {
    WaterPumpSettings(uint8_t PumpPin = 0, bool PumpPinNegativeLogic = false, bool PumpEnabled = false, uint8_t Speed = 100, uint8_t SpeedLowLimit = 0, uint16_t PumpTimeOut = 0, int PrimingTime = 0, int BlowOffTime = 0, uint8_t BypassSolenoidPin = 0, bool BypassSolenoidNegativeLogic = false) : PumpPin(PumpPin), PumpPinNegativeLogic(PumpPinNegativeLogic), PumpEnabled(PumpEnabled), Speed(Speed), SpeedLowLimit(SpeedLowLimit), PumpTimeOut(PumpTimeOut), PrimingTime(PrimingTime), BlowOffTime(BlowOffTime), BypassSolenoidPin(BypassSolenoidPin), BypassSolenoidNegativeLogic(BypassSolenoidNegativeLogic) {}
    uint8_t PumpPin;                  ///< Pump relay pin
    bool PumpPinNegativeLogic;        ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    uint8_t BypassSolenoidPin;        ///< Bypass solenoid relay pin [optional]
    bool BypassSolenoidNegativeLogic; ///< Set to true if Relay/MOSFET controlling the power to the solenoid requires LOW signal to Turn ON [optional]
    bool PumpEnabled;                 ///< Enable/disable pump. false= Block running the pump
    uint8_t Speed;                    ///< Duty cycle of the PWM Motor speed
    uint8_t SpeedLowLimit;            ///< Duty cycle limit, does not allow lowering the speed too much. Avoids stalling the motor
    uint16_t PumpTimeOut;             ///< (Sec) Max pump run time
    int PrimingTime;                  ///< (Sec) Only if BypassSolenoid is present. For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
    int BlowOffTime;                  ///< (Sec) Only if BypassSolenoid is present. For how long to open the bypass solenoid on after turning the pump off - Release pressure from pump discharge side
  };
  struct WaterPumpSettings HempyPump1 = {.PumpPin = 3, .PumpPinNegativeLogic = false, .PumpEnabled = true, .Speed = 100, .SpeedLowLimit = 30, .PumpTimeOut = 420}; ///< Pumps do not need a bypass solenoid
  struct WaterPumpSettings HempyPump2 = {.PumpPin = 5, .PumpPinNegativeLogic = false, .PumpEnabled = true, .Speed = 100, .SpeedLowLimit = 30, .PumpTimeOut = 420}; ///< Pumps do not need a bypass solenoid

  struct WeightSensorSettings ///< WeightSensor default settings
  {
    WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, long Offset = 0, float Scale = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Offset(Offset), Scale(Scale) {}
    uint8_t DTPin;  ///< Weight sensor DT pin
    uint8_t SCKPin; ///< Weight sensor SCK pin
    long Offset;    ///< Reading at 0 weight on the scale
    float Scale;    ///< Scale factor
  };
  struct WeightSensorSettings WeightB1 = {.DTPin = 4, .SCKPin = 6, .Offset = 382746, .Scale = -21787.00};   ///< Bucket 1 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyBucketPlatforms/Test-WeightSensor_HempyBucketPlatforms.ino
  struct WeightSensorSettings WeightB2 = {.DTPin = 7, .SCKPin = 8, .Offset = -192194, .Scale = -20213.20};    ///< Bucket 2 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyBucketPlatforms/Test-WeightSensor_HempyBucketPlatforms.ino
  struct WeightSensorSettings WeightWR1 = {.DTPin = A0, .SCKPin = A1, .Offset = -64705, .Scale = -22366.30}; ///< Waste Reservoir 1 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyWastePlatforms/Test-WeightSensor_HempyWastePlatforms.ino
  struct WeightSensorSettings WeightWR2 = {.DTPin = A2, .SCKPin = A3, .Offset = 261992, .Scale = -22139.30}; ///< Waste Reservoir 2 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyWastePlatforms/Test-WeightSensor_HempyWastePlatforms.ino

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
