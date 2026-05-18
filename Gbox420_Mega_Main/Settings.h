#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100,000 cycles. Constantly updating variables inside a loop will wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 17; ///< Increment this after changing the structure of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;        ///< Default char* buffer length for storing a single word + null terminator. Memory intense!
static const uint8_t MaxShortTextLength = 128;   ///< Default char* buffer length for storing multiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1280; ///< Default char* buffer length for storing long text strings. Memory intense!
static const uint8_t LogDepth = 4;              ///< Total number of log entries displayed on the website. Note: A maximum of 1024 bits can be transferred during a Website Refresh/Load event.
static const uint8_t QueueDepth = 32;           ///< Limits the maximum number of components allowed within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;   ///< Number of historical readings retained for calculating moving averages. Memory intense!
static const uint8_t NTPTimeout = 15;           ///< (Sec) Network Time Protocol response timeout window used during the ESP-link reset sequence.

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temporary storage array for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShortTextLength]; // Temporary storage array for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing the current time string in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 49;             ///< nRF24l01+ wireless transmitter CSN pin connection
static const uint8_t WirelessCEPin = 53;              ///< nRF24l01+ wireless transmitter CE pin connection
static const uint8_t WirelessDelay = 8;               ///< Retransmission delay step (250us increments), Max 15. 0 maps to 250us, 15 maps to 4000us.
static const uint8_t WirelessRetry = 10;              ///< Hardware retransmission retry limit before packet drops, Max 15.
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) Complete single package exchange cycle time limit window (including retries and delays).
static const uint16_t WirelessReceiveTimeout = 65000; ///< (ms) Offline designation delay threshold. Should be set several seconds longer than the child module's respective timeout values.

///< PUSHINGBOX REST API
#define PushingBoxLogRelayID "v755877CF53383E1" ///< UPDATE THIS: DeviceID assigned to the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging

///< MQTT Server Settings - The actual MQTT server is configured on the ESP-link web interface REST/MQTT tab
#define MqttPubTopic "Gbox420/"          ///< Topic string for publishing outbound MQTT messages. Must end with a forward slash.
#define MqttSubTopic "Gbox420CMD/#"      ///< Inbound subscription topic filtering messages for this topic and all its sub-topics.
#define MqttLwtTopic "Gbox420LWT/"       ///< Destination topic where the MQTT broker will publish a Last Will and Testament message upon unexpected disconnects. Must end with a forward slash.
#define MqttLwtMessage "Gbox420 Offline" ///< Status string payload published under the MqttLwtTopic when the MQTT client drops offline.

