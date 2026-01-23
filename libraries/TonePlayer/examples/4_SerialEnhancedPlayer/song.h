// Required by the 4_SerialEnhancedPlayer.ino example sketch.


#include <TonePlayer.h>

#define NUMBER_OF_SONG 10
// Song  1: Pirates of The Carribean by Alan Walker
// Song  2: Ignite by K-391 ft. Alan Walker, Julie Bergan, & Seungri
// Song  3: He's a Pirate - Live performance by D. Garrett (Composed By H. Zimmer & K. Badelt)
// Song  4: Hello World by Alan Walker ft. Torine
// Song  5: Catch Me If You Can by Alan Walker ft. Sorana
// Song  6: Lovesick by Alan Walker ft. Sophie Simmons
// Song  7: Unity by Alan Walker ft. The Walkers
// Song  8: The Spectre by Alan Walker
// Song  9: Alone by Alan Walker ft. Tove Stryrke
// Song 10: Creatures by Hot Shade ft. Nomi Bontegard


/*****************************************************************************************/
// Song 1: Pirates of The Carribean by Alan Walker
const uint8_t SONG_1[] PROGMEM = {
  REST_FACTOR, 50,
  ND6, -4, NG6, -4,
  // Address 6
  NA6, 20, NAIS6, 20, LOOP, NA6, -2, 4, NG6, 20,
  REST_FACTOR, 0,
  SEQUENCE_TONE, ND4, NA6, 1, -4,
  
  // Address 23
  REST_FACTOR, 50,
  // Address 25
  REPEAT, 6, 0, 10, 0,
  NG6, 20, NF6, 20, NE6, 6, NF6, 6, NE6, 6, ND6, 20, REST, -1,
  
  REPEAT, 25, 0, 19, 0,
  
  // Address 49
  ND5,    8, NF5,   -4,
  NA5,   -2, NA5,   -2, NA5, -2, NF5, -2,
  NAIS5, -2, NAIS5, -2, NA5, -2, NG5, -2, NA5, -1, REST, -1,
  REPEAT, 49, 0, 12, 0,
  NG5,   -2, NF5,   -2, NE5, -2, NF5, -2, ND5, -1, REST, -1,
  
  REPEAT, 49, 0, 12, 0,
  REPEAT, 49, 0, 12, 0,
  REPEAT, 49, 0,  8, 0,
  REPEAT, 49, 0,  8, 0,
  
  // Address 110
  REPEAT, 49, 0, 41, 0,
  
  REPEAT, 49, 0, 24, 0,
  NG5,  8, NG5, -4, NG5, -2, NA5, -2, NF5, -2,
  NF5, -4, NG5, -4, NF5, -2, ND5, -2, NE5, -2,
  NE5, -4, NE5, -4, NF5, -2, NE5, -2, ND5, -1, REST, -1,
  
  REPEAT, 110, 0, 42, 0,
  
  REPEAT, 25, 0, 24, 0,
  
  REPEAT, 25, 0, 132, 0,
  REST_FACTOR, 20
};


