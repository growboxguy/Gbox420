//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for grow box monitoring and controlling
//UNDER DEVELOPEMENT - This is the next generation of Gbox420 rewritten to depend fully on the web interface for control and feedback. 


//HELLO, You are probably here looking for the following tab:
// 420Settings.h : First time setup or changing the default settings and Modify the Pin assignment

//TODO:
//addToLog - make Log class
//Convert Website to a Singleton class (Restricts the instantiation to one single instance) 
//Make alerting a class, every object will have a "bool StatusOK" variable inherited from Common, Alerting will go throught all subscribed classes and check status
//Subscribe to thread timer events using the attach method.
//sendEmailAlert implementation
//Sounds implementation
//Light sensor to separate object from Light
//Add buttons to trigger runMinute,runHour for debugging
//HempyBucket controls
//Aeroponics
//Light sensor
//PH sensor
//Water temp sensor
//Water level sensor

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "avr/wdt.h" //Watchdog timer
#include "avr/boot.h" //Watchdog timer related bug fix
#include "TimerThree.h"  //Interrupt handling for webpage
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "Thread.h" //Splitting functions to threads for timing
#include "StaticThreadController.h"  //Grouping threads
#include "420Settings.h"  //for storing/reading defaults
#include "420Helpers.h"  //global functions
#include "GrowBox.h" //Represents a complete box with lights,temp/humidity/ph/light sensors,power meter, etc..

//Global variables
Settings MySettings;
char Message[512];   //temp storage for assembling log messages, buffer for REST and MQTT API messages
char CurrentTime[20]; //buffer for storing current time

//Component initialization
ELClient ESPLink(&Serial3);  //ESP-link. Both SLIP and debug messages are sent to ESP over Serial3
ELClientWebServer WebServer(&ESPLink); //ESP-link WebServer API
ELClientCmd ESPCmd(&ESPLink);//ESP-link - Get current time from the internet using NTP
GrowBox * GBox;

//Threading to time tasks
  Thread OneSecThread = Thread();
  Thread FiveSecThread = Thread();
  Thread MinuteThread = Thread();
  Thread HalfHourThread = Thread();
  StaticThreadController<4> ThreadControl (&OneSecThread,&FiveSecThread,&MinuteThread,&HalfHourThread);

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //esp wifi console output
  logToSerials(F("GrowBox initializing..."),true);
  wdt_enable(WDTO_8S); //Watchdog timeout set to 8 seconds, if watchdog is not reset every 8 seconds assume a lockup and reset sketch
  boot_rww_enable(); //fix watchdog not loading sketch after a reset error on Mega2560  
  loadSettings();
  GBox = new GrowBox(&MySettings);
  GBox -> refresh();
  
  ESPLink.resetCb = &resetWebServer;  //Callback subscription: When wifi reconnects, restart the WebServer
  resetWebServer();  //reset the WebServer 
  setTime(getNtpTime()); 
  
  //Threading - Setting up threads and setting how often they should be called
  OneSecThread.setInterval(1000);
  OneSecThread.onRun(runSec);
  FiveSecThread.setInterval(5000);
  FiveSecThread.onRun(runFiveSec);
  MinuteThread.setInterval(60000);
  MinuteThread.onRun(runMinute);
  HalfHourThread.setInterval(1800000);
  HalfHourThread.onRun(runHalfHour);
  
  logToSerials(F("Grow Box initialized"),true);

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
  ESPLink.Process();  //Interrupt calls this every 0.5 sec to process any request coming from the ESP-Link hosted webpage  
}

void runSec(){
  if(MySettings.DebugEnabled)logToSerials(F("One sec trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  GBox -> runSec();
}

void runFiveSec(){
  if(MySettings.DebugEnabled){
    logToSerials(F("Five sec trigger.."),true);
    getFreeMemory();
  }
  wdt_reset(); //reset watchdog timeout   
  GBox -> runFiveSec();
}

void runMinute(){
  if(MySettings.DebugEnabled)logToSerials(F("Minute trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  GBox -> runMinute();
}

void runHalfHour(){
  if(MySettings.DebugEnabled)logToSerials(F("Half hour trigger.."),true);
  wdt_reset(); //reset watchdog timeout
  GBox -> runHalfHour();
}
