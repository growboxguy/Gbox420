/*
 * 4_SerialEnhancedPlayer.ino
 * 
 * Exposes the capabilities of this library, from the callback functions
 * like in the previous example sketches, standard media player functions
 * (play, pause, continue, next track, previous track, and stop),
 * playback mode (single track, contionuous, and repeat), till the function
 * to changes the playback speed, also the function to shift the tone.
 * 
 * Single character command set (send via Serial panel):
 *    =  : Playback the current song
 *    ,  : Pause/continue play
 *    .  : Stops playback
 *    >  : Playback the next song
 *    <  : Playback the previous song
 *    +  : Increases the playback speed (max 2.00x)
 *    -  : Decreases the playback speed (min 0.50x)
 *    )  : Increases the volume (max 100)
 *    (  : Decreases the volume (min 0)
 *    ]  : Increases the tone shift by one pitch
 *    [  : Decreases the tone shift by one pitch
 *    *  : Toggles RGB LED indicator on/off
 *    c  : Toggles continous playback mode on/off
 *    r  : Toggles repeat playback mode on/off
 *    l  : Prints the playlist
 *    s  : Prints the all current states
 *    ?  or  h  : Prints help
 * 
 * Playlist:
 *    Song  1: Pirates of The Carribean by Alan Walker
 *    Song  2: Ignite by K-391 ft. Alan Walker, Julie Bergan, & Seungri
 *    Song  3: He's a Pirate - Live performance by D. Garrett (Composed By H. Zimmer & K. Badelt)
 *    Song  4: Hello World by Alan Walker ft. Torine
 *    Song  5: Catch Me If You Can by Alan Walker ft. Sorana
 *    Song  6: Lovesick by Alan Walker ft. Sophie Simmons
 *    Song  7: Unity by Alan Walker ft. The Walkers
 *    Song  8: The Spectre by Alan Walker
 *    Song  9: Alone by Alan Walker ft. Tove Stryrke
 *    Song 10: Creatures by Hot Shade ft. Nomi Bontegard
 * 
 * The circuit:
 *                                    _______________________________________________
 *                                   | Arduino Uno | Arduino Mega |        |         |
 *                                   |-------------|--------------| ESP32  | ESP8266 |
 *                                   | Norm | Mod  | Norm | Mode  |        |         |
 *                                   |------|------|------|-------|--------|---------|
 *     Buzzer |   + |----------------| 13   | 9    | 13   | 11    | GPIO4  | GPIO16  |
 *            |   - |----------------| GND  | GND  | GND  | GND   | GND    | GND     |
 *                                   |      |      |      |       |        |         |
 *            |   R |-----|R180|-----| 11   | 11   | 8    | 8     | GPIO19 | GPIO5   |
 *    RGB LED | Com |----------------| GND  | GND  | GND  | GND   | GND    | GND     |
 *            |   G |-----|R180|-----| 9    | 6    | 7    | 7     | GPIO18 | GPIO4   |
 *            |   B |-----|R100|-----| 3    | 5    | 6    | 6     | GPIO5  | GPIO14  |
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
#define RGB_RED_PIN                 19
#define RGB_GREEN_PIN               18
#define RGB_BLUE_PIN                5
#define RGB_PWM_FREQUENCY           1000
#define RGB_PWM_DUTY_RESOLUTION_BIT LEDC_TIMER_8_BIT
#define RGB_RED_PWM_CHANNEL         LEDC_CHANNEL_2
#define RGB_GREEN_PWM_CHANNEL       LEDC_CHANNEL_3
#define RGB_BLUE_PWM_CHANNEL        LEDC_CHANNEL_4
#define analogWrite                 ESP32_analog_write
#elif defined(ESP8266)
#define SPEAKER_PIN                 16
#define RGB_RED_PIN                 5
#define RGB_GREEN_PIN               4
#define RGB_BLUE_PIN                14
#elif defined(__AVR_ATmega328P__) and defined(USE_MODIFIED_TONE_GENERATOR)
//      SPEAKER_PIN must be pin     9  (OC1A)
#define RGB_RED_PIN                 11
#define RGB_GREEN_PIN               6
#define RGB_BLUE_PIN                5
#elif defined(__AVR_ATmega328P__)
#define SPEAKER_PIN                 13
#define RGB_RED_PIN                 11
#define RGB_GREEN_PIN               9
#define RGB_BLUE_PIN                3
#elif defined(__AVR_ATmega2560__) and defined(USE_MODIFIED_TONE_GENERATOR)
//      SPEAKER_PIN must be pin     11 (OC1A)
#define RGB_RED_PIN                 8
#define RGB_GREEN_PIN               7
#define RGB_BLUE_PIN                6
#elif defined(__AVR_ATmega2560__)
#define SPEAKER_PIN                 13
#define RGB_RED_PIN                 8
#define RGB_GREEN_PIN               7
#define RGB_BLUE_PIN                6
#endif  // ESP32

#if defined(ESP32) or defined(ESP8266)
#define pgm_read_pointer pgm_read_dword
#else
#define pgm_read_pointer pgm_read_word
#endif

const char SKETCH_TITLE[] PROGMEM = {
  "*************************************\n"
  "*                                   *\n"
  "*  Serial Advanced Player by ZulNs  *\n"
  "*                                   *\n"
  "*************************************\n"
};

const char HELP[] PROGMEM = {
  "\n"
  "Single character command set (send via Serial panel):\n"
  " =  : Plays the current song\n"
  " ,  : Pause/continue play\n"
  " .  : Stops playback\n"
  " >  : Playback the next song\n"
  " <  : Playback the previous song\n"
  " +  : Increases the playback speed (max 2.00x)\n"
  " -  : Decreases the playback speed (min 0.50x)\n"
#if defined(USE_MODIFIED_TONE_GENERATOR) or defined(ESP32)
  " )  : Increases the volume (max 100)\n"
  " (  : Decreases the volume (min 0)\n"
#endif
  " ]  : Increases the tone shift by one pitch\n"
  " [  : Decreases the tone shift by one pitch\n"
  " *  : Toggles RGB LED indicator on/off\n"
  " c  : Toggles continous playback mode on/off\n"
  " r  : Toggles repeat playback mode on/off\n"
  " l  : Prints the playlist\n"
  " s  : Prints the all current states\n"
  " ?  or  h  : Prints help (this page)\n"
};

FREQ_DATA_TYPE lowestFreq, highestFreq, freqRange;
uint16_t tempo;
float currentSpeed = 1.0;
int8_t currentSong, toneShift;
bool isPlaying, isPaused, isContinousMode, isRepeatMode, isDisplayIndicator;

#if defined(USE_MODIFIED_TONE_GENERATOR) or defined(ESP32)
uint8_t volume = 100;
#endif

#ifdef ESP32
ledc_timer_config_t   ledcTimer1, ledcTimer2;
ledc_channel_config_t redChannel, greenChannel, blueChannel;
#endif

#ifdef USE_MODIFIED_TONE_GENERATOR
TonePlayer top;
#else
TonePlayer top(SPEAKER_PIN);
#endif  // USE_MODIFIED_TONE_GENERATOR

void setup() {
#ifdef ESP32
  ledcTimer1.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledcTimer1.duty_resolution = RGB_PWM_DUTY_RESOLUTION_BIT;
  ledcTimer1.timer_num = LEDC_TIMER_1;
  ledcTimer1.freq_hz = RGB_PWM_FREQUENCY;
  
  ledcTimer2.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledcTimer2.duty_resolution = RGB_PWM_DUTY_RESOLUTION_BIT;
  ledcTimer2.timer_num = LEDC_TIMER_2;
  ledcTimer2.freq_hz = RGB_PWM_FREQUENCY;
  
  redChannel.gpio_num = RGB_RED_PIN;
  redChannel.speed_mode = LEDC_HIGH_SPEED_MODE;
  redChannel.channel = RGB_RED_PWM_CHANNEL;
  redChannel.intr_type = LEDC_INTR_DISABLE;
  redChannel.timer_sel = LEDC_TIMER_1;
  redChannel.hpoint = 0;
  
  greenChannel.gpio_num = RGB_GREEN_PIN;
  greenChannel.speed_mode = LEDC_HIGH_SPEED_MODE;
  greenChannel.channel = RGB_GREEN_PWM_CHANNEL;
  greenChannel.intr_type = LEDC_INTR_DISABLE;
  greenChannel.timer_sel = LEDC_TIMER_1;
  greenChannel.hpoint = 0;
  
  blueChannel.gpio_num = RGB_BLUE_PIN;
  blueChannel.speed_mode = LEDC_HIGH_SPEED_MODE;
  blueChannel.channel = RGB_BLUE_PWM_CHANNEL;
  blueChannel.intr_type = LEDC_INTR_DISABLE;
  blueChannel.timer_sel = LEDC_TIMER_2;
  blueChannel.hpoint = 0;
  
  ledc_timer_config(&ledcTimer1);
  ledc_timer_config(&ledcTimer2);
#endif
  
  top.setOnToneCallback(onTone);
  // The onTone() function will be called everytime a tone begins to sound.
  top.setOnMuteCallback(onMute);
  // While the onMute() function will be called when the buzzer begins to be muted. 
  top.setOnEndOfSongCallback(onEndOfSong);
  // The onEndOfSong() function will be called everytime the end of song has reached.
  
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println((const __FlashStringHelper *)SKETCH_TITLE);
  delay(1000);
  printHelp();
  delay(1000);
  printPlaylist();
  delay(1000);
  isDisplayIndicator = true;
  playSong();
  printStatus();
}

void loop() {
  top.loop();  // Call this line inside the loop() function to keep the song playing.
  checkSerialInput();
}

void onTone(FREQ_DATA_TYPE freq) {  // This function will be called by TonePlayer object with one
							                      // argument as the frequency of the recently sounded tone.
  if (!isDisplayIndicator) {
    return;
  }
  uint16_t norm;
  uint8_t  val, red, green, blue, grade;
  if (freq < lowestFreq) {
    freq = lowestFreq;
  }
  else if (freq > highestFreq) {
    freq = highestFreq;
  }
  freq -= lowestFreq;
  norm = freq / (float)freqRange * 5.0 * 255.0;
  // Uses 5.0 value instead of 6.0 to put the violet lights on top of the rainbow spectrum
  val = norm / 255;
  grade = norm % 255;
  red = green = blue = 0;
  if (bitRead(val, 0)) {
    grade = 255 - grade;
  }
  switch (val) {
    case 0:
      red   = 255;
      green = grade;
      break;
    case 1:
      red   = grade;
      green = 255;
      break;
    case 2:
      green = 255;
      blue  = grade;
      break;
    case 3:
      green = grade;
      blue  = 255;
      break;
    case 4:
      blue  = 255;
      red   = grade;
      break;
    case 5:
      blue  = grade;
      red   = 255;
      break;
  }
#ifdef ESP32
  redChannel.duty   = red;
  greenChannel.duty = green;
  blueChannel.duty  = blue;
  ledc_channel_config(&redChannel);
  ledc_channel_config(&greenChannel);
  ledc_channel_config(&blueChannel);
#else
  analogWrite(RGB_RED_PIN,   red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN,  blue);
#endif
}

void onMute() {
  if (!isDisplayIndicator) {
    return;
  }
#ifdef ESP32
  redChannel.duty   = 0;
  greenChannel.duty = 0;
  blueChannel.duty  = 0;
  ledc_channel_config(&redChannel);
  ledc_channel_config(&greenChannel);
  ledc_channel_config(&blueChannel);
#else
  analogWrite(RGB_RED_PIN,   0);
  analogWrite(RGB_GREEN_PIN, 0);
  analogWrite(RGB_BLUE_PIN,  0);
#endif
}

void onEndOfSong() {
  isPlaying = false;
  if (isContinousMode && (isRepeatMode || currentSong < NUMBER_OF_SONG - 1)) {
    currentSong = (currentSong + 1) % NUMBER_OF_SONG;
    playSong();
  }
  else if (isRepeatMode) {
    playSong();
  }
}

void playSong() {
  const uint8_t * songBuf = (const uint8_t *)pgm_read_pointer(SONG_BUFFER + currentSong);
  uint16_t noteSz = pgm_read_word(SONG_SIZE + currentSong);
  tempo = pgm_read_word(SONG_TEMPO + currentSong);
  top.setSong(songBuf, noteSz, tempo * currentSpeed);
  top.setRestFactor(20);  // Set to default rest factor
  top.play();
  initIndicator();
  isPlaying = true;
  isPaused = false;
  Serial.print(F("Playback "));
  printSongTitle(currentSong);
}

void initIndicator() {
  lowestFreq  = top.getFreqShift(pgm_read_byte_near(TONE_RANGE + currentSong * 2));
  highestFreq = top.getFreqShift(pgm_read_byte_near(TONE_RANGE + currentSong * 2 + 1));
  freqRange = highestFreq - lowestFreq;
}

void checkSerialInput() {
  uint8_t  chr;
  while (Serial.available()) {
    chr = Serial.read();
    if (isUpperCase(chr)) {
      chr += 0x20;  // Converts to lower case
    }
    switch (chr) {
      case '=':  // play
        if (!isPlaying) {
          playSong();
        }
        else if (isPaused) {
          top.cont();
          isPaused = false;
          printPauseCont();
        }
        break;
      case '.':  // stop
        top.stop();
        isPlaying = false;
        Serial.println(F("Stopped..."));
        break;
      case ',': // pause/continue
        if (isPlaying) {
          if (isPaused) {
            top.cont();
          }
          else {
            top.pause();
          }
          isPaused = !isPaused;
          printPauseCont();
        }
        break;
      case '>':
        currentSong = (currentSong + 1) % NUMBER_OF_SONG;
        playSong();
        break;
      case '<':
        currentSong = (currentSong - 1 + NUMBER_OF_SONG) % NUMBER_OF_SONG;
        playSong();
        break;
      case '+':
        if (currentSpeed < 2.0) {
          currentSpeed += (currentSpeed >= 1.0) ? 0.1 : 0.05;
          top.setTempo(tempo * currentSpeed);
          printSpeed();
        }
        break;
      case '-':
        if (currentSpeed > 0.5) {
          currentSpeed -= (currentSpeed > 1.0) ? 0.1 : 0.05;
          top.setTempo(tempo * currentSpeed);
          printSpeed();
        }
        break;
#if defined(USE_MODIFIED_TONE_GENERATOR) or defined(ESP32)
      case ')':
        ++volume;
        if (volume > 100) {
          volume = 100;
        }
        else {
          top.setVolume(volume / 100.0 * 255.0);
          printVolume();
        }
        break;
      case '(':
        --volume;
        if (volume > 100) {
          volume = 0;
        }
        else {
          top.setVolume(volume / 100.0 * 255.0);
          printVolume();
        }
        break;
#endif
      case ']':
        top.setToneShift(1);
        ++toneShift;
        initIndicator();
        printToneShift();
        break;
      case '[':
        top.setToneShift(-1);
        --toneShift;
        initIndicator();
        printToneShift();
        break;
      case '*':
        if (isDisplayIndicator) {
          onMute();
        }
        isDisplayIndicator = !isDisplayIndicator;
        printDisplayIndicatorStatus();
        break;
      case 'c':
        isContinousMode = !isContinousMode;
        printContinousModeStatus();
        break;
      case 'r':
        isRepeatMode = !isRepeatMode;
        printRepeatModeStatus();
        break;
      case 's':
        Serial.println();
        if (isPlaying) {
          if (isPaused) {
            Serial.print(F("Paused "));
          }
          else {
            Serial.print(F("Playback "));
          }
        }
        printSongTitle(currentSong);
        printStatus();
        break;
      case 'l':
        printPlaylist();
        break;
      case '?':
      case 'h':
        printHelp();
        break;
    }
  }
}

void printHelp() {
  Serial.println((const __FlashStringHelper *)HELP);
}

void printStatus() {
  printContinousModeStatus();
  printRepeatModeStatus();
  printDisplayIndicatorStatus();
  printSpeed();
#if defined(USE_MODIFIED_TONE_GENERATOR) or defined(ESP32)
  printVolume();
#endif
  printToneShift();
  Serial.println();
}

void printPlaylist() {
  Serial.println();
  Serial.println(F("Playlist:"));
  for (int8_t idx = 0; idx < NUMBER_OF_SONG; ++idx) {
    printSongTitle(idx);
  }
  Serial.println();
}

void printSongTitle(uint8_t songIdx) {
  Serial.print(F("Song "));
  Serial.print(songIdx + 1);
  Serial.print(F(": "));
  Serial.println((const __FlashStringHelper *)pgm_read_pointer(SONG_TITLE  + songIdx));
}

void printPauseCont() {
  if (isPaused) {
    Serial.println(F("Paused..."));
  }
  else {
    Serial.print(F("Continue playback "));
    printSongTitle(currentSong);
  }
}

void printSpeed() {
  Serial.print(F("Playback speed: "));
  Serial.print(currentSpeed);
  Serial.println(F("x"));
}

#if defined(USE_MODIFIED_TONE_GENERATOR) or defined(ESP32)
void printVolume() {
  Serial.print(F("Volume: "));
  Serial.println(volume);
}
#endif

void printToneShift() {
  Serial.print(F("Tone shift: "));
  Serial.println(toneShift);
}

void printContinousModeStatus() {
  printOnOffStatus(F("Continous playback mode"), isContinousMode);
}

void printRepeatModeStatus() {
  printOnOffStatus(F("Repeat playback mode"), isRepeatMode);
}

void printDisplayIndicatorStatus() {
  printOnOffStatus(F("Display indicator"), isDisplayIndicator);
}

void printOnOffStatus(const __FlashStringHelper * str, bool status) {
  Serial.print(str);
  Serial.print(F(": "));
  Serial.println(status ? F("ON") : F("OFF"));
}
