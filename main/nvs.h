#pragma once

#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "hal/cpu_hal.h"

uint64_t get_serial_no();
esp_err_t set_serial_no(uint64_t serial_number);
esp_err_t nvs_rara_init();
esp_err_t nvs_rara_deinit();