# Titan research DSP v0.8

Adds **working detector-model comparison** to main C++17 engine, seven passing regression suites, generator for 48 deterministic gain-reduction response traces, and a **JUCE oversampling adapter source** (requires compile/test with installed JUCE; not compiled here).

Build core: `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`

Generate controlled traces: `./build/titan_controlled_v08 controlled_response_v08.csv`.

Read `docs/ENGINEERING_V08.md` before integrating wrapper in the VST3. This is research-stage behavior, not a circuit-exact Titan claim.
