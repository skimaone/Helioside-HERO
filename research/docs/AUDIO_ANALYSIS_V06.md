# v0.6 original audio-analysis findings (2026-10-23)

Six actual Amazona recordings have been provided by the user. This study measures four new MP3s at 44.1 kHz stereo. These recordings are compressed music, not controlled test signals; these results are calibration TARGETS only. The `MEASURED_AUDIO_METRICS_v06.json` contains raw summary measurements and exact analysis windows. The Python script `tests/analyze_reference_audio.py` reproduces metrics when the original source recordings are available alongside the project in `/mnt/data` (change input path for portability).

## Results — central 70% of approximate equal-length segments

* Shape 99→0→99: RMS -21.82 / -28.52 / -22.40 dBFS. Shape 0 is 6.41 dB quieter than mean of flanking Shape 99 passages. The difference cannot be decomposed into a definitive ratio or knee law.
* Attack 0→99→0: RMS -31.63 / -27.22 / -35.17 dBFS. Attack 99 is 6.18 dB louder than average of Attack 0 segments; flanking passages differ by 3.54 dB, so this is a less stable numerical target. Do not fit a specific time constant from it.
* Release 0→99→0: RMS -21.47 / -25.67 / -21.79 dBFS. Release 99 is 4.04 dB quieter than mean flanking Release 0 passages. Difference is consistent with more sustained attenuation on this material, NOT a directly measured release time constant.
* Normal→Fat→Normal→Air: RMS -18.46 / -17.44 / -18.47 / -18.46 dBFS. Bandpass RMS 30–150 Hz: Normal -20.69 / -20.72, Fat -19.57 => Fat +1.14 dB. Bandpass RMS 2.5–12 kHz: Normal -36.26 / -36.28, Air -35.71 => Air +0.56 dB. Mid 150–2500 Hz virtually unchanged. These are cleaner targets than static boost speculation.

## DSP decisions now implemented

1. Preserve v0.5 Shape mapping direction; provide evidence to future fit routines, but DON'T force the static model to equal 6.41 dB for an unrelated test stimulus.
2. Keep effective attack and release coupled to feedback; use music changes only as directional tests, not a measured attack/release control-point table.
3. Preserve sidechain weighting as the major Fat/Air mechanism, but tune the secondary gain-reduction-dependent audio correction to approximately +1.1 dB (Fat) and +0.55 dB (Air) at 12 dB modeled GR. These are *musical-excerpt* numbers, not the manual's +3 dB/12 dB GR test-mode response. The published +3 dB response is another separate stimulus-specific constraint; matching both requires calibrated spectral gain-control modeling.
4. Keep zero gain reduction -> no coloration for Dynamic Color and zero Fat/Air audio correction.
5. No inferred H2/H3 phase/THD numbers: these materials don't contain coherent isolated tones.

## Next steps for Codex

* Create calibrated test-excitation harness: repeated drums, frequency sweeps, stepped tones, bursts. Use least squares with confidence intervals to optimize parameter curves against hardware output when source excitation is available.
* Validate on exactly matched musical excerpts and use multiresolution alignment before optimizing; avoid assuming changes in MP3 passages prove all dynamic response differences.
* Implement stereo matched envelopes, parameter smoothing, true oversampled nonlinear stages with decimation filters and latency compensation; current oversampler remains standalone research code.
* Do not assert a circuit-identical emulator or claim exact release milliseconds from these MP3s.
