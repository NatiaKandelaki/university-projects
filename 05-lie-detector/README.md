#  Lie Detector (Physiological Stress Monitor)

Course project — Agricultural University of Georgia, Faculty of Engineering and Technology

---

##  Overview

This project is a non-invasive, sensor-based system for measuring physiological stress responses — the same physiological basis used by polygraph examinations. It's built around the **Atmega168** microcontroller and streams live sensor data to **MATLAB** for real-time signal processing and visualization.

The system continuously measures three physiological signals:
- **Pulse (PPG — photoplethysmography):** heart rate and heart rate variability, via an infrared LED/photodiode pair
- **GSR (galvanic skin response):** skin conductance changes tied to sweat gland activity, one of the most sensitive indicators of emotional arousal
- **Skin temperature:** micro-changes in skin temperature via a digital DS18B20 sensor

Raw sensor data is amplified, filtered, and sampled by the microcontroller, then sent over UART to MATLAB, which applies digital filtering, peak detection, and BPM calculation, and plots all three signals live.

 **Important:** this system is built for scientific/educational purposes. It measures physiological stress, which can have many causes — it is **not** admissible as legal evidence and does not constitute a certified polygraph examination.

---

##  How It Works

1. **PPG (pulse) circuit:** An IR LED continuously shines light into the skin; a photodiode picks up the reflected light. Blood volume changes with each heartbeat change how much IR light is absorbed, producing a periodic analog signal tied to heart rate.
2. **GSR circuit:** A Wheatstone bridge measures skin resistance via electrodes. Sweat gland activity (triggered by stress/emotional arousal) increases skin conductance, unbalancing the bridge and producing a measurable differential signal.
3. **Temperature circuit:** A DS18B20 digital sensor (1-Wire protocol) tracks skin temperature, which drops slightly when blood vessels constrict under nervous system activation.
4. All three amplified/filtered analog signals (PPG, GSR) are read by the microcontroller's ADC at 50Hz; temperature is read separately over 1-Wire.
5. Data is packaged into 7-byte packets (`0xAA` header + PPG high/low + GSR high/low + TEMP high/low) and streamed over UART at 9600 baud.
6. MATLAB reads the serial stream, applies a 4th-order Butterworth bandpass filter (0.5–4 Hz) to the PPG signal, detects heartbeat peaks, and computes BPM — while plotting all four signals (raw PPG, filtered PPG with peaks, GSR, temperature) live.

### Signal chain (PPG)

```
IR LED/Photodiode → Op-amp gain stage (LM358N) → Twin-T notch filter (0.5–4 Hz bandpass)
   → Second gain stage → Diode-limited output (protects ADC) → MCU ADC → UART → MATLAB
```

### Signal chain (GSR)

```
Skin electrodes (Wheatstone bridge) → Differential amplifier (LM358N, gain = 10)
   → Low/high-frequency smoothing (C10, C11) → MCU ADC → UART → MATLAB
```

### Signal chain (Temperature)

```
DS18B20 (1-Wire) → MCU (bit-banged 1-Wire read) → UART → MATLAB
```

---

##  Bill of Materials

| # | Component | Qty |
|---|---|---|
| 1 | Atmega168 microcontroller | 1 |
| 2 | RS232 (serial interface) | 1 |
| 3 | LM358N dual op-amp | 3 |
| 4 | DS18B20 digital temperature sensor | 1 |
| 5 | Infrared LED (PPG emitter) | 1 |
| 6 | Photodiode (PPG receiver) | 1 |
| 7 | GSR electrodes | 2 |
| 8 | 7805 voltage regulator | 1 |
| 9 | Diode 1N4148 | 2 |
| 10 | Potentiometer: 10kΩ (4), 100kΩ (2) | 6 |
| 11 | Resistor: 1kΩ, 16.5kΩ, 10kΩ, 33kΩ, 100kΩ, 4.7kΩ | various |
| 12 | Capacitor: 4.7µF, 1µF, 100nF, 40µF | various |
| 13 | Connector, 10-pin | – |
| 14 | Breadboard / prototyping board | 1 |
| 15 | Power supply | 1 |

