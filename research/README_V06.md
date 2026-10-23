# Titan DSP v0.6 — four new musical-reference measurements

Build with `cmake -S . -B build && cmake --build build -j && ctest --test-dir build --output-on-failure`.
Read `docs/AUDIO_ANALYSIS_V06.md` and the JSON metrics before altering model constants. `src/TitanCore.h` includes newly measured, **provisional program-specific** Fat/Air calibration coefficients. Use them only as rough targets, not universal static EQ gains. This package is a C++17 DSP research core; it is not a compiled VST3. JUCE integration and production oversampling are still required.
