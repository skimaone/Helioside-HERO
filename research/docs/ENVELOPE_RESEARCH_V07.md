# TITAN v0.7: Envelope study and detector refactor

## Input evidence
Four user-supplied MP3 control demonstrations, original hardware manual, and previous v0.6 package. `tests/envelope_study_v07.py` provides reproducible 10-ms envelope RMS metrics and approximate passage correlations; results saved in `docs/ENVELOPE_STUDY_V07.json`. The script divides passages into equal durations and excludes the outer 10% of each; these are **not verified control-switch sample indices**.

## Evidence and identifiability
- Shape 99/0/99: RMS dBFS approximately -22.19 / -28.52 / -22.49 and P90–P10 envelope spans ~21.35 / 17.06 / 20.64 dB. The middle passage is dynamically narrower, consistent with stronger compression.
- Attack 0/99/0: strongly differing first and final reference levels and low envelope correlations make control-only time-constant estimation unreliable.
- Release 0/99/0: lower middle RMS is compatible with maintained attenuation, not a unique fitted release constant.
- Envelope correlation under these coarse sections is low to moderate, so ratio-of-envelope traces cannot be interpreted as measured instantaneous gain reduction. The original input and exact switch timestamps are unavailable. No ms-precise hardware attacks, RMS detector topology, or H3 phase can be recovered from these MP3s.

## Actual code change in TitanCore.h
The v0.6 core smoothed detector magnitude with the nominal attack/release coefficients, and then applied the SAME coefficients to the gain reduction target. This cascaded response changes effective time constants. v0.7 uses a rapid rectifier with independent fixed provisional constants (0.25 ms rise, 4 ms decay) and follows it with attack/release-controlled GR smoothing. This is a *control-structure improvement*, not a claim about the actual Titan control circuit. The feedback input still comes from the modeled wet output before parallel blending and makeup. A sample of delayed feedback remains, as in the previous model.

## Test evidence
`tests/test_envelopes_v07.cpp` compares held 500-Hz tone behavior with fast and slow attack and verifies finite outputs. Older regression tests are retained. The CMake target registers the new test; original v0.6 package did not include all old tests in the CMake manifest.

## Next steps for Codex
1. Make detector time constants separately tunable for calibration. Model peak, power/RMS and hybrid detector candidates, then compare against controlled step and burst measurements.
2. Reconstruct exact control-change timestamps and loop phase from uncompressed controlled input and wet recordings before doing time-domain system identification.
3. Fit Shape/Attack/Release parameters simultaneously, including possible multi-stage and GR-dependent recovery. Do not use file-wide RMS shifts as exact attenuation laws.
4. Integrate genuine low-aliasing 4x/8x processing for nonlinear audio path; prior oversampling utility is a research-only component.
5. Build JUCE VST3, automate controls, state recall, latency compensation, handle sample-rate changes and denormals, and add end-to-end DAW tests.
