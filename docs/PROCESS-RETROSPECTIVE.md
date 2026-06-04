# Samplexpress — Development Process Retrospective

**Date:** 2026-06-04
**Scope:** v0.1 Initial Release + v0.2 Interactive UI (2026-05-24 → 2026-06-04)
**Purpose:** Document how the PAUL Framework was used to develop this VST3 plugin — the parts that helped, the parts that needed human steering, and the test methodology that actually ran on this machine.

This is a project-internal retrospective. It cites real artifacts (commit hashes, plan IDs, file paths, decision-table rows) rather than generalising about PAUL in the abstract. If a claim below cannot be backed by an artifact in the repo, it is labelled as a claim about intent rather than a fact.

---

## 1. How the PAUL Framework helped (and where it didn't)

### 1.1 What PAUL actually gave us

The PAUL framework installed under `.claude/paul-framework/` provides a PLAN → APPLY → UNIFY loop, phase directories under `.paul/phases/NN-name/`, a `ROADMAP.md` that knows about milestones, and structured frontmatter templates for `PLAN.md` and `SUMMARY.md` (the YAML at the top of each artifact).

In this project, that translated into five concrete habits:

| Habit | PAUL mechanism that enforced it | Concrete example in this repo |
|---|---|---|
| Atomic task commits | `SUMMARY.md` frontmatter's "Task Commits" table | `08-01` has Task 1 → `852a3da`, Task 2 → `c91cca7` (visible in `git log`) |
| Acceptance criteria before declaring done | `SUMMARY.md` AC table with explicit Pass/Fail | `09-01` AC-3 verified by human at checkpoint ("approved") |
| Decisions recorded with rationale | `SUMMARY.md` "Decisions Made" table | `04-02` records the atomic-flag fix for the Play button race |
| Deviations caught, not hidden | `SUMMARY.md` "Deviations from Plan" with `auto-fixed / scope-additions / deferred` buckets | `10-01` flags `juce::Font::getStringWidth` deprecation as Deferred rather than silently swallowing the warning |
| A `STATE.md` that survives session breaks | `STATE.md` "Session Continuity" block | `STATE.md:79-87` recorded exactly what to pick up after v0.2 closed |

### 1.2 What PAUL did *not* do

PAUL is a process shell, not a code generator. Three things it did not give us, and that we had to do ourselves:

- **Did not pick APIs.** Every PAUL plan that touched a JUCE class needed verification against the local JUCE source. The `08-01` plan asked for `keyboardState.isNoteOn(midiNote)`; that method does not exist in `juce::MidiKeyboardState`. The APPLY step caught it by reading the JUCE header and switched to `isNoteOnForChannels(0xffff, midiNote)`. Same story for `10-01`'s `Component::getTooltip()` (no such virtual) — switched to the `juce::TooltipClient` interface.
- **Did not invent the milestone scope.** The user described v0.2 in plain prose ("visual keyboard below tabs, ADSR drag handles too small, button style is too rounded") and PAUL's job was to turn that into a 3-phase roadmap, not to decide what to build. PROJECT.md § "Key Decisions" is the only place where *user intent* is preserved at decision granularity.
- **Did not run tests autonomously.** The `Smoke Build Gate` and "Continuous Validation Standard" sections of `CLAUDE.md` describe a CI pipeline. None of that pipeline exists on this Windows host (see § 4). PAUL tracked the absence (`pluginval strictness high` is marked "not run" in `08-01` and `09-01` verification checklists) but did not conjure `pluginval` into `$PATH`.

### 1.3 Where the loop added real value

