#pragma once

//// @attention Define the preferred default settings here.

///Update the Version when you make change to the structure of the EEPROM stored Settings struct. This will overwrite the EEPROM settings with the sketch defaults.
static const byte Version = 12;

///THIS SECTION DOES NOT GET STORED IN EEPROM:
///Global constants
static const byte MaxTextLength = 32;      ///Default char * buffer for storing a word + null terminator. Memory intense!
static const byte MaxShotTextLength = 64; ///Default char * buffer for storing mutiple words. Memory intense!
static const int MaxLongTextLength = 256; ///Default char * buffer for storing a long text. Memory intense!

static const byte QueueDepth = 8;               ///Limits the maximum number of active modules. Memory intense!
static const byte RollingAverageDepth = 10;               ///Limits the maximum number of active modules. Memory intense!
///Global variables
extern char LongMessage[MaxLongTextLength];  ///temp storage for assembling long messages (REST API, MQTT API)
extern char ShortMessage[MaxShotTextLength]; ///temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[MaxTextLength];      ///buffer for storing current time in text

///SAVED TO EEPROM - Settings struct
///If you change things here, increase the Version variable in line 4
typedef struct
{
  bool Debug = true;          ///Logs debug messages to serial and web outputs
  bool Metric = true;   ///Switch between Imperial/Metric units. If changed update the default temp and pressure values too.
  
  struct HempyModuleSettings{  ///TODO: Remove the parameters
    HempyModuleSettings(bool Debug, bool Metric) : Debug(Debug) , Metric(Metric) {} 
    bool Debug;  ///Enable/disable debug output to Serial
    bool Metric; ///Metric or Imperial
  };  
  struct HempyModuleSettings HempyMod1 = {.Debug = true, .Metric = true};  ///Default settings for the Hempy Module
  
  struct DHTSensorSettings
  { ///initialized via Designated initializer https:///riptutorial.com/c/example/18609/using-designated-initializers
    DHTSensorSettings(byte Pin = 0, byte Type = 0) : Pin(Pin), Type(Type) {}
    byte Pin;
    byte Type; ///Type defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings DHT1 = {.Pin = 8, .Type = 22};  ///Default settings for the DHT sensor
 
  struct SoundSettings
  {
    SoundSettings(byte Pin = 0) : Pin(Pin) {}
    byte Pin;            ///PC buzzer+ (red)
    bool Enabled = true; ///Enable PC speaker / Piezo buzzer
  };
  struct SoundSettings Sound1 = {.Pin = 2};  ///Default settings for the  Sound output

  struct PHSensorSettings
  {
    PHSensorSettings(byte Pin = 0, float Slope = 0.0, float Intercept = 0.0) : Pin(Pin), Slope(Slope), Intercept(Intercept) {}
    byte Pin;
    float Slope;
    float Intercept;
  };
  struct PHSensorSettings PHSensor1 = {.Pin = A6, .Slope = -0.033256, .Intercept = 24.08651}; //////Default settings for the PH sensor, update the calibration values

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

  struct WaterTempSensorSettings
  {
    WaterTempSensorSettings(byte Pin = 0) : Pin(Pin) {}
    byte Pin;
  };
  struct WaterTempSensorSettings WaterTemp1 = {.Pin = 7}; ///Data(yellow) - DS18B20 waterproof temp sensor

  struct WaterLevelSensorSettings
  {
    WaterLevelSensorSettings(byte Pin_1 = 0, byte Pin_2 = 0, byte Pin_3 = 0, byte Pin_4 = 0) : Pin_1(Pin_1), Pin_2(Pin_2), Pin_3(Pin_3), Pin_4(Pin_4) {}
    byte Pin_1; ///Lowest water level
    byte Pin_2;
    byte Pin_3;
    byte Pin_4; ///Full
  };
  struct WaterLevelSensorSettings WaterLevel1 = {.Pin_1 = A0, .Pin_2 = A1, .Pin_3 = A2, .Pin_4 = A3};

  byte CompatibilityVersion = Version; ///Should always be the last value stored.
} Settings;
