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

const float period_us = (((float)1/LTC_FRAMERATE)/LTC_BITS_PER_FRAME)*1000000;
const float half_period_us = period_us/2;
const float frame_period_us = ((float)1/24) * 1000000;

simple_frame current_simple_frame;
ltc_frame current_frame;
uint8_t* current_bits[10];
bool state = false;
int bit_index = 0;
int bit_local_counter = 0;

static void periodic_timecode_callback(void* arg)
{
    // uint8_t bits[10] = &current_bits;
    // uint8_t byte = bits[bit_index/8];
    //get the bit, do logic checks, toggle pin.
}

void periodic_perframe_callback(void* arg) {
    current_simple_frame.frame++;
    if(current_simple_frame.frame > LTC_FRAMERATE) {
        current_simple_frame.frame = 0;
        current_simple_frame.second++;
        create_frame_from_timecode(&current_frame, current_simple_frame.frame, current_simple_frame.second, current_simple_frame.minute, current_simple_frame.hour);
        create_bits_from_frame(current_bits, current_frame);
    }
    if(current_simple_frame.second>60) {
        current_simple_frame.second = 0;
        current_simple_frame.minute++;
    }
    if(current_simple_frame.minute>60) {
        current_simple_frame.minute = 0;
        current_simple_frame.hour++;
    }
    if(current_simple_frame.hour>24) {
        current_simple_frame.hour = 0;
    }
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
    current_simple_frame.frame = 1;

    const esp_timer_create_args_t periodic_timecode_args = {
            .callback = &periodic_timecode_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic_timecode"
    };
    esp_timer_handle_t periodic_timecode;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timecode_args, &periodic_timecode));

    const esp_timer_create_args_t periodic_perframe_args = {
            .callback = &periodic_perframe_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic_perframe"
    };
    esp_timer_handle_t periodic_perframe;
    /* The timer has been created but is not running yet */

    ESP_ERROR_CHECK(esp_timer_create(&periodic_perframe_args, &periodic_perframe));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timecode, half_period_us));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_perframe, frame_period_us));
    
    
    ltc_frame testframe = {};
    create_frame_from_timecode(&testframe, 21, 16, 29, 05);

    uint8_t *test[10] = {};
    create_bits_from_frame(test, testframe);
    print_binary(test);

    ESP_LOGI(TAG, "%.6f", period_us);
    // while(true) {
    //     ESP_LOGI(TAG, "%u", current_frame.frame);
    //     sys_delay_ms(100);
    // }
}
