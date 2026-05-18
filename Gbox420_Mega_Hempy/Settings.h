#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100,000 cycles. Constantly updating variables inside a loop will wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 9; ///< Increment this after changing the structure of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;        ///< Default char* buffer length for storing a single word + null terminator. Memory intense!
static const uint8_t MaxShortTextLength = 128;   ///< Default char* buffer length for storing multiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1024; ///< Default char* buffer length for storing long text strings. Memory intense!
static const uint8_t LogDepth = 4;              ///< Total number of log entries displayed on the website. Note: A maximum of 1024 bits can be transferred during a Website Refresh/Load event.
static const uint8_t QueueDepth = 32;           ///< Limits the maximum number of components allowed within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;   ///< Number of historical readings retained for calculating moving averages. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temporary storage array for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShortTextLength]; // Temporary storage array for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing the current time string in text format

///< PUSHINGBOX REST API
#define PushingBoxLogRelayID "v755877CF53383E1" ///< UPDATE THIS: DeviceID assigned to the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging

///< MQTT Server Settings - The actual MQTT server is configured on the ESP-link web interface REST/MQTT tab
#define MqttPubTopic "Gbox420/"          ///< Topic string for publishing outbound MQTT messages. Must end with a forward slash.
#define MqttSubTopic "Gbox420CMD/#"      ///< Inbound subscription topic filtering messages for this topic and all its sub-topics.
#define MqttLwtTopic "Gbox420LWT/"       ///< Destination topic where the MQTT broker will publish a Last Will and Testament message upon unexpected disconnects. Must end with a forward slash.
#define MqttLwtMessage "Gbox420 Offline" ///< Status string payload published under the MqttLwtTopic when the MQTT client drops offline.


