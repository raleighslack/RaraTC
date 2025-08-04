#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc_cal.h"
#include "esp_mac.h"
#include "driver/adc.h"

float get_lipo_voltage();
esp_err_t init_lipo();
int get_lipo_percent();