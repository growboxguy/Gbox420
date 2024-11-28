#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 17; ///< Increment this after changing the stucture of the SAVED TO EEPROM section to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;        ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 128;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1024; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t LogDepth = 4;              ///< Show X number of log entries on website. Be careful, Max 1024 bits can be passed during a Website Refresh/Load event
static const uint8_t QueueDepth = 32;           ///< Limits the maximum number of components within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;   ///< Number of previous readings to keep when calculating average. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 49;             ///< nRF24l01+ wireless transmitter CSN pin
static const uint8_t WirelessCEPin = 53;              ///< nRF24l01+ wireless transmitter CE pin
static const uint8_t WirelessDelay = 8;               ///< How long to wait between each retry (250ms increments), Max 15. 0 means 250us, 15 means 4000us,
static const uint8_t WirelessRetry = 10;              ///< How many retries before giving up, max 15
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
static const uint16_t WirelessReceiveTimeout = 65000; ///< (ms) Consider a module offline after this timeout. Should be a few seconds longer then the WirelessReceiveTimeout configured on the Modules

///< PUSHINGBOX REST API
#define PushingBoxLogRelayID "v755877CF53383E1" ///< UPDATE THIS DeviceID of the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging

///< MQTT Server Settings - The actual MQTT server is configured on the ESP-link web interface REST/MQTT tab
#define MqttPubTopic "Gbox420/"          ///< Publish MQTT messages to this topic. Ends with a forward slash
#define MqttSubTopic "Gbox420CMD/#"      ///< Subscribe to messages of this topic and all sub-topic
#define MqttLwtTopic "Gbox420LWT/"       ///< When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
#define MqttLwtMessage "Gbox420 Offline" ///< Subscribers will get this message under the topic specified by MqttLwtTopic when the MQTT client goes offline

