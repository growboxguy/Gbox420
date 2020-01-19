#pragma once

//This is a collection of common functions that all classes (GrowBox, PH reader, Lights, DHTSensor..) can use
//Needs to be included in every module's header (.h) file:  #include "420Common.h"

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "MemoryFree.h" //checking remaining memory - only for debugging
#include "avr/wdt.h" //Watchdog timer
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "420Settings.h"  //for storing/reading defaults
#include "420RollingAverage.h"  //for smoothing sensor readings, average of last readings

//Forward declaration
class GrowBox;
extern ELClientCmd ESPCmd;
extern HardwareSerial& ArduinoSerial;
extern HardwareSerial& ESPSerial;
extern GrowBox * GBox;
extern char LongMessage[1024];
extern char ShortMessage[128];
extern char CurrentTime[20];

//Class specific variables
time_t getNtpTime();
void getFreeMemory();
char * getFormattedTime(bool PrintToSerials);  
float convertBetweenTempUnits(float);
float convertBetweenPressureUnits(float); 
char * toText(const __FlashStringHelper *);    
char * toText(int);
char * toText(long); 
char * toText(float);  //function overloading: Same named function, different parameter type 
char * toText(int Number1, int Number2,const char * Separator); 
char * toText(float Number1, float Number2,const char * Separator); 
char * toPrecisionText(float Number);  
char * timeToText(byte Hour, byte Minute);
char * tempToText(float Temp);
char * pressureToText(float Pressure);
char * percentageToText(float Humidity);    
char * onOffToText(bool Status);  
char * yesNoToText(bool Status); 
char * statusToText(bool Status);
char * enabledToText(bool Status);

//////////////////////////////////////////////////////////////////
//Logging to Arduino Serial and ESP-link's Microcontroller Console (uC Console)

void logToSerials (const __FlashStringHelper* ToPrint,bool BreakLine=true,byte Indent=3);  //logs to both Arduino and ESP Link serial console, 2 optional parameters to adding a break line at after printing and the indentation in front
template <class logLine> void logToSerials (logLine* ToPrint,bool BreakLine=true,byte Indent=3) { 
  while(Indent>0){
     ArduinoSerial.print(F(" "));
     ESPSerial.print(F(" "));
     Indent--;
  }
  if(BreakLine){ArduinoSerial.println((*ToPrint));ESPSerial.println((*ToPrint));}
  else{ArduinoSerial.print((*ToPrint));ESPSerial.print((*ToPrint));}
}

template <class logLine> void logToSerials (logLine& ToPrint,bool BreakLine=true,byte Indent=3) { 
  while(Indent>0){
     ArduinoSerial.print(F(" "));
     ESPSerial.print(F(" "));
     Indent--;
  }
  if(BreakLine){ArduinoSerial.println(ToPrint);ESPSerial.println(ToPrint);}
  else{ArduinoSerial.print(ToPrint);ESPSerial.print(ToPrint);}
}