/*****************************************************************************************/
// Song 2: Ignite by K-391 ft. Alan Walker, Julie Bergan, & Seungri
const uint8_t SONG_2[] PROGMEM = {
  REST_FACTOR, 50,
  LOOP, NGIS5, -1, 4,
  LOOP, NFIS5, -1, 4,
  LOOP, NE5,   -1, 4,
  LOOP, NGIS5, -1, 4, REST, -1,
  REST_FACTOR, 20,
  
  // Address 22
  NGIS5, -3, NAIS5, -3, NB5,   -1, REST,  -1,
  NB5,   -3, NAIS5, -3, NGIS5, -3, NB5,   -3, NAIS5, -1, NFIS5, -1, REST,  -1,
  NCIS6, -3, NB5,   -3, NAIS5, -3, NGIS5, -3, NB5,   -1, REST,  -1,
  NCIS6, -3, NB5,   -3, NAIS5, -3, NGIS5, -3, NB5,   -1, NB5,   -3, NAIS5, -1, REST, -1,
  
  REPEAT, 22, 0, 50, 0,
  
  // Address 77
  NGIS5, -4, NAIS5, -4, NB5,   -2, NB5,   -2, NAIS5, -2, NAIS5, -2,
  LOOP,      NGIS5, -4, 3,         NGIS5, -2, REST,  -4,
  NGIS5, -4, NFIS5, -4, NGIS5, -2, NGIS5, -2, NAIS5, -2, NAIS5, -2,
  LOOP,      NB5,   -4, 3,         NB5,   -2, REST,  -4,
  // Address 117
  REPEAT, 77, 0, 20, 0,
  // Address 122
  NDIS6, -2, NCIS6, -2, NB5,   -2, NAIS5, -2, NFIS5, -2, NGIS5, -2, REST, -2,
  
  REPEAT, 77, 0, 59, 0,
  
  // Address 141
  NFIS5, -4, NGIS5, -4, NAIS5, -4,
  REST_FACTOR, 50,
  NB5,   -2, NB5,   -2, NAIS5, -2, NAIS5, -2, NGIS5, -2, NGIS5, -2, REST, -2,
  REST_FACTOR, 20,
  NGIS5, -4, NFIS5, -4,
  REST_FACTOR, 50,
  NGIS5, -2, NGIS5, -2, NAIS5, -2, NAIS5, -2, NB5,   -2, NB5,   -2, REST, -2,
  REST_FACTOR, 20,
  REPEAT, 141, 0, 22, 0,
  REPEAT, 122, 0, 14, 0,
  REST_FACTOR, 20,
  
  REPEAT, 141, 0, 58, 0,
  
  REPEAT, 22, 0, 182, 0,
  
  // Address 209
  NB4,   -3, REST, -3,
  LOOP,      NB4,   -3, 4,         REST,  -3,
  NGIS4, -3, NAIS4, -3, NB4,   -3, NAIS4, -3, REST, -3,
  NAIS4, -3, NAIS4, -3, NB4,   -3, NAIS4, -3, REST, -3,
  NFIS4, -3, NGIS4, -3, REST,  -3,
  LOOP,      NGIS4, -3, 4,         REST,  -3,
  NDIS4, -3, NE4,   -3, NFIS4, -3, NGIS4, -3, REST, -3,
  NGIS4, -3, NAIS4, -3, NB4,   -3, NAIS4, -3, REST,  7,
  
  // Address 271
  NGIS4, -3, NAIS4, -3, NB4,   -3, REST,  -3,
  LOOP,      NB4,   -3, 4,         REST,  -3,
  NAIS4, -3, NAIS4, -3, NB4,   -3, NDIS5, -3, REST, -3,
  NAIS4, -3, NAIS4, -3, NB4,   -3, NAIS4, -3, REST, -3,
  NGIS4, -3, NAIS4, -3, NB4,   -3, REST,  -3,
  NGIS4, -3, NFIS4, -3, NGIS4, -3, REST,  -3,
  NGIS4, -3, NAIS4, -3, NB4,   -3, REST,  -3,
  NB4,   -3, REST,  -3, NAIS4, -3, REST,   6,
  
  // Address 337 (81, 1)
  NGIS4, -4, NAIS4, -4, NB4,   -2, NB4,   -2, NAIS4, -2, NAIS4, -2,
  LOOP,      NGIS4, -4, 3,         NGIS4, -2, REST,  -4,
  NGIS4, -4, NFIS4, -4, NGIS4, -2, NGIS4, -2, NAIS4, -2, NAIS4, -2,
  LOOP,      NB4,   -4, 3,         NB4,   -2, REST,  -4,
  REPEAT, 81, 1, 20, 0,
  NDIS5, -2, NCIS5, -2, NB4,   -2, NAIS4, -2, NFIS4, -2, NGIS4, -2, REST, -1,
  
  // Address 396
  NB4,   -3, NCIS5, -3, NDIS5, -3, REST, -3,
  NDIS5, -3, REST,  -3, NDIS5, -3, REST, -3, NDIS5, -3, REST, -3, NE5,   -3, REST, 13,
  NE5,   -3, NDIS5, -3, NCIS5, -3, REST, -3,
  NCIS5, -3, REST,  -3, NCIS5, -3, REST, -3, NB4,   -3, REST, -3, NDIS5, -3, REST, -3,
  NFIS5, -4, REPEAT, 117, 0, 19, 0,
  
  REPEAT, 141, 0, 56, 0,
  NAIS5, -2, NB5,   -2, NB5,   -2, NAIS5, -2, NFIS5, -2, NGIS5, -2, REST, -2,
  NAIS5, -2, NB5,   -2, NB5,   -2, NCIS6, -2, NB5,   -2, NDIS6, -2, REST, -2,
  NDIS6, -2, NDIS6, -2, NDIS6, -2, NCIS6, -2, NAIS5, -2, NB5,   -2, REST, -2,
  REPEAT, 122, 0, 14, 0,
  REST_FACTOR, 20
};


