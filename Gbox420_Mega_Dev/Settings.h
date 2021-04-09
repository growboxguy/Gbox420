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
static const uint8_t MaxWordLength = 32;        ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 128;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1024; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t LogDepth = 4;              ///< Show X number of log entries on website. Be careful, Max 1024 bits can be passed during a Website Refresh/Load event
static const uint8_t QueueDepth = 32;           ///< Limits the maximum number of components within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;   ///< Smooth out sensor readings by calculating the average of the last X results. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 49;             ///< nRF24l01+ wireless transmitter CSN pin
static const uint8_t WirelessCEPin = 53;              ///< nRF24l01+ wireless transmitter CE pin
static const uint8_t WirelessDelay = 8;               ///< How long to wait between each retry (250ms increments), Max 15. 0 means 250us, 15 means 4000us,
static const uint8_t WirelessRetry = 10;              ///< How many retries before giving up, max 15
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
static const uint16_t WirelessReceiveTimeout = 65000; ///< (ms) Consider a module offline after this timeout. Should be a few seconds longer then the WirelessReceiveTimeout configured on the Modules

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  char PushingBoxLogRelayID[MaxWordLength] = {"v755877CF53383E1"}; ///< UPDATE THIS DeviceID of the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging
  char MqttPubTopic[MaxShotTextLength] = {"Gbox420/"};             ///< Publish MQTT messages to this topic. Ends with a forward slash
  char MqttSubTopic[MaxShotTextLength] = {"Gbox420CMD/#"};         ///< Subscribe to messages of this topic and all sub-topic
  char MqttLwtTopic[MaxShotTextLength] = {"Gbox420LWT/"};          ///< When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
  char MqttLwtMessage[MaxWordLength] = {"Gbox420 Offline"};        ///< this is the message subscribers will get under the topic specified by MqttLwtTopic variable when the MQTT client unexpectedly goes offline

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
    uint8_t Pin;  ///< DAT pin of the DHT sensor
    uint8_t Type; ///< Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct DevModule_WebSettings ///< Dev module default settings
  {
    DevModule_WebSettings(uint16_t SerialReportFrequency = 0, bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSON = true, bool SerialReportJSONFriendly = true, bool SerialReportWireless = true, bool ReportToGoogleSheets = false, uint16_t SheetsReportingFrequency = 0, bool ReportToMQTT = false, uint16_t MQTTReportFrequency = 0) : SerialReportFrequency(SerialReportFrequency), SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSON(SerialReportJSON), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportWireless(SerialReportWireless), ReportToGoogleSheets(ReportToGoogleSheets), SheetsReportingFrequency(SheetsReportingFrequency), ReportToMQTT(ReportToMQTT), MQTTReportFrequency(MQTTReportFrequency) {}
    uint16_t SerialReportFrequency;    ///< How often to report to Serial console. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
    bool SerialReportDate;             ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;           ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSON;           ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportJSONFriendly;           ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
    bool SerialReportWireless;   ///< Enable/disable sending wireless package exchange reports to the Serial output  
    bool ReportToGoogleSheets;         ///< Enable/disable reporting sensor readings to Google Sheets
    uint16_t SheetsReportingFrequency; ///< How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    bool ReportToMQTT;                 ///< Enable/disable reporting sensor readings to an MQTT broker
    uint16_t MQTTReportFrequency;      ///< How often to report to MQTT. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
  };
  struct DevModule_WebSettings DevModule_Web1 = {.SerialReportFrequency = 15, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless=true, .ReportToGoogleSheets = true, .SheetsReportingFrequency = 30, .ReportToMqtt = true, .MQTTReportFrequency = 5};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin;         ///< Piezo Buzzer red(+) cable
    bool Enabled = true; ///< Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2};

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