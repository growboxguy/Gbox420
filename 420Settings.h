//Default settings of the grow box

//Global constants
  const bool debug = false; //Logs debug messages to serial and web outputs 
  const char PushingBoxLogRelayID[]= "v755877CF53383E1"; //UPDATE THIS to your PushingBox logging scenario`s DeviceID  
  const char PushingBoxEmailRelayID[]  = "vC5244859A2453AA";  //UPDATE THIS to your PushingBox email alert scenario`s DeviceID 
  const byte PotStepping = 100;  // Digital potentiometer adjustment steps
  const byte ScreenRotation = 1;  //LCD screen rotation: 1,3:landscape 2,4:portrait
  const unsigned long AeroPumpTimeout = 600000;  // Aeroponics - Max pump run time (10minutes)
  const float PressureSensorOffset = 0.58;  //Pressure sensor voltage reading at 0 pressure
  const float PressureSensorVoltageToPressure = 2.7; //Pressure sensor voltage to pressure ratio
  const float PHCalibrationSlope = -0.033256;  //Update this to your own PH meter calibration values
  const float PHCalibrationIntercept = 24.08651;  //Update this to your own PH meter calibration values
  const byte ReadCountBeforeAlert = 5; //number of consecutive out of range sensor readings for the alert to trigger
  const byte LogDepth = 8;  //Show X log entries on website
  const byte LogLength = 31;  //30 characters + null terminator for one log entry
    
//Settings saved to EEPROM persistent storage
  byte Version= 1; //increment this when you update the test values or change the stucture to invalidate the EEPROM stored settings
  typedef struct //when Version is changed these values get stored in EEPROM, else EEPROM content is loaded
  {
  byte AeroInterval = 15; //Aeroponics - Spray every 15 minutes
  byte AeroDuration = 2; //Aeroponics - Spray for 5 secondsf
  float AeroPressureLow= 5.5; //Aeroponics - Turn on pump below this pressure (bar)
  float AeroPressureHigh = 7.0 ; //Aeroponics - Turn on pump below this pressure (bar)
  bool isAeroSprayEnabled = true;  //Enable/disable misting
  bool isAeroQuietEnabled = true;  //Enable/disable quiet time
  bool AeroRefillBeforeQuiet = true; //Enable/disable refill before quiet time
  byte AeroQuietFromHour = 22;  //Quiet time to block pump - hour
  byte AeroQuietFromMinute = 00; //Quiet time to block pump - minute
  byte AeroQuietToHour = 8; //Quiet time end - hour
  byte AeroQuietToMinute = 00; //Quiet time end - minute
  
  bool isLightOn = true;  //Startup status for lights: True-ON / False-OFF
  byte LightBrightness; //0 - 100 range for controlling led driver output  
  byte LightOnHour = 4;  //Light ON time - hour
  byte LightOnMinute = 20; //Light ON time - minute
  byte LightOffHour = 16; //Light OFF time - hour
  byte LightOffMinute = 20; //Light OFF time - minute
  bool isTimerEnabled = false;  //Enable timer controlling lights  

  bool automaticInternalFan = true;  //Adjust internal fan based on temperature On/Off
  int internalFanSwitchTemp = 25; // Above limit turn the internal fan to High, turn to Low if limit-3 degrees is reached.
  bool automaticExhaustFan = true;  //Adjust exhaust fan based on temp and humidity On/Off
  int exhaustFanHighHumid = 65; //Above set humidity turn exhaust fan High if automatic fan control is enabled
  int exhaustFanLowHumid = 55; //Above set humidity turn exhaust fan Low if automatic fan control is enabled
  int exhaustFanOffHumid = 45; //Below set humidity turn exhaust fan Off if automatic fan control is enabled
  bool isInternalFanOn = true;  //Internal fan On/Off, default:ON
  bool isInternalFanHigh = false; //Internal fan Low/High, default:Low
  bool isExhaustFanOn = false;  //Exhaust fan On/Off, default:OFF
  bool isExhaustFanHigh = false;  //Exhaust fan Low/High, default:Low
  bool isPCPowerSupplyOn = true;  //Startup status for PC power supply: True-ON / False-OFF, default:ON
  
  bool ReportToGoogleSheets = true;
  bool ReportToMqtt = true;   
  
  byte DigitDisplayBacklight = 25; //4 digit display - backlight strenght (0-100)
  int DigitDisplayValue = -1; //select which screen to display(0-15), -1 cycles through all screens
  bool isSoundEnabled = true;  //Enable PC speaker  
 
  bool AlertEmails = true; //completely disable/enable email sending  
  int TempAlertLow = 15; //Low temp warning email
  int TempAlertHigh = 35; //High temp warning email
  int HumidityAlertLow = 35; //Low humidity warning email
  int HumidityAlertHigh = 75; //High humidity warning email
  float PressureAlertLow = 3.0; //Low pressure warning
  float PressureAlertHigh = 7.5; //High pressure warning  
  float PHAlertLow = 5.5; //Low pressure warning
  float PHAlertHigh = 6.8; //High pressure warning

  byte StructureVersion = Version;  
  } Settings;  //New type called: Settings
  Settings MySettings;  //create a variable of type "Settings"  with default values from the struct
