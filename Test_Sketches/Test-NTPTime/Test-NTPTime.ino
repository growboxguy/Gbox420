//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing getting the current time from an NTP server over the ESP-link
//This sketch assumes you already have the ESP8266 chip flashed with the ESP-Link firmware,
//and connected it to the internet, as described in https://sites.google.com/site/growboxguy/arduino/esp8266
//If you get stuck at ESP-Link starting....... message means the Arduino cannot get in sync with the ESP8266 chip over Serial3 port.
//Make sure to enabled the SLIP protocol under REST/MQTT tab on the ESP-link website!

//Libraries
#include "ELClient.h"    // ESP-link
#include "ELClientCmd.h" //ESP-link - Get current time from the internet using NTP
#include "TimeLib.h"     //Time handling

//Global variable
char CurrentTime[20]; //buffer for getting current time

// Component initialization
ELClient ESPLink(&Serial3);
ELClientCmd EspCmd(&ESPLink);

void setup()
{
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.print(F("ESP-Link starting.."));
  while (!ESPLink.Sync())
  {
    Serial.print(".");
    Serial3.print(".");
    delay(500);
  };
  Serial.println("ready");
  setSyncProvider(getNtpTime); //points to the function that returns time_t current time
  setSyncInterval(30);         //sync every 30 seconds for testing (normally 3600sec - 1 hour)
}

void loop()
{
  ESPLink.Process();
  Serial.println(getFormattedTime());
  Serial.print("Time status: ");
  Serial.print(timeStatus());
  Serial.println(". ( 0: Not synced, 1: Sync needed , 2: In Sync )");
  delay(10000);
}

time_t getNtpTime()
{
  long LastRefresh = millis();
  time_t NTPResponse = 0;
  Serial.print(F("Waiting for NTP time (30sec timeout)..."));
  while (NTPResponse == 0 && millis() - LastRefresh < 30000)
  {
    NTPResponse = EspCmd.GetTime();
    delay(50);
  }
  if (NTPResponse == 0)
    Serial.println(F("sync failed"));
  else
    Serial.println(F("NTP synchronized"));
  return NTPResponse;
}

char *getFormattedTime()
{
  time_t Now = now();                                                                                                                                // Get the current time and date from the TimeLib library
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d/%02d/%02d-%02d:%02d:%02d", year(Now), month(Now), day(Now), hour(Now), minute(Now), second(Now)); // YYYY/MM/DD-HH:mm:SS formatted time will be stored in CurrentTime global variable
  return CurrentTime;
}
