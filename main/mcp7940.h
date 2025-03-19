#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driver/i2c_master.h"


#define MCP7940_I2C         0x6F
#define REG_SECONDS         0x00
#define REG_MINUTES         0x01
#define REG_HOUR            0x02
#define REG_WEEKDAY         0x03
#define REG_DATE            0x04
#define REG_MONTH           0x05
#define REG_YEAR            0x06
#define REG_CONTROL         0x07

#define HOUR_MODE_12        0x01
#define HOUR_MODE_24        0x00
#define AM_INDICATOR        0x00
#define PM_INDICATOR        0x01


#ifdef __cplusplus
extern "C" {
#endif

void init_rtc(int SCL_PIN, int SDA_PIN, int DEVICE_ADDR);

void set_rtc_hour_mode24(int HOUR);
void set_rtc_hour_mode12(int AM_OR_PM, int HOUR);

#ifdef __cplusplus
}
#endif
