# CODEx IMPLEMENTATION BRIEF — execute immediately

Objective: Create a cross-platform VST3 called **[choose a distinct original name]** with clean UI and experimental Titan-inspired dynamics, using the included `src/TitanCore.h`. This is independent research; do not claim official endorsement or circuit accuracy.

## Phase A — Build a working VST3
1. Inspect and preserve the compiling DSP core and existing tests. Start a JUCE 8 CMake project (`juce_add_plugin`) targeting VST3 on Windows/macOS, and optionally standalone. Let the developer configure local JUCE path; do not silently fetch code from untrusted URLs.
2. `AudioProcessorValueTreeState` parameter mapping for threshold, attack, release, shape (integers 0–99); vca color, dynamic color, parallel mix (integers 0–15); makeup 0–23; Tone {Fat/Normal/Air}; bypass; stereo link.
3. Maintain parameter migration/versioning; ensure host automation, save/load recall and no audio-thread allocation; initialize DSP `prepareToPlay`, handle sample rate changes, mono/stereo and variable blocks.
4. Create original simple accessible UI: waveform/transfer curve preview, GR/output meters, named controls and clearly mark *Research Prototype* in About. Avoid copying hardware front-panel artwork or manufacturer logos.
5. Run unit tests and DAW/load tests; document tested build configurations. Make plugin load and pass audio before advanced model changes.

## Phase B — Correct science and real-time robustness
6. Build oversampling for harmonics and level-dependent diodes, compensating latency through all wet/dry paths. Test impulse alignment and sweep alias energy.
7. Add glitch-free parameter smoothing, including color step switching and bypass crossfade.
8. Rework nonlinear gain-cell test bench: clean VCA/diode co-blend, distortion vs input level & GR, signed 3rd harmonic mapping, fundamental stability, DC blocker as needed.
9. Improve detector and lookup-limited transfer curves. Guard feedback state against runaway and bursty gain oscillations, include minimum/maximum attack and release constraints; compare control across sample rates.
10. Add deterministic offline CLI renders with WAV test vectors to prove behavior; FFT/harmonic/IMD/attack-release reports; support CSV export for model fitting.

## Phase C — Hardware identification
11. Use `MEASUREMENT_PLAN.md` to solicit source-aligned dry/wet hardware recordings. Never pretend manufacturer's screenshots are measured full transfer data.
12. Fit candidate models against captures. Expose switchable hypotheses in internal research tools, then freeze best-scoring model.
13. Validate on unseen program material including aggressive sparse bass/kick, snare, vocals and full mix. Both subjective ABX and objective metrics.

## Deliverables to commit
- JUCE VST3 source, CMake project and build instructions
- separate no-JUCE DSP core and expanded tests
- reference parameter mappings and method notes
- no claims of official affiliation, authenticity or measured accuracy without evidence

## Scope freeze
Prioritize functional VST3 + correct automation + no aliasing before chasing analog component conjectures. Hardware circuit semiconductor types, Shape curve and exact stereo link network remain UNKNOWN.
