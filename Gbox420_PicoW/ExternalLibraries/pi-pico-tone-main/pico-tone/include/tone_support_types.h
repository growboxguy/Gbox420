#pragma once
#include "pico/stdlib.h"

#define TICKS 768000  // nr of ticks = 0.768MHz (48kHz * 16 ticks = 384000)
#define MAXTICKS (TICKS / 110) // lowest frequency
#define TONE_BLOCKING true  // for use in init() ;
#define TONE_NON_BLOCKING false  // for use in init() ;
#define MAX_TONE_HANDLERS 4  //maximum number of tone objects that can be created
#define NOT_ASSIGNED_DMA 16



#ifdef __cplusplus
extern "C" {
#endif

struct ts_type {
	uint32_t no_bits ;
	uint32_t words[MAXTICKS/32+1] ;
	uint32_t no_words ;
};  // timeseries bit patterns container type

typedef void (*void_func_pt)() ; // This is a type holding a pointer to a C-style DMA interrupt handler

#ifdef __cplusplus
}
#endif