#ifndef Helpers_H
#define Helpers_H

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "TimeLib.h" //Keeping track of time
#include "MemoryFree.h" //checking remaining memory - only for debugging
#include "avr/wdt.h" //Watchdog timer
#include "ELClient.h" //ESP-link
#include "ELClientWebServer.h" //ESP-link - WebServer API
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "420Settings.h"  //for storing/reading defaults

extern Settings MySettings;
extern char CurrentTime[20];
extern char Message[512];
extern ELClientCmd ESPCmd;

void logToSerials (const __FlashStringHelper* ToPrint,bool BreakLine);
time_t getNtpTime();
char * getFreeMemory();
char * getFormattedTime();

void saveSettings(bool LogThis);
void loadSettings();    
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


template <class logLine> void logToSerials (logLine* ToPrint,bool BreakLine) { 
  if(BreakLine){Serial.println((*ToPrint));Serial3.println((*ToPrint));}
  else{Serial.print((*ToPrint));Serial3.print((*ToPrint));}
}

template <class logLine> void logToSerials (logLine& ToPrint,bool BreakLine) { 
  if(BreakLine){Serial.println(ToPrint);Serial3.println(ToPrint);}
  else{Serial.print(ToPrint);Serial3.print(ToPrint);}
}

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
};

#endif
