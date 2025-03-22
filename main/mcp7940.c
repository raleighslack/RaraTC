#include "mcp7940.h"

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

static esp_err_t i2c_get_port(int port, i2c_port_t *i2c_port) {
    if (port >= I2C_NUM_MAX) {
        return ESP_FAIL;
    }
    *i2c_port = port;
    return ESP_OK;
}

esp_err_t init_rtc(void) {
    i2c_port_t port;
    ESP_ERROR_CHECK(i2c_get_port(0, &port));
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = port,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };
    
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MCP7940_I2C,
        .scl_speed_hz = 400000,
    };
    
    return i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
}

uint8_t get_rtc_seconds() {
    uint8_t data;
    uint8_t register_sec = REG_SECONDS;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    uint8_t tens = (data >> 4) & 0b0111;                                            // Extract bits 6-4
    uint8_t ones = data & 0b1111;                                                   // Extract bits 3-0
    return (tens * 10) + ones;
}

uint8_t get_rtc_minutes() {
    uint8_t data;
    uint8_t register_sec = REG_MINUTES;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    uint8_t tens = (data >> 4) & 0b0111;                                            // Extract bits 6-4
    uint8_t ones = data & 0b1111;                                                   // Extract bits 3-0
    return (tens * 10) + ones;
}

uint8_t get_rtc_hours() {
    uint8_t data;
    uint8_t register_sec = REG_HOUR;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    bool isTwelveHourMode = (data & 0b01000000) != 0;
    uint8_t tens;
    if(isTwelveHourMode) {
        tens = (data >> 4) & 0b0011;
    } else {
        tens = (data >> 4) & 0b0001;
    }
    uint8_t ones = data & 0b1111;                                                   // Extract bits 3-0
    return (tens * 10) + ones;
}

uint8_t get_rtc_weekday() {
    uint8_t data;
    uint8_t register_sec = REG_WEEKDAY;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    uint8_t ones = data & 0b0111;                                                   // Extract bits 2-0
    return ones;
}

uint8_t get_rtc_date() {
    uint8_t data;
    uint8_t register_sec = REG_DATE;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    uint8_t tens = (data >> 4) & 0b0011;                                            // Extract bits 6-4
    uint8_t ones = data & 0b1111;                                                   // Extract bits 3-0
    return (tens * 10) + ones;
}

uint8_t get_rtc_month() {
    uint8_t data;
    uint8_t register_sec = REG_MONTH;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    uint8_t tens = (data >> 4) & 0b0001;                                            // Extract bits 6-4
    uint8_t ones = data & 0b1111;                                                   // Extract bits 3-0
    return (tens * 10) + ones;
}

uint8_t get_rtc_year() {
    uint8_t data;
    uint8_t register_sec = REG_YEAR;
    i2c_master_transmit_receive(dev_handle, &register_sec, 1, &data, 1, -1);
    uint8_t tens = (data >> 4) & 0b1111;                                            // Extract bits 6-4
    uint8_t ones = data & 0b1111;                                                   // Extract bits 3-0
    return (tens * 10) + ones;
}

esp_err_t set_rtc_seconds(uint8_t seconds) {
    uint8_t tens = (seconds / 10) & 0b0111;                                         // Extract tens place and mask
    uint8_t ones = (seconds % 10) & 0b1111;                                         // Extract ones place and mask
    uint8_t encoded = (1 << 7) | (tens << 4) | ones;                                // Set flag bit, shift tens, and add ones
    uint8_t write_buf[2] = {REG_SECONDS, encoded};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), -1);
}

esp_err_t set_rtc_minutes(uint8_t minutes) {
    uint8_t tens = (minutes / 10) & 0b0111;
    uint8_t ones = (minutes % 10) & 0b1111;
    uint8_t encoded = ((tens << 4) | ones);
    uint8_t write_buf[2] = {REG_MINUTES, encoded};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), -1);
}

esp_err_t set_rtc_hour_mode24(uint8_t hour) {
    uint8_t tens = (hour / 10) & 0b0011;
    uint8_t ones = (hour % 10) & 0b1111;
    uint8_t encoded = (HOUR_MODE_24 << 6) | (tens << 4) | ones;
    uint8_t buffer[2] = {REG_HOUR, encoded};
    return i2c_master_transmit(dev_handle, buffer, sizeof(buffer), -1);
}

esp_err_t set_rtc_date_from_bt(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hour, uint8_t minute, uint8_t second) {
    set_rtc_register(REG_SECONDS, 0x00);//Stops the rtc from counting.
    set_rtc_register(REG_CONTROL, 0x00);//Stops the rtc from counting using the external oscillator. 
    esp_err_t status = ESP_OK;

    uint8_t yeartens = (year / 10) & 0b1111;
    uint8_t yearones = (year % 10) & 0b1111;
    uint8_t yearencoded = ((yeartens << 4) | yearones);
    uint8_t yearbuffer[2] = {REG_YEAR, yearencoded};
    status += i2c_master_transmit(dev_handle, yearbuffer, sizeof(yearbuffer), -1);

    uint8_t monthtens = (month / 10) & 0b0001;
    uint8_t monthones = (month % 10) & 0b1111;
    bool isLeapYear = (year%4==0) ? true : false;
    uint8_t monthencoded = (isLeapYear << 6) | (monthtens << 4) | monthones;
    uint8_t monthbuffer[2] = {REG_MONTH, monthencoded};
    status += i2c_master_transmit(dev_handle, monthbuffer, sizeof(monthbuffer), -1);

    uint8_t datetens = (date / 10) & 0b0011;
    uint8_t dateones = (date % 10) & 0b1111;
    uint8_t dateencoded = (datetens << 4) | dateones;
    uint8_t datebuffer[2] = {REG_DATE, dateencoded};
    status += i2c_master_transmit(dev_handle, datebuffer, sizeof(datebuffer), -1);

    uint8_t weekdayencoded = weekday & 0b0111;
    uint8_t weekdaybuffer[2] = {REG_WEEKDAY, weekdayencoded};
    status += i2c_master_transmit(dev_handle, weekdaybuffer, sizeof(weekdaybuffer), -1);

    status += set_rtc_hour_mode24(hour);

    status += set_rtc_minutes(minute);

    status += set_rtc_register(REG_CONTROL, 0x48);
    status += set_rtc_seconds(second);
    return status;
}

uint8_t get_rtc_register(uint8_t REG) {
    uint8_t data;
    i2c_master_transmit_receive(dev_handle, &REG, 1, &data, 1, -1);
    return data;
}

esp_err_t set_rtc_register(uint8_t REG, uint8_t DATA) {
    uint8_t write_buf[2] = {REG, DATA};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), -1);
}