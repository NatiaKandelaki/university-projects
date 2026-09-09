#  Temperature Monitoring and Control System

---

##  Overview

This project is a temperature monitoring and control system built around the **ATtiny13** microcontroller. It continuously reads ambient temperature via an NTC thermistor, lets the user set a target temperature using two buttons, displays both values on a 2-digit 7-segment display, and switches a relay (e.g. a cooling device) on or off based on the comparison between the target and measured temperature.

Possible use cases:
- Automatic cooling for electronic components
- Climate control in enclosed spaces
- Other temperature-dependent applications

---

##  How It Works

1. The user sets the desired temperature with two buttons (one increases, one decreases; pressing both at once confirms/locks the value).
2. An NTC thermistor produces an analog signal, which the microcontroller reads via its ADC and converts into a temperature value.
3. The 7-segment display alternates (every ~5 seconds) between the target and the measured temperature — the measured temperature is shown with a trailing decimal point (DP) to distinguish it.
4. If the measured temperature exceeds the target, the relay turns ON and powers the cooling device; otherwise it turns OFF.
5. The system runs in a continuous loop (`while(1)`).



##  Bill of Materials

| # | Component | Qty |
|---|---|---|
| 1 | ATtiny13 microcontroller | 1 |
| 2 | NTC thermistor, 10kΩ | 1 |
| 3 | CD4094 shift register | 2 |
| 4 | Dual (2-digit) 7-segment display | 1 |
| 5 | Relay, 5V | 1 |
| 6 | Transistor 2N2222 | 1 |
| 7 | Diode 1N4007 | 1 |
| 8 | Push button | 2 |
| 9 | Switch | 1 |
| 10 | Capacitor, 100µF | 1 |
| 11 | Resistor: 1kΩ | 9 |
| 12 | Resistor: 1MΩ | 1 |
| 13 | Resistor: 10kΩ | 3 |
| 14 | Resistor: 300Ω | 9 |
| 15 | Connectors (10-pin, 2-pin) | – |
| 16 | Breadboard / prototyping board | 1 |
| 17 | Power supply (~5.5V) | 1 |

---

##  Pinout (ATtiny13, PORTB)

| Pin | Function |
|---|---|
| PB0 | DATA — data input to the 4094 shift register |
| PB1 | RELAY — relay control (active-low) |
| PB2 | STROBE — 4094 strobe/latch |
| PB3 | CLOCK — 4094 clock signal |
| PB4 (ADC2) | Thermistor analog input |
| PB5 (ADC0) | Multi-level voltage divider for buttons |

### Button detection (by ADC value on PB5)

| ADC range | State |
|---|---|
| < 200 | Both buttons (confirm/lock) |
| 200–299 | Decrease (DEC) |
| 300–699 | Increase (INC) |
| ≥ 700 | None |

---

##  Firmware

**Language:** C (AVR-GCC) · **MCU:** ATtiny13 @ 9.6 MHz

### Key functions

- `read_adc(channel)` — reads an analog value from the given ADC channel.
- `read_buttons()` — determines which button is pressed based on the ADC reading.
- `adc_to_temperature(adc)` — converts an ADC reading into a temperature (°C) using a precomputed `temp_thresholds[]` lookup table (20°C–60°C range, 2°C steps).
- `shift_out(data)` — sends one byte to the CD4094 shift register.
- `display_temperature(temp, show_dp)` — outputs a 2-digit number to the 7-segment display, optionally with the decimal point.
- `init_timer()` — configures Timer0 (prescaler 1024) with an overflow interrupt roughly every 5 seconds, toggling the display mode (target ↔ measured temperature).
- `main()` — initialization plus the main loop: read buttons → read temperature → relay logic → update display.

### ADC-to-temperature lookup table formulas

```
R = R0 · e^(B(1/T − 1/T0))          // NTC thermistor resistance
ADC = 1023 · Rdiv / (R + Rdiv)      // voltage divider → ADC reading
```

---


##  References

1. [herbert-dingfelder.de](http://www.herbert-dingfelder.de/?page_id=568)
2. [Engineering Fundamentals – An Introduction to Engineering (AIU)](https://students.aiu.edu/submissions/profiles/resources/onlineBook/M4K3T4_Engineering%20Fundamentals_%20An%20Introduction%20to%20Engineering,%20Fourth%20Edition%20(2010).pdf)
3. [ATtiny13A Datasheet — Microchip](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny13A-Data-SheetDS40002307A.pdf)
4. [CD4094B Datasheet — TI](https://www.ti.com/lit/ds/symlink/cd4094b.pdf)
5. [Farnell Datasheet](https://www.farnell.com/datasheets/95204.pdf)
