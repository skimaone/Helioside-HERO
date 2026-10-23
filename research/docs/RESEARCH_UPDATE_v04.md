# Titan v0.4: evidence, inference, and resulting DSP changes

## Evidence grading

### Established by Dave Hill manual v1.1
- Discrete Class-A analog signal path, digitally managed sidechain; feedback compressor.
- VCA Color is a SIXTEEN-step crossfade between clean PWM attenuation and vintage diode attenuation (page 5).
- Dynamic Color only affects audio when gain reduction occurs, chiefly generates third harmonic with phase opposite the vintage gain-control harmonics, and its audible response depends on attack/release (page 5).
- Fat/Air depend on gain reduction. Published graph measured in test mode at 12 dB reduction, not full program-dependent response (page 5).
- Direct mix is ~94% dry / 6% compressor path, not a bypass (page 4).
- Stereo link shares detector analog control signals as well as control data (page 6).
- Shape is 1.1:1 to 10:1; attack 50 us to 400 ms; release 50 ms to >5 seconds (page 8).

### Independent cross-check
- Hugh Robjohns, Sound On Sound January 2013, describes third harmonic, PWM versus vintage, and sidechain-dependent Fat/Air.
  https://www.soundonsound.com/reviews/dave-hill-designs-titan
- Dave Hill interview confirms PWM is a gain-control topology, not a detector, and 870 kHz sidechain DSP.
  https://varietyofsound.wordpress.com/2013/06/18/compressor-aficionados-5-dave-hill/
- Dave Hill PWM note discusses artifact suppression and clean PWM choice.
  https://dl.icdst.org/pdfs/files3/bbcc11bf72f1f327347065a4953515fa.pdf
- Pro Audio Review April 2012 describes the vintage setting softening and rounding peaks, compared with PWM; Dynamic Color restores attack.
  https://www.worldradiohistory.com/Archive-All-Audio/Pro-Audio-Review/ProAudio-Review-2012-04.pdf

### Earlier musical-recording analysis (v0.3)
- Dynamic Color ON passage approximately 0.95 dB higher in RMS than OFF passages; reported sample peaks up ~5.8-6.4 dB, and median 100 ms-window crest increases ~1.3 dB. These are **observations of a musical recording**, not calibrated transient responses, THD, absolute gains, or guarantees at any setting.
- The PWM-Vintage-PWM musical example is not aligned as a coherent identical waveform, so no defensible absolute harmonic ratios/phase can be extracted.
- MP3 compression and unknown gain/setup add uncertainty. References are included in reference/audio/.

## NEW MODEL v0.4
1. Prior model's diode path `tanh(u*drive)/tanh(drive)` at large drive had considerable uncontrolled small-signal amplification. Replaced with unity-small-signal derivative `tanh(drive*u)/drive` blended proportionally to measured/control gain reduction. This is a *phenomenological diode proxy*, not a physical diode bridge.
2. Vintage and PWM are processed as separate audio-path candidates, crossfaded via VCA Color. Soft saturation's third-harmonic polarity opposes the positive-cubic expansion stage by construction.
3. Dynamic Color is now a continuous, bounded positive-cubic functional `z^3/(1+z^2)` with a gain-reduction-dependent coefficient. Unlike an unnormalized Chebyshev T3, this does not arbitrarily alter the fundamental for very low amplitudes or explosively grow without bound. The *specific nonlinear functional is inferred*, not measured.
4. Added an **optional** developer-only GR-rise coupling parameter; defaults OFF because no controlled evidence establishes an explicit derivative dependency. Detector smoothing itself still gives attack/release dependence.
5. Dynamic shelves use GR-activated low/high bands. The precise placement in the analog loop is not known.
6. Feedback detector now samples the colored, pre-makeup / pre-parallel wet signal (a model hypothesis); earlier version used only clean attenuation for detector input. This increases feedback interaction and requires stability testing.
7. Stereo detector linking uses shared peak envelope approximation; real device ties analog detector controls and may behave differently.
8. User-selectable alignment `referenceDbfs` is exposed for lab calibration. Threshold 0..99 mapping to dBu remains UNKNOWN, as do most nonlinear coefficients.

## Critical missing work before release
- Anti-alias filtering / oversampling of nonlinear stages. This reference implementation DOES NOT yet oversample. Recommended implement factor 4/8 with polyphase halfband IIR/FIR, explicit latency management, then remeasure spectra at 44.1/48 kHz.
- Automated parameter smoothing/ramping at control changes. Stepped switches on real hardware may sometimes produce clicks, but a plugin needs practical mitigation.
- Independent isolated tone recordings at several known GR setpoints to fit third-harmonic complex coefficients and IMD.
- Validate transfer curves from tone stepped-level sweep, map hardware Shape 0..99.
- Validate frequency curves at different GR and real drums. Spectral shelving values are rough fits to the published graph, not exact.
- Stereo detector correlation and linked-channel behavior tests under hard-panned excitation.
- Plugin wrapper VST3/JUCE, GUI, serialization, meter calibration, oversampling, robustness tests, CPU benchmarks.

## Warning about physical identifiability
Different diode circuits, analog gain architectures, and DSP sidechain loops can generate similar THD and audio on full mixes. No semiconductor part numbers, actual feedback-loop poles, or true odd-harmonic phase response have been identified. Do not present this prototype as circuit-authentic.
