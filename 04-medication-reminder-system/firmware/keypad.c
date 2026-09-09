#include "keypad.h"

const char keys[4][3] = {
	{'1', '2', '3'},
	{'4', '5', '6'},
	{'7', '8', '9'},
	{'*', '0', '#'}
};

void keypad_init(void) {
	DDRB |= (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);

	DDRB &= ~((1 << COL0) | (1 << COL1) | (1 << COL2));
	PORTB |= (1 << COL0) | (1 << COL1) | (1 << COL2);

	PORTB |= (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
}

char keypad_get_key(void) {
	for (uint8_t row = 0; row < 4; row++) {
		PORTB |= (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
		PORTB &= ~(1 << (ROW0 - row));

		_delay_us(5);

		for (uint8_t col = 0; col < 3; col++) {
			if (!(PINB & (1 << (COL0 + col)))) {
				_delay_ms(20); 

				while (!(PINB & (1 << (COL0 + col))));

				return keys[row][col];
			}
		}
	}
	return 0;
}
