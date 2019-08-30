//Libraries
#include "Thread.h"  //Splitting functions to threads
#include "StaticThreadController.h"  //Grouping threads
#include "TimerThree.h"  //Timing Threads
#include "DHT.h"  //DHT11 or DHT22 sensor
#include "DHT.h"  //DHT11 or DHT22 sensor
#include "SevenSegmentTM1637.h" // 4 digit LED display
#include "SevenSegmentExtended.h" // 4 digit LED display
#include "PZEM004T.h"  //PowerMeter
#include "ELClient.h"  // ESP-link
#include "ELClientRest.h" // ESP-link - REST API
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "DS1302.h" //Real time clock
#include "SPI.h" //TFT Screen - communication
#include "Adafruit_ILI9341.h" //TFT Screen - hardware specific driver
#include "Adafruit_GFX.h" //TFT Screen - generic graphics driver
#include "SD.h" //SD Card

//Pins
const int BuzzerOutPin = 3; //PC speaker 
const int SDCardCSOutPin = 53; //SD Card - CS port
const int BuiltInLEDOutPin = 13;  // Digital output - Built-in LED light for testing
const int PowerLEDOutPin = 46;  // Digital output - PC case Power LED
const int HddLEDOutPin = 47; // Digital output - PC case HDD LED 
const int LightSensorInPin = 36; // D0 digital input - LM393 light sensor                                                                                                              
const int LightSensorAnalogInPin = A0; // D0 digital input - LM393 light sensor
const int WaterCriticalInPin = A4; // Analog input - Water sensor1
const int WaterLowInPin = A5; // Analog input - Water sensor2
const int WaterMediumInPin = A6; // Analog input - Water sensor3
const int WaterFullInPin = A7; // Analog input - Water sensor4
const int DigitDisplayCLKOutPin = 31; // CLK digital output - 4 digit LED display
const int DigitDisplayDI0OutPin = 30; // DIO digital output - 4 digit LED display
const int DHTInPin = 43; // DAT digital input - DHT22 temp/humidity sensor
const int RtcRSTPin   = 5;  // Reset -  Real time clock
const int RtcDATPin   = 6;  // Input/Output -  Real time clock
const int RtcCLKPin = 7;  // Serial Clock -  Real time clock
const int Relay1OutPin = 21;  //Port 1 - Power relay
const int Relay2OutPin = 22;  //Port 2 - Power relay
const int Relay3OutPin = 23;  //Port 3 - Power relay - PC power supply
const int Relay4OutPin = 24;  //Port 4 - Power relay
const int Relay5OutPin = 25;  //Port 5 - Power relay
const int Relay6OutPin = 26;  //Port 6 - Power relay
const int Relay7OutPin = 27;  //Port 7 - Power relay
const int Relay8OutPin = 28;  //Port 8 - Power relay - LED lights
const int PowerButtonInPin = 29;  //Power button
const int PHInPin = A15;  //Po analog port - PH metter
const int AMoistureSensorPin = A8; //A0 analog port - Soil moisture sensor 
const int DMoistureSensorPin = 45; //D0 digital port - Soil moisture sensor
const int PotCSOutPin = 34; //Digital output - X9C104 digital potentiometer CS 
const int PotUDOutPin = 32;  // Digital output - X9C104 digital potentiometer UD
const int PotINCOutPin = 33;  // Digital output - X9C104 digital potentiometer INC
const int ScreenReset = 37; //Screen Screen - RESET(3.3V)
const int ScreenSCK = 38; //Screen Screen - SCK(3.3V)
const int ScreenMOSI = 39; //Screen Screen - SDO/MOSI(3.3V)
const int ScreenMISO = 40; //Screen Screen - SDI/MISO(3.3V), not needed can be -1
const int ScreenCS = 41; //Screen Screen - CS(3.3V)
const int ScreenDC = 42; //Screen Screen - DC/RS(3.3V)

//Global constants
const uint8_t  DHTType=DHT22;  //specify temp/humidity sensor type
const int ScreenBacklight = 100; //  0-254 backlight strenght
const short LogDepth = 10;  //Show last 6 entries on website
const short LogLength = 31;  //30 characters + null terminator

//Global variables
float Humidity; // Stores relative humidity - %
float BoxTempC; // Stores Temperature - Celsius
float BoxTempF; // Stores Temperature - Fahrenheit
float PH; //PH meter
float Moisture; //Soil moisture sensor - %
float Wattage; //Power sensor - W
float Energy; //Power sensor - Wh
float Voltage; //Power sensor - V
float Current; //Power sensor - A
bool isBright;  //Ligth sensor feedback: True-Bright / False-Dark
bool isLightOn = true;  //Startup status for lights: True-ON / False-OFF
bool isTimerEnabled = false;  //Enable timer controlling lights
bool isPCPowerSupplyOn = false;  //Startup status for PC power supply: True-ON / False-OFF
bool PotGettingHigh = false;  // Digital potentiometer direction, false: Decrease , true: Increase 
bool WaterAboveCritical = false; //Analog input - Water sensor
bool WaterAboveLow = false; //Analog input - Water sensor
bool WaterAboveMedium = false; //Analog input - Water sensor
bool WaterAboveFull = false; //Analog input - Water sensor
int PotSteppingCount = 100;  // Digital potentiometer step count
int Brightness; // 0 - 100 range for controlling led driver output
int LightReading;  //light sensor analog reading
int LightReadingPercent;  //Compared to calibrated min/max values what is the detected light level %
int MaxLightReading = 0; // 
int MinLightReading = 1023; //
char CurrentDate[11];  //time gets read into this
char CurrentTime[11];  //time gets read into this
short LightOnHour = 4;  //Light ON time - hour
short LightOnMinute = 20; //Light ON time - minute
short LightOffHour = 16; //Light OFF time - hour
short LightOffMinute = 20; //Light OFF time - minute
char Logs[LogDepth][LogLength];  //two dimensional array for storing log histroy (array of char arrays)
char LogMessage[LogLength]; //temp storage for assemling log messages

