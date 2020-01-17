//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Set time on real time clock module. Update the timeToSet variable and run the sketch once.

//Libraries
#include "stdio.h"  //Format Real time clock output
#include "DS1302.h"  //Real time clock

//Pins
const int ClockRSTPin   = 5;  // Reset -  Real time clock
const int ClockDATPin   = 6;  // Input/Output -  Real time clock
const int ClockCLKPin = 7;  // Serial Clock -  Real time clock

//Global Variables
Time timeToSet(2018, 10, 17, 16, 20, 00, Time::kSunday); // Sunday, October 17, 2018 at 04:20:00PM.
DS1302 Clock(ClockRSTPin, ClockDATPin, ClockCLKPin);

void setup() {
  Serial.begin(115200);
  Clock.writeProtect(false);  //Disable write protection
  Clock.halt(false);  //Enable setting the time
  Clock.time(timeToSet);  // Set the time and date on the chip.
  Clock.writeProtect(true); //Re-enable write protection
  //End of One-Time section
}

void loop() {  // Loop and print the time every second.
  Serial.println(getTime());
  delay(1000);
}

String getTime() {
  Time Now = Clock.time();  // Get the current time and date from the chip.
  char CurrentTime[20];
  snprintf(CurrentTime, sizeof(CurrentTime), "%04d-%02d-%02d %02d:%02d:%02d",Now.yr, Now.mon, Now.date,Now.hr, Now.min, Now.sec);
  return CurrentTime;
}
