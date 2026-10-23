# Codex incremental task — incorporate program-audio findings

Read docs/AUDIO_MEASUREMENTS_v03.md and the existing DSP design first. Treat the data as comparative program-audio evidence, NOT as measured diode harmonics or device impulse response.

1. Write a reproducible analysis executable that accepts the included references and aligns Dynamic Color's OFF-ON-OFF repeats. Output whole-window RMS/crest and short-window crest statistics, sample-rate agnostic.
2. Extend the core's GR-dependent Dynamic Color model to allow a separately calibrated nonlinear waveform-expansion strength; default to a safe conservative level; ensure setting 0 and zero GR are exactly neutral.
3. Validate behavior at 44.1, 48, 96, 192 kHz; test spectral aliasing and inter-sample overs.
4. Do not infer or hard-code +6 dB peaks as a universal behavior, because program dependence is strong.
5. Preserve unmodified mono/stereo original signals in true bypass and calibrate reporting before fitting.
6. Add null/residual, crest distribution and stable release regression tests. Document exact findings vs. hypotheses.
7. For VCA Color: retain configurable diode characteristics pending laboratory-grade test tones, because program sections are not sample-coherent.
