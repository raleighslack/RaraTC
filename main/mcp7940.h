#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_mac.h"
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
#define SCL_PIN             6
#define SDA_PIN             7
#define I2C_PORT_NUM        0

#define HOUR_MODE_12        0x01
#define HOUR_MODE_24        0x00
#define AM_INDICATOR        0x00
#define PM_INDICATOR        0x01

#define DAY_SUNDAY          1
#define DAY_MONDAY          2
#define DAY_TUESDAY         3
#define DAY_WEDNESDAY       4
#define DAY_THURSDAY        5
#define DAY_FRIDAY          6
#define DAY_SATURDAY        7

#ifdef __cplusplus
extern "C" {
#endif

static esp_err_t i2c_get_port(int port, i2c_port_t *i2c_port);
esp_err_t init_rtc(void);
//Getters
uint8_t get_rtc_seconds();
uint8_t get_rtc_minutes();
uint8_t get_rtc_hours();
uint8_t get_rtc_weekday();
uint8_t get_rtc_date();
uint8_t get_rtc_month();
uint8_t get_rtc_year();
uint8_t get_rtc_register(uint8_t REG);
//Setters
esp_err_t set_rtc_seconds(uint8_t seconds);
esp_err_t set_rtc_hour_mode24(uint8_t HOUR);
esp_err_t set_rtc_hour_mode12(uint8_t AM_OR_PM, uint8_t HOUR);
esp_err_t set_rtc_register(uint8_t REG, uint8_t DATA);

#ifdef __cplusplus
}
#endif
