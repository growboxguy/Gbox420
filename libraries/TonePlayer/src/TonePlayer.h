/*
 * TonePlayer.h
 * 
 * A library for playing melody in background process using the builtin tone() function.
 * No needed for delay() function to waste the MCU processing power.
 * It parses melodic tones from PROGMEM to conserve the SRAM.
 * The frequency of each tone is computed instead of being stored as constant values.
 * Supports repeat to a specified number of tones till five levels of stack.
 * 
 * By ZulNs, @Gorontalo, December 2022
 */

#ifndef ARDUINO_TONE_PLAYER_H
#define ARDUINO_TONE_PLAYER_H

#include "Arduino.h"

#ifdef ESP32
#include "driver/ledc.h"
#endif

#ifdef USE_MODIFIED_TONE_GENERATOR
#if defined(__AVR_ATmega328P__) or defined(__AVR_ATmega2560__)
#define USE_AVR_TIMER1
#else
#undef USE_MODIFIED_TONE_GENERATOR
#endif  // __AVR_ATmega328P__
#endif  // USE_MODIFIED_TONE_GENERATOR

#define DEFAULT_BPM                  60
#define DEFAULT_REST_FACTOR          20    // of 100
#define STACK_LEVEL_LIMIT            5     // used by REPEAT command

#ifdef ESP32
#define TONE_PWM_CHANNEL             0
#define TONE_PWM_DUTY_RESOLUTION_BIT 9
#define DEFAULT_VOLUME               255
#endif  // ESP32

#ifdef USE_AVR_TIMER1
#define DEFAULT_VOLUME               255
#define FREQ_DATA_TYPE               float
#ifdef __AVR_ATmega328P__
#define OC1A_BIT_MAP DDB1
#elif defined(__AVR_ATmega2560__)
#define OC1A_BIT_MAP DDB5
#endif  // __AVR_ATmega328P__
#else
#define FREQ_DATA_TYPE               uint16_t
#endif  // USE_AVR_TIMER1

/***********************************************************************************************************************************/

enum NOTES {
  NC0, NCIS0, ND0, NDIS0, NE0, NF0, NFIS0, NG0, NGIS0, NA0, NAIS0, NB0,  // First   octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC1, NCIS1, ND1, NDIS1, NE1, NF1, NFIS1, NG1, NGIS1, NA1, NAIS1, NB1,  // First   octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC2, NCIS2, ND2, NDIS2, NE2, NF2, NFIS2, NG2, NGIS2, NA2, NAIS2, NB2,  // Second  octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC3, NCIS3, ND3, NDIS3, NE3, NF3, NFIS3, NG3, NGIS3, NA3, NAIS3, NB3,  // Third   octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC4, NCIS4, ND4, NDIS4, NE4, NF4, NFIS4, NG4, NGIS4, NA4, NAIS4, NB4,  // Fourth  octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC5, NCIS5, ND5, NDIS5, NE5, NF5, NFIS5, NG5, NGIS5, NA5, NAIS5, NB5,  // Fifth   octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC6, NCIS6, ND6, NDIS6, NE6, NF6, NFIS6, NG6, NGIS6, NA6, NAIS6, NB6,  // Sixth   octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC7, NCIS7, ND7, NDIS7, NE7, NF7, NFIS7, NG7, NGIS7, NA7, NAIS7, NB7,  // Seventh octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  NC8, NCIS8, ND8, NDIS8, NE8, NF8, NFIS8, NG8, NGIS8, NA8, NAIS8, NB8,  // Eighth  octave of note C, C#, D, D#, E, F, F#, G, G#, A, A#, and B
  REST,              // (also tone NC1...NB8) followed by duration*
  REST_FACTOR,       // followed by value (0 - 100)
  SHIFT_TONE,        // followed by tone difference value
  RESET_TONE_SHIFT,
  REPEAT,            // followed by lowByte(address), highByte(address), lowByte(size), highByte(size)
  LOOP,              // followed by tone, duration*, & repetition
  SEQUENCE_TONE,     // followed by initial tone, destination tone, tone shift, & duration*
  SWING_CENTER,      // followed by tone to swing, duration*, swing strength (1 - 100), number of step
  FETCH,             // internal code
};                   //
                     // *duration consists of one or three bytes.
                     // The tempo value is 60000 divide by BPM in milliseconds.
                     // If the duration expressed by one byte:
                     // * Negative value (-1 ... -128) means current tempo multiplied by 1/-value.
                     //   Has a range from 1/1 to 1/128 of current tempo.
                     // * Positive value (1 ... 127) means current tempo multiplied by value/10.
                     //   Has a range from 0.1 to 12.7 of current tempo.
                     // If the duration expressed by three bytes (b1, b2, b3):
                     // * b1 must be 0, then the tempo multiplied by b2, then divided by b3.
                     //   For example, the sequence (0, 3, 4) represents the tempo multiplied by 3/4.

