//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for grow box monitoring and controlling

//TODO: flow meter

//Libraries
  #include "420Pins.h" //Load pins layout file
  #include "420Settings.h" //Load settings file
  #include "avr/wdt.h" //Watchdog timer
  #include "Thread.h"  //Splitting functions to threads for timing
  #include "StaticThreadController.h"  //Grouping threads
  #include "TimerThree.h"  //Interrupt handling for webpage
  #include "DHT.h"  //DHT11 or DHT22 Digital Humidity and Temperature sensor
  #include "SevenSegmentTM1637.h" // 4 digit LED display
  #include "SevenSegmentExtended.h" // 4 digit LED display
  #include "PZEM004T.h"  //Power meter
  #include "ELClient.h"  // ESP-link
  #include "ELClientRest.h" // ESP-link - REST API
  #include "ELClientWebServer.h" //ESP-link - WebServer API
  #include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
  #include "ELClientMqtt.h" //ESP-link - MQTT protocol for sending and receiving IoT messages
  #include "TimeLib.h" //Keeping track of time
  #include "SPI.h" //TFT Screen - communication
  #include "Adafruit_GFX.h" //TFT Screen - generic graphics driver
  #include "Adafruit_ILI9341.h" //TFT Screen - hardware specific driver
  //#include "MemoryFree.h" //checking for remaining memory - only for debugging

//Global variables
  bool LightOK = true; //Track component health, at startup assume every component is OK
  bool PressureOK = true;  //Aeroponics - Pressure within range
  bool PumpOK = true; //Aeroponics - High pressure pump health
  bool PowerOK = true; //AC voltage present
  bool VentOK = true; //Temperatur and humidity withing range
  bool ReservOK = true; //Reservoir not empty
  bool PhOK = true;  //Nutrient solution PH within range
  byte LightsAlertCount = 0;  //Counters of out of range readings before triggering an alert
  byte PressureAlertCount = 0; //All counters are compared agains ReadCountBeforeAlert from 420Settings
  byte PowerAlertCount = 0;
  byte VentilationAlertCount = 0;
  byte ReservoirAlertCount = 0;
  byte PHAlertCount = 0;
  float BoxTempC; // stores Temperature - Celsius
  float BoxTempF; // stores Temperature - Fahrenheit
  float Humidity; // stores relative humidity - %
  float Power; //Power sensor - W
  float Energy; //Power sensor - Wh Total consumption 
  float Voltage; //Power sensor - V
  float Current; //Power sensor - A
  float PHRaw; //PH meter reading 
  float PH; //Calculated PH  
  int LightReading;  //light sensor analog reading
  bool isBright;  //Ligth sensor digital feedback: True-Bright / False-Dark
  bool isPotGettingHigh = false;  // Digital potentiometer direction, false: Decrease , true: Increase 
  byte reservoirLevel = 4;
  char reservoirText[9]= "E[####]F";
  bool PlayOnSound = false; //Play on beep flag - website controls it
  bool PlayOffSound = false; //Play off beep flag - website controls it
  bool PlayEE = false; //Surprise :) - website controls it
  bool CalibrateLights = false; //Calibrate lights flag - website controls it
  int MaxLightReading = 0; // stores the highest light sensor analog reading
  int MinLightReading = 1023; //stores the lowest light sensor analog reading
  unsigned long AeroSprayTimer = millis();  //Aeroponics - Spary cycle timer - https://www.arduino.cc/reference/en/language/functions/time/millis/
  unsigned long AeroPumpTimer = millis();  //Aeroponics - Pump cycle timer
  bool isAeroSprayOn = false; //Aeroponics - Spray state, set to true to spay at power on
  bool isAeroPumpOn = false; //Aeroponics - High pressure pump state
  float AeroPressure = 0.0;  //Aeroponics - Current pressure (bar)
  float AeroPressurePSI = 0.0;  //Aeroponics - Current pressure (psi)
  char WebMessage[512];   //buffer for REST and MQTT API messages
  char CurrentTime[20]; //buffer for getting current time
  char LogMessage[LogLength]; //temp storage for assembling log messages
  char Logs[LogDepth][LogLength];  //two dimensional array for storing log histroy (array of char arrays)

