//Default settings of the grow box
//If you are new here: 


//Global constants
  const char PushingBoxLogRelayID[]= "v755877CF53383E1"; //UPDATE THIS to your PushingBox logging scenario`s DeviceID  
  const char PushingBoxEmailRelayID[]  = "vC5244859A2453AA";  //UPDATE THIS to your PushingBox email alert scenario`s DeviceID 
  const byte PotStepping = 100;  // Digital potentiometer adjustment step count
  const byte ScreenRotation = 1;  //LCD screen rotation: 1,3:landscape 2,4:portrait
  const unsigned long AeroPumpTimeout = 360000;  // Aeroponics - Max pump run time (6 minutes), measue zero to max pressuretank refill time and adjust accordingly
  const byte ReadCountBeforeAlert = 5; //number of consecutive out of range sensor readings before the email alert is triggered
  const byte LogDepth = 12;  //Show X log entries on website
  const byte LogLength = 31;  //30 characters + null terminator for one log entry
  const float DividingFactor = 4.7;  //Voltage dividing factor on the ATXPowerGood input = Measured voltage / Voltage over voltage devider
    
//Settings saved to EEPROM persistent storage
  byte Version= 4; //increment this when you change the Settings stucture to invalidate the EEPROM stored settings
  typedef struct //when Version is changed these values get stored in EEPROM, else EEPROM content is loaded
  {
  byte AeroInterval = 15; //Aeroponics - Spray every 15 minutes
  byte AeroDuration = 1; //Aeroponics - Spray time in seconds
  float AeroPressureLow= 5.5; //Aeroponics - Turn on pump below this pressure (bar)
  float AeroPressureHigh = 7.0 ; //Aeroponics - Turn off pump above this pressure (bar)
  bool isAeroSprayEnabled = true;  //Enable/disable misting
  bool isAeroQuietEnabled = true;  //Enable/disable quiet time then pump should not run
  bool AeroRefillBeforeQuiet = true; //Enable/disable refill before quiet time
  byte AeroQuietFromHour = 22;  //Quiet time to block pump - hour
  byte AeroQuietFromMinute = 0; //Quiet time to block pump - minute
  byte AeroQuietToHour = 8; //Quiet time end - hour
  byte AeroQuietToMinute = 0; //Quiet time end - minute
  bool isAirPumpOn = true;  //Startup status for Reservoir Air Pump: True-ON / False-OFF, default:ON
  
  bool isLightOn = true;  //Startup status for lights: True-ON / False-OFF
  byte LightBrightness = 0; //Light intensity: 0 - 100 range for controlling led driver output  
  byte LightOnHour = 4;  //Light ON time - hour
  byte LightOnMinute = 20; //Light ON time - minute
  byte LightOffHour = 16; //Light OFF time - hour
  byte LightOffMinute = 20; //Light OFF time - minute
  bool isTimerEnabled = true;  //Enable timer controlling lights  

  bool automaticInternalFan = true;  //Adjust internal fan based on temperature
  int internalFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached.
  bool automaticExhaustFan = true;  //Adjust exhaust fan based on temp and humidity
  int exhaustFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  int exhaustFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  int exhaustFanOffHumid = 40; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
  bool isInternalFanOn = true;  //Internal fan On/Off, default:ON
  bool isInternalFanHigh = false; //Internal fan Low/High, default:Low
  bool isExhaustFanOn = false;  //Exhaust fan On/Off, default:OFF
  bool isExhaustFanHigh = false;  //Exhaust fan Low/High, default:Low
  
  bool ReportToGoogleSheets = true;  //Controls reporting sensor readings to Google Sheets
  bool ReportToMqtt = true;    //Controls reporting sensor readings to an MQTT broker
  
  byte DigitDisplayBacklight = 25; //4 digit display - backlight strenght (0-100)
  int DigitDisplayValue = -1; //select which sensor reading to display(0-17), -1 cycles through all values
  bool isSoundEnabled = true;  //Enable PC speaker
  bool isDebugEnabled = false; //Logs debug messages to serial and web outputs  
 
  bool AlertEmails = true; //disable/enable email sending  
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