/*
///< Home Assistant REST API - Since HA requires HTTPS this not works - ESPLink doest not support REST over HTTPS
#define HomeAssistantServerIP "192.168.1.100"               ///< Address of Home Assistant server ip
#define HomeAssistantServerPort 8123                        ///< Address of Home Assistant server port
#define HomeAssistantServerURL "/api/states/sensor.gbox420" ///< Where to send the JSON formatted Log containing sensor readings
#define HomeAssistantServerToken "Authorization: Bearer YOUR-TOKEN" ///< Generate a token in Home Assistant - USERNAME- Security - Long-lived access tokens
*/

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct __attribute__((packed))
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial and Metric units. If changed, manually update the default temperature and pressure values below.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct AeroModuleSettings ///< AeroModule default settings
  {
    float Duration;           ///< Spray length in seconds (Actual duration is ~0.5sec longer due to thread + solenoid execution overhead)
    float MaxPressure;        ///< Upper cut-off pressure point where the pump motor disengages
    float MinPressure;        ///< Lower cut-in pressure point where the pump motor engages
    uint16_t DayInterval;     ///< Misted sequence timing interval in minutes maintained when active daytime lighting profiles run
    uint16_t NightInterval;   ///< Misted sequence timing interval in minutes maintained when active nighttime lighting profiles run
    uint16_t PumpTimeOut;     ///< (Sec) Maximum continuous safety timeout window constraint for a single pump run cycle
    uint16_t PrimingTime;     ///< (Sec) Operation duration window assigned to open the bypass priming solenoid at initial startup loops to bleed internal air pockets
    uint8_t PumpSpeed;        ///< Base target PWM motor speed value represented via scale duty cycle constants
    bool PressureTankPresent; ///< Structural setup profile flag identifying if a pneumatic pressure vessel is integrated into the pipe layout
  };
  struct AeroModuleSettings AeroModule1 = {.Duration = 3.0, .MaxPressure = 7.0, .MinPressure = 5.0, .DayInterval = 6, .NightInterval = 10, .PumpTimeOut = 420, .PrimingTime = 10, .PumpSpeed = 100, .PressureTankPresent = true};

  struct ACMotorModuleSettings ///< AeroModule default settings
  {
    uint8_t Speed; ///< Rotational velocity profile parameter set via tracking variable PWM duty cycle constants
  };
  struct ACMotorModuleSettings ACMotor1 = {.Speed = 50};

  struct AirPumpSettings ///< AirPump default settings
  {
    uint8_t Pin; ///< Logic output control line mapped to the AC isolation relay driving the aeration pump
    bool State;  ///< Runtime power status tracker: true maps to active ON, false maps to idle OFF
  };
  struct AirPumpSettings Ap1 = {.Pin = 23, .State = true};

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    uint8_t Pin;  ///< Data (DAT) pin mapped to the DHT sensor
    uint8_t Type; ///< Sensor model type definitions: 11 for DHT11, 12 for DHT12, 21 for DHT21/AM2301, 22 for DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct FanSettings ///< Fan default settings
  {
    uint8_t OnOffPin; ///< Target relay channel pin driving master alternating current routing lines
    uint8_t SpeedPin; ///< Target relay channel pin stepping speed adjustments
    bool State;       ///< Runtime operational state tracking flag: true implies running ON, false implies cut OFF
    bool HighSpeed;   ///< Multi-tap coil speed mapping setting: true commands High range, false commands Low range
  };
  struct FanSettings FanI = {.OnOffPin = 25, .SpeedPin = 26, .State = true, .HighSpeed = true};
  struct FanSettings FanE = {.OnOffPin = 27, .SpeedPin = 28, .State = true, .HighSpeed = true};

  /*
  // PWM adjusted AC signal - Need to move this to a dedicated module, Mega already uses interrupts to talk to ESP-link and it messes with counting the phase zero crossings
  struct Fan_PWMSettings ///< Fan default settings
  {
    uint8_t ZeroCrossingPin; ///< On Arduino Mega2560 and Nano this has to be D2 pin
    uint8_t PWMPin;          ///< PWM capable digital pin
    bool State;              ///< ON or OFF
    uint8_t MinSpeed;        ///< Limit the lowest output (%)
    uint8_t Speed;           ///< Speed between 0-100 (%)  (Real output mapped between MinSpeed - 100)
  };
  struct Fan_PWMSettings FanI = {.Status = true, .ZeroCrossingPin = 2, .PWMPin = 9, .State = true, .MinSpeed = 35, .Speed = 80};
  struct Fan_PWMSettings FanE = {.Status = true, .ZeroCrossingPin = 2, .PWMPin = 10, .State = true, .MinSpeed = 35, .Speed = 80};
  */

  struct MainModuleSettings ///< MainModule default settings
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
  struct MainModuleSettings Main1 = {.SheetsReportingFrequency = 30, .MQTTReportFrequency = 5, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless = true, .ReportToGoogleSheets = false, .ReportToMqtt = true};

  struct HempyModuleSettings ///< Hempy default settings
  {
    float StartWeight_B1;       ///< Sub-weight threshold checked at boot for Bucket 1. Triggers immediate watering if current mass falls below this setting.
    float WateringIncrement_B1; ///< (kg/lbs) Exact discrete fluid package dose size delivered per cycle execution block for Bucket 1
    float EvaporationTarget_B1; ///< (kg/lbs) Mass volume of water allowed to evaporate from Bucket 1 substrate before initiating automated watering loops
    float DrainTargetWeight_B1; ///< (kg/lbs) Targeted runoff volume weight tracking parameter required to settle in the runoff tray to validate a completed watering run for Bucket 1
    float MaxWeight_B1;         ///< Safe maximum weight limit configuration threshold for the run-off tray for Bucket 1. Shuts down delivery loops if exceeded.
    float StartWeight_B2;       ///< Sub-weight threshold checked at boot for Bucket 2. Triggers immediate watering if current mass falls below this setting.
    float WateringIncrement_B2; ///< (kg/lbs) Exact discrete fluid package dose size delivered per cycle execution block for Bucket 2
    float EvaporationTarget_B2; ///< (kg/lbs) Mass volume of water allowed to evaporate from Bucket 2 substrate before initiating automated watering loops
    float DrainTargetWeight_B2; ///< (kg/lbs) Targeted runoff volume weight tracking parameter required to settle in the runoff tray to validate a completed watering run for Bucket 2
    float MaxWeight_B2;         ///< Safe maximum weight limit configuration threshold for the run-off tray for Bucket 2. Shuts down delivery loops if exceeded.
    uint16_t PumpTimeOut_B1;     ///< Safe continuous runtime window constraints allotted to the Bucket 1 delivery pump line before emergency trip routines engage
    uint16_t DrainWaitTime_B1;   ///< (sec) Post-feed liquid settlement monitoring period maintained for Bucket 1 to finalize drainage flow patterns
    uint16_t PumpTimeOut_B2;     ///< Safe continuous runtime window constraints allotted to the Bucket 2 delivery pump line before emergency trip routines engage
    uint16_t DrainWaitTime_B2;   ///< (sec) Post-feed liquid settlement monitoring period maintained for Bucket 2 to finalize drainage flow patterns
    uint8_t PumpSpeed_B1;        ///< Operational speed scaling constraint mapped to the Bucket 1 motor driver via tracking PWM duty configuration targets
    uint8_t PumpSpeed_B2;        ///< Operational speed scaling constraint mapped to the Bucket 2 motor driver via tracking PWM duty configuration targets
  };
  struct HempyModuleSettings HempyModule1 = {.StartWeight_B1 = 16, .WateringIncrement_B1 = 0.3, .EvaporationTarget_B1 = 2.0, .DrainTargetWeight_B1 = 0.1, .MaxWeight_B1 = 20, .StartWeight_B2 = 16, .WateringIncrement_B2 = 0.3, .EvaporationTarget_B2 = 2.0, .DrainTargetWeight_B2 = 0.1, .MaxWeight_B2 = 20.0, .PumpTimeOut_B1 = 60, .DrainWaitTime_B1 = 300, .PumpTimeOut_B2 = 60, .DrainWaitTime_B2 = 300, .PumpSpeed_B1 = 35, .PumpSpeed_B2 = 35};

  struct LightSensorSettings ///< LightSensor default settings
  {
    uint8_t DigitalPin; ///< Hardware pin assignment interface listening to the digital output (D0) line of the light detector circuit
    uint8_t AnalogPin;  ///< Hardware pin assignment interface reading variable voltage parameters via the analog (A0) output channel line
  };
  struct LightSensorSettings Ls1 = {.DigitalPin = 4, .AnalogPin = A0};

  struct LightsSettings ///< Lights default settings
  {
    uint16_t FadingInterval;  ///< (Sec) Time delay interval separating step increments during progressive luminous fade-in/fade-out shifts <Not exposed to Web interface>
    uint8_t RelayPin;         ///< Main power isolation loop control pin commanding lines to the primary solid-state/magnetic LED ballast drivers
    uint8_t DimmingPin;       ///< High-speed timer pin running hardware PWM to drive integrated analog optocoupler base steps
    uint8_t DimmingLimit;     ///< Minimum low threshold clamping limit configuration safeguarding the ballasts from drop-out or flickering artifacts (Typically ~5%)
    uint8_t DimmingDuration;  ///< Target transient window track count for step variations scaled across seconds units boundaries
    uint8_t Brightness;       ///< Operating luminous output command vector mapped explicitly across a normalized 0 to 100 percentage calculation array
    uint8_t OnHour;           ///< Hour marker specification component defining when the automated scheduler initiates the lighting period
    uint8_t OnMinute;         ///< Minute marker specification component defining when the automated scheduler initiates the lighting period
    uint8_t OffHour;          ///< Hour marker specification component defining when the automated scheduler terminates the lighting period
    uint8_t OffMinute;         ///< Minute marker specification component defining when the automated scheduler terminates the lighting period
    uint8_t FadingIncrements; ///< Percentage adjustments calculated for brightness modifications inside singular step transformations <Not exposed to Web interface>
    bool Status;              ///< Luminous profile state initialization parameter tracker: true corresponds to active ON, false corresponds to idle OFF
    bool TimerEnabled;        ///< Activation switch logic driving structural clock control scripts over raw localized system timing calculations
    bool FadingEnabled;       ///< Soft-start execution toggles running slow progressive brightness transitions instead of instantaneous line steps <Not exposed to Web interface>
  };
  struct LightsSettings Lt1 = {.FadingInterval = 1, .RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 16, .DimmingDuration = 10, .Brightness = 75, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingIncrements = 1, .Status = false, .TimerEnabled = true, .FadingEnabled = false}; ///< Creating a LightSettings instance, passing in the unique parameters
  struct LightsSettings Lt2 = {.FadingInterval = 1, .RelayPin = 24, .DimmingPin = 12, .DimmingLimit = 6, .DimmingDuration = 10, .Brightness = 55, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingIncrements = 1, .Status = false, .TimerEnabled = false, .FadingEnabled = false}; ///< Creating a LightSettings instance, passing in the unique parameters

  struct ReservoirModuleSettings ///< ReservoirModule default settings
  {
    // ReservoirModuleSettings() :  {}
  };
  struct ReservoirModuleSettings ReservoirMod1 = {};

  struct SoundSettings ///< Sound default settings
  {
    uint8_t Pin;  ///< Hardware connection pin for Piezo Buzzer red (+) positive line
    bool Enabled; ///< Enable/Disable audible signaling alerts
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

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