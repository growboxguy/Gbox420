//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Watchdog for detecting crashes and autorestart

#include "avr/wdt.h"  //Watchdog
#include "avr/boot.h"  //Watchdog

byte loopcounter = 0;

void setup ()  {
  Serial.begin (115200);
  Serial.println (F("Sketch started."));
  wdt_enable(WDTO_8S);  //Watchdog set to 8seconds timeout (longest interval possible)
  boot_rww_enable();
  }  


void loop()
{
  Serial.println(loopcounter++);
  delay(1000);
  //wdt_reset(); // This needs to be called at least once every 8 seconds, else the Arduino will reset. Commented out to force a reset.
}
