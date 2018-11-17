#include "Arduino.h"

// Output raw timings to serial port. Timings are adjusted for sensor lag.

// IR detector output is active low
#define MARK   0
#define SPACE  1
#define IR_BUF_SIZE 64

byte interruptPin;

volatile unsigned long irBufferDuration[IR_BUF_SIZE];
volatile byte irBufferType[IR_BUF_SIZE];
volatile unsigned int headIrBuffer = 0;
volatile unsigned int tailIrBuffer = 0;

unsigned long bitMarkSumTime = 0;
unsigned int bitMarkCnt = 0;
unsigned long zeroSpaceSumTime = 0;
unsigned int zeroSpaceCnt = 0;
unsigned long oneSpaceSumTime = 0;
unsigned int oneSpaceCnt = 0;

volatile unsigned long lastInterrupt = micros();

void irSetup(byte intPin) {
	pinMode(LED_BUILTIN, OUTPUT);

	interruptPin = intPin;
	attachInterrupt(digitalPinToInterrupt(interruptPin), irISR, CHANGE);  // attach interrupt handler
}

void irISR() {
	unsigned long duration = 0;
	unsigned long now = micros();
	if (now > lastInterrupt) {
		duration = now - lastInterrupt;
	} else {
		duration = 0xffffffff - lastInterrupt;
		duration += now + 1;
	}
	lastInterrupt = now;

	uint8_t  irPinState = (uint8_t)digitalRead(interruptPin);
	if (irPinState == MARK) {
		// A space ended
		duration += 100;
	} else {
		// A mark ended
		duration -= 95;
	}

	irBufferDuration[headIrBuffer] = duration;
	irBufferType[headIrBuffer] = 1 - irPinState;
	headIrBuffer++;
	headIrBuffer &= IR_BUF_SIZE - 1;
	if (headIrBuffer == tailIrBuffer) {
		// Buffer overrun, discard the oldest value
		tailIrBuffer++;
		tailIrBuffer &= IR_BUF_SIZE - 1;
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("Serial started.");
	pinMode(LED_BUILTIN, OUTPUT);
	irSetup(2);
}

unsigned int totalCount = 0;
unsigned int count = 0;
void loop() {
	if (headIrBuffer != tailIrBuffer) {
		if ((irBufferType[tailIrBuffer] == SPACE) && irBufferDuration[tailIrBuffer] > 6000) {
			// Large gap.
			Serial.println();
			Serial.println();
			totalCount = count = 0;
			if (bitMarkCnt > 0) {
				Serial.print("Bit mark count: ");
				Serial.print(bitMarkCnt);
				Serial.print("  Average: ");
				Serial.println(bitMarkSumTime/bitMarkCnt);
			}
			if (zeroSpaceCnt > 0) {
				Serial.print("Zero space count: ");
				Serial.print(zeroSpaceCnt);
				Serial.print("  Average: ");
				Serial.println(zeroSpaceSumTime/zeroSpaceCnt);
			}
			if (oneSpaceCnt > 0) {
				Serial.print("One space count: ");
				Serial.print(oneSpaceCnt);
				Serial.print("  Average: ");
				Serial.println(oneSpaceSumTime/oneSpaceCnt);
			}
		} else {
			char buffer[16];
			sprintf(buffer, "  %5lu", irBufferDuration[tailIrBuffer]);
			Serial.print(buffer);
			Serial.print(irBufferType[tailIrBuffer] == SPACE ? "S" : "M");

			totalCount++;
			if (totalCount == 2) {
				Serial.println();
				count = 0;
			} else {
				count++;
				if (count == 16) {
					Serial.println();
					count = 0;
				}
			}
			if (irBufferType[tailIrBuffer] == MARK) {
				if (irBufferDuration[tailIrBuffer] < 1500) {
					bitMarkSumTime += irBufferDuration[tailIrBuffer];
					bitMarkCnt++;
				}
			} else {
				if (irBufferDuration[tailIrBuffer] < 1200) {
					zeroSpaceSumTime += irBufferDuration[tailIrBuffer];
					zeroSpaceCnt++;
				} else if (irBufferDuration[tailIrBuffer] < 2400) {
					oneSpaceSumTime += irBufferDuration[tailIrBuffer];
					oneSpaceCnt++;
				}
			}
		}
		tailIrBuffer++;
		tailIrBuffer &= IR_BUF_SIZE - 1;
	}
//	delay(10);
}
