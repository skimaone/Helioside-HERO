# Titan v0.9 — stability-focused revision

- Three selectable feedback control laws (`Legacy`, `Regularized`, `Equilibrium`), with a separate input-envelope GR ceiling for the latter two.
- Original research DSP and six MP3 references preserved.
- 144 scenario stability benchmark and 8 standalone CTest suites.
- See `docs/FEEDBACK_STABILITY_V09.md` and `docs/STABILITY_METRICS_V09.csv` before integrating into VST3.
- **Do not call this a circuit-identical Titan emulation.** No new harmonic measurements or JUCE plugin integration are claimed.
