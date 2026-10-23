# Suntech Audio HERO 0.1.1 — hardware GUI and control feel

The updated supplied faceplate, Suntech sun logo, finned knob strips, two-position toggles, recessed LED window and individual lime/orange/red LED sprites replace the previous artwork. Shadows are baked into the knob sprites; no second shadow layer is applied. The supplied 1x panel is used at 100% physical scale; the 2x panel serves larger/HiDPI views. The live LCD uses area-averaged text sampling and a sharper dot cache.

Red encoders now require 16 logical pixels per detent, eight times their previous drag distance; Shift requires 64. Teal controls require a 360-pixel full-range drag, or 1440 with Shift. Small drag reversals do not chatter around a detent, adding Shift during a drag does not jump, and smooth wheel input accumulates rather than skipping values. Red encoders still wrap.

LINK and NORMAL are preserved in the right-click menu. The IN / OUT front switch remembers the chosen linked/independent stereo behavior. The FAT / AIR front switch is accompanied by a neutral NORMAL badge when the menu selects NORMAL. The original Linked and NORMAL defaults remain. An optional state attribute remembers the stereo preference while OUT without adding host parameters; earlier states remain accepted.

## Audio scope

No engine, gain law, parameter mapping or calibration change is included. Bus negotiation, prepare/reset, audio processing and bypass code are identical to 0.1.0. All 13 parameter IDs, defaults, ranges and VST3 class IDs are retained; latency remains 65 host samples. This is a GUI/control revision of the supplied Titan-inspired research model.

## Validation

Local Release processor/editor/native-wrapper suites passed. Input tests exercise actual JUCE mouse events for detent distance, reversal, Shift changes, wrap, trackpad input, balanced host gestures and toggle behavior. State tests include Independent → OUT → save/restore → IN. GUI captures cover 75–200% sizes plus the LCD cache.

Windows MSVC x64 Release on `windows-2022` with JUCE 8.0.4: **all 14 CTest suites passed**, no skips or failures (approximately 11 seconds). The actual native VST3 opens/reopens during audio, restores state and reports the expected identity and latency. The original supplied DSP regressions remain enabled in Release.

The downloaded Actions archives match GitHub's SHA256 digests. The inner plugin ZIP, all four bundle files, AMD64 exports and 0.1.1 metadata were independently reverified. All twelve GUI PNGs and the LCD font were found byte-for-byte in the Windows DLL. Windows 100%/200% captures and the high-resolution live LCD were visually checked.

- [Download complete Windows VST3](https://github.com/skimaone/Helioside-HERO/actions/runs/39976283992/artifacts/12511562027)
- [Tests and GUI captures](https://github.com/skimaone/Helioside-HERO/actions/runs/39976283992/artifacts/12512220181)
- [Matching PDB/MAP symbols](https://github.com/skimaone/Helioside-HERO/actions/runs/39976283992/artifacts/12512190294)
- [Successful build](https://github.com/skimaone/Helioside-HERO/actions/runs/39976283992)
- Compiled revision: `ca8daa597c8dad4e4228b8574a8170756f25330c`
- Inner plugin ZIP SHA256: `03fbc986ef218809428e364bcc9c4921972f6b65b9669083a0cccb3f49c21545`
- [Verified delivery record](WINDOWS_DELIVERY_0.1.1.json)

Extract the Actions artifact and its inner `SuntechAudioHERO-0.1.1-Windows-x64-VST3.zip`. With FL Studio closed, replace the complete `Suntech Audio HERO.vst3` folder in `C:\Program Files\Common Files\VST3`, then rescan. Keep the bundle's `Contents` folder intact.

FL Studio itself is not available on the runner. Native VST3 validation tests the actual Windows module, including audio, state and repeated editor openings. The native wrapper snapshot may be black because the editor owns a separate Win32 child view; use the editor harness captures under gui/ for rendered artwork.
