# Milestones

Completed milestone log for this project.

| Milestone | Completed | Duration | Stats |
|-----------|-----------|----------|-------|
| v0.1 Initial Release | 2026-05-28 | 4 days | 6 phases, 14 plans, 10 files |
| v0.2 Interactive UI | 2026-06-04 | 1 day | 3 phases, 3 plans, 10 files |

---

## v0.1 Initial Release (v0.1.0)

**Completed:** 2026-05-28
**Duration:** 4 days (2026-05-24 to 2026-05-28)

### Stats

| Metric | Value |
|--------|-------|
| Phases | 6 |
| Plans | 14 |
| Files changed | 10+ |
| Build status | Clean (zero errors, zero warnings) |

### Key Accomplishments

- **Project scaffolding:** JUCE VST3 + Standalone plugin with CMake + Ninja build pipeline, passing pluginval validation
- **Sample loading:** WAV/MP3/AIFF via file browser and drag-drop with cyan border visual feedback, base64-encoded state persistence
- **Polyphonic playback:** 16-voice Synthesiser with pitch-shifting, cubic interpolation, SpinLock thread-safe sound changes
- **ADSR envelopes:** Three envelopes (Volume, Filter, Pitch) with StateVariableTPTFilter low-pass, VCF→VCA signal chain
- **UI overhaul:** Ableton-inspired dark UI with tab-based layout, graphic ADSR displays replacing knobs, draggable filter response curve, real-time FFT spectrum analyzer
- **Loop playback:** Forward loop with cosine crossfade, normalised loop start/end points, waveform visual markers, APVTS-wired controls in SAMPLE tab
- **Preset system:** XML-based save/load/delete with factory "Init" preset, PresetManager class
- **Build pipeline:** Per-user VST3 install path, standalone .exe, build scripts for Windows

### Key Decisions

| Decision | Rationale | Date | Status |
|----------|-----------|------|--------|
| Classic Mode only | Focused scope — most versatile mode | 2026-05-24 | Active |
| 16 voices for polyphony | Sufficient for typical use | 2026-05-24 | Active |
| StateVariableTPTFilter | Per-sample cutoff modulation without artifacts | 2026-05-24 | Active |
| ADSR knobs removed entirely | User wants graphic display interaction only | 2026-05-28 | Active |
| Cubic interpolation for pitch-shifting | Replace linear; smoother pitch shifts | 2026-05-28 | Validated |
| Cosine equal-power crossfade at loop boundary | Smooth transition without clicks | 2026-05-28 | Validated |
| Preset system via XML files | Portable, no external dependencies | 2026-05-28 | Validated |

### Deferred to v0.2 / v1.1

- QWERTY keyboard mapping for standalone mode
- Exponential filter cutoff mapping (currently linear Hz)
- PITCH_ENV_SEMITONES parameterization (currently hardcoded at 12)
- Multi-mode filter (HP/BP modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice playback modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern)

---

## v0.2 Interactive UI (v0.2.0)

**Completed:** 2026-06-04
**Duration:** 1 day (2026-06-03 to 2026-06-04)

### Stats

| Metric | Value |
|--------|-------|
| Phases | 3 |
| Plans | 3 |
| Files changed | 10 unique |
| Build status | Clean (zero errors; 1 deprecation warning: juce::Font::getStringWidth — deferred) |

### Key Accomplishments

- **Visual keyboard (Phase 8):** Custom-painted 2-octave piano (C3–B4) at the bottom of the editor that highlights keys in real time on MIDI note-on/off, with its own 30 Hz internal Timer (decoupled from the editor's 10 Hz metadata timer for lower visual latency)
- **UI polish (Phase 9):** Flat non-rounded TextButtons, 30%-larger 34 px tab-alpha knob centred in the 28 px title bar, preset ComboBox + Save + Delete relocated to the title bar (always visible), spectrum height clamp preventing keyboard overlap
- **ADSR UX (Phase 10):** Enlarged drag handles (5 px rest, 7 px hover/drag), per-node tooltips via `juce::TooltipClient` ("Attack" / "Decay / Sustain" / "Release"), live numeric readouts ("Atk 0.10 s", "Dec 0.50 s / Sus 0.65", "Rel 1.20 s") drawn in `paint()` while dragging, plus a layout fix so the visual keyboard no longer hides tab content

### Key Decisions

| Decision | Rationale | Date | Status |
|----------|-----------|------|--------|
| Display-only Component for keyboard (no LnF) | Self-contained, internal 30 Hz repaint, queries MidiKeyboardState directly | 2026-06-03 | Validated |
| `isNoteOnForChannels(0xffff, note)` over non-existent `isNoteOn` | Canonical MidiKeyboardState API; channel mask 0xffff = any channel | 2026-06-03 | Validated |
| Editor height 440 → 480 px for 50 px keyboard | Spectrum shrinks by 54 px; ADSR/filter use proportional sizing | 2026-06-03 | Validated |
| Flat (non-rounded) TextButton rendering with 2-px vertical inset | "Thinner" appearance without restructuring row layout | 2026-06-03 | Validated |
| Preset controls in title bar, not SAMPLE tab | Always accessible; reduces per-tab UI complexity | 2026-06-03 | Validated |
| Spectrum height clamp via `jmax(50, content-50-4)` | Robust against future resizable window; prevents zero/negative spectrum height | 2026-06-03 | Validated |
| ADSR tooltips via `juce::TooltipClient` (not `Component::getTooltip`) | Component has no virtual `getTooltip`; `TooltipWindow` dispatches to `TooltipClient*` | 2026-06-04 | Validated |
| Tab content positioned in `contentMinusKeyboard` strip | Visual keyboard at bottom 50 px otherwise occludes lower portion of ADSR/filter displays | 2026-06-04 | Validated |

### Deferred to v0.3+

- QWERTY keyboard mapping for standalone mode
- Exponential filter cutoff mapping (currently linear Hz)
- PITCH_ENV_SEMITONES parameterization (currently hardcoded at 12)
- `juce::Font::getStringWidth` deprecation in `AdsrDisplayComponent.cpp` — migrate to `GlyphArrangement` / `TextLayout`
- Tab z-order refinements and broader visual feedback (originally listed under Phase 10 ROADMAP description, but redirected to ADSR polish at planning)
- Multi-mode filter (HP/BP modes) — v1.1
- LFO modulation — v1.1
- One-Shot / Slice playback modes — v1.1
- Lock-free queue for UI→audio communication (giada pattern) — v0.3
- pluginval strictness high test for VST3 (pluginval not in PATH; recommend running before v0.2 ships)

---
