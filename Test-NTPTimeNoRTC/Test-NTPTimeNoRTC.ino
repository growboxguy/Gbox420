//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing getting the current time from an NTP server over the ESP-link

//Libraries
#include "ELClient.h"  // ESP-link
#include "ELClientCmd.h"  //ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"  //Time handling

//Component initialization
ELClient ESPLink(&Serial3);
ELClientCmd EspCmd(&ESPLink);

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
  setSyncProvider(getNtpTime); //points to the function that returns time_t current time
  setSyncInterval(30); //sync every 30 seconds
}

void loop() {
  char CurrentTime[20];  //time gets read into this from clock  
  ESPLink.Process();
  time_t Now = now(); // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d",year(Now), month(Now), day(Now),hour(Now), minute(Now), second(Now)); //Format and store time
  
  Serial.println(CurrentTime);
  Serial.println(timeStatus());  //Returns 0: Not synced, 1: Sync needed , 2: In Sync
  delay(2000);
}

time_t getNtpTime(){
  long LastRefresh = millis();
  time_t NTPResponse = 0;
  Serial.print("Waiting for NTP time, timeout:10sec...");
  while(NTPResponse == 0 && millis() - LastRefresh < 10000){
   NTPResponse = EspCmd.GetTime();
   delay(100);
  }
  if(NTPResponse == 0) Serial.println ("FAILED");
  else Serial.println ("DONE");
  return NTPResponse;
}
