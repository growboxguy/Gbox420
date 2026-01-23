/*
 * 3_RainbowLightsOfTheSpectre.ino
 * 
 * Demonstrates the use of:
 *    -  .setOnToneCallback(function_name_to_callback) function.
 *       The callback function must provide one parameter as the
 *        frequency of the recently sounded tone.
 *    -  .setOnMuteCallback(function_name_to_callback) function.
 * 
 * The above two functions, together with the hardware of an RGB LED
 * driven by three PWM pins, realize the light indicator in the rainbow
 * light spectrum corresponding to the frequency of sounding tone.
 * The lower the frequency, the more towards the red color of the
 * rainbow spectrum the light is.Converselly, the higher the frequency,
 * the more towards the purple color the light is.
 * 
 * The song title: The Spectre by Alan Walker
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

#ifdef USE_MODIFIED_TONE_GENERATOR
TonePlayer top;
#else
TonePlayer top(SPEAKER_PIN);
#endif  // USE_MODIFIED_TONE_GENERATOR

#ifdef ESP32
ledc_timer_config_t   ledcTimer1, ledcTimer2;
ledc_channel_config_t redChannel, greenChannel, blueChannel;
#endif

FREQ_DATA_TYPE lowestFreq, highestFreq, freqRange;

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
  
  lowestFreq  = top.getFreq(LOWEST_TONE);
  highestFreq = top.getFreq(HIGHEST_TONE);
  // Both LOWEST_TONE & HIGHEST_TONE are defined in file "song.h"
  freqRange = highestFreq - lowestFreq;
  top.setOnToneCallback(onTone);         // The onTone() function will be called everytime a tone begins to sound.
  top.setOnMuteCallback(onMute);         // While the onMute() function wiil be called when the buzzer begins to be muted. 
  top.setSong(SONG, sizeof(SONG), BPM);  // Set the song data, note: it must be in PROGMEM area.
                                         // Song buffer SONG is declared in file "song.h"
  top.play();  // Begins playback a song.
}

void loop() {
  top.loop();  // Call this line inside the loop() function to keep the song playing.
}

void onTone(FREQ_DATA_TYPE freq) {  // This function will be called by TonePlayer object with one
      							                // argument as the frequency of the recently sounded tone.
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
