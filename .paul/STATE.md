# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-06-03)

**Core value:** Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.
**Current focus:** v0.2 Interactive UI (v0.2.0) — Phase 08 complete, ready to plan Phase 09

## Current Position

Milestone: v0.2 Interactive UI (v0.2.0)
Phase: 08 of 10 — Visual Keyboard Component (Complete; transitioning to Phase 09)
Plan: 08-01 unified, ready for next plan
Status: Loop closed; awaiting `/paul:plan` for Phase 09
Last activity: 2026-06-03 — Phase 08 unified, SUMMARY written, transition ready

Progress:
- v0.1 Initial Release: [██████████] 100% Complete
- v0.2 Interactive UI:  [███░░░░░░░] 33% Complete
- Phase 08: [██████████] 100% — Complete

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ✓        ✓        ✓     [Loop closed for 08-01]
```

## Accumulated Context

### Decisions

| Decision | Phase | Impact |
|----------|-------|--------|
| Visual keyboard as display-only Component (no LnF) | 08 | Self-contained, internal 30 Hz repaint timer |
| isNoteOnForChannels(0xffff, note) over non-existent isNoteOn | 08 | Canonical MidiKeyboardState API; channel mask 0xffff = any channel |
| Declare editor value members after processorRef, before ref members | 08 | Avoids -Wreorder; keeps member-init order consistent |
| Editor height 440 → 480 px for 50 px keyboard | 08 | Resized layout: spectrum shrinks by 54 px; ADSR/filter use proportional sizing |

### Deferred Issues

- QWERTY keyboard mapping — deferred to v0.2 (Phase TBD, after Phase 09/10)
- Exponential filter cutoff mapping — Phase 10 (v0.2)
- PITCH_ENV_SEMITONES hardcoded at 12 semitones (to be parameterized in v1.1)
- Filter is low-pass only (no high-pass or band-pass modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern) — v0.3
- pluginval strictness high test for VST3 — not run at 08-01 verify (pluginval not in PATH); recommend running before v0.2 ships

### Reference Codebase

**Giada** (`giada/`) — C++23 loop machine mapped at `giada/.paul/codebase/`:
- `TECH-STACK.md` — FLTK, JUCE plugin host, vcpkg, RtAudio/RtMidi
- `ARCHITECTURE.md` — 6-layer MVC, atomic swapper, real-time rendering
- `PATTERNS.md` — Lock-free queues, facades, atomic model swap, concurrent queues
- `CONCERNS.md` — 52 TODOs, god classes, assert misuse, global singletons

### Git State

Last commit: 852a3da (feat(08-01): add VisualKeyboardComponent for MIDI note display)
Branch: main
Uncommitted: .paul/ROADMAP.md, .paul/STATE.md, .paul/paul.json, .paul/PROJECT.md, .paul/phases/08-visual-keyboard-component/08-01-SUMMARY.md, Source/PluginEditor.{cpp,h}, Source/PluginProcessor.{cpp,h}, deleted build_standalone.bat, untracked build_kb.{bat,log}, build_vst3.{bat,log}, Absorb/, giada/, resources/Loop-Specs.md, resources/Screenshot 2026-06-03 125956.png

## Session Continuity

Last session: 2026-06-03
Stopped at: Phase 08 complete, SUMMARY written, transition ready
Next action: Run phase transition (commit phase 08, then plan Phase 09)
Resume file: .paul/phases/08-visual-keyboard-component/08-01-SUMMARY.md
Resume context:
- Plan 08-01: custom VisualKeyboardComponent + MidiKeyboardState integration
- 2 task commits (`852a3da`, `c91cca7`) + 1 human-verify checkpoint approved
- Editor height 440 → 480 px to accommodate 50 px keyboard
- No mouse interaction on keys (scope limit); visual display only

---
*STATE.md — Updated after every significant action*