/***********************************************************************************************************************************/

class TonePlayer {
public:
#ifdef USE_AVR_TIMER1
  TonePlayer();
  void setVolume(uint8_t vol = 0);
  void tone(float freq);
  void noTone();
#elif defined(ESP32)
  TonePlayer(uint8_t speaker_pin);
  void setSpeakerPin(uint8_t gpio_pin) { speakerPin = gpio_pin; }
  void setVolume(uint8_t vol = 0);
  void tone(uint8_t speaker_pin, uint16_t freq, uint8_t vol = DEFAULT_VOLUME);
  void noTone(void);
#else
  TonePlayer(uint8_t speaker_pin);
  void setSpeakerPin(uint8_t gpio_pin);
#endif   // USE_AVR_TIMER1
  void setSong(const uint8_t * ptr_song_buffer, uint16_t tone_size, uint16_t bpm = DEFAULT_BPM);
  void setOnEndOfSongCallback(void (*funct)(void)) { onEndOfSongCallback = funct; }
  void setOnToneCallback(void (*funct)(FREQ_DATA_TYPE freq)) { onToneCallback = funct; }
  void setOnMuteCallback(void (*funct)(void)) { onMuteCallback = funct; }
  void setTonePointer(uint16_t tone_pointer) { tonePointer = tone_pointer; }
  void setTempo(uint16_t bpm);
  void setRestFactor(uint8_t rest_factor = DEFAULT_REST_FACTOR);
  void setToneShift(int8_t tone_shift = 0);
  void play(uint16_t start_from_address = 0);
  void pause(void);
  void cont(void);
  void stop(void);
  void loop(void);
  FREQ_DATA_TYPE getFreq(int8_t pitch);
  FREQ_DATA_TYPE getFreqShift(int8_t pitch);

private:
#ifdef USE_AVR_TIMER1
  uint8_t volume;
  uint16_t onCompReg;
#elif defined(ESP32)
  uint8_t volume;
  uint8_t  speakerPin;
  ledc_timer_config_t ledcTimer;
  ledc_channel_config_t ledcChannel;
#else
  uint8_t  speakerPin;
#endif  // USE_AVR_TIMER1
  static void (*onEndOfSongCallback)(void);
  static void (*onToneCallback)(FREQ_DATA_TYPE freq);
  static void (*onMuteCallback)(void);
  void soundSpeaker(FREQ_DATA_TYPE freq, uint16_t duration);
  void muteSpeaker(uint16_t duration = 0);
  
  uint16_t getDuration(void);

  bool     isPlaying, isBuzzing;
  uint8_t  stackPointer, toneCode;
  const uint8_t * songBuffer;
  int8_t   toneShift;
  uint16_t toneSize, tonePointer, toneDuration, beatUnit, restFactor = DEFAULT_REST_FACTOR;
  uint32_t toneTimer;
  uint32_t stackBuffer[STACK_LEVEL_LIMIT];
};  // class TonePlayer

/***********************************************************************************************************************************/

void (*TonePlayer::onEndOfSongCallback)(void);
void (*TonePlayer::onToneCallback)(FREQ_DATA_TYPE freq);
void (*TonePlayer::onMuteCallback)(void);

#ifdef USE_AVR_TIMER1
TonePlayer::TonePlayer() {
  volume = DEFAULT_VOLUME;
}

void TonePlayer::setVolume(uint8_t vol) {
  if (isBuzzing) {
    OCR1A = (float)onCompReg * (volume / 255.0 / 2.0);
  }
  volume = vol;
}

