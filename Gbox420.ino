//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for grow box monitoring and controlling

//TODO: flow meter,PH Meter calibration,email alerts review
//TODO: publish set of bookmarks useful for the box to the webinterface (Gbox420,Pushingbox,DIoTY,Sheets..)


//Libraries
  #include "avr/wdt.h" //Watchdog timer
  #include "Thread.h"  //Splitting functions to threads
  #include "StaticThreadController.h"  //Grouping threads
  #include "TimerThree.h"  //Interrupt handling for webpage
  #include "DHT.h"  //DHT11 or DHT22 humidity and temperature sensor
  #include "SevenSegmentTM1637.h" // 4 digit LED display
  #include "SevenSegmentExtended.h" // 4 digit LED display
  #include "PZEM004T.h"  //Power meter
  #include "ELClient.h"  // ESP-link
  #include "ELClientRest.h" // ESP-link - REST API
  #include "ELClientWebServer.h" //ESP-link - WebServer API
  #include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
  #include "ELClientMqtt.h" //ESP-link - MQTT protocol for sending and receiving IoT messages
  #include "DS1302.h" //Real time clock
  #include "Time.h"  //Time handling
  #include "Stdio.h" //Time formatting
  #include "SPI.h" //TFT Screen - communication
  #include "Adafruit_ILI9341.h" //TFT Screen - hardware specific driver
  #include "Adafruit_GFX.h" //TFT Screen - generic graphics driver
  //#include "MemoryFree.h" //checking for remaining memory - only for debugging

//Pins
  const byte BuzzerOutPin = 4; //PC speaker+ (red)
  const byte BuiltInLEDOutPin = 13;  //Built-in LED light for testing
  const byte PowerLEDOutPin = 46;  //PC case Power LED
  //const byte HddLEDOutPin = 47; //PC case HDD LED - rewired to ESP chip`s wifi status pin
  const byte PowerButtonInPin = 49;  //Power button 
  const byte LightSensorInPin = 36; //D0 - LM393 light sensor                                                                                                              
  const byte LightSensorAnalogInPin = A0; //A0 - LM393 light sensor
  const byte WaterCriticalInPin = A4; //Water sensor1
  const byte WaterLowInPin = A5;     //Water sensor2
  const byte WaterMediumInPin = A6; //Water sensor3
  const byte WaterFullInPin = A7; // Water sensor4
  const byte DigitDisplayCLKOutPin = 31; //CLK - 4 digit LED display
  const byte DigitDisplayDI0OutPin = 30; //DIO - 4 digit LED display
  const byte TempSensorInPin = 43; //DAT - DHT22 temp/humidity sensor
  const byte ClockRSTPin = 5;  //Reset -  Real time clock
  const byte ClockDATPin = 6;  //Data -  Real time clock
  const byte ClockCLKPin = 7;  //Serial Clock -  Real time clock
  const byte Relay1OutPin = 22;  //Power relay Port 1 - Aeroponics solenoid
  const byte Relay2OutPin = 23;  //Power relay Port 2 - Aeroponics high pressure pump
  const byte Relay3OutPin = 24;  //Power relay Port 3 - PC power supply
  const byte Relay4OutPin = 25;  //Power relay Port 4 - Internal fan Off/On
  const byte Relay5OutPin = 26;  //Power relay Port 5 - Internal fan Low/High
  const byte Relay6OutPin = 27;  //Power relay Port 6 - Exhauset fan Off/On
  const byte Relay7OutPin = 28;  //Power relay Port 7 - Exhauset fan Low/High
  const byte Relay8OutPin = 29;  //Power relay Port 8 - LED lights
  const byte PHMeterInPin = A3;  //Po - PH meter
  const byte AMoistureSensorInPin = A8; //A0 - Soil moisture sensor 
  const byte DMoistureSensorInPin = 45; //D0 - Soil moisture sensor
  const byte PotCSOutPin = 34;  //CS - X9C104 digital potentiometer  
  const byte PotUDOutPin = 32;  //UD - X9C104 digital potentiometer
  const byte PotINCOutPin = 33; //INC - X9C104 digital potentiometer
  const byte ScreenReset = 37; //RESET(3.3V) - Screen Screen
  const byte ScreenSCK = 38;  //SCK(3.3V) - Screen Screen
  const byte ScreenMOSI = 39; //SDO/MOSI(3.3V) - Screen Screen
  const byte ScreenMISO = 40; //SDI/MISO(3.3V), not used - Screen Screen 
  const byte ScreenCS = 41;  //CS(3.3V) - Screen Screen
  const byte ScreenDC = 42; //DC/RS(3.3V) - Screen Screen
  const byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor

