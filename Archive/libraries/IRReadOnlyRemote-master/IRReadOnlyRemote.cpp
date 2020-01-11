/**
 * Receive and store IR frames from IR remote controls on an Arduino using the NEC style protocol.
 * This library is suitable for projects that only need to receive (robots?), no sending required.
 * No timer interrupts are used, so it will not interfere with other libraries. Only one hardware interrupt is used.
 *
 * The decoded IR frames are stored in a circular buffer that can store from 1 to 127 frames.
 * The default is 15 frames, change IR_BUF_SIZE for other sizes (size must be a power of 2). The capacity
 * of the buffer is one less than the defined size.
 * The processing needs are very low, the Interrupt Service Routine is called only on input pin state change.
 *
 * There is no need to retrieve the frames from the buffer as soon as they arrive. This enables the main program
 * to read the frames when it has time, and is not constrained by a requirement to read the frames as soon as they arrive
 * (as long as the buffer does not overflow) in order to not lose any frames.
 *
 * The NEC style protocol consists of a header mark, a header space, and a number of bit marks and bit spaces.
 * The length of the bit mark is always the same.
 * The length of the bit space determines if the bit is a 0 or a 1.
 * There can be a repeat frame (ditto).
 *
 * For a NEC remote, the constructor would be called like this:
 *
 * With the NEC_ONLY define set to 1:
 * IRReadOnlyRemote remote(2);
 * This means: Use pin 2 for the IR detector. All other values are set by defines.
 *
 * With the NEC_ONLY define set to 0:
 * IRReadOnlyRemote remote(2, 564, 16, 8, 4, 1, 1, 3, 32);
 * This means: Use pin 2 for the IR detector, the base time unit (btu) is 564 microseconds, the header mark is 16 btu long,
 * the header space is 8 btu long, the repeat header space is 4 btu long, the bit mark is 1 btu long,
 * the zero space is 1 btu long, the one space is 3 btu long, and finally, the frame contains 32 bits.
 * A Samsung TV remote (BN59-01175N) works with this:
 * IRReadOnlyRemote remote(2, 564, 8, 8, 0, 1, 1, 3, 32);
 *
 * See http://www.hifi-remote.com/johnsfine/DecodeIR.html for a large number of IR protocols.
 * See IRLibReference Appendix B (.docx or .pdf) at https://github.com/cyborg5/IRLib2/tree/master/IRLib2/manuals
 * for an explanation of the IRP notation that is understandable (the one in DecodeIR.html isn't).
 *
 * Only one function needs to be called - read() - which will return the next frame (if any) from the frame buffer.
 *
 * @author   Øyvind Tryti
 * @version	 1.0
 *
 * MIT License
 *
 * Copyright (c) 2017 otryti
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "IRReadOnlyRemote.h"


byte interruptPin;
volatile byte headIrBuffer = 0;
volatile byte tailIrBuffer = 0;

#if NEC_ONLY

// Size of buffer containing completed codes. Must be a power of 2.
// Minimal memory usage - only 3 frames stored
#define IR_BUF_SIZE 4

#define HDR_MARK_LOW	 7219
#define HDR_MARK_HIGH	11280
#define HDR_SPACE_LOW	 3610
#define HDR_SPACE_HIGH	 5640
#define RPT_SPACE_LOW	 1805
#define RPT_SPACE_HIGH	 2820
#define BIT_MARK_LOW	  451
#define BIT_MARK_HIGH	  705
#define ZERO_SPACE_LOW	  451
#define ZERO_SPACE_HIGH	  705
#define ONE_SPACE_LOW	 1354
#define ONE_SPACE_HIGH	 2115
#define FRAME_SIZE		   32

IRReadOnlyRemote::IRReadOnlyRemote(byte intPin) {
	interruptPin = intPin;
	attachInterrupt(digitalPinToInterrupt(interruptPin), irISR, CHANGE);
}

#else

// Size of buffer containing completed codes. Must be a power of 2.
#define IR_BUF_SIZE 16

// The limit of discrepancy from 'ideal' timing allowed
#define ALLOWED_LIMIT	 1.25

#define HDR_MARK_LOW	hdrMarkLow
#define HDR_MARK_HIGH	hdrMarkHigh
#define HDR_SPACE_LOW	hdrSpaceLow
#define HDR_SPACE_HIGH	hdrSpaceHigh
#define RPT_SPACE_LOW	rptSpaceLow
#define RPT_SPACE_HIGH	rptSpaceHigh
#define BIT_MARK_LOW	bitMarkLow
#define BIT_MARK_HIGH	bitMarkHigh
#define ZERO_SPACE_LOW	zeroSpaceLow
#define ZERO_SPACE_HIGH	zeroSpaceHigh
#define ONE_SPACE_LOW	oneSpaceLow
#define ONE_SPACE_HIGH	oneSpaceHigh
#define FRAME_SIZE		frameSize

unsigned int hdrMarkLow, hdrMarkHigh;
unsigned int hdrSpaceLow, hdrSpaceHigh;
unsigned int rptSpaceLow, rptSpaceHigh;
unsigned int bitMarkLow, bitMarkHigh;
unsigned int zeroSpaceLow, zeroSpaceHigh;
unsigned int oneSpaceLow, oneSpaceHigh;
unsigned int frameSize;

/**
 * In order to avoid doing any more calculations than absolutely necessary inside the
 * Interrupt Service Routine, all upper and lower limits of mark and space timings are calculated
 * here in the constructor, and stored for use by the ISR. A variance of 25% up or down is allowed.
 *
 * @param intPin		The pin to connect the detector to (must be 2 or 3 for Arduino UNO)
 * @param baseTimeUnit	The time in microseconds that all other timings will be a multiple of
 * @param hdrMark		Length of header mark in number of base time units
 * @param hdrSpace		Length of header space in number of base time units
 * @param rptSpace		Length of repeat header space in number of base time units
 * @param bitMark		Length of mark separating bits in number of base time units
 * @param zeroSpace		Size of space representing a zero bit in number of base time units
 * @param oneSpace		Size of space representing a one bit in number of base time units
 * @param bits			Number of bits in the complete frame (maximum 32)
 */
