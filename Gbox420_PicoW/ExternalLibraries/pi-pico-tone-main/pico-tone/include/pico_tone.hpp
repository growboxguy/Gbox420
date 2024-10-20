#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "tone_support_types.h"
#include "pitches.h"

static bool pio_program_loaded[2] = {false,false} ;
static uint offset[2] ;

class Tone {
  private:
  
	uint dma_chan = NOT_ASSIGNED_DMA;
	PIO pio;
	uint sm ;
	uint pio_pin ;
	uint32_t wavetimes ;
	int this_handler_instance ;
	void_func_pt this_handler_function ;
	
	ts_type timeseries ;	//assume they are aligned at 32 bits words with no padding. In PI PICO they will. 

	struct tone_t {
		uint frequency ;
		uint32_t t; // ticks
		uint harmonics[7]; //tone_shape
		uint32_t duration ; // # of cycles (in case of 440Hz, 440 cycles = 1 second).  
		uint loudness;
	} tone_v ;

//	void print_timeseries ();
	float y(int n);
	void set_high(int n);
	void set_low(int n);
	void pdm_timeseries_fill();
	void playtone_dma(); 
	void playtone_blocking_no_dma();
	void reconstruct_tone(uint base, uint h2, uint h3, uint h4, uint h5, uint h6, uint h7);
	void construct_tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5, uint h6, uint h7);
	
	public:
	
	//Constructors
	Tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5, uint h6, uint h7);
	Tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5, uint h6); 
	Tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5); 
	Tone(uint pin, uint base, uint h2, uint h3, uint h4); 
	Tone(uint pin, uint base, uint h2, uint h3); 
	Tone(uint pin, uint base, uint h2); 
	Tone(uint pin, uint base); 
	Tone(uint pin); 
	
	// Functions
	int  init(bool blocking, bool do_panic);
	void init(bool blocking);	
	void stop() ;
	void tone(uint frequency, float duration); 
	void tone(uint frequency);
	void reconfigure_harmonics(uint base,uint h2,uint h3,uint h4,uint h5,uint h6,uint h7); 
	void play_melody(uint tempo, uint length, uint * pitches, uint * values);
	void play_melody(uint tempo, uint length, int * melody);
};
