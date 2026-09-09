
#ifndef SHIFTREG_H_
#define SHIFTREG_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define SR_DATA    0  // PC0
#define SR_CLOCK   1  // PC1
#define SR_STROBE  2  // PC2



void shiftreg_init(void);
void shiftreg_led_on(uint8_t led_num);
void shiftreg_led_off(uint8_t led_num);
void shiftreg_all_off(void);
void shiftreg_all_on(void);

#endif /* SHIFTREG_H_ */