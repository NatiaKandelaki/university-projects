#define F_CPU 9600000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define RELAY_PIN  PB1 // rele

// 4094 shift registri
#define DATA_PIN   PB0
#define STB_PIN    PB2
#define CLK_PIN    PB3
#define BUTTONS_ADC 0  // PB5 / ADC0
#define THERMISTOR_ADC 2   // PB4 / ADC2 thermistor

// Macroebi
#define SET(port, pin) (port |= (1 << pin))
#define CLR(port, pin) (port &= ~(1 << pin))

#define DP_BIT 0x80

//  0-9 (gfedcba)
const uint8_t digits[] = {  0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
0b01101111 }; // 9
//0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF};

//cifri 1 an 2
const uint8_t digit_select[2] = {
	0x01, // Right digit (Q0 of 2nd 4094)
	0x02  // Left digit (Q1 of 2nd 4094)
};
// ADC mnishvnelobebi 20 dan 60is cht, 2 is bijit
const uint16_t temp_thresholds[] = {
569,546,523,500,478,455,434,413,392,373,353,335,317,300,284,269,254,240,227,214,202};

typedef enum {
	BTN_NONE,     //0, gilaki araa dawerili
	BTN_INC,      //1 vumatebt
	BTN_DEC,      //2 vaklebt
	BTN_BOTH      //3 orivezea dawerili
} ButtonState;

volatile uint16_t timer_counter = 0;
volatile uint8_t display_mode = 0;
//volatile uint8_t timer_counter = 0;
/*
// Timer overflow interrupt handler
ISR(TIM0_OVF_vect) {
	timer_counter++;
	
	// Increase the delay between display modes to about 5 scs
	if (timer_counter >= 185) {
		display_mode = 1 - display_mode;
		timer_counter = 0;
	}
}
*/
ISR(TIM0_OVF_vect, ISR_NAKED) {
	asm volatile("push r16"); 
	asm volatile("in r16, __SREG__");
	asm volatile("push r16");

	asm volatile("sei"); // Re-enable interrupts if needed
	timer_counter++;
	if (timer_counter >= 185) {
		display_mode = 1 - display_mode;
		timer_counter = 0;
	}

	asm volatile("pop r16");
	asm volatile("out __SREG__, r16");
	asm volatile("pop r16");
	asm volatile("reti");
}

uint16_t read_adc(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x03); // ATtiny13 aqvs 4 ADC channels (0-3)
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

ButtonState read_buttons() {
	uint16_t adc = read_adc(BUTTONS_ADC);
	if (adc < 200) return BTN_BOTH;
	if (adc < 300) return BTN_DEC;
	if (adc < 700) return BTN_INC;
	return BTN_NONE;
}
//viyeneb 10k NTC thermistors
uint8_t adc_to_temperature(uint16_t adc) {
	for (uint8_t i = 0; i < 20; i++) {
		uint16_t high = temp_thresholds[i];
		uint16_t low = temp_thresholds[i + 1];

		if (adc >= high)
		return 20 + i * 2;
		
		if (adc < high && adc >= low) {
			uint8_t offset = ((high - adc) * 2) / (high - low);
			return 20 + i * 2 + offset; //oris bijit maqvs cxrilshi
		}
	}
	return 60;
}

// 7segmentaze gamotana
void shift_out(uint8_t data) {
	uint8_t i = 8;
	while (i--) {
		(data & 0x80) ? SET(PORTB, DATA_PIN) : CLR(PORTB, DATA_PIN);
		SET(PORTB, CLK_PIN);
		_delay_us(1);
		CLR(PORTB, CLK_PIN);
		data <<= 1;
	}
}

void display_temperature(uint8_t temp, bool show_dp) {
	uint8_t tens = temp / 10;
	uint8_t units = temp % 10;

	// Tens digit (left)
	shift_out(digit_select[1]);
	shift_out(digits[tens] & ~DP_BIT);
	SET(PORTB, STB_PIN); _delay_us(1); CLR(PORTB, STB_PIN);_delay_ms(5);
	

	// Units digit (right)
	shift_out(digit_select[0]);
	uint8_t units_pattern = digits[units];
	shift_out(show_dp ? (units_pattern | DP_BIT) : (units_pattern & ~DP_BIT));
	SET(PORTB, STB_PIN); _delay_us(1); CLR(PORTB, STB_PIN); _delay_ms(2);

}


void init_timer() {
	// viyeneb Timer0
	// Set prescaler to 1024
	TCCR0B = (1 << CS02) | (1 << CS00);
	
	// Enable timer overflow interrupt
	TIMSK0 = (1 << TOIE0);
	
	// Enable global interrupts
	sei();
}


int main(void) {
	DDRB = 0x0F;  // Set PB0-PB3 as outputs
	PORTB = (1 << RELAY_PIN);  // Relay off initially
	
	// Initialize ADC
	ADMUX = 0;
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) |    // Set prescaler to 64
	(1 << ADPS1);     // 9.6MHz/64 = 150kHz
	
	// Initialize timer
	init_timer();
	
	uint8_t set_temp = 30;  //aqedan daiwyebs
	uint8_t current_temp;
	ButtonState  btn_state = BTN_NONE;
	bool fan_state = 0;
	//	uint16_t raw_adc;
	
	while(1) {
		ButtonState  new_btn = read_buttons();
		if (new_btn != btn_state) {
			btn_state = new_btn;
			
			if (btn_state == BTN_INC && set_temp < 99) set_temp++;
			else if (btn_state == BTN_DEC && set_temp > 0) set_temp--;
		}
		
		// Read current temperature
		uint16_t raw_adc = read_adc(THERMISTOR_ADC);
		current_temp = adc_to_temperature(raw_adc);
		
		// reles kontroli
		if (!fan_state && current_temp >= set_temp) {
			CLR(PORTB, RELAY_PIN);
			fan_state = 1;
			} else if (fan_state && current_temp < set_temp) {
			SET(PORTB, RELAY_PIN);
			fan_state = 0;
		}
		
		display_temperature((display_mode == 0 ? set_temp : current_temp), display_mode == 1);
		_delay_ms(20);
		
	}
}