IRReadOnlyRemote::IRReadOnlyRemote(byte intPin,
		unsigned int baseTimeUnit,
		unsigned int hdrMark,
		unsigned int hdrSpace,
		unsigned int rptSpace,
		unsigned int bitMark,
		unsigned int zeroSpace,
		unsigned int oneSpace,
		unsigned int bits) {
	interruptPin = intPin;

	hdrMark *= baseTimeUnit;
	hdrMarkLow = hdrMark / ALLOWED_LIMIT;
	hdrMarkHigh = hdrMark * ALLOWED_LIMIT;

	hdrSpace *= baseTimeUnit;
	hdrSpaceLow = hdrSpace / ALLOWED_LIMIT;
	hdrSpaceHigh = hdrSpace * ALLOWED_LIMIT;

	if (rptSpace > 0) {
		rptSpace *= baseTimeUnit;
		rptSpaceLow = rptSpace / ALLOWED_LIMIT;
		rptSpaceHigh = rptSpace * ALLOWED_LIMIT;
	} else {
		rptSpaceLow = 0;
		rptSpaceHigh = 0;
	}

	bitMark *= baseTimeUnit;
	bitMarkLow = bitMark / ALLOWED_LIMIT;
	bitMarkHigh = bitMark * ALLOWED_LIMIT;

	zeroSpace *= baseTimeUnit;
	zeroSpaceLow = zeroSpace / ALLOWED_LIMIT;
	zeroSpaceHigh = zeroSpace * ALLOWED_LIMIT;

	oneSpace *= baseTimeUnit;
	oneSpaceLow = oneSpace / ALLOWED_LIMIT;
	oneSpaceHigh = oneSpace * ALLOWED_LIMIT;

	frameSize = bits;

	attachInterrupt(digitalPinToInterrupt(interruptPin), irISR, CHANGE);
}

#endif

volatile unsigned long irBuffer[IR_BUF_SIZE];

// IR detector output is active low
#define MARK   0
#define SPACE  1

#define WAIT_FOR_HDR_MARK 0
#define WAIT_FOR_HDR_SPACE 1
#define REPEAT_INCOMING 2
#define NORMAL_INCOMING 3

