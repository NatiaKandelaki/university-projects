
#include "lcd.h"

static void lcd_enable_pulse(void) {
    PORTD |= (1 << LCD_E);    // E HIGH
    _delay_us(1);             
    PORTD &= ~(1 << LCD_E);   // E LOW
    _delay_us(50);            
}

static void lcd_send_nibble(uint8_t nibble) {
    PORTD &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
                  
    PORTD |= (nibble & 0x0F) << 4;   
    lcd_enable_pulse();               
}


static void lcd_send_byte(uint8_t rs, uint8_t byte) {
    if (rs) {
        PORTD |= (1 << LCD_RS);       // RS = 1 
    } else {
        PORTD &= ~(1 << LCD_RS);      // RS = 0 
    }
    
    lcd_send_nibble(byte >> 4);      
    lcd_send_nibble(byte);           
    _delay_us(50);                    
}

void lcd_command(uint8_t cmd) {
    lcd_send_byte(0, cmd);  // RS = 0 for commands
    _delay_ms(2);          
}

void lcd_putchar(char c) {
    lcd_send_byte(1, c);    // RS = 1 for data
    _delay_us(50);
}

void lcd_init(void) {
    //gamosasvlelebad
    DDRD |= (1 << LCD_RS) | (1 << LCD_E) | 
            (1 << LCD_D4) | (1 << LCD_D5) | 
            (1 << LCD_D6) | (1 << LCD_D7);
    
    _delay_ms(50);
    
    PORTD &= ~(1 << LCD_RS);  // RS = 0 
    
    lcd_send_nibble(0x03);
    _delay_ms(5);            
    
    lcd_send_nibble(0x03);
    _delay_us(150);       
    
    lcd_send_nibble(0x03);
    _delay_ms(1);
    
    //4-bit mode
    lcd_send_nibble(0x02);
    _delay_ms(1);
    
    lcd_command(LCD_FUNCTION_SET_4BIT);
    
    lcd_command(LCD_DISPLAY_ON);
    
    lcd_command(LCD_CLEAR_DISPLAY);
    _delay_ms(2);
    
    lcd_command(LCD_ENTRY_MODE_SET);
}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR_DISPLAY);
    _delay_ms(2);  
}

void lcd_set_cursor(uint8_t row, uint8_t colmn) {
    uint8_t address;
    if (row == 0) {
        address = 0x00 + colmn;
    } else {
        address = 0x40 + colmn;
    }
    
    lcd_command(0x80 | address);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_putchar(*str);
        str++;
    }
}