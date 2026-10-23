# TITAN-inspired compressor: Codex handoff (2026-10-23)

This research package contains **a working, dependency-free C++17 DSP prototype**, research notes, tests and a prioritized Codex implementation plan. It is **not** a circuit-authentic emulation, a finished VST, or official Dave Hill Designs software. Hardware trademarks belong to their owners; use your own product name and original UI artwork.

## Quick start

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

The header-only `src/TitanCore.h` provides `titan::Engine::prepare(sampleRate, channels)`, `setParameters`, `process(const float* const* input, float* const* output, size_t frames)`, and `gainReductionDb()`.

## What exists now
- Causal feedback detector, configurable linked stereo or dual mono
- 0–99 numbered threshold, attack, release and Shape controls (mappings are provisional)
- Clean PWM-style gain, diode-*inspired* variable soft saturation and stepped blend
- Gain-reduction-gated predominantly cubic Dynamic Color prototype
- GR-dependent Fat/Normal/Air shelving approximation
- 16-position dry/wet with ~6% wet at Direct, 0–11.5dB stepped makeup gain
- One-file C++ source and CTest smoke tests

## What does NOT exist
- No VST3 wrapper, GUI, preset system, physical PWM circuit, physical diode-equation solver, oversampling, calibrated parameters, full anti-aliasing or accurate sample-peak metering.
- Stereo link detector is a deliberately chosen **max amplitude** hypothesis, not a measured fact.
- Gain reduction feedback is simplified: the detector taps compressed gain-only output, not full colored analog output. Alternate routings must be evaluated.
- Dynamic Color may alias at higher frequencies; do not ship this algorithm without 4–8x oversampling or an antialiased replacement.
- Parameter smoothing and latency management are not yet production ready.

## Start here
1. `docs/CODEX_TASK.md` — instructions for Codex
2. `docs/RESEARCH.md` — substantiated hardware facts vs inferences
3. `docs/DSP_DESIGN.md` — architecture, equations, caveats and upgrade path
4. `docs/MEASUREMENT_PLAN.md` — measurement and fitting protocol
5. `docs/SOURCES.md` — primary sources and reviews

## Legal / product identity
Do not market the finished plugin as an official Titan, claim equivalence, imply endorsement, or reuse protected trade dress. Choose a distinct name and visual identity. Document the accuracy and provenance of all modeled characteristics.


### Update v0.3 — real public-demo analysis
See `docs/AUDIO_MEASUREMENTS_v03.md` and `docs/CODEX_AUDIO_TASK_v03.md`. Original supplied MP3 comparisons are included under `reference/audio/`. Findings establish increased transient peakiness in the Dynamic Color ON passage but **do not** establish diode THD or third-harmonic phase.