void TonePlayer::tone(float freq) {
  float ocr;
  uint8_t psc;
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  ocr = F_CPU / freq;
  if ((uint32_t)(ocr-0.5) <= 0xFFFF) {
    psc = bit(CS10);   // IO-CLOCK/1 (no prescaling)
  }
  else {
    ocr /= 8.0;
    if ((uint32_t)(ocr-0.5) <= 0xFFFF) {
      psc = bit(CS11);  // IO-CLOCK/8 (from prescaler)
    }
    else {
      ocr /= 8.0;
      if ((uint32_t)(ocr-0.5) <= 0xFFFF) {
        psc = bit(CS11) | bit(CS10);  // IO-CLOCK/64 (from prescaler)
      }
      else {
        ocr /= 4.0;
        if ((uint32_t)(ocr-0.5) <= 0xFFFF) {
          psc = bit(CS12);  // IO-CLOCK/256 (from prescaler)
        }
        else {
          ocr /= 4.0;
          psc = bit(CS12) | bit(CS10);  // IO-CLOCK/1024 (from prescaler)
        }
      }
    }
  }
  
  onCompReg = (uint16_t)(ocr - 0.5);
  ICR1 = onCompReg;  // ICR1 as TOP value
  OCR1A = (float)onCompReg * (volume / 255.0 / 2.0);  // Compare match to define pulse width (duty cycle from 0 to 50% as volume from 0 to 100%)
  TCCR1A = bit(COM1A1) | bit(WGM11);  // Clear the OC1A on compare match, set at BOTTOM
  TCCR1B = bit(WGM13) | bit(WGM12) | psc;  // Mode 14 (fast PWM which ICR1 as TOP) and set prescaler
  DDRB |= bit(OC1A_BIT_MAP);  // Connect the OC1A to the port pin (pin 9 on UNO/NANO board, pin 11 on MEGA board)
}

void TonePlayer::noTone(void) {
  DDRB &= ~bit(OC1A_BIT_MAP);  // Disconnect the OC1A from the port pin (pin 9 on UNO/NANO board, pin 11 on MEGA board).
  TCCR1A = 0;
  TCCR1B = 0;
}
#elif defined(ESP32)
TonePlayer::TonePlayer(uint8_t speaker_pin) {
	speakerPin = speaker_pin;
  volume = DEFAULT_VOLUME;
  
  ledcTimer.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledcTimer.duty_resolution = LEDC_TIMER_9_BIT;
  ledcTimer.timer_num = LEDC_TIMER_0;
  
  ledcChannel.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledcChannel.channel = LEDC_CHANNEL_0;
  ledcChannel.intr_type = LEDC_INTR_DISABLE;
  ledcChannel.timer_sel = LEDC_TIMER_0;
  ledcChannel.hpoint = 0;
}

void TonePlayer::setVolume(uint8_t vol) {
  volume = vol;
  ledcChannel.duty = volume;
  if (isBuzzing) {
    ledc_channel_config(&ledcChannel);
  }
}

void TonePlayer::tone(uint8_t speaker_pin, uint16_t freq, uint8_t vol) {
  ledcTimer.freq_hz = freq;
  ledcChannel.gpio_num = speaker_pin;
  ledcChannel.duty = vol;
  ledc_timer_config(&ledcTimer);
  ledc_channel_config(&ledcChannel);
}

void TonePlayer::noTone() {
  ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
}
#else
TonePlayer::TonePlayer(uint8_t speaker_pin) {
	speakerPin = speaker_pin;
}
#endif  // USE_AVR_TIMER1

void TonePlayer::setSong(const uint8_t* ptr_song_buffer, uint16_t tone_size, uint16_t bpm) {
  if (isPlaying) {
    stop();
  }
  songBuffer = ptr_song_buffer;
  toneSize = tone_size;
  beatUnit = (bpm == 0) ? 0 : 60000 / bpm;
}

void TonePlayer::setTempo(uint16_t bpm) {
  beatUnit = (bpm == 0) ? 0 : 60000 / bpm;
  if (beatUnit == 0 && isPlaying) {
    stop();
  }
}

void TonePlayer::setRestFactor(uint8_t rest_factor) {
  restFactor = (rest_factor > 100) ? rest_factor % 100 : rest_factor;
}

void TonePlayer::setToneShift(int8_t tone_shift) {
  toneShift = (tone_shift != 0) ? toneShift + tone_shift : 0;
}

void TonePlayer::play(uint16_t start_from_address) {
  if (beatUnit) {
    tonePointer = start_from_address;
    stackPointer = 0;
    toneCode = FETCH;
    isPlaying = true;
  }
}

void TonePlayer::pause() {
  if (isBuzzing) {
    muteSpeaker();
  }
  isPlaying = false;
}

void TonePlayer:: cont() {
  if (tonePointer > 0) {
    isPlaying = true;
  }
}

void TonePlayer::stop() {
  if (isBuzzing) {
    muteSpeaker();
  }
  isPlaying = false;
}

