// Required by the 2_PlaybackMultipleWalkers.ino example sketch.


#include <TonePlayer.h>


/*****************************************************************************************/
// Song 1: Catch Me If You Can by Alan Walker ft. Sorana

#define SONG_1_BPM 147

const uint8_t SONG_1[] PROGMEM = {
  NA5, 35, NAIS5, 35, NG5, 35, NA5, 35,
  
  // Address 8
  NA4, -1,
  // Address 10
  NF5, -1, NE5, -1, NF5, -2, NAIS4, 15,
  NF5, -1, NE5, -1, NF5, -2, NB4,   15,
  NF5, -1, NE5, -1, NF5, -2, NA4,   15,
  REPEAT, 8, 0, 24, 0,       NG5,   15,
  NF5, -1, NE5, -1, NG5, -1, NF5,   -2, NE5, 25,

  REPEAT, 8, 0, 43, 0,
  
  // Address 56
  NA5, -1, ND5, -2, NAIS5, 15, NAIS5, -2, NA5,   -2, NG5,   15, REST, -2,
  NG5, -1, NF5, -2, NA5,   15, NA5,   -2, NA5,   -2, NA5,   -2, NG5,  -2, NF5, 15, REST, -2,
  NA5, -1, NC6, -2, NAIS5, 15, NAIS5, -2, NAIS5, -2, NAIS5, -2, NA5,  -2, NG5, 15, REST, -2,
  // Address 106
  NA5, -2, NG5, -2, NA5,   -2, NG5,   -2, NA5,   -1, NA5,   -2, NA5,  -2, NF6, -2, NE6,  -1, ND6, 15,
  
  // Address 126
  NG5, -2, NA5, -2, NF6,   -2, NE6,   -1, ND6,   15,
  NG5, -2, NA5, -2, NAIS5, -2, NC6,   -1, NAIS5, 15,
  NF5, -2, NG5, -2, NA5,   -2, NAIS5, -1, NA5,   -1, NAIS5, -2, NA5, -1,
  NA5, -2, NA5, -2, NF6,   -2, NE6,   -1, ND6,   15,
  
  REPEAT, 126, 0, 44, 0,
  
  REPEAT, 8, 0, 43, 0,

  REPEAT, 56, 0, 119, 0,
  
  // Address 185
  NA5, -1,
  // Address 187
  ND5, -1, ND5, -1, NG5, -2, NF5, -2, NE5, -1,
  REPEAT, 187, 0, 10, 0,
  REPEAT, 185, 0, 12, 0,
  ND6, -1, ND6, -1,
  
  REPEAT, 185, 0, 26, 0,
  
  REPEAT, 56, 0, 119, 0,
  
  REPEAT, 126, 0, 30, 0,

  REPEAT, 106, 0, 20, 0
};


/*****************************************************************************************/
// Song 2: Lovesick by Alan Walker ft. Sophie Simmons

#define SONG_2_BPM 123

const uint8_t SONG_2[] PROGMEM = {
  // Address 0
  ND5, -2, NE5, 15, NA5, -2, NC6, 15, NA5, -2, NGIS5, 15, NB5, -2, NA5, 15,
  // Address 16 x24
  NE5, -2, NF5, 15, ND5, -2, NE5, 15, NC5, -2, NB4,   15, NG4, -2, NA4, 15,
  
  // Address 32
  NC5, -2, NF5, -2, NF5, -2, NE5, -2, NC5, -2, ND5, -2, ND5, -2, ND5, -2,
  NB4, -2, NC5, -2, NC5, -2, NB4, -2, NC5, -2, NB4, -2, NA4, -2, NA4, -2,
  REPEAT, 32, 0, 24, 0,               NG4, -2, NA4, 15,
  REPEAT, 32, 0, 32, 0,
  NC5, -2, NF5, -2, NF5, -2, NF5, -2, NG5, -2, NF5, -2, NE5, -2, NE5, -2,
  NA4, -2, NC5, -2, NC5, -2, NB4, -2, NG4, -2, NA4, 15,
  
  // Address 106
  REPEAT, 0, 0, 16, 0,
  REPEAT, 16, 0, 12, 0,                                   ND5, -2, NC5, 15,
  NE5, -2, REPEAT, 2, 0, 14, 0,
  // Address 127
  NE6, -2, NF6, 15, ND6, -2, NE6, 15, NC6, -2, NB5, 15, NG5, -2, NA5, 15,
  // Address 143 x173
  NF6, 20, NE6, 20, NB5, 20, NC6, 20,
  NF6, 20, NE6, -2, NF6, -2, NE6, -2, ND6, -2, NC6, -2, NB5, 20,
  
  REPEAT, 32, 0, 41, 0,
  
  NG5, -2,
  NC6, -1, NC6, 15,
  // Address 176
  NC6, -2, NB5, -2, NA5, -2, NG5, -2, NG5, -2,
  ND6, -1, ND6, 15,
  REPEAT, 176, 0, 10, 0,
  NE6, -1, NE6, 15,
  NE6, -2, NF6, -2, NE6, -2, ND6, -2, NC6, -2, NB5, 15, REST, -1,
  
  REPEAT, 106, 0, 37, 0,
  
  // Address 218
  REPEAT, 0, 0, 16, 0,
  NF5, 20, NE5, 20, NB4, 20, NA4, 20,
  
  REPEAT, 218, 0, 13, 0,

  REPEAT, 218, 0, 13, 0,
  
  REPEAT, 0, 0, 16, 0,
  REPEAT, 127, 0, 16, 0,

  REPEAT, 106, 0, 37, 0
};


/*****************************************************************************************/
// Song 3: Unity by Alan Walker ft. The Walkers

#define SONG_3_BPM 115

const uint8_t SONG_3[] PROGMEM = {
  NB5, 30, NGIS5, 30, NE6, -2, REST, -2, NDIS6, 30, NAIS5, 30,
  
  // Address 12
  NDIS5, -2, NB5,   -1, NAIS5, -2, NGIS5, -1, NFIS5, -2, NGIS5, -1, NFIS5, -2, NDIS5, -1,
  NDIS5, -2, NFIS5, -1, NDIS5, -2, NCIS5, -1, NB4,   -2, NCIS5, -1, NB4,   -2, NGIS4, -1,
  REPEAT, 12, 0, 18, 0,
             NB5,   -1, NAIS5, -2, NAIS5, -1, NGIS5, -2, NB5,   -1, NAIS5, -2, NGIS5, -1,
  
  // Address 63
  LOOP, NGIS5, 4, 4,
  NGIS5, 8, NFIS5, 8, NB4, 8, NFIS5, 8, NFIS5, 16,
  LOOP, NDIS5, 4, 4,                    NCIS5,  8,
  NCIS5, 4, NCIS5, 4, NE5, 4, NDIS5, 4, NB4,    4, NGIS4, 8, REST, 8,
  
  REPEAT, 63, 0, 30, 0,                            NGIS5, 8, REST, 8,
  
  // Address 106
  NB5,   4, NAIS5, 4, NGIS5, 4, NFIS5, 4, NGIS5, 16,
  NFIS5, 4, NE5,   4, NDIS5, 4, NCIS5, 4, NDIS5, 12,
  NDIS5, 4, NDIS5, 8, NFIS5, 4, NFIS5, 8,
  // Address 134
  NAIS4, 4, NB4,   4, NCIS5, 4, NB4,   4, NGIS4, 12, REST, 4,
  
  REPEAT, 106, 0, 28, 0,
  SHIFT_TONE, 12, REPEAT, 134, 0, 12, 0, SHIFT_TONE, -12
};
