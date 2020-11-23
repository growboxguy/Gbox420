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

static const uint8_t Version = 8; ///< Increment this when you make a change in the SAVED TO EEPROM section

///State machine - Defining possible states
  enum PumpStates {DISABLED, IDLE, PRIMING, RUNNING, BLOWOFF, MIXING};
  enum ReservoirState {EMPTY, LEVEL_LOW, OK};
  //enum HempyState { DRY, WATERING};
  //enum AeroState { SPRAYING };

///THIS SECTION DOES NOT GET STORED IN EEPROM:

  ///Global constants
  static const uint8_t MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
  static const uint8_t MaxShotTextLength = 64; ///Default char * buffer for storing mutiple words. Memory intense!
  static const int MaxLongTextLength = 256; ///Default char * buffer for storing a long text. Memory intense!
  static const uint8_t QueueDepth = 8;               ///Memory intense!
  static const uint8_t RollingAverageDepth = 10;     ///Memory intense!

  ///Global variables
  extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
  extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
  extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

  ///nRF24L01+ wireless receiver
  static const uint8_t WirelessCSNPin = 9;   //< Pre-connected on RF-Nano
  static const uint8_t WirelessCEPin = 10;   //< Pre-connected on RF-Nano
  static const uint8_t WirelessChannel[6] = {"Res1"};    ///This needs to be unique and match with the Name of the HempyModule_Web object in the Main module
  static const uint8_t WirelessPayloadSize = 32; //Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
  static const uint16_t WirelessMessageTimeout = 500; //Default 0.5sec -  One package should be exchanged within this timeout (Including retries and delays)

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

    struct PHSensorSettings
    {
      PHSensorSettings(uint8_t Pin = 0, float Slope = 0.0, float Intercept = 0.0) : Pin(Pin), Slope(Slope), Intercept(Intercept) {}
      uint8_t Pin;
      float Slope;
      float Intercept;
    };
    struct PHSensorSettings PHSen1 = {.Pin = A0, .Slope = -0.031199, .Intercept = 22.557617}; ///Update this to your own PH meter calibration values
   
    struct ReservoirModuleSettings{
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
  
    struct WaterTempSensorSettings
    {
      WaterTempSensorSettings(uint8_t Pin = 0) : Pin(Pin) {}
      uint8_t Pin;
    };
    struct WaterTempSensorSettings WTemp1 = {.Pin = 4}; ///Data(yellow) - DS18B20 waterproof temp sensor

    struct WeightSensorSettings
    {
     WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, long Offset = 0, float Scale = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Offset(Offset), Scale(Scale) {}
      uint8_t DTPin;     ///Weight sensor DT pin
      uint8_t SCKPin; ///Weight sensor SCK pin
      long Offset; ///Reading at 0 weight on the scale
      float Scale;  ///Scale factor      
    };
    struct WeightSensorSettings Weight1 = {.DTPin = 5, .SCKPin = 6, .Offset = 255457, .Scale = -19615.00}; ///Update the calibration values here for the Weight Sensor
      
    uint8_t CompatibilityVersion = Version; ///Version should always be the last value stored in the struct
  } Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored 

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false );
void restoreDefaults(Settings *ToOverwrite);