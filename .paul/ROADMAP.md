# Roadmap: Samplexpress

## Overview

Build a JUCE-based Classic Mode sample player — load a WAV/MP3, play it polyphonically with pitch-shifting across the keyboard, shape it with ADSR envelopes. Now evolving to support **loop playback with crossfade**, drawing on giada's real-time loop machine patterns. Ships as VST3 plugin and standalone .exe.

## Current Milestone

**v0.1 Initial Release** (v0.1.0)
Status: In progress
Phases: 4 of 6 complete
**Current focus:** Phase 6 — Loop Playback with crossfade, cubic interpolation, exponential cutoff, presets

## Phases

| Phase | Name | Plans | Status | Completed |
|-------|------|-------|--------|-----------|
| 1 | Project Setup & Scaffolding | 1/1 | Complete | 2026-05-24 |
| 2 | Sample Loading | 2/2 | Complete | 2026-05-24 |
| 3 | Polyphonic Playback Engine | 1/1 | Complete | 2026-05-24 |
| 4 | ADSR Envelopes | 2/2 | Complete | 2026-05-24 |
| 5 | QWERTY Keyboard & UI Integration | 2/3 | In Progress | - |
| 6 | Preset System & Audio Engine — Loop Playback | 0/3 | In Progress — Planning | - |

## Phase Details

### Phase 1: Project Setup & Scaffolding (Complete)

**Goal:** JUCE project builds as VST3 and standalone with CMake + Ninja
**Depends on:** Nothing (first phase)
**Completed:** 2026-05-24

**Scope:**
- CMakeLists.txt with VST3 and Standalone targets
- Basic PluginProcessor / PluginEditor skeleton
- Build verification (compiles, pluginval passes, standalone launches)

**Plans:**
- [x] 01-01: Create CMake project with JUCE VST3 + Standalone targets

**Validated:**
- VST3 passes pluginval strictness level 10
- Standalone launches with visible editor window
- Build pipeline works with MSVC + Ninja

### Phase 2: Sample Loading (Complete)

**Goal:** Users can load WAV/MP3 files via file browser and drag-drop
**Depends on:** Phase 1
**Research:** Complete (AudioFormatManager, FileChooser, FileDragAndDropTarget)
**Completed:** 2026-05-24

**Scope:**
- File browser component for selecting samples
- Drag-drop support onto plugin window
- AudioFormatManager setup for WAV and MP3
- Audio buffer storage in processor

**Plans:**
- [x] 02-01: Add AudioFormatManager, file loading, buffer storage, state persistence, and Load button UI
- [x] 02-02: Add drag-drop support for audio file loading onto editor

**Validated:**
- File browser loads WAV/MP3/AIFF via Load button
- Drag-drop loads audio files onto editor
- Visual feedback (cyan border) during drag-over
- State persistence preserves sample data across DAW save/reload

### Phase 3: Polyphonic Playback Engine (Complete)

**Goal:** Sample plays polyphonically with pitch-shifting across the keyboard range
**Depends on:** Phase 2
**Completed:** 2026-05-24

**Scope:**
- SynthesiserVoice subclass for sample playback
- Synthesiser for voice allocation and polyphony
- Pitch calculation from MIDI note vs root note
- processBlock implementation

**Plans:**
- [x] 03-01: SamplexpressVoice with pitch-shifting, Synthesiser wiring, processBlock rendering

**Validated:**
- Polyphonic playback with 16 voices
- Pitch-shifting via linear interpolation across keyboard range
- Note-off with ~20ms release fade
- SpinLock-protected sound changes for thread safety
- State persistence correctly re-creates sounds after DAW reload

### Phase 4: ADSR Envelopes (Complete)

**Goal:** Three ADSR envelopes shape Volume, Filter cutoff, and Pitch
**Depends on:** Phase 3
**Completed:** 2026-05-24

**Scope:**
- ADSR parameters as AudioProcessorValueTreeState
- Volume ADSR applied in voice render
- Filter ADSR modulating low-pass filter cutoff (StateVariableTPTFilter)
- Pitch ADSR applied as pitch offset in voice
- Cutoff and resonance controls
- Play button for standalone preview
- Per-user VST3 install path

**Plans:**
- [x] 04-01: Add ADSR parameters to ValueTreeState and UI sliders
- [x] 04-02: Apply all three ADSR envelopes in voice rendering with filter, Play button, per-user VST3

**Validated:**
- Three ADSR envelopes (Volume, Filter, Pitch) fully wired to audio rendering
- Low-pass filter with cutoff/resonance modulation via Filter ADSR
- VCF→VCA signal chain (filter before volume envelope)
- Play button with thread-safe MIDI injection
- Per-user VST3 install path (no admin required)

### Phase 5: QWERTY Keyboard & UI Integration

**Goal:** Standalone plays via QWERTY keyboard; UI shows sample info and ADSR controls
**Depends on:** Phase 4
**Research:** Likely (JUCE KeyboardComponent, keyPressEvents mapping)

**Scope:**
- QWERTY-to-MIDI-note mapping for standalone
- Visual keyboard component
- Sample info display (filename, sample rate, length)
- Final UI layout with all controls
- Build, deploy VST3, test standalone

**Plans:**
- [x] 05-01: Interactive filter response curve replaces knobs
- [x] 05-02: Real-time spectrum analyzer with FFT
- [ ] 05-03: QWERTY keyboard mapping and visual keyboard (deferred)

### Phase 6: Preset System & Audio Engine — Loop Playback

**Goal:** Sample looping with crossfade; preset system; audio engine improvements inspired by giada real-time rendering patterns
**Depends on:** Phase 5
**Status:** In Progress — Planning

**Scope:**
- Sample looping with crossfade (giada real-time loop rendering pattern)
- Loop start/end points editable via waveform display
- Loop on/off toggle with ADSR-aware release handling
- Cubic pitch interpolation (replace linear; giada uses libsamplerate)
- Exponential filter cutoff mapping (better low-frequency UX)
- PresetManager for XML-based preset save/load/delete
- Preset browser UI in editor (combobox + buttons)
- Factory "Init" preset

**Giada best practices to reuse:**
- Atomic model swapper (`mcl::AtomicSwapper`) for thread-safe parameter changes
- Lock-free queues (`moodycamel::ConcurrentQueue`) for UI→audio communication
- Real-time rendering pipeline: sequencer advance → channel render → plugin stack → mixer

**Plans:**
- [ ] 06-01: Sample looping — loop points, crossfade, waveform UI
- [ ] 06-02: Audio engine improvements — cubic interpolation, exponential cutoff
- [ ] 06-03: Preset system — save/load/browse/delete presets

---
*Roadmap created: 2026-05-24*
*Last updated: 2026-05-26 — Phase 6 added, Phase 5 QWERTY deferred*