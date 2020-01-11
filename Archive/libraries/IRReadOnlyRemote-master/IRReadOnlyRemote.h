/**
 * @author   �yvind Tryti
 * @version	 1.0
 */
#ifndef IRReadOnlyRemote_h
#define IRReadOnlyRemote_h

#include "Arduino.h"

// Set NEC_ONLY to 1 if you do not need to tweak the time settings in order to read other protocols
// Set NEC_ONLY to 0 if you need other time settings
#define NEC_ONLY 1

#if NEC_ONLY

class IRReadOnlyRemote {
	public:
		IRReadOnlyRemote(byte intPin);
		unsigned long read();
	private:
};

#else

class IRReadOnlyRemote {
	public:
		IRReadOnlyRemote(byte intPin,
			unsigned int baseTimeUnit,
			unsigned int hdrMark,
			unsigned int hdrSpace,
			unsigned int rptSpace,
			unsigned int bitMark,
			unsigned int zeroSpace,
			unsigned int oneSpace,
			unsigned int bits);
		unsigned long read();
	private:
};

#endif

static void irISR();

#endif
