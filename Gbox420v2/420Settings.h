#pragma once

//Change this when you make change to the structure of the EEPROM stored part
static const byte Version = 6;

//THIS SECTION DOES NOT GET STORED IN EEPROM: They never change during runtime
static const byte MaxTextLength = 32;  //Default char* buffer size: 31 characters + null terminator, used for logging and converting to text
static const byte RollingAverageQueueDepth = 10;  //How many previous sensor readings should be stored
static const byte LogDepth = 10;  //Show X number of log entries on website, do not go above 10


//SAVED TO EEPROM
typedef struct
{     
  bool ATXPowerSupplyOn = true; //ATX power supply ON(true) or OFF(false)
  bool Sound1Enabled = true;  //Enable PC speaker / Piezo buzzer
  
struct AeroponicsSettings{
    bool SprayEnabled = true;  //Enable/disable misting
    int Interval = 15; //Aeroponics - Spray every 15 minutes
    int Duration = 2; //Aeroponics - Spray time in seconds
    int PumpTimeout = 6;  // Aeroponics - Max pump run time in minutes, measue zero to max pressuretank refill time and adjust accordingly
    int PrimingTime = 10;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
 } Aeroponics_Tank1,Aeroponics_NoTank1;

struct AeroponicsSettings_NoTankSpecific{  //Without pressure tank specific settings
    int BlowOffTime = 3; //Aeroponics - BlowOff time in seconds: After spraying open the bypass valve for X seconds to release pressure
 } Aeroponics_Tank1_NoTankSpecific;

  struct AeroponicsSettings_TankSpecific{  //Pressure tank specific settings
    float PressureLow= 5.0; //Aeroponics - Turn on pump below this pressure (bar)
    float PressureHigh = 7.0 ; //Aeroponics - Turn off pump above this pressure (bar)
    bool QuietEnabled = false;  //Enable/disable quiet time then pump should not run
    bool RefillBeforeQuiet = true; //Enable/disable refill before quiet time
    byte QuietFromHour = 23;  //Quiet time to block pump - hour
    byte QuietFromMinute = 0; //Quiet time to block pump - minute
    byte QuietToHour = 6; //Quiet time end - hour
    byte QuietToMinute = 0; //Quiet time end - minute
 } Aeroponics_Tank1_TankSpecific;

  struct LightsSettings{
    //Light specific settings, unique for each item, initialized via a consturctor
    LightsSettings(byte RelayPin = 0, byte DimmingPin = 0,byte DimmingLimit = 0 ) : RelayPin(RelayPin),DimmingPin(DimmingPin),DimmingLimit(DimmingLimit) {}
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
    
  struct PHSensorSettings{
    PHSensorSettings(float PHCalibrationSlope = 0.0, float PHCalibrationIntercept = 0.0 ) : PHCalibrationSlope(PHCalibrationSlope),PHCalibrationIntercept(PHCalibrationIntercept) {}
    float PHCalibrationSlope;     //Update this to your own PH meter calibration values
    float PHCalibrationIntercept;  //Update this to your own PH meter calibration values    f
    float PHAlertLow = 5.5; //Low pressure warning
    float PHAlertHigh = 6.5; //High pressure warning
  };
  struct PHSensorSettings PHSensor1 = {.PHCalibrationSlope = -0.033256, .PHCalibrationIntercept = 24.08651 };

  struct PressureSensorSettings{
    PressureSensorSettings(float PressureSensorOffset = 0.0, float PressureSensorRatio = 0.0 ) : PressureSensorOffset(PressureSensorOffset),PressureSensorRatio(PressureSensorRatio) {}
    float PressureSensorOffset;        //Pressure sensor calibration: voltage reading at 0 pressure
    float PressureSensorRatio;          //Pressure sensor voltage to pressure ratio
    float PressureAlertLow = 4.0; //Low pressure warning
    float PressureAlertHigh = 8.0; //High pressure warning 
  };
  struct PressureSensorSettings PressureSensor1 = {.PressureSensorOffset = 0.57, .PressureSensorRatio = 2.7 };

  struct DHTSensorSettings{
    DHTSensorSettings(byte Pin = 0 ) : Pin(Pin){}
    byte Pin;
    int TempAlertLow = 15; //Low temp warning email
    int TempAlertHigh = 35; //High temp warning email
    int HumidityAlertLow = 35; //Low humidity warning email
    int HumidityAlertHigh = 70; //High humidity warning email
  };
  struct DHTSensorSettings InternalDHTSensor = {.Pin = 43 };
  struct DHTSensorSettings ExternalDHTSensor = {.Pin = 44 };

  bool AutomaticInternalFan = false;  //Adjust internal fan based on temperature
  bool AutomaticExhaustFan = false;  //Adjust exhaust fan based on temp and humidity
  bool InternalFanOn = true;  //Internal fan On/Off, default:ON
  bool InternalFanHigh = true; //Internal fan Low/High, default:Low
  bool ExhaustFanOn = true;  //Exhaust fan On/Off, default:OFF
  bool ExhaustFanHigh = true;  //Exhaust fan Low/High, default:Low
  int InternalFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached. Has to match default unit type(Metric C or Imperial K)
  byte ExhaustFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  byte ExhaustFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  byte ExhaustFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
   
  bool DebugEnabled = true; //Logs debug LongMessages to serial and web outputs
  bool MetricSystemEnabled = true; //Switch between Imperial/Metric units. If changed update the default temp and pressure values.  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength]= "v420"; //UPDATE THIS DeviceID of the PushingBox logging scenario 

  byte DigitDisplayBacklight = 25; //4 digit display - backlight strength (0-100)
  byte DigitDisplayValue = 0; //select which sensor reading to display(1-18), 0 cycle through all values
    
  bool AlertEmails = true; //disable/enable email sending
  char PushingBoxEmailRelayID[MaxTextLength]  = "v420";  //UPDATE THIS DeviceID of the PushingBox email alert scenario
  int TriggerCountBeforeAlert = 12; //number of consecutive out of range sensor readings before the email alert is triggered (5sec between reads -> 12= Out of range reading through 1 minute)  


  byte CompatibilityVersion=Version;  //Should always be the last value stored.
   }Settings;
