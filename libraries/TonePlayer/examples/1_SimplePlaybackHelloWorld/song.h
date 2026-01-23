// Required by the 1_SimplePlaybackHelloWorld.ino example sketch.


#include <TonePlayer.h>


// Song title: Hello World by Alan Walker ft. Torine
#define BPM 104

const uint8_t SONG[] PROGMEM = {
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
