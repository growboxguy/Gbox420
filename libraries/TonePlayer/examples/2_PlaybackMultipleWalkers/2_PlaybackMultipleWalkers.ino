/*
 * 2_PlaybackMultipleWalkers.ino
 * 
 * Demonstrates the use of .setOnEndOfSongCallback(function_name_to_callback) function.
 * This function helps to realize melody playback of more than one song
 * (three songs exactly in this example).
 * 
 * Those three songs of Walker are:
 * Song 1: Catch Me If You Can by Alan Walker ft. Sorana
 * Song 2: Lovesick by Alan Walker ft. Sophie Simmons
 * Song 3: Unity by Alan Walker ft. The Walkers
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
  top.setOnEndOfSongCallback(onEndOfSong);  // The onEndOfSong() function will be
                                           // called everytime the end of song has reached.
  onEndOfSong();  // Call this function once to begin playback the first song.
}

void loop() {
  top.loop();  // Call this line inside the loop() function to keep the song playing.
}

void onEndOfSong() {
  static int songCounter;
  const uint8_t * songBuffer;
  uint16_t songSize;
  uint16_t songBPM;
  if (songCounter == 0) {
    songBuffer = SONG_1;
    songSize = sizeof(SONG_1);
    songBPM = SONG_1_BPM;
  }
  else if (songCounter == 1) {
    songBuffer = SONG_2;
    songSize = sizeof(SONG_2);
    songBPM = SONG_2_BPM;
  }
  else {
    songBuffer = SONG_3;
    songSize = sizeof(SONG_3);
    songBPM = SONG_3_BPM;
  }
  // SONG_1, SONG_2, & SONG_3 are declared in file "song.h"
  top.setSong(songBuffer, songSize, songBPM);
  top.play();
  songCounter = (songCounter + 1) % 3;
}
