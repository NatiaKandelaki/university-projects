#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/eeprom.h>

#include "lcd.h"
#include "keypad.h"
#include "shiftreg.h"

#define BUZZER_PIN 7     // PB7 
void buzzer_init(void) {
	DDRB |= (1 << BUZZER_PIN);   
	PORTB &= ~(1 << BUZZER_PIN); 
}

void buzzer_on(void) {
	PORTB |= (1 << BUZZER_PIN);
}

void buzzer_off(void) {
	PORTB &= ~(1 << BUZZER_PIN);
}
void buzzer_beep(uint16_t duration_ms) {
	buzzer_on();
	for (uint16_t i = 0; i < duration_ms; i++) {
		_delay_ms(1);
	}
	buzzer_off();
}
void buzzer_alert(void) {
	for (uint8_t i = 0; i < 3; i++) {
		buzzer_beep(200);
		_delay_ms(200);
	}
}
//es aris RTC
//SD serial data (sends and receives), SC serial clock (masteria when to read and write)
//0x68 0x68 << 1 | 0
#define F_SCL 100000UL
#define Prescaler 1
#define TWBR_VALUE ((((F_CPU / F_SCL) / Prescaler) - 16 ) / 2)

void TWI_init(void) {
	TWSR = 0x00;
	TWBR = (uint8_t)TWBR_VALUE;
}

void TWI_start(void) {
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

void TWI_stop(void) {
	TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN);
}

void TWI_write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

uint8_t TWI_read_ack(void) {
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}
uint8_t TWI_read_nack(void) {
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}
#define DS1307_ADDR 0x68

uint8_t bcd_to_decimal(uint8_t val) { return (val >> 4) * 10 + (val & 0x0F); }
uint8_t decimal_to_bcd(uint8_t val) { return ((val / 10) << 4) | (val % 10); }

void DS1307_init(void) {
	TWI_init();
}

void DS1307_set_time(uint8_t sec, uint8_t min, uint8_t hour,
uint8_t day, uint8_t date, uint8_t month, uint8_t year) {
	TWI_start();
	TWI_write((DS1307_ADDR << 1) | 0); 
	TWI_write(0x00);                  
	TWI_write(decimal_to_bcd(sec));
	TWI_write(decimal_to_bcd(min));
	TWI_write(decimal_to_bcd(hour));
	TWI_write(decimal_to_bcd(day));
	TWI_write(decimal_to_bcd(date));
	TWI_write(decimal_to_bcd(month));
	TWI_write(decimal_to_bcd(year));
	TWI_stop();
}

void DS1307_get_time(uint8_t *sec, uint8_t *min, uint8_t *hour,
uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year) {
	TWI_start();
	TWI_write((DS1307_ADDR << 1) | 0); 
	TWI_write(0x00);                  
	TWI_start();
	TWI_write((DS1307_ADDR << 1) | 1); 

	*sec   = bcd_to_decimal(TWI_read_ack() & 0x7F);
	*min   = bcd_to_decimal(TWI_read_ack());
	*hour  = bcd_to_decimal(TWI_read_ack());
	*day   = bcd_to_decimal(TWI_read_ack());
	*date  = bcd_to_decimal(TWI_read_ack());
	*month = bcd_to_decimal(TWI_read_ack());
	*year  = bcd_to_decimal(TWI_read_nack());

	TWI_stop();
}

//keypad

uint16_t get_number(uint8_t digits) {
	char input[5] = {0}; 
	uint8_t count = 0;
	char key;
	uint16_t result = 0;

	while (1) {
		key = keypad_get_key();

		if (key >= '0' && key <= '9') {
			if (count < digits) {
				input[count++] = key;
				lcd_putchar(key);
			}
			_delay_ms(200);
		}
//* washla
		else if (key == '*') {
			if (count > 0) {
				count--;
				input[count] = '\0';

				lcd_set_cursor(1, count);
				lcd_putchar(' ');
				lcd_set_cursor(1, count);
			}
			_delay_ms(200);
		}
//# sheyvana
		else if (key == '#') {
			break;
		}
	}

	for (uint8_t i = 0; i < count; i++) {
		result = result * 10 + (input[i] - '0');
	}

	return result;
}

int main(void) {
 lcd_init();
 keypad_init();

 uint16_t year;
 uint8_t month, day, hour, minute;

 while (1) {
	 lcd_clear();
	 lcd_print("Enter Year :");
	 lcd_set_cursor(1, 0);
	 year = get_number(4);
	 if (year < 2000 || year > 2099) {
		 lcd_clear();
		 lcd_print("Invalid Year!");
		 _delay_ms(1000);
		 continue;
	 }

	 lcd_clear();
	 lcd_print("Enter Month :");
	 lcd_set_cursor(1, 0);
	 month = get_number(2);
	 if (month < 1 || month > 12) {
		 lcd_clear();
		 lcd_print("Invalid Month!");
		 _delay_ms(1000);
		 continue;
	 }

	 lcd_clear();
	 lcd_print("Enter Day :");
	 lcd_set_cursor(1, 0);
	 day = get_number(2);
	 if (day < 1 || day > 31) {
		 lcd_clear();
		 lcd_print("Invalid Day!");
		 _delay_ms(1000);
		 continue;
	 }

	 lcd_clear();
	 lcd_print("Enter Hour :");
	 lcd_set_cursor(1, 0);
	 hour = get_number(2);
	 if (hour > 23) {
		 lcd_clear();
		 lcd_print("Invalid Hour!");
		 _delay_ms(1000);
		 continue;
	 }

	 lcd_clear();
	 lcd_print("Enter Minute :");
	 lcd_set_cursor(1, 0);
	 minute = get_number(2);
	 if (minute > 59) {
		 lcd_clear();
		 lcd_print("Invalid Minute!");
		 _delay_ms(1000);
		 continue;
	 }

	 lcd_clear();
	 lcd_print("Saved in memory:");
	 _delay_ms(1500);

	 char buf[17];
	 lcd_clear();
	 sprintf(buf, "%04d/%02d/%02d", year, month, day);
	 lcd_print(buf);
	 lcd_set_cursor(1, 0);
	 sprintf(buf, "%02d:%02d", hour, minute);
	 lcd_print(buf);

	 _delay_ms(4000);
 }
	/*
	  lcd_init();
	  DS1307_init();
	  shiftreg_init(); 
      buzzer_init();
	  
	  lcd_clear();
	  lcd_print("LED Test");
	  _delay_ms(1000);
	  
	  for (uint8_t i = 1; i <= 15; i++) {
		  lcd_clear();
		  lcd_set_cursor(0, 0);
		  lcd_print("LED: ");
		  char buf[3];
		  sprintf(buf, "%d", i);
		  lcd_print(buf);
		  
		  shiftreg_led_on(i);
		  _delay_ms(500);
	  }
	  
	  _delay_ms(2000);
	  
	  shiftreg_all_off();
	  
	lcd_init();
	DS1307_init();


	uint8_t sec, min, hour, day, date, month, year;
	char buffer[17];

	while (1) {
		DS1307_get_time(&sec, &min, &hour, &day, &date, &month, &year);

		lcd_set_cursor(0, 0);
		sprintf(buffer, "%02d:%02d:%02d", hour, min, sec);
		lcd_print(buffer);

		lcd_set_cursor(1, 0);
		sprintf(buffer, "%02d/%02d/20%02d", date, month, year);
		lcd_print(buffer);

		_delay_ms(1000);
	}*/
}
