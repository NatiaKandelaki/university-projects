# Burglar Alarm System with Location Identification

An analog intrusion-detection system built as a coursework project at the Georgian Agrarian University, Faculty of Engineering and Technology.

The system monitors 4 independent "rooms" over a single control wire and identifies which room has been breached using a dedicated colored LED per zone — without requiring any digital logic.

## How it works

The design is built entirely from op-amp comparators, avoiding the need for a microcontroller or digital circuitry, which keeps it simple and low-cost for basic home security applications.

1. A 12V single-supply source powers a voltage-divider network of four resistors (R1–R4), each paired with a push-button that simulates an intrusion in its "room."
2. Pressing a button changes the voltage on the shared control line, simulating someone breaching that specific room. In a real deployment, this single wire would connect a control room to the sensors in each monitored room.
3. A voltage-follower op-amp (U6:A) buffers this incoming signal, isolating the sensor line from the rest of the circuit so downstream loading doesn't distort the reading.
4. That buffered voltage feeds five comparator stages (TL082 op-amps), each tuned via its own potentiometer to a distinct voltage threshold — one per room.
5. When the input voltage crosses a comparator's threshold, its output switches and lights the corresponding colored LED, telling the observer which room triggered the alarm.

**Signal path:**

```
Power Supply → Control Object (room sensors) → Buffer (Voltage Follower) → Comparators → LEDs
```

## Block diagram

```
┌─────────────┐     ┌───────────────────┐     ┌────────────────────────────┐     ┌──────────────┐     ┌─────┐
│ Power Supply │ --> │ Control Object     │ --> │ Control Operational        │ --> │ Comparator    │ --> │ LED │
│              │     │ (room sensors)     │     │ Amplifier (buffer)         │     │               │     │     │
└─────────────┘     └───────────────────┘     └────────────────────────────┘     └──────────────┘     └─────┘
```

## Design notes

- **Sensor network (R1–R4):** four resistors, each in parallel with a push-button, forming a voltage divider. Pressing a button produces a distinct voltage per room, simulating intrusion at that location. A fifth resistor (R5) represents the control room in the block diagram concept.
- **U6:A (voltage follower):** isolates the shared sensor line from the comparator stages so that a voltage change in one room doesn't affect the reading of another — each room's signal stays independent.
- **Comparators (U1–U5, TL082):** each dual op-amp package provides two comparator channels. When the non-inverting input (+) exceeds the inverting input (–), the output swings high (~12V) and lights the LED; otherwise the output stays low (~0V).
- **Potentiometers (RV1–RV10):** set the reference (threshold) voltage on each comparator's inverting/non-inverting input, allowing fine calibration so each room's voltage triggers only its own LED without overlapping adjacent thresholds.
- **R6, R7:** chosen so the voltage dropped across them roughly matches the voltage drop at their corresponding "room," keeping the comparator's reference in the right range.

## Key calculations

**LED current-limiting resistors** (R8, R11, R14, R17, R20), using:

```
R = (V - V_LED) / I_LED
```

With a 12V supply and 20 mA LED current, and LED forward voltages of: red 2.2V, yellow/green 2.6V, blue/white 3.1V:

```
R_red            = (12 − 2.2) / 0.02 ≈ 490 Ω
R_yellow, green  = (12 − 2.6) / 0.02 ≈ 470 Ω
R_blue, white    = (12 − 3.1) / 0.02 ≈ 445 Ω
```

R1–R4 (the room sensor resistors) were chosen so that the voltage produced by each button press is sufficiently spaced from the others — this keeps LED thresholds from overlapping and allows all rooms to share a single control wire.

## Components used

| Component | Value | Qty |
|---|---|---|
| Resistor | 24kΩ | 1 |
| Resistor | 33kΩ | 1 |
| Resistor | 47kΩ | 2 |
| Resistor | 10kΩ | 5 |
| Resistor | 1kΩ | 2 |
| Resistor | 1.3kΩ | 2 |
| Resistor | 1.8kΩ | 1 |
| Resistor | 500Ω | 2 |
| Resistor | 430Ω | 2 |
| Resistor | 470Ω | 1 |
| Potentiometer | 10kΩ | 10 |
| Op-amp | TL082 | 6 |
| LED | Green | 1 |
| LED | Red | 1 |
| LED | Blue | 1 |
| LED | Yellow | 1 |
| LED | White | 1 |
| Push-button | — | 4 |

## Limitations & future work

- Currently limited to four rooms plus the control line — scaling to more zones would require more comparator stages and careful threshold spacing.
- Uses simulated intrusion (button press) rather than real sensors (PIR, magnetic reed, etc.); swapping in real sensors at each room would be the next step toward a deployable system.
- Purely analog design — no logging, remote alerting, or digital integration (e.g., microcontroller-based notification) yet.

## References

- [Texas Instruments — TL082 datasheet](https://www.ti.com/lit/ds/symlink/tl082-q1.pdf)
- [Components101 — 5mm Round LED](https://components101.com/diodes/5mm-round-led)
- [Components101 — Potentiometer](https://components101.com/resistors/potentiometer)