- **Plan correctness vs. APPLY fidelity is auditable.** `05-qwerty-keyboard-ui-integration/PHASE5-AUDIT.md` is a hand-written audit that walks every plan→code check (filter SVF formula, lock-free ring buffer memory ordering, FFT out-of-place fix) and produces a verdict. That audit only exists because the loop produces structured artifacts you can audit against. Without YAML frontmatter, there is nothing to diff.
- **Scope changes don't become invisible.** When the user redirected Phase 9 from "waveform drag" to "UI polish" at 09-01 planning, the redirect is recorded in `09-01-SUMMARY.md` under "Deferred Items" and in `STATE.md` "Decisions" (`Phase 09 scope redirected from 'waveform drag' to 'UI polish'`). Phase 10's scope adoption (ADSR UX polish) is similarly captured. Six months from now, the chain "user said X → plan said Y → we built Z" is recoverable.
- **Cross-phase audits happen.** The 05 audit, the 09 redirect, and the 10 layout fix all came from reading prior plans against current code. That is cheap when artifacts are structured (5 minutes) and expensive when they are not (an afternoon of archaeology).

---

## 2. How the human described features

The user never wrote a single spec document. Every feature in this project was described in chat, in fragments, and often after seeing the previous build. Below is a representative sample, drawn from the SUMMARY files' "Decisions" and "Scope Additions" sections.

### 2.1 Style of description

The descriptions are short, outcome-oriented, and reference the visible artifact, not the implementation. Examples from `04-02-SUMMARY.md` and `09-01-SUMMARY.md`:

- *"the plugin had no way to audition the loaded sample"* — describes the missing affordance, not the API to add.
- *"the painted surface should be visibly thinner"* — describes a visual outcome, leaves the implementation (2-px vertical inset) to the planner.
- *"the keyboard is hidden by the ADSR controls"* — describes a regression in the editor, not a layout diff to apply.
- *"ADSR drag handles are too small to grab"* — describes a discoverability problem, not a `setRadius()` call.

This style worked *because* PAUL's planning step forces the planner to translate prose into acceptance criteria. The `09-01` plan took "thinner buttons" and produced `AC-1: Flat, Thinner Buttons` with a measurable interpretation (22-px painted surface inside a 26-px row). The plan is the translation layer; the user never had to write that.

### 2.2 Mid-flight scope changes

The user's strongest pattern was redirecting scope *during* a phase, not before it. Two examples:

| Phase | Original ROADMAP description | User redirect | Recorded in |
|---|---|---|---|
| 09 | "Waveform Interaction: click/drag on waveform to set loop start/end points; optional playhead cursor" | "polish the UI: flat buttons, bigger alpha knob, move preset controls to the title bar" | `09-01-SUMMARY.md` "Deferred Items" + `STATE.md` "Decisions" |
| 10 | "Exponential filter cutoff mapping, tab z-order refinements, broader visual feedback" | "ADSR drag handles are too small, give them tooltips and live readouts" | `10-01-SUMMARY.md` header note + `STATE.md` "Decisions" (`Phase 10 scope adopted: ADSR drag-handle UX polish`) |

The PAUL loop absorbs these redirects cleanly: the SUMMARY's "Deviations" table is exactly the right place to record "we built something different than the ROADMAP promised", and the ROADMAP archive is updated at milestone close (`v0.2.0-ROADMAP.md` notes both redirects).

### 2.3 Where the user's description was ambiguous

Two cases where the prose was unclear and the planner had to ask or guess:

- **"polyphonic"** in Phase 3's user description turned into 16 voices (`03-01-SUMMARY.md` decision: "16 voices for polyphony"). The user's intent was probably "enough to play chords"; 16 was a guess that stuck.
- **"resonance rings through the release"** in Phase 4 (a subtle mixing-engineering intent) was translated to "VCF before VCA signal chain" (`04-02-SUMMARY.md`). The chain ordering is the implementation, but the audible result is what the user asked for.

The plan → AC → deviation pipeline catches the cases where the guess was wrong (e.g. the `04-02` Play-button thread-safety fix was triggered by the user reporting "the play button doesn't make sound" — i.e. the implementation diverged from intent and the user noticed).

---

## 3. How PAUL addressed the coding task

### 3.1 The PLAN step in practice

