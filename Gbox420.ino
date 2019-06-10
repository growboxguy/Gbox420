//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for grow box monitoring and controlling

//HELLO, You are probably here looking for the following tabs:
// 420Pins.h : Modify the Pin assignment
// 420Settings.h : First time setup or changing the default settings

//TODO: 
//Flow meter: Use LastPulseCount in determining priming is complete
//EC meter
//Reservoir Refill button
//MQTT reporting memory overflow?

//Libraries
  #include "420Pins.h" //Load pins layout file
  #include "420Settings.h" //Load settings file
  #include "avr/wdt.h" //Watchdog timer
  #include "avr/boot.h" //Watchdog timer related bug fix
  #include "Thread.h" //Splitting functions to threads for timing
  #include "StaticThreadController.h"  //Grouping threads
  #include "TimerThree.h"  //Interrupt handling for webpage
  #include "DHT.h"  //DHT11 or DHT22 Digital Humidity and Temperature sensor
  #include "OneWire.h" //DS18B20 waterproof temperature sensor
  #include "DallasTemperature.h" //DS18B20 waterproof temperature sensor
  #include "SevenSegmentTM1637.h" //4 digit LED display
  #include "SevenSegmentExtended.h" //4 digit LED display
  #include "PZEM004T.h" //Power meter
  #include "ELClient.h" //ESP-link
  #include "ELClientRest.h" //ESP-link - REST API
  #include "ELClientWebServer.h" //ESP-link - WebServer API
  #include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
  #include "ELClientMqtt.h" //ESP-link - MQTT protocol for sending and receiving IoT messages
  #include "TimeLib.h" //Keeping track of time
  #include "SPI.h" //TFT Screen - communication
  #include "Adafruit_GFX.h" //TFT Screen - generic graphics driver
  #include "Adafruit_ILI9341.h" //TFT Screen - hardware specific driver
  #include "util/atomic.h" //Loading the ATOMIC_BLOCK macro, helps blocking interoups while commands in the block are running
  //#include "MemoryFree.h" //checking remaining memory - only for debugging

//Global variables
  bool LightOK = true; //Track component health, at startup assume every component is OK
  bool PressureOK = true;  //Aeroponics - Pressure within range
  bool PumpOK = true; //Aeroponics - High pressure pump health
  bool ACPowerOK = true; //AC voltage present
  bool DCPowerOK = true; //DC voltage present
  bool VentOK = true; //Temperatur and humidity withing range
  bool ReservOK = true; //Reservoir not empty
  bool PhOK = true;  //Nutrient solution PH within range
  int LightsTriggerCount = 0;  //Counters of out of range readings before triggering an alert
  int PressureTriggerCount = 0; //All counters are compared agains ReadCountBeforeAlert from 420Settings before triggering an alert
  int ACPowerTriggerCount = 0;
  int DCPowerTriggerCount = 0;
  int VentilationTriggerCount = 0;
  int ReservoirTriggerCount = 0;
  int PHTriggerCount = 0;
  float InternalTemp; //Internal Temperature - Celsius
  float InternalHumidity; //Internal relative humidity - %
  float ExternalTemp; //External Temperature - Celsius
  float ExternalHumidity; //External relative humidity - %
  float Power; //Power sensor - W
  float Energy; //Power sensor - Wh Total consumption 
  float Voltage; //Power sensor - V
  float Current; //Power sensor - A
  float PHRaw; //PH meter reading 
  float PH; //Calculated PH  
  int LightReading;  //light sensor analog reading
  bool Bright;  //Ligth sensor digital feedback: True-Bright / False-Dark
  byte ReservoirLevel = 4;
  char ReservoirText[9]= "E[####]F";
  float ReservoirTemp; //Reservoir water temperature
  bool PlayOnSound = false; //Play on beep flag - website controls it
  bool PlayOffSound = false; //Play off beep flag - website controls it
  bool PlayEE = false; //Surprise :) - website controls it
  bool CalibrateLights = false; //Calibrate lights flag - website controls it
  int MaxLightReading = 0; // stores the highest light sensor analog reading
  int MinLightReading = 1023; //stores the lowest light sensor analog reading
  uint32_t AeroSprayTimer = millis();  //Aeroponics - Spary cycle timer - https://www.arduino.cc/reference/en/language/functions/time/millis/
  uint32_t AeroPumpTimer = millis();  //Aeroponics - Pump cycle timer
  uint32_t FlowMeterTimer = millis();  //Flow meter timer
  unsigned int LastPulseCount = 0; //stores last pulse/sec value
  bool AeroSpraySolenoidOn = false; //Aeroponics - Controls the spray valve, set to true to spay at power on. Only used with the Pressure Tank option.
  bool AeroBypassSolenoidOn = false; //Aeroponics - Controls the bypass valve, true opens the solenoid
  bool AeroPumpOn = false; //Aeroponics - High pressure pump state
  bool AeroPumpKeepOn = false; //Aeroponics - Used to temporary suspend pump timers and keep the high pressure pump on. Do not change.
  bool AeroBlowOffInProgress = false; //Aeroponics - True while bypass valve is open during a pressure blow-off. Only used without the Pressure Tank option.
  float AeroPressure = 0.0;  //Aeroponics - Current pressure (bar)
  char WebMessage[512];   //buffer for REST and MQTT API messages
  char CurrentTime[20]; //buffer for storing current time
  char LogMessage[MaxTextLength]; //temp storage for assembling log messages
  char Logs[LogDepth][MaxTextLength];  //two dimensional array for storing log histroy (array of char arrays)

