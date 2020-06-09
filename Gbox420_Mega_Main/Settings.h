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

static const uint8_t Version = 4; ///< Increment this when you make a change in the SAVED TO EEPROM secton

///State machine - Defining possible states
enum PumpState {DISABLED, IDLE, PRIMING, RUNNING, BLOWOFF, MIXING};
//enum HempyState { DRY, WATERING};
//enum AeroState { SPRAYING };

///THIS SECTION DOES NOT GET STORED IN EEPROM:
  ///Global constants
  static const uint8_t MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
  static const uint8_t MaxShotTextLength = 128; ///Default char * buffer for storing mutiple words. Memory intense!
  static const int MaxLongTextLength = 1024; ///Default char * buffer for storing a long text. Memory intense!
  static const uint8_t LogDepth = 5;                  ///Show X number of log entries on website. Be careful, Max 1024uint8_ts can be passed during a Website refresh event, incuding all parameters passed
  static const uint8_t QueueDepth = 32;               ///Limits the maximum number of active modules. Memory intense!
  static const uint8_t RollingAverageDepth = 10;               ///Limits the maximum number of active modules. Memory intense!

  ///Global variables
  extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
  extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
  extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

  ///nRF24L01+ wireless receiver
  static const uint8_t Wireless_CSNPin = 49;
  static const uint8_t Wireless_MISOPin = 50;
  static const uint8_t Wireless_MOSIPin = 51;
  static const uint8_t Wireless_SCKPin = 52;
  static const uint8_t Wireless_CEPin = 53;
  static const uint8_t Wireless_Delay = 6;  ///< How long to wait between each retry, in multiples of 250us. Max is 15. 0 means 250us, 15 means 4000us
  static const uint8_t Wireless_Retry = 5;  ///< How many retries before giving up, max 15