/*
///< Home Assistant REST API - Since HA requires HTTPS this not works - ESPLink doest not support REST over HTTPS
#define HomeAssistantServerIP "192.168.1.100"               ///< Address of Home Assistant server ip
#define HomeAssistantServerPort 8123                        ///< Address of Home Assistant server port
#define HomeAssistantServerURL "/api/states/sensor.gbox420" ///< Where to send the JSON formatted Log containing sensor readings
#define HomeAssistantServerToken "Authorization: Bearer YOUR-TOKEN" ///< Generate a token in Home Assistant - USERNAME- Security - Long-lived access tokens
*/

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct AeroModuleSettings ///< AeroModule default settings
  {
    bool PressureTankPresent; ///< Is there a pressure tank connected or not
    float Duration;           ///< Spray length in seconds (Actual duration is ~0.5sec longer due to thread + solenoid delay)
    uint16_t DayInterval;     ///< Spray every X minutes - When the lights are ON
    uint16_t NightInterval;   ///< Spray every X minutes - When the lights are OFF
    uint8_t PumpSpeed;        ///< Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut;     ///< (Sec) Max pump run time
    uint16_t PrimingTime;     ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Removes air bubbles from the lines
    float MaxPressure;        ///< Turn off pump above this pressure
    float MinPressure;        ///< Turn on pump below this pressure
  };
  struct AeroModuleSettings AeroModule1 = {.PressureTankPresent = false, .Duration = 3.0, .DayInterval = 6, .NightInterval = 10, .PumpSpeed = 100, .PumpTimeOut = 420, .PrimingTime = 10, .MaxPressure = 7.0, .MinPressure = 5.0};

  struct ACMotorModuleSettings ///< AeroModule default settings
  {
    uint8_t Speed; ///< PWM duty cycle to adjust motor speed
  };
  struct ACMotorModuleSettings ACMotor1 = {.Speed = 50};

  struct AirPumpSettings ///< AirPump default settings
  {
    uint8_t Pin; ///< Relay pin controlling AC power to the air pump
    bool State;  ///< true - ON, false - OFF
  };
  struct AirPumpSettings Ap1 = {.Pin = 23, .State = true};

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    uint8_t Pin;  ///< DAT pin of the DHT sensor
    uint8_t Type; ///< Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct FanSettings ///< Fan default settings
  {
    uint8_t OnOffPin; ///< Relay pin controlling AC power
    uint8_t SpeedPin; ///< Relay pin for speed selection
    bool State;       ///< true - ON, false - OFF
    bool HighSpeed;   ///< true - High speed, false - Low speed
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
    bool SerialReportDate;         ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;       ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSON;         ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportJSONFriendly; ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
    bool SerialReportWireless;     ///< Enable/disable sending wireless package exchange reports to the Serial output
    bool ReportToGoogleSheets;     ///< Enable/disable reporting sensor readings to Google Sheets
    uint16_t SheetsReportingFrequency; ///< How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    bool ReportToMqtt;                 ///< Enable/disable reporting sensor readings to an MQTT broker
    uint16_t MQTTReportFrequency;      ///< How often to report to MQTT. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
  };
  struct MainModuleSettings Main1 = {.SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless = true, .ReportToGoogleSheets = false, .SheetsReportingFrequency = 30, .ReportToMqtt = true, .MQTTReportFrequency = 5};

  struct HempyModuleSettings ///< Hempy default settings
  {
    float WateringIncrement_B1; ///< (kg/lbs) Amount of water that should be pumped at each watering cycle
    float EvaporationTarget_B1; ///< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float DrainTargetWeight_B1; ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    float MaxWeight_B1;         ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint8_t PumpSpeed_B1;       ///< Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut_B1;    ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint16_t DrainWaitTime_B1;  ///< (sec) How long to wait after watering for the water to drain
    float WateringIncrement_B2; ///< (kg/lbs) Amount of water that should be pumped at each watering cycle
    float EvaporationTarget_B2; ///< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float DrainTargetWeight_B2; ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    float MaxWeight_B2;         ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint8_t PumpSpeed_B2;       ///< Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut_B2;    ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint16_t DrainWaitTime_B2;  ///< (sec) How long to wait after watering for the water to drain
  };
  struct HempyModuleSettings HempyModule1 = {.WateringIncrement_B1 = 0.3, .EvaporationTarget_B1 = 2.0, .DrainTargetWeight_B1 = 0.1, .MaxWeight_B1 = 20, .PumpSpeed_B1 = 35, .PumpTimeOut_B1 = 60, .DrainWaitTime_B1 = 300, .WateringIncrement_B2 = 0.3, .EvaporationTarget_B2 = 2.0, .DrainTargetWeight_B2 = 0.1, .MaxWeight_B2 = 20.0, .PumpSpeed_B2 = 35, .PumpTimeOut_B2 = 60, .DrainWaitTime_B2 = 300};

  struct LightSensorSettings ///< LightSensor default settings
  {
    uint8_t DigitalPin; ///< Light sensor D0 pin
    uint8_t AnalogPin;  ///< Light sensor A0 pin
  };
  struct LightSensorSettings Ls1 = {.DigitalPin = 4, .AnalogPin = A0};

  struct LightsSettings ///< Lights default settings
  {
    bool Status;              ///< Startup status for lights: True-ON / False-OFF
    uint8_t RelayPin;         ///< Relay port controlling AC power to LED driver
    uint8_t DimmingPin;       ///< PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    uint8_t DimmingLimit;     ///< Sets the LED dimming limit (Usually around 5%)
    uint8_t DimmingDuration;  ///< Temporary dimming duration in Seconds
    uint8_t Brightness;       ///< Light intensity: 0 - 100 range for controlling led driver output
    bool TimerEnabled;        ///< Enable/Disable timer controlling lights
    uint8_t OnHour;           ///< Light ON time - hour
    uint8_t OnMinute;         ///< Light ON time - minute
    uint8_t OffHour;          ///< Light OFF time - hour
    uint8_t OffMinute;        ///< Light OFF time - minute
    bool FadingEnabled;       ///< Enables/disables fading lights in when turning on, and fading lights out when turning off <Not exposed to Web interface>
    uint16_t FadingInterval;  ///< (Sec) How often should the brightness change during a fade in/out <Not exposed to Web interface>
    uint8_t FadingIncrements; ///< How much to change the brightness during a fade in/out in Percentage <Not exposed to Web interface>
  };
  struct LightsSettings Lt1 = {.Status = false, .RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 16, .DimmingDuration = 10, .Brightness = 75, .TimerEnabled = true, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingEnabled = false, .FadingInterval = 1, .FadingIncrements = 1}; ///< Creating a LightSettings instance, passing in the unique parameters
  struct LightsSettings Lt2 = {.Status = false, .RelayPin = 24, .DimmingPin = 12, .DimmingLimit = 6, .DimmingDuration = 10, .Brightness = 55, .TimerEnabled = false, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingEnabled = false, .FadingInterval = 1, .FadingIncrements = 1}; ///< Creating a LightSettings instance, passing in the unique parameters

  struct ReservoirModuleSettings ///< ReservoirModule default settings
  {
    // ReservoirModuleSettings() :  {}
  };
  struct ReservoirModuleSettings ReservoirMod1 = {};

  struct SoundSettings ///< Sound default settings
  {
    uint8_t Pin;  ///< Piezo Buzzer red(+) cable
    bool Enabled; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2, .Enabled = true};

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