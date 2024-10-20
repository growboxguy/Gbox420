
#include "pico/time.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "tone_support_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tone_dma_handler_info_type {
	uint dma_chan ;
	int32_t no_waves ;
	struct ts_type * begin_block_mem ;
	void_func_pt handler_pointer ;
	PIO pio ;
	uint sm ;
	uint offset ;
}; // information needed by the interrupt handlers
 
void tone_dma_handler0() ; 	
void tone_dma_handler1() ;
void tone_dma_handler2() ;
void tone_dma_handler3() ; 
int tone_dma_claim_free_handler_instance(uint dma_chan, struct ts_type * ts_address, PIO pio, uint sm, uint offset);
void_func_pt tone_dma_get_handler_pointer(uint this_handler_instance) ;
void tone_dma_handler_info_set_no_waves(uint this_handler_instance, uint no_waves); 
void tone_dma_handler_exit_now(uint this_handler_instance);
void print_this_h(int i);

#ifdef __cplusplus
}
#endif