//Global constants
  const int UTCOffsetHour = 0;  //UTC Time hour offset (Can be adjusted on the Web interface too)
  const int UTCOffsetMinute = 0;  //UTC Time minute offset
  const char PushingBoxDeviceID[]= "v755877CF53383E1"; //UPDATE THIS to your grow box logging scenario DeviceID from PushingBox
  const char ReservoirAlertDeviceID[]  = "v6DA52FDF6FCDF74";  //UPDATE THIS to your reservoir alert scenario DeviceID from PushingBox
  const char PumpAlertDeviceID[]  = "v9F3E0683C4B3B49";  //UPDATE THIS to your pump alert scenario DeviceID from PushingBox
  const uint8_t  DHTType=DHT22;  //specify temp/humidity sensor type: DHT21(AM2301) or DHT22(AM2302,AM2321)
  const byte ScreenRotation = 1;  //1,3:landscape 2,4:portrait
  const byte LogDepth = 8;  //Show X log entries on website
  const byte LogLength = 31;  //30 characters + null terminator for one log entry
  const byte PotStepping = 100;  // Digital potentiometer adjustment steps
  const unsigned long AeroPumpTimeout = 900000;  // Aeroponics - Max pump run time (15minutes)
 
//Settings saved to EEPROM persistent storage
  byte Version= 2; //increment this when you update the test values or change the stucture to invalidate the EEPROM stored settings
  struct SettingsStructure //when Version is changed these values get stored in EEPROM, else EEPROM content is loaded
  {
  bool isLightOn = true;  //Startup status for lights: True-ON / False-OFF
  bool isSoundEnabled = true;  //Enable PC speaker
  byte LightBrightness; //0 - 100 range for controlling led driver output
  byte DigitDisplayBacklight = 75; //4 digit display - backlight strenght (0-100)
  bool isInternalFanOn;  //Internal fan On/Off
  bool isInternalFanHigh; //Internal fan Low/High
  bool isExhaustFanOn;  //Exhaust fan On/Off
  bool isExhaustFanHigh;  //Exhaust fan Low/High
  bool isTimerEnabled = false;  //Enable timer controlling lights
  bool isPCPowerSupplyOn = true;  //Startup status for PC power supply: True-ON / False-OFF 
  byte LightOnHour = 4;  //Light ON time - hour
  byte LightOnMinute = 20; //Light ON time - minute
  byte LightOffHour = 16; //Light OFF time - hour
  byte LightOffMinute = 20; //Light OFF time - minute
  bool isAeroSprayEnabled = true;
  unsigned long AeroInterval = 15; //Aeroponics - Spray every 15 minutes
  unsigned long AeroDuration = 5; //Aeroponics - Spray for 5 secondsf
  float AeroPressureLow= 5.5; //Aeroponics - Turn on pump below this pressure (bar)
  float AeroPressureHigh = 7.0 ; //Aeroponics - Turn on pump below this pressure (bar)
  float AeroOffset = 0.5; //Pressure sensor calibration - offset voltage
  bool isAeroQuietEnabled = true;  //Enable/disable quiet time
  byte AeroQuietFromHour = 21;  //Quiet time to block pump - hour
  byte AeroQuietFromMinute = 00; //Quiet time to block pump - minute
  byte AeroQuietToHour = 9; //Quiet time end - hour
  byte AeroQuietToMinute = 00; //Quiet time end - minute
  bool ReportToGoogleSheets = true;
  bool ReportToMqtt = true;
  byte StructureVersion = Version;  //do not update this value inside the loop
  };
  typedef struct SettingsStructure settings;  //create the "settings" type using the stucture
  settings MySettings;  //create a variable of type "settings"  with default values from SettingsStructure

