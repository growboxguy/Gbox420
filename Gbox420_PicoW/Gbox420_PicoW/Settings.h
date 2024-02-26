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
  char MqttPubTopic[MaxShotTextLength] = {"Gbox420/Hempy/"};        ///< Publish MQTT messages to this topic. Ends with a forward slash
  char MqttSubTopic[MaxShotTextLength] = {"Gbox420CMD/Hempy/#"};   ///< Subscribe to messages of this topic and all sub-topic
  char MqttLwtTopic[MaxShotTextLength] = {"Gbox420LWT/Hempy/"};    ///< When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
  char MqttLwtMessage[MaxWordLength] = {"Hempy Offline"};          ///< this is the message subscribers will get under the topic specified by MqttLwtTopic variable when the MQTT client unexpectedly goes offline

  
  struct SoundSettings ///< Sound default settings
  {
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