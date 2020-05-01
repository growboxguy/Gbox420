#pragma once

//// @attention Define the preferred default settings here.

///Update the Version when you make change to the structure of the EEPROM stored Settings struct. This will overwrite the EEPROM settings with the sketch defaults.
static const uint8_t Version = 12;

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

///nRF24L01+ wireless receiver pins
static const uint8_t Wireless_CSNPin = 49;
static const uint8_t Wireless_MISOPin = 50;
static const uint8_t Wireless_MOSIPin = 51;
static const uint8_t Wireless_SCKPin = 52;
static const uint8_t Wireless_CEPin = 53;
static const uint8_t Wireless_Delay = 5;  ///< How long to wait between each retry, in multiples of 250us. Max is 15. 0 means 250us, 15 means 4000us
static const uint8_t Wireless_Retry = 5;  ///< How many retries before giving up, max 15

///SAVED TO EEPROM - Settings struct
///If you change things here, increase the Version variable in line 4
typedef struct
{
  bool Debug = false;          ///Logs debug messages to serial and web outputs
  bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
  char PushingBoxLogRelayID[MaxTextLength] = {"v755877CF53383E1"};   ///UPDATE THIS DeviceID of the PushingBox logging scenario 

  struct GrowModuleSettings{
    GrowModuleSettings(bool ReportToGoogleSheets, uint8_t SheetsReportingFrequency) : ReportToGoogleSheets(ReportToGoogleSheets) , SheetsReportingFrequency(SheetsReportingFrequency) {} 
    bool ReportToGoogleSheets;  ///Enable/disable reporting sensor readings to Google Sheets
    uint8_t SheetsReportingFrequency; ///How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    ///bool ReportToMqtt = true;    ///Controls reporting sensor readings to an MQTT broker
  };
  struct GrowModuleSettings Gbox1 = {.ReportToGoogleSheets = true, .SheetsReportingFrequency = 30};
  

  struct HempyModuleSettings{  /// \TODO Probably remove this
    HempyModuleSettings() {}
   }; 
  struct HempyModuleSettings HempyModule1 = {};

  struct DHTSensorSettings
  { ///initialized via Designated initializer https:///riptutorial.com/c/example/18609/using-designated-initializers
    DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
    uint8_t Pin;
    uint8_t Type; ///Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings IDHT = {.Pin = 43, .Type = 22};
  struct DHTSensorSettings EDHT = {.Pin = 44, .Type = 22};
  struct DHTSensorSettings DHT1 = {.Pin = 44, .Type = 22};

  struct SoundSettings
  {
    SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin;            ///PC buzzer+ (red)
    bool Enabled = true; ///Enable PC speaker / Piezo buzzer
  };
  struct SoundSettings Sound1 = {.Pin = 2};

  struct LightSensorSettings
  {
    LightSensorSettings(uint8_t DigitalPin = 0, uint8_t AnalogPin = 0) : DigitalPin(DigitalPin), AnalogPin(AnalogPin) {}
    uint8_t DigitalPin;
    uint8_t AnalogPin;
  };
  struct LightSensorSettings LtSen1 = {.DigitalPin = 42, .AnalogPin = A0};

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
  struct LightsSettings Lt1 = {.RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 8}; ///Creating a LightSettings instance, passing in the unique parameters

  struct PHSensorSettings
  {
    PHSensorSettings(uint8_t Pin = 0, float Slope = 0.0, float Intercept = 0.0) : Pin(Pin), Slope(Slope), Intercept(Intercept) {}
    uint8_t Pin;
    float Slope;
    float Intercept;
  };
  struct PHSensorSettings PHSensor1 = {.Pin = A3, .Slope = -0.033256, .Intercept = 24.08651}; ///Update this to your own PH meter calibration values

  struct PressureSensorSettings
  {
    PressureSensorSettings(uint8_t Pin = 0, float Offset = 0.0, float Ratio = 0.0) : Pin(Pin), Offset(Offset), Ratio(Ratio) {}
    uint8_t Pin;     ///Pressure sensor Pin: Signal(yellow)
    float Offset; ///Pressure sensor calibration: voltage reading at 0 pressure
    float Ratio;  ///Pressure sensor voltage to pressure ratio
  };
  struct PressureSensorSettings Pres1 = {.Pin = A1, .Offset = 0.57, .Ratio = 2.7};

  struct WeightSensorSettings
  {
    WeightSensorSettings(uint8_t DTPin = 0, uint8_t SCKPin = 0, float Scale = 0.0, long TareOffset = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Scale(Scale), TareOffset(TareOffset) {}
    uint8_t DTPin;     ///Weight sensor DT pin
    uint8_t SCKPin; ///Weight sensor SCK pin
    float Scale;  ///Calibration scale value
    long TareOffset; ///Reading at 0 weight on the scale
  };
  struct WeightSensorSettings Weight1 = {.DTPin = 3, .SCKPin = 4, .Scale = 125000.0, .TareOffset=146000};
  struct WeightSensorSettings Weight2 = {.DTPin = 5, .SCKPin = 6, .Scale = 126000.0, .TareOffset=267461};

  struct HempyBucketSettings
  {
    HempyBucketSettings( float StartWeight = 0.0, float StopWeight = 0.0) : StartWeight(StartWeight), StopWeight(StopWeight)  {}
    float StartWeight; ///Start watering below this weight
    float StopWeight;  ///Stop watering above this weight
  };
  struct HempyBucketSettings Bucket1 = { .StartWeight = 4.2, .StopWeight = 6.9};
  struct HempyBucketSettings Bucket2 = { .StartWeight = 4.2, .StopWeight = 6.9};

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
  struct AeroponicsSettings Aero1_Common = {.BypassSolenoidPin = 46, .PumpPin = 47};

  struct AeroponicsSettings_NoTankSpecific
  {                           ///Settings for an Aeroponics setup WITHOUT a pressure tank
    int BlowOffTime = 3;      ///After spraying open the bypass valve for X seconds to release pressure. Helps to stop spraying immediately
    float PressureHigh = 7.0; ///Safety feature - Turn off pump above this pressure
  } Aero1_Specific;

  struct AeroponicsSettings_TankSpecific
  { ///Settings for an Aeroponics setup WITH a pressure tank
    AeroponicsSettings_TankSpecific(uint8_t SpraySolenoidPin = 0) : SpraySolenoidPin(SpraySolenoidPin) {}
    uint8_t SpraySolenoidPin;    ///Spray solenoid relay pin
    float PressureLow = 5.0;  ///Turn on pump below this pressure
    float PressureHigh = 7.0; ///Turn off pump above this pressure
  };
  struct AeroponicsSettings_TankSpecific AeroT1_Specific = {.SpraySolenoidPin = 22};

  struct WaterPumpSettings
  {
    WaterPumpSettings(uint8_t Pin = 0) : Pin(Pin)  {}
    uint8_t Pin;            ///Hempy bucket watering pump relay pin
    int Timeout = 120;   ///Max pump run time in seconds
    bool PumpEnabled = true; ///Enable/disable automatic watering based on weight    
  };
  struct WaterPumpSettings Pump1 = {.Pin = 7};  /// \TODO Find optimal port
  struct WaterPumpSettings Pump2 = {.Pin = 8};  /// \TODO Find optimal port

  struct WaterTempSensorSettings
  {
    WaterTempSensorSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin;
  };
  struct WaterTempSensorSettings WaterTemp1 = {.Pin = 45}; ///Data(yellow) - DS18B20 waterproof temp sensor

  struct WaterLevelSensorSettings
  {
    WaterLevelSensorSettings(uint8_t Pin_1 = 0, uint8_t Pin_2 = 0, uint8_t Pin_3 = 0, uint8_t Pin_4 = 0) : Pin_1(Pin_1), Pin_2(Pin_2), Pin_3(Pin_3), Pin_4(Pin_4) {}
    uint8_t Pin_1; ///Lowest water level
    uint8_t Pin_2;
    uint8_t Pin_3;
    uint8_t Pin_4; ///Full
  };
  struct WaterLevelSensorSettings WaterLevel1 = {.Pin_1 = A4, .Pin_2 = A5, .Pin_3 = A6, .Pin_4 = A7};

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

  struct ModuleSkeletonSettings
  { ///Test module
    ModuleSkeletonSettings(bool PersistentBool = 0, float PersistentFloat = 0.0) : PersistentBool(PersistentBool), PersistentFloat(PersistentFloat) {}
    bool PersistentBool;
    int PersistentInt = 420; ///Same value on all instances
    float PersistentFloat;
  };
  struct ModuleSkeletonSettings ModuleSkeleton1 = {.PersistentBool = false, .PersistentFloat = 1.23}; ///Instance 1
  struct ModuleSkeletonSettings ModuleSkeleton2 = {.PersistentBool = true, .PersistentFloat = 4.56};  ///Instance 2

  /*   
  bool AutomaticIFan = false;  ///Adjust internal fan based on temperature
  bool AutomaticEFan = false;  ///Adjust exhaust fan based on temp and humidity
  int IFanSwitchTemp = 25; /// Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached. Has to match default unit type(Metric C or Imperial K)
  uint8_t EFanHighHumid = 65; ///Above set humidity turn exhaust fan High if automatic fan control is enabled
  uint8_t EFanLowHumid = 55; ///Above set humidity turn exhaust fan Low if automatic fan control is enabled
  uint8_t EFanOffHumid = 40; ///Below set humidity turn exhaust fan Off if automatic fan control is enabled
  */

  uint8_t CompatibilityVersion = Version; ///Should always be the last value stored.
} Settings;
