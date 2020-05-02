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

static const uint8_t Version = 20; ///< Increment this when you make a change in the SAVED TO EEPROM secton

///THIS SECTION DOES NOT GET STORED IN EEPROM:

  ///Global constants
  static const uint8_t WirelessChannel[6] = {"Hemp1"};    ///This needs to be unique and match with the Name of the HempyModule_Web object in the Main module

  static const uint8_t MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
  static const uint8_t MaxShotTextLength = 64; ///Default char * buffer for storing mutiple words. Memory intense!
  static const int MaxLongTextLength = 256; ///Default char * buffer for storing a long text. Memory intense!
  static const uint8_t QueueDepth = 8;               ///Limits the maximum number of active modules. Memory intense!
  static const uint8_t RollingAverageDepth = 10;     ///Limits the maximum number of active modules. Memory intense!

  ///Global variables
  extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
  extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
  extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

///SAVED TO EEPROM - Before uploading the schetch increase the Version variable to override whatever is stored in the Arduino's EEPROM 
  typedef struct
  {
    bool Debug = true;          ///Logs debug messages to serial and web outputs
    bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
    
    struct AeroModuleSettings
    {
      //AeroModuleSettings( ) :  {}      
    };
    struct AeroModuleSettings AeroModule1 = {};

    struct HempyBucketSettings
    {
      HempyBucketSettings( float StartWeight = 0.0, float StopWeight = 0.0) : StartWeight(StartWeight), StopWeight(StopWeight)  {}
      float StartWeight; ///Start watering below this weight
      float StopWeight;  ///Stop watering above this weight
    };
    struct HempyBucketSettings Bucket1 = { .StartWeight = 4.2, .StopWeight = 6.9};
    struct HempyBucketSettings Bucket2 = { .StartWeight = 4.2, .StopWeight = 6.9};

    struct HempyModuleSettings{  ///TODO: Remove the parameters
      //HempyModuleSettings() :  {}     
    };  
    struct HempyModuleSettings HempyMod1 = {};  ///Default settings for the Hempy Module

    struct SoundSettings
    {
      SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
      uint8_t Pin;            ///PC buzzer+ (red)
      bool Enabled = true; ///Enable PC speaker / Piezo buzzer
    };
    struct SoundSettings Sound1 = {.Pin = 2};  ///Default settings for the  Sound output

    struct WaterPumpSettings
    {
      WaterPumpSettings(uint8_t Pin = 0, int TimeOut = 0, bool PumpEnabled = false) : Pin(Pin), TimeOut(TimeOut), PumpEnabled(PumpEnabled)  {}
      uint8_t Pin;            ///Hempy bucket watering pump relay pin
      int TimeOut;   ///Max pump run time in seconds
      bool PumpEnabled; ///Enable/disable automatic watering based on weight    
    };
    struct WaterPumpSettings Pump1 = {.Pin = 7, .TimeOut = 120, .PumpEnabled = true};
    struct WaterPumpSettings Pump2 = {.Pin = 8, .TimeOut = 120, .PumpEnabled = true};

    struct WeightSensorSettings
    {
      WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, float Scale = 0.0, long TareOffset = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Scale(Scale), TareOffset(TareOffset) {}
      uint8_t DTPin;     ///Weight sensor DT pin
      uint8_t SCKPin; ///Weight sensor SCK pin
      float Scale;  ///Calibration scale value
      long TareOffset; ///Reading at 0 weight on the scale
    };
    struct WeightSensorSettings Weight1 = {.DTPin = 3, .SCKPin = 4, .Scale = 125000.0, .TareOffset=146000}; ///Default settings for the hempy bucket 1 weight sensor
    struct WeightSensorSettings Weight2 = {.DTPin = 5, .SCKPin = 6, .Scale = 126000.0, .TareOffset=267461}; ///Default settings for the hempy bucket 2 weight sensor

    uint8_t CompatibilityVersion = Version; ///Should always be the last value stored.
  } Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored 

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false );
void restoreDefaults(Settings *ToOverwrite);