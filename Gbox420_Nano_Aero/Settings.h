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

static const uint8_t Version = 22; ///< Increment this when you make a change in the SAVED TO EEPROM secton

///THIS SECTION DOES NOT GET STORED IN EEPROM:

  ///Global constants
  static const uint8_t WirelessChannel[6] = {"Aero1"};    ///This needs to be unique and match with the Name of the HempyModule_Web object in the Main module

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

 struct AeroponicsSettings
    { ///Common settings for both inheriting classes: Aeroponics_Tank and Aeroponics_NoTank
      AeroponicsSettings(uint8_t BypassSolenoidPin = 0, uint8_t PumpPin = 0) : BypassSolenoidPin(BypassSolenoidPin), PumpPin(PumpPin) {}
      uint8_t BypassSolenoidPin;   ///Aeroponics bypass solenoid relay pin
      uint8_t PumpPin;             ///Aeroponics high pressure pump relay pin
      bool SprayEnabled = true; ///Enable/disable spraying cycle
      int Interval = 15;        ///Spray every X minutes
      int Duration = 10;        ///Spray time in seconds
      int PumpTimeout = 6;      ///Max pump run time in minutes
      int PrimingTime = 10;     ///At pump startup the bypass valve will be open for X seconds to let the pump remove air from the tubes
    };
    struct AeroponicsSettings AeroT1_Common = {.BypassSolenoidPin = 23, .PumpPin = 24};
    struct AeroponicsSettings AeroNT1_Common = {.BypassSolenoidPin = 46, .PumpPin = 47};

    struct AeroponicsSettings_NoTankSpecific  ///<Settings for an Aeroponics setup WITHOUT a pressure tank
    {                           
      AeroponicsSettings_NoTankSpecific(int BlowOffTime, float PressureHigh) : BlowOffTime(BlowOffTime), PressureHigh(PressureHigh) {}
      int BlowOffTime = 3;      ///After spraying open the bypass valve for X seconds to release pressure. Helps to stop spraying immediately
      float PressureHigh = 7.0; ///Safety feature - Turn off pump above this pressure
    };
    struct AeroponicsSettings_NoTankSpecific AeroNT1_Specific = {.BlowOffTime = 3, .PressureHigh = 7.0};

    struct AeroponicsSettings_TankSpecific
    { ///Settings for an Aeroponics setup WITH a pressure tank
      AeroponicsSettings_TankSpecific(uint8_t SpraySolenoidPin = 0) : SpraySolenoidPin(SpraySolenoidPin) {}
      uint8_t SpraySolenoidPin;    ///Spray solenoid relay pin
      float PressureLow = 5.0;  ///Turn on pump below this pressure
      float PressureHigh = 7.0; ///Turn off pump above this pressure
    };
    struct AeroponicsSettings_TankSpecific AeroT1_Specific = {.SpraySolenoidPin = 22};
   
    struct PressureSensorSettings
    {
      PressureSensorSettings(uint8_t Pin = 0, float Offset = 0.0, float Ratio = 0.0) : Pin(Pin), Offset(Offset), Ratio(Ratio) {}
      uint8_t Pin;     ///Pressure sensor Pin: Signal(yellow)
      float Offset; ///Pressure sensor calibration: voltage reading at 0 pressure
      float Ratio;  ///Pressure sensor voltage to pressure ratio
    };
    struct PressureSensorSettings Pres1 = {.Pin = A1, .Offset = 0.57, .Ratio = 2.7};

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

 

    uint8_t CompatibilityVersion = Version; ///Should always be the last value stored.
  } Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored 

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false );
void restoreDefaults(Settings *ToOverwrite);