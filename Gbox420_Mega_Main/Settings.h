#pragma once

/*! 
 *  \brief     Default Settings for each component. Loaded when the Arduino starts, updated by user interaction on the website.
 *  \details   The Settings object is stored in EEPROM and kept between reboots. 
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 2; ///Increment this when you make a change in the SAVED TO EEPROM secton

///State machine - Defining possible states
enum PumpStates
{
  DISABLED,
  IDLE,
  PRIMING,
  RUNNING,
  BLOWOFF,
  MIXING
};

///THIS SECTION DOES NOT GET STORED IN EEPROM:
///Global constants
static const uint8_t MaxWordLength = 32;        ///Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 128;   ///Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1024; ///Default char * buffer length for storing a long text. Memory intense!
static const uint8_t LogDepth = 4;              ///Show X number of log entries on website. Be careful, Max 1024 bits can be passed during a Website Refresh/Load event
static const uint8_t QueueDepth = 32;           ///Limits the maximum number of components within a module. Memory intense!
static const uint8_t RollingAverageDepth = 10;  ///Smooth out sensor readings by calculating the average of the last X results. Memory intense!

///Global variables
extern char LongMessage[MaxLongTextLength];  ///Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; ///Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      ///Buffer for storing current time in text format

///nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 49;           ///nRF24l01+ wireless transmitter CSN pin
static const uint8_t WirelessCEPin = 53;            ///nRF24l01+ wireless transmitter CE pin
static const uint8_t WirelessDelay = 8;             ///How long to wait between each retry (250ms increments), Max 15. 0 means 250us, 15 means 4000us,
static const uint8_t WirelessRetry = 10;            ///How many retries before giving up, max 15
static const uint8_t WirelessPayloadSize = 32;      ///Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500; ///(ms) One package should be exchanged within this timeout (Including retries and delays)

///SAVED TO EEPROM - Settings struct
///If you change things here, increase the Version variable in line 12
typedef struct
{
  bool Debug = true;                                               ///Logs debug messages to serial and web outputs
  bool Metric = true;                                              ///Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.
  char PushingBoxLogRelayID[MaxWordLength] = {"v755877CF53383E1"}; ///UPDATE THIS DeviceID of the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging

  struct AeroModuleSettings ///initialized via Designated initializer https:///riptutorial.com/c/example/18609/using-designated-initializers
  {
    AeroModuleSettings(bool PressureTankPresent = false, uint16_t DayInterval = 0, uint16_t DayDuration = 0, uint16_t NightInterval = 0, uint16_t NightDuration = 0, uint16_t PumpTimeOut = 0, uint16_t PrimingTime = 0, float MaxPressure = 0.0, float MinPressure = 0.0) : PressureTankPresent(PressureTankPresent), DayInterval(DayInterval), DayDuration(DayDuration), NightInterval(NightInterval), NightDuration(NightDuration), PumpTimeOut(PumpTimeOut), PrimingTime(PrimingTime), MaxPressure(MaxPressure), MinPressure(MinPressure) {}
    bool PressureTankPresent; ///Is there a pressure tank connected or not
    uint16_t DayInterval;     ///Spray every X minutes - When the lights are ON
    uint16_t DayDuration;     ///Spray length in seconds - When the lights are ON
    uint16_t NightInterval;   ///Spray every X minutes - When the lights are OFF
    uint16_t NightDuration;   ///Spray length in seconds - When the lights are OFF
    uint8_t PumpSpeed;        ///Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut;     ///(Sec) Max pump run time
    uint16_t PrimingTime;     ///(Sec) For how long to keep the bypass solenoid on when starting the pump - Removes air bubbles from the lines
    float MaxPressure;        ///Turn off pump above this pressure
    float MinPressure;        ///Turn on pump below this pressure
  };
  struct AeroModuleSettings AeroModule1 = {.PressureTankPresent = false, .DayInterval = 15, .DayDuration = 10, .NightInterval = 30, .NightDuration = 8, .PumpTimeOut = 420, .PrimingTime = 10, .MaxPressure = 7.0, .MinPressure = 5.0};

  struct AirPumpSettings
  {
    AirPumpSettings(uint8_t Pin = 0, bool State = false) : Pin(Pin), State(State) {}
    uint8_t Pin; ///Relay pin controlling AC power to the air pump
    bool State;  ///true - ON, false - OFF
  };
  struct AirPumpSettings APump1 = {.Pin = 23, .State = true};

  struct DHTSensorSettings
  {
    DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
    uint8_t Pin;  ///DAT pin of the DHT sensor
    uint8_t Type; ///Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct FanSettings
  {
    FanSettings(uint8_t OnOffPin = 0, uint8_t SpeedPin = 0) : OnOffPin(OnOffPin), SpeedPin(SpeedPin) {}
    uint8_t OnOffPin;       ///Relay pin controlling AC power
    uint8_t SpeedPin;       ///Relay pin for speed selection
    bool State = true;      ///true - ON, false - OFF
    bool HighSpeed = false; ///true - High speed, false - Low speed
  };
  struct FanSettings IFan = {.OnOffPin = 25, .SpeedPin = 26};
  struct FanSettings EFan = {.OnOffPin = 27, .SpeedPin = 28};

  struct MainModuleSettings
  {
    MainModuleSettings(bool ReportToGoogleSheets, uint16_t SheetsReportingFrequency) : ReportToGoogleSheets(ReportToGoogleSheets), SheetsReportingFrequency(SheetsReportingFrequency) {}
    bool ReportToGoogleSheets;         ///Enable/disable reporting sensor readings to Google Sheets
    uint16_t SheetsReportingFrequency; ///How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    ///bool ReportToMqtt = true;    ///Controls reporting sensor readings to an MQTT broker
  };
  struct MainModuleSettings Main1 = {.ReportToGoogleSheets = true, .SheetsReportingFrequency = 30};

  struct HempyModuleSettings
  {
    HempyModuleSettings(bool WeightBasedWatering_B1 = false, float StartWeight_B1 = 0.0, float StopWeight_B1 = 0.0, bool TimerBasedWatering_B1 = false, uint16_t WateringInterval_B1 = 0, uint16_t WateringDuration_B1 = 0, uint8_t PumpSpeed_B1 = 0, uint16_t TimeOutPump_B1 = 0, bool WeightBasedWatering_B2 = false, float StartWeight_B2 = 0.0, float StopWeight_B2 = 0.0, bool TimerBasedWatering_B2 = false, uint16_t WateringInterval_B2 = 0, uint16_t WateringDuration_B2 = 0, uint8_t PumpSpeed_B2 = 0, uint16_t TimeOutPump_B2 = 0) : WeightBasedWatering_B1(WeightBasedWatering_B1), StartWeight_B1(StartWeight_B1), StopWeight_B1(StopWeight_B1), TimerBasedWatering_B1(TimerBasedWatering_B1), WateringInterval_B1(WateringInterval_B1), WateringDuration_B1(WateringDuration_B1), PumpSpeed_B1(PumpSpeed_B1), TimeOutPump_B1(TimeOutPump_B1), WeightBasedWatering_B2(WeightBasedWatering_B2), StartWeight_B2(StartWeight_B2), StopWeight_B2(StopWeight_B2), TimerBasedWatering_B2(TimerBasedWatering_B2), WateringInterval_B2(WateringInterval_B2), WateringDuration_B2(WateringDuration_B2), PumpSpeed_B2(PumpSpeed_B2), TimeOutPump_B2(TimeOutPump_B2) {}
    bool WeightBasedWatering_B1;  ///Enable/Disable weight based watering
    float StartWeight_B1;         ///Start watering below this weight
    float StopWeight_B1;          ///Stop watering above this weight
    bool TimerBasedWatering_B1;   ///Enable/Disable timer based watering
    uint16_t WateringInterval_B1; ///Water every X minutes
    uint16_t WateringDuration_B1; ///Water for X seconds
    uint8_t PumpSpeed_B1;         ///Bucket 1 - Pump duty cycle to adjust motor speed
    uint16_t TimeOutPump_B1;      ///Max pump runtime in seconds, target StopWeight should be reached before hitting this. Pump gets disabled if timeout is reached ///\todo Add email alert when pump fails
    bool WeightBasedWatering_B2;  //Enable/Disable weight based watering
    float StartWeight_B2;         ///Start watering below this weight
    float StopWeight_B2;          ///Stop watering above this weight
    bool TimerBasedWatering_B2;   //Enable/Disable timer based watering
    uint8_t PumpSpeed_B2;         ///Bucket 2 - Pump duty cycle to adjust motor speed
    uint16_t WateringInterval_B2; //Water every X minutes
    uint16_t WateringDuration_B2; //Water for X seconds
    uint16_t TimeOutPump_B2;      ///Max pump runtime in seconds, target StopWeight should be reached before hitting this. Pump gets disabled if timeout is reached ///\todo Add email alert when pump fails
  };
  struct HempyModuleSettings HempyModule1 = {.WeightBasedWatering_B1 = true, .StartWeight_B1 = 13.0, .StopWeight_B1 = 18.0, .TimerBasedWatering_B1 = true, .WateringInterval_B1 = 1440, .WateringDuration_B1 = 30, .PumpSpeed_B1 = 100, .TimeOutPump_B1 = 420, .WeightBasedWatering_B2 = true, .StartWeight_B2 = 13.0, .StopWeight_B2 = 18.0, .TimerBasedWatering_B2 = true, .WateringInterval_B2 = 1440, .WateringDuration_B2 = 30, .PumpSpeed_B2 = 100, .TimeOutPump_B2 = 420};

  struct LightSensorSettings
  {
    LightSensorSettings(uint8_t DigitalPin = 0, uint8_t AnalogPin = 0) : DigitalPin(DigitalPin), AnalogPin(AnalogPin) {}
    uint8_t DigitalPin; ///Light sensor D0 pin
    uint8_t AnalogPin;  ///Light sensor A0 pin
  };
  struct LightSensorSettings LtSen1 = {.DigitalPin = 4, .AnalogPin = A0};

  struct LightsSettings
  {
    LightsSettings(uint8_t RelayPin = 0, uint8_t DimmingPin = 0, uint8_t DimmingLimit = 0, uint8_t DimmingDuration = 0, uint8_t Brightness = 0, bool TimerEnabled = false, uint8_t OnHour = 0, uint8_t OnMinute = 0, uint8_t OffHour = 0, uint8_t OffMinute = 0, bool FadingEnabled = false, uint16_t FadingInterval = 0, uint8_t FadingIncrements = 0) : RelayPin(RelayPin), DimmingPin(DimmingPin), DimmingLimit(DimmingLimit), DimmingDuration(DimmingDuration), Brightness(Brightness), TimerEnabled(TimerEnabled), OnHour(OnHour), OnMinute(OnMinute), OffHour(OffHour), OffMinute(OffMinute), FadingEnabled(FadingEnabled), FadingInterval(FadingInterval), FadingIncrements(FadingIncrements) {}
    uint8_t RelayPin;         ///Relay port controlling AC power to LED driver
    uint8_t DimmingPin;       ///PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    uint8_t DimmingLimit;     ///Sets the LED dimming limit (Usually around 5%)
    uint8_t DimmingDuration;  ///Temporary dimming duration in Seconds
    bool Status = false;      ///Startup status for lights: True-ON / False-OFF
    uint8_t Brightness;       ///Light intensity: 0 - 100 range for controlling led driver output
    bool TimerEnabled;        ///Enable/Disable timer controlling lights
    uint8_t OnHour;           ///Light ON time - hour
    uint8_t OnMinute;         ///Light ON time - minute
    uint8_t OffHour;          ///Light OFF time - hour
    uint8_t OffMinute;        ///Light OFF time - minute
    bool FadingEnabled;       ///Enables/disables fading lights in when turning on, and fading lights out when turning off
    uint16_t FadingInterval;  ///(Sec) How often should the brightness change during a fade in/out
    uint8_t FadingIncrements; ///How much to change the brightness during a fade in/out in Percentage
  };
  struct LightsSettings Lt1 = {.RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 16, .DimmingDuration = 10, .Brightness = 75, .TimerEnabled = true, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingEnabled = true, .FadingInterval = 1, .FadingIncrements = 1}; ///Creating a LightSettings instance, passing in the unique parameters
  struct LightsSettings Lt2 = {.RelayPin = 24, .DimmingPin = 12, .DimmingLimit = 6, .DimmingDuration = 10, .Brightness = 55, .TimerEnabled = true, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingEnabled = true, .FadingInterval = 1, .FadingIncrements = 1};  ///Creating a LightSettings instance, passing in the unique parameters

  struct ReservoirModuleSettings
  {
    //ReservoirModuleSettings() :  {}
  };
  struct ReservoirModuleSettings ReservoirMod1 = {};

  struct SoundSettings
  {
    SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
    uint8_t Pin;         ///Piezo Buzzer red(+) cable
    bool Enabled = true; ///Enable/Disable sound
  };
  struct SoundSettings Sound1 = {.Pin = 2};

  uint8_t CompatibilityVersion = Version; ///Should always be the last value stored.
} Settings;

///////////////////////////////////////////////////////////////
///EEPROM related functions - Persistent storage between reboots
///Use cautiously, EEPROM has a write limit of 100.000 cycles - Only use these in the setup() function, or when a user initiated change is stored

void saveSettings(Settings *ToSave);
Settings *loadSettings(bool ResetEEPROM = false);
void restoreDefaults(Settings *ToOverwrite);