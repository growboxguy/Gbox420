#pragma once

/*! 
 *  \brief     Default Settings for each component within the module. Loaded when the Arduino starts.
 *  \details   Settings are stored in EEPROM and kept between reboots. Stored values are updated by the website controls on user interaction.  
 *  \warning   EEPROM has a write limit of 100.000 cycles, constantly updating the variables inside a loop would wear out the EEPROM memory! 
 *  \attention Update the Version number when you change the structure of the settings. This will overwrite the EEPROM stored settings with the sketch defaults from this file.
 *  \author    GrowBoxGuy
 *  \version   4.20
 */

static const uint8_t Version = 10; ///< Increment this after changing the stucture of the SAVED TO EEPROM secton to force overwriting the stored settings in the Arduino's EEPROM.

///< NOT SAVED TO EEPROM

///< Global constants
static const uint8_t MaxWordLength = 32;        ///< Default char * buffer length for storing a word + null terminator. Memory intense!
static const uint8_t MaxShotTextLength = 128;   ///< Default char * buffer length for storing mutiple words. Memory intense!
static const uint16_t MaxLongTextLength = 1024; ///< Default char * buffer length for storing a long text. Memory intense!
static const uint8_t LogDepth = 4;              ///< Show X number of log entries on website. Be careful, Max 1024 bits can be passed during a Website Refresh/Load event
static const uint8_t QueueDepth = 32;           ///< Limits the maximum number of components within a module. Memory intense!
static const uint8_t MovingAverageDepth = 10;   ///< Smooth out sensor readings by calculating the average of the last X results. Memory intense!

///< Global variables
extern char LongMessage[MaxLongTextLength];  // Temp storage for assembling long messages (REST API - Google Sheets reporting)
extern char ShortMessage[MaxShotTextLength]; // Temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxWordLength];      // Buffer for storing current time in text format

///< nRF24L01+ wireless receiver
static const uint8_t WirelessCSNPin = 49;             ///< nRF24l01+ wireless transmitter CSN pin
static const uint8_t WirelessCEPin = 53;              ///< nRF24l01+ wireless transmitter CE pin
static const uint8_t WirelessDelay = 8;               ///< How long to wait between each retry (250ms increments), Max 15. 0 means 250us, 15 means 4000us,
static const uint8_t WirelessRetry = 10;              ///< How many retries before giving up, max 15
static const uint8_t WirelessPayloadSize = 32;        ///< Size of the wireless packages exchanged with the Main module. Max 32 bytes are supported on nRF24L01+
static const uint16_t WirelessMessageTimeout = 500;   ///< (ms) One package should be exchanged within this timeout (Including retries and delays)
static const uint16_t WirelessReceiveTimeout = 65000; ///< (ms) Consider a module offline after this timeout. Should be a few seconds longer then the WirelessReceiveTimeout configured on the Modules

///< SAVED TO EEPROM - Settings struct
///< If you change things here, increase the Version variable in line 12

