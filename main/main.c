/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "esp_timer.h"
#include "smpte_timecode.h"
#include "esp_sntp.h"
#include <string.h>

#define LTC_OUTPUT_PIN      4
#define LTC_FRAMERATE       24
#define LTC_BITS_PER_FRAME  80

static const char *TAG = "main";

static void periodic_timer_callback(void* arg); //stolen from example

const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "periodic"
};
esp_timer_handle_t periodic_timer;
/* The timer has been created but is not running yet */

const float period_us = (((float)1/LTC_FRAMERATE)/LTC_BITS_PER_FRAME)*10000;

static void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us", time_since_boot);
}

void print_binary(uint8_t* bits[10]) {
    for (int i = 0; i < 10; i++) {
        if (bits[i] == NULL) {
            printf("NULL ");
            continue;
        }
        for (int j = 7; j >= 0; j--) {
            printf("%d", (*bits[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

void app_main(void)
{
    esp_timer_init();
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, period_us));
    
    ltc_frame testframe = {};
    create_frame_from_timecode(&testframe, 21, 16, 29, 05);

    two_digits twodigis = {};
    convert_digits_to_single(&twodigis, testframe.frame);
    ESP_LOGI(TAG, "LSB: %u, MSB: %u", twodigis.lsb, twodigis.msb);

    uint8_t *test[10] = {};
    create_bits_from_frame(test, testframe);

    ESP_LOGI(TAG, "%.6f", period_us);
    print_binary(test);
}
