#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H
/**
 * Author	: Nico Verduin (nico.verduin@ziggo.nl)
 * Date		: 31-8-2016
 *
 * generic MovingAverage class
 * This is a template class that allows one to create a parametric moving average object.
 * The class will be initialized based on a template parameter (i.e. float, double, int , uint16_t etc)
 * non floating point objects will always return a new moving average based on the same type as
 * it was initialized.
 * Keep in mind that floating point on Arduino has a limitation in accuracy up to 6-7 digits.
 * If the input for the MA is too lager, intermediate results will lose significant digits causing
 * inaccuracy.
 *
 * functions
 * ---------
 * MovingAverage(uint16_t)		Constructor creating object suitable for the parameterized number of entries
 * ~MovingAverage()				Destructor freeing up unallocated memory
 * T CalculateMovingAverage(T)	Calculate a moving average based on type T (can be any type i.e. float, int etc)
 * 								and returns type T (same as values passed)
 *
 * assert
 * ------
 * Operational guard routine	The library has an assert built in but is turned off under normal conditions.
 * 								However switching this on through #define DEBUG_MODE will ensure that the
 * 								program halts if more memory is requested than available. For example in an
 * 								Arduino UNO Ram is limited to 2K. If you want to do a moving average on 1000
 * 								floating point values this would require (1000 x 4 bytes) = 4000 bytes. The check
 * 								will output an error on the Serial if this occurs.
 *
 * 	using RAM efficiëntly
 * 	---------------------
 * 	If a moving average does not need to stay alive until the Arduino is reset or switch off, use the
 * 	deconstructor (! MovingAverage() to free up Ram.
 * 	This will free up the dynamic allocated internal array of sizeof variable type * number of entries for
 * 	moving average.
 */

#include "Arduino.h"

//#define DEBUG_MODE			// comment this line to avoid assert checks
#ifdef DEBUG_MODE

#define __ASSERT_USE_STDERR
#include <assert.h>
//
// handle diagnostic informations given by assertion and abort program execution:
//
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
	Serial.begin(9600);
    // transmit diagnostic informations through serial link.
    Serial.println(__func);
    Serial.println(__file);
    Serial.println(__lineno, DEC);
    Serial.println(__sexp);
    Serial.flush();
    // abort program execution.
    abort();
}
#endif
//
// Generic catch-all implementation for determining type of variable
// with thanks to http://arduino.stackexchange.com/questions/3079/how-to-retrieve-the-data-type-of-a-variable
//
template <typename T_ty> struct TypeInfo { static const char * name; };
template <typename T_ty> const char * TypeInfo<T_ty>::name = "unknown";
//
// Handy macro to make querying stuff easier.
//
#define TYPE_NAME(var) TypeInfo< typeof(var) >::name
//
// Handy macro to make defining stuff easier.
//
#define MAKE_TYPE_INFO(type)  template <> const char * TypeInfo<type>::name = #type;
//
// Type-specific implementations. Only float and double need to be tested
//
MAKE_TYPE_INFO( float )
MAKE_TYPE_INFO( double )

/**
 * Moving Average Class as a template
 */
template<class T>
class MovingAverage {
private:
	T 			*elements;     			// Type pointer elements in our array
	uint16_t 	_currentIndex;			// current index
	T 			_sum;					// sum for floats and double
	int32_t 	_sum_32;				// sum for rest
	boolean 	_loopedThrough;			// used to determine if we went through the whole array
	uint16_t 	_numberOfElements;		// number of elements in our array

public:
	MovingAverage(uint16_t);			// constructor
	~MovingAverage();					// destructor
	T CalculateMovingAverage(T);		// Calculate new moving Average
};
/**
 * @name MovingAverage Constructor
 * @param numberOfElements number of elements in the array
 * Creates dynamically an array of our type values and initializes the array
 * and some local variables. There is no destructor as this is quite useless
 * due to lack of garbage collection in Arduino
 */
template<class T>
MovingAverage<T>::MovingAverage(uint16_t numberOfelements) {

	//
	// allocate an array of size T
	//
	_numberOfElements = numberOfelements;
	elements = (T *) malloc(_numberOfElements * sizeof(T));

#ifdef DEBUG_MODE
	//
	// verify enough memory
	//
	assert(elements != NULL);
#endif

	for (uint8_t i = 0; i < _numberOfElements; i++) {
		elements[i] = 0.0;
	}
	_loopedThrough 	= false;				// this becomes true when the array is minimally filled once
	_currentIndex 	= 0;					// start with index 0

	if ((strcmp(TYPE_NAME(T), "float") != 0) && (strcmp(TYPE_NAME(T), "double") != 0) ){
		_sum_32 = 0;
	} else {
		_sum = 0.0;
	}
}
/**
 * @name ~MovingAverage Constructor
 * frees dynamic allocated memory
 */
template<class T>
MovingAverage<T>::~MovingAverage() {

	free(elements);							// deletes dynamic allocated memory
	elements = NULL;						//
}
/**
 * @name CalculateMovingAverage
 * @param newValue	value to be added to array
 * @returns T returns the new moving average.
 * During the first filling of the elements array the moving average is determined on a less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */

template<class T>
T MovingAverage<T>::CalculateMovingAverage(T newValue) {
	//
	// check if index is at our last entry in the array
	//
	if (_currentIndex >= _numberOfElements) {
		_currentIndex 	= 0;						// reset index
		_loopedThrough 	= true;						// we looped through at least once
	}
	//
	// subtract our oldest and add latest value
	//
	if ((strcmp(TYPE_NAME(T), "float") != 0) && (strcmp(TYPE_NAME(T), "double") != 0) ){
		//
		// non floating point
		//
		_sum_32 = _sum_32 - elements[_currentIndex] + newValue;
	} else {
		//
		// floating point
		//
		_sum = _sum - elements[_currentIndex] + newValue;
	}
	elements[_currentIndex] = newValue;
	//
	// and increment index
	//
	_currentIndex++;

	//
	// return the correct moving average
	//
	if ((strcmp(TYPE_NAME(T), "float") != 0) && (strcmp(TYPE_NAME(T), "double") != 0) ){
		//
		// non floating point
		//
		if (_loopedThrough) {
			return (T) (_sum_32 / _numberOfElements);
		} else {
			return (T) (_sum_32 / _currentIndex);
		}
	} else {
		//
		// non floating point
		//
		if (_loopedThrough) {
			return (T) (_sum / _numberOfElements);
		} else {
			return (T) (_sum / _currentIndex);
		}
	}
}

#endif

