
#include "shiftreg.h"

static uint8_t led_state[2] = {0, 0};

void shiftreg_init(void) {
    DDRC |= (1 << SR_DATA) | (1 << SR_CLOCK) | (1 << SR_STROBE);
    
    PORTC &= ~((1 << SR_DATA) | (1 << SR_CLOCK) | (1 << SR_STROBE));
    
    shiftreg_all_off();
}


static void shiftreg_update(void) {
    PORTC &= ~(1 << SR_STROBE);    
    for (int8_t i = 15; i >= 0; i--) {
        uint8_t byte_index = i / 8; 
        uint8_t bit_index = i % 8;  
        
        uint8_t bit_value;
        if (i < 15) { 
            bit_value = (led_state[byte_index] >> bit_index) & 0x01;
        } else {
            bit_value = 0;  
        }
        
        if (bit_value) {
            PORTC |= (1 << SR_DATA);
        } else {
            PORTC &= ~(1 << SR_DATA);
        }
        
        PORTC |= (1 << SR_CLOCK);
        _delay_us(1);
        PORTC &= ~(1 << SR_CLOCK);
        _delay_us(1);
    }
    
    PORTC |= (1 << SR_STROBE);
    _delay_us(1);
    PORTC &= ~(1 << SR_STROBE);
}

void shiftreg_led_on(uint8_t led_num) {
    if (led_num < 1 || led_num > 15) return;
    
    led_num--;  
    
    uint8_t byte_index = led_num / 8;
    uint8_t bit_index = led_num % 8;
    
    led_state[byte_index] |= (1 << bit_index);  
    
    shiftreg_update(); 
}

void shiftreg_led_off(uint8_t led_num) {
    if (led_num < 1 || led_num > 15) return;
    
    led_num--;
    
    uint8_t byte_index = led_num / 8;
    uint8_t bit_index = led_num % 8;
    
    led_state[byte_index] &= ~(1 << bit_index);  
    
    shiftreg_update();
}

void shiftreg_all_off(void) {
    led_state[0] = 0;
    led_state[1] = 0;
    shiftreg_update();
}

void shiftreg_all_on(void) {
    led_state[0] = 0xFF;  
    led_state[1] = 0x7F;  
    shiftreg_update();
}