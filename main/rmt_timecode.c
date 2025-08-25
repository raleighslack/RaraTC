#include "rmt_timecode.h"
static int refill = 0;

void IRAM_ATTR rmt_isr_handler(void *arg) {
//   RMT.int_clr.ch0_tx_thr_event = 1;

  refill ++; // no need for critical section protection with mux is unnecessary (since the ISR can NOT interrupt itself)

//   RMT.int_clr.ch0_tx_thr_event = 1;
}

void rmt_setup(gpio_num_t gpio_num) {
    rmt_channel_handle_t tx_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio_num,
        .mem_block_symbols = 64,
        .resolution_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,           // set the number of transactions that can pend in the background
        .flags.invert_out = false,        // do not invert output signal
        .flags.with_dma = false,          // do not need DMA backend
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));
}

void fill() {
    
}