typedef struct
{
  bool Debug = true;  ///< Logs debug messages to serial and web outputs
  bool Metric = true; ///< Switch between Imperial/Metric units. If changed update the default temp and pressure values below too.

  char PushingBoxLogRelayID[MaxWordLength] = {"v755877CF53383E1"}; ///< UPDATE THIS DeviceID of the PushingBox logging scenario: https://sites.google.com/site/growboxguy/arduino/logging
  char MqttPubTopic[MaxShotTextLength] = {"Gbox420/"};             ///< Publish MQTT messages to this topic. Ends with a forward slash
  char MqttSubTopic[MaxShotTextLength] = {"Gbox420CMD/#"};         ///< Subscribe to messages of this topic and all sub-topic
  char MqttLwtTopic[MaxShotTextLength] = {"Gbox420LWT/"};          ///< When the connection is lost the MQTT broker will publish a final message to this topic. Ends with a forward slash
  char MqttLwtMessage[MaxWordLength] = {"Gbox420 Offline"};        ///< this is the message subscribers will get under the topic specified by MqttLwtTopic variable when the MQTT client unexpectedly goes offline

  // initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
  struct AeroModuleSettings ///< AeroModule default settings
  {
    AeroModuleSettings(bool PressureTankPresent = false, float Duration = 0.0, uint16_t DayInterval = 0, uint16_t NightInterval = 0, uint8_t PumpSpeed = 0, uint16_t PumpTimeOut = 0, uint16_t PrimingTime = 0, float MaxPressure = 0.0, float MinPressure = 0.0) : PressureTankPresent(PressureTankPresent), Duration(Duration), DayInterval(DayInterval), NightInterval(NightInterval), PumpSpeed(PumpSpeed), PumpTimeOut(PumpTimeOut), PrimingTime(PrimingTime), MaxPressure(MaxPressure), MinPressure(MinPressure) {}
    bool PressureTankPresent; ///< Is there a pressure tank connected or not
    float Duration;           ///< Spray length in seconds (Actual duration is ~0.5sec longer due to thread + solenoid delay)
    uint16_t DayInterval;     ///< Spray every X minutes - When the lights are ON
    uint16_t NightInterval;   ///< Spray every X minutes - When the lights are OFF
    uint8_t PumpSpeed;        ///< Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut;     ///< (Sec) Max pump run time
    uint16_t PrimingTime;     ///< (Sec) For how long to keep the bypass solenoid on when starting the pump - Removes air bubbles from the lines
    float MaxPressure;        ///< Turn off pump above this pressure
    float MinPressure;        ///< Turn on pump below this pressure
  };
  struct AeroModuleSettings AeroModule1 = {.PressureTankPresent = false, .Duration = 3.0, .DayInterval = 6, .NightInterval = 10, .PumpSpeed = 100, .PumpTimeOut = 420, .PrimingTime = 10, .MaxPressure = 7.0, .MinPressure = 5.0};

struct ACMotorModuleSettings ///< AeroModule default settings
  {
    ACMotorModuleSettings(uint8_t Speed = 0) : Speed(Speed) {}
    uint8_t Speed;        ///< PWM duty cycle to adjust motor speed
  };
  struct ACMotorModuleSettings ACMotor1 = {.Speed = 50};

  struct AirPumpSettings ///< AirPump default settings
  {
    AirPumpSettings(uint8_t Pin = 0, bool State = false) : Pin(Pin), State(State) {}
    uint8_t Pin; ///< Relay pin controlling AC power to the air pump
    bool State;  ///< true - ON, false - OFF
  };
  struct AirPumpSettings Ap1 = {.Pin = 23, .State = true};

  struct DHTSensorSettings ///< DHTSensor default settings
  {
    DHTSensorSettings(uint8_t Pin = 0, uint8_t Type = 0) : Pin(Pin), Type(Type) {}
    uint8_t Pin;  ///< DAT pin of the DHT sensor
    uint8_t Type; ///< Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 3, .Type = 22};

  struct FanSettings ///< Fan default settings
  {
    FanSettings(uint8_t OnOffPin = 0, uint8_t SpeedPin = 0) : OnOffPin(OnOffPin), SpeedPin(SpeedPin) {}
    uint8_t OnOffPin;       ///< Relay pin controlling AC power
    uint8_t SpeedPin;       ///< Relay pin for speed selection
    bool State = true;      ///< true - ON, false - OFF
    bool HighSpeed = false; ///< true - High speed, false - Low speed
  };
  struct FanSettings FanI = {.OnOffPin = 25, .SpeedPin = 26};
  struct FanSettings FanE = {.OnOffPin = 27, .SpeedPin = 28};

  /*
  // PWM adjusted AC signal - Need to move this to a dedicated module, Mega already uses interrupts to talk to ESP-link and it messes with counting the phase zero crossings
  struct Fan_PWMSettings ///< Fan default settings
  {
    Fan_PWMSettings(uint8_t ZeroCrossingPin = 0, uint8_t PWMPin = 0, bool State = false, uint8_t MinSpeed = 0, uint8_t Speed = 0) : ZeroCrossingPin(ZeroCrossingPin), PWMPin(PWMPin), State(State), MinSpeed(MinSpeed), Speed(Speed) {}
    uint8_t ZeroCrossingPin; ///< On Arduino Mega2560 and Nano this has to be D2 pin
    uint8_t PWMPin;          ///< PWM capable digital pin
    bool State = true;       //ON or OFF
    uint8_t MinSpeed;        //Limit the lowest output (%)
    uint8_t Speed;           //Speed between 0-100 (%)  (Real output mapped between MinSpeed - 100)
  };
  struct Fan_PWMSettings FanI = {.ZeroCrossingPin = 2, .PWMPin = 9, .State = true, .MinSpeed = 35, .Speed = 80};
  struct Fan_PWMSettings FanE = {.ZeroCrossingPin = 2, .PWMPin = 10, .State = true, .MinSpeed = 35, .Speed = 80};
  */

  struct MainModuleSettings ///< MainModule default settings
  {
    MainModuleSettings(uint16_t SerialReportFrequency = 0, bool SerialReportDate = true, bool SerialReportMemory = true, bool SerialReportJSON = true, bool SerialReportJSONFriendly = true, bool SerialReportWireless = true, bool ReportToGoogleSheets = false, uint16_t SheetsReportingFrequency = 0, bool ReportToMQTT = false, uint16_t MQTTReportFrequency = 0) : SerialReportFrequency(SerialReportFrequency), SerialReportDate(SerialReportDate), SerialReportMemory(SerialReportMemory), SerialReportJSON(SerialReportJSON), SerialReportJSONFriendly(SerialReportJSONFriendly), SerialReportWireless(SerialReportWireless), ReportToGoogleSheets(ReportToGoogleSheets), SheetsReportingFrequency(SheetsReportingFrequency), ReportToMQTT(ReportToMQTT), MQTTReportFrequency(MQTTReportFrequency) {}
    uint16_t SerialReportFrequency;    ///< How often to report to Serial console. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
    bool SerialReportDate;             ///< Enable/disable reporting the current time to the Serial output
    bool SerialReportMemory;           ///< Enable/disable reporting the remaining free memory to the Serial output
    bool SerialReportJSON;             ///< Enable/disable sending JSON formatted reports to the Serial output
    bool SerialReportJSONFriendly;     ///< Enable/disable sending JSON report with friendly values (Sec,%,Min,kg/lbs..etc appended) to Serial
    bool SerialReportWireless;         ///< Enable/disable sending wireless package exchange reports to the Serial output
    bool ReportToGoogleSheets;         ///< Enable/disable reporting sensor readings to Google Sheets
    uint16_t SheetsReportingFrequency; ///< How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    bool ReportToMQTT;                 ///< Enable/disable reporting sensor readings to an MQTT broker
    uint16_t MQTTReportFrequency;      ///< How often to report to MQTT. Use 5 Sec increments, Min 5sec, Max 86400 (1day)
  };
  struct MainModuleSettings Main1 = {.SerialReportFrequency = 15, .SerialReportDate = true, .SerialReportMemory = true, .SerialReportJSON = true, .SerialReportJSONFriendly = true, .SerialReportWireless = true, .ReportToGoogleSheets = true, .SheetsReportingFrequency = 30, .ReportToMqtt = true, .MQTTReportFrequency = 5};

  struct HempyModuleSettings ///< Hempy default settings
  {
    HempyModuleSettings(float EvaporationTarget_B1 = 0.0, float OverflowTarget_B1 = 0.0, float WasteLimit_B1 = 0.0, uint8_t PumpSpeed_B1 = 0, uint16_t PumpTimeOut_B1 = 0, uint16_t DrainWaitTime_B1 = 0.0, float EvaporationTarget_B2 = 0.0, float OverflowTarget_B2 = 0.0, float WasteLimit_B2 = 0.0, uint8_t PumpSpeed_B2 = 0, uint16_t PumpTimeOut_B2 = 0, uint16_t DrainWaitTime_B2 = 0.0) : EvaporationTarget_B1(EvaporationTarget_B1), OverflowTarget_B1(OverflowTarget_B1), WasteLimit_B1(WasteLimit_B1), PumpSpeed_B1(PumpSpeed_B1), PumpTimeOut_B1(PumpTimeOut_B1), DrainWaitTime_B1(DrainWaitTime_B1), EvaporationTarget_B2(EvaporationTarget_B2), OverflowTarget_B2(OverflowTarget_B2), WasteLimit_B2(WasteLimit_B2), PumpSpeed_B2(PumpSpeed_B2), PumpTimeOut_B2(PumpTimeOut_B2), DrainWaitTime_B2(DrainWaitTime_B2) {}
    float EvaporationTarget_B1; ///< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float OverflowTarget_B1;    ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    float WasteLimit_B1;        ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint8_t PumpSpeed_B1;       ///< Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut_B1;    ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint16_t DrainWaitTime_B1;  ///< (sec) How long to wait after watering for the water to drain
    float EvaporationTarget_B2; ///< (kg/lbs) Amount of water that should evaporate before starting the watering cycles
    float OverflowTarget_B2;    ///< (kg/lbs) Amount of water that should go to the waste reservoir after a watering cycle
    float WasteLimit_B2;        ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint8_t PumpSpeed_B2;       ///< Pump duty cycle to adjust motor speed
    uint16_t PumpTimeOut_B2;    ///< Waste reservoir full weight -> Pump gets disabled if reached
    uint16_t DrainWaitTime_B2;  ///< (sec) How long to wait after watering for the water to drain
  };
  struct HempyModuleSettings HempyModule1 = {.EvaporationTarget_B1 = 2.0, .OverflowTarget_B1 = 0.2, .WasteLimit_B1 = 13.0, .PumpSpeed_B1 = 100, .PumpTimeOut_B1 = 120, .DrainWaitTime_B1 = 300, .EvaporationTarget_B2 = 2.0, .OverflowTarget_B2 = 0.2, .WasteLimit_B2 = 13.0, .PumpSpeed_B2 = 100, .PumpTimeOut_B2 = 120, .DrainWaitTime_B2 = 300};

  struct LightSensorSettings ///< LightSensor default settings
  {
    LightSensorSettings(uint8_t DigitalPin = 0, uint8_t AnalogPin = 0) : DigitalPin(DigitalPin), AnalogPin(AnalogPin) {}
    uint8_t DigitalPin; ///< Light sensor D0 pin
    uint8_t AnalogPin;  ///< Light sensor A0 pin
  };
  struct LightSensorSettings Ls1 = {.DigitalPin = 4, .AnalogPin = A0};

  struct LightsSettings ///< Lights default settings
  {
    LightsSettings(uint8_t RelayPin = 0, uint8_t DimmingPin = 0, uint8_t DimmingLimit = 0, uint8_t DimmingDuration = 0, uint8_t Brightness = 0, bool TimerEnabled = false, uint8_t OnHour = 0, uint8_t OnMinute = 0, uint8_t OffHour = 0, uint8_t OffMinute = 0, bool FadingEnabled = false, uint16_t FadingInterval = 0, uint8_t FadingIncrements = 0) : RelayPin(RelayPin), DimmingPin(DimmingPin), DimmingLimit(DimmingLimit), DimmingDuration(DimmingDuration), Brightness(Brightness), TimerEnabled(TimerEnabled), OnHour(OnHour), OnMinute(OnMinute), OffHour(OffHour), OffMinute(OffMinute), FadingEnabled(FadingEnabled), FadingInterval(FadingInterval), FadingIncrements(FadingIncrements) {}
    uint8_t RelayPin;         ///< Relay port controlling AC power to LED driver
    uint8_t DimmingPin;       ///< PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    uint8_t DimmingLimit;     ///< Sets the LED dimming limit (Usually around 5%)
    uint8_t DimmingDuration;  ///< Temporary dimming duration in Seconds
    bool Status = false;      ///< Startup status for lights: True-ON / False-OFF
    uint8_t Brightness;       ///< Light intensity: 0 - 100 range for controlling led driver output
    bool TimerEnabled;        ///< Enable/Disable timer controlling lights
    uint8_t OnHour;           ///< Light ON time - hour
    uint8_t OnMinute;         ///< Light ON time - minute
    uint8_t OffHour;          ///< Light OFF time - hour
    uint8_t OffMinute;        ///< Light OFF time - minute
    bool FadingEnabled;       ///< Enables/disables fading lights in when turning on, and fading lights out when turning off <Not exposed to Web interface>
    uint16_t FadingInterval;  ///< (Sec) How often should the brightness change during a fade in/out <Not exposed to Web interface>
    uint8_t FadingIncrements; ///< How much to change the brightness during a fade in/out in Percentage <Not exposed to Web interface>
  };
  struct LightsSettings Lt1 = {.RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 16, .DimmingDuration = 10, .Brightness = 75, .TimerEnabled = true, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingEnabled = false, .FadingInterval = 1, .FadingIncrements = 1}; ///< Creating a LightSettings instance, passing in the unique parameters
  struct LightsSettings Lt2 = {.RelayPin = 24, .DimmingPin = 12, .DimmingLimit = 6, .DimmingDuration = 10, .Brightness = 55, .TimerEnabled = false, .OnHour = 4, .OnMinute = 20, .OffHour = 16, .OffMinute = 20, .FadingEnabled = false, .FadingInterval = 1, .FadingIncrements = 1}; ///< Creating a LightSettings instance, passing in the unique parameters

  struct ReservoirModuleSettings ///< ReservoirModule default settings
  {
    //ReservoirModuleSettings() :  {}
  };
  struct ReservoirModuleSettings ReservoirMod1 = {};

  struct SoundSettings ///< Sound default settings
  {
    SoundSettings(uint8_t Pin = 0) : Pin(Pin) {}
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