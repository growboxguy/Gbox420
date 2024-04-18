#include "pico/stdlib.h"
#include <stdio.h>

#pragma once

/*!
 *  \brief     Default Settings for each component within the module. Loaded when the Pico W starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory!
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 1; ///< Increment this after changing the stucture of the SAVED TO EEPROM section to force overwriting the stored settings in EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
#define WIFI_SSID "GboxNet"                 ///< UPDATE THIS
#define WIFI_PASSWORD "SuperSecretPassword" ///< UPDATE THIS
#define WIFI_TIMEOUT 15                     ///< In seconds. WiFi connection attempt timeout, connectivity check frequency, retry frequency
#define DNS_TIMEOUT 30                      ///< In seconds. DNS lookup timeout

static const uint8_t MaxWordLength = 32;        ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 128;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1024; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t LogDepth = 4;              ///< Show X number of log entries on website. Be careful, Max 1024 bits can be passed during a Website Refresh/Load event
static const uint8_t QueueDepth = 32;           ///< Limits the maximum number of components within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;   ///< Number of previous readings to keep when calculating average. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];     // Temp storage for assembling long messages (REST API - Google Sheets reporting) //TODO Use Mutex to protect the content getting overwritten by a parallel write to LongMessage
extern char ShortMessage[MaxShotTextLength];    // Temp storage for assembling short messages (Log entries, Error messages) //TODO Use Mutex to protect the content getting overwritten by a parallel write to LongMessage
extern char CurrentTimeText[MaxShotTextLength]; // Buffer for storing current time in text format

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable

typedef struct
{
  char PushingBoxLogRelayID[MaxWordLength] = {"v755877CF53383E1"}; ///< UPDATE THIS DeviceID of the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging

  struct GboxModuleSettings ///< Stripped down core module only containing a Sound component
  {
    char Name[MaxWordLength];          ///< Must be unique
    bool Debug;  ///< Logs debug messages to serial and web outputs
    bool Metric; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.
  };
  struct GboxModuleSettings Gbox1 = {.Name = "Gbox1", .Debug = true, .Metric = true};

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    uint8_t Pin;  ///< DAT pin of the DHT sensor
    uint8_t Type; ///< Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct HempyModuleSettings ///< Dev module default settings
  {
    char Name[MaxWordLength];          ///< Must be unique
    uint16_t SerialReportFrequency;    ///< How often to report to Serial console. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
    bool SerialReportDate;             ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;           ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSON;             ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportJSONFriendly;     ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
    bool SerialReportWireless;         ///< Enable/disable sending wireless package exchange reports to the Serial output
    bool ReportToGoogleSheets;         ///< Enable/disable reporting sensor readings to Google Sheets
    uint16_t SheetsReportingFrequency; ///< In seconds - How often to report to Google Sheets
    bool ReportToMqtt;                 ///< Enable/disable reporting sensor readings to an MQTT broker
  };
  struct HempyModuleSettings HempyModule1 = {.Name = "Hempy1", .SerialReportFrequency = 15, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless = true, .ReportToGoogleSheets = true, .SheetsReportingFrequency = 1800, .ReportToMqtt = true};

  struct HempyBucketSettings ///< HempyBucket default settings
  {
    float EvaporationTarget; //< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float OverflowTarget;    //< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    float InitialDryWeight;  ///< (kg/lbs) When the module starts up start watering if Bucket weight is below this. Set to 0 to instantly start watering until OverflowTarget is reached.
    uint16_t DrainWaitTime;  ///< (sec) How long to wait after watering for the water to drain
  };
  struct HempyBucketSettings Bucket1 = {.EvaporationTarget = 2.0, .OverflowTarget = 0.3, .InitialDryWeight = 18.0, .DrainWaitTime = 180};
  struct HempyBucketSettings Bucket2 = {.EvaporationTarget = 2.0, .OverflowTarget = 0.3, .InitialDryWeight = 18.0, .DrainWaitTime = 180};

  struct MqttClientSettings ///< MQTT client settings
  {
    char Name[MaxWordLength];               ///< Must be unique
    char MqttServerDNS[MaxWordLength];      ///< MQTT server DNS name, "" to use MqttServerIP instead
    char MqttServerIP[MaxWordLength];       ///< MQTT server IP. Used when MqttServerDNS is empty, or the DNS lookup fails
    uint16_t MqttServerPort;                ///< MQTT server Port
    char MqttServerUser[MaxWordLength];     ///< MQTT server username, "" if not needed
    char MqttServerPassword[MaxWordLength]; ///< MQTT server password, "" if not needed
    uint8_t MqttServerTimeoutSec;           ///< MQTT server connection timeout in seconds, indicating the duration to wait before deeming the server unresponsive
    char ClientID[MaxWordLength];           ///< Should be unique across the MQTT server
    char PubTopic[MaxWordLength];           ///< Publish MQTT messages to this topic. Ends with a forward slash/
    char SubTopic[MaxWordLength];           ///< Subscribe to messages of this topic and all sub-topics
    char LwtTopic[MaxWordLength];           ///< Last Will and Testament topic: When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
    char LwtMessage[MaxWordLength];         ///< Last Will and Testament message: Subscribers will get this message under the topic specified by LwtTopic when the MQTT client goes offline
    bool LwtRetain;                         ///< Last Will and Testament retention: 0:No retention, 1:Broker keeps the message and sends it to future subscribers (recommended)
    bool PublishRetain;                     ///< Should the MQTT server retain Publish messages: 0:No retention (recommended), 1:Broker keeps the message and sends it to future subscribers
    uint8_t QoS;                            ///< Quality of Service levels: 0:No QoS, 1: Broker ensures to send the message to the subscribers (recommended), 2: Broker ensures to send the message to the subscribers exactly once   https://www.hivemq.com/blog/mqtt-essentials-part-6-mqtt-quality-of-service-levels/
    uint32_t KeepAliveSeconds;              ///< Ping the MQTT server every X seconds to keep the connection active
  };
  struct MqttClientSettings HempyMqttServer1 = {.Name = "MQTT1", .MqttServerDNS = "mqttserver.gbox420.net", .MqttServerIP = "192.168.1.100", .MqttServerPort = 1883, .MqttServerUser = "MqttUser", .MqttServerPassword = "SuperSecretPassword", .MqttServerTimeoutSec = 10, .ClientID = "Hempy", .PubTopic = "Gbox420/Hempy/", .SubTopic = "Gbox420CMD/Hempy/#", .LwtTopic = "Gbox420LWT/Hempy/", .LwtMessage = "Hempy Offline", .LwtRetain = true, .PublishRetain = true, .QoS = 1, .KeepAliveSeconds = 30};

  struct NtpClientSettings ///< MQTT client settings
  {
    char Name[MaxWordLength];         ///< Must be unique
    char NtpServerDNS[MaxWordLength]; ///< NTP server DNS name, "" to use MqttServerIP instead
    char NtpServerIP[MaxWordLength];  ///< NTP server IP. Used when MqttServerDNS is empty, or the DNS lookup fails
    uint16_t NtpServerPort;           ///< NTP server Port
    int8_t TimeZoneDifference;        ///< UTC time and current timezone difference
    uint8_t StartupDelaySeconds;      ///< Delay the first NTP request, to allow time for WiFi connection to establish
    uint32_t TimeoutSeconds;          ///< Duration in seconds that an NTP client waits for a response from an NTP server
  };
  struct NtpClientSettings NtpServer1 = {.Name = "NTP1", .NtpServerDNS = "pool.ntp.org", .NtpServerIP = "192.168.1.100", .NtpServerPort = 123, .TimeZoneDifference = -7, .StartupDelaySeconds = 15, .TimeoutSeconds = 15};

  struct SoundSettings ///< Sound default settings
  {
    char Name[MaxWordLength]; ///< Must be unique
    uint8_t Pin;              ///< Piezo Buzzer red(+) cable
    bool Enabled;             ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Name = "Sound1", .Pin = 2, .Enabled = true};

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

void saveSettings(Settings *ToSave);      ///< eeprom_update_block((const void *)ToSave, (void *)0, sizeof(Settings)); // update_block only writes the bytes that chang
Settings *loadSettings(bool ResetEEPROM); ///< Load settings from EEPROM, Force loading the defaults from the sketch and overwriting the EEPROM with it
void restoreDefaults();                   ///< Load sketch default settings into EEPROM