//Component initialization
  const uint8_t  DHTType=DHT22;  //specify temp/humidity sensor type: DHT21(AM2301) or DHT22(AM2302,AM2321)
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
  Serial3.begin(115200);  //esp wifi console output
  wdt_enable(WDTO_8S); //Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 minutes assume a lockup and reset sketch
  addToLog(F("GrowBox initializing..."));
  loadSettings();

   //Pin setup, defining what Pins are inputs/outputs and setting initial output signals. Pins are defined in 420Pins.h tab
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
  digitalWrite(BuiltInLEDOutPin, LOW); //LED OFF,without this LED would be randomly on or off when the board is powered on
  digitalWrite(Relay1OutPin, HIGH); //default OFF (Uses negative logic - HIGH turns relay off, LOW on) 
  digitalWrite(Relay2OutPin, HIGH); 
  digitalWrite(Relay3OutPin, HIGH);
  digitalWrite(Relay4OutPin, HIGH);
  digitalWrite(Relay5OutPin, HIGH);
  digitalWrite(Relay6OutPin, HIGH);
  digitalWrite(Relay7OutPin, HIGH); 
  digitalWrite(Relay8OutPin, HIGH); 
    
  //Initialize web connections
  ESPLink.resetCb = resetWebServer;  //Callback subscription: When wifi reconnects, restart the WebServer
  resetWebServer();  //reset the WebServer
  ESPLink.Process();  //Process any command from ESP-Link
  setSyncProvider(getNtpTime); //points to method for updating time from NTP server
  setSyncInterval(3600); //Sync time every hour
  setupMqtt();  //MQTT message relay setup. Logs "ConnectedCB is XXXX" to serial if successful
 
  //Threading - Linking functions to threads and setting how often they should be called
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
  Screen.begin(); //start LCD screen
  Screen.setRotation(ScreenRotation);
  DigitDisplay.begin(); //start 4 digit LED display
  DigitDisplay.setBacklight(MySettings.DigitDisplayBacklight); //set 4 digit LED display backlight intensity
  PowerSensor.setAddress(PowerSensorIP); //start power meter, pass mandatory fake IP address
  LogToSerials(F("Calibrating lights..."),false);
  calibrateLights();
  addToLog(F("Grow Box initialized"));

  //triger all threads at startup
  fiveSecRun(); //needs to run first to get sensor readings
  oneSecRun();  
  minuteRun();
  halfHourRun();
  
  //Start interrupts to handle request from ESP-Link firmware
  Timer3.initialize(500);  //check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processEspLink);
  Timer3.start();

  sendEmailAlert(F("Grow%20box%20(re)started"));
}

void loop() {  // put your main code here, to run repeatedly:
  ThreadControl.run();    //loop only checks if it's time to trigger one of the threads
}

void processEspLink(){
  ESPLink.Process();  //Interrupt calls this every 0.5 sec and process any request coming from th ESP-Link
}

void oneSecRun(){
  if(debug)LogToSerials(F("One sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  checkLightStatus(); 
  checkAero();  
  checkRelays();
  checkSound();  
}

void fiveSecRun(){
  if(debug)LogToSerials(F("Five sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout   
  readSensors();
  wdt_reset(); //reset watchdog timeout
  updateDisplay(); //Updates 7 digit display  
  //LogToSerials(freeMemory(),true);  
}

void minuteRun(){
  if(debug)LogToSerials(F("Minute trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  checkLightTimer(); 
  LogToSerials(logToText(),true);  //Logs sensor readings to Serial  
  logToScreen();  //Display sensore readings on LCD screen  
}

void halfHourRun(){
  if(debug)LogToSerials(F("Half hour trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  if(MySettings.ReportToGoogleSheets) ReportToGoogleSheets(false);  //uploads readings to Google Sheets via ESP REST API
  if(MySettings.ReportToMqtt) mqttPublush(false);  //publish readings via ESP MQTT API
}

//Helper functions

void readSensors(){  //Bundles functions to get sensor readings  
  readDHTSensor();
  checkLightSensor();
  readPowerSensor();
  readPH();
  checkReservoir();
  readAeroPressure();
}

void saveSettings(bool AddToLog){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings));
  if(AddToLog) addToLog(F("Settings saved to EEPROM"));
}

void loadSettings(){
  Settings EEPROMSettings; //temporary storage with "settings" type
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));  
  if(EEPROMSettings.StructureVersion != MySettings.StructureVersion){
    LogToSerials(F("Change detected, updating EEPROM..."),false);
    saveSettings(false);  //overwrites stored settings with defaults from this sketch
  }
  else {
    LogToSerials(F("Same structure version detected, applying restored settings..."),false);
    MySettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
  }
  LogToSerials(F("done"),true);
}

time_t getNtpTime(){
  long LastRefresh = millis();
  time_t NTPResponse = 0;
  LogToSerials(F("Waiting for NTP time (30sec timeout)..."),false);
  while(NTPResponse == 0 && millis() - LastRefresh < 30000){
   NTPResponse = EspCmd.GetTime();
   delay(50);
   wdt_reset(); //reset watchdog timeout
  }
  if(NTPResponse == 0) {
    addToLog(F("NTP time sync failed"));
    sendEmailAlert(F("NTP%20time%20sync%20failed")); 
  }
  else LogToSerials(F("NTP time synchronized"),true);
  return NTPResponse;
}

char * getFormattedTime(){
  time_t Now = now(); // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  return CurrentTime;
}
