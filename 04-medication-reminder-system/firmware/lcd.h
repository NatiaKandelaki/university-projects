
#ifndef LCD_H_
#define LCD_H_
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define LCD_RS    2  // PD2,
#define LCD_E     3  // PD3
#define LCD_D4    4  // PD4
#define LCD_D5    5  // PD5
#define LCD_D6    6  // PD6
#define LCD_D7    7  // PD7

#define LCD_CLEAR_DISPLAY       0x01
#define LCD_RETURN_HOME         0x02
#define LCD_ENTRY_MODE_SET      0x06
#define LCD_DISPLAY_ON          0x0C
#define LCD_DISPLAY_OFF         0x08
#define LCD_CURSOR_ON           0x0E
#define LCD_CURSOR_BLINK        0x0F
#define LCD_FUNCTION_SET_4BIT   0x28


void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_putchar(char c);
void lcd_print(const char *str);
void lcd_command(uint8_t cmd);

#endif /* LCD_H_ */