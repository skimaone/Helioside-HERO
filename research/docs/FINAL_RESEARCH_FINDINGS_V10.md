# TITAN FINAL RESEARCH-STAGE FINDINGS — v1.0

## Evidence and limits
- Manufacturer manual (included): stepped 0–99 Threshold/Attack/Release/Shape; 16-step VCA blend, Dynamic Color and Mix; PWM clean analog gain control, colored diode gain control; third-harmonic-dominant opposite-phase GR-linked Dynamic Color; GR-dependent Fat/Air; detector analog outputs linked for stereo.
- Manufacturer manual specifies ~50 µs–400 ms attack, 50 ms–>5 s release, Shape 10:1 to 1.1:1, clean and vintage operating-point THD figures, and +25 dBu headroom.
- Six musical MP3s give relative dynamic/spectral observations **only**, not circuit transfer identification, a phase reference, or unique attack/release constants. See earlier docs and measurements.
- Actual physical diode topology, coefficient tables, exact detector type and sidechain algorithm, filter coefficients and 870 kHz control implementation remain proprietary/unknown.

## Model selection
- Regularized feedback is the **stable numerical default**, not a verified Dave Hill feedback law. Preserve legacy and quasi-static modes as researcher alternatives.
- A measured musical section difference must not become a universal output gain adjustment.
- Default 4x/8x linear-phase multistage FIR oversampling in host wrapper; JUCE integration and host latency still require real compilation/testing. Do not call this production-ready VST3.
- Do not claim specific harmonic-phase accuracy: positive vs negative cubic curvature can give opposite third-harmonic sine polarity for a memoryless tone but hardware reactive stages are unidentified.

## Fixes in this release
- Input-envelope sidechain frequency weighting has separate filter memories from post-compressor feedback sidechain. Fixes an unintended shared-state distortion/feedback interaction.
- JUCE research adapter explicitly permits 2x, 4x, 8x, 16x (avoids dummy-stage ambiguity at factor 1). Host latency compensation still must be validated.
- Adds controlled independent-state regression test.

## Completion criteria still required before an ACCURATE emulation claim
1. Real matched line input and output from hardware with controlled sine, stepped tone, sweeps, multitone and bursts at diverse gain reduction levels.
2. Coherent phase-referenced H2/H3/H5 measurements, not lossy musical MP3 spectrum deltas.
3. Hardware envelope fitted across Shape/threshold and 0-99 attack/release.
4. Verify CPU-safe JUCE callback, integer-latency dry alignment, PDC, no heap allocation on audio thread, cross-block invariance and alias rejection.
5. Blind ABX comparison with hardware and matched level; no quantitative fidelity percentage before tests.

## Critical note on stabilization
Clamping gain reduction to an independent input-reference equilibrium prevents feedback explosion in the numerical model, but can reshape legitimate onset transients. Treat the guard as adjustable and run AB tests.
