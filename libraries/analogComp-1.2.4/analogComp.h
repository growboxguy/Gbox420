/*
	analogComp
	This library can be used to set and manage the analog comparator
    that is integrated in several Atmel microcontrollers

	Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>

    The latest version of this library can be found at:
    http://www.leonardomiliani.com/

	Current version: 1.2.2 - 2016/02/20
    (for a complete history of the previous versions, see the README file)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 3.0 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*/


#ifndef ANALOG_COMP_H
#define ANALOG_COMP_H

//library version
#define ANALOGCOMP_VERSION 124

//Library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//used to set the man number of analog pins
#ifdef NUM_ANALOG_INPUTS
#undef NUM_ANALOG_INPUTS
#endif

#define AC_REGISTER ADCSRA
//check if the micro is supported
#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega48P__) || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega168P__) || defined (__AVR_ATmega328P__)
	#define ATMEGAx8
	#define NUM_ANALOG_INPUTS 6
#elif defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
	#define ATTINYx5
	#define NUM_ANALOG_INPUTS 4
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	#define ATMEGA8
	#undef AC_REGISTER
	#define AC_REGISTER SFIOR
	#define NUM_ANALOG_INPUTS 6
#elif defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny84__)
	#define ATTINYx4
	#define NUM_ANALOG_INPUTS 8
#elif defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)
	#define ATMEGAx0
	#define NUM_ANALOG_INPUTS 16
#elif defined (__AVR_ATmega344__) || defined (__AVR_ATmega344P__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1284P__)
	#define ATMEGAx4
	#define NUM_ANALOG_INPUTS 8
#elif defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny4313__)
	#define ATTINYx313
	#define NUM_ANALOG_INPUTS 0
#elif defined (__AVR_ATmega32U4__)
	#define ATMEGAxU
	#define NUM_ANALOG_INPUTS 14 	// there are more inputs on the 32u4 than are broken out to the board
															// and they are not mapped sequentially
#else
	#error Sorry, microcontroller not supported!
#endif


const uint8_t AIN0 = 0;
const uint8_t INTERNAL_REFERENCE = 1;
const uint8_t AIN1 = 255;


class analogComp {
	public:
		//public methods
		//analogComp();
        uint8_t setOn(uint8_t = AIN0, uint8_t = AIN1);
        void setOff(void);
        void enableInterrupt(void (*)(void), uint8_t tempMode = CHANGE);
        void disableInterrupt(void);
        uint8_t waitComp(unsigned long = 0);
    private:
        //private methods
        uint8_t _initialized;
};
extern analogComp analogComparator;

#endif
