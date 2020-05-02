#pragma once

//// @attention Define the preferred default settings here.

///Update the Version when you make change to the structure of the EEPROM stored Settings struct. This will overwrite the EEPROM settings with the sketch defaults.
static const byte Version = 19;
//// @attention Increment this when you make a change in the SAVED TO EEPROM secton

///THIS SECTION DOES NOT GET STORED IN EEPROM:
  ///Global constants
  static const byte WirelessChannel[6] = {"Hemp1"};    ///This needs to be unique and match with the Name of the HempyModule_Web object in the Main module

  static const byte MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
  static const byte MaxShotTextLength = 64; ///Default char * buffer for storing mutiple words. Memory intense!
  static const int MaxLongTextLength = 256; ///Default char * buffer for storing a long text. Memory intense!
  static const byte QueueDepth = 8;               ///Limits the maximum number of active modules. Memory intense!
  static const byte RollingAverageDepth = 10;     ///Limits the maximum number of active modules. Memory intense!

  ///Global variables
  extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
  extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
  extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

///SAVED TO EEPROM - Settings struct
///If you change things here, increase the Version variable to override whatever is stored in the EEPROM
typedef struct
{
  bool Debug = true;          ///Logs debug messages to serial and web outputs
  bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
  
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
    SoundSettings(byte Pin = 0) : Pin(Pin) {}
    byte Pin;            ///PC buzzer+ (red)
    bool Enabled = true; ///Enable PC speaker / Piezo buzzer
  };
  struct SoundSettings Sound1 = {.Pin = 2};  ///Default settings for the  Sound output

  struct WaterPumpSettings
  {
    WaterPumpSettings(byte Pin = 0, int Timeout = 0, bool PumpEnabled = false) : Pin(Pin), Timeout(Timeout), PumpEnabled(PumpEnabled)  {}
    byte Pin;            ///Hempy bucket watering pump relay pin
    int Timeout;   ///Max pump run time in seconds
    bool PumpEnabled; ///Enable/disable automatic watering based on weight    
  };
  struct WaterPumpSettings Pump1 = {.Pin = 7, .Timeout = 120, .PumpEnabled = true};
  struct WaterPumpSettings Pump2 = {.Pin = 8, .Timeout = 120, .PumpEnabled = true};

  struct WeightSensorSettings
  {
    WeightSensorSettings(byte DTPin = 0, byte SCKPin = 0, float Scale = 0.0, long TareOffset = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Scale(Scale), TareOffset(TareOffset) {}
    byte DTPin;     ///Weight sensor DT pin
    byte SCKPin; ///Weight sensor SCK pin
    float Scale;  ///Calibration scale value
    long TareOffset; ///Reading at 0 weight on the scale
  };
  struct WeightSensorSettings Weight1 = {.DTPin = 3, .SCKPin = 4, .Scale = 125000.0, .TareOffset=146000}; ///Default settings for the hempy bucket 1 weight sensor
  struct WeightSensorSettings Weight2 = {.DTPin = 5, .SCKPin = 6, .Scale = 126000.0, .TareOffset=267461}; ///Default settings for the hempy bucket 2 weight sensor

  byte CompatibilityVersion = Version; ///Should always be the last value stored.
} Settings;
