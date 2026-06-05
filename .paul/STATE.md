# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-06-05)

**Core value:** Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.
**Current focus:** Awaiting next milestone definition (v0.3 Project Website complete 2026-06-05)

## Current Position

Milestone: v0.3 Project Website (v0.3.0) — UNIFY complete
Phase: 11 of 11 (Single-page Landing Site) — Complete
Plan: 11-01 — all 5 auto tasks + human-verify + SUMMARY written
Status: Loop complete, ready for phase transition
Last activity: 2026-06-05 — Wrote 11-01-SUMMARY.md; UNIFY complete

Progress:
- v0.1 Initial Release: [██████████] 100% ✓
- v0.2 Interactive UI:  [██████████] 100% ✓
- v0.3 Project Website: [██████████] 100% ✓ (UNIFY complete; transition pending)

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ✓        ✓        ✓     [Loop complete - ready for next PLAN / phase transition]
```

## Accumulated Context

### Decisions

| Decision | Phase | Impact |
|----------|-------|--------|
| Visual keyboard as display-only Component (no LnF) | 08 | Self-contained, internal 30 Hz repaint timer |
| isNoteOnForChannels(0xffff, note) over non-existent isNoteOn | 08 | Canonical MidiKeyboardState API; channel mask 0xffff = any channel |
| Declare editor value members after processorRef, before ref members | 08 | Avoids -Wreorder; keeps member-init order consistent |
| Editor height 440 → 480 px for 50 px keyboard | 08 | Resized layout: spectrum shrinks by 54 px; ADSR/filter use proportional sizing |
| Phase 09 scope redirected from "waveform drag" to "UI polish" | 09 | User explicit redirect at 09-01 planning; loop-drag deferred to a later plan |
| Flat (non-rounded) TextButton rendering with 2-px vertical inset | 09 | "Thinner" appearance without changing row layout |
| 34-px alpha knob centred in 28-px title row via withY(-3) | 09 | Visually larger knob without restructuring title bar |
| Preset controls live in title bar, not in SAMPLE tab content | 09 | Always accessible; reduces per-tab UI complexity |
| Spectrum height clamp via jmax(50, content-50-4) | 09 | Robust against future resizable window work |
| Phase 10 scope adopted: ADSR drag-handle UX polish (10-adsr-ux-polish) | 10 | Diverges from original "Exponential cutoff / tab z-order" ROADMAP description; deferred to later phases |
| ADSR tooltips via juce::TooltipClient (not Component::getTooltip) | 10 | Component has no virtual getTooltip; TooltipWindow dispatches to TooltipClient*; allows per-node dynamic text |
| Tab content positioned in contentMinusKeyboard strip, not full content | 10 | Visual keyboard at bottom 50 px otherwise occludes lower portion of ADSR/filter displays |
| v0.3 = Project Website (single-page landing, plain HTML+CSS, GitHub Pages) | 11 | Decided 2026-06-05; orthogonal to plugin feature work; v0.3 plugin features deferred to v0.4 |

### Deferred Issues

- QWERTY keyboard mapping — deferred to v0.4
- Exponential filter cutoff mapping — v0.4
- PITCH_ENV_SEMITONES hardcoded at 12 semitones (to be parameterized in v1.1)
- Filter is low-pass only (no high-pass or band-pass modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern) — v0.4
- pluginval strictness high test for VST3 — not run at 08-01 verify (pluginval not in PATH); recommend running before next plugin release
- `juce::Font::getStringWidth` deprecation in AdsrDisplayComponent.cpp — migrate to GlyphArrangement / TextLayout
- Tab z-order refinements and broader visual feedback (originally listed under Phase 10 ROADMAP) — deferred to v0.4

### Reference Codebase

**Giada** (`giada/`) — C++23 loop machine mapped at `giada/.paul/codebase/`:
- `TECH-STACK.md` — FLTK, JUCE plugin host, vcpkg, RtAudio/RtMidi
- `ARCHITECTURE.md` — 6-layer MVC, atomic swapper, real-time rendering
- `PATTERNS.md` — Lock-free queues, facades, atomic model swap, concurrent queues
- `CONCERNS.md` — 52 TODOs, god classes, assert misuse, global singletons

### Git State

Last commit: d68b075 (fix(09-01): clamp spectrum height so it never overlaps the keyboard)
Branch: main
Uncommitted: .paul/ROADMAP.md, .paul/STATE.md, .paul/paul.json, .paul/PROJECT.md, .paul/phases/11-landing-page/11-01-PLAN.md (new), deleted build_standalone.bat, untracked build_kb.{bat,log}, build_vst3.{bat,log}, Absorb/, giada/, resources/Loop-Specs.md, resources/Screenshot 2026-06-03 125956.png

## Session Continuity

Last session: 2026-06-05
Stopped at: UNIFY complete for 11-01; phase transition pending
Next action: Run phase transition (.paul/phases/11-landing-page is the only plan in the phase) → archive v0.3 milestone
Resume file: .paul/phases/11-landing-page/11-01-SUMMARY.md
Resume context:
- v0.3 milestone: Project Website (Phase 11 only)
- 1 plan, 1 SUMMARY, 5 new files + 1 README edit
- All 6 ACs passed; 1 minor deviation (process.html word count 190 vs. 120-180 target); human-verified
- Phase 11 is the last phase in v0.3 → transition triggers milestone close-out

---
*STATE.md — Updated after every significant action*
