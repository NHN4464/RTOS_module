#include "lcd_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include <string.h>

void i2c_write_byte(lcd_i2c_handle_t* lcd, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (lcd->address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(lcd->num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void lcd_i2c_write(lcd_i2c_handle_t* lcd, char rs_flag, char data_byte) {
    uint8_t hi = (data_byte & 0xF0);
    uint8_t lo = (data_byte << 4) & 0xF0;
    uint8_t ctrl = (rs_flag << RS) | (lcd->backlight << BL);

    for (int i = 0; i < 2; i++) {
        uint8_t data = (i == 0) ? hi : lo;
        uint8_t packet = data | ctrl | (1 << EN);
        i2c_write_byte(lcd, packet);
        ets_delay_us(10);
        packet &= ~(1 << EN);
        i2c_write_byte(lcd, packet);
        ets_delay_us(50);
    }

    if (data_byte == CLEAR_DISPLAY)
        vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_i2c_init(lcd_i2c_handle_t* lcd) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    lcd_i2c_write(lcd, 0, RETURN_HOME_UNSHIFT);
    lcd_i2c_write(lcd, 0, SET_4BIT_MODE);
    lcd_i2c_write(lcd, 0, CLEAR_DISPLAY);
    lcd_i2c_write(lcd, 0, DISPLAY_ON_CURSOR_OFF);
    lcd_i2c_write(lcd, 0, CURSOR_RIGHT_NO_SHIFT_LEFT);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void lcd_i2c_cursor_set(lcd_i2c_handle_t* lcd, uint8_t col, uint8_t row) {
    uint8_t pos = (row == 0) ? 0x00 : 0x40;
    lcd_i2c_write(lcd, 0, 0x80 + pos + col);
}

void lcd_i2c_print(lcd_i2c_handle_t* lcd, const char* format, ...) {
    char buffer[64];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    for (int i = 0; i < strlen(buffer); i++) {
        lcd_i2c_write(lcd, 1, buffer[i]);
    }
}
