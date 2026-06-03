# Milestones

Completed milestone log for this project.

| Milestone | Completed | Duration | Stats |
|-----------|-----------|----------|-------|
| v0.1 Initial Release | 2026-05-28 | 4 days | 6 phases, 14 plans, 10 files |

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