//Global variables
  float BoxTempC; // stores Temperature - Celsius
  float BoxTempF; // stores Temperature - Fahrenheit
  float Humidity; // stores relative humidity - %
  float Power; //Power sensor - W
  float Energy; //Power sensor - Wh
  float Voltage; //Power sensor - V
  float Current; //Power sensor - A
  float PH; //PH meter  
  int LightReading;  //light sensor analog reading
  byte LightReadingPercent;  //Compared to calibrated min/max values what is the detected light level %
  bool isBright;  //Ligth sensor feedback: True-Bright / False-Dark
  bool isPotGettingHigh = false;  // Digital potentiometer direction, false: Decrease , true: Increase 
  bool isWaterAboveCritical = false; //Water sensor, true if level reached
  bool isWaterAboveLow = false; //Water sensor, true if level reached
  bool isWaterAboveMedium = false; //Water sensor, true if level reached
  bool isWaterAboveFull = false; //Water sensor, true if level reached
  bool PlayOnSound = false; //Turn on sound - website controls it
  bool PlayOffSound = false; //Turn off sound - website controls it
  bool PlayEE = false; //Surprise :) - website controls it
  bool UpdateNtpTime = false; //When set to true clock is updated using NTP
  bool CalibrateLights = false; //Calibrate lights flag - website controls it
  int MaxLightReading = 0; // stores the highest light source strengt reading
  int MinLightReading = 1023; //stores the lowest light source strengt reading
  bool isWet = false; //Moisture content reaches pre-set threshold
  int MaxMoisture = 0; // stores the highest moisture reading
  int MinMoisture = 1023; //stores the lowest moisture reading
  int Moisture ; //Reading of moisture content
  byte MoisturePercentage ; //Moisture content relative to max,min values measued
  char CurrentTime[20];  //time gets read into this from clock  
  unsigned long AeroSprayTimer = millis();  //Aeroponics - Spary cycle timer - https://www.arduino.cc/reference/en/language/functions/time/millis/
  unsigned long AeroPumpTimer = millis();  //Aeroponics - Pump cycle timer
  bool isAeroSprayOn = false; //Aeroponics - Spray state, set to true to spay at power on
  bool isAeroPumpOn = false; //Aeroponics - High pressure pump state
  bool isAeroPumpDisabled = false; //Aeroponics - High pressure pump health
  float AeroPressure = 0.0;  //Aeroponics - Current pressure (bar)
  float AeroPressurePSI = 0.0;  //Aeroponics - Current pressure (psi)
  char LogMessage[LogLength]; //temp storage for assemling log messages
  char Logs[LogDepth][LogLength];  //two dimensional array for storing log histroy (array of char arrays)
  char WebMessage[512];   //buffer for REST and MQTT API messages