void TonePlayer::loop() {
#ifdef USE_AVR_TIMER1
  static float freqFrom, currentFreq;
  static float freqStep;
  float freq, freqTo, freqRange;
#else
  static uint16_t freqFrom, currentFreq;
  static uint8_t freqStep;
  uint16_t freq, freqTo, freqRange;
#endif  // USE_AVR_TIMER1
  static uint16_t rest, eachToneDuration, eachRestDuration;
  static uint8_t repetition, stepCount, stepCtr;
  static int8_t toneFrom, toneStep, currentTone;
  uint16_t duration, toneCtr, toneSz;
  uint8_t  swingStrength, swingStep;
  int8_t toneTo;
  uint32_t val;
  if (isPlaying && millis() - toneTimer >= toneDuration) {
    switch (toneCode) {
      case FETCH:
        if (tonePointer >= toneSize) {
          if (stackPointer) {
            val = stackBuffer[--stackPointer];
            toneSize    = val & 0xFFFF;
            tonePointer = val >> 16;
          }
          else {
            isPlaying = false;
            if (onEndOfSongCallback) {
              onEndOfSongCallback();
            }
          }
        }
        else {
          toneCode = pgm_read_byte(songBuffer + tonePointer++);
          switch (toneCode) {
            case NC1 ... NB8:
              freq     = getFreqShift(toneCode);
              duration = getDuration();
              rest     = (uint32_t)duration * restFactor / 100;
              soundSpeaker(freq, duration - rest);
              break;
            case REST:
              rest = getDuration();
              muteSpeaker(rest);
              toneCode = FETCH;
              break;
            case REST_FACTOR:
              restFactor = pgm_read_byte(songBuffer + tonePointer++);
              if (restFactor > 100) {
                restFactor %= 100;
              }
              toneCode = FETCH;
              toneDuration = 0;
              break;
            case SHIFT_TONE:
              toneShift += pgm_read_byte(songBuffer + tonePointer++);
              toneCode = FETCH;
              toneDuration = 0;
              break;
            case RESET_TONE_SHIFT:
              toneShift = 0;
              toneCode = FETCH;
              toneDuration = 0;
              break;
            case REPEAT:
              toneCtr  =           pgm_read_byte(songBuffer + tonePointer++);
              toneCtr += (uint16_t)pgm_read_byte(songBuffer + tonePointer++) << 8;
              toneSz   =           pgm_read_byte(songBuffer + tonePointer++);
              toneSz  += (uint16_t)pgm_read_byte(songBuffer + tonePointer++) << 8;
              toneSz  += toneCtr;
              if (stackPointer < STACK_LEVEL_LIMIT) {
                stackBuffer[stackPointer++] = ((uint32_t)tonePointer << 16) + toneSize;
                tonePointer = toneCtr;
                toneSize    = toneSz;
              }
              toneCode = FETCH;
              toneDuration = 0;
              break;
            case LOOP:
              currentFreq = getFreqShift(pgm_read_byte(songBuffer + tonePointer++));
              duration    = getDuration();
              repetition  =              pgm_read_byte(songBuffer + tonePointer++);
              eachRestDuration = (uint32_t)duration * restFactor / 100;
              eachToneDuration = duration - eachRestDuration;
              soundSpeaker(currentFreq, eachToneDuration);
              break;
            case SEQUENCE_TONE:
              toneFrom   = pgm_read_byte(songBuffer + tonePointer++);
              toneTo     = pgm_read_byte(songBuffer + tonePointer++);
              toneStep   = pgm_read_byte(songBuffer + tonePointer++);
              duration   = getDuration();
              rest = (uint32_t)duration * restFactor / 100;
              eachToneDuration = duration - rest;
              if (toneStep != 0) {
                if (toneStep < 0) {
                  toneStep = -toneStep;
                }
                stepCount = (toneFrom > toneTo) ? toneFrom - toneTo : toneTo - toneFrom;
                stepCount++;
                stepCount /= toneStep;
                if (toneFrom > toneTo) {
                  toneStep = -toneStep;
                }
                currentTone = toneFrom;
                stepCtr = stepCount;
                eachRestDuration = rest;
              }
              else {
                toneCode = toneFrom;  // converts to regular tone
              }
              soundSpeaker(getFreqShift(currentTone), eachToneDuration);
              break;
            case SWING_CENTER:
              currentTone   = pgm_read_byte(songBuffer + tonePointer++);
              duration = getDuration();
              swingStrength = pgm_read_byte(songBuffer + tonePointer++);
              stepCount     = pgm_read_byte(songBuffer + tonePointer++);
              rest = (uint32_t)duration * restFactor / 100;
              eachToneDuration = duration - rest;
              freq = getFreqShift(currentTone);
              if (stepCount > 1 && swingStrength > 0) {
                if (swingStrength > 100) {
                  swingStrength %= 100;
                }
                freqRange = (getFreqShift(currentTone + 1) - freq) * swingStrength / 50;
                currentFreq = freqFrom = freq - freqRange / 2;
                freqTo   = freqFrom + freqRange;
                freqStep = freqRange / (stepCount - 1);
                eachToneDuration /= stepCount;
                if (eachToneDuration > 0 && freqStep > 0) {
                  rest = duration - eachToneDuration * stepCount;
                  stepCtr = stepCount;
                }
                else {
                  currentFreq = freq;
                  toneCode = currentTone;  // converts to regular tone
                  eachToneDuration = duration - rest;
                }
              }
              else {
                currentFreq = freq;
                toneCode = currentTone;  // converts to regular tone
              }
              soundSpeaker(currentFreq, eachToneDuration);
              break;
          }
        }
        break;
      case NC1 ... NB8:
        muteSpeaker(rest);
        toneCode = FETCH;
        break;
      case LOOP:
        if (!isBuzzing || eachRestDuration == 0) {
          if (--repetition == 0) {
            if (isBuzzing) {
              muteSpeaker();
            }
            toneCode = FETCH;
          }
          else {
            soundSpeaker(currentFreq, eachToneDuration);
          }
        }
        else {
          muteSpeaker(eachRestDuration);
        }
        break;
      case SEQUENCE_TONE:
        if (!isBuzzing || eachRestDuration == 0) {
          if (--stepCtr == 0) {
            if (isBuzzing) {
              muteSpeaker();
            }
            toneCode = FETCH;
            break;
          }
          else {
            currentTone += toneStep;
            soundSpeaker(getFreqShift(currentTone), eachToneDuration);
          }
        }
        else {
          muteSpeaker(eachRestDuration);
        }
        break;
      case SWING_CENTER:
        if (--stepCtr == 0) {
          muteSpeaker(rest);
          toneCode = FETCH;
        }
        else {
          currentFreq += freqStep;
          soundSpeaker(currentFreq, eachToneDuration);
        }
        break;
    }
  }
}