///SAVED TO EEPROM - Settings struct
  ///If you change things here, increase the Version variable in line 4
  typedef struct
  {
    bool Debug = true;          ///Logs debug messages to serial and web outputs
    bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
    char PushingBoxLogRelayID[MaxTextLength] = {"v755877CF53383E1"};   ///UPDATE THIS DeviceID of the PushingBox logging scenario 

    struct AeroModuleSettings
    {
      AeroModuleSettings(bool PressureTankPresent = false, int Interval = 0, int Duration = 0, int PumpTimeOut = 0, int PrimingTime =0, float MaxPressure = 0.0, float MinPressure = 0.0) : PressureTankPresent(PressureTankPresent), Interval(Interval), Duration(Duration), PumpTimeOut(PumpTimeOut), PrimingTime(PrimingTime), MaxPressure(MaxPressure), MinPressure(MinPressure) {}
      bool PressureTankPresent; 
      int Interval;        ///Spray every X minutes
      int Duration;        ///Spray time in seconds
      int PumpTimeOut;   ///< (Sec) Max pump run time        
      int PrimingTime;    ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Remove air bubbles from pump intake side
      float MaxPressure; ///Turn off pump above this pressure
      float MinPressure;  ///Turn on pump below this pressure     
    };
    struct AeroModuleSettings AeroModule1 = {.PressureTankPresent = false, .Interval=15, .Duration = 10, .PumpTimeOut = 120, .PrimingTime = 10, .MaxPressure = 7.0, .MinPressure = 5.0};
    
    struct DHTSensorSettings
    { ///initialized via Designated initializer https:///riptutorial.com/c/example/18609/using-designated-initializers
      DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
      uint8_t Pin;
      uint8_t Type; ///Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
    };
    struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

    struct FanSettings
    {
      FanSettings(uint8_t OnOffPin = 0, uint8_t SpeedPin = 0) : OnOffPin(OnOffPin), SpeedPin(SpeedPin) {}
      uint8_t OnOffPin;          ///Relay pin for power
      uint8_t SpeedPin;          ///Relay pin for speed selection
      bool State = true;      ///true - ON, false - OFF
      bool HighSpeed = false; ///true - High speed, false - Low speed
    };
    struct FanSettings IFan = {.OnOffPin = 25, .SpeedPin = 26};
    struct FanSettings EFan = {.OnOffPin = 27, .SpeedPin = 28};

    struct MainModuleSettings{
      MainModuleSettings(bool ReportToGoogleSheets, uint8_t SheetsReportingFrequency) : ReportToGoogleSheets(ReportToGoogleSheets) , SheetsReportingFrequency(SheetsReportingFrequency) {} 
      bool ReportToGoogleSheets;  ///Enable/disable reporting sensor readings to Google Sheets
      uint8_t SheetsReportingFrequency; ///How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
      ///bool ReportToMqtt = true;    ///Controls reporting sensor readings to an MQTT broker
    };
    struct MainModuleSettings Main1 = {.ReportToGoogleSheets = true, .SheetsReportingFrequency = 30};
          
    struct HempyModuleSettings
    {
      HempyModuleSettings( float StartWeightBucket_B1 = 0.0, float StopWeightBucket_B1 = 0.0, int TimeOutPump_B1 = 0, float StartWeightBucket_B2 = 0.0, float StopWeightBucket_B2 = 0.0, int TimeOutPump_B2 = 0 ) : StartWeightBucket_B1(StartWeightBucket_B1), StopWeightBucket_B1(StopWeightBucket_B1), TimeOutPump_B1(TimeOutPump_B1), StartWeightBucket_B2(StartWeightBucket_B2), StopWeightBucket_B2(StopWeightBucket_B2), TimeOutPump_B2(TimeOutPump_B2) {}
      float StartWeightBucket_B1; ///Start watering below this weight
      float StopWeightBucket_B1;  ///Stop watering above this weight
      int TimeOutPump_B1;  ///Max pump runtime in seconds, target StopWeight should be reached before hitting this. Pump gets disabled if timeout is reached /// \todo Add email alert when pump fails
      float StartWeightBucket_B2; ///Start watering below this weight
      float StopWeightBucket_B2;  ///Stop watering above this weight   
      int TimeOutPump_B2;  
    };
    struct HempyModuleSettings HempyModule1 = {.StartWeightBucket_B1 = 4.2, .StopWeightBucket_B1 = 6.9, .TimeOutPump_B1 = 120, .StartWeightBucket_B2 = 4.2, .StopWeightBucket_B2 = 6.9, .TimeOutPump_B2 = 120};
    
    struct LightSensorSettings
    {
      LightSensorSettings(uint8_t DigitalPin = 0, uint8_t AnalogPin = 0) : DigitalPin(DigitalPin), AnalogPin(AnalogPin) {}
      uint8_t DigitalPin;
      uint8_t AnalogPin;
    };
    struct LightSensorSettings LtSen1 = {.DigitalPin = 4, .AnalogPin = A0};

    struct LightsSettings
    {
      LightsSettings(uint8_t RelayPin = 0, uint8_t DimmingPin = 0, uint8_t DimmingLimit = 0) : RelayPin(RelayPin), DimmingPin(DimmingPin), DimmingLimit(DimmingLimit) {} ///Designated initializer
      uint8_t RelayPin;                                                                                                                                            ///Power relay Port 8 - LED lights
      uint8_t DimmingPin;                                                                                                                                          ///PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
      uint8_t DimmingLimit;                                                                                                                                        ///Sets the LED dimming limit (Usually around 5%)
      bool Status = true;                                                                                                                                       ///Startup status for lights: True-ON / False-OFF
      uint8_t Brightness = 15;                                                                                                                                     ///Light intensity: 0 - 100 range for controlling led driver output
      bool TimerEnabled = true;                                                                                                                                 ///Enable timer controlling lights
      uint8_t OnHour = 4;                                                                                                                                          ///Light ON time - hour
      uint8_t OnMinute = 20;                                                                                                                                       ///Light ON time - minute
      uint8_t OffHour = 16;                                                                                                                                        ///Light OFF time - hour
      uint8_t OffMinute = 20;                                                                                                                                      ///Light OFF time - minute
    };
    struct LightsSettings Lt1 = {.RelayPin = 29, .DimmingPin = 5, .DimmingLimit = 8}; ///Creating a LightSettings instance, passing in the unique parameters

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
    struct SoundSettings Sound1 = {.Pin = 2};   

    uint8_t CompatibilityVersion = Version; ///Should always be the last value stored.
  } Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored 

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false );
void restoreDefaults(Settings *ToOverwrite);