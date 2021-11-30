/*
This is a simple sketch to demonstrate the use of analogComp, a
library to manage the analog comparator included in a wide 
variety of Atmel microcontrollers

This sketch checks if the voltage on AIN0 will begin greater
than the voltage on AIN1. To test the sketch, build the following
circuit:
- connect pin AIN1 to pin 3V3
- connect pin AIN0 to GND using a pull-down resistor (10Kohm or greater)
- connect pin AIN0 to pin 5V to activate the comparation.

More info on the analog comparator can be found reading the datasheet.

Please read the README file to know how to use this library.

Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
	
This code and the analogComp library are free software; you can redistribute 
and/or modify them under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3.0 of the License,
or (at your option) any later version.

This work is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

*/

//include the library
#include "analogComp.h"

//let's set up the hardware
void setup() {
    //opens the serial comm
    Serial.begin(9600);
    delay(2000);

}

//main loop
void loop() {
    //wait 5 seconds for AIN0 to begin greater than AIN1
    Serial.println(analogComparator.waitComp(), DEC);
}
