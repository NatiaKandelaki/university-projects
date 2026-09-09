
#ifndef KEYPAD_H_
#define KEYPAD_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// striqonebi
#define ROW0   3  // PB0
#define ROW1   2  // PB1
#define ROW2   1  // PB2
#define ROW3   0  // PB3

//svetebi
#define COL0   4  // PB4
#define COL1   5  // PB5
#define COL2   6  // PB6

void keypad_init(void);
char keypad_get_key(void);

#endif