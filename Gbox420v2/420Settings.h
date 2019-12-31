#pragma once

//Change this when you make change to the structure of the EEPROM stored part
static const byte Version = 12;

//THIS SECTION DOES NOT GET STORED IN EEPROM: They never change during runtime
static const byte MaxTextLength = 32;  //Default char * buffer size: 31 characters + null terminator, used for logging and converting to text
static const byte RollingAverageQueueDepth = 10;  //How many previous sensor readings should be stored
static const byte LogDepth = 10;  //Show X number of log entries on website, do not go above 10
static const byte QueueDepth = 32;  //Limits the refresh queue depth. Memory usage is multiplied by the number of refresh queues!

//SAVED TO EEPROM
typedef struct
{  
  bool DebugEnabled = true; //Logs debug LongMessages to serial and web outputs
  bool MetricSystemEnabled = true; //Switch between Imperial/Metric units. If changed update the default temp and pressure values.  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength]= "v755877CF53383E1"; //UPDATE THIS DeviceID of the PushingBox logging scenario 
    
  bool AlertEmails = true; //disable/enable email sending
  char PushingBoxEmailRelayID[MaxTextLength]  = "vC5244859A2453AA";  //UPDATE THIS DeviceID of the PushingBox email alert scenario
  int TriggerCountBeforeAlert = 12; //number of consecutive out of range sensor readings before the email alert is triggered (5sec between reads -> 12= Out of range reading through 1 minute)  

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
  struct LightSensorSettings LightSensor1 = {.DigitalPin = 36 , .AnalogPin = A0};

  struct LightsSettings{
   //Light specific settings, unique for each item, initialized via Designated initializer https://riptutorial.com/c/example/18609/using-designated-initializers
    LightsSettings(byte RelayPin = 0, byte DimmingPin = 0,byte DimmingLimit = 0 ) : RelayPin(RelayPin),DimmingPin(DimmingPin),DimmingLimit(DimmingLimit) {} //Designated initializer 
    byte RelayPin;  //Power relay Port 8 - LED lights
    byte DimmingPin; //PWM based dimming, connected to optocoupler`s base over 1k ohm resistor
    byte DimmingLimit; //Sets the LED dimming limit (Usually around 5%)

    //Default settings for all light objects
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
    int TempAlertLow = 15; //Low temp warning email
    int TempAlertHigh = 35; //High temp warning email
    int HumidityAlertLow = 35; //Low humidity warning email
    int HumidityAlertHigh = 70; //High humidity warning email
  };
  struct DHTSensorSettings InternalDHTSensor = {.Pin = 43, .Type = 22 };
  struct DHTSensorSettings ExternalDHTSensor = {.Pin = 44, .Type = 22 };  

  struct PHSensorSettings{
    PHSensorSettings(float PHCalibrationSlope = 0.0, float PHCalibrationIntercept = 0.0 ) : PHCalibrationSlope(PHCalibrationSlope),PHCalibrationIntercept(PHCalibrationIntercept) {}
    float PHCalibrationSlope;     //Update this to your own PH meter calibration values
    float PHCalibrationIntercept;  //Update this to your own PH meter calibration values    f
    float PHAlertLow = 5.5; //Low pressure warning
    float PHAlertHigh = 6.5; //High pressure warning
  };
  struct PHSensorSettings PHSensor1 = {.PHCalibrationSlope = -0.033256, .PHCalibrationIntercept = 24.08651 };

  struct PressureSensorSettings{
    PressureSensorSettings(byte Pin = 0, float Offset = 0.0, float Ratio = 0.0 ) : Pin(Pin),Offset(Offset),Ratio(Ratio) {}
    byte Pin;
    float Offset;        //Pressure sensor calibration: voltage reading at 0 pressure
    float Ratio;          //Pressure sensor voltage to pressure ratio
    float AlertLow = 4.0; //Low pressure warning
    float AlertHigh = 8.0; //High pressure warning 
  };
  struct PressureSensorSettings PressureSensor1 = {.Pin = A1, .Offset = 0.57, .Ratio = 2.7 };

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
  struct AeroponicsSettings Aeroponics_Tank1_Common = {.BypassSolenoidPin = 30, .PumpPin = 31 };
  struct AeroponicsSettings Aeroponics_NoTank1_Common = {.BypassSolenoidPin = 23, .PumpPin = 24 };
  
  struct AeroponicsSettings_NoTankSpecific{  //Without pressure tank specific settings    
    int BlowOffTime = 3; //Aeroponics - BlowOff time in seconds: After spraying open the bypass valve for X seconds to release pressure    
  }Aeroponics_NoTank1_Specific;

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
  struct AeroponicsSettings_TankSpecific Aeroponics_Tank1_Specific = {.SpraySolenoidPin= 22};

/* THIS DOES NOT WORK
   struct AeroponicsSettings_Tank {
    union{
      AeroponicsSettings_TankSpecific Specific;
      AeroponicsSettings Common;      
    };
  };
  struct AeroponicsSettings_Tank Aeroponics_Tank1 = { .Specific{.SpraySolenoidPin= 22}, .Common{.BypassSolenoidPin = 23, .PumpPin = 24 }};
 */

struct FanSettings{
    FanSettings(byte OnOffPin = 0 , byte SpeedPin = 0) : OnOffPin(OnOffPin),SpeedPin(SpeedPin){}
    byte OnOffPin;
    byte SpeedPin;
    bool State = true;  //true - ON, false - OFF
    bool HighSpeed = false; //true - High speed, false - Low speed
  };
  struct FanSettings InternalFan = {.OnOffPin = 25, .SpeedPin = 26 };
  struct FanSettings ExhaustFan = {.OnOffPin = 27, .SpeedPin = 28 };

/*   
  bool AutomaticInternalFan = false;  //Adjust internal fan based on temperature
  bool AutomaticExhaustFan = false;  //Adjust exhaust fan based on temp and humidity
  int InternalFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached. Has to match default unit type(Metric C or Imperial K)
  byte ExhaustFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  byte ExhaustFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  byte ExhaustFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
*/

//PHSensor pins
  byte PHSensorInPin = A3;  //PH meter - Po port


//Analog pins
  byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor
  byte WaterCriticalInPin = A4; //Water sensor1
  byte WaterLowInPin = A5;     //Water sensor2
  byte WaterMediumInPin = A6; //Water sensor3
  byte WaterFullInPin = A7; // Water sensor4
  
//Digital pins

  byte BuiltInLEDOutPin = 13;  //Built-in LED light for testing
  byte ATXPowerONOutPin = 34; //Turns ATX power supply on by connecting ATX PowerON pin to GND through an optocoupler
  byte ATXPowerGoodInPin = 35; //5V signal from ATX powersupply, inverted by optocoupler: LOW if DC power output is OK
  byte ReservoirTempSensorInPin = 45;  //Data(yellow) - DS18B20 waterproof temp sensor 
  
  byte CompatibilityVersion=Version;  //Should always be the last value stored.
   }Settings;
