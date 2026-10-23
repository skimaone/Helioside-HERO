# TITAN R&D v0.8 — oversampling, detectors, controlled response

## Evidence versus inference
Hardware manual documents 50 µs–400 ms attack, 50 ms to >5 s release, feedback compression, digital control of a discrete Class A analogue path, and Shape 0–99. These numbers are NOT necessarily the 63.2% step response measured at the output. Published audio demonstrations are music, not controlled step or sine measurements; do not infer unique detector coefficients from them.

## Production integration of oversampling
New `src/JuceTitanOversampledAdapter.h` uses JUCE DSP multistage high-quality linear-phase FIR oversampling, configurable at 1x,2x,4x,8x, with optional integer latency. It oversamples the ENTIRE loop deliberately: sample delays, detector integration, fast nonlinear color and audio feedback remain on the same simulation clock. It is computationally expensive but a straightforward reference baseline, and its dry + wet are processed at the same sampling rate so parallel mix is internally aligned. Plugin must call `setLatencySamples(adapter.latencySamples())` after prepare and on any quality-setting reconfiguration, subject to host support. Do NOT switch oversampling configurations from the audio thread, allocate on audio thread, or assume a plugin can change latency glitch-free during playback. In bypass, oversampled zero-latency identity still passes through the oversampling filters: use a delay-compensated bypass path and host PDC if switching bypass modes. `Engine`'s sample-identical bypass guarantee only applies to the native standalone core, not to the wrapper. Test actual latency by impulse cross-correlation and alignment in plugin.

**Adapter is uncompiled in this environment (JUCE not installed).** Existing standalone oversampler is experimental; prior single-tone alias suppression ~16.4 dB does NOT meet production quality. End-to-end measured alias rejection, passband ripple, output and crossfade latency, stereo delay equality, denormal stress, sample-rate changes and performance profiling are required.

Long-term optimization: 4× core by default, 8× on high quality, IIR option for live low-latency, FIR for mixing/mastering. Oversample nonlinear gain cell + dynamic color *together* with control-path coherence. A narrow nonlinear island may reduce CPU but requires a carefully aligned feedback/control tap and phase-correct wet/dry.

## Detector hypotheses
v0.8 adds PEAK (one-pole rectified magnitude), RMS (one-pole mean-square then sqrt) and HYBRID (linear blend of envelope magnitudes, default 65% peak) candidates. Detector rectification has fast tracking independent of the separately smoothed gain-reduction command. PEAK is most sensitive to isolated transients, RMS to sustained energy, hybrid intermediate. These are **candidate models**, not Dave Hill's disclosed algorithm. A linear peak/RMS blend is also NOT equivalent to averaging instantaneous powers. For genuinely program-dependent detection, compare two-stage RMS, sliding RMS, log-domain and peak-hold variants in follow-up.

## Controlled tests and signal design
`tests/controlled_response_v08.cpp` generates deterministic 500 Hz sine gated 0.20–0.90 s, silence to 1.30 s, 0.20 s burst and silence. It runs 48 combinations: 3 detector modes × four Attack settings × four Release settings. Its 10 ms CSV logs output RMS, input RMS and internal GR readout, with no external audio source confounds. Run `titan_controlled_v08 /path/trace.csv`.

Analysis recommendations: (1) use internal GR to establish attack time to 63.2% of *the achieved steady GR change*, not 63.2% of sample amplitude; (2) likewise release time to 36.8% of initial GR; (3) compare 10–90% attack time, overshoot/GR oscillation, program dependence, frequency dependence 40 Hz/500 Hz/8 kHz, 1ms–100ms bursts; (4) measure at 44.1, 48, 96 kHz host rates and 1×/4×/8× to check numerical consistency; (5) compare one large process block against random block segmentation.

Critical: for 400 ms attack the 0.7 second plateau may not settle sufficiently to define *steady* GR; use ≥5× the slowest time constant for sustained tones, and multiple seconds after release to quantify long tails. The included 0.7-second gate is a practical quick suite, not a definitive slow-ballistics calibration. Abruptly gated 500 Hz tones create broadband discontinuity; use ramps if frequency-domain alias analysis is required. For alias tests, isolate nonlinear processing at constant attenuation, use 997 Hz and 17–21 kHz coherent tone sweeps, multitone IMD, compare against a 16× or greater filtered reference, and report the largest aliased spur and integrated alias energy.

## Acceptance criteria (engineering goals, not hardware claims)
- 4×/8× nonlinear alias-suppression improvement versus 1× >= 50 dB in representative high-drive 17–20 kHz tests where physically attainable. Confirm passband and reference-band conditions; test both output tones and multitone.
- Delay error between dry/wet < 0.05 host sample after compensation; identical mono/stereo group delay.
- Finite outputs, no unexpected pumping or oscillation on silence/impulses; steady tonal drift and GR clamp bounded.
- Detector-choice difference verified on low-duty-cycle bursts; parameter sweep monotonicity where appropriate.
- No allocations, locks or file I/O on audio processing thread; deterministic block-size invariance.

## Caveat about frequency-dependent feedback
Fat/Air detector weighting and a secondary gain-reduction-dependent spectral correction remain hypotheses. Testing whether detector weighting alone can fit both musical demo measurements AND manufacturer's reference response at 12 dB GR is still necessary. Do not add a fixed shelf as a substitute for verification.

## References
- Dave Hill Designs TITAN Manual v1.1 (included PDF)
- https://docs.juce.com/develop/classjuce_1_1dsp_1_1Oversampling.html
- https://www.soundonsound.com/reviews/dave-hill-designs-titan
