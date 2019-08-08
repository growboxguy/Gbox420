#include "Common.h" 
#include "avr/wdt.h" //Watchdog timer
#include "avr/boot.h" //Watchdog timer related bug fix
#include "MemoryFree.h" //checking remaining memory - only for debugging
#include "TimerThree.h"  //Interrupt handling for webpage
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "Thread.h" //Splitting functions to threads for timing
#include "StaticThreadController.h"  //Grouping threads
#include "DHTSensor.h"
#include "Lights.h"

//TODO:
//addToLog - make Log class
//Convert Website to a Singleton class (Restricts the instantiation to one single instance) 
//Make alerting a class, every object will have a "bool StatusOK" variable inherited from Common, Alerting will go throught all subscribed classes and check status
//Subscribe to thread timer events using the attach method.
//sendEmailAlert implementation
//Sounds implementation
//Light sensor to separate object from Light

//Component initialization
ELClient MyESPLink(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientWebServer WebServer(&MyESPLink); //ESP-link WebServer API
DHTSensor * InternalDHTSensor;  //Pointer to a Digital Humidity Sensor measuring the internal temperature of the grow box
DHTSensor * ExternalDHTSensor; //Pointer to a Digital Humidity Sensor measuring the external temperature of the grow box
Lights * Light1;  //Pointer to a Digital Humidity Sensor measuring the external temperature of the grow box
//Lights * Light2;  //You can control multiple lights, just add its defaults to 420Settings.h

 
//Threading to time tasks
  Thread OneSecThread = Thread();
  Thread FiveSecThread = Thread();
  Thread MinuteThread = Thread();
  Thread HalfHourThread = Thread();
  StaticThreadController<4> ThreadControl (&OneSecThread,&FiveSecThread,&MinuteThread,&HalfHourThread);

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //esp wifi console output
  wdt_enable(WDTO_8S); //Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 minutes assume a lockup and reset sketch
  boot_rww_enable(); //fix watchdog not loading sketch after a reset error on Mega2560  
  Common::logToSerials(F("GrowBox initializing..."),true);
  Common::addToLog(F("GrowBox initializing..."));
  Common::loadSettings();

  InternalDHTSensor = new DHTSensor(Common::MySettings.InternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  ExternalDHTSensor = new DHTSensor(Common::MySettings.ExternalDHTSensorPin,DHT22);  //passing: Pin and Sensor type: DHT22 or DHT11)
  Light1 = new Lights(Common::MySettings.Light1RelayPin,Common::MySettings.Light1DimmingPin,&Common::MySettings.Light1DimmingLimit,&Common::MySettings.Light1Status,&Common::MySettings.Light1Brightness,&Common::MySettings.Light1TimerEnabled,&Common::MySettings.Light1OnHour,&Common::MySettings.Light1OnMinute,&Common::MySettings.Light1OffHour,&Common::MySettings.Light1OffMinute);   //Passing MySettings members as references: Changes get written back to MySettings and saved to EEPROM. (byte *)(((byte *)&Common::MySettings) + offsetof(Settings, LightOnHour))
  
  MyESPLink.resetCb = &resetWebServer;  //Callback subscription: When wifi reconnects, restart the WebServer
  resetWebServer();  //reset the WebServer  
  
  //Threading - Setting up threads and setting how often they should be called
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(oneSecRun);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(fiveSecRun);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(minuteRun);
  HalfHourThread.setInterval(1800000);
  HalfHourThread.onRun(halfHourRun);
  
  Common::logToSerials(F("Grow Box initialized"),true);
  Common::addToLog(F("Grow Box initialized"));

   //triger all threads at startup
  fiveSecRun(); //needs to run first to get sensor readings
  oneSecRun();  
  minuteRun();
  halfHourRun();

   //Start interrupts to handle request from ESP-Link firmware
  Timer3.initialize(500);  //check every 0.5sec, using a larger interval can cause web requests to time out
  Timer3.attachInterrupt(processTimeCriticalStuff);
  Timer3.start();

  //  sendEmailAlert(F("Grow%20box%20(re)started"));
}

void loop() { // put your main code here, to run repeatedly:
 ThreadControl.run();    //loop only checks if it's time to trigger one of the threads
}

void processTimeCriticalStuff(){
  MyESPLink.Process();  //Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage  
}

void oneSecRun(){
  if(Common::MySettings.DebugEnabled)Common::logToSerials(F("One sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  
}

void fiveSecRun(){
  if(Common::MySettings.DebugEnabled)Common::logToSerials(F("Five sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout   
  Common::logToSerials(F("Free memory(bytes): "),false); Common::logToSerials(freeMemory(),true); 
  InternalDHTSensor -> refresh();
  ExternalDHTSensor -> refresh();
  Light1 -> refresh(); 
}

void minuteRun(){
  if(Common::MySettings.DebugEnabled)Common::logToSerials(F("Minute trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  
}

void halfHourRun(){
  if(Common::MySettings.DebugEnabled)Common::logToSerials(F("Half hour trigger.."),true);
  wdt_reset(); //reset watchdog timeout
}
