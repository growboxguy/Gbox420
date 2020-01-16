#pragma once

//Update the Version when you make change to the structure of the EEPROM stored Settings struct. This will overwrite the EEPROM settings with the sketch defaults.
static const byte Version = 22;

//THIS SECTION DOES NOT GET STORED IN EEPROM: 
//Global constants
static const byte MaxTextLength = 32;  //Default char * buffer size: 31 characters + null terminator. Memory intense!
static const byte RollingAverageQueueDepth = 10;  //How many previous sensor readings should be stored
static const byte LogDepth = 5;  //Show X number of log entries on website. Be careful, Max 1024bytes can be passed during a Website refresh event, incuding all parameters passed
static const byte QueueDepth = 64;  //Limits the maximum number of active modules. Memory intense!
//Global variables
extern char LongMessage[1024];   //temp storage for assembling long messages (REST API, MQTT API)
extern char ShortMessage[128];  //temp storage for assembling short messages (Log entries, Error messages)
extern char CurrentTime[20]; //buffer for storing current time in text

//SAVED TO EEPROM - Settings struct
typedef struct
{  
  bool DebugEnabled = true; //Logs debug messages to serial and web outputs
  bool MetricSystemEnabled = true; //Switch between Imperial/Metric units. If changed update the default temp and pressure values.  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  int SheetsReportingFrequency = 30; //How often to report to Google Sheets. Use 15 minute increments only! Min 15min, Max 1440 (1day)
  //bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength]= "v755877CF53383E1"; //UPDATE THIS DeviceID of the PushingBox logging scenario 
  char PushingBoxEmailRelayID[MaxTextLength]  = "vC5244859A2453AA";  //UPDATE THIS DeviceID of the PushingBox email alert scenario

  struct SoundSettings{
    SoundSettings(byte Pin = 0) : Pin(Pin){} 
    byte Pin; //PC speaker+ (red)   
    bool Enabled = true;  //Enable PC speaker / Piezo buzzer
  };
  struct SoundSettings Sound1 = {.Pin = 4};

  struct LightSensorSettings{
    LightSensorSettings(byte DigitalPin = 0, byte AnalogPin = 0 ) : DigitalPin(DigitalPin),AnalogPin(AnalogPin){}
    byte DigitalPin;
    byte AnalogPin;     
  };
  struct LightSensorSettings LightSensor1 = {.DigitalPin = 50 , .AnalogPin = A0};

  struct LightsSettings{
   //Light specific settings, unique for each item, initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
    LightsSettings(byte RelayPin = 0, byte DimmingPin = 0,byte DimmingLimit = 0 ) : RelayPin(RelayPin),DimmingPin(DimmingPin),DimmingLimit(DimmingLimit) {} //Designated initializer 
    byte RelayPin;  //Power relay Port 8 - LED lights
    byte DimmingPin; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    byte DimmingLimit; //Sets the LED dimming limit (Usually around 5%)
    bool Status = true;  //Startup status for lights: True-ON / False-OFF
    byte Brightness = 15; //Light intensity: 0 - 100 range for controlling led driver output
    bool TimerEnabled = true;  //Enable timer controlling lights  
    byte OnHour = 4;  //Light ON time - hour
    byte OnMinute = 20; //Light ON time - minute
    byte OffHour = 16; //Light OFF time - hour
    byte OffMinute = 20; //Light OFF time - minute    
  };  
  struct LightsSettings Light1 = {.RelayPin = 29, .DimmingPin = 11, .DimmingLimit = 8}; //Creating a LightSettings instance, passing in the unique parameters
  
  struct DHTSensorSettings{
    DHTSensorSettings(byte Pin = 0 , byte Type = 0) : Pin(Pin),Type(Type){}
    byte Pin;
    byte Type; //Defines the sensor type: 11 - DHT11, 12 - DHT12, 21 - DHT21 or AM2301 , 22 - DHT22
  };
  struct DHTSensorSettings InDHT = {.Pin = 49, .Type = 22 };
  struct DHTSensorSettings ExDHT = {.Pin = 48, .Type = 22 };  

  struct PHSensorSettings{
    PHSensorSettings(byte Pin = 0 ,float Slope = 0.0, float Intercept = 0.0 ) : Pin(Pin),Slope(Slope),Intercept(Intercept) {}
    byte Pin;
    float Slope;     //Update this to your own PH meter calibration values
    float Intercept;  //Update this to your own PH meter calibration values
  };
  struct PHSensorSettings PHSensor1 = {.Pin = A3, .Slope = -0.033256, .Intercept = 24.08651 };

  struct PressureSensorSettings{
    PressureSensorSettings(byte Pin = 0, float Offset = 0.0, float Ratio = 0.0 ) : Pin(Pin),Offset(Offset),Ratio(Ratio) {}
    byte Pin;
    float Offset;        //Pressure sensor calibration: voltage reading at 0 pressure
    float Ratio;          //Pressure sensor voltage to pressure ratio
  };
  struct PressureSensorSettings Pressure1 = {.Pin = A1, .Offset = 0.57, .Ratio = 2.7 }; //Pressure sensor Pin: Signal(yellow)

  struct AeroponicsSettings{ //Common settings for each inheriting class
    AeroponicsSettings(byte BypassSolenoidPin = 0, byte PumpPin = 0 ) : BypassSolenoidPin(BypassSolenoidPin),PumpPin(PumpPin) {}
    byte BypassSolenoidPin; //Power relay Port 3 - Aeroponics bypass solenoid 
    byte PumpPin;  //Power relay Port 2 - Aeroponics high pressure pump
    bool SprayEnabled = true;  //Enable/disable misting
    int Interval = 15; //Aeroponics - Spray every 15 minutes
    int Duration = 2; //Aeroponics - Spray time in seconds
    int PumpTimeout = 6;  // Aeroponics - Max pump run time in minutes, measue zero to max pressuretank refill time and adjust accordingly
    int PrimingTime = 10;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
  };
  struct AeroponicsSettings Aero_T1_Common = {.BypassSolenoidPin = 30, .PumpPin = 31 };
  struct AeroponicsSettings Aero_NT1_Common = {.BypassSolenoidPin = 52, .PumpPin = 53 };
  
  struct AeroponicsSettings_NoTankSpecific{  //Without pressure tank specific settings    
    int BlowOffTime = 3; //Aeroponics - BlowOff time in seconds: After spraying open the bypass valve for X seconds to release pressure    
  }Aero_NT1_Specific;

  struct AeroponicsSettings_TankSpecific{  //Pressure tank specific settings
  AeroponicsSettings_TankSpecific(byte SpraySolenoidPin = 0) : SpraySolenoidPin(SpraySolenoidPin) {}
    byte SpraySolenoidPin;  //Power relay Port 1 - Aeroponics spray solenoid (Only used with Aeroponics_Tank module)
    float PressureLow= 5.0; //Aeroponics - Turn on pump below this pressure (bar)
    float PressureHigh = 7.0 ; //Aeroponics - Turn off pump above this pressure (bar)
    bool QuietEnabled = false;  //Enable/disable quiet time then pump should not run
    bool RefillBeforeQuiet = true; //Enable/disable refill before quiet time
    byte QuietFromHour = 23;  //Quiet time to block pump - hour
    byte QuietFromMinute = 0; //Quiet time to block pump - minute
    byte QuietToHour = 6; //Quiet time end - hour
    byte QuietToMinute = 0; //Quiet time end - minute
  };
  struct AeroponicsSettings_TankSpecific Aero_T1_Specific = {.SpraySolenoidPin= 22};

  struct WaterTempSensorSettings{
    WaterTempSensorSettings(byte Pin = 0) : Pin(Pin){} 
    byte Pin;
  };
  struct WaterTempSensorSettings WaterTemp1 = {.Pin = 51}; //Data(yellow) - DS18B20 waterproof temp sensor 

  struct WaterLevelSensorSettings{
    WaterLevelSensorSettings(byte Pin_1 = 0,byte Pin_2 = 0,byte Pin_3 = 0,byte Pin_4 = 0) : Pin_1(Pin_1),Pin_2(Pin_2),Pin_3(Pin_3),Pin_4(Pin_4) {} 
    byte Pin_1;  //Lowest water level
    byte Pin_2;
    byte Pin_3;
    byte Pin_4;  //Full
  };
  struct WaterLevelSensorSettings WaterLevel1 = {.Pin_1 = A4, .Pin_2 = A5, .Pin_3 = A6, .Pin_4 = A7, }; //Data(yellow) - DS18B20 waterproof temp sensor 

  struct FanSettings{
    FanSettings(byte OnOffPin = 0 , byte SpeedPin = 0) : OnOffPin(OnOffPin),SpeedPin(SpeedPin){}
    byte OnOffPin;
    byte SpeedPin;
    bool State = true;  //true - ON, false - OFF
    bool HighSpeed = false; //true - High speed, false - Low speed
  };
  struct FanSettings InFan = {.OnOffPin = 25, .SpeedPin = 26 };
  struct FanSettings ExFan = {.OnOffPin = 27, .SpeedPin = 28 };


  struct ModuleSkeletonSettings{
    ModuleSkeletonSettings(bool PersistentBool = 0 , float PersistentFloat = 0.0) : PersistentBool(PersistentBool),PersistentFloat(PersistentFloat){}
    bool PersistentBool;
    int PersistentInt = 420;  //Same value on all instances
    float PersistentFloat;
  };
  struct ModuleSkeletonSettings ModuleSkeleton1 = {.PersistentBool = false, .PersistentFloat = 1.23 };  //Instance 1
  struct ModuleSkeletonSettings ModuleSkeleton2 = {.PersistentBool = true, .PersistentFloat = 4.56 };  //Instance 2


    

  /*   
  bool AutomaticInFan = false;  //Adjust internal fan based on temperature
  bool AutomaticExFan = false;  //Adjust exhaust fan based on temp and humidity
  int InFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached. Has to match default unit type(Metric C or Imperial K)
  byte ExFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  byte ExFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  byte ExFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
  */
   
  byte CompatibilityVersion=Version;  //Should always be the last value stored.
}Settings;
