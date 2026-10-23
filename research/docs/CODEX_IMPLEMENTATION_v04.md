# Codex action list - v0.4 executable research engine

1. Open `src/TitanCore.h` (now version 0.4), `tests/test_core.cpp`, `tests/test_research.cpp`, then `docs/RESEARCH_UPDATE_v04.md`.
2. Preserve v0.4 shared-detector topology and 16-step PWM-diode wet crossfade; make the detector tap point user-selectable ONLY for offline A/B fit experiments.
3. Build VST3 with JUCE (JUCE not vendored). No runtime allocations and no locks in callback. Parameter changes need click-safe interpolation and proper model state resets.
4. First DSP task: add 4x/8x polyphase oversampling for the *nonlinear* audio path; ensure path latency alignment and no dry/wet comb filtering. Measure alias rejection using 8-15 kHz sine sweeps at 44.1/48 kHz. Do not label 'oversampled' merely because the detector updates every sample.
5. Calibrate gain cell on sine stimuli with THD and harmonic phases. Crucially test harmonic sign across VCA Color and Dynamic Color; do not assume perfect third-harmonic cancellation.
6. Implement parameter smoothing, input/output dBu calibrations and auto gain (optional, not hardware).
7. Retain all developer-only tuners: vintageDrive, vintageStrength, colorStrength, colorRiseWeight and referenceDbfs, until hardware measurements constrain them.
8. Re-run `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure` after every DSP change. Add hard-panned stereo/link, THD, alias and impulse response tests.
9. Make truthful marketing claims. This is a Titan-inspired research prototype until fit against known hardware.

## Suggested next lab measurement
Record 100, 500, 1000, 5000 Hz tones in PWM vs Vintage and Dynamic Color 0/15 at 0, 3, 6, 12 dB gain reduction. Preserve exact phase reference by recording input and hardware output simultaneously; de-embed interface latency, then derive complex H2..H7, compression envelope and gain maps. Also capture kick bursts with peak/RMS and makeup gain matched.
