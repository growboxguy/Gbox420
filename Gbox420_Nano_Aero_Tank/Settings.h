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

static const uint8_t Version = 14; ///< Increment this when you make a change in the SAVED TO EEPROM secton

///State machine - Defining possible states
  enum PumpStates {DISABLED, IDLE, PRIMING, RUNNING, BLOWOFF, MIXING};
  //enum HempyState { DRY, WATERING};
  //enum AeroState { SPRAYING };

///THIS SECTION DOES NOT GET STORED IN EEPROM:

  ///Global constants
  static const uint8_t MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
  static const uint8_t MaxShotTextLength = 64; ///Default char * buffer for storing mutiple words. Memory intense!
  static const int MaxLongTextLength = 256; ///Default char * buffer for storing a long text. Memory intense!
  static const uint8_t QueueDepth = 8;               ///Limits the maximum number of active modules. Memory intense!
  static const uint8_t RollingAverageDepth = 10;     ///Limits the maximum number of active modules. Memory intense!

  ///Global variables
  extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
  extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
  extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

  ///nRF24L01+ wireless receiver
  static const uint8_t WirelessCSNPin = 9;   //< Pre-connected on RF-Nano
  static const uint8_t WirelessCEPin = 10;   //< Pre-connected on RF-Nano
  static const uint8_t WirelessChannel[6] = {"Aero1"};    ///This needs to be unique and match with the Name of the HempyModule_Web object in the Main module
  static const uint8_t WirelessPayloadSize = 32; //Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
  static const uint16_t WirelessMessageTimeout = 500; //Default 0.5sec -  One package should be exchanged within this timeout (Including retries and delays)