///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct __attribute__((packed))
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial and Metric units. If changed, manually update the default temperature and pressure values below.

   struct DHTSensorSettings ///< DHTSensor default settings
  {    
    uint8_t Pin;  ///< Data (DAT) pin mapped to the DHT sensor
    uint8_t Type; ///< Sensor model type definitions: 11 for DHT11, 12 for DHT12, 21 for DHT21/AM2301, 22 for DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct Hempy_StandaloneSettings ///< Dev module default settings
  {
    uint16_t SheetsReportingFrequency; ///< Time interval for reporting data to Google Sheets. Constrain strictly to 15-minute increments! Minimum 15min, Maximum 1440min (1 day).
    uint16_t MQTTReportFrequency;      ///< Time interval for publishing to MQTT. Adjusted in 5-second steps. Minimum 5sec, Maximum 86400sec (1 day).
    bool SerialReportDate;             ///< Enable/disable printing timestamps to the Serial monitor output
    bool SerialReportMemory;           ///< Enable/disable printing available dynamic free memory status to the Serial monitor output
    bool SerialReportJSON;             ///< Enable/disable streaming raw JSON formatted data strings to the Serial monitor output
    bool SerialReportJSONFriendly;     ///< Enable/disable appending human-readable standard units (Sec, %, Min, kg/lbs, etc.) to the Serial JSON output streams
    bool SerialReportWireless;         ///< Enable/disable outputting RF transceiver package exchange statistics to the Serial monitor
    bool ReportToGoogleSheets;         ///< Enable/disable automated dataset uploads to a designated Google Sheets log spreadsheet
    bool ReportToMqtt;                 ///< Enable/disable broadcasting system logs and data points to an external MQTT broker
  };
  struct Hempy_StandaloneSettings Hempy_Standalone1 = {.SheetsReportingFrequency = 30, .MQTTReportFrequency = 5, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless = true, .ReportToGoogleSheets = true, .ReportToMqtt = true};

 struct HempyBucketSettings ///< HempyBucket default settings
  {
    float EvaporationTarget; ///< (kg/lbs) Mass volume of water allowed to evaporate from a substrate medium before triggering a new automated watering routine
    float DrainTargetWeight; ///< (kg/lbs) Targeted runoff volume weight expected to reach the waste reservoir after a completed watering cycle
    float StartWeight;       ///< (kg/lbs) Sub-weight threshold checked at boot. Triggers immediate watering if current mass falls below this setting. Set to 0 to instantly trigger runtime cycles until DrainTargetWeight parameters match.
    uint16_t DrainWaitTime;  ///< (sec) Idle settling delay window maintained directly after water cycles to allow fluid to fully drain into reservoirs
    bool DisabledState;       ///< ADDED: Tracks if the bucket is disabled
    float WateringIncrement; ///< ADDED: Amount of water to add per step
    float MaxWeight;         ///< ADDED: Maximum safety weight limit
  };
  struct HempyBucketSettings Bucket1 = {.EvaporationTarget = 2.0, .DrainTargetWeight = 0.3, .StartWeight = 18.0, .DrainWaitTime = 180, .DisabledState = false, .WateringIncrement = 0.2, .MaxWeight = 25.0};
  struct HempyBucketSettings Bucket2 = {.EvaporationTarget = 2.0, .DrainTargetWeight = 0.3, .StartWeight = 18.0, .DrainWaitTime = 180, .DisabledState = false, .WateringIncrement = 0.2, .MaxWeight = 25.0};

  struct SoundSettings ///< Sound default settings
  {
    uint8_t Pin;  ///< Hardware connection pin for Piezo Buzzer red (+) positive line
    bool Enabled; ///< Enable/Disable audible signaling alerts
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

  struct WasteReservoirSettings ///< WaterPump default settings
  {
    float WasteLimit; ///< Safe absolute structural weight limit of the waste collection tank. Pumping operations freeze completely if this weight limit is exceeded.
  };
  struct WasteReservoirSettings WR1 = {.WasteLimit = 13.0};

  struct WaterPumpSettings ///< WaterPump default settings
  {
    uint16_t PumpTimeOut;       ///< (Sec) Maximum fail-safe continuous runtime window allowed for a single pump operation
    uint8_t PumpPin;           ///< Logic signal interface output pin assigned to the pump driver relay
    uint8_t Speed;             ///< Standard operational PWM speed command setting scaled via duty cycle values
    uint8_t SpeedLimitLow;     ///< Floor safety threshold limit for the PWM duty cycle preventing electrical motor stalls
    uint8_t SpeedLimitHigh;    ///< Ceiling limitation threshold restricting the maximum speed setting to avoid high-rev motor damage
    bool PumpPinNegativeLogic; ///< Drive orientation flags: set to true if relay interfaces trigger on LOW states; set to false if triggered on HIGH states
    bool PumpEnabled;          ///< Master software override switch. Setting to false entirely locks out and halts pump executions
  };
  struct WaterPumpSettings B1P = {.PumpTimeOut = 120, .PumpPin = 3, .Speed = 100, .SpeedLimitLow = 30, .SpeedLimitHigh = 100, .PumpPinNegativeLogic = false, .PumpEnabled = true};
  struct WaterPumpSettings B2P = {.PumpTimeOut = 120, .PumpPin = 5, .Speed = 100, .SpeedLimitLow = 30, .SpeedLimitHigh = 100, .PumpPinNegativeLogic = false, .PumpEnabled = true};

  struct WeightSensorSettings ///< WeightSensor default settings
  {
    long Offset;    ///< Raw analog-to-digital converter reading observed at zero weight conditions on the loading platform
    float Scale;    ///< Calculated proportional calibration slope adjustment scale multiplier factor
    uint8_t DTPin;  ///< Hardware interface connection pin mapped to the data (DT) line of the load cell instrumentation amplifier
    uint8_t SCKPin; ///< Hardware interface connection pin mapped to the clock (SCK) line of the load cell instrumentation amplifier
  };
  struct WeightSensorSettings NRW = {.Offset = -76242, .Scale = -22686.00, .DTPin = 24, .SCKPin = 25}; ///< Nutrient Reservoir Weight Sensor - Generate structural calibration points via tool script: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyBucketPlatforms/Test-WeightSensor_HempyBucketPlatforms.ino
  struct WeightSensorSettings WRW = {.Offset = 154450, .Scale = 95451.25, .DTPin = 26, .SCKPin = 27};  ///< Waste Reservoir Weight Sensor scale interface configuration properties
  struct WeightSensorSettings B1W = {.Offset = -76382, .Scale = -22697.10, .DTPin = 28, .SCKPin = 29}; ///< Bucket 1 scale interface platform configuration properties
  struct WeightSensorSettings B2W = {.Offset = 260682, .Scale = -22084.60, .DTPin = 30, .SCKPin = 31}; ///< Bucket 2 scale interface platform configuration properties

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