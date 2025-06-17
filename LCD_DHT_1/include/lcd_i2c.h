#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>
#include <stdarg.h>
#include "driver/i2c.h"

#define RS 0
#define RW 1
#define EN 2
#define BL 3

#define DISPLAY_16X02 0

#define CLEAR_DISPLAY 0x01
#define RETURN_HOME_UNSHIFT 0x02
#define DISPLAY_ON_CURSOR_OFF 0x0C
#define CURSOR_RIGHT_NO_SHIFT_LEFT 0x06
#define SET_4BIT_MODE 0x28
#define RETURN_HOME 0x80

typedef struct {
    uint8_t address;
    uint8_t num;
    uint8_t backlight;
    uint8_t size;
} lcd_i2c_handle_t;

void i2c_write_byte(lcd_i2c_handle_t* lcd, uint8_t data);
void lcd_i2c_write(lcd_i2c_handle_t* lcd, char rs_flag, char data_byte);
void lcd_i2c_init(lcd_i2c_handle_t* lcd);
void lcd_i2c_cursor_set(lcd_i2c_handle_t* lcd, uint8_t col, uint8_t row);
void lcd_i2c_print(lcd_i2c_handle_t* lcd, const char* format, ...);

#endif