A PLAN.md in this project has a fixed shape: YAML frontmatter (requires / provides / key-decisions / patterns-established), then a `Tasks` checklist, then an `Acceptance Criteria` table. The plan is *executable* — every step names the file and the method to touch.

Concrete example, from `08-01-PLAN.md` (paraphrased):

```
Task 1: VisualKeyboardComponent
  - File: Source/VisualKeyboardComponent.h
    Add: Component subclass with paint() + resized() + timerCallback()
    Constants: white key color 0xFFFFFFFF, black key 0xFF202020, highlight 0xFFFFAA33
  - File: Source/VisualKeyboardComponent.cpp
    Layout: 14 white keys (C3..B4) at width = bounds.w/14
    paint(): draw white rects, overlay black rects at index 1,3,6,8,10
    Query: keyboardState.isNoteOnForChannels(0xffff, midiNote) for highlight
  Acceptance: AC-1..AC-4 in plan header
```

The APPLY step then executes that plan and reports back. Deviations get a `SUMMARY.md` entry. In `08-01` the APPLY agent discovered the API name was wrong (`isNoteOn` does not exist) and corrected it before writing the code; the SUMMARY records the correction as "Auto-fixed Issue #1" with file:line citations.

### 3.2 The "I just read the JUCE source" step

Three concrete cases where the APPLY agent had to consult the local JUCE source rather than guess:

