/* This file is part of the analogComp library.
   Please check the README file and the notes
   inside the analogComp.h file
*/

//include required libraries
#include "analogComp.h"


//global variables
typedef void (*userFunc)(void);
volatile static userFunc userFunction;
uint8_t _initialized;
uint8_t _interruptEnabled;
uint8_t oldADCSRA;


//setting and switching on the analog comparator
uint8_t analogComp::setOn(uint8_t tempAIN0, uint8_t tempAIN1) {
    if (_initialized) { //already running
        return 1;
    }

    //initialize the analog comparator (AC)
    ACSR &= ~(1<<ACIE); //disable interrupts on AC
    ACSR &= ~(1<<ACD); //switch on the AC

    //choose the input for non-inverting input
    if (tempAIN0 == INTERNAL_REFERENCE) {
        ACSR |= (1<<ACBG); //set Internal Voltage Reference (1V1)
    } else {
        ACSR &= ~(1<<ACBG); //set pin AIN0
    }

//for Atmega32U4, only ADMUX is allowed as input for AIN-
#ifdef ATMEGAxU
	if (tempAIN1 == AIN1) {
		tempAIN1 = 0; //choose ADC0
	}
#endif

//AtTiny2313/4313 don't have ADC, so inputs are always AIN0 and AIN1
#ifndef ATTINYx313
    // allow for channel or pin numbers
#if defined (ATMEGAx0)
    if (tempAIN1 >= 54) tempAIN1 -= 54;
#elif defined (ATMEGAxU)
    //ATMEGAxU pins A0..A11 don't map to boards in sequence, so easier to deal with this in a case statement
    if(tempAIN1>=A0 && tempAIN1<=A11){
        switch(tempAIN1){
            case A0:
                tempAIN1 = 7;
                break;
            case A1:
                tempAIN1 = 6;
                break;
            case A2:
                tempAIN1 = 5;
                break;
            case A3:
                tempAIN1 = 4;
                break;
            case A4:
                tempAIN1 = 1;
                break;
            case A5:
                tempAIN1 = 0;
                break;
            case A6:
                tempAIN1 = 8;
                break;
            case A7:
                tempAIN1 = 10;
                break;
            case A8:
                tempAIN1 = 11;
                break;
            case A9:
                tempAIN1 = 12;
                break;
            case A10:
                tempAIN1 = 13;
                break;
            case A11:
                tempAIN1 = 9;
                break;
        }
    }
#elif defined (ATMEGAx4)
	if (tempAIN1 >= 24) tempAIN1 -= 24;
#elif defined (CORE_ANALOG_FIRST) && (defined (ATTINYx5) || defined (ATTINYx4))
    if (tempAIN1 >= CORE_ANALOG_FIRST) {
        tempAIN1 -= CORE_ANALOG_FIRST;
    }
#else
	if (tempAIN1 >= 14) tempAIN1 -= 14;
#endif
    //choose the input for inverting input
    oldADCSRA = ADCSRA;
    if ((tempAIN1 >= 0) && (tempAIN1 < NUM_ANALOG_INPUTS)) { //set the AC Multiplexed Input using an analog input pin
        ADCSRA &= ~(1<<ADEN);
        ADMUX &= ~31; //reset the first 5 bits
        #ifndef ATMEGAxU
            ADMUX |= tempAIN1; //choose the ADC channel (0..NUM_ANALOG_INPUTS-1)
        #else
            switch(tempAIN1){
                // see p. 313 of Atmel-7766J-USB-ATmega16U4/32U4-Datasheet_04/2016
                case 0:
                    //ADMUX |= 0b00000000; // redundant
                    AC_REGISTER &= ~bit(MUX5);
                    break;
                case 1:
                    ADMUX |= 0b00000001;
                    AC_REGISTER &= ~bit(MUX5);
                    break;
                case 2:
                    // not a valid choice - and not broken out onto Leonardo / Micro type boards
                    break;
                case 3:
                    // not a valid choice - and not broken out onto Leonardo / Micro type boards
                    break;
                case 4:
                    ADMUX |= 0b00000100;
                    AC_REGISTER &= ~bit(MUX5);
                    break;
                case 5:
                    ADMUX |= 0b00000101;
                    AC_REGISTER &= ~bit(MUX5);
                    break;
                case 6:
                    ADMUX |= 0b00000110;
                    AC_REGISTER &= ~bit(MUX5);
                    break;
                case 7:
                    ADMUX |= 0b00000111;
                    AC_REGISTER &= ~bit(MUX5);
                    break;
                case 8:
                    //ADMUX |= 0b00000000; // redundant
                    AC_REGISTER |= bit(MUX5);
                    break;
                case 9:
                    ADMUX |= 0b00000001;
                    AC_REGISTER |= bit(MUX5);
                    break;
                case 10:
                    ADMUX |= 0b00000010;
                    AC_REGISTER |= bit(MUX5);
                    break;
                case 11:
                    ADMUX |= 0b00000011;
                    AC_REGISTER |= bit(MUX5);
                    break;
                case 12:
                    ADMUX |= 0b00000100;
                    AC_REGISTER |= bit(MUX5);
                    break;
                case 13:
                    ADMUX |= 0b00000101;
                    AC_REGISTER |= bit(MUX5);
                    break;
                default:
                    break;
            }
        #endif
        AC_REGISTER |= (1<<ACME);
    } else {
        AC_REGISTER &= ~(1<<ACME); //set pin AIN1
    }
#endif

//disable digital buffer on pins AIN0 && AIN1 to reduce current consumption
#if defined(ATTINYx5)
		DIDR0 |= ((1<<AIN1D) | (1<<AIN0D));
#elif defined(ATTINYx4)
		DIDR0 |= ((1<<ADC2D) | (1<<ADC1D));
#elif defined (ATMEGAx4)
		DIDR1 |= (1<<AIN0D);
#elif defined (ATTINYx313)
		DIDR |= ((1<<AIN1D) | (1<<AIN0D));
#elif defined (ATMEGAx8) || defined(ATMEGAx4) || defined(ATMEGAx0)
		DIDR1 |= ((1<<AIN1D) | (1<<AIN0D));
#endif
    _initialized = 1;
    return 0; //OK
}