/*****************************************************************************************/
// Song 3: He's a Pirate - Live performance by D. Garrett (Composed By H. Zimmer & K. Badelt)
const uint8_t SONG_3[] PROGMEM = {
  NE5, -1, NG5, -1, NA5, 40, NC5, 20, ND5, 40,
  NE5, -1, NF5, 40, NE5, 20, ND5, 40,
  NE5, -1, NF5, 40, NF5, -1, NG5, 40, ND5, -1, NA5, 60,
  
  // Address 30
  NA4, -3, NC5, -3, ND5, -6, ND5, -6, REST, -3,
  // Address 40 x54
  ND5, -3, NE5, -3, NF5, -6, NF5, -6, REST, -3,
  // Address 64
  NF5, -3, NG5, -3, NE5, -6, NE5, -6, REST, -3,
  NE5, -3, ND5, -3, NC5, -6, ND5, -6, REST, -3,
  
  REPEAT, 30, 0, 32, 0,
           NC5, -3, ND5, -3, REST, -3,
  
  REPEAT, 30, 0, 12, 0,
           NF5, -3, NG5,   -6, NG5,   -6, REST, -3,
  NG5, -3, NA5, -3, NAIS5, -6, NAIS5, -6, REST, -3,
  NA5, -3, NG5, -3, NA5,   -6, ND5,   -6, REST, -3,
  
  // Address 114
  ND5, -3, NE5, -3, NF5, -3, REST, -6, NF5, -6, NG5, -3, NA5, -3, ND5, -3, REST, -3,
  ND5, -3, NF5, -3, NE5, -3, REST, -6, NE5, -6, NF5, -3, ND5, -3, NE5, -3, REST, -3,
  
  REPEAT, 30, 0, 114, 0,                                 NE5, -3, ND5, -3, REST, -3,
  
  // Address 161
  NF5, -4, NG5, -4, LOOP,    NA5, -2, 3,        NAIS5, -4, NA5, -4, REST, -2,
  LOOP,    NG5, -2, 3,       NG5, -4, NA5,  -4, REST,  -2,
  REPEAT, 165, 0, 10, 0,
  NG5, -2, NF5, -2, NE5, -2, ND5, -2, REST, -2,
  
  REPEAT, 161, 0, 39, 0,
  
  // Address 205
  NA4, -3, NC5, -3, ND5, -6, ND5, -6, REST, 7, NF5, -3, REST, -6, NF5, -3, REST, -3,
  ND5, -3, NE5, -3, NF5, -6, NF5, -6, REST, 7, NG5, -3, REST, -6, NG5, -3, REST, -3,
  NF5, -3, NG5, -3, NE5, -6, NE5, -6, REST, 7, NE5, -3, REST, -6, NE5, -3, REST, -3,
  ND5, -3, NC5, -3, NC5, -6, ND5, -6, REST, 7, ND5, -6, NE5,  -6, ND5, -6, NC5,  -6, ND5, -6, REST, -3,
  
  REPEAT, 205, 0, 54, 0,
  NA5, -3, NA5, -3, NAIS5, -4, NG5, -4, NA5, -4, REST, -4,
  
  REPEAT, 30, 0, 131, 0,
  
  // Address 303 (47, 1)
  NA5, -3, NC6, -3, REST, -3,
  NC6, -3, NB5, -3, NC6,  -3, REST, -3, NA5, -3, REST, -3,
  NC6, -3, NB5, -3, NG5,  -3, REST, -3, NC6, -3, REST, -3,
  NC6, -3, NB5, -3, NG5,  -3, REST, -3, NA5, -3, REST, -3,
  NF5, -3, ND5, -3, REST, -3,
  
  REPEAT, 47, 1, 44, 0,
           NA5, -3, REST, -3,
  
  REPEAT, 47, 1, 36, 0,
  NG5, -3, NF5, -3, NA5, -3, NF5, -3, NG5, -3, NF5, -3, NE5, -3, NC5, -3, ND5, -3, REST, 20,
  
  REPEAT, 30, 0, 127, 0,
  ND5,   -3, REST, -3, ND5, -3, REST, -3, NE5, -3, REST, -3, NF5, -3, REST, -3,
  NF5,   -3, REST, -3, NG5, -3, REST, -3, NA5, -3, REST, 0, 2, 3,
  NF5,   -6, NE5,  -6, ND5, -3, REST, 0, 2, 3,
  NAIS5, -3, REST, 0, 2, 3,
  NG5,   -6, NF5,  -6, NE5, -3, REST, 0, 2, 3,
  NG5,   -3, REST, -3, NF5, -3, REST, -3, NE5, -3, REST, 0, 2, 3,
  REPEAT, 161, 0, 39, 0,
  
  NF5,   -4, NG5,  -4, NA5, -2, REST, -2,
  NAIS5, -2, REST, -2,
  // Address 477 (221, 1)
  LOOP,      NA5,  -2, 3,       NA5,  -4, NG5, -4, REST, -2,
  NG5, -2,   REST, -2, NF5, -2, REST, -2,
  NE5, -2,   NF5,  -2, NE5, -2, NE5,  -4, ND5, -4, REST, -4,
  
  NF5, -4, NE5, -4, ND5, -4, REST, -4, NA5,   -2, REST, -4,
  ND5, -4, NE5, -4, NF5, -4, REST, -4, NAIS5, -2, REST, -4,
  ND5, -4, NE5, -4, NF5, -4, REST, -4,
  REPEAT, 221, 1, 10, 0,
  REST_FACTOR, 50,
  // Address 556 (34, 2)
  ND3, -2, NE3, -2, NF3, -2, NG3, -2, NA3, -2, NAIS3, -2, NC4, -2,
  SHIFT_TONE, 12, REPEAT, 34, 2, 14, 0,
  SHIFT_TONE, 12, REPEAT, 34, 2, 8, 0,
  SHIFT_TONE, -24, REST_FACTOR, 20,
  NA5, -2, REST, -2, NAIS5, -2, REST, -2,
  NA5, -2, REST, -6, NG5,   -2, REST, -6, NF5, -2, REST, -6,
  NF5, -4, NE5,  -4, ND5,   -2, REST, -2,
  NA4, -2, REST, -6, NG4,   -2, REST, -6, NF4, -4, NE4, -4, ND4, 60, ND6, -2, REST, -2
};


