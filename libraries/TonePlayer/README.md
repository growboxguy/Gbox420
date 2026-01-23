# Arduino TonePlayer Library

## Overview
A library for playing melody in background process using the builtin `tone()` function. No needed for `delay()` function to waste the MCU processing power. It parses melodic tones from `PROGMEM` to conserve the SRAM. The frequency of each tone is computed instead of being stored as constant values. Supports repeat to a specified number of tones till five levels of stack.

## Example
```c
// This option only affect on the AVR MCU based board
#define USE_MODIFIED_TONE_GENERATOR

#include <TonePlayer.h>

const PROGMEM uint8_t SONG[] = {
  NFIS6, -2, NFIS6, -2, ND6,  -2, NB5,   -2, REST,  -2, NB5,   -2, REST,  -2, NE6,   -2,
  REST,  -2, NE6,   -2, REST, -2, NE6,   -2, NGIS6, -2, NGIS6, -2, NA6,   -2, NB6,   -2,
  NA6,   -2, NA6,   -2, NA6,  -2, NE6,   -2, REST,  -2, ND6,   -2, REST,  -2, NFIS6, -2,
  REST,  -2, NFIS6, -2, REST, -2, NFIS6, -2, NE6,   -2, NE6,   -2, NFIS6, -2, NE6,   -2,
  REPEAT, 0, 0, 64, 0
};

#ifdef ESP32
#define SPEAKER_PIN                 4
#elif defined(ESP8266)
#define SPEAKER_PIN                 16
//#elif defined(__AVR_ATmega328P__) and defined(USE_MODIFIED_TONE_GENERATOR)
//      SPEAKER_PIN must be pin     9  (OC1A)
//#elif defined(__AVR_ATmega2560__) and defined(USE_MODIFIED_TONE_GENERATOR)
//      SPEAKER_PIN must be pin     11 (OC1A)
#elif defined(__AVR_ATmega328P__) or defined(__AVR_ATmega2560__)
#define SPEAKER_PIN                 13
#endif  // ESP32

#ifdef USE_MODIFIED_TONE_GENERATOR
TonePlayer top;
#else
TonePlayer top(SPEAKER_PIN);
#endif  // USE_MODIFIED_TONE_GENERATOR

void setup() {
  // Set the song data (note: it must be in PROGMEM area) with BPM 169.
  top.setSong(SONG, sizeof(SONG), 169);
  top.play();  // Begins playback a song.
}

void loop() {
  top.loop();  // Call this line inside the loop() function to keep the song playing.
}
```


&nbsp;

&nbsp;

---
#### Created by ZulNs
##### @Gorontalo, Indonesia, January 2023
---
