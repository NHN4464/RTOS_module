#include "i2c_config.h"

void i2c_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 26,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = 25,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_NUM_1, &conf);
    i2c_driver_install(I2C_NUM_1, I2C_MODE_MASTER, 0, 0, 0);
}
