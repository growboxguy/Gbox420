/*
 * 1_SimplePlaybackHelloWorld.ino
 * 
 * Demonstrates how simple it is to playback the melody of a song.
 * 
 * The song title: Hello World by Alan Walker ft. Torine
 * 
 * The circuit:
 *                                    _______________________________________________
 *                                   | Arduino Uno | Arduino Mega |        |         |
 *                                   |-------------|--------------| ESP32  | ESP8266 |
 *                                   | Norm | Mod  | Norm | Mode  |        |         |
 *                                   |------|------|------|-------|--------|---------|
 *     Buzzer |   + |----------------| 13   | 9    | 13   | 11    | GPIO4  | GPIO16  |
 *            |   - |----------------| GND  | GND  | GND  | GND   | GND    | GND     |
 *                                   |------|------|------|-------|--------|---------|
 * 
 * 
 * By ZulNs, @Gorontalo, January 2023
*/


// This option only affect on the AVR MCU based board
#define USE_MODIFIED_TONE_GENERATOR

#include <TonePlayer.h>
#include "song.h"

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
  top.setSong(SONG, sizeof(SONG), BPM);  // Set the song data, note: it must be in PROGMEM area.
                                         // Song buffer SONG is declared in file "song.h"
  top.play();  // Begins playback a song.
}

void loop() {
  top.loop();  // Call this line inside the loop() function to keep the song playing.
}
