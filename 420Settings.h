//Default settings of the grow box can be adjusted here

//Global constants
  const byte MaxDimming = 235; //Sets the maximum dimming duty cycle to 92% (255*0.92=~235). 255=100% dimming (LED drivers usually do not support full dimming, check the specification before changing this!)
  const byte ScreenRotation = 1;  //LCD screen rotation: 1,3:landscape 2,4:portrait
  const byte LogDepth = 10;  //Show X number of log entries on website, do not go above 10
  const byte MaxTextLength = 31;  //30 characters + null terminator for one log entry
   
//Settings saved to EEPROM persistent storage
  byte Version= 5; //increment this when you change the Settings stucture to invalidate the EEPROM stored settings
  typedef struct //when Version is changed these values get stored in EEPROM, else EEPROM content is loaded
  {
  byte AeroInterval = 15; //Aeroponics - Spray every 15 minutes
  byte AeroDuration = 1; //Aeroponics - Spray time in seconds
  float AeroPressureLow= 5.5; //Aeroponics - Turn on pump below this pressure (bar)
  float AeroPressureHigh = 7.0 ; //Aeroponics - Turn off pump above this pressure (bar)
  bool AeroSprayEnabled = true;  //Enable/disable misting
  bool AeroQuietEnabled = false;  //Enable/disable quiet time then pump should not run
  bool AeroRefillBeforeQuiet = true; //Enable/disable refill before quiet time
  byte AeroQuietFromHour = 22;  //Quiet time to block pump - hour
  byte AeroQuietFromMinute = 0; //Quiet time to block pump - minute
  byte AeroQuietToHour = 8; //Quiet time end - hour
  byte AeroQuietToMinute = 0; //Quiet time end - minute
  bool AirPumpOn = false;  //Startup status for Reservoir Air Pump: True-ON / False-OFF, default:ON
  uint32_t AeroPumpTimeout = 360000;  // Aeroponics - Max pump run time in miliseconds (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
 
  bool LightStatus = true;  //Startup status for lights: True-ON / False-OFF
  byte LightBrightness = 0; //Light intensity: 0 - 100 range for controlling led driver output  
  byte LightOnHour = 4;  //Light ON time - hour
  byte LightOnMinute = 20; //Light ON time - minute
  byte LightOffHour = 16; //Light OFF time - hour
  byte LightOffMinute = 20; //Light OFF time - minute
  bool TimerEnabled = true;  //Enable timer controlling lights  

  bool AutomaticInternalFan = true;  //Adjust internal fan based on temperature
  int InternalFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached.
  bool AutomaticExhaustFan = true;  //Adjust exhaust fan based on temp and humidity
  int ExhaustFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  int ExhaustFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  int ExhaustFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
  bool InternalFanOn = true;  //Internal fan On/Off, default:ON
  bool InternalFanHigh = false; //Internal fan Low/High, default:Low
  bool ExhaustFanOn = false;  //Exhaust fan On/Off, default:OFF
  bool ExhaustFanHigh = false;  //Exhaust fan Low/High, default:Low
   
  bool SoundEnabled = true;  //Enable PC speaker
  bool DebugEnabled = true; //Logs debug messages to serial and web outputs
  bool MetricSystemEnabled = true; //Swith between Imperial/Metric units  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  char PushingBoxLogRelayID[MaxTextLength]= "v420"; //UPDATE THIS DeviceID of the PushingBox logging scenario 

  byte DigitDisplayBacklight = 25; //4 digit display - backlight strenght (0-100)
  int DigitDisplayValue = -1; //select which sensor reading to display(0-17), -1 cycle through all values
    
  bool AlertEmails = true; //disable/enable email sending
  char PushingBoxEmailRelayID[MaxTextLength]  = "v420";  //UPDATE THIS DeviceID of the PushingBox email alert scenario
  int ReadCountBeforeAlert = 12; //number of consecutive out of range sensor readings before the email alert is triggered (5sec between reads -> 12= Out of range reading through 1 minute)  
  int TempAlertLow = 15; //Low temp warning email
  int TempAlertHigh = 35; //High temp warning email
  int HumidityAlertLow = 35; //Low humidity warning email
  int HumidityAlertHigh = 70; //High humidity warning email
  float PressureAlertLow = 3.0; //Low pressure warning
  float PressureAlertHigh = 7.5; //High pressure warning  
  float PHAlertLow = 5.5; //Low pressure warning
  float PHAlertHigh = 6.5; //High pressure warning

  float PressureSensorOffset = 0.57;        //Pressure sensor calibration: voltage reading at 0 pressure
  float PressureSensorRatio = 2.7;          //Pressure sensor voltage to pressure ratio
  float PHCalibrationSlope = -0.033256;     //Update this to your own PH meter calibration values
  float PHCalibrationIntercept = 24.08651;  //Update this to your own PH meter calibration values

  byte StructureVersion = Version;  
  } Settings;  //New type called: Settings
  Settings MySettings;  //create a variable of type "Settings"  with default values from the struct
