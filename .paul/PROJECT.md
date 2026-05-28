# Samplexpress

## What This Is

A JUCE-based sample player implementing Classic Mode playback — polyphonic, pitch-shifted across the keyboard. Users load a WAV or MP3 sample, play it via QWERTY keys (standalone) or MIDI (VST3), and shape the sound with ADSR envelopes for Volume, Filter, and Pitch.

## Core Value

Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.

## Current State

| Attribute | Value |
|-----------|-------|
| Type | Application (VST3 + Standalone) |
| Version | 0.1.0 |
| Status | In progress |
| Last Updated | 2026-05-28 |

**Reference codebase mapped:** `giada` (monocasual/giada) — C++23 loop machine with atomic model swapping, real-time rendering, and FLTK UI. Best practices to reuse: sample looping with crossfade, lock-free concurrency (`moodycamel::ConcurrentQueue`), atomic model swapper pattern, and strict real-time thread safety.

## Requirements

### Core Features

- Load a sample from local folder (drag-drop or file browser)
- Play via computer keyboard (QWERTY keys mapped to notes, polyphonic, pitch-shifted per key)
- Shape with ADSR envelopes (Volume, Filter, Pitch)

### Validated (Shipped)

- [x] JUCE project builds as VST3 and Standalone with CMake + Ninja — Phase 1
- [x] VST3 passes pluginval strictness level 10 — Phase 1
- [x] Standalone app launches with visible editor window — Phase 1
- [x] Sample loading via file browser (Load button) — Phase 2
- [x] Sample loading via drag-drop onto editor — Phase 2
- [x] AudioFormatManager with WAV/MP3/AIFF support — Phase 2
- [x] State persistence (base64-encoded audio in ValueTree) — Phase 2
- [x] Visual feedback for drag-over (cyan border) — Phase 2
- [x] Polyphonic playback with pitch-shifting across keyboard range — Phase 3
- [x] 16-voice Synthesiser with voice allocation — Phase 3
- [x] Note-off with ~20ms release fade — Phase 3
- [x] Thread-safe sound changes (SpinLock) — Phase 3
- [x] Volume ADSR shapes audio amplitude — Phase 4
- [x] Pitch ADSR modulates pitch (no effect at defaults) — Phase 4
- [x] Filter ADSR modulates low-pass filter cutoff — Phase 4
- [x] Cutoff and resonance controls work — Phase 4
- [x] Play button triggers sample preview (thread-safe) — Phase 4
- [x] VST3 installs to per-user path (no admin) — Phase 4

### Active (In Progress)

- [ ] QWERTY keyboard mapping for standalone mode
- [ ] Visual keyboard component
- [ ] Final UI layout and build verification

### Planned (Next)

- [ ] Sample looping with crossfade (inspired by giada loop machine architecture)
- [ ] Cubic interpolation for pitch-shifting (replace linear)
- [ ] Exponential filter cutoff mapping
- [ ] Preset system — save/load/browse/delete presets

### Out of Scope

- Multi-mode filter (HP/BP modes, Drive) — deferred to v1.1
- LFO modulation — deferred to v1.1
- One-Shot Mode — not in scope
- Slice Mode — not in scope
- Warp/time-stretch engine — not in scope

## Target Users

**Primary:** Electronic music producers and sound designers
- Work with single samples to create instruments
- Need quick audition and playability without complex setup
- Want a lightweight alternative to full samplers for simple tasks

**Secondary:** Beginner producers exploring sampling
- Learning sampler concepts
- Want immediate playability from the computer keyboard

## Context

**Business Context:** Inspired by Ableton Simpler's Classic Mode — a focused, single-sample instrument rather than a full multi-sampler.

**Technical Context:** JUCE framework handles audio I/O, plugin format wrapping, and cross-platform concerns. MP3 decoding via JUCE's built-in decoder. Standalone uses QWERTY keyboard; VST3 receives MIDI from DAW host.

## Constraints

### Technical Constraints

- JUCE framework at `C:\Users\ideiudicibus\JUCE` (required)
- CMake + Ninja build system
- Windows-only (host arch) for local builds
- MP3 decoding via JUCE built-in decoder
- QWERTY keyboard input for standalone mode
- VST3 receives MIDI from DAW host (not QWERTY)

### Business Constraints

- Local builds only — no CI/CD
- No external dependencies beyond JUCE

