#pragma once

#include <portmacro.h>
#include <esp_log.h>
#include <rmt_common.h>
#include <rmt_tx.h>

#define RMT_TX_CHANNEL (RMT_CHANNEL_0)
#define RMT_TX_GPIO (GPIO_NUM_4)

#define HALF_BLOCK_NUMS (4)
#define BLOCK_NUMS (HALF_BLOCK_NUMS * 2)
#define RUNLEN (80)

void rmt_setup(gpio_num_t gpio_num);
void fill();