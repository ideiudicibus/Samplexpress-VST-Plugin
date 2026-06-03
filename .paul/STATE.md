# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-05-28)

**Core value:** Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.
**Current focus:** v0.1 Initial Release — Milestone complete, awaiting next milestone

## Current Position

Milestone: v0.1 Initial Release (v0.1.0) — Complete
Phase: None active
Plan: None
Status: Milestone complete — ready for next
Last activity: 2026-06-03 — Milestone v0.1.0 archived; MILESTONES.md created

Progress:
- v0.1 Initial Release: [██████████] 100% Complete

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [Milestone complete - ready for next]
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

- QWERTY keyboard mapping still deferred to v0.2
- Exponential filter cutoff mapping deferred to v0.2
- PITCH_ENV_SEMITONES hardcoded at 12 semitones (to be parameterized in v1.1)
- Filter is low-pass only (no high-pass or band-pass modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern) — v0.2

### Reference Codebase

**Giada** (`giada/`) — C++23 loop machine mapped at `giada/.paul/codebase/`:
- `TECH-STACK.md` — FLTK, JUCE plugin host, vcpkg, RtAudio/RtMidi
- `ARCHITECTURE.md` — 6-layer MVC, atomic swapper, real-time rendering
- `PATTERNS.md` — Lock-free queues, facades, atomic model swap, concurrent queues
- `CONCERNS.md` — 52 TODOs, god classes, assert misuse, global singletons

### Git State

Last commit: c94707a (feat(06-loop-playback): sample looping with crossfade)
Branch: main
Uncommitted: .paul/MILESTONES.md, .paul/milestones/, .paul/ROADMAP.md, .paul/STATE.md

## Session Continuity

Last session: 2026-06-03
Stopped at: Milestone v0.1.0 complete — archived, ready for next milestone
Next action: /paul:discuss-milestone or /paul:milestone
Resume file: .paul/MILESTONES.md
Resume context:
- v0.1.0 milestone: all 6 phases complete, all acceptance criteria passed
- Build clean: zero errors, zero warnings
- VST3 installed to per-user path; standalone .exe built
- MILESTONES.md created with full accomplishment log

---
*STATE.md — Updated after every significant action*
