#include "mcp7940.h"

esp_err_t init_rtc(void) {
    int i2c_master_port = 0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

void set_rtc_hour_mode24(int HOUR) {
    //bit 7 is zero
    //bit 6 is 12 or 24 hour selection
    //bit 5-4 is from 0-2 in bcd
    //bit 3-0 is from 0-9 in bcd
}

uint8_t get_rtc_register(uint8_t REG) {
    uint8_t data;
    ESP_ERROR_CHECK(i2c_master_write_read_device(0, MCP7940_I2C, &REG, 1, &data, 1, -1));
    return data;
}

esp_err_t set_rtc_register(uint8_t REG, uint8_t DATA) {
    int ret;
    uint8_t write_buf[2] = {REG, DATA};

    return i2c_master_write_to_device(0, MCP7940_I2C, write_buf, sizeof(write_buf), -1);
}