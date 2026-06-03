# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-05-28)

**Core value:** Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.
**Current focus:** v0.1 Initial Release — Milestone complete, awaiting next milestone

## Current Position

Milestone: v0.2 Interactive UI (v0.2.0)
Phase: 08 of 10 — Visual Keyboard Component
Plan: Not started
Status: Ready to plan
Last activity: 2026-06-03 — Milestone v0.2 created; Phase 07 deferred to v0.3

Progress:
- v0.1 Initial Release: [██████████] 100% Complete
- v0.2 Interactive UI:  [░░░░░░░░░░] 0% Complete

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [Ready for first PLAN]
```

## Accumulated Context

### Decisions

| Decision | Phase | Impact |
|----------|-------|--------|
| Classic Mode only (no One-Shot/Slice) | Init | Defines entire v1 scope |
| 16 voices for polyphony (Synthesiser) | 03 | Sufficient for typical use |
| SpinLock for sound changes | 03 | Thread safety |
| StateVariableTPTFilter over IIR biquad | 04 | Per-sample cutoff modulation without artifacts |
| VCF before VCA signal chain | 04 | Resonance rings through volume release |
| ADSR knobs removed entirely | 05 | User wants interaction via graphic displays only |
| Cubic interpolation for pitch-shifting | 06 | Smoother pitch shifts than linear |
| Cosine crossfade at loop boundary | 06 | Click-free loop transitions |
| Preset system via XML files | 06 | No external dependencies |

### Deferred Issues

- QWERTY keyboard mapping — deferred to v0.3
- Exponential filter cutoff mapping — Phase 10 (v0.2)
- PITCH_ENV_SEMITONES hardcoded at 12 semitones (to be parameterized in v1.1)
- Filter is low-pass only (no high-pass or band-pass modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern) — v0.3

### Reference Codebase

**Giada** (`giada/`) — C++23 loop machine mapped at `giada/.paul/codebase/`:
- `TECH-STACK.md` — FLTK, JUCE plugin host, vcpkg, RtAudio/RtMidi
- `ARCHITECTURE.md` — 6-layer MVC, atomic swapper, real-time rendering
- `PATTERNS.md` — Lock-free queues, facades, atomic model swap, concurrent queues
- `CONCERNS.md` — 52 TODOs, god classes, assert misuse, global singletons

### Git State

Last commit: 1862460 (fix(ui): ensure loop controls render above spectrum background)
Branch: main
Uncommitted: .paul/ROADMAP.md, .paul/STATE.md

## Session Continuity

Last session: 2026-06-03
Stopped at: Milestone v0.2 created; Phase 07 deferred to v0.3
Next action: /paul:plan for Phase 08
Resume file: .paul/ROADMAP.md
Resume context:
- v0.2 milestone created with phases 08-10
- Phase 07 (QWERTY keyboard mapping) deferred to v0.3
- Post-milestone fix: loop controls z-order fix committed

---
*STATE.md — Updated after every significant action*
