#include "dht_display.h"
#include "i2c_config.h"
#include "lcd_i2c.h"
#include "dht11.h"

void vDisplay(void* pvParam) {
    i2c_init();

    lcd_i2c_handle_t lcd = {
        .address = 0x27,
        .num = I2C_NUM_1,
        .backlight = 1,
        .size = DISPLAY_16X02
    };

    lcd_i2c_init(&lcd);
    DHT11_init(GPIO_NUM_17);

    while (1) {
        struct dht11_reading dht = DHT11_read();

        lcd_i2c_write(&lcd, 0, CLEAR_DISPLAY);
        lcd_i2c_cursor_set(&lcd, 0, 0);
        lcd_i2c_print(&lcd, "Temp: %dC", dht.temperature);
        lcd_i2c_cursor_set(&lcd, 0, 1);
        lcd_i2c_print(&lcd, "Humi: %d%%", dht.humidity);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
