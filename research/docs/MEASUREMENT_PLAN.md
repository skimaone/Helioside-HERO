# Hardware capture and model identification protocol

All stimuli rendered float WAV at 96kHz/24bit or 32bit float; send into analog unit with a **documented calibrated interface** and capture dry loopback plus processed output simultaneously. Use the same cabling/converter gain for all experiments, leave headroom, and record hardware serial/firmware/settings, sample rate, analog level in dBu, input-output clock alignment and temperature if feasible. Avoid random normalized online clips as the sole measurement source.

**Stage 1 baseline:** Bypass and engaged PWM, color=0, shape neutral, makeup=0, mix=Comp. 50Hz/1k/10k sine sweeps from -30 to +20dBu, stepped -60 to +20dBu transfer curve, 1ms pulses, log sine sweep and silence/noise. Distinguish baseline amplifier from gain element.

**Stage 2 dynamic response:** input 1kHz sine step quiet->loud->quiet for 50ms/250ms/2s, repeat 6 thresholds x 5 Shape x 5 Attack x 5 Release. Fit envelope magnitude and GR trajectory with confidence intervals; identify overshoot and recovery, attack v release coupling; repeat with 60Hz and 10kHz carrier.

**Stage 3 VCA Color:** synchronized 1kHz and 5kHz sines and two-tones (e.g. 700/900Hz), level sweep at 0, 3, 6, 12dB target GR at all 16 steps; compute harmonic amplitude+phase and IMD. Do not assume crossfade law.

**Stage 4 Dynamic Color:** hold ~0,3,6,12dB GR; all 16 Dynamic Color positions, VCA 0/8/15; repeat 1kHz sines and snare-like bursts; examine signed complex transfer function and GR-driven spectral contributions, anti/phase relationship and metering effect.

**Stage 5 Fat/Air:** measure steady swept frequency magnitudes using consistent 0,3,6,12dB GR as far as technically meaningful; fit GR-dependent low/high shelf poles; check timing and dependence on spectral content. Manufacturer graph at 12dB test mode is a low-confidence visual constraint, not a full audio measurement.

**Stage 6 stereo:** inverse and correlated stereo transients with link on/off; examine simultaneous GR, master/slave control, possible feed control difference. Measure dry/wet phase and latency at all mix positions, particularly 0 (reported ~6% wet).

**Model evaluation:** fit training set only, then publish blind hold-out RMS envelope errors, spectral magnitude/phase residuals, difference-file loudness, harmonic and IMD errors, transient overshoot and ABX outcomes. Match amplitude and delay before null tests.

**Without hardware:** do not invent measurement scores. Build signal generator and analyzer pipeline; identify with provisional coefficients until verified dry/wet audio arrives.
