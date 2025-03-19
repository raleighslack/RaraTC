#include "mcp7940.h"

i2c_master_dev_handle_t dev_handle;
i2c_master_bus_handle_t bus_handle;

void init_rtc(int SCL_PIN, int SDA_PIN, int DEVICE_ADDR) {
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };
    
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = DEVICE_ADDR,
        .scl_speed_hz = 200000,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    // ESP_ERROR_CHECK(i2c_master_transmit(bus_handle));
}

void set_rtc_hour_mode24(int HOUR) {
    //bit 7 is zero
    //bit 6 is 12 or 24 hour selection
    //bit 5-4 is from 0-2 in bcd
    //bit 3-0 is from 0-9 in bcd
}