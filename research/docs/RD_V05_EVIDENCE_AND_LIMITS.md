# TITAN v0.5: evidence, inferences, and what remains unresolved

## Primary evidence
- TITAN manual v1.1 (2012), bundled at `reference/TITAN_MANUAL_VER_1.1.pdf`: pages 4–6, 8–9.
- Sound on Sound, Hugh Robjohns (Jan 2013): https://www.soundonsound.com/reviews/dave-hill-designs-titan
- Amazona, Jan 21 2019: https://www.amazona.de/test-dave-hill-designs-titan-kompressor/

## Why 870 kHz does not equal required DAW audio rate
The hardware carries program audio in analog discrete Class A electronics and uses a high-speed DSP *sidechain*, reported at 870 kHz by Sound on Sound. It is an internal control update/sampling rate, not a requirement to resample a 48 kHz digital audio stream to exactly 870 kHz. A software detector may require substeps for accuracy at shortest attack times; nonlinear audio requires sufficient oversampling and anti-alias filtering. Test fidelity convergence against reference signals, not numerical clock-rate imitation. 870 kHz isn't an exact integer multiple of 44.1 or 48 kHz. Hardware may process control dynamics the DAW cannot reproduce just by raising the sampling rate.

## Verified from manufacturer
- Feedback-style compressor; attack 50 us–400 ms, release 50 ms–over 5 s (effective values depend on feedback state).
- Shape 0–99 determines knee/ratio (nominal 10:1 hard to 1.1:1 soft).
- VCA color crossfades a PWM cell and a diode-based vintage gain cell in 16 positions.
- Dynamic Color generates predominantly third harmonics opposite in phase to vintage-generated harmonics; requires gain reduction; GR meter response also changes.
- Fat/Air gain-reduction-dependent frequency response at 12 dB GR chart, not fixed output EQ.
- The Direct end of Parallel Mix is ~94% dry/6% wet; true direct is hard bypass.
- Two-unit link connects analog detector outputs as well as DSP commands.

## External test interpretation
Amazona reports Shape 0 results in more GR than Shape 99, and Shape changes knee and ratio. The reviewer estimates practical attack 2–500 ms and release 200 ms–3.5 s under particular conditions. These are not internally calibrated setting tables.
Amazona describes Fat/Air as influencing sidechain control character; SOS agrees that it is *not simply an output EQ*. The physical sidechain weighting and any frequency-selective audio correction are not known. v0.5 adds provisional detector weighting and keeps residual adjustable dynamic audio correction, defaulting low.

## Audio example access status
Amazona has four corresponding public MP3 references but downloading the audio bytes failed in this environment. No fabricated audio measurements are reported. The four files should be obtained as original MP3 and analyzed before tuning DSP constants:
- https://www.amazona.de/wp-content/uploads/2023/01/Dave-Hill-Designs-Titan-SHAPE-99-0-99.mp3
- https://www.amazona.de/wp-content/uploads/2023/01/Dave-Hill-Designs-Titan-RELEASE-0-99-0.mp3
- https://www.amazona.de/wp-content/uploads/2023/01/Dave-Hill-Designs-Titan-ATTACK-0-99-0.mp3
- https://www.amazona.de/wp-content/uploads/2023/01/Dave-Hill-Designs-Titan-NORMAL-FAT-NORMAL-AIR.mp3
The earlier user-supplied Dynamic Color and VCA Color recordings are included unchanged.

## v0.5 actual changes
- Corrects feedback-law slope: if attenuation command GR = k*(post_gain_level-threshold), the *closed loop* hard-knee effective ratio is 1+k, not 1/(1-k).
- Shape maps ratio 10 -> 1.1 and transition knee width 1.5 -> 25 dB, both **hypotheses**. The exact control curves are not measured.
- Adds optional low/high sidechain weighting, preserving bass in Fat and treble in Air during modeled gain reduction.
- Preserves small residual gain-reduction-dependent frequency correction, adjustable independent of sidechain.
- Introduces gain-reduction-dependent diode proxy asymmetry to support even harmonics (small, uncalibrated).
- New independent FIR oversampling research module, with 4x/8x options. NOT yet integrated into the feedback engine: group delay, latency reporting, dry/wet alignment, and detector synchronization need to be solved. At 48 kHz in a 15 kHz cubic test, module reduced the 3 kHz aliased artifact ~16.4 dB (not production grade; improve reconstruction filter, multirate chain and attenuation).
- Four unit test groups; tests validate intended behavior, not hardware-equivalent accuracy.

## Critical caveat about shape slope
The mathematical formula assumes gain command is based on *instantaneous output dB*. Real v0.5 has smoothing in both detector and gain command and color in feedback, so static closed-loop ratio is approximate; derive it by steady-state numerical measurement before citing ratio matching.

## Further required work
- Download and segment the four additional source MP3s; compare overlapping passages after transient-aware alignment and loudness matching; do not claim precise phase/THD from musical clips.
- Use controlled sine sweeps across 0–15 VCA/Color, multiple GR settings, and coherent phase reference for H3 sign.
- Improve oversampler to 80–100 dB alias rejection over a defined band, integrate and latency compensate; verify response/aliasing at 44.1/48/96/192 kHz.
- Fit Shape and envelope constants to test data; assess feedback loop stability with abrupt program dynamics.
- If using the Titan product name commercially, independently review trademark/trade dress concerns and use original artwork.
