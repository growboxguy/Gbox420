#include "pico_tone.hpp"
// #include "pico/stdio.h"
#include "pico/time.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "pico/float.h"
#include "tone_dma_handlers.h"
#include "pdm_pushout.pio.h"

//	void Tone::print_timeseries () {
//		int i = 0;
//		printf("No of bits = %d\n", this->timeseries.no_bits);
//		while (i<this->timeseries.no_bits) {
//			uint32_t word = this->timeseries.words[i/32] ;
//			for (int j = 0; j <32 ; j++) {
//				uint bit = (word & ((uint32_t)1 << 31))>>31;
//				word <<= 1 ;
//				printf("%d,",bit);
//			} 
//		i += 32 ;
//		}
//		printf("\n");
//	};

//volatile uint32_t now_stamp, then_stamp ;

	float Tone::y(int n) {
		float r = 0;
		float factor = 2.0 * this->tone_v.loudness;
		for (int i = 0 ; i< 7 ; i++) {
			if (this->tone_v.harmonics[i] == 0) break ;
			r = r + sinf( ((n*(i+1)%this->tone_v.t)/float(this->tone_v.t)) * M_PI * 2) * (this->tone_v.harmonics[i]/factor) + (this->tone_v.harmonics[i]/factor);
		}
		return r;
	};
	
	void Tone::set_high(int n) {
		this->timeseries.words[n/32] |= (1u << (31 - n%32));
	};

	void Tone::set_low(int n) {
		this->timeseries.words[n/32] &= ~(1u <<(31 - n%32));
	};
	
	void Tone::pdm_timeseries_fill() { 
		
		//Zeta time series calculation
		float qe = 0;// initial running error is zero
		for (int n = 0; n < this->tone_v.t ; n++) {
			float analogue_value = y(n) ;
			int y = 0 ;
			if (analogue_value >= qe) {
				set_high(n) ; y = 1 ;
			} else {
				set_low(n) ; y = 0 ;
			}
			qe = y - analogue_value + qe;
		};
		
		//no_bits and no_words in time series
		this->timeseries.no_bits = this->tone_v.t ;
		this->timeseries.no_words = (this->timeseries.no_bits + (32-(this->timeseries.no_bits%32))%32) / 32 ;
	};
	
	void Tone::playtone_dma() {
//		printf("In Playtone dma %d\n", this->this_handler_instance);
		stop();	// make sure possible running tones are stopped. 
		pio_sm_set_enabled(this->pio,this->sm,false);
		pio_sm_clear_fifos(this->pio,this->sm);
		pio_sm_set_enabled(this->pio,this->sm,true);
		pio_sm_restart(this->pio,this->sm);
		pio_sm_exec_wait_blocking(this->pio, this->sm, pio_encode_jmp(offset[pio_get_index(this->pio)]));
//		printf("Playtone_dma instance %d: #waves = %d\n", this->this_handler_instance,this->tone_v.duration);
		tone_dma_handler_info_set_no_waves(this->this_handler_instance,this->tone_v.duration) ;
//		printf("Trans_count = %d, no_bit = %d\n", this->timeseries.no_words + 1, this->timeseries.no_bits);
		dma_channel_set_trans_count(this->dma_chan, this->timeseries.no_words + 1, false);
		dma_channel_set_read_addr(this->dma_chan, &(this->timeseries), true);
	};
	
	void Tone::playtone_blocking_no_dma() {
//		printf("starting to play tone on pio %d, sm %d\n", pio_get_index(this->pio),this->sm);
		pio_sm_clear_fifos(this->pio,this->sm);
		pio_sm_set_enabled(this->pio,this->sm,true);
		pio_sm_restart(this->pio,this->sm);
		pio_sm_exec_wait_blocking(this->pio, this->sm, pio_encode_jmp(offset[pio_get_index(this->pio)]));
		for (int32_t wave_no = 0 ; wave_no < this->tone_v.duration ; ++wave_no){
			pio_sm_put_blocking(this->pio,this->sm,this->timeseries.no_bits); // enter the number of bits to process into the statemachine
			for (int word_out = 0 ; word_out < this->timeseries.no_words ; ++word_out) {			
				pio_sm_put_blocking(this->pio, this->sm, this->timeseries.words[word_out]);
			};
		};
		pio_sm_put_blocking(this->pio, this->sm, (uint32_t)4);
		pio_sm_put_blocking(this->pio, this->sm, (uint32_t)0);
	};

	void Tone::reconstruct_tone(uint base, uint h2, uint h3, uint h4, uint h5, uint h6, uint h7) {
		uint factor = (base + h2 + h3 + h4 + h5 + h6 + h7) ;
		this->tone_v.harmonics[0] = (base * 100) / factor ;
		this->tone_v.harmonics[1] = (h2 * 100) / factor ;
		this->tone_v.harmonics[2] = (h3 * 100) / factor ;
		this->tone_v.harmonics[3] = (h4 * 100) / factor ;
		this->tone_v.harmonics[4] = (h5 * 100) / factor ;
		this->tone_v.harmonics[5] = (h6 * 100) / factor ;
		this->tone_v.harmonics[6] = (h7 * 100) / factor ;
	};
	
	void Tone::construct_tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5, uint h6, uint h7) {
		reconstruct_tone(base,h2,h3,h4,h5,h6,h7);
		this->pio_pin = pin ;
	};	
	
	Tone::Tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5, uint h6, uint h7) {construct_tone(pin,base,h2,h3,h4,h5,h6,h7);};
	Tone::Tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5, uint h6) {construct_tone(pin, base, h2, h3, h4, h5, h6, 0);};
	Tone::Tone(uint pin, uint base, uint h2, uint h3, uint h4, uint h5) {construct_tone(pin, base, h2, h3, h4, h5,  0, 0);};	
	Tone::Tone(uint pin, uint base, uint h2, uint h3, uint h4) {construct_tone(pin, base, h2, h3, h4,  0,  0, 0);};	
	Tone::Tone(uint pin, uint base, uint h2, uint h3) {construct_tone(pin, base, h2, h3,  0,  0,  0, 0);};
	Tone::Tone(uint pin, uint base, uint h2) {construct_tone(pin, base, h2,  0,  0,  0,  0, 0);};
	Tone::Tone(uint pin, uint base) {construct_tone(pin, base,  0,  0,  0,  0,  0, 0);};
	Tone::Tone(uint pin) {construct_tone(pin,  100,  0,  0,  0,  0,  0, 0);};
	
	int Tone::init(bool blocking, bool do_panic) {
		
//		Inititalize PIO statemachine
		PIO pio = pio0 ;
		int sm = pio_claim_unused_sm(pio, false);
		
		if (sm == -1) {
			pio = pio1 ;
			sm = pio_claim_unused_sm(pio, false);
		};
		
		if (sm == -1 & do_panic) {panic("No tone_pio_sm available");};
		if (sm == -1 & !do_panic){return -1;};
		
		this->sm = sm ;
		this->pio = pio ;
		uint pio_pin = this->pio_pin ;
		
		if ( !(pio_program_loaded[pio_get_index(pio)])) {
			bool can_add = pio_can_add_program (pio, &pdm_pushout_program) ;
			if  ( !can_add  & do_panic) { panic("Cannot add tone program to PIO\n");};
			if  ( !can_add  & !do_panic){ return (-2 - pio_get_index(pio)) ;};
			offset[pio_get_index(pio)] = pio_add_program(pio, &pdm_pushout_program);
			pio_program_loaded[pio_get_index(pio)] = true ;
		};		

		pio_gpio_init(pio, pio_pin);
		pio_sm_set_consecutive_pindirs(pio, sm, pio_pin, 1, true);
		pio_sm_config c = pdm_pushout_program_get_default_config(offset[pio_get_index(pio)]);
		sm_config_set_out_pins(&c,pio_pin,1);
		sm_config_set_set_pins(&c,pio_pin,1);
		sm_config_set_out_shift(&c, false, true, 32);
		int cycles_per_bit = 6;
		float freq = TICKS;
		float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
//		printf("DIV=%f\n",div);
		sm_config_set_clkdiv(&c, div);
		sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
		pio_sm_init(pio, sm, offset[pio_get_index(pio0)], &c);
		pio_sm_set_enabled(pio, sm, false);
		this->dma_chan = NOT_ASSIGNED_DMA ;
		this->this_handler_instance = 0 ;
		
		if (!blocking) {
//		Initialize DMA channel but not yet set the count
		int dma_chan = dma_claim_unused_channel(false);
		
		if (dma_chan == -1 & do_panic) {panic("No tone_dma_channel available");};
		if (dma_chan == -1 & !do_panic){return -4;};
		
		this->dma_chan = dma_chan;
		this->this_handler_instance = tone_dma_claim_free_handler_instance(dma_chan, &(this->timeseries), pio, sm, offset[pio_get_index(this->pio)]) ;
//		printf("dma channel = %d, %d in instance # %d\n",dma_chan, this->dma_chan, this->this_handler_instance );
		
		if (this->this_handler_instance == -1 & do_panic) {panic("No tone_dma_handler available");};
		if (this->this_handler_instance == -1 & !do_panic){return -5;};
		
		this->this_handler_function  = tone_dma_get_handler_pointer(this->this_handler_instance) ;
		volatile void * pio_sm_fifo_addr = (volatile void *)&pio->txf[sm];	
		dma_channel_config cdma = dma_channel_get_default_config(dma_chan) ;
		channel_config_set_read_increment(&cdma, true);
		channel_config_set_write_increment(&cdma, false);
		channel_config_set_dreq(&cdma, pio_get_dreq(pio,sm,true));
		dma_channel_set_config(dma_chan, &cdma, false);
		dma_channel_set_write_addr(dma_chan, pio_sm_fifo_addr, false);
		dma_channel_set_irq0_enabled(dma_chan, true);
		irq_add_shared_handler(DMA_IRQ_0, this->this_handler_function, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY) ;
		irq_set_enabled(DMA_IRQ_0, true);
		}
		return (dma_chan*256 + this->this_handler_instance*16 + pio_get_index(pio)*4 + sm) ; //succes. 
	};

	void Tone::init(bool blocking) {
		int result = init(blocking,true); // panic if something is wrong
	};
	
	void Tone::tone(uint frequency, float duration) {
		// silent if frequency is zero, blocking when no_dma. 
		if (frequency == 0) {
			if (this->dma_chan == NOT_ASSIGNED_DMA) sleep_ms(duration*1000) ;
			return;
		};
		// fill the tone information		
		this->tone_v.frequency = (frequency<110)?110:frequency ;
		this->tone_v.frequency = (this->tone_v.frequency>12000)?12000:this->tone_v.frequency ;
		this->tone_v.t = TICKS / this->tone_v.frequency ;
		this->tone_v.duration = ( duration * this->tone_v.frequency) ;
		this->tone_v.loudness = 100 ;
		//fill the timeseries

		pdm_timeseries_fill() ;

		// start playing
		if (this->dma_chan == NOT_ASSIGNED_DMA) {
			playtone_blocking_no_dma() ;
		} else {
			playtone_dma();
		};
	};
	
	void Tone::tone(uint frequency) {
		tone(frequency,0);
	};
	
	void Tone::stop() {
//		printf("Stopping Tone %d\n", this->this_handler_instance);
		if (this->dma_chan != NOT_ASSIGNED_DMA) { 
			tone_dma_handler_exit_now(this->this_handler_instance);
			//give three waves of frequency plus the 8 FIFO word buffer + 1 wave of silence (2 words) timing to allow
			//for stopping the tone. It is ugly, but I have not (yet) found a better way. 
			int sleep_calc = ((3 * 1000)/110 + 5 );
			sleep_ms(sleep_calc) ;
		};
	};
	
	void Tone::reconfigure_harmonics(uint base,uint h2,uint h3,uint h4,uint h5,uint h6,uint h7) {
		reconstruct_tone(base,h2,h3,h4,h5,h6,h7);
	};
	
	void Tone::play_melody(uint tempo, uint no_notes, int * melody) {
		float duration_unit = 4 * 60.0 / (float)tempo ; // tempo is beats per minute for quarter notes
		float tone_duration ;
		for (int i = 0; i < (no_notes*2) ; i=i+2) {
			if (melody[i+1] < 0) {
				tone_duration = melody[i+1]* -1.5 ;
			} else {
				tone_duration = melody[i+1];
			};
			tone(melody[i], 0.90 * duration_unit / tone_duration);
			if (this->dma_chan == NOT_ASSIGNED_DMA) {
				sleep_ms(1000* 0.10 * duration_unit / tone_duration);
			} else {
				sleep_ms(1000 * duration_unit / tone_duration);
			};
		};
	};

//for backwards compatibility with pre-release version (not advertised). 
	void Tone::play_melody(uint tempo, uint length, uint * pitches, uint * values) {
		float duration_unit = 4 * 60.0 / (float)tempo ; // tempo is beats per minute for quarter notes
		for (int i = 0; i < length ; i++) {
			tone(pitches[i], 0.90 * duration_unit / values[i]);
			if (this->dma_chan == NOT_ASSIGNED_DMA) {
				sleep_ms(1000* 0.10 * duration_unit / values[i]);
			} else {
				sleep_ms(1000 * duration_unit / values[i]);
			};
		};
	};