/*****************************************************************************************/
// Song 4: Hello World by Alan Walker ft. Torine
const uint8_t SONG_4[] PROGMEM = {
  NAIS5, -4, NC6,   -4, REST,  -4, NCIS6, 8,
  // Address 8
  NAIS5, -4, REST,  -4, NCIS6, -2, NDIS6, -4, REST, -4, NC6,   8,
  NAIS5, -4, NGIS5, -4, REST,  -4, NAIS5, 8,
  NGIS5, -4, REST,  -4, NAIS5, -2, NCIS5, -4, REST, -4, NF5,   8,
  NDIS5, -4, NCIS5, -4, REST,  -4, NFIS5, 8,
  NDIS5, -4, REST,  -4, NFIS5, -2, NGIS5, -4, REST, -4, NF5,   8,
  NDIS5, -4, NCIS5, -4, REST,  -4, NDIS5, 8,
  NCIS5, -4, REST,  -4, NDIS5, -2, NGIS4, -4, REST, -4, NAIS4, 8,
  REST,  -1,
  
  // Address 82
  NAIS4,   -2, NF5,    -2, REST, -4, NF5,   15,
  NDIS5,   -2, NCIS5,  -2, REST, -4,
  // Address 96
  NFIS5,   -2, NFIS5,  -2, REST, -4,
  NGIS5,   -2, NAIS5,  -2, REST, -4, NF5,   15,
  REPEAT, 96, 0, 14, 0,
  NDIS5,   -2, NCIS5,  -2, REST, -4, NDIS5, 15, REST, 20,
  
  REPEAT, 82, 0, 43, 0,
  
  // Address 130
  NAIS5, -2, NC6,   -4, REST, -4, NCIS6, -1,
  NAIS5, -2, NCIS6, -4, REST, -4, NC6,   -1,
  NAIS5, -2, NC6,   -4, REST, -4, NAIS5, -1,
  NGIS5, -2, NFIS5, -4, REST, -4, NF5,   -1, REST, -1,
  NFIS5, -1,
  NDIS5, -2, NFIS5, -4, REST, -4, NF5,   -1,
  // Address 174
  NDIS6, -8, NCIS5,  0, 3,     8, NCIS5, -4, REST, -4, NDIS5, -1,
  NCIS5, -2, NC5,   -4, REST, -4, NAIS4, -1, REST, -1,
  
  REPEAT, 136, 0, 60, 0,
  
  // Address 201
  NF5, -4,
  REPEAT, 4, 0, 76, 0,
  
  // Address 208
  NAIS4, -6, NC5, -6, NCIS5, -6, NAIS5, -6, NC6, -6, NCIS6, -6,
  REPEAT, 8, 0, 74, 0,
  
  // Address 225
  NAIS4, -2, NAIS4, -1, REST,  -2,
  NC5,   -2, NC5,   -1, REST,  -2,
  NCIS5, -2, NCIS5, -2, NDIS5, -4, NF5,   -1, REST, -4,
  NCIS5, -2, NFIS5, -2, NFIS5, -2,
  NF5,   -2, NFIS5, -2, NF5,   -2, REST,  -4,
  NDIS5, -4, NCIS5, -2, NCIS5, -2, NDIS5, -1,
  NCIS5, -2, NC5,   -2, NAIS4, -1,
  
  REPEAT, 208, 0, 12, 0,
  REPEAT, 225, 0, 50, 0,
  
  REPEAT, 80, 0, 50, 0,
  
  // Address 290
  NAIS5, -4, NC6,   -4, REST,  -4, NCIS6, -4, NC6,   -4, REST,  -4, NAIS5, -4, NAIS5, -4, REST, -1,
  NDIS5, -2, REST,  -2, NCIS5, -2, REST,  -2,
  NDIS5, -2, NCIS5, -2, NCIS5, -2, NCIS5, -2, NDIS5, -2, NF5,   -1, REST,  -2,
  NDIS5, -2, NCIS5, -2, NCIS5, -2, NFIS5, -2, NGIS5, -2, NF5,   -1, REST,  -2,
  NF6,   -2, NDIS6, -2, NCIS6, -2, NCIS6, -2, NC6,   -2, NAIS5, -1, REST,  -2,
  
  REPEAT, 130, 0, 95, 0,
  
  REPEAT, 80, 0, 50, 0
};


