# HERO 0.1.4 — restore TONE caption

Restores the original TONE caption beneath the right-hand FAT/AIR switch by removing the later NORMAL overlay. The caption is taken directly from the supplied 1x/2x faceplate artwork, matching MODE and METER in type, colour, texture and alignment.

FAT is left, AIR is right, and Normal remains the neutral down position. The tooltip and Tone menu retain the name Normal for clarity. Toggle behavior, DSP, parameters, presets, default size and all original embedded images are unchanged.

Verification: existing editor checks cover all three throws, keyboard/mouse gestures, artwork decoding and captures at 75–200%. Windows x64 Release on windows-2022 / MSVC / JUCE 8.0.4: all 15 CTest suites passed, zero failures or skips. Actual native VST3 loading, project recall and live GUI playback checks pass. The complete downloaded four-file bundle passes AMD64 exports, product/version, stable class IDs and SHA256 checks. All 12 original PNG masters and the LCD font remain embedded byte-for-byte. Windows captures at 100% and 200% were visually inspected for the restored TONE caption and neutral-down throw.

[Download complete VST3](https://github.com/skimaone/Helioside-HERO/actions/runs/40093417494/artifacts/12559768462) · [Windows build](https://github.com/skimaone/Helioside-HERO/actions/runs/40093417494) · [Tests and high-resolution GUI captures](https://github.com/skimaone/Helioside-HERO/actions/runs/40093417494/artifacts/12560122150) · [Delivery verification](WINDOWS_DELIVERY_0.1.4.json).

Compiled revision: `ddd0f85e79df28511a181457cd736af507d706c5`. Inner plugin ZIP SHA256: `2fd5f15a032c2b6da91cc915c0293564bb0f37ba25d662cf559e07a92054b9db`. FL Studio itself is not installed on CI.