FREQ_DATA_TYPE TonePlayer::getFreq(int8_t pitch) {
  float freq = pow(1.059463, (float)pitch - 57.0) * 440.0;
#if FREQ_DATA_TYPE == flost
  return freq;
#else
  return (uint16_t)(freq + 0.5);
#endif
}

FREQ_DATA_TYPE TonePlayer::getFreqShift(int8_t pitch) {
  return getFreq(pitch + toneShift);
}

uint16_t TonePlayer::getDuration(void) {
  uint16_t duration;
  uint8_t  mul, div;
  int8_t   val = pgm_read_byte(songBuffer + tonePointer++);
  if (val < 0) {
    duration = beatUnit / -val;
  }
  else if (val > 0) {
    duration = (uint32_t)beatUnit * val / 10;
  }
  else {  // val == 0
    mul = pgm_read_byte(songBuffer + tonePointer++);
    div = pgm_read_byte(songBuffer + tonePointer++);
    if (div == 0) {
      div = 255;
    }
    duration = (uint32_t)beatUnit * mul / div;
  }
  return duration;
}

void TonePlayer::soundSpeaker(FREQ_DATA_TYPE freq, uint16_t duration) {
#ifdef USE_AVR_TIMER1
  TonePlayer::tone(freq);
#elif defined(ESP32)
  TonePlayer::tone(speakerPin, freq, volume);
#else
  if (isBuzzing) {
    noTone(speakerPin);
  }
  tone(speakerPin, freq);//, duration);
#endif  // USE_AVR_TIMER1
  toneTimer = millis();
  toneDuration = duration;
  isBuzzing = true;
  if (onToneCallback) {
    onToneCallback(freq);
  }
}

void TonePlayer::muteSpeaker(uint16_t duration) {
#ifdef USE_AVR_TIMER1
  TonePlayer::noTone();
#elif defined(ESP32)
  TonePlayer::noTone();
#else
  noTone(speakerPin);
#endif  // USE_AVR_TIMER1
  toneTimer = millis();
  toneDuration = duration;
  isBuzzing = false;
  if (onMuteCallback) {
    onMuteCallback();
  }
}

#endif   // ARDUINO_TONE_PLAYER_H