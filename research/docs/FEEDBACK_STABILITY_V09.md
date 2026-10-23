# Titan v0.9 feedback stabilization study — 2026-10-23

## Source distinction
- FACT (manual): Feedback-style compression; actual attack and release depend on threshold, knee shape and degree of reduction. DSP controls analog gain circuitry.
- OBSERVATION (our simulated v0.8/v0.9): Large overshoot occurs in synthetic step testing. It is NOT evidence of Titan hardware overshoot.
- HYPOTHESIS (our software): A parallel input-envelope constraint can regularize the simulated closed-loop gain command. No evidence confirms Titan uses this method.

## Implemented modes
- `Legacy`: earlier feedback-only law, retained for A/B testing.
- `Regularized` (default): input envelope drives quasi-static feedback equation g = reduction(input_level - g); blend this solution with the wet feedback target; bound applied reduction to predicted + guard_db.
- `Equilibrium`: only the quasi-static solution controls target with nominal attack/release and same bound. Not an exact feedback-compressor model.

The static equilibrium is solved by a 16-step bisection. This is intentionally a calibration/reference approach; optimize or replace with closed-form knee branches / lookup interpolation for low-CPU production processing.

## Test procedure
48 kHz 500 Hz sine, gate 0.20 to 0.95 s at 0.65 linear amplitude, 500 ms recovery; threshold 25, shape 0, PWM, dry mix disabled. Modes: peak/RMS/hybrid, attack and release 0/30/70/99; three control laws = 144 cases. Peak GR minus average GR during 0.80–0.94 s is the overshoot proxy. Recovery-to-1 dB is measured after the gate closes. NB: hard discontinuous gating can expose artificial behavior and transient edge effects.

## Limitations
1. This validates **algorithmic stability against synthetic inputs**, not hardware fidelity.
2. Dynamics can be altered by the input-constraint detector; the constraint should be made optional and calibrated before shipping.
3. v0.8 JUCE oversampling adapter still not compiled or latency-tested in this environment; v0.9 does not claim full VST3 integration.
4. Frequency, program-dependence, high sample-rate, parameter automation, and dense multitone tests remain to be evaluated.
5. The detector uses two passes through its input-weighting filters per sample in stabilized modes, a known issue for strict path accuracy; splitting independent filter states or caching band-limited detector samples is required in the next pass.

## Suggested Codex tasks
- Inspect overshoot and max/steady/recovery metrics in `docs/STABILITY_METRICS_V09.csv`.
- Replace bisection with a monotonic transfer-curve lookup table or analytic piecewise solution and benchmark CPU.
- Unify/filter sidechain reads to avoid double advancing `scLow`/`scHigh` states per sample.
- Cross-validate with clean mono sine, swept AM, impulse and stereo anti-phase burst stimuli, then compare to actual hardware before choosing a final loop architecture.
- Validate wet/dry oversampling latency, automation and DAW block-size invariance in JUCE.
