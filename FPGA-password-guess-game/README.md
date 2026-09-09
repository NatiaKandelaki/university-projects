## FPGA Passcode Game

A fully hardware-implemented passcode guessing game written in synthesisable VHDL,
targeting the Digilent Anvyl development board (Xilinx Spartan-6 XC6SLX45, 100 MHz).

### How It Works
An administrator enters a secret 6-digit hexadecimal passcode using a 4×4 matrix keypad.
A player then has 5 attempts to guess it. After each guess, correct digits lock into their
positions permanently and remain visible in subsequent rounds — wrong digits are replaced
with a dash (–). The game ends when all 6 digits are guessed correctly (win) or all 5
lives are exhausted (loss).

### Features
- 4×4 matrix keypad driver with column scanning, 2-stage metastability synchroniser,
  20 ms debounce, and single-fire logic
- 6-digit multiplexed 7-segment display (~190 Hz refresh, hex digits 0–F + dash)
- Progressive digit-locking mechanic (Wordle / Mastermind style)
- Flicker animation on correct digits (4× 0.4 s blink) as visual feedback
- 5-LED lives indicator (thermometer encoding)
- 480×272 TFT LCD with custom pixel pipeline: smiley face on win (green screen),
  sad face on loss (red screen), bitmapped glyph text rendered entirely in hardware
- All logic is RTL VHDL — no processor, no software, no IP cores

### Project Structure
- keypad_display_test.vhd - top-level game FSM
- keypad_scanner.vhd     - keypad column-scan and debounce driver
- seven_segm_disp.vhd    - multiplexed 7-segment controller
- decoder.vhd            - 4-bit hex to 7-segment combinational decoder
- tft_display.vhd        - TFT video pipeline (timing, geometry, text)
- keypad_display_test.ucf - pin constraints for Digilent Anvyl

### Tools
Xilinx ISE 14.7 · VHDL-93 · Digilent Anvyl (Spartan-6 XC6SLX45)