/**
 * The Interrupt Service Routine is called for each state change of the interrupt pin.
 * It will listen for a header mark followed by a header space, and then codeLength pairs of
 * bit mark and bit space. Each time a bit mark arrives after a bit space, the value (0 or 1) of the
 * bit space is added to a variable irData.
 *
 * When the IR frame is finished with a last stop mark, a certain amount of time is supposed to elapse
 * before the frame is considered complete.
 * This represents a problem here, since if there are no more IR frames to be received, there will be
 * no more interrupts to process.
 * In order to do this correctly, a timer interrupt would have to be issued when an appropriate amount of time
 * has passed. But we do not wish to use any more resources (timer interrupts) than we have to.
 * An alternative (as used in IRremote/IRLib/IRLib2) is to require the routine reading the codes to do so in
 * a fairly tight loop, making it possible for the function called to check if the space after the stop bit is long
 * enough to consider the frame complete.
 * We do not wish to impose this requirement either, so a calculated risk is taken: When the stop mark ends, the frame
 * is considered complete and the result is placed in the output buffer.
 *
 * The next mark to arrive must be a header mark for the decoding process to start up again.
 */
static void irISR() {
	static unsigned long lastInterrupt = micros();
	static unsigned int isrState = WAIT_FOR_HDR_MARK;
	static unsigned long irData = 0;
	static unsigned int irBitCount = 0;
	static bool dataLost = false;
	unsigned long duration;

	unsigned long now = micros();
	duration = now - lastInterrupt;
	lastInterrupt = now;

	uint8_t  irPinState = (uint8_t)digitalRead(interruptPin);

	if (irPinState == SPACE) {
		// A space has started (a mark just finished)
		// Due to sensor lag, when received, Marks tend to be too long
		duration -= 95;
		if (isrState == WAIT_FOR_HDR_MARK) {
			if ((duration >= HDR_MARK_LOW) && (duration <= HDR_MARK_HIGH)) {
				isrState = WAIT_FOR_HDR_SPACE;
			}
		} else {
			if ((duration >= BIT_MARK_LOW) && (duration <= BIT_MARK_HIGH)) {
				if ((isrState == REPEAT_INCOMING) || ((isrState == NORMAL_INCOMING) && (irBitCount == FRAME_SIZE))) {
					// A complete frame has been received. Store it and start waiting for the next frame.
					irBuffer[headIrBuffer++] = irData;
					headIrBuffer &= IR_BUF_SIZE - 1;
					if (headIrBuffer == tailIrBuffer) {
						// Buffer overrun, discard the oldest value
						tailIrBuffer++;
						tailIrBuffer &= IR_BUF_SIZE - 1;
					}
					isrState = WAIT_FOR_HDR_MARK;
				}
			} else {
				dataLost = true;
				isrState = WAIT_FOR_HDR_MARK;	// Bad bit mark length. Abandon current frame, start waiting for a new one.
			}
		}
	} else {
		// A mark has started (a space just finished)
		// Due to sensor lag, when received, Spaces tend to be too short
		duration += 100;
		if (isrState == WAIT_FOR_HDR_SPACE) {
			if ((duration >= HDR_SPACE_LOW) && (duration <= HDR_SPACE_HIGH)) {
				isrState = NORMAL_INCOMING;
				dataLost = false;
				irData = 0;
				irBitCount = 0;
			} else if ((duration >= RPT_SPACE_LOW) && (duration <= RPT_SPACE_HIGH)) {
				irData = 0xffffffff;	// The 'repeat frame' value
				// If a preceding frame was dropped, don't add repeat frames to the input stream.
				isrState = dataLost ? WAIT_FOR_HDR_MARK : REPEAT_INCOMING;
			} else {
				dataLost = true;
				isrState = WAIT_FOR_HDR_MARK;	// Bad header space length. Resume waiting for a header mark.
			}
		} else if (isrState == NORMAL_INCOMING) {
			irData <<= 1;
			irBitCount++;
			if ((duration >= ONE_SPACE_LOW) && (duration <= ONE_SPACE_HIGH)) {
				irData |= 1;
			} else if ((duration < ZERO_SPACE_LOW) || (duration > ZERO_SPACE_HIGH)) {
				dataLost = true;
				isrState = WAIT_FOR_HDR_MARK;	// Bad bit space length. Abandon current frame, start waiting for a new one.
			}
		}
	}
}

/**
 * Get the next code from the buffer.
 *
 * @Return	The next code from the buffer. Return 0 (zero) if the buffer is empty.
 */
unsigned long IRReadOnlyRemote::read() {
	uint8_t oldSREG = SREG;
	cli();
	if (headIrBuffer == tailIrBuffer) {
		SREG = oldSREG;
		return 0;	// No data available
	}
	unsigned long irData = irBuffer[tailIrBuffer];
	tailIrBuffer++;
	tailIrBuffer &= IR_BUF_SIZE - 1;
	SREG = oldSREG;
	return irData;
}

