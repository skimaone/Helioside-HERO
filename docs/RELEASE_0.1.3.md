# HERO 0.1.3 — audible colour range and compact opening size

The red VCA Colour and Dynamic Colour knobs remain the Titan-style controls described in the supplied operator manual, not Trakka’s gain-slew VCA artifact selector. VCA blends the clean gain cell with the existing diode surrogate. Dynamic Colour expands waveform curvature with predominantly H3 opposite the Vintage contribution; it operates only during reduction. Parallel Mix still ranges from approximately 6% to 100% processed signal. The controls can partially offset each other, so compare them separately with Parallel Mix at COMP before judging their range.

## Targeted gain-cell change

Previously both nonlinearities shaped the already attenuated sample, while a smoothstep activation also suppressed them heavily below 9 dB GR. The cubic terms therefore lost strength roughly with the square of attenuation relative to the wanted signal. This made the controls especially restrained under ordinary compression.

Both existing curves now operate at the signal’s incoming gain-cell reference and receive the same controlled attenuation afterward. This is an inferred gain-cell model, not a recovered physical diode circuit. The smooth activity function is `1 - exp(-activeGR/3)`; one preserved solver grid quantum (50/65536 dB) is excluded so its numerical midpoint floor cannot activate colour below compression onset. No separate always-on saturation, noise, transformer or tube stage is added. Existing drive, strength and asymmetry coefficients, feedback law/guard, detector, curves, timings, tone, parallel mixing and makeup are retained. Colour remains in the existing pre-makeup feedback tap, so coloured modes can affect feedback GR, as expected for the manual’s Dynamic Colour behavior.

With both colours at zero the original compression remains unchanged within the existing 2e-6 reference tolerance across 96 varied rate/curve/tone/link/detector/feedback scenarios. These comparisons deliberately exclude the revised nonzero colour paths. Thirteen host parameters, IDs/ranges/defaults and state format remain; nonzero colour presets intentionally adopt the refined sound. Whole-engine 4x/8x FIR oversampling, 65-sample host latency, exact delayed bypass, slow red controls, three-position Tone, LCD texture and timing display are retained.

## Before/after measurements

48 kHz double-precision core/float output, 1 kHz sine, peak input 0.35. Threshold is solved separately for each case to match approximately 6 dB measured reduction; final measurement is the last second of a three-second tone. Scalar level differences are removed for residual measurements. These are synthetic model measurements, not a hardware listening match.

| Control endpoints | 0.1.2 H3 | 0.1.3 H3 |
| --- | ---: | ---: |
| VCA 15, Dynamic 0 | −44.13 dBc | approximately −31.81 dBc |
| VCA 0, Dynamic 15 | −54.32 dBc | approximately −42.13 dBc |

The complete [matched-GR CSV](measurements/0.1.3/matched-gr-before-after.csv) covers peak inputs 0.15/0.35/0.70 and 3/6/12 dB GR. This tests the shape difference without counting simple gain changes as character. In a separate actually loaded Linux VST3 probe at fixed default controls, full VCA versus zero has a level-matched waveform residual of −30.36 dB relative to output; full Dynamic versus zero is −41.16 dB. This latter probe verifies routing through the real oversampling/plugin wrapper; it does not match feedback GR independently.

The new colour contract covers all 16 isolated steps, exact inactivity below reduction onset, opposite coherent H3 signs, Dynamic influence on feedback GR, timing dependence, and hot combined endpoints at 44.1/48/96/192 kHz. Opposite signs are established for this model probe; a frequency-independent hardware phase match is not claimed. The current full-colour 15 kHz foldback probe is reduced by about 73 dB at 4x and 84 dB at 8x locally; this is a specific synthetic alias test.

The manual’s approximately 13% Vintage THD at +24 dBu is an operating-point specification, not a calibration for every input/GR combination. Its exact line-level alignment, gain-control state and spectrum cannot be recovered from the supplied musical MP3s. Numerical colour activation and the incoming-cell reference remain explicit engineering assumptions. No new hardware-fidelity percentage is claimed.

## Opening size

The code’s nominal default was already 1200×111, but a globally remembered 200% width could make the next instance open at 2400×222. A one-time preference migration caps legacy widths at 1200 and retains smaller legacy choices. Fresh instances use 1200×111, compared with Trakka’s 1440×133 logical pixels. New deliberate zoom choices still persist; no continuous reset fights user resizing. Original 1200/2400 artwork, aspect ratio, control geometry and 75–200% menu remain.

Preference regression fixtures use an isolated directory, so they test migration and later zoom persistence without changing real user settings or requiring access to a developer home directory. Production preference paths are unchanged.

Local testing establishes all 15 current suite statuses passing, including a focused editor/native rerun after isolating preference fixtures. Native window lifecycle requires a display and is exercised on Windows CI. Windows x64 Release passed all 15 CTest suites with zero failures and zero skips (19 seconds) using MSVC / windows-2022 / JUCE 8.0.4. The 96-case Clean reference gives maximum sample error 0. The actual Windows VST3 confirms level-matched residuals of −30.356 dB for VCA and −41.158 dB for Dynamic. Both preference migration and subsequent zoom persistence pass on Windows; the compact capture is exactly 1200×111. Native playback/window lifecycle, project recall, no-allocation processing, stable 65-sample latency and all three exact delayed bypass routes pass. The complete artifact was downloaded again and independently verified against GitHub’s SHA256, the inner ZIP checksum, per-file manifest, AMD64 exports, 0.1.3 identity and original class IDs. All 12 original GUI PNG masters and the LCD font are embedded byte-for-byte in the delivered DLL. FL Studio itself is not installed on the runner.


## Windows download

- [Complete Windows x64 VST3](https://github.com/skimaone/Helioside-HERO/actions/runs/40083175534/artifacts/12556031867)
- [Windows build](https://github.com/skimaone/Helioside-HERO/actions/runs/40083175534)
- [Tests and high-resolution GUI captures](https://github.com/skimaone/Helioside-HERO/actions/runs/40083175534/artifacts/12555328851)
- [Machine-readable delivery verification](WINDOWS_DELIVERY_0.1.3.json)
- Compiled revision: `6674d3898faf79cbf7b6975a3a155be9abd47e47`
- Inner plugin ZIP SHA256: `7af22d7a01581fbdf598d2f076d3b4ca42a13635b29e79fee07f88601969ea91`

Close FL Studio before replacing the complete `Suntech Audio HERO.vst3` folder; preserve `Contents` and rescan. The compact default is in logical UI pixels; the host/OS controls physical display scaling.
