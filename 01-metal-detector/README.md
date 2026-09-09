# Metal Detector

A BFO-principle (beat-frequency oscillator) metal detector built as a coursework project at the Georgian Agrarian University, Faculty of Engineering and Technology.

The circuit detects nearby ferrous metal objects and signals their presence through a buzzer, using two coils to generate and sense changes in a magnetic field.

## How it works

The detector runs on electromagnetic induction. Two coils — a transmitter and a receiver — are kept separate to improve sensitivity:

1. A 9V source powers an LC oscillator circuit built around the transmitter coil (L1) and a capacitor, producing an oscillating magnetic field at the coil's resonant frequency.
2. When a metal object comes close, it induces eddy currents, which distort the magnetic field around the coil.
3. The receiver coil (L2) picks up this disturbance as a small induced current.
4. Since this signal is very weak, a transistor stage amplifies the output of L2.
5. The amplified signal drives a speaker/buzzer, which sounds when metal is detected.

**Signal path:**

```
Power Supply → Oscillator/Detector Circuit (Coils L1, L2) → Amplifier → Speaker
```


## Key calculations

**Resonant frequency** of the LC circuit (L = 1 mH, C = 2.2 nF):

```
f = 1 / (2π√(LC)) = 1 / (2π√(0.001 × 2.2×10⁻⁹)) ≈ 107.3 kHz
```

Higher frequencies increase sensitivity to small objects, at the cost of reduced detection depth — a known trade-off in this design.

**Number of coil turns** (using 25 AWG copper wire, coil diameter 6.69 cm, thickness 2.4 cm):

```
N = √(L·l / (μ·A)) = √(0.001 × 0.03 / (0.00000125662 × 0.00351513587)) ≈ 83 turns
```

## Circuit design notes

- **Potentiometer (RV2, 100Ω):** adjusts sensitivity — tuned until the buzzer's background noise just stops, giving the best achievable sensitivity.
- **100nF capacitor:** filters noise on the input signal and smooths out small voltage fluctuations from the battery, stabilizing circuit operation.
- **Q3 (2N2222, NPN):** base resistor (220kΩ) limits base current, switching the transistor on once the LC circuit reaches resonance and produces a varying current.
- **R4 (2.2kΩ):** sets the operating point of the second transistor stage (Q1).
- **Q5:** final amplification stage, boosting the oscillating signal enough to drive the speaker clearly.

## Components used

| Component | Value | Qty |
|---|---|---|
| Resistor | 220kΩ | 1 |
| Resistor | 2.2kΩ | 1 |
| Potentiometer | 5kΩ | 1 |
| Ceramic capacitor | 0.1 µF | 2 |
| Ceramic capacitor | 2.2 nF | 2 |
| Electrolytic capacitor | 10 µF | 1 |
| Transistor | 2N2222 | 1 |
| Transistor | SS8550 | 2 |
| Coil | 1 mH | 2 |
| Speaker / buzzer | — | 1 |
| Battery | 9V | 1 |

Coils were hand-wound using 25 AWG (0.45 mm) copper wire, ~6.69 cm diameter, 2.4 cm thick.

## Limitations & future work

- Cannot detect objects at large depths — a trade-off of the high-frequency design chosen for small-object sensitivity.
- Only distinguishes ferrous metals in this configuration.
- Potential improvements: dual-coil sensitivity tuning, adjustable frequency ranges, non-ferrous metal discrimination.

## References

- [Number of turns of solenoid — formula calculator](http://www.wikicalculator.com/formula_calculator/Number-of-turns-of-solenoid-%28given-magnetic-permeability-inductance-area-length-of-coil%29-267.htm)
- [Electronics Tutorials — Oscillators](https://www.electronics-tutorials.ws/oscillator/oscillators.html)
- [ON Semiconductor — P2N2222A datasheet](https://www.onsemi.com/pdf/datasheet/p2n2222a-d.pdf)
- [ON Semiconductor — SS8550 datasheet](https://www.onsemi.com/pdf/datasheet/ss8550-d.pdf)

