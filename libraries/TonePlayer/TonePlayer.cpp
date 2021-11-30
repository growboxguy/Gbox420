/*
 
 Play tones with hardware timers
 Author: Nick Gammon
 Date:   1 April 2013

 
 PERMISSION TO DISTRIBUTE
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 and associated documentation files (the "Software"), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in 
 all copies or substantial portions of the Software.
 
 
 LIMITATION OF LIABILITY
 
 The software is provided "as is", without warranty of any kind, express or implied, 
 including but not limited to the warranties of merchantability, fitness for a particular 
 purpose and noninfringement. In no event shall the authors or copyright holders be liable 
 for any claim, damages or other liability, whether in an action of contract, 
 tort or otherwise, arising from, out of or in connection with the software 
 or the use or other dealings in the software. 

 
 Example of use:
 
 #include <TonePlayer.h>

 ...
 
 TonePlayer tone1 (TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno), D11 (Mega)

 or, on the Mega 2560:

 TonePlayer tone3 (TCCR3A, TCCR3B, OCR3AH, OCR3AL, TCNT3H, TCNT3L);  // pin D5
 TonePlayer tone4 (TCCR4A, TCCR4B, OCR4AH, OCR4AL, TCNT4H, TCNT4L);  // pin D6
 TonePlayer tone5 (TCCR5A, TCCR5B, OCR5AH, OCR5AL, TCNT5H, TCNT5L);  // pin D46

 ...
 
 pinMode (9, OUTPUT);  // D9 on Uno, D11 on Mega 2560
 tone1.tone (440);     // play 440 Hz
 delay (500);          // wait half a second
 tone1.noTone ();      // stop playing

 or, on the Mega 2560:
 
 pinMode (46, OUTPUT);  
 tone5.tone (440);

*/

#include <TonePlayer.h>


void TonePlayer::tone (const unsigned int Hz)
{
  // it takes two toggles for one "cycle"
  unsigned long ocr = F_CPU / Hz / 2;
  byte prescaler = _BV (CS10);  // start with prescaler of 1  (bits are the same for all timers)
  
  // too large? prescale it
  if (ocr > 0xFFFF)
    {
    prescaler |= _BV (CS11);    // now prescaler of 64
    ocr /= 64;
    }
  
  // stop timer
  *timerRegA_ = 0;
  *timerRegB_ = 0;
  
  // reset counter
  *timerTCNTH_ = 0;
  *timerTCNTL_ = 0;
  
  // what to count up to
  *timerOCRH_ = highByte (ocr);
  *timerOCRL_ = lowByte (ocr);
  
  *timerRegA_ = _BV (COM1A0);             // toggle output pin
  *timerRegB_ = _BV (WGM12) | prescaler;  // CTC
  }  // end of TonePlayer::tone

void TonePlayer::noTone ()
  {
  // stop timer
  *timerRegA_ = 0;
  *timerRegB_ = 0;  
  } // end of TonePlayer::noTone

