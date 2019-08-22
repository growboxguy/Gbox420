#pragma once

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "MemoryFree.h" //checking remaining memory - only for debugging
#include "avr/wdt.h" //Watchdog timer
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "420Settings.h"  //for storing/reading defaults

class GrowBox;

extern HardwareSerial& ArduinoSerial;
extern HardwareSerial& ESPSerial;
extern GrowBox * GBox;
extern char CurrentTime[20];
extern char Message[512];
extern ELClientCmd ESPCmd;

time_t getNtpTime();
void getFreeMemory();
char * getFormattedTime();  
float convertBetweenTempUnits(float);
float convertBetweenPressureUnits(float);    
char * toText(int); 
char * toText(long); 
char * toText(float);  //function overloading: Same named function, different parameter type 
char * toText(int Number1, int Number2,const char * Separator); 
char * toText(float Number1, float Number2,const char * Separator); 
char * toPrecisionText(float Number);  
char * timeToText(byte Hour, byte Minute);
char * tempToText(float Temp);
char * percentageToText(float Humidity);    
const __FlashStringHelper * onOffToText(bool Status);  
const __FlashStringHelper * yesNoToText(bool Status); 
const __FlashStringHelper * statusToText(bool Status);
const __FlashStringHelper * enabledToText(bool Status);

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



//////////////////////////////////////////////////////////////////
//RollingAverage class: For smoothing sensor readyings

class RollingAverage
{  
  private:
    long Sum = 0;
    int History[RollingAverageQueueDepth] = {0}; //array to store historical readings, 10 readings default
    byte Oldest = 0; //Points to the oldest reading      
   
  public:  
    int updateAverage(int LatestReading);
    float updateAverage(float LatestReading);
    int getAverageInt();
    float getAverageFloat();
    char * getAverageIntText();
    char * getAverageFloatText();
};

//////////////////////////////////////////////////////////////////
//AlertHandler class: Tracking health of 

class AlertHandler
{  
  private:
    bool HealthOK = false;
    bool LastCheckResult = true;
    byte TriggerCount = 0 ; //Counts out of range readings
   
  public:  
    //AlertHandler();  //default constuctor: automatically declared
    bool reportHealth(bool HealthOK); //    
};
