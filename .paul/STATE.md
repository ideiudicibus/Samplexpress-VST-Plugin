# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-06-04)

**Core value:** Users can load audio samples from local folders and play them polyphonically with pitch-shifting across the keyboard, as a VST3 plugin or standalone app.
**Current focus:** Awaiting next milestone definition (v0.2 Interactive UI archived 2026-06-04)

## Current Position

Milestone: Awaiting next milestone (v0.2 archived)
Phase: None active
Plan: None
Status: Milestone v0.2 Interactive UI complete — ready for next
Last activity: 2026-06-04 — v0.2 milestone archived

Progress:
- v0.1 Initial Release: [██████████] 100% ✓
- v0.2 Interactive UI:  [██████████] 100% ✓

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

### Deferred Issues

- QWERTY keyboard mapping — deferred to v0.2 (Phase TBD, after Phase 09/10)
- Exponential filter cutoff mapping — Phase 10 (v0.2)
- PITCH_ENV_SEMITONES hardcoded at 12 semitones (to be parameterized in v1.1)
- Filter is low-pass only (no high-pass or band-pass modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern) — v0.3
- pluginval strictness high test for VST3 — not run at 08-01 verify (pluginval not in PATH); recommend running before v0.2 ships
- `juce::Font::getStringWidth` deprecation in AdsrDisplayComponent.cpp — migrate to GlyphArrangement / TextLayout
- Tab z-order refinements and broader visual feedback (originally listed under Phase 10 ROADMAP) — deferred to next milestone

### Reference Codebase

**Giada** (`giada/`) — C++23 loop machine mapped at `giada/.paul/codebase/`:
- `TECH-STACK.md` — FLTK, JUCE plugin host, vcpkg, RtAudio/RtMidi
- `ARCHITECTURE.md` — 6-layer MVC, atomic swapper, real-time rendering
- `PATTERNS.md` — Lock-free queues, facades, atomic model swap, concurrent queues
- `CONCERNS.md` — 52 TODOs, god classes, assert misuse, global singletons

### Git State

Last commit: d68b075 (fix(09-01): clamp spectrum height so it never overlaps the keyboard)
Branch: main
Uncommitted: .paul/ROADMAP.md, .paul/STATE.md, .paul/paul.json, .paul/PROJECT.md, .paul/phases/09-ui-enhancements/09-01-SUMMARY.md, deleted build_standalone.bat, untracked build_kb.{bat,log}, build_vst3.{bat,log}, Absorb/, giada/, resources/Loop-Specs.md, resources/Screenshot 2026-06-03 125956.png

## Session Continuity

Last session: 2026-06-04
Stopped at: Milestone v0.2 Interactive UI complete and archived
Next action: /paul:discuss-milestone or /paul:milestone to define v0.3
Resume file: .paul/MILESTONES.md
Resume context:
- v0.2 shipped: visual keyboard, UI polish, ADSR UX polish
- 3 phases, 3 plans, 10 unique files changed
- 1 deprecation warning (juce::Font::getStringWidth) — deferred
- Source/PluginEditor.cpp has layout fix for keyboard-vs-tabs
- All .paul/* and Source/* changes are uncommitted (awaiting user)

---
*STATE.md — Updated after every significant action*
