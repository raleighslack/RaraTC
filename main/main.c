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
#include "smpte_timecode.h"
#include <string.h>

#define LTC_OUTPUT_PIN      4
#define LTC_FRAMERATE       24

static const char *TAG = "main";

void print_binary(uint8_t bits[10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (bits[i] >> j) & 1);
        }
    }
    printf("\n");
}

void app_main(void)
{
    ltc_frame testframe = {};
    create_frame_from_timecode(&testframe, 22, 16, 29, 05);

    two_digits twodigis = {};
    convert_digits_to_single(&twodigis, testframe.frame);
    ESP_LOGI(TAG, "LSB: %u, MSB: %u", twodigis.lsb, twodigis.msb);

    uint8_t test[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    print_binary(test);
}
