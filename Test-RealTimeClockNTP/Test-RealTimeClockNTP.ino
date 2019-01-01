//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing getting the current time from an NTP server over the ESP-link

//Libraries
#include "ELClient.h"  // ESP-link
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "Time.h"  //Time handling
#include "Stdio.h" //Time formatting
#include "DS1302.h" //Real time clock
  
//Pins
const byte ClockRSTPin = 5;  //Reset -  Real time clock
const byte ClockDATPin = 6;  //Data -  Real time clock
const byte ClockCLKPin = 7;    //Serial Clock -  Real time clock

//Global constants
const int UTCOffsetHour = -8;   //UTC Time hour offset: Pacific Standard time (UTC-8)
const int UTCOffsetMinute = 0;  //UTC Time minute offset

//Global variables
long LastRefresh = 0;
char CurrentTime[20];  //time gets read into this from clock

//Component initialization
ELClient ESPLink(&Serial3);
ELClientCmd EspCmd(&ESPLink);
DS1302 Clock(ClockRSTPin, ClockDATPin, ClockCLKPin); //Real time clock

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.print(F("ESP-Link starting.."));
  while(!ESPLink.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  Serial.println("ready");
}

void loop() {
  ESPLink.Process();
  getRTCTime();
  Serial.println(CurrentTime);
  if(LastRefresh == 0 || millis() - LastRefresh > 10000){
    Serial.println("Updating time...");
    getNtpTime();
    LastRefresh = millis();
  }
  delay(2000);
}

void getRTCTime() {  //fills the CurrentTime global variable
  Time Now = Clock.time();  // Get the current time and date from the chip.
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",Now.yr, Now.mon, Now.date,Now.hr, Now.min, Now.sec);  //Format and store time
}

void getNtpTime(){
   time_t now = EspCmd.GetTime() - UNIX_OFFSET + (UTCOffsetHour * 3600) + (UTCOffsetMinute * 60); //https://forum.arduino.cc/index.php?topic=567637.0
   struct tm ts = *localtime(&now);
   strftime(CurrentTime, sizeof(CurrentTime), "%Y/%m/%d-%H:%M:%S", &ts);
   Serial.print("NTP response: ");Serial.println(CurrentTime);
   setTime(CurrentTime);
}

void setTime(char* dateToSet) {  //sets the real time clock
  int year    = cropFromString( dateToSet, 0, 4);
  int month   = cropFromString( dateToSet, 5, 2);
  int day     = cropFromString ( dateToSet, 8, 2);
  int hour    = cropFromString ( dateToSet, 11, 2);
  int minute  = cropFromString ( dateToSet, 14, 2);
  int sec  = cropFromString ( dateToSet, 17, 2);  
  Clock.writeProtect(false);  //Disable write protection
  Time timeToSet(year,month,day,hour,minute,sec,Time::kSaturday); //its ALWAYS Saturday :)
  Clock.time(timeToSet);
  Clock.halt(false);  //Enable setting the time
  Clock.writeProtect(true); //Re-enable write protection
}

int cropFromString(char* string,byte start, byte width){
  int value=0;
  for( byte n=0; n < width; n++ )
    value = value * 10 + string[start+n] - '0';
  return value;  
}
