#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driver/i2c_master.h"


#define MCP7940_I2C         0x6F
#define REG_SECONDS         0x00
#define REG_MINUTES         0x42
#define REG_HOUR            0x03
#define REG_WEEKDAY         0x03
#define REG_DATE            0x04
#define REG_MONTH           0x05
#define REG_YEAR            0x06


#ifdef __cplusplus
extern "C" {
#endif

void create_rtc(int SCL_PIN, int SDA_PIN, int DEVICE_ADDR);

#ifdef __cplusplus
}
#endif