//Component initialization
  DS1302 Clock(ClockRSTPin, ClockDATPin, ClockCLKPin); //Real time clock
  DHT TempSensor(TempSensorInPin, DHTType); // Temp/Humidity sensor
  SevenSegmentExtended DigitDisplay(DigitDisplayCLKOutPin, DigitDisplayDI0OutPin); //4 digit LED Display
  PZEM004T PowerSensor(&Serial2);  // Power Sensor using hardware Serial 2, use software serial if needed
  IPAddress PowerSensorIP(192,168,1,1); // Power Sensor address (fake,just needs something set)
  ELClient ESPLink(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
  ELClientRest RestAPI(&ESPLink);    // ESP-link REST API
  ELClientWebServer WebServer(&ESPLink); //ESP-link WebServer API
  ELClientCmd EspCmd(&ESPLink);//ESP-link - Get current time from the internet using NTP
  ELClientMqtt Mqtt(&ESPLink); //ESP-link - MQTT protocol for sending and receiving IoT messages
  Adafruit_ILI9341 Screen = Adafruit_ILI9341(ScreenCS, ScreenDC, ScreenMOSI, ScreenSCK, ScreenReset, ScreenMISO);

//Threading to time tasks
  Thread OneSecThread = Thread();
  Thread FiveSecThread = Thread();
  Thread MinuteThread = Thread();
  Thread HalfHourThread = Thread();
  StaticThreadController<4> ThreadControl (&OneSecThread,&FiveSecThread,&MinuteThread,&HalfHourThread);

void setup() {     // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  wdt_enable(WDTO_8S); //Watchdog timer set to 8 seconds
  addToLog("GrowBox initializing...");
  loadSettings();

   //Pin setup, defining what Pins are inputs/outputs and setting initial output signals
  pinMode(LightSensorInPin, INPUT);
  pinMode(WaterCriticalInPin, INPUT_PULLUP);
  pinMode(WaterLowInPin, INPUT_PULLUP);
  pinMode(WaterMediumInPin, INPUT_PULLUP);
  pinMode(WaterFullInPin, INPUT_PULLUP);
  pinMode(BuzzerOutPin, OUTPUT);
  pinMode(BuiltInLEDOutPin, OUTPUT);
  pinMode(Relay1OutPin, OUTPUT);
  pinMode(Relay2OutPin, OUTPUT);
  pinMode(Relay3OutPin, OUTPUT);
  pinMode(Relay4OutPin, OUTPUT);
  pinMode(Relay5OutPin, OUTPUT);
  pinMode(Relay6OutPin, OUTPUT);
  pinMode(Relay7OutPin, OUTPUT);
  pinMode(Relay8OutPin, OUTPUT);
  pinMode(DigitDisplayCLKOutPin, OUTPUT);
  pinMode(DigitDisplayDI0OutPin, OUTPUT);  
  pinMode(PowerLEDOutPin, OUTPUT);
  //pinMode(HddLEDOutPin, OUTPUT);
  pinMode(PowerButtonInPin, INPUT_PULLUP);
  pinMode(PotCSOutPin, OUTPUT);
  pinMode(PotUDOutPin, OUTPUT);
  pinMode(PotINCOutPin, OUTPUT);
  pinMode(ScreenReset, OUTPUT);
  pinMode(ScreenDC, OUTPUT);
  pinMode(ScreenCS, OUTPUT);
  pinMode(ScreenMOSI, OUTPUT);
  pinMode(ScreenMISO, INPUT);
  pinMode(ScreenSCK, OUTPUT);
  digitalWrite(PotCSOutPin,LOW); //CS input is low when the chip is active, https://www.intersil.com/content/dam/intersil/documents/x9c1/x9c102-103-104-503.pdf
  digitalWrite(BuiltInLEDOutPin, LOW); //without this LED would be randomly on or off when the board is powered on
  digitalWrite(Relay1OutPin, HIGH); //default OFF
  digitalWrite(Relay2OutPin, HIGH); 
  digitalWrite(Relay3OutPin, HIGH);
  digitalWrite(Relay4OutPin, HIGH);
  digitalWrite(Relay5OutPin, HIGH);
  digitalWrite(Relay6OutPin, HIGH);
  digitalWrite(Relay7OutPin, HIGH); 
  digitalWrite(Relay8OutPin, HIGH); 
    
  //Initialize web connections
  ESPLink.resetCb = resetWebServer;  //Callback subscription: When wifi reconnects, restart the WebServer
  resetWebServer();
  setupMqtt();
 
  //Threading
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(oneSecRun);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(fiveSecRun);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(minuteRun);
  HalfHourThread.setInterval(1800000);
  HalfHourThread.onRun(halfHourRun);

  //Start devices
  TempSensor.begin(); //start humidity/temp sensor
  Screen.begin(); //start screen
  Screen.setRotation(ScreenRotation);
  DigitDisplay.begin(); //start 4 digit LED display
  DigitDisplay.setBacklight(MySettings.DigitDisplayBacklight); //set 4 digit LED display backlight intensity
  PowerSensor.setAddress(PowerSensorIP); //start power meter
  calibrateLights();
  if(MySettings.AeroOffset == 1023) calibratePressureSensor();
  addToLog("Grow Box initialized");

  //triger all threads at startup
  fiveSecRun(); //needs to run first to get sensor readings
  oneSecRun();  
  minuteRun();
  halfHourRun();
  
  //Start interrupts to handle request from ESP-Link website
  Timer3.initialize(500);
  Timer3.attachInterrupt(processEspLink);
  Timer3.start();
}

void loop() {  // put your main code here, to run repeatedly:
  ThreadControl.run();    
}

void processEspLink(){
  ESPLink.Process();
}

void oneSecRun(){
  //addToLog("One sec trigger..");
  wdt_reset(); //reset watchdog timeout
  lightCheck(); 
  aeroCheck();  
  relayCheck();
  soundCheck();  
}

void fiveSecRun(){
  //LogToSerials("Five sec trigger..",true);
  wdt_reset(); //reset watchdog timeout
  getRTCTime();     
  readSensors();
  wdt_reset(); //reset watchdog timeout
  updateDisplay(); //Updates 7 digit display  
  //LogToSerials(freeMemory(),true);  
}

void minuteRun(){
  //LogToSerials("Minute trigger..",true);
  wdt_reset(); //reset watchdog timeout
  checkLightTimer(); 
  logToSerial();  //Logs sensor readings to Serial  
  logToScreen();  //Display sensore readings on lcd screen  
}

void halfHourRun(){
  //LogToSerials("Half hour trigger..",true);
  wdt_reset(); //reset watchdog timeout
  if(MySettings.ReportToGoogleSheets) ReportToGoogleSheets(false);  //uploads readings to Google Sheets via ESP REST API
  if(MySettings.ReportToMqtt) mqttPublush(false);  //publish readings via ESP MQTT API
}

//Helper functions

void saveSettings(bool LogMessage){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings));
  if(LogMessage) addToLog("Settings saved to EEPROM");
}

void loadSettings(){
  settings EEPROMSettings; //temporary storage with same "settings" type
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));  
  if(EEPROMSettings.StructureVersion != MySettings.StructureVersion){
    Serial.print(F("Change detected, updating EEPROM..."));
    saveSettings(false);
  }
  else {
    Serial.print(F("Same structure version detected, applying restored settings..."));
    MySettings = EEPROMSettings;
  }
  Serial.println("done");
}

void SendEmailAlert(const char* AlertType){
  memset(&WebMessage[0], 0, sizeof(WebMessage));  //clear variable
  strcat(WebMessage,"/pushingbox?devid="); strcat(WebMessage,AlertType);  
  //LogToSerials(WebMessage,true);   
  RestAPI.get(WebMessage);
}
