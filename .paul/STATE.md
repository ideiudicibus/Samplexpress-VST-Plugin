# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-05-24)

**Core value:** Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.
**Current focus:** v0.1 Initial Release — Phase 5 QWERTY Keyboard & UI Integration

## Current Position

Milestone: v0.1 Initial Release (v0.1.0)
Phase: 6 of 6 (Preset System & Audio Engine — Loop Playback Focus)
Plan: 06-01 created, awaiting approval
Status: PLAN created, ready for APPLY
Last activity: 2026-05-28 — 06-01 PLAN.md created (loop parameters, voice crossfade, waveform markers, UI wiring)

Progress:
- Milestone: [████████░░] 80%
- Phase 1: [██████████] 100% Complete
- Phase 2: [██████████] 100% Complete
- Phase 3: [██████████] 100% Complete
- Phase 4: [██████████] 100% Complete
- Phase 5: [██████░░░░] 60% In Progress (4/4 ad-hoc plans complete; original QWERTY still deferred)
- Phase 6: [░░░░░░░░░░] 0% Planning

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ✓        ○        ○     [Plan created, awaiting approval]
```

## Accumulated Context

### Decisions

| Decision | Phase | Impact |
|----------|-------|--------|
| Classic Mode only (no One-Shot/Slice) | Init | Defines entire v1 scope |
| 16 voices for polyphony (Synthesiser) | 03 | Sufficient for typical use |
| Linear interpolation for pitch-shifting | 03 | Acceptable for v0.1 |
| SpinLock for sound changes | 03 | Thread safety |
| appliesToVelocity omitted (JUCE 8) | 03 | API compatibility |
| setSkewForCentre() for ADSR time params | 04 | Better UX at short values |
| Pitch ADSR defaults to no effect | 04 | No audible change from Phase 3 |
| JUCE ADSR class with getNextSample() per-voice | 04 | Correct per-voice envelope rendering |
| PITCH_ENV_SEMITONES = 12 (1 octave range), hardcoded for v0.1 | 04 | To be parameterized in v1.1 |
| Pitch ADSR: deviation relative to sustain level | 04 | Default sustain=1.0 = no pitch change |
| ADSR Parameters uses float fields (not double) | 04 | JUCE 8 API — removed static_cast<double> |
| lgain/rgain consolidated to velocityGain | 04 | Simpler — both were always velocity*0.7f |
| StateVariableTPTFilter over IIR biquad | 04 | Per-sample cutoff modulation without artifacts |
| VCF before VCA signal chain | 04 | Resonance rings through volume release |
| Linear cutoff mapping (Hz) | 04 | Could be improved to exponential in v1.1 |
| Atomic flag for Play button | 04 | Thread-safe UI→audio communication |
| Per-user VST3 path | 04 | No admin elevation required |
| ADSR knobs removed entirely | 05 | User wants interaction via graphic displays only |
| FilterSpectrumComponent combined approach failed | 05 | Too complex; separate components recommended |
| Filter response curve replaces knobs entirely | 05 | Consistent with ADSR display interaction model |
| Mono mix for spectrum analyzer | 05 | Both channels identical; simplifies ring buffer |
| Complex FFT over real-only transform | 05 | Output format unambiguous across JUCE versions |
| Timer-based spectrum updates (30 Hz) | 05 | No background thread; sufficient for visualization |

### Deferred Issues

- PITCH_ENV_SEMITONES hardcoded at 12 semitones (to be parameterized in v1.1)
- Filter is low-pass only (no high-pass or band-pass modes)
- Filter cutoff mapping is linear in Hz (exponential could improve low-frequency UX)
- QWERTY keyboard mapping still deferred

### Reference Codebase

**Giada** (`giada/`) — C++23 loop machine mapped at `giada/.paul/codebase/`:
- `TECH-STACK.md` — FLTK, JUCE plugin host, vcpkg, RtAudio/RtMidi
- `ARCHITECTURE.md` — 6-layer MVC, atomic swapper (`mcl::AtomicSwapper<Document,6>`), real-time rendering
- `PATTERNS.md` — Lock-free queues, facades, atomic model swap, concurrent queues
- `CONCERNS.md` — 52 TODOs, god classes, assert misuse, global singletons (`g_engine`, `g_ui`)
- Key insight: Giada uses `std::optional<SampleChannel>` / `std::optional<MidiChannel>` for polymorphism without inheritance

### Git State

Last commit: a1544aa (Update screenshots to reflect current Ableton-inspired UI)
Previous: 6b1c407 (Add build_all.bat batch script for Windows builds)
Branch: main
Uncommitted: PROJECT.md, STATE.md, ROADMAP.md updated for Phase 6

## Session Continuity

Last session: 2026-05-28
Stopped at: Giada codebase mapped; Phase 6 scoped for loop playback with crossfade
Next action: Run /paul:plan for Phase 6 (Preset System & Audio Engine — Loop Playback)
Resume file: .paul/HANDOFF-2026-05-28.md
Resume context:
- Giada codebase map committed to `giada/.paul/codebase/` (7 docs)
- Phase 6 ready to plan: looping with crossfade, cubic interpolation, exponential cutoff, presets
- QWERTY keyboard remains deferred

---
*STATE.md — Updated after every significant action*
