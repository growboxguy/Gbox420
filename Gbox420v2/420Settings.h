#pragma once

//THIS SECTION DOES NOT GET STORED IN EEPROM: They never change during runtime
static const byte MaxTextLength = 32;  //Default char* buffer size: 31 characters + null terminator, used for logging and converting to text
static const byte RollingAverageQueueDepth = 10;  //How many previous sensor readings should be stored
static const byte LogDepth = 10;  //Show X number of log entries on website, do not go above 10

//SAVED TO EEPROM
typedef struct
{ 
  byte CompatibilityVersion=14;  //UPDATE this every time you change something in the stucture, that will force the EEPROM stored settings to update.
    
  bool ATXPowerSupplyOn = true; //ATX power supply ON(true) or OFF(false)
  
  bool AeroSprayEnabled = true;  //Enable/disable misting
  uint32_t AeroInterval = 15; //Aeroponics - Spray every 15 minutes
  uint32_t AeroDuration = 2; //Aeroponics - Spray time in seconds  
  uint32_t AeroPumpTimeout = 360;  // Aeroponics - Max pump run time in seconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
  uint32_t AeroPrimingTime = 10;  // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.

  bool Sound1Enabled = true;  //Enable PC speaker

  struct LightsSettings{
    bool Status = true;  //Startup status for lights: True-ON / False-OFF
    byte Brightness = 15; //Light intensity: 0 - 100 range for controlling led driver output
    bool TimerEnabled = true;  //Enable timer controlling lights  
    byte OnHour = 4;  //Light ON time - hour
    byte OnMinute = 20; //Light ON time - minute
    byte OffHour = 16; //Light OFF time - hour
    byte OffMinute = 20; //Light OFF time - minute    
  }Light1,Lights2;
  
//PHSensor1 settings  - Anybody knows how to handle multiple PH sensors with different inital calibration values?
  struct PHSensorSettings{
    float PHCalibrationSlope = -0.033256;     //Update this to your own PH meter calibration values
    float PHCalibrationIntercept = 24.08651;  //Update this to your own PH meter calibration values
    float PressureSensorOffset = 0.57;        //Pressure sensor calibration: voltage reading at 0 pressure
    float PressureSensorRatio = 2.7;          //Pressure sensor voltage to pressure ratio
    float PHAlertLow = 5.5; //Low pressure warning
    float PHAlertHigh = 6.5; //High pressure warning
  }PHSensor1;

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
   
  bool DebugEnabled = true; //Logs debug messages to serial and web outputs
  bool MetricSystemEnabled = true; //Swith between Imperial/Metric units. If changed update the default temp and pressure values.  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength]= "v420"; //UPDATE THIS DeviceID of the PushingBox logging scenario 

  byte DigitDisplayBacklight = 25; //4 digit display - backlight strenght (0-100)
  byte DigitDisplayValue = 0; //select which sensor reading to display(1-18), 0 cycle through all values
    
  bool AlertEmails = true; //disable/enable email sending
  char PushingBoxEmailRelayID[MaxTextLength]  = "v420";  //UPDATE THIS DeviceID of the PushingBox email alert scenario
  int TriggerCountBeforeAlert = 12; //number of consecutive out of range sensor readings before the email alert is triggered (5sec between reads -> 12= Out of range reading through 1 minute)  
  int TempAlertLow = 15; //Low temp warning email
  int TempAlertHigh = 35; //High temp warning email
  int HumidityAlertLow = 35; //Low humidity warning email
  int HumidityAlertHigh = 70; //High humidity warning email
  float PressureAlertLow = 4.0; //Low pressure warning
  float PressureAlertHigh = 8.0; //High pressure warning 
}Settings;