//Component initialization
  const uint8_t  DHTType=DHT22;  //specify temp/humidity sensor type: DHT21(AM2301) or DHT22(AM2302,AM2321)
  DHT InternalDHTSensor(InternalDHTSensorInPin, DHTType); //Internal Temp/Humidity sensor (DHT22)
  DHT ExternalDHTSensor(ExternalDHTSensorInPin, DHTType); //External Temp/Humidity sensor (DHT22)
  OneWire ReservoirTempSensorWire(ReservoirTempSensorInPin); //Reservoir waterproof temperature sensor (DS18B20)
  DallasTemperature ReservoirTempSensor(&ReservoirTempSensorWire); //Reservoir waterproof temperature sensor (DS18B20)
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
  boot_rww_enable(); //fix watchdog not loading sketch after a reset error on Mega2560  
  addToLog(F("GrowBox initializing..."));
  loadSettings();

   //Pin setup, defining what Pins are inputs/outputs and setting initial output signals. Pins are defined in 420Pins.h tab
  pinMode(ATXPowerGoodInPin, INPUT_PULLUP);
  pinMode(ATXPowerONOutPin, OUTPUT);
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
  pinMode(DimmingOutPin, OUTPUT);
  pinMode(ScreenReset, OUTPUT);
  pinMode(ScreenDC, OUTPUT);
  pinMode(ScreenCS, OUTPUT);
  pinMode(ScreenMOSI, OUTPUT);
  pinMode(ScreenMISO, INPUT);
  pinMode(ScreenSCK, OUTPUT);
  digitalWrite(BuiltInLEDOutPin, LOW); //LED OFF,without this LED would be randomly on or off when the board is powered on
  digitalWrite(ATXPowerONOutPin, LOW);  //Avoids random ON/OFF state at startup
  digitalWrite(Relay1OutPin, HIGH); //default OFF (negative logic - HIGH turns relay off, LOW on) 
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
  setTime(getNtpTime());
  //setSyncProvider((time_t)getNtpTime); //points to method for updating time from NTP server
  //setSyncInterval(3600); //Sync time every hour
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
  InternalDHTSensor.begin(); //start humidity/temp sensor
  ExternalDHTSensor.begin(); //start external humidity/temp sensor
  ReservoirTempSensor.begin();
  Screen.begin(); //start LCD screen
  Screen.setRotation(ScreenRotation);
  DigitDisplay.begin(); //start 4 digit LED display
  DigitDisplay.setBacklight(MySettings.DigitDisplayBacklight); //set 4 digit LED display backlight intensity
  PowerSensor.setAddress(PowerSensorIP); //start power meter, pass mandatory fake IP address
  logToSerials(F("Calibrating lights..."),false);
  calibrateLights();
  addToLog(F("Grow Box initialized"));

  //triger all threads at startup
  fiveSecRun(); //needs to run first to get sensor readings
  oneSecRun();  
  minuteRun();
  halfHourRun();
  
  //Start interrupts to handle request from ESP-Link firmware
  Timer3.initialize(500);  //check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();

  sendEmailAlert(F("Grow%20box%20(re)started"));
}

void loop() {  // put your main code here, to run repeatedly:
  ThreadControl.run();    //loop only checks if it's time to trigger one of the threads
}

void processTimeCriticalStuff(){
  ESPLink.Process();  //Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage
  if(AeroSpraySolenoidOn || AeroBypassSolenoidOn || AeroPumpOn) checkAero(true);  //when the aeroponics is doing a time critical task (Priming, spraying)
}

