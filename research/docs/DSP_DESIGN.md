# DSP design document — v0.1

## Algorithm now
For each input sample x and channel c, retain a causal prior-sample output `lastOut`. Follow that with a rectified one-pole detector (attack/release); derive a soft-knee curve. Smooth target gain reduction in the dB domain using attack/release coefficients, then apply g=10^(-GR/20). Test signal path is:

`input -> feedback gain cell -> PWM/vintage crossfade -> GR-gated cubic Dynamic Color -> GR-gated Fat/Air approximation -> makeup -> parallel blend -> output`

The feedback tap is from gain-cell output only. The exact analog topology is unknown. Stereo link applies the maximum of per-channel detector estimates. To avoid unstable algebraic instantaneous loops, feedback uses prior-output state; that incurs an intentional single-sample control signal delay.

## Controls and provisional mappings
- threshold 0–99 => -40..0 dBFS (`threshold` mapping has not been verified)
- attack 0–99 => 50us..400ms exponential
- release 0–99 => 50ms..6s exponential
- shape 0–99 => ratio 10..1.1 log-mapped, knee width 0.5..28dB
- VCA Color 0–15 linear amplitude mix between gain-only and tanh soft clipper
- Dynamic Color 0–15 multiplies 0..0.22 GR-modulated cubic basis; attenuation/normalization currently **not** calibrated
- Parallel Mix 0–15 => 6..100% wet in equal numeric steps
- makeupStep 0–23 => 0..11.5dB
- Fat/Air two 1-pole filters lowpass 80Hz/highpass 4200Hz providing maximum +3.1/+3.0dB when GR>=12dB (approximation)

## Why cubic
For normalized sinusoid sin(wt), `4*sin^3(wt)-3*sin(wt)=-sin(3wt)`; adding scaled expression produces a third-harmonic component with a phase opposite to a positive cubic term. The actual dry/wet and diode harmonic **signs require measurements**. Because a normalized sinusoid is assumed, this basis generates amplitude-dependent fundamental shifts and different behavior on arbitrary complex audio; a fully normalized and antialiased waveshaper is a future research objective.

## Prototype limitations and corrective tasks
1. **Gain-dependent clipping**: currently vintage uses tanh(u*drive)/tanh(drive); at amplitude above unity it saturates; at low amplitude it changes gain depending on drive. Calibrate output gain consistently versus GR and VCA step.
2. **No oversampling**: harmonic generator aliases at high frequency. Implement high-quality anti-aliased interpolation/downsampling filters around *all* nonlinear stages, not just final output. Propagate same detector semantics if processing oversampled stages. Provide fixed, reported plugin latency and alignment.
3. **Control smoothing**: discrete control changes may click. Add per-sample smoothers for threshold, dry/wet, colors, makeup and bypass soft transition; gain law and stage coefficient updates should not allocate.
4. **Detector**: current detector effectively uses two cascaded attack/release stages. Compare this hypothesis with amplitude/RMS detectors, peak-hold and multi-slope release, and fit log-domain dynamics to measured envelope trajectories.
5. **Calibration**: `referenceDbfs` is an unused future control. Propose `0VU=+4dBu` matched to configurable `-18dBFS` as a UI convention; hardware +25dBu headroom cannot be converted to digital fullscale without explicit interface calibration. Do not automatically impose saturation at +25dBu.
6. **Stereo**: test max, mean, power-link and source-dependent link; hardware master/slave UI mechanism does not resolve audio gain linkage.
7. **Tone**: first-order shelving *is not a fitted reproduction* of the manufacturer's measured frequency-response plot, and is not a topology confirmation. Fit parametric curves at multiple GR amounts.
8. **Validation**: double precision processing variant, denormal prevention, NaN and parameter sanitization, silence handling, 44.1/48/88.2/96/192k, mono/stereo and in-place rendering, parameter automation stress.
9. **Intermodulation**: avoid harmonic fitting using only single sine THD. Track multitone and intermodulation products including their phases.

## Production architecture
- `Detector` includes parameterized feedback tap and stereo-link strategy.
- `TransferCurve` renders desired GR in dB from input level and threshold/Shape.
- `CleanGainCell` / `VintageGainCell` process separate gain paths and share control smoothing.
- `DynamicColor` accepts GR and separate harmonic-phase state; optional alternative transfer functions exposed in developer mode.
- `ToneCircuit` supports active shelves and sidechain-interaction candidate algorithms.
- `Oversampler` wraps nonlinear stages, with explicit delay and double precision coefficients.
- `Meter` shows physical GR separately from color-related visual LED behavior.
- `Processor` no alloc/locks during audio callback; atomic or safe snapshot of UI params.

## Acceptance targets for the *next* milestone, not achieved by this demo
- Pass VST3 plugval + DAW smoke suite; no NaNs, infs or denormals.
- Strict null-color clean behavior at matched gain; no unintended harmonic production for bypass.
- Delta between 0 GR / Color 0 and dry below -100dBFS when no compression and unity makeup, except explicitly modeled baseline hardware harmonic/nonlinear residual.
- Reproduce measured reference curves within <=0.5dB magnitude and <=10% timing error at test conditions, **only after** acquiring reference measurements.
- Dynamic Color with zero GR must be silent in difference compared to Color=0, given otherwise identical parameter states.
