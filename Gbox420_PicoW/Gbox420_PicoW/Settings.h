#include "pico/stdlib.h"
#include <stdio.h>

#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 1; ///< Increment this after changing the stucture of the SAVED TO EEPROM secton to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
#define WIFI_SSID "GboxNet"                     // UPDATE THIS
#define WIFI_PASSWORD "SuperSecretPassword"     // UPDATE THIS
#define NTP_SERVER "pool.ntp.org"               // NTP Server
#define TIMEZONEDIFFERENCE 1                    // UTC time and current timezone difference
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

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  char PushingBoxLogRelayID[MaxWordLength] = {"v755877CF53383E1"}; ///< UPDATE THIS DeviceID of the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging
  char MqttPubTopic[MaxShotTextLength] = {"Gbox420/Hempy/"};       ///< Publish MQTT messages to this topic. Ends with a forward slash
  char MqttSubTopic[MaxShotTextLength] = {"Gbox420CMD/Hempy/#"};   ///< Subscribe to messages of this topic and all sub-topic
  char MqttLwtTopic[MaxShotTextLength] = {"Gbox420LWT/Hempy/"};    ///< When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
  char MqttLwtMessage[MaxWordLength] = {"Hempy Offline"};          ///< this is the message subscribers will get under the topic specified by MqttLwtTopic variable when the MQTT client unexpectedly goes offline

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    uint8_t Pin;  ///< DAT pin of the DHT sensor
    uint8_t Type; ///< Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct Hempy_StandaloneSettings ///< Dev module default settings
  {
    uint16_t SerialReportFrequency;    ///< How often to report to Serial console. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
    bool SerialReportDate;             ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;           ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSON;             ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportJSONFriendly;     ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
    bool SerialReportWireless;         ///< Enable/disable sending wireless package exchange reports to the Serial output
    bool ReportToGoogleSheets;         ///< Enable/disable reporting sensor readings to Google Sheets
    uint16_t SheetsReportingFrequency; ///< How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    bool ReportToMQTT;                 ///< Enable/disable reporting sensor readings to an MQTT broker
    uint16_t MQTTReportFrequency;      ///< How often to report to MQTT. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
  };
  struct Hempy_StandaloneSettings Hempy_Standalone1 = {.SerialReportFrequency = 15, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless = true, .ReportToGoogleSheets = true, .SheetsReportingFrequency = 30, .ReportToMQTT = true, .MQTTReportFrequency = 5};

  struct HempyBucketSettings ///< HempyBucket default settings
  {
    float EvaporationTarget; //< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float OverflowTarget;    //< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    float InitialDryWeight;  ///< (kg/lbs) When the module starts up start watering if Bucket weight is below this. Set to 0 to instantly start watering until OverflowTarget is reached.
    uint16_t DrainWaitTime;  ///< (sec) How long to wait after watering for the water to drain
  };
  struct HempyBucketSettings Bucket1 = {.EvaporationTarget = 2.0, .OverflowTarget = 0.3, .InitialDryWeight = 18.0, .DrainWaitTime = 180};
  struct HempyBucketSettings Bucket2 = {.EvaporationTarget = 2.0, .OverflowTarget = 0.3, .InitialDryWeight = 18.0, .DrainWaitTime = 180};

  struct SoundSettings ///< Sound default settings
  {
    uint8_t Pin;         ///< Piezo Buzzer red(+) cable
    bool Enabled = true; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2};

  struct WasteReservoirSettings ///< WaterPump default settings
  {
    float WasteLimit; ///< Waste reservoir full weight -> Pump gets disabled if reached
  };
  struct WasteReservoirSettings WR1 = {.WasteLimit = 13.0};

  struct WaterPumpSettings ///< WaterPump default settings
  {
    uint8_t PumpPin;           ///< Pump relay pin
    bool PumpPinNegativeLogic; ///< true - Relay turns on to LOW signal, false - Relay turns on to HIGH signal
    bool PumpEnabled;          ///< Enable/disable pump. false= Block running the pump
    uint16_t PumpTimeOut;      ///< (Sec) Max pump run time
    uint8_t Speed;             ///< Duty cycle of the PWM Motor speed
    uint8_t SpeedLowLimit;     ///< Duty cycle limit, does not allow lowering the speed too much. Avoids stalling the motor
  };
  struct WaterPumpSettings B1P = {.PumpPin = 3, .PumpPinNegativeLogic = false, .PumpEnabled = true, .PumpTimeOut = 120, .Speed = 100, .SpeedLowLimit = 30};
  struct WaterPumpSettings B2P = {.PumpPin = 5, .PumpPinNegativeLogic = false, .PumpEnabled = true, .PumpTimeOut = 120, .Speed = 100, .SpeedLowLimit = 30};

  struct WeightSensorSettings ///< WeightSensor default settings
  {
    uint8_t DTPin;  ///< Weight sensor DT pin
    uint8_t SCKPin; ///< Weight sensor SCK pin
    long Offset;    ///< Reading at 0 weight on the scale
    float Scale;    ///< Scale factor
  };
  struct WeightSensorSettings NRW = {.DTPin = 24, .SCKPin = 25, .Offset = -76242, .Scale = -22686.00}; ///< Nutrient Reservoir Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyBucketPlatforms/Test-WeightSensor_HempyBucketPlatforms.ino
  struct WeightSensorSettings WRW = {.DTPin = 26, .SCKPin = 27, .Offset = 154450, .Scale = 95451.25};  ///< Waste Reservoir Weight Sensor
  struct WeightSensorSettings B1W = {.DTPin = 28, .SCKPin = 29, .Offset = -76382, .Scale = -22697.10}; ///< Bucket 1 Weight Sensor
  struct WeightSensorSettings B2W = {.DTPin = 30, .SCKPin = 31, .Offset = 260682, .Scale = -22084.60}; ///< Bucket 2 Weight Sensor

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