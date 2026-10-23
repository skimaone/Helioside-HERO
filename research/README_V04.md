# Titan research update v0.4

**Actual DSP source has been revised.** `src/TitanCore.h` is the new DSP version; `docs/RESEARCH_UPDATE_v04.md` explains the changes. Previous documents are preserved as historical reference; if they disagree with the code, v0.4 docs take precedence.

Build and run reference tests:

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

C++17; no external dependencies for core/test. Compiled and both test suites passed on Linux.

**Not included:** JUCE/VST3 binaries; oversampling and nonlinear anti-aliasing; hardware THD/phase calibration; absolute threshold mapping; released/verified compressor plug-in.

See `docs/CODEX_IMPLEMENTATION_v04.md` for next development steps.
