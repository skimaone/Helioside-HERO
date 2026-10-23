# HERO 0.1.0 implementation notes

## What the evidence supports

The supplied manufacturer manual describes stepped controls, feedback dynamics, PWM/diode colour blending, reduction-dependent Dynamic Colour and Fat/Air, parallel mixing with approximately 6% wet at Direct, and relay bypass. It provides nominal attack/release/ratio ranges, not a transferable software circuit model. Musical MP3 comparisons do not establish harmonic phases or uniquely identify the detector/feedback law.

The supplied v10 engine is the starting model. Its regularized feedback law, input-equilibrium guard, peak detector, diode/cubic proxies and voicing coefficients remain research assumptions. Legacy/equilibrium laws and alternative detector models remain available to the original research tests, not as new front-panel sound modes. No additional saturation or invented hardware harmonic target is added.

## Production integration

- APVTS exposes 13 version-1 parameters. XML state is versioned, recalled into existing/new instances, rejects unrelated/invalid state and clamps restored values.
- Mono and stereo main buses are supported. No external sidechain is invented; LINK shares detector control and IN uses independent channels.
- Whole-loop 4x and 8x JUCE high-quality linear-phase FIR oversamplers use integer latency, both preallocated and kept warm. Delay padding aligns both quality paths and raw bypass to a constant 65 samples. Internal parallel dry/wet mixing happens on the same oversampled clock.
- Power, OUT and host bypass settle to a bit-identical delayed raw input; they do not leave FIR treble phase in the bypassed path. Bypass/quality transitions fade for 20 ms.
- Eight numeric engine controls use fractional internal values for 20 ms ramps while host values remain the supplied integer steps. Tone and link weights also ramp. Coefficients are updated on a persistent eight-host-sample control grid, which is independent of host block boundaries.
- The audio callback sanitizes non-finite input, uses denormal suppression, preallocated small buffers, fixed delay rings and atomic metering. It takes no locks and constructs no heap buffers. Large and zero-length callbacks are handled.

## Numerically equivalent CPU work

The reference recomputes a 16-iteration bisection and shape powers in every sample/channel. The production core analytically solves the same monotonic soft-knee feedback equation, including the reference bisection's midpoint grid, and caches parameter/detector coefficients. Stable integer settings retain the supplied output; 96 varied rate/curve/tone/link/detector/feedback scenarios are compared to untouched v10 source. Continuous weights only affect transitions, with the original endpoint behavior retained.

The two oversampling paths both run during normal playback to avoid a cold detector/envelope when quality changes. Reported CPU measurements therefore include this policy. No sample-rate-dependent control retuning is presented as hardware evidence.

## Interface

The supplied 2400×222 panel, separately lit red/teal filmstrips, fixed shadows, toggles and glass LED meter are embedded unchanged. The 1200×111 default window scales to 200%. The live LCD renders a 100×80 round-dot grid and follows the engine's actual threshold/shape law. Its knee graph is an explanatory clean static curve, not a prediction of transient colour behavior. The GUI pack's alternative control mappings are not substituted into the DSP.

## Validation

The original nine CTest programs run unchanged with assertions enabled in Release. New checks cover core equivalence, independent FIR-wrapper comparison, sample-exact bypass, fixed latency, mono/stereo rates, block-size invariance, state, automation, allocations, aliasing/CPU, live GUI controls/assets/snapshots and actual dynamically loaded VST3 audio/window lifecycle. Build/test measurements and 100%/200% captures are delivered as Actions artifacts.

Hardware-matched transfer, phase-referenced harmonics, exact transient calibration, ABX and FL Studio manual testing remain outside these automated checks. This release makes no quantitative hardware-fidelity claim.

Current Windows results and downloads are in [the 0.1.0 release report](RELEASE_0.1.0.md).