/*****************************************************************************************/
// Song 5: Catch Me If You Can by Alan Walker ft. Sorana
const uint8_t SONG_5[] PROGMEM = {
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
// Song 6: Lovesick by Alan Walker ft. Sophie Simmons
const uint8_t SONG_6[] PROGMEM = {
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
// Song 7: Unity by Alan Walker ft. The Walkers
const uint8_t SONG_7[] PROGMEM = {
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


/*****************************************************************************************/
// Song 8: The Spectre by Alan Walker
const uint8_t SONG_8[] PROGMEM = {
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


/*****************************************************************************************/
// Song 9: Alone by Alan Walker ft. Tove Stryrke
const uint8_t SONG_9[] PROGMEM = {
  // Address 0
  NG5, 20, NF5, 20, NDIS5, 20, NAIS4, 20, ND5, 20,
  // Address 10
  ND5, -3, NDIS5, -3, ND5, -3, NC5, 20,
  ND5, -1, NF5, -1,
  REPEAT, 0, 0, 10, 0,
  ND5, -4, ND5, -4, NDIS5, -4, ND5, -4, NC5, 20,
  // Address 37
  NG5, -1, NA5, -1,
  // Address 41
                   LOOP, NAIS5, -2, 3, NA5, -4, NA5, -2, NG5, -1,
  ND5,   -2, LOOP, NF5,   -2, 3, NG5, -4, NF5, -2, ND5, -1,
  NC5,   -2, LOOP, NAIS4, -2, 3, ND5, -4, NF5, -1,
  NAIS4, -2, LOOP, NA4,   -2, 3, ND5, -4, NF5, -1,
  
  NF5,   -2, REPEAT, 41, 0, 32, 0,
  // Address 90
  NF5, -2, REST_FACTOR, 60, NF5, -1, NG5, -1,  NAIS5, -1,
  REST_FACTOR, 20, NA5, -2, NAIS5, -2, NG5, -1,
  // Address 108
  NG5,   -3, REST, -3, ND5,   -6, ND5,   -6, REST,  -3, NF5,  -3, REST, -3,
  // Address 122
  ND6,   -4, NC6,  -4, ND6,   -4, NC6,   -4, ND6,   -4, REST, -4,
  NG5,   -3, REST, -3, NDIS5, -6, NDIS5, -6, REST,  -3, NF5,  -3, REST, -3,
  // Address 148
  NDIS6, -4, NC6,  -4, NDIS6, -4, NC6,   -4, NDIS6, -4, REST, -4,
  NF5,   -3, REPEAT, 110, 0, 24, 0,
  // Address 167 x269 (13, 1)
  REPEAT, 90, 0, 18, 0,
  // Address 172 x274 (18, 1)
  NAIS5, -3, REST, -3, NA5,   -6, NA5,   -6, REST, -3, ND5,   -3,
  REPEAT, 120, 0, 14, 0,
  // Address 189
  NDIS6, -3, REST, -3, ND6,   -6, ND6,   -6, REST, -3, NAIS5, -3,
  REPEAT, 146, 0, 16, 0,
             REST, -3, NA5,   -6, NA5,   -6, REST, -3, NAIS5, -3,
  REPEAT, 120, 0, 14, 0,
  
  REPEAT, 90, 0, 18, 0,
  
  // Address 226
  REST_FACTOR, 50,
  NG5, -2, ND5, -2, NF5, -2, ND5, -2,
  NG5, -4, ND5, -4, NF5, -4, ND5, -4, NG5, -2, NG5, -2,
  // Address 248 x370 (114, 1)
  NG5,   -1, NG5,   -1, NF5,   -1, NF5, -1, NDIS5, -1,
  NDIS5, -1, NAIS4, -1, NAIS4, -1, ND5, -1, ND5,   -1,
  ND5,   -3, NDIS5, -3, ND5,   -3, NC5, -1, NC5,   -1,
  ND5,   -1, NF5,   -1, REPEAT, 248, 0, 30, 0,
  
  // Address 287 (31, 1)
  NF5, -1, ND5, -2, NAIS5, -2, NA5, -2, NG5, 15,
  REPEAT, 31, 1, 10, 0,
  REPEAT, 31, 1, 8, 0,
  NAIS5, -1, NF5, -1, NAIS5, -1, NC6, -2, NAIS5, -2, NA5, 20,
  
  REST_FACTOR, 20, REPEAT, 37, 0, 189, 0,
  
  // Address 326 (70, 1)
  REPEAT, 172, 0, 12, 0, REST, -3,
  NG5,   -2, NA5,   -2, NAIS5, -4, REST, -4, NDIS5, -2,
  REPEAT, 189, 0, 12, 0, REST, -3,
  NAIS5, -2, NAIS5, -2, NA5,   -4, REST, -4, NAIS5, -2,
  NF5, -3, REPEAT, 206, 0, 10, 0, REST, -3,
  NAIS5, -2, ND6, -2, NC6, -4, REST, -4,
  REPEAT, 90, 0, 18, 0,
  
  REPEAT, 70, 1, 56, 0
};


/*****************************************************************************************/
// Song 10: Creatures by Hot Shade ft. Nomi Bontegard
const uint8_t SONG_10[] PROGMEM = {
  NG6,   -2,
  NFIS6, -4, NG6,   -4, NFIS6, -2,
  NE6,   -4, NFIS6, -4, NE6,   -2,
  ND6,   -4, NE6,   -4, ND6,   -2,
  NCIS6, -4, ND6,   -4, NCIS6, 30, REST, -1,
  REPEAT, 6, 0, 18, 0,
                        NCIS6, -2,
  NB5,   -4, NCIS6, -4, NB5,   30, REST, -1,
  //
  //  Address 43
  NB4, -1,
  REST, -2, NB4, -2, ND5, -1,
  REST, -2, ND5, -2, NE5, -1,
  NFIS5, -1, NA4, -1,
  REST, -1, NFIS5, -1,
  REST, -2, NE5, -4, NFIS5, -4, NE5, -1,
  REST, -2, NCIS5, -4, ND5, -4, NB4, 30, REST, -1,
  //
  NG4, -2, REPEAT, 43, 0, 18, 0,
  REST, -2, NA4, -2, REPEAT, 63, 0, 8, 0,
                             NA5, -1,
  REST, -2, NFIS5, -4, NA5, -4, NCIS6, 30, REST, -2,
  //
  // Address 111
  NE5, -2, NFIS5, -2, NG5,   -2, NG5,   -2, NFIS5, -2, NE5,   -2, NFIS5, -2, REST, -2,
  NE5, -2, ND5,   -2, NE5,   -2, NFIS5, -2, NE5,   -2, ND5,   -2, NE5,   -2, REST, -2,
  ND5, -2, NE5,   -2, NFIS5, -1, NA5,   -1, NE5,   -1, NFIS5, -1, NB4,   20, REST, -1,
  //
  // Address 159
  REPEAT, 111, 0, 42, 0,                               ND5,   -1, NB4,   20, REST, -1,
  //
  // Address 170
  NE5,   -1, NB5, -1, NA5, -1, ND6,   -1, NCIS6, -1, NE5, -1, NE5,  15,
  NE5,   -2, ND6, -1, NE6, -1, NCIS6, -1, NA5,   -1, NB5, 30, REST, -1,
  //
  REPEAT, 170, 0, 8, 0,        NA5,   -2, NCIS6, -2, NE5, -1, NE5,  15,
  NFIS5, -2, NG5, -1, NA5, -1, NFIS5, -1, ND5,   -1, NE5, -2, NFIS5, -2, NG5, -2, ND6, -2, NB5, 20,
  //
  // Address 231
  REPEAT, 0, 0, 43, 0,
  REPEAT, 0, 0, 231, 0,
  REPEAT, 0, 0, 43, 0,
  REPEAT, 0, 0, 43, 0,
  REPEAT, 170, 0, 61, 0
};


const uint8_t * const SONG_BUFFER[] PROGMEM = {
  SONG_1,
  SONG_2,
  SONG_3,
  SONG_4,
  SONG_5,
  SONG_6,
  SONG_7,
  SONG_8,
  SONG_9,
  SONG_10
};


const uint16_t SONG_SIZE[] PROGMEM = {
  sizeof(SONG_1),
  sizeof(SONG_2),
  sizeof(SONG_3),
  sizeof(SONG_4),
  sizeof(SONG_5),
  sizeof(SONG_6),
  sizeof(SONG_7),
  sizeof(SONG_8),
  sizeof(SONG_9),
  sizeof(SONG_10)
};


const uint16_t SONG_TEMPO[] PROGMEM = {
  128,  // Song  1
  90,   // Song  2
  106,  // Song  3
  104,  // Song  4
  147,  // Song  5
  123,  // Song  6
  115,  // Song  7
  128,  // Song  8
  97,   // Song  9
  114   // Song 10
};


const char SONG_1_TITLE [] PROGMEM = "Pirates of The Carribean by Alan Walker";
const char SONG_2_TITLE [] PROGMEM = "Ignite by K-391 ft. Alan Walker, Julie Bergan, & Seungri";
const char SONG_3_TITLE [] PROGMEM = "He's a Pirate - Live performance by D. Garrett (Composed By H. Zimmer & K. Badelt)";
const char SONG_4_TITLE [] PROGMEM = "Hello World by Alan Walker ft. Torine";
const char SONG_5_TITLE [] PROGMEM = "Catch Me If You Can by Alan Walker ft. Sorana";
const char SONG_6_TITLE [] PROGMEM = "Lovesick by Alan Walker ft. Sophie Simmons";
const char SONG_7_TITLE [] PROGMEM = "Unity by Alan Walker ft. The Walkers";
const char SONG_8_TITLE [] PROGMEM = "The Spectre by Alan Walker";
const char SONG_9_TITLE [] PROGMEM = "Alone by Alan Walker ft. Tove Stryrke";
const char SONG_10_TITLE[] PROGMEM = "Creatures by Hot Shade ft. Nomi Bontegard";


const char * const SONG_TITLE[] PROGMEM = {
  SONG_1_TITLE,
  SONG_2_TITLE,
  SONG_3_TITLE,
  SONG_4_TITLE,
  SONG_5_TITLE,
  SONG_6_TITLE,
  SONG_7_TITLE,
  SONG_8_TITLE,
  SONG_9_TITLE,
  SONG_10_TITLE
};


const uint8_t TONE_RANGE[] PROGMEM = {
  ND5,   NAIS6,  // Song  1
  NDIS4, NDIS6,  // Song  2
  NA4,   ND6,    // Song  3
  NGIS4, NF6,    // Song  4
  ND5,   NF6,    // Song  5
  NG4,   NF6,    // Song  6
  NGIS4, NF6,    // Song  7
  NB4,   NGIS6,  // Song  8
  NA4,   NDIS6,  // Song  9
  NG4,   NG6     // Song 10
};  // lowest tone, highest tone
