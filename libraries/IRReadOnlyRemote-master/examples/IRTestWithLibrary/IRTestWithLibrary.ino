#include "Arduino.h"
#include "IRReadOnlyRemote.h"

#if NEC_ONLY
IRReadOnlyRemote remote(2);		// NEC
#else
//IRReadOnlyRemote remote(2, 564, 16, 8, 4, 1, 1, 3, 32);		// NEC
IRReadOnlyRemote remote(2, 564, 8, 8, 0, 1, 1, 3, 32);	// Samsung TV
#endif

void setup() {
	Serial.begin(115200);
	Serial.println("Serial started.");
}

void loop() {
	unsigned long data = remote.read();
	char key;
	if (data != 0) {
		// The values here are read from the 'KEYES' IR remote
		switch (data) {
		case 0xFF4AB5: key = '0'; break;
		case 0xFF6897: key = '1'; break;
		case 0xFF9867: key = '2'; break;
		case 0xFFB04F: key = '3'; break;
		case 0xFF30CF: key = '4'; break;
		case 0xFF18E7: key = '5'; break;
		case 0xFF7A85: key = '6'; break;
		case 0xFF10EF: key = '7'; break;
		case 0xFF38C7: key = '8'; break;
		case 0xFF5AA5: key = '9'; break;
		case 0xFF42BD: key = '*'; break;
		case 0xFF52AD: key = '#'; break;
		case 0xFF22DD: key = 'L'; break;
		case 0xFFC23D: key = 'R'; break;
		case 0xFF629D: key = 'U'; break;
		case 0xFFA857: key = 'D'; break;
		case 0xFF02FD: key = 'K'; break;
		case 0xFFFFFFFF: key = 'X'; break;
		default: key = '?';
		}

		Serial.print(key);
		Serial.print("   ");
		Serial.println(data, HEX);
	}
//	delay(10);
}
