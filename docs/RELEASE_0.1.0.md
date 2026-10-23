# Suntech Audio HERO 0.1.0 — verified Windows VST3

Built with MSVC x64 Release on windows-2022 and JUCE 8.0.4. All 14 CTest suites passed with no skips/failures (approximately 11.6 seconds). Original research tests retain active assertions in Release.

- [Complete Windows VST3](https://github.com/skimaone/Helioside-HERO/actions/runs/39965710047/artifacts/12506857462)
- [Tests, measurements and 100%/200% GUI screenshots](https://github.com/skimaone/Helioside-HERO/actions/runs/39965710047/artifacts/12507565450)
- [Matching PDB/MAP symbols](https://github.com/skimaone/Helioside-HERO/actions/runs/39965710047/artifacts/12507655226)
- [Successful Windows run](https://github.com/skimaone/Helioside-HERO/actions/runs/39965710047)
- Compiled revision: `ba6c0b3b1262d317ca55c9a0d1219c548e3c45df`
- Inner VST3 ZIP SHA256: `425d87b583ba20eaaee84237e424e63984a382d5ac89622bb5a4a5e655b30b75`

## Verified behavior

| Check | Result |
|---|---|
| Production vs untouched v10 core | 96 scenarios; maximum sample difference 0 |
| Independent whole-engine FIR reference | Maximum absolute difference 1.49e-8 at both 4x/8x |
| Host parameters and state | All 13 values initialize and restore; unrelated state safely ignored |
| Mono/stereo and rates | 44.1/48/96/192 kHz; short, irregular and oversized callbacks stay finite |
| Reported latency | Fixed 65 host samples for both quality settings |
| Power, OUT and host bypass | Settled output is a sample-exact delayed raw copy, including 19 kHz input |
| Block partitioning | 1/127/512 sample blocks agree within 2e-6 at both quality settings |
| Audio-thread C++ allocations | Zero in prepared callback and automated transitions |
| Native VST3 | Factory scan, stereo I/O, parameters, audio, recall and three live editor openings passed |
| GUI | Supplied assets decode, controls automate, red knobs wrap, LCD readouts are visibly rendered, 75–200% captures produced |
| Bundle | AMD64 DLL exports GetPluginFactory/InitDll/ExitDll; correct name/vendor/version and three stable class IDs |

All ten supplied GUI PNGs and the embedded LCD font were also located byte-for-byte inside the downloaded Windows DLL. The complete four-file bundle includes the DLL, moduleinfo.json and two license notices. The Actions ZIP matched GitHub's digest; its inner archive, file manifest, PE exports and metadata were independently reverified after download. See [delivery record](WINDOWS_DELIVERY.json).

## DSP scope and measured limits

The supplied regularized feedback law, default peak detector, colour curves and provisional calibration are retained. Caching and the equivalent analytic equilibrium solve reduce repeated CPU work; 20 ms parameter/quality/bypass ramps and host-correct FIR/dry alignment are integration changes. The Windows LCD uses software-backed staging images before dot sampling to avoid deferred Direct2D readback; the actual editor still uses the normal platform renderer.

In one controlled 48 kHz, 15 kHz / 0.9-amplitude colour test at threshold 60, shape/attack 0 and both colour controls 15, the 3 kHz foldback bin measured −143.79 dBFS at 4x and −139.54 dBFS at 8x, about 89.59/85.34 dB below the native-rate core. These bins approach the numerical floor and are not a universal aliasing specification or evidence that one quality setting always wins. The original research oversampler test is separate and is not the production audio path.

The same one-second stereo probe ran about 4.18/4.19 times realtime on the runner. Both quality paths remain warm, so this includes their combined CPU cost. This is a single-instance synthetic benchmark, not an FL Studio CPU guarantee. [CSV results](WINDOWS_ALIAS_CPU.csv).

The hardware detector, semiconductor topology, exact nonlinear harmonic phase and envelope calibration remain unidentified. This is a working independent Titan-inspired VST3 based on the supplied research, not a verified circuit-identical replica. FL Studio itself was not available; the actual Windows VST3 was tested through the native host. HERO has no final digital true-peak brickwall limiter.

## Install

Extract the Actions artifact, then its inner plugin ZIP. Close FL Studio and copy the entire `Suntech Audio HERO.vst3` folder to `C:\Program Files\Common Files\VST3`, keeping Contents intact. Rescan in Plugin Manager. Choose Suntech Audio HERO 0.1.0, and use a normal effect-slot mix of 100% when evaluating its own Parallel Mix control.

The native VST3 wrapper owns a separate Win32 child view; its `native-vst3-editor.png` wrapper snapshot can be black. Use the `gui/hero-width-1200.png` and `gui/hero-width-2400.png` editor captures for the actual rendered interface. Native loading/playback/window lifecycle are checked separately.
