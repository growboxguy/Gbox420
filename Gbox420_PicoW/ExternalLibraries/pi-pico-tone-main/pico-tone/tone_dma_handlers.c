// This code encapsulates the creation of 4 different dma handlers, each geared towards a 
// specific instance of the Tone object. It is used in the init() function of a tone object. 

#include <stdio.h>
#include "tone_dma_handlers.h"
#include "pdm_pushout.pio.h"

volatile static struct tone_dma_handler_info_type tone_dma_handler_info[MAX_TONE_HANDLERS]; // array of info for dma handlers
volatile static uint free_tone_dma_handler = 0;
volatile static int32_t push_sm_to_zero[2] = {4,0} ;

void tone_dma_handler0() {
		const int i = 0;
		if (! (dma_hw->ints0 & ( 1u << tone_dma_handler_info[i].dma_chan ))) {return;} ; // not mine
		dma_hw->ints0 = ( 1u << tone_dma_handler_info[i].dma_chan ) ; // reset my bit
		if (tone_dma_handler_info[i].no_waves == 1) { return; } // nothing to handle, finished with this interrupt
		tone_dma_handler_info[i].no_waves = tone_dma_handler_info[i].no_waves?(tone_dma_handler_info[i].no_waves - 1):0;  // if 0 keep it zero 
		if (tone_dma_handler_info[i].no_waves == 1) {
//			printf("I (0) am pushing the 2 words silence now\n");
			dma_channel_set_trans_count(tone_dma_handler_info[i].dma_chan,2,0);
			dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, push_sm_to_zero, true);
			return;
		};
		dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, tone_dma_handler_info[i].begin_block_mem, true);
	};
	
void tone_dma_handler1() {
		const int i = 1;
		if (! (dma_hw->ints0 & ( 1u << tone_dma_handler_info[i].dma_chan ))) {return;} ; // not mine
		dma_hw->ints0 = ( 1u << tone_dma_handler_info[i].dma_chan ) ; // reset my bit
		if (tone_dma_handler_info[i].no_waves == 1) { return; } // nothing to handle, finished with this interrupt
		tone_dma_handler_info[i].no_waves = tone_dma_handler_info[i].no_waves?(tone_dma_handler_info[i].no_waves - 1):0;  // if 0 keep it zero 
		if (tone_dma_handler_info[i].no_waves == 1) {
//			printf("I (1) am pushing the 2 words silence now\n");
			dma_channel_set_trans_count(tone_dma_handler_info[i].dma_chan,2,0);
			dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, push_sm_to_zero, true);
			return;
		};
		dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, tone_dma_handler_info[i].begin_block_mem, true);
	};

void tone_dma_handler2() {
		const int i = 1;
		if (! (dma_hw->ints0 & ( 1u << tone_dma_handler_info[i].dma_chan ))) {return;} ; // not mine
		dma_hw->ints0 = ( 1u << tone_dma_handler_info[i].dma_chan ) ; // reset my bit
		if (tone_dma_handler_info[i].no_waves == 1) { return; } // nothing to handle, finished with this interrupt
		tone_dma_handler_info[i].no_waves = tone_dma_handler_info[i].no_waves?(tone_dma_handler_info[i].no_waves - 1):0;  // if 0 keep it zero 
		if (tone_dma_handler_info[i].no_waves == 1) {
//			printf("I (2) am pushing the 2 words silence now\n");
			dma_channel_set_trans_count(tone_dma_handler_info[i].dma_chan,2,0);
			dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, push_sm_to_zero, true);
			return;
		};
		dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, tone_dma_handler_info[i].begin_block_mem, true);
	};
	
void tone_dma_handler3() {
		const int i = 1;
		if (! (dma_hw->ints0 & ( 1u << tone_dma_handler_info[i].dma_chan ))) {return;} ; // not mine
		dma_hw->ints0 = ( 1u << tone_dma_handler_info[i].dma_chan ) ; // reset my bit
		if (tone_dma_handler_info[i].no_waves == 1) { return; } // nothing to handle, finished with this interrupt
		tone_dma_handler_info[i].no_waves = tone_dma_handler_info[i].no_waves?(tone_dma_handler_info[i].no_waves - 1):0;  // if 0 keep it zero 
		if (tone_dma_handler_info[i].no_waves == 1) {
//			printf("I (3) am pushing the 2 words silence now\n");
			dma_channel_set_trans_count(tone_dma_handler_info[i].dma_chan,2,0);
			dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, push_sm_to_zero, true);
			return;
		};
		dma_channel_set_read_addr(tone_dma_handler_info[i].dma_chan, tone_dma_handler_info[i].begin_block_mem, true);
	};

//void print_this_h(int i) {
//	printf ("This handler %d was free:\n", i);
//	printf ("  - dma-chan= %d\n", tone_dma_handler_info[i].dma_chan);
//	printf ("  - timersies= %X\n", tone_dma_handler_info[i].begin_block_mem);
//	printf ("  - pio = %d\n", pio_get_index(tone_dma_handler_info[i].pio));
//	printf ("  - sm= %d\n", tone_dma_handler_info[i].sm);
//	printf ("  - offset= %d\n", tone_dma_handler_info[i].offset);
//	printf ("  - handlerpointer= %X\n", tone_dma_handler_info[i].handler_pointer);
//	printf ("  - no_waves= %d\n", tone_dma_handler_info[i].no_waves);
//	printf ("-----------------------\n");
//}
		
	
int tone_dma_claim_free_handler_instance(uint dma_chan, struct ts_type * ts_address, PIO pio, uint sm, uint offset) {
		if (free_tone_dma_handler >= MAX_TONE_HANDLERS) { return -1; };
		int this_handler_instance = free_tone_dma_handler ;
		free_tone_dma_handler++;
		tone_dma_handler_info[this_handler_instance].dma_chan = dma_chan ;
		tone_dma_handler_info[this_handler_instance].begin_block_mem = ts_address ;
		tone_dma_handler_info[this_handler_instance].pio = pio ;
		tone_dma_handler_info[this_handler_instance].sm = sm ;
		tone_dma_handler_info[this_handler_instance].offset = offset ;
		switch (this_handler_instance) {
			case 0: tone_dma_handler_info[0].handler_pointer = tone_dma_handler0 ; return 0 ;
			case 1: tone_dma_handler_info[1].handler_pointer = tone_dma_handler1 ; return 1 ;
			case 2: tone_dma_handler_info[2].handler_pointer = tone_dma_handler2 ; return 2 ;
			case 3: tone_dma_handler_info[3].handler_pointer = tone_dma_handler3 ; return 3 ;
		};
		return -1;
	};

void_func_pt tone_dma_get_handler_pointer(uint this_handler_instance) {
	return tone_dma_handler_info[this_handler_instance].handler_pointer ;
};

void tone_dma_handler_info_set_no_waves(uint this_handler_instance, uint no_waves) {
	if (no_waves != 0) {
		tone_dma_handler_info[this_handler_instance].no_waves = no_waves+1 ;
	} else {
		tone_dma_handler_info[this_handler_instance].no_waves = 0 ;
	};
//	printf("SETTING NO_WAVES====>");
//	print_this_h(this_handler_instance);
};

void tone_dma_handler_exit_now(uint this_handler_instance) {
	tone_dma_handler_info[this_handler_instance].no_waves = 2 ;
};