## Key Decisions

| Decision | Rationale | Date | Status |
|----------|-----------|------|--------|
| Classic Mode only (no One-Shot/Slice) | Focused v1 scope — most versatile mode | 2026-05-24 | Active |
| Skip multi-mode filter for v1 | Reduce complexity; ADSR shaping is the core value | 2026-05-24 | Active |
| QWERTY keyboard for standalone | Enables playability without MIDI controller | 2026-05-24 | Active |
| isBusesLayoutSupported over getNextBestLayout | Correct JUCE 8.x API (latter is private non-virtual) | 2026-05-24 | Active |
| VS Build Tools 2026 + Ninja build pipeline | Required for MSVC compiler; only available toolchain | 2026-05-24 | Active |
| pluginval v1.0.4 for validation | Meets >= 0.6.7 requirement; latest stable | 2026-05-24 | Active |
| Base64-encoded audio in state (not file paths) | Portable across systems; larger state size | 2026-05-24 | Active |
| Timer polling (10 Hz) for sampleChanged flag | Simple; sufficient for metadata display | 2026-05-24 | Active |
| getWildcardForAllFormats() for FileChooser | Auto-includes all registered formats | 2026-05-24 | Active |
| Cyan 3px border for drag-over visual feedback | Clear visual indicator against dark background | 2026-05-24 | Active |
| 16 voices for polyphony (Synthesiser) | Sufficient for typical use; adjustable later | 2026-05-24 | Active |
| Linear interpolation for pitch-shifting | Acceptable for v0.1; better interpolation deferred | 2026-05-24 | Active |
| Default root note = MIDI 60 (middle C) | Standard convention | 2026-05-24 | Active |
| SpinLock for sound changes | Thread safety between message and audio threads | 2026-05-24 | Active |
| appliesToVelocity omitted from SynthesiserSound | JUCE 8 API doesn't include this virtual method | 2026-05-24 | Active |
| StateVariableTPTFilter for low-pass | TPT topology handles per-sample cutoff changes without artifacts | 2026-05-24 | Active |
| VCF before VCA signal chain | Standard subtractive synth; resonance rings through volume release | 2026-05-24 | Active |
| Atomic flag for Play button | Thread-safe UI→audio communication via std::atomic in processBlock | 2026-05-24 | Active |
| Per-user VST3 path | %LOCALAPPDATA% avoids admin elevation requirement | 2026-05-24 | Active |
| Cubic interpolation for pitch-shifting | Replace linear; giada uses libsamplerate | 2026-05-28 | Planned |
| Sample looping with crossfade | Core of new phase; giada real-time loop rendering pattern | 2026-05-28 | Planned |
| Lock-free queue for UI-audio communication | giada `moodycamel::ConcurrentQueue` pattern | 2026-05-28 | Planned |

## Success Metrics

| Metric | Target | Current | Status |
|--------|--------|---------|--------|
| Sample loads and plays polyphonically | WAV + MP3, 8+ voices | ✓ 16 voices | Validated |
| Pitch-shifting across keyboard | ±2 octaves range | ✓ linear interpolation | Validated |
| ADSR shaping audible | All 3 envelopes functional | ✓ Volume, Filter, Pitch | Validated |
| VST3 loads in DAW | Passes pluginval high | ✓ strictness 10 | Validated |
| Standalone plays via QWERTY | All mapped keys respond | - | Not started |
| File browser loading | Load button opens native dialog | ✓ | Validated |
| Drag-drop loading | Drop audio file onto editor | ✓ | Validated |

## Tech Stack / Tools

| Layer | Technology | Notes |
|-------|------------|-------|
| Framework | JUCE 8.0.12 | At C:\Users\ideiudicibus\JUCE |
| Language | C++20 | Modern JUCE conventions |
| Build | CMake + Ninja | VS Build Tools 2026 (MSVC 19.50) |
| Plugin Format | VST3 | Copied to %LOCALAPPDATA%\Programs\Common\VST3 |
| Standalone | .exe | Launched directly |
| Validation | pluginval 1.0.4 | Strictness level 10 |
| Audio Formats | WAV, MP3 | JUCE built-in decoders |

---
*Last updated: 2026-05-28 — Phase 6 (loop playback) planning initiated. Giada codebase mapped at `giada/.paul/codebase/`.*