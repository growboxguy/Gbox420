# MovingAverage
Robust library for calculating a moving average. A moving average is an average value based on a predefined set of vales For example a moving average on 3 values is calculated on the last 3 values summed devided by 3. The catch is that the type of variable may be a series of ints or longs or floats or doubles etc. 
This libary allows one to create an object for a moving averages that is determined by the definition parameter.

A moving avarage based on floats would be defined as follows:

```
MovingAverage <float> myMovingAverage(numberOfEntries);
```

Where ```<float>``` means the moving average is calculated based on floats
Other examples could be ```<unsigned int>``` or ```<uint8_t>``` or ```<long>``` or ```<double>```

```NumberOfEntries``` tells the Moving Average on how many values the average is based. Also note that when the moving average starts, there could be insufficient values given to the moving average object. The object keeps track of this. If only one value has been added during calculation the average will of course be based on the single value.

######Accuracy
As a moving average concerns adding numbers to divide them later an overflow might occur. The Arduino has no safety built in so the following is done. ```<floats>``` and ```<doubles>``` are summed in a ````<float total>```. This means that since floats have an accuracy of 6-7 digits at most, significance might be lost by larger values. Unfortunately the library does not resolve this. all other types are summed in a ```<32-bit signed integer>``` as a moving average might also imply negative values. The result is always returned in the type defined when creating the object.

######Memory Check
During thew setup internally a dynamic array will be created. If there is not enough (and with only 2K on an Arduino UNO) you may set the assert function on. This might be useful during debugging. Turning the assert function on is done by uncommenting the line

```//#define DEBUG_MODE			// comment this line to avoid assert checks```
At the top of **MovingAverage.h** in the library. 

##Functions
######**MovingAverage(uint16_t)**
This is the object constructor that initializes the object by setting certain values to zero and allocating a dynamic array used for internal processing. The parameter is the number of values used to calculate the moving averager.

######**~MovingAverage()**
This is the destructor of the object. When called, It frees the memory of the internal array giving it back to the free memory heap. The object still exists but cannot be used any more.

######**T CalculateMovingAverage(T)**
T is the type used i.e. ```<float>``` or ```<int>``` etc.

##Example sketch
A simple sample sketch which is also included in the library is as follows:

```
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
```