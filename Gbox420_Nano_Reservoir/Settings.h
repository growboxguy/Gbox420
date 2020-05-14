#pragma once

/*! 
 *  \brief     Use this to change the Default Settings for each component. Loaded when the Arduino starts, updated by user interaction on the website.
 *  \details   The Settings object is stored in EEPROM and kept between reboots. 
 *  \author    GrowBoxGuy
 *  \version   4.20
 *  \warning   Only use these in the setup() function, or when a user initiated change is stored. EEPROM has a write limit of 100.000 cycles
 *  \attention Never use the funtions defined here in loop() or a repeating cycle! EEPROM would wear out very fast
 *  \attention Update the Version number when you make change to the structure in the SAVED TO EEPROM secton. This will overwrite the EEPROM settings with the sketch defaults.
 */

static const uint8_t Version = 5; ///< Increment this when you make a change in the SAVED TO EEPROM section

///State machine - Defining possible states
  enum PumpState {DISABLED, IDLE, PRIMING, RUNNING, BLOWOFF, MIXING};
  enum ReservoirState {EMPTY, LEVEL_LOW, OK};
  //enum HempyState { DRY, WATERING};
  //enum AeroState { SPRAYING };

///THIS SECTION DOES NOT GET STORED IN EEPROM:

  ///Global constants
  static const uint8_t WirelessChannel[6] = {"Res1"};    ///This needs to be unique on each Arduino Nano

  static const uint8_t MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
  static const uint8_t MaxShotTextLength = 64; ///Default char * buffer for storing mutiple words. Memory intense!
  static const int MaxLongTextLength = 256; ///Default char * buffer for storing a long text. Memory intense!
  static const uint8_t QueueDepth = 8;               ///Memory intense!
  static const uint8_t RollingAverageDepth = 10;     ///Memory intense!

  ///Global variables
  extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
  extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
  extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

///SAVED TO EEPROM - Before uploading the schetch increase the Version variable to override whatever is stored in the Arduino's EEPROM 
  typedef struct
  {
    bool Debug = true;          ///Logs debug messages to serial and web outputs
    bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
    
    struct DHTSensorSettings
    { ///initialized via Designated initializer https:///riptutorial.com/c/example/18609/using-designated-initializers
      DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
      uint8_t Pin;
      uint8_t Type; ///Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
    };
    struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

    struct DistanceSensorSettings
    {
      DistanceSensorSettings(uint8_t TriggerPin = 0, uint8_t EchoPin = 0) : TriggerPin(TriggerPin), EchoPin(EchoPin) {}
      uint8_t TriggerPin;
      uint8_t EchoPin;
    };
    struct DistanceSensorSettings Dist1 = {.TriggerPin = 4, .EchoPin = 5};
    
    struct PHSensorSettings
    {
      PHSensorSettings(uint8_t Pin = 0, float Slope = 0.0, float Intercept = 0.0) : Pin(Pin), Slope(Slope), Intercept(Intercept) {}
      uint8_t Pin;
      float Slope;
      float Intercept;
    };
    struct PHSensorSettings PHSen1 = {.Pin = A0, .Slope = -0.033256, .Intercept = 24.08651}; ///Update this to your own PH meter calibration values
   
    struct ReservoirModuleSettings{  ///TODO: Remove the parameters
      //ReservoirModuleSettings() :  {}     
    };  
    struct ReservoirModuleSettings ReservoirMod1 = {};  ///Default settings for the Hempy Module

    struct SoundSettings
    {
      SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
      uint8_t Pin;            ///PC buzzer+ (red)
      bool Enabled = true; ///Enable PC speaker / Piezo buzzer
    };
    struct SoundSettings Sound1 = {.Pin = 2};  ///Default settings for the  Sound output
  
    struct WaterLevelSensorSettings
    {
      WaterLevelSensorSettings(uint8_t Pin_1 = 0, uint8_t Pin_2 = 0, uint8_t Pin_3 = 0, uint8_t Pin_4 = 0) : Pin_1(Pin_1), Pin_2(Pin_2), Pin_3(Pin_3), Pin_4(Pin_4) {}
      uint8_t Pin_1; ///Lowest water level
      uint8_t Pin_2;
      uint8_t Pin_3;
      uint8_t Pin_4; ///Full
    };
    struct WaterLevelSensorSettings WLev1 = {.Pin_1 = A1, .Pin_2 = A2, .Pin_3 = A3, .Pin_4 = A4};

    struct WaterTempSensorSettings
    {
      WaterTempSensorSettings(uint8_t Pin = 0) : Pin(Pin) {}
      uint8_t Pin;
    };
    struct WaterTempSensorSettings WTemp1 = {.Pin = 6}; ///Data(yellow) - DS18B20 waterproof temp sensor

  
    uint8_t CompatibilityVersion = Version; ///Version should always be the last value stored in the struct
  } Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored 

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false );
void restoreDefaults(Settings *ToOverwrite);