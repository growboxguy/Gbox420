#pragma once

//Update the Version when you make change to the structure of the EEPROM stored Settings struct. This will overwrite the EEPROM settings with the sketch defaults.
static const byte Version = 7;

//THIS SECTION DOES NOT GET STORED IN EEPROM:
//Global constants
static const byte MaxTextLength = 32;      //Default char * buffer for storing a word + null terminator. Memory intense!
static const byte MaxShotTextLength = 128; //Default char * buffer for storing mutiple words. Memory intense!
static const int MaxLongTextLength = 1024; //Default char * buffer for storing a long text. Memory intense!

static const byte LogDepth = 5;                  //Show X number of log entries on website. Be careful, Max 1024bytes can be passed during a Website refresh event, incuding all parameters passed
static const byte QueueDepth = 32;               //Limits the maximum number of active modules. Memory intense!
static const byte RollingAverageDepth = 10;               //Limits the maximum number of active modules. Memory intense!
//Global variables
extern char LongMessage[MaxLongTextLength];  //temp storage for assembling long messages (REST API, MQTT API)
extern char ShortMessage[MaxShotTextLength]; //temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxTextLength];      //buffer for storing current time in text

//SAVED TO EEPROM - Settings struct
//If you change things here, increase the Version variable in line 4
typedef struct
{
  bool Debug = false;          //Logs debug messages to serial and web outputs
  bool MetricSystemEnabled = true;   //Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
  char PushingBoxLogRelayID[MaxTextLength] = {"v755877CF53383E1"};   //UPDATE THIS DeviceID of the PushingBox logging scenario 

  struct GrowBoxSettings{
    GrowBoxSettings(bool ReportToGoogleSheets, int SheetsReportingFrequency) : ReportToGoogleSheets(ReportToGoogleSheets) , SheetsReportingFrequency(SheetsReportingFrequency) {} 
    bool ReportToGoogleSheets;  //Enable/disable reporting sensor readings to Google Sheets
    int SheetsReportingFrequency; //How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
    //bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  };
  struct GrowBoxSettings Gbox1 = {.ReportToGoogleSheets = true, .SheetsReportingFrequency = 30};

  struct DHTSensorSettings
  { //initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
    DHTSensorSettings(byte Pin = 0, byte Type = 0) : Pin(Pin), Type(Type) {}
    byte Pin;
    byte Type; //Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings IDHT = {.Pin = 49, .Type = 22};
  struct DHTSensorSettings EDHT = {.Pin = 48, .Type = 22};

  struct SoundSettings
  {
    SoundSettings(byte Pin = 0) : Pin(Pin) {}
    byte Pin;            //PC buzzer+ (red)
    bool Enabled = true; //Enable PC speaker / Piezo buzzer
  };
  struct SoundSettings Sound1 = {.Pin = 2};

  struct LightSensorSettings
  {
    LightSensorSettings(byte DigitalPin = 0, byte AnalogPin = 0) : DigitalPin(DigitalPin), AnalogPin(AnalogPin) {}
    byte DigitalPin;
    byte AnalogPin;
  };
  struct LightSensorSettings LtSen1 = {.DigitalPin = 50, .AnalogPin = A0};

  struct LightsSettings
  {
    LightsSettings(byte RelayPin = 0, byte DimmingPin = 0, byte DimmingLimit = 0) : RelayPin(RelayPin), DimmingPin(DimmingPin), DimmingLimit(DimmingLimit) {} //Designated initializer
    byte RelayPin;                                                                                                                                            //Power relay Port 8 - LED lights
    byte DimmingPin;                                                                                                                                          //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    byte DimmingLimit;                                                                                                                                        //Sets the LED dimming limit (Usually around 5%)
    bool Status = true;                                                                                                                                       //Startup status for lights: True-ON / False-OFF
    byte Brightness = 15;                                                                                                                                     //Light intensity: 0 - 100 range for controlling led driver output
    bool TimerEnabled = true;                                                                                                                                 //Enable timer controlling lights
    byte OnHour = 4;                                                                                                                                          //Light ON time - hour
    byte OnMinute = 20;                                                                                                                                       //Light ON time - minute
    byte OffHour = 16;                                                                                                                                        //Light OFF time - hour
    byte OffMinute = 20;                                                                                                                                      //Light OFF time - minute
  };
  struct LightsSettings Lt1 = {.RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 8}; //Creating a LightSettings instance, passing in the unique parameters

  struct PHSensorSettings
  {
    PHSensorSettings(byte Pin = 0, float Slope = 0.0, float Intercept = 0.0) : Pin(Pin), Slope(Slope), Intercept(Intercept) {}
    byte Pin;
    float Slope;
    float Intercept;
  };
  struct PHSensorSettings PHSensor1 = {.Pin = A3, .Slope = -0.033256, .Intercept = 24.08651}; //Update this to your own PH meter calibration values

  struct PressureSensorSettings
  {
    PressureSensorSettings(byte Pin = 0, float Offset = 0.0, float Ratio = 0.0) : Pin(Pin), Offset(Offset), Ratio(Ratio) {}
    byte Pin;     //Pressure sensor Pin: Signal(yellow)
    float Offset; //Pressure sensor calibration: voltage reading at 0 pressure
    float Ratio;  //Pressure sensor voltage to pressure ratio
  };
  struct PressureSensorSettings Pressure1 = {.Pin = A1, .Offset = 0.57, .Ratio = 2.7};

  struct WeightSensorSettings
  {
    WeightSensorSettings(byte DTPin = 0, byte SCKPin = 0, float Scale = 0.0, long TareOffset = 0.0) : DTPin(DTPin), SCKPin(SCKPin), Scale(Scale), TareOffset(TareOffset) {}
    byte DTPin;     //Weight sensor DT pin
    byte SCKPin; //Weight sensor SCK pin
    float Scale;  //Calibration scale value
    long TareOffset; //Reading at 0 weight on the scale
  };
  struct WeightSensorSettings Weight1 = {.DTPin = 3, .SCKPin = 4, .Scale = 125000.0, .TareOffset=146000};
  struct WeightSensorSettings Weight2 = {.DTPin = 5, .SCKPin = 6, .Scale = 126000.0, .TareOffset=267461};

  struct AeroponicsSettings
  { //Common settings for both inheriting classes: Aeroponics_Tank and Aeroponics_NoTank
    AeroponicsSettings(byte BypassSolenoidPin = 0, byte PumpPin = 0) : BypassSolenoidPin(BypassSolenoidPin), PumpPin(PumpPin) {}
    byte BypassSolenoidPin;   //Aeroponics bypass solenoid relay pin
    byte PumpPin;             //Aeroponics high pressure pump relay pin
    bool SprayEnabled = true; //Enable/disable spraying cycle
    int Interval = 15;        //Spray every X minutes
    int Duration = 10;        //Spray time in seconds
    int PumpTimeout = 6;      //Max pump run time in minutes
    int PrimingTime = 10;     //At pump startup the bypass valve will be open for X seconds to let the pump remove air from the tubes
  };
  struct AeroponicsSettings AeroT1_Common = {.BypassSolenoidPin = 23, .PumpPin = 24};
  struct AeroponicsSettings Aero1_Common = {.BypassSolenoidPin = 52, .PumpPin = 53};

  struct AeroponicsSettings_NoTankSpecific
  {                           //Settings for an Aeroponics setup WITHOUT a pressure tank
    int BlowOffTime = 3;      //After spraying open the bypass valve for X seconds to release pressure. Helps to stop spraying immediately
    float PressureHigh = 7.0; //Safety feature - Turn off pump above this pressure
  } Aero1_Specific;

  struct AeroponicsSettings_TankSpecific
  { //Settings for an Aeroponics setup WITH a pressure tank
    AeroponicsSettings_TankSpecific(byte SpraySolenoidPin = 0) : SpraySolenoidPin(SpraySolenoidPin) {}
    byte SpraySolenoidPin;    //Spray solenoid relay pin
    float PressureLow = 5.0;  //Turn on pump below this pressure
    float PressureHigh = 7.0; //Turn off pump above this pressure
  };
  struct AeroponicsSettings_TankSpecific AeroT1_Specific = {.SpraySolenoidPin = 22};

  struct WaterTempSensorSettings
  {
    WaterTempSensorSettings(byte Pin = 0) : Pin(Pin) {}
    byte Pin;
  };
  struct WaterTempSensorSettings WaterTemp1 = {.Pin = 51}; //Data(yellow) - DS18B20 waterproof temp sensor

  struct WaterLevelSensorSettings
  {
    WaterLevelSensorSettings(byte Pin_1 = 0, byte Pin_2 = 0, byte Pin_3 = 0, byte Pin_4 = 0) : Pin_1(Pin_1), Pin_2(Pin_2), Pin_3(Pin_3), Pin_4(Pin_4) {}
    byte Pin_1; //Lowest water level
    byte Pin_2;
    byte Pin_3;
    byte Pin_4; //Full
  };
  struct WaterLevelSensorSettings WaterLevel1 = {.Pin_1 = A4, .Pin_2 = A5, .Pin_3 = A6, .Pin_4 = A7};

  struct FanSettings
  {
    FanSettings(byte OnOffPin = 0, byte SpeedPin = 0) : OnOffPin(OnOffPin), SpeedPin(SpeedPin) {}
    byte OnOffPin;          //Relay pin for power
    byte SpeedPin;          //Relay pin for speed selection
    bool State = true;      //true - ON, false - OFF
    bool HighSpeed = false; //true - High speed, false - Low speed
  };
  struct FanSettings IFan = {.OnOffPin = 25, .SpeedPin = 26};
  struct FanSettings EFan = {.OnOffPin = 27, .SpeedPin = 28};

  struct ModuleSkeletonSettings
  { //Test module
    ModuleSkeletonSettings(bool PersistentBool = 0, float PersistentFloat = 0.0) : PersistentBool(PersistentBool), PersistentFloat(PersistentFloat) {}
    bool PersistentBool;
    int PersistentInt = 420; //Same value on all instances
    float PersistentFloat;
  };
  struct ModuleSkeletonSettings ModuleSkeleton1 = {.PersistentBool = false, .PersistentFloat = 1.23}; //Instance 1
  struct ModuleSkeletonSettings ModuleSkeleton2 = {.PersistentBool = true, .PersistentFloat = 4.56};  //Instance 2

  /*   
  bool AutomaticIFan = false;  //Adjust internal fan based on temperature
  bool AutomaticEFan = false;  //Adjust exhaust fan based on temp and humidity
  int IFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached. Has to match default unit type(Metric C or Imperial K)
  byte EFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  byte EFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  byte EFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
  */

  byte CompatibilityVersion = Version; //Should always be the last value stored.
} Settings;
