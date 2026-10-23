# Hardware research and inference ledger

Evidence labels: **FACT** manufacturer explicitly reports; **CORROBORATED** multiple descriptions/reviews; **HYPOTHESIS** DSP design choice, not observed; **UNKNOWN** requires measurement.

## FACT / corroborated
- Main program audio path discrete Class A, digitally controlled analog; feedback compressor; control DSP runs at 870kHz (designer interview). This **does not** mean digitized 870kHz audio is used.
- Threshold, Attack, Release, Shape each display 0–99. Attack and release time depend on threshold, knee and GR; range indications are approximate, not a guaranteed linear parameter mapping. Shaping knee alters effective ratio; extremes approximately 10:1 and 1.1:1.
- Attack 50 microseconds to 400ms; release 50ms to >5seconds; makeup 0–11.5dB in 0.5dB increments.
- VCA Color: 16 steps between clean PWM analog gain control and high-color diode compressor element. Manual gives ~0.018% and ~13% THD at +24dBu (respective extremes). THD is **level- and operating-point-specific**, not continuous specification.
- Dynamic Color: 16 steps; generates harmonic content opposite phase to vintage gain element; mostly 3rd harmonic; requires gain reduction; envelope/timing dependent. It also affects the visual GR meter presentation; meter is not a reliable absolute indicator of GR when high Dynamic Color is active.
- Fat/Air: modifies frequency response depending on GR and compressor timing; supplied reference plot was made at *12dB GR test mode*. Plot appears ~+3dB near 20Hz Fat and ~+3dB near highest displayed treble Air; numbers are visual estimates.
- Parallel Mix: 16 steps; Direct is about 94% dry + 6% wet; true dry through bypass; wet extreme is 100% compressed.
- Max balanced input/output +25dBu; nominal +4dBu=0VU indication; audio response 3Hz to 175kHz; published noise −91dBu over 20kHz bandwidth.
- Stereo link: hardware master/slave disables slave controls; actual detector combining law **not documented**.

## Inference — prioritized
**H1 [medium]**: diode gain path can be approximated by reduction-dependent odd nonlinearity; tanh is a placeholder, **not** circuit identification. A classic diode forward equation cannot determine stage distortion without topology, bias and feedback network.
**H2 [medium]**: Dynamic Color's 3rd harmonic opposite phase suggests a cubic contribution that counteracts vintage soft limiting's third harmonic. However *opposite phase* alone does NOT guarantee literal waveshaping inverse or transient restoration. Current model is a testable hypothesis.
**H3 [medium]**: Fat/Air may involve sidechain-conditioned coloration or program-dependent spectral feedback, rather than a downstream static EQ. An adaptive shelving approximation matches a fraction of its observable effect; detector/EQ coupling remains unknown.
**H4 [low]**: 16-position VCA Color might be linear amplitude crossfade, equal-power crossfade, an analog current blend, or more complex interaction. Model linear blend initially; capture and fit levels at each step.
**H5 [low]**: 870kHz DSP likely helps accurately track peaks / resolve short time constants, but it does NOT imply a 870kHz audio switching rate or define detector architecture. Neither RMS nor peak detector has been conclusively established.
**H6 [low]**: Exact dynamic attack/release asymmetry, detector tap location, stereo linking and lookahead absent from documentation.

## Correctness guardrails
- Treat all nonlinear transfer functions, drive coefficients, GR dependencies, shelf turnovers and input calibration in prototype as placeholders.
- The first build is about architecture/API/test scaffolding and musical experimentation, **not measured authenticity**.
- Need original hardware or paired dry/wet recordings plus test sine bursts before any accuracy claims.
- Prefer fitting time-dependent behavior (envelope trajectories) separately from coloration, then validating the coupled model.