///SAVED TO EEPROM - Before uploading the schetch increase the Version variable to override whatever is stored in the Arduino's EEPROM 
  typedef struct
  {
    bool Debug = true;          ///Logs debug messages to serial and web outputs
    bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
    
    struct AeroModuleSettings
    {
      AeroModuleSettings(bool PressureTankPresent = false) : PressureTankPresent(PressureTankPresent) {}
      bool PressureTankPresent; 
    };
    struct AeroModuleSettings AeroModule1 = {.PressureTankPresent = true};
 
    struct AeroponicsSettings
    { ///Common settings for both inheriting classes: Aeroponics_Tank and Aeroponics_NoTank
      AeroponicsSettings(bool SprayEnabled = true, int DayInterval = 0, int DayDuration = 0, int NightInterval = 0, int NightDuration = 0, float MaxPressure = 0.0) : SprayEnabled(SprayEnabled), DayInterval(DayInterval), DayDuration(DayDuration), NightInterval(NightInterval), NightDuration(NightDuration), MaxPressure(MaxPressure)   {}
      bool SprayEnabled; ///Enable/disable spraying cycle
      int DayInterval;        ///Spray every X minutes - When the lights are ON
      int DayDuration;        ///Spray time in seconds - When the lights are ON
      int NightInterval;        ///Spray every X minutes - When the lights are OFF
      int NightDuration;        ///Spray time in seconds - When the lights are OFF
      float MaxPressure; ///Turn off pump above this pressure      
    };
    struct AeroponicsSettings AeroT1_Common = {.SprayEnabled= true, .DayInterval=15, .DayDuration = 10, .NightInterval=30, .NightDuration = 8, .MaxPressure = 7.0};
    struct AeroponicsSettings AeroNT1_Common = {.SprayEnabled= true, .DayInterval=15, .DayDuration = 10, .NightInterval=30, .NightDuration = 8, .MaxPressure = 7.0};
    
    struct AeroponicsSettings_TankSpecific
    { ///Settings for an Aeroponics setup WITH a pressure tank
      AeroponicsSettings_TankSpecific(float MinPressure = 0.0, uint8_t SpraySolenoidPin = 0, bool SpraySolenoidNegativeLogic=false) : MinPressure(MinPressure), SpraySolenoidPin(SpraySolenoidPin), SpraySolenoidNegativeLogic(SpraySolenoidNegativeLogic) {}
      float MinPressure;  ///Turn on pump below this pressure  
      uint8_t SpraySolenoidPin;
      bool SpraySolenoidNegativeLogic;
    };
    struct AeroponicsSettings_TankSpecific AeroT1_Specific = {.MinPressure = 5.0, .SpraySolenoidPin = 5, .SpraySolenoidNegativeLogic=true};

    struct PressureSensorSettings
    {
      PressureSensorSettings(uint8_t Pin = 0, float Offset = 0.0, float Ratio = 0.0) : Pin(Pin), Offset(Offset), Ratio(Ratio) {}
      uint8_t Pin;     ///Pressure sensor Pin: Signal(yellow)
      float Offset; ///Pressure sensor calibration: voltage reading at 0 pressure
      float Ratio;  ///Pressure sensor voltage to pressure ratio
    };
    struct PressureSensorSettings Pres1 = {.Pin = A7, .Offset = 0.57, .Ratio = 2.7};

    struct SoundSettings
    {
      SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
      uint8_t Pin;            ///PC buzzer+ (red)
      bool Enabled = true; ///Enable PC speaker / Piezo buzzer
    };
    struct SoundSettings Sound1 = {.Pin = 2};  ///Default settings for the  Sound output  

    struct WaterPumpSettings
    {
      WaterPumpSettings(uint8_t PumpPin = 0, bool PumpPinNegativeLogic = false, uint8_t BypassSolenoidPin = 0, bool BypassSolenoidNegativeLogic = false, bool PumpEnabled = false, uint8_t Speed = 100, uint8_t SpeedLowLimit = 0, int PumpTimeOut = 0, int PrimingTime = 0, int BlowOffTime = 0) : PumpPin(PumpPin), PumpPinNegativeLogic(PumpPinNegativeLogic), BypassSolenoidPin(BypassSolenoidPin), BypassSolenoidNegativeLogic(BypassSolenoidNegativeLogic), PumpEnabled(PumpEnabled), Speed(Speed), SpeedLowLimit(SpeedLowLimit), PumpTimeOut(PumpTimeOut), PrimingTime(PrimingTime), BlowOffTime(BlowOffTime)  {}
      uint8_t PumpPin;         ///< Pump relay pin
      bool PumpPinNegativeLogic;
      uint8_t BypassSolenoidPin;        ///< Bypass solenoid relay pin
      bool BypassSolenoidNegativeLogic;
      bool PumpEnabled; ///< Enable/disable pump. false= Block running the pump
      uint8_t Speed;  ///< Duty cycle of the PWM Motor speed 
      uint8_t SpeedLowLimit;  ///< Duty cycle limit, does not allow lowering the speed too much. Avoids stalling the motor
      int PumpTimeOut;   ///< (Sec) Max pump run time        
      int PrimingTime;    ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
      int BlowOffTime;     ///< (Sec) For how long to open the bypass solenoid on after turning the pump off - Release pressure from pump discharge side
    };
    struct WaterPumpSettings AeroPump1 = {.PumpPin = 3, .PumpPinNegativeLogic= false, .BypassSolenoidPin = 4, .BypassSolenoidNegativeLogic = true, .PumpEnabled = true, .Speed = 70, .SpeedLowLimit = 30, .PumpTimeOut = 120, .PrimingTime = 10, .BlowOffTime = 3};
    
    struct WeightSensorSettings
    {
      WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, long Offset = 0, float Scale = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Offset(Offset), Scale(Scale) {}
      uint8_t DTPin;     ///Weight sensor DT pin
      uint8_t SCKPin; ///Weight sensor SCK pin
      long Offset; ///Reading at 0 weight on the scale
      float Scale;  ///Scale factor      
    };
    struct WeightSensorSettings Weight1 = {.DTPin = A0, .SCKPin = A1, .Offset = 22716, .Scale = -22278.50}; ///Waste Reservoir 1 Weight Sensor - Generate the calibration values using: https://github.com/growboxguy/Gbox420/blob/master/Test_Sketches/Test-WeightSensor_HempyWastePlatforms/Test-WeightSensor_HempyWastePlatforms.ino
    
    uint8_t CompatibilityVersion = Version; ///Should always be the last value stored.
  } Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored 

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false );
void restoreDefaults(Settings *ToOverwrite);