//Initialize devices
DS1302 rtc(RtcRSTPin, RtcDATPin, RtcCLKPin); //Real time clock
DHT dht(DHTInPin, DHTType); // DHT sensor
SevenSegmentExtended display(DigitDisplayCLKOutPin, DigitDisplayDI0OutPin); //4 digit LED Display
PZEM004T PowerSensor(&Serial2);  // Power Sensor using hardware Serial 2, use software serial if needed
IPAddress PowerSensorIP(192,168,1,1); // Power Sensor address
ELClient esp(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientRest rest(&esp);    // ESP-link REST API
ELClientWebServer webServer(&esp); //ESP-link WebServer API
Adafruit_ILI9341 Screen = Adafruit_ILI9341(ScreenCS, ScreenDC, ScreenMOSI, ScreenSCK, ScreenReset, ScreenMISO);

//Threading to ensure web interface resons to clicks
  Thread OneSecThread = Thread();
  Thread FiveSecThread = Thread();
  Thread MinuteThread = Thread();
  Thread HalfHourThread = Thread();
  StaticThreadController<4> ThreadControl (&OneSecThread,&FiveSecThread,&MinuteThread,&HalfHourThread);

void setup() {     // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  addToLog("GrowBox initializing...");

  //Load conf.txt from SD card
  pinMode(SDCardCSOutPin, OUTPUT);
  unsigned long ResponseTimer = millis();  //store the number of milliseconds since the program started, https://www.arduino.cc/reference/en/language/functions/time/millis/
  while (!SD.begin(SDCardCSOutPin)) {
    Serial.print(".");
    delay(500);
    if(millis() - ResponseTimer >= 25000){
      addToLog("Error: SD card timeout");
      break; }
    }
  ReadFromSD();

  //Pin setup, defining what Pins are inputs/outputs and setting initial output signals
  pinMode(LightSensorInPin, INPUT);
  pinMode(WaterCriticalInPin, INPUT_PULLUP);
  pinMode(WaterLowInPin, INPUT_PULLUP);
  pinMode(WaterMediumInPin, INPUT_PULLUP);
  pinMode(WaterFullInPin, INPUT_PULLUP);
  pinMode(PowerButtonInPin,  INPUT_PULLUP);
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
  pinMode(HddLEDOutPin, OUTPUT);
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
  if(isLightOn) digitalWrite(Relay8OutPin, LOW); else digitalWrite(Relay8OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay)
  if(isPCPowerSupplyOn) digitalWrite(Relay3OutPin, LOW); else digitalWrite(Relay3OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay)
    
  //Initialize web connections
  esp.resetCb = ResetWebServer;  //Callback subscription: When wifi is reset, restart the webServer
  ResetWebServer();

  //Threading
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(OneSecRun);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(FiveSecRun);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(MinuteRun);
  HalfHourThread.setInterval(1800000);
  HalfHourThread.onRun(HalfHourRun);

  //Start devices
  dht.begin(); //start humidity/temp sensor
  Screen.begin(); //start screen
  Screen.setRotation(3);
  display.begin(); //start 4 digit LED display
  display.setBacklight(ScreenBacklight); //set 4 digit LED display backlight intensity
  PowerSensor.setAddress(PowerSensorIP); //start power meter
  calibrateLights();
  addToLog("Grow Box initialized");

 //triger al threads at startup
  OneSecRun();
  FiveSecRun();
  MinuteRun();
  HalfHourRun();

  //Start interrupts 
  Timer3.initialize(500);
  Timer3.attachInterrupt(ProcessWebsite);
  Timer3.start();
}

void loop() {  // put your main code here, to run repeatedly:
    ThreadControl.run();    
    //while(Serial3.available()) Serial.println(Serial3.readString() ); //Read output of ESP Serial - For setup/debugging, comment the line out once wifi setup is complete
}

void ProcessWebsite(){
  esp.Process();
}

void OneSecRun(){
   if(!digitalRead(PowerButtonInPin))isLightOn = !isLightOn;  //If the power button is held in at the time of the measure invert the light status
   TurnLightOnOff();   
}

void FiveSecRun(){
   if(isBright)digitalWrite(HddLEDOutPin, HIGH); else digitalWrite(HddLEDOutPin, LOW);  //Turn on HDD Led on PC case if light is detected
   if(isPCPowerSupplyOn) digitalWrite(Relay3OutPin, LOW); else digitalWrite(Relay3OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay)
   updateDisplay(); //Updates 8 digit display
   readSensors();
}

void MinuteRun(){ 
    updateTime();
    CheckLightStatus();
    logToScreen();  //Display sensore readings on screen
    logToSerial();  //Logs sensor readings to Serial
}

void HalfHourRun(){
    reportToGoogleSheets();  //uploads results to Google Sheets via the ESP REST API
}

void updateTime() {  //fills the CurrentTime global variable
  Time Now = rtc.time();  // Get the current time and date from the chip.
  snprintf(CurrentDate, 11, "%04d-%02d-%02d",Now.yr, Now.mon, Now.date);  //Format and store time
  snprintf(CurrentTime, 11, "%02d:%02d:%02d",Now.hr, Now.min, Now.sec);  //Format and store time
}