void oneSecRun(){
  if(MySettings.DebugEnabled)logToSerials(F("One sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  checkAero(false);  
  checkLightStatus();
  checkRelays();
  checkSound();  
}

void fiveSecRun(){
  if(MySettings.DebugEnabled)logToSerials(F("Five sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout   
  readSensors();
  wdt_reset(); //reset watchdog timeout
  updateDisplay(); //Updates 7 digit display  
  //logToSerials(freeMemory(),true);  
}

void minuteRun(){
  if(MySettings.DebugEnabled)logToSerials(F("Minute trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  checkLightTimer(); 
  logToSerials(logToText(),true);  //Logs sensor readings to Serial  
  logToScreen();  //Display sensor readings on LCD screen  
}

void halfHourRun(){
  if(MySettings.DebugEnabled)logToSerials(F("Half hour trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  if(MySettings.ReportToGoogleSheets) ReportToGoogleSheets(false);  //uploads readings to Google Sheets via ESP REST API
  if(MySettings.ReportToMqtt) mqttPublush(false);  //publish readings via ESP MQTT API
}

//Helper functions

void readSensors(){  //Bundles functions to get sensor readings  
  readAeroPressure();
  readDHTSensor();
  checkLightSensor();
  readPowerSensor();
  readATXPowerGood();
  readReservoirPH(false);
  checkReservoir();  
}

void saveSettings(bool AddToLog){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings)); //update_block only writes the bytes that changed
  if(AddToLog) addToLog(F("Settings saved to EEPROM"));
}

void loadSettings(){
  Settings EEPROMSettings; //temporary storage with "settings" type
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));  
  if(EEPROMSettings.StructureVersion != MySettings.StructureVersion){
    logToSerials(F("Change detected, updating EEPROM..."),false);
    saveSettings(false);  //overwrites stored settings with defaults from this sketch
  }
  else {
    logToSerials(F("Same structure version detected, applying restored settings..."),false);
    MySettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
  }
  logToSerials(F("done"),true);
}

bool SyncInProgress=false;
time_t getNtpTime(){
  time_t NTPResponse = 0;
  if(!SyncInProgress){ //blocking calling the sync again in an interrupt
    SyncInProgress = true;
    uint32_t LastRefresh = millis();  
    logToSerials(F("Waiting for NTP time (30sec timeout)..."),false);  
    while(NTPResponse == 0 && millis() - LastRefresh < 30000){
     NTPResponse = EspCmd.GetTime();
     delay(500);
     wdt_reset(); //reset watchdog timeout
    }
    SyncInProgress = false;
    if(NTPResponse == 0) {
      addToLog(F("NTP time sync failed"));
      sendEmailAlert(F("NTP%20time%20sync%20failed")); 
    }
    else logToSerials(F("NTP time synchronized"),true);
    }
  return NTPResponse;
}

char * getFormattedTime(){
  time_t Now = now(); // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  return CurrentTime;
}

void setMetricSystemEnabled(bool MetricEnabled){
  if(MetricEnabled != MySettings.MetricSystemEnabled){  //if there was a change
    MySettings.MetricSystemEnabled = MetricEnabled;
    MySettings.InternalFanSwitchTemp = convertBetweenTempUnits(MySettings.InternalFanSwitchTemp);
    MySettings.TempAlertLow= convertBetweenTempUnits(MySettings.TempAlertLow);
    MySettings.TempAlertHigh= convertBetweenTempUnits(MySettings.TempAlertHigh);
    MySettings.AeroPressureLow=convertBetweenPressureUnits(MySettings.AeroPressureLow);
    MySettings.AeroPressureHigh=convertBetweenPressureUnits(MySettings.AeroPressureHigh);
    MySettings.PressureAlertLow=convertBetweenPressureUnits(MySettings.PressureAlertLow);
    MySettings.PressureAlertHigh=convertBetweenPressureUnits(MySettings.PressureAlertHigh);
  }    
  if(MySettings.MetricSystemEnabled) addToLog(F("Using Metric units"));
  else addToLog(F("Using Imperial units"));  
}

float convertBetweenTempUnits(float Value){
if(MySettings.MetricSystemEnabled) return round((Value-32) * 55.555555)/100.0;
else return round(Value*180 + 3200.0)/100.0;
}

float convertBetweenPressureUnits(float Value){
if(MySettings.MetricSystemEnabled) return round(Value / 0.145038)/100.0;
else return round(Value*1450.38)/100.0;
}
