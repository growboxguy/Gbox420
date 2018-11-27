//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing getting the current time from an NTP server over the ESP-link

//Libraries
#include "ELClient.h"
#include "ELClientCmd.h"
#include "stdio.h"
#include "time.h"

//Global constants
const int UTCOffsetHours = 1;

//Global variables
long LastRefresh = 0;

//Component initialization
ELClient ESPLink(&Serial3);
ELClientCmd EspCmd(&ESPLink);

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.print(F("WebServer starting.."));
  while(!ESPLink.Sync())  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
    };
  Serial.println("ready");
}

void loop() {
  ESPLink.Process();
  if(LastRefresh == 0 || millis() - LastRefresh > 10000){
    getNtpTime();
    LastRefresh = millis();
  }  
}

void getNtpTime(){
   time_t now = EspCmd.GetTime() - UNIX_OFFSET + (UTCOffsetHours * 3600); //https://forum.arduino.cc/index.php?topic=567637.0
   struct tm ts = *localtime(&now);
   char FormattedTime[20];
   strftime(FormattedTime, sizeof(FormattedTime), "%Y/%m/%d-%H:%M:%S", &ts);
   Serial.println(FormattedTime);
}