---

##  Pinout Summary (Atmega168)

| Pin | Function |
|---|---|
| PC0 (ADC0) | PPG analog input |
| PC1 (ADC1) | GSR analog input |
| PD2 | DS18B20 1-Wire data line (bit-banged) |
| UART (TX) | Streams sensor packets to MATLAB via serial |

Circuit runs at **7V** input, regulated down to **5V** via a 7805 regulator for the Atmega168 and analog front end.

---

##  Firmware

**Language:** C (AVR-GCC) · **MCU:** Atmega168 @ 8 MHz
**Sampling:** 50Hz (Timer0 CTC interrupt) · **UART:** 9600 baud

### Key components

- **Double-buffered sampling:** `ppg_buf1/2`, `gsr_buf1/2` (100 samples each = 2-second window) — one buffer fills while the other transmits, avoiding data loss.
- **ADC ISR chain:** `ISR(TIMER0_COMPA_vect)` kicks off a PPG conversion every 20ms; `ISR(ADC_vect)` reads PPG then immediately triggers a GSR conversion on the same tick, storing both into the active buffer.
- **1-Wire (DS18B20) driver:** `ow_reset()`, `ow_write_bit/byte()`, `ow_read_bit/byte()`, `ow_start_conversion()`, `ow_read_temp()` — a fully bit-banged 1-Wire implementation on PD2, since the temperature reading isn't time-critical and doesn't need a dedicated buffer.
- **UART packet transmission:** `ISR(USART_TX_vect)` sends a 7-byte packet per sample (`0xAA` sync byte, PPG high/low, GSR high/low, TEMP high/low), automatically continuing to the next sample until the full 100-sample buffer has been streamed.
- **Main loop:** starts a new DS18B20 conversion once the previous UART transmission finishes, keeping temperature readings fresh without blocking the sampling loop.

---

##  MATLAB (Signal Processing & Visualization)

**Requirements:** MATLAB with Signal Processing Toolbox (`butter`, `filtfilt`, `findpeaks`)

### What it does

1. Opens a serial connection (`COM4`, 9600 baud) and reads 7-byte packets synced on the `0xAA` header.
2. Converts raw ADC/DS18B20 values into physical units: PPG/GSR → volts (0–5V), temperature → °C (`raw × 0.0625`).
3. Applies a 4th-order Butterworth bandpass filter (0.5–4 Hz) to the PPG signal to isolate the heart-rate frequency band.
4. Smooths the filtered PPG with a 5-sample moving average, then detects heartbeat peaks with `findpeaks` (minimum peak distance and prominence tuned to physiological heart rate limits, 45–150 BPM).
5. Computes instantaneous BPM from peak-to-peak (RR) intervals and displays the running median.
6. Plots four live subplots: raw PPG, filtered PPG with detected peaks, GSR, and skin temperature.
7. At the end of the 100-sample window, re-runs peak detection on the full buffer, applies a 60%-amplitude validity rule to reject spurious peaks, and prints a summary: heartbeat count, average BPM, average GSR, and average skin temperature.

---

##  Build & Run

1. Wire the circuit according to the pinout and bill of materials above (PPG front end, GSR Wheatstone bridge, DS18B20, RS232 level shifting for serial).
2. Compile and flash the firmware to the Atmega168 using an AVR programmer with `avr-gcc` / `avrdude`.
3. Connect the device via USB-to-serial (RS232) to your computer.
4. In MATLAB, update the serial port name (`COM4`) in the script to match your system, then run the script.
5. Attach the PPG sensor to a fingertip, the GSR electrodes to two fingers (or palm), and the DS18B20 to the skin.
6. Watch the four live plots update in real time; after the buffer window completes, check the console for the BPM/GSR/temperature summary.
