# Suntech Audio HERO 0.1.2 — tone toggle and LCD realism

FAT is the left tone throw, NORMAL points down, and AIR is the right throw. The existing toggle artwork supplies the lower throw through a display transform. The obsolete TONE caption is covered with neighboring panel texture and replaced by NORMAL below the lever. Clicking or dragging toward a throw selects it; keyboard left/right/down follows the same layout. The menu retains all three choices, and NORMAL remains the original default. Parameter values and saved states are unchanged.

The live LCD now follows the supplied reference renderer more closely: area-averaged glyph sampling, a 100×80 round-dot matrix at 800×640, fixed spatial dot variation, two Gaussian bloom layers, cover-glass reflection and vignette. Software-backed caches preserve reliable Windows Direct2D rendering. The bloom and texture are generated for live values rather than using a static screenshot.

The white line remains the steady-state clean-cell compression curve. Attack and release describe time behavior, so they must not alter that steady curve. A separate amber trace now shows the actual clean engine's response to an approximately one-second level sweep, rising from −35 to +25 dBu in 500 ms then returning in 500 ms, after 100 ms at the low initial level. A 48 kHz display-only engine runs 100 samples per plotted step. The axes are input/output dBu, as for the white curve. Threshold, Attack, Release and Shape feed this engine; colour, tone, make-up and parallel-path effects are intentionally omitted. This is a repeatable control-response preview, not a real-time music measurement or a calibrated hardware timing claim.

The extra engine and image effects run only in the editor when their inputs change. No audio engine, processor, parameter layout, calibration, latency, quality or bypass implementation changed. Red knob sensitivity from 0.1.1 is retained.

## Validation

Local Release processor and editor suites passed. Tone input tests cover left/right/down clicks, a downward drag, keyboard choices and default reset. The timing test finds attack/release-dependent trajectories while the white static curve remains identical when attack changes; all points remain finite and attenuating. The extreme-index sweep differences were approximately 11.04 dB for attack and 1.43 dB for release under this probe, not a hardware specification. Captures include all three throws, fast/slow timing examples, 75–200% interface sizes and the LCD cache.

MSVC x64 Release on `windows-2022` with JUCE 8.0.4: **14/14 CTest suites passed**, no skips or failures (approximately 12 seconds). The new tone and timing-display tests passed on Windows. The native VST3 host also checks loading, audio, state, latency and repeated editor openings during playback.

The downloaded Actions archives matched GitHub's SHA256 digests. The inner plugin archive, all four bundle files, AMD64 exports, unchanged class IDs and 0.1.2 metadata were independently reverified. All twelve GUI PNGs and the LCD font were located byte-for-byte in the Windows DLL. The actual Windows 200% editor and high-resolution LCD captures were visually checked.

- [Complete Windows VST3](https://github.com/skimaone/Helioside-HERO/actions/runs/39985144107/artifacts/12514798226)
- [Tests and GUI captures](https://github.com/skimaone/Helioside-HERO/actions/runs/39985144107/artifacts/12515556885)
- [Matching PDB/MAP symbols](https://github.com/skimaone/Helioside-HERO/actions/runs/39985144107/artifacts/12514878089)
- [Successful Windows run](https://github.com/skimaone/Helioside-HERO/actions/runs/39985144107)
- Compiled revision: `c2770be06490e7ac7a402e429ace3428b8c4473a`
- Inner VST3 ZIP SHA256: `a573c72fcafc1d7a74f62f725d13471c80c6c79c08624f2cd6b8664dc1972fa5`
- [Delivery record](WINDOWS_DELIVERY_0.1.2.json)

Extract the Actions artifact and its inner `SuntechAudioHERO-0.1.2-Windows-x64-VST3.zip`. With FL Studio closed, replace the complete `Suntech Audio HERO.vst3` folder in `C:\Program Files\Common Files\VST3`, then rescan. Keep `Contents` intact.

FL Studio itself is not installed on the runner; the actual Windows VST3 is exercised through the native test host. Its separate Win32 child-view snapshot can be black, so use the gui/ editor captures to review the artwork.
