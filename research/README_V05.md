# Titan-inspired v0.5 DSP research update
The **executable engine is improved** (feedback Shape slope, spectral detector weighting, diode asymmetry). Standalone oversampling research is supplied but **not yet integrated** into the audio engine. None of the inferred transfer-function coefficients are hardware-calibrated. Read `docs/RD_V05_EVIDENCE_AND_LIMITS.md` and `docs/CODEX_ACTION_V05.md`.

Build/test: `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`.

All original v0.4 files are retained. Four Amazona audio URLs are supplied as research references; their MP3 bytes were not obtainable during this session and are NOT included.
