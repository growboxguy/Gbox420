//
// example sketch
//
#include "Arduino.h"
#include "MovingAverage.h"

float tableTest[] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
MovingAverage <float> test(3);			// create a moving average over last 3 values
/**
 * @name setup()
 * initializes the program
 */
void setup()
{
	Serial.begin(9600);
	//
	// loop through our numbers
	//
	for (uint8_t i = 0; i < 10; i++) {

		Serial.print(F(" value = "));
		Serial.print(tableTest[i]);
		Serial.print(F(" moving AVG = "));
		Serial.println(test.CalculateMovingAverage(tableTest[i]));
	}
	//
	// free up memory
	//
	test.~MovingAverage();
}
/**
 * @name loop()
 * continues until powerdown or reset
 */
void loop()
{
}

