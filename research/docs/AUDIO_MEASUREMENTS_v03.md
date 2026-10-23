# Titan public MP3 measurement addendum (2026-10-23)

## Provenance
Two audio assets supplied by user, both from published Amazona comparisons. Both stereo MP3 at 44,100 Hz. Dynamic Color recording = 27.000 s; VCA Color recording = 20.045 s. Analysis here is **not** isolated-tone THD/phase measurement. Treat harmonic polarities and diode coefficients as undetermined.

## Dynamic Color OFF–ON–OFF
The 27 s demonstration contains three approximately 9 s repetitions of the same instrumental passage. Envelope lag correlation peaks at ~8.99 and 17.98 seconds. Time-domain cross-correlation of aligned mono signals: first OFF against ON ~0.959 on coarse search window (more conservative 8.7 s window ~0.946); first OFF against final OFF ~0.964 coarse (~0.975 in 8.7 s window). Differences across windows reflect different alignment/cropping and content.

Approximate unaligned full-9-second section statistics (mono average of stereo):

| section | RMS dBFS | peak dBFS | crest dB |
|---|---:|---:|---:|
| 0–9 s OFF | -22.41 | -7.28 | 15.13 |
| 9–18 s ON | -21.48 | -0.84 | 20.64 |
| 18–27 s OFF | -22.44 | -6.69 | 15.75 |

Interpretation: ON raises RMS by approximately +0.95 dB while raising largest sample peaks approximately +5.8 to +6.4 dB, as compared with OFF sections. Full-section peak comparison can be affected by transitions and recording differences. Robust 100-ms-window crest factor (median) in matched 8.7 s segments: OFF1 ~9.69 dB, ON ~10.97 dB, OFF2 ~9.68 dB. Approximate 90th-percentile crest: OFF1 ~13.32, ON ~15.08, OFF2 ~13.33 dB. These support enhanced transient peakiness, not merely an overall static gain boost. The phase-coherent third harmonic is NOT determined by these statistics.

Frequency-band integrated levels for full sections were broadly elevated in ON across the spectrum, but strongly correlated program material plus clipping/MP3 make standalone harmonic fitting inappropriate. If the reference were controlled sinusoidal input, a complex FFT coefficient measurement would be preferable.

## VCA Color PWM–Vintage–PWM
Total duration 20.045 s. Three approximate equal thirds are NOT sample-aligned equivalents (waveform correlation under 0.06 for attempted starts ~6.68 and ~13.36 s). Rhythmic envelope repeats at ~2.58 s and its multiples, but do not assume exact matched transients or source processing. Statistics of approximate sections:

| section | RMS dBFS | peak dBFS | crest dB |
|---|---:|---:|---:|
| first ~6.68 s | -25.20 | -3.27 | 21.93 |
| middle ~6.68 s | -26.58 | -5.47 | 21.11 |
| final ~6.68 s | -26.45 | -4.36 | 22.09 |

Given poor time-domain alignment, these numbers cannot reliably quantify vintage saturation, attack differences, or the diode third harmonic. Sound/music arrangement variability and differing section length should be investigated before parameter fitting. DO NOT FIT diode coefficients from these section averages.

## Falsifiable DSP hypotheses and next measurement
1. **Dynamic Color as GR-coupled expansion:** use an amplitude-normalized odd nonlinear term; modulation coefficient vanishes smoothly when gain reduction vanishes. Calibrate to increase windowed crest under compression in program-like test examples, but do not tune to fixed +6 dB for all inputs.
2. **Opposing odd-order components:** manufacturer states opposite-phase harmonic content vs. vintage element and mostly third harmonic. Use coefficient signs allowing opposite complex H3 vectors, but unknown phase vs frequency and operating GR means a constant pi phase assumption is only a test hypothesis.
3. **Feedback coupling:** compare measured release/attack with Dynamic Color settings before deciding whether its audio signal is actually inside detector feedback or meter responds for a different reason.
4. **For true identification:** obtain sustained sine sweeps and two-tone stimuli with fixed input headroom, output at different GR, PWM/vintage endpoints and Dynamic Color 0..15; measure complex harmonic ratios with latency-aligned reference and remove measurement-chain distortions. Stereo link must be measured with intentionally asymmetric left/right stimuli.
5. **Safety/quality:** oversample odd nonlinear audio terms, avoid excessive signal-dependent coefficient jumps, fit distinct amplitudes for peaks and RMS, and test aliased residual. MP3 is inadequate for sub-percent THD calibration.

## Software status
Existing C++17 source remains the initial prototype. This addendum supplies evidence and instructions; it does NOT imply the code's harmonic model has been recalibrated from controlled hardware sweeps.
