#  Medication Reminder System

Course project — Agricultural University of Georgia, Faculty of Engineering and Technology

---

##  Overview

This project is an automated medication reminder system built around the **Atmega168** microcontroller. It helps patients — especially the elderly or those managing chronic conditions — stick to their medication schedule by tracking real time, storing reminder schedules, and alerting the user with sound, an LCD message, and an LED pointing to the correct pill container when it's time to take medication.

The device:
- Lets the user enter medication time, container number, and pill quantity via a keypad
- Uses a real-time clock (RTC) module to continuously track the current time
- Compares the current time against saved reminder schedules
- Triggers a buzzer, LCD message, and container LED when a scheduled time is reached
- Requires user confirmation (button press) to silence the alert once medication is taken
- Stores reminders in external EEPROM so schedules persist across power loss

---

##  How It Works

1. The user enters a reminder's date, time, container number, and pill count using a 4x3 matrix keypad.
2. This data is saved to external EEPROM (24C32), addressed by reminder "slot."
3. The DS1307 RTC continuously tracks seconds, minutes, hours, day, and date over I²C, keeping time even when the main power is off.
4. The microcontroller polls the RTC once per second (via a Timer1 Compare Match interrupt) and checks all active reminders against the current time.
5. When a scheduled reminder time is reached:
   - The buzzer sounds an alert
   - The LCD displays the reminder's date and which container/dose to take
   - The corresponding LED (via shift registers) lights up to indicate the container
6. The alert stays active until the user presses the confirmation button (`#`), which silences the buzzer and turns off the LED.
7. The main menu lets the user set the RTC time, add a reminder, view saved reminders, or delete a reminder.

---

##  Bill of Materials

| # | Component | Qty |
|---|---|---|
| 1 | Atmega168 microcontroller | 1 |
| 2 | DS1307 real-time clock module | 1 |
| 3 | 24C32 EEPROM (reminder storage) | 1 |
| 4 | 16x2 LCD (character display) | 1 |
| 5 | 4x3 matrix keypad | 1 |
| 6 | CD4094 shift register | 2 |
| 7 | LED (container indicators) | 10 |
| 8 | Transistor 2N2222 | 1 |
| 9 | Speaker/buzzer, 5V | 1 |
| 10 | Potentiometer, 10kΩ (LCD contrast) | 1 |
| 11 | Resistor: 1kΩ | 1 |
| 12 | Resistor: 4.7kΩ | 2 |
| 13 | Resistor: 220Ω | 10 |
| 14 | Resistor: 10kΩ | 1 |
| 15 | Capacitor, 10µF | 1 |
| 16 | Connector, 10-pin | – |
| 17 | Breadboard / prototyping board | 1 |
| 18 | Power supply (5V) | 1 |

---

##  Pinout Summary (Atmega168)

| Pin(s) | Function |
|---|---|
| PC0–PC2 | Shift register (CD4094) — data, clock, strobe for LED control |
| PB0–PB3 | Keypad rows (output) |
| PB4–PB6 | Keypad columns (input, pull-up) |
| PB7 | Buzzer (via transistor driver) |
| PD (RS, E, D4–D7) | LCD, 4-bit parallel interface |
| I²C (SDA/SCL) | Shared bus for DS1307 RTC and 24C32 EEPROM |

Circuit runs at **5V**, the safe supply voltage for the Atmega168. A transistor driver (2N2222) is used for the buzzer since it needs more current (30–80mA) than an MCU pin can safely supply (20–40mA).

---

##  Firmware

**Language:** C (AVR-GCC) · **MCU:** Atmega168 @ 8 MHz
**Communication:** I²C (TWI) for RTC and EEPROM

### File structure

| File | Purpose |
|---|---|
| `main.c` | Main program: menu logic, reminder scheduling, RTC polling, buzzer/LED triggering |
| `lcd.c` / `lcd.h` | 4-bit LCD driver (init, print, cursor control) |
| `keypad.c` / `keypad.h` | 4x3 matrix keypad scanning |
| `shiftreg.c` / `shiftreg.h` | CD4094 shift register driver for the 10 container LEDs |


### Main menu

```
1. Set Time   2. Add Reminder
3. View       4. Delete       * = Back to clock view
```

---

##  Build & Flash

1. Wire the circuit according to the pinout and bill of materials above.
2. Compile and flash the firmware to the Atmega168 using an AVR programmer with `avr-gcc` / `avrdude` (e.g. via ISP: MOSI/MISO/SCK/RESET).
3. Connect the power supply (5V).
4. On power-up, the LCD shows "Medication Reminder System," then the current date/time from the RTC.
5. Press any key to enter the main menu, set the time first (`1`), then add reminders (`2`).
6. When a reminder triggers, follow the LCD prompt and press `#` once medication is taken.

---

##  References

1. [CD4049UB Datasheet — TI](https://www.ti.com/lit/ds/symlink/cd4049ub.pdf)
2. [ATmega88/ATmega168 Datasheet — Microchip](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-9365-Automotive-Microcontrollers-ATmega88-ATmega168_Datasheet.pdf)
3. [HD44780 LCD Controller Datasheet](https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf)
4. [TCA8418 Keypad Controller Datasheet — TI](https://www.ti.com/lit/ds/symlink/tca8418.pdf)
