# Codex handoff — v0.5 implementation
1. Read `docs/RD_V05_EVIDENCE_AND_LIMITS.md` before editing. The Shape law, detector weighting and diode asymmetry are **hypotheses**.
2. Build with `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`.
3. Integrate `src/OversampledNonlinearity.h` into the nonlinear wet audio path only AFTER improving anti-alias performance substantially. The module is a reference prototype and intentionally not wired to Engine. Create phase-accurate dry delay and report fixed latency through JUCE. Consider a production-grade halfband/polyphase cascade.
4. Optimize detector as needed for a 50 microsecond attack without naively operating whole plugin at 870 kHz. Expose QA control-rate multipliers and check gain-reduction error convergence.
5. Fit and test Shape vs. threshold via steady state curves, ensuring no runaway attack/release in feedback. Shape control has 100 steps.
6. Treat Fat/Air as detector coloration primarily, with optional residual audio-path compensation, and benchmark against page 5 graph at 12 dB reduction. This is an implementation hypothesis, NOT a proven circuit.
7. Make interpolating parameter changes click-free; automate 16-step knobs and 0–99 encoders. Distinguish direct (6% wet) from bypass (100% dry).
8. Download and analyze Amazona four MP3s listed in research file; infer spectral envelopes and time-constant trends, not individual THD phase from music.
9. Deliver JUCE VST3 with channel configuration, denorm protection, save/recall, sample rate changes, reporting and automated unit/spectral tests.
