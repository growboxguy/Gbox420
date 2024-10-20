 //GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing built in comperator
//Library for the comperator: https://github.com/leomil72/analogComp
/*
To choose the analog comparator inputs use the method setOn():

analogComparator.setOn([AIN+, AIN-]);

For the AIN+ you can choose between the following:
AIN0: set the AIN0 pin as input
INTERNAL_REFERENCE: set the internal voltage reference as input

For the AIN- you can choose between the following:
AIN1: set the AIN1 pin as input
A0..Ax: set the Analog Input Channel (max number depends of the MCU)

AIN+ and AIN- are optionals, if not set, AIN0 and AIN1 will be used.
*/

#include "analogComp.h"

volatile boolean triggered = false; //used to check if the interrupt has raised
volatile int triggerCounter = 0; //Count the total number of interrupts

void setup() {
Serial.begin(115200);
Serial.println(F("Sketch for testing: built in comperator"));
 analogComparator.setOn(INTERNAL_REFERENCE, AIN1); // AINO (D6) , the external signal is connected to ANI1 (D7)
 analogComparator.enableInterrupt(trigger, RISING); // all the trigger function when an intertupt is raised by the comperator (When ANT1 (D7) goes over 1.1V)
}

void loop() {
  delay(1000); //every seconds 
  if(triggered)
  {
    triggered = false; 
    Serial.print(F("Comparator triggered. Counter: "));
    Serial.println(triggerCounter);     
  }
}

void trigger() {
    triggered = true;
    triggerCounter++;
}
