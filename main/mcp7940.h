#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_mac.h"
#include "driver/i2c.h"


#define MCP7940_I2C         0x6F
#define REG_SECONDS         0x00
#define REG_MINUTES         0x01
#define REG_HOUR            0x02
#define REG_WEEKDAY         0x03
#define REG_DATE            0x04
#define REG_MONTH           0x05
#define REG_YEAR            0x06
#define REG_CONTROL         0x07
#define SCL_PIN             6
#define SDA_PIN             7
#define I2C_PORT_NUM        0

#define HOUR_MODE_12        0x01
#define HOUR_MODE_24        0x00
#define AM_INDICATOR        0x00
#define PM_INDICATOR        0x01


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t init_rtc(void);

void set_rtc_hour_mode24(int HOUR);
void set_rtc_hour_mode12(int AM_OR_PM, int HOUR);
uint8_t get_rtc_register(uint8_t REG);
esp_err_t set_rtc_register(uint8_t REG, uint8_t DATA);

#ifdef __cplusplus
}
#endif
