//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing built in comperator
//Library for the comperator: https://github.com/leomil72/analogComp
//Pins on Nano: AIN0: D6 (Fixed), ANI1: D7 (Can be changed to A0-A6)

#include "analogComp.h"

volatile boolean triggered = false; //used to check if the interrupt has raised
volatile int triggerCounter = 0; //Count the total number of interrupts

void setup() {
Serial.begin(115200);
Serial.println(F("Sketch for testing: built in comperator"));
 analogComparator.setOn(AIN0, AIN1); // AINO (D6) , the external signal is connected to ANI1 (D7)
 analogComparator.enableInterrupt(trigger, RISING); // all the trigger function when an intertupt is raised by the comperator (When ANT1 (D7) goes over 1.1V)
}

void loop() {
  delay(1000); //every seconds 
  if(triggered)
  {
    triggered = false; 
    Serial.print(F("Input over 1.1V triggered. Counter: "));
    Serial.println(triggerCounter);     
  }
}

void trigger() {
    triggered = true;
    triggerCounter++;
}
