// Required by the 3_RainbowLightsOfTheSpectre.ino example sketch.


#include <TonePlayer.h>


// Song title: The Spectre by Alan Walker

#define BPM 128

const uint8_t SONG[] PROGMEM = {
  NE5, -1, NGIS5, -1, NE6,   20, NB5,   -1, NCIS6, -1, NGIS5, 20,
  NE5, -1, NFIS5, -1, NDIS5, 20, NCIS5, -1, NE5,   -1, NCIS5, 20,
  
  // Address 24
  NGIS5, -2, NGIS5, -2, NB5,   -2, NCIS6, 15, NCIS6, -1, NB5, -1, NGIS5, 15, NFIS5, 15,
  NDIS5, -1, NDIS5, -2, NCIS5, -1, NB4,   -1, NGIS4, 15,
  REPEAT, 24, 0, 16, 0,
  NCIS5, -2, NDIS5, -1, NDIS5, -2, NFIS5, -1, NDIS5, -2, NE5, 15,
  
  // Address 67
  NGIS5, -2, NE6,   -2, NDIS6, -2, NCIS6, -2, NB5,   -2,
  NCIS6, 20, NB5,   15, NGIS5, 20,
  NGIS5, -2, NFIS5, -1, NFIS5, -2, NA5,   -1, NFIS5, -2, NGIS5, 15,
  REPEAT, 67, 0, 20, 0,
  // Address 100
                        NE5,   -2, NDIS5, -1, NB4,   -2, NCIS5, 15,
  
  // Address 108
  LOOP, NGIS5, -2, 3,   NGIS5, -1,
                        NFIS5, -2, NE5,   -1, NB5,   -2, NB5,   15,
  NE5,   -2, NFIS5, -1, NE5,   -2, NFIS5, -1, NA5,   -2, NGIS5, 15,
  REPEAT, 108, 0, 14, 0,
  // Address 139
  NGIS5, -2, NFIS5, -1, REPEAT, 100, 0, 8, 0,
  
  // Address 148
  NB4,   -2, NCIS5, -1,
  NE5,   -2, NGIS5, -2, NE6,   -1,
  NE6,   -4, NDIS6, -4, NB5,   -2, NCIS6, -1,
  NE6,   -2, NCIS6, -2, NA5,   20,
  NGIS5, -1,
  NGIS5, -2, NFIS5, -2, NE5,   -1,
  NCIS6, -3, NA5,   -4, NGIS5, -4, NE5,   -4, NFIS5, -1,
  NE5,   -2, NFIS5, -2, NDIS5, 15,
  
  REPEAT, 148, 0, 42, 0,
  REST_FACTOR, 50,
  LOOP, NFIS5, 7, 3, NGIS5, 7, NE5, 7, NE5, 7, NDIS5, 7, NDIS5, 7, NCIS5, 17,
  REST_FACTOR, 20,
  
  REPEAT, 148, 0, 48, 0,
  
  REPEAT, 148, 0, 48, 0,
  
  REPEAT, 24, 0, 84, 0,
  
  REST_FACTOR, 50,
  // Address 238
  LOOP, NGIS5, -2, 5, NFIS5, -2, NE5, -2, NE5, -2, LOOP, NB5, -2, 4,
  NE5, -2, NFIS5, -2, NFIS5, -2, NE5, -2, NFIS5, -2, NFIS5, -2, NA5, -2, LOOP, NGIS5, -2, 3,
  REPEAT, 238, 0, 14, 0,
  REST_FACTOR, 20,
  REPEAT, 139, 0, 57, 0,
  REPEAT, 148, 0, 42, 0,
  REPEAT, 100, 0, 8, 0,
  REPEAT, 148, 0, 10, 0,
  REPEAT, 148, 0, 10, 0,
  // Address 302 (46, 1)
  REPEAT, 152, 0, 6, 0,
  REPEAT, 152, 0, 6, 0,
  REPEAT, 152, 0, 6, 0,
  REPEAT, 46, 1, 10, 0,
  // Address 322 (66, 1)
  NGIS5, -2, NE6,   -1,
  NGIS5, -2, NE6,   -1,
  NGIS5, -2, NE6,   -1,
  REPEAT, 66, 1, 8, 0,
  
  REPEAT, 148, 0, 48, 0,
  
  REPEAT, 148, 0, 48, 0,

  NE5, -2, NDIS6, -2, NB5, -2, NCIS6, -1,
  // Address 357 (101, 1)
  NB5,   -2, NGIS5, -2, NGIS6, -1,
  NB5,   -4, NGIS5, -4, NB5,   -4, NGIS5, -4, NE6, -1,
  NE6,   -2, NFIS6, -2, NCIS6, 20,
  NB5,   -1,
  NE6,   -2, NFIS6, -2, NDIS6, -1,
  NGIS6, -4, NFIS6, -4, NE6,   -4, NDIS6, -4, NE6, -1,
  NE6,   -2, NFIS6, -2, NDIS6, 15,
  
  REPEAT, 101, 1, 44, 0, NDIS6, 40
};

#define LOWEST_TONE  NB4
#define HIGHEST_TONE NGIS6