| Plan said | Local JUCE source said | What we did |
|---|---|---|
| `keyboardState.isNoteOn(midiNote)` | No such method; canonical is `isNoteOnForChannels(channelMask, midiNote)` | Used `isNoteOnForChannels(0xffff, midiNote)` (`08-01-SUMMARY.md` Auto-fixed #1) |
| `Component::getTooltip()` is virtual and overridable | `Component` has no `getTooltip()` virtual; the dispatch path is `TooltipWindow::getTipFor` → `dynamic_cast<TooltipClient*>(component)->getTooltip()` | Made `AdsrDisplayComponent` inherit `juce::TooltipClient` (`10-01-SUMMARY.md` Auto-fixed #1) |
| `AlertWindow::showTextInputDialogAsync` | Method does not exist in JUCE 8 | Used `enterModalState` with a `TextEditor` child (`06-preset-system-audio-engine/06-01-SUMMARY.md` Deviations) |

This is the value the `CLAUDE.md` "Context7 MCP first" rule added: every plan touching a JUCE API was re-checked against the actual installed library, not memory. The three corrections above all came from `grep` against `C:\Users\ideiudicibus\JUCE\modules\`.

### 3.3 Patterns that propagated across phases

A pattern established in one plan appears as `patterns-established:` in the next plan's frontmatter. Real chain in this project:

1. **03-01** established `SpinLock` for sound changes (UI thread modifies `synth.clearSounds/addSound`, audio thread reads via voice rendering).
2. **04-01** established `SliderAttachment` for APVTS ↔ slider sync, plus `setSkewForCentre` for ADSR feel.
3. **05-01** established the "invisible slider bridge" — a visible graphic component that drives an `invisible` slider (alpha 0) to keep APVTS attachment intact.
4. **05-02** reused the invisible-slider bridge for spectrum-analyzer controls.
5. **06-01** reused `getStateInformation`/`setStateInformation` for preset serialization.
6. **09-01** added the "flat inset" pattern (`fillRect`/`drawRect` with `reduced(0.5f, 2.0f)`) to the LnF.

The next plan that needed a slider-with-custom-visual picks up the invisible-slider bridge by reading prior frontmatter, not by reinventing it.

### 3.4 Where the plan-vs-actual diverged

Honest count from the SUMMARY files (v0.1 + v0.2):

| Deviation type | Count | Example |
|---|---|---|
| Auto-fixed (essential API / build correction) | 9 | `isBusesLayoutSupported` (Phase 1), `TooltipClient` (Phase 10), `appliesToVelocity` omitted (Phase 3) |
| Scope addition (user requested extra work) | 3 | Filter enabled (Phase 4), Play button (Phase 4), per-user VST3 path (Phase 4) |
| Pre-implemented (work landed before plan finalised) | 2 | Loop parameters + crossfade in `06-01` |
| Deferred | 5+ | QWERTY keyboard, exponential filter cutoff, PITCH_ENV_SEMITONES param, `getStringWidth` migration, pluginval high test |

Auto-fixes dominated. That tells us the planning step is good at scoping but the APPLY step still has to verify every named API against reality. We have not yet seen a plan-time API check; that is a known limitation.

---

## 4. Test methodology

This section describes what was *actually* run, not what `CLAUDE.md` says should run. The `CLAUDE.md` standard (pluginval strictness high, offline audio render, real-time CPU benchmark, UI screenshot, GUI overlap test) is the target. The achieved test surface on this Windows host is much smaller.

### 4.1 What ran on every plan

**Build:** `cmake --build build --config Debug --target {Samplexpress_VST3|Samplexpress_Standalone}` via `build_vst3.bat` / `build_kb.bat`. Both batch files source `vcvarsall.bat` from VS Build Tools 2026, set `JUCE_FRAMEWORK=C:\Users\ideiudicibus\JUCE`, then run the CMake build. The build scripts emit a `build_*.log` artifact per run.

**Build success criterion:** "zero errors, zero warnings" was the explicit goal in every plan's verification checklist. It was met in every v0.2 plan (with the documented `juce::Font::getStringWidth` deprecation as a known exception in `10-01`).

**Standalone smoke:** launch `build/Samplexpress_artefacts/Debug/Standalone/Samplexpress.exe`, confirm the window appears. If a plan added visible behaviour (drag-drop border, filter curve, spectrum bars, ADSR readouts), the human-verify checkpoint was the visual confirmation. The `08-01` SUMMARY records the user message *"the keyboard is visible"* as the AC-1 verification.

**VST3 deploy:** copy the built `.vst3` to `%LOCALAPPDATA%\Programs\Common\VST3` (per-user, no admin). Done in `04-02` onward; `full_build.bat` / `rebuild_all.bat` updated to use the per-user path.

### 4.2 What did *not* run

| CLAUDE.md test | Status | Evidence |
|---|---|---|
| `pluginval --strictness high --validate-in-process` | Not run on this host | `08-01-SUMMARY.md` verification: `[ ]`; `09-01-SUMMARY.md` same; reason: `pluginval not in PATH` |
| Offline audio render regression (Python/NumPy) | Not implemented | No test file in repo; no `pytest.ini`; no NumPy harness |
| Real-time CPU & glitch benchmark | Not implemented | No harness; only subjective observation that 30 Hz repaint + 30 Hz repaint + 10 Hz repaint felt "negligible" (`08-01-SUMMARY.md` "Concerns") |
| `juce_pluginhost --headless --capture-ui` | Not implemented | No such command on host; no PNG artifacts produced |
| `tests/gui_layout_test.cpp` | Not implemented | No `tests/` directory in repo |
| `z_order_test.cpp` | Not implemented | Same |

`PHASE5-AUDIT.md` flags this gap explicitly:

> **No GUI unit tests** for component bounds overlap or z-order (CLAUDE.md requires these, but none exist in the repo)
> **No automated screenshot sanity check** (CLAUDE.md requirement: `juce_pluginhost --headless --capture-ui`)
> **No offline audio render test** (CLAUDE.md requirement: pump known WAV, verify no NaNs)

### 4.3 Where the test surface came from

The `09-01` and `10-01` plans were verified by **visual checkpoint** — the human launched the standalone, looked at the window, and approved. That works for layout work but does not catch:
- Audible regression (no DI rendering, no FFT-of-output diff)
- Threading regression (no stress test; `MidiKeyboardState` read-from-`paint()` is only annotated as "worth a future stress test", `08-01-SUMMARY.md` "Concerns")
- Memory regression (no leak check, no `valgrind` equivalent on Windows)

### 4.4 What we did to compensate

- **PLAN-level code review** by reading the SUMMARY against the diff. The Phase 5 audit is the formal instance; informal review happens at every `/paul:unify` step.
- **Cross-check API names against local JUCE** in every plan (see § 3.2). Catches a class of bugs that would otherwise reach the build step.
- **Lock-free ring buffer memory ordering** was hand-checked (`05-02-SUMMARY.md`: "lock-free ring buffer uses correct memory ordering (`relaxed` write, `release` store, `acquire` read)" — confirmed by reading the `.cpp`).
- **Build hygiene** — zero-warnings gate caught things like the `04-01` ADSR skew factor that was mathematically backwards, and the `06-01` `setupLoopSlider` unused-parameter warning that was suppressed with `/*name*/`.

### 4.5 Honest assessment

The achieved test methodology is: build clean → launch standalone → human looks at it. That is enough to ship a UI polish release where the visible behaviour is the contract. It is not enough to claim an audio regression test suite, and it is not enough to validate a release-tag build (the CI pipeline in `CLAUDE.md` does not exist locally — `v*` tag builds would need to be done on a machine with `pluginval` and a host DAW installed).

The deferred `pluginval strictness high` run should happen *before* the next tag is cut, and the offline-audio + GUI-layout test files are real technical debt — they were named in `CLAUDE.md` from day one and have not been written.

---

## 5. What I'd do differently next milestone

Concrete, in priority order:

1. **Add a `tests/` directory and one offline-audio render test.** A 2-second pink-noise render through the plugin, FFT'd, diffed against a stored baseline, with a tolerance for the float jitter. This is the highest-leverage missing test and the easiest one to write.
2. **Run `pluginval` once before the next tag.** Even if we don't wire it into CI, getting a single `pluginval --strictness high` pass on `main` proves the VST3 still loads cleanly in a host. Right now we have "build succeeds" but no "host loads it" guarantee since Phase 4.
3. **Plan-time API verification step.** Every plan that names a JUCE method should grep `C:\Users\ideiudicibus\JUCE\modules\` for the exact signature before writing code. The 8 auto-fixes we've accumulated would largely have been caught by this. It's a 5-minute prompt addition to the planner.
4. **Decide on resizable editor before adding the next major visual component.** The 50-px spectrum floor in `09-01` is a dormant fix waiting for a real resize callback. The next plan that adds a side panel or modal should commit to resizability at the same time, or we'll have a third layer of clamp-based layout hacks.
5. **Migrate `getStringWidth` off the deprecation path.** `10-01` flagged it; it's still in the codebase. Small change, removes a warning on every build.

---

## 6. Evidence index

For anyone auditing this retrospective, the artifacts are:

- **Phase plans & summaries:** `.paul/phases/*/[NN]-{PLAN,SUMMARY}.md` (15 plan/summary pairs across v0.1 + v0.2)
- **Milestone archives:** `.paul/milestones/v0.{1,2}.0-ROADMAP.md`
- **Decision log:** `.paul/PROJECT.md` "Key Decisions" table (33 rows, dated)
- **State continuity:** `.paul/STATE.md` "Accumulated Context" and "Session Continuity"
- **Cross-phase audit:** `.paul/phases/05-qwerty-keyboard-ui-integration/PHASE5-AUDIT.md`
- **Source of intent:** `.paul/PROJECT.md` "Active (In Progress)" and "Planned (Next)" lists
- **Build scripts:** `build_vst3.bat`, `build_kb.bat`
- **Test standard (not yet met):** `CLAUDE.md` "Continuous Validation Standard" and "Smoke Build Gate" sections
- **Reference codebase used during planning:** `giada/.paul/codebase/{ARCHITECTURE,PATTERNS,CONCERNS,TECH-STACK}.md`

---

*Written: 2026-06-04 — covers the full 2026-05-24 → 2026-06-04 development window. No code changes were made by writing this document; it is a retrospective on completed work.*