//enable the interrupt on comparations
void analogComp::enableInterrupt(void (*tempUserFunction)(void), uint8_t tempMode) {
    if (_interruptEnabled) { //disable interrupts
		SREG &= ~(1<<SREG_I);
        ACSR &= ~(1<<ACIE);
    }

    if (!_initialized) {
        setOn(AIN0, AIN1);
    }

    //set the interrupt mode
    userFunction = tempUserFunction;
    if (tempMode == CHANGE) {
        ACSR &= ~((1<<ACIS1) | (1<<ACIS0)); //interrupt on toggle event
    } else if (tempMode == FALLING) {
        ACSR &= ~(1<<ACIS0);
        ACSR |= (1<<ACIS1);
    } else { //default is RISING
        ACSR |= ((1<<ACIS1) | (1<<ACIS0));

    }
    //enable interrupts
    ACSR |= (1<<ACIE);
    SREG |= (1<<SREG_I);
    _interruptEnabled = 1;
}


//disable the interrupt on comparations
void analogComp::disableInterrupt(void) {
    if ((!_initialized) || (!_interruptEnabled)) {
        return;
    }
    ACSR &= ~(1<<ACIE); //disable interrupt
    _interruptEnabled = 0;
}


//switch off the analog comparator
void analogComp::setOff() {
    if (_initialized) {
		if (_interruptEnabled) {
			ACSR &= ~(1<<ACIE); //disable interrupts on AC events
			_interruptEnabled = 0;
		}
        ACSR |= (1<<ACD); //switch off the AC

//reenable digital buffer on pins AIN0 && AIN1
#if defined(ATTINYx5)
	DIDR0 &= ~((1<<AIN1D) | (1<<AIN0D));
#elif defined(ATTINYx4)
	DIDR0 &= ~((1<<ADC2D) | (1<<ADC1D));
#elif defined (ATMEGAx4)
	DIDR1 &= ~(1<<AIN0D);
#elif defined (ATTINYx313)
	DIDR &= ~((1<<AIN1D) | (1<<AIN0D));
#elif defined (ATMEGAx8) || defined(ATMEGAx4) || defined(ATMEGAx0)
    DIDR1 &= ~((1<<AIN1D) | (1<<AIN0D));
#endif

#ifndef ATTINYx313
        if (oldADCSRA & (1<<ADEN)) { //ADC has to be powered up
            AC_REGISTER |= (1<<ADEN); 
        }
#endif
		_initialized = 0;
    }
}


//wait for a comparation until the function goes in timeout
uint8_t analogComp::waitComp(unsigned long _timeOut) {
	//exit if the interrupt is on
	if (_interruptEnabled) {
		return 0; //error
	}

	//no timeOut?
	if (_timeOut == 0) {
		_timeOut = 5000; //5 secs
	}

	//set up the analog comparator if it isn't
	if (!_initialized) {
		setOn(AIN0, AIN1);
		_initialized = 0;
	}

	//wait for the comparation
	unsigned long _tempMillis = millis() + _timeOut;
	do {
		if ((ACSR && (1<<ACO)) == 1) { //event raised
			return 1;
		}
	} while ((long)(millis() - _tempMillis) < 0);

	//switch off the analog comparator if it was off
	if (!_initialized) {
		setOff();
	}
	return 0;
}


//ISR (Interrupt Service Routine) called by the analog comparator when
//the user choose the raise of an interrupt
#if defined(ATMEGAx8) || defined(ATMEGAx0) || defined(ATMEGAx4) || defined(ATMEGAxU)
ISR(ANALOG_COMP_vect) {
#else
ISR(ANA_COMP_vect) {
#endif
    userFunction(); //call the user function
}


analogComp analogComparator;
