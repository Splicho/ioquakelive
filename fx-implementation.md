# WolfcamQL `.fx` Scripting Port - Implementation Notes

Last updated: 2026-11-21
Repository: `C:\Users\stupi\Documents\Repos\ioquakelive`
Reference tree: `C:\Users\stupi\Documents\Repos\wolfcamql-master`

This document describes what was ported, where it lives, what is verified, what is broken, and what a follow-up agent should do next. It intentionally documents approximations and failures. Do not assume "it compiles" means "it works."

---

## 0. Post-handoff update (latest)

A later round of changes (by ChatGPT, then reviewed/staged here) supersedes parts of sections 1, 9, and 10 below:

- The full FX local-entity lifecycle was ported:
  - nested `distance` / `impact` evaluation
  - motion / gravity / bounce / trace handling
  - model axes and lifetime
  - light handling
  - fade/scale updates
- The JIT/token cache was replaced:
  - the huge offset-indexed `jitToken[...]` array is gone
  - a bounded 4096-bucket hash table is used
  - token strings are owned by the cache
  - `CG_FreeFxJitTokens()` walks bucket chains
- Regression tests were added under `tests/`:
  - rail, fade, motion, nested distance, impact, model axes, lifetime, light
  - both interpreted and `--compiled` cache paths pass
  - reported CPU check: roughly 23.7 ms/update interpreted vs 7.3 ms/update compiled for 10,240 rings (mock renderer)
- Rail and gib rendering issues were reported fixed after these changes.
- Latest staged cgame: **1,483,776 bytes**. Verify staging again before testing.
- FX logging was reduced:
  - `cg_fxDebug 0` is the default
  - per-submitted-ref-entity logging is now level **4**, not 3
  - `fxdebug.log` has a hard **8 MB** cap
  - the previous 190 MB debug log was deleted
- Set `cg_fxDebug 0` for normal play. Level 3 is diagnostic; level 4 is extremely verbose.

If the reader needs the original failure analysis, sections 9-10 are retained below as historical context.

---

## Current repair status (2026-11-21)

This section supersedes the historical notes below, including the old DLL-size
expectations and the list of broken emitter features. Visual confirmation in-game
is still required; no game or server was launched during this repair.

### Fixed and checked

- Rail hooks now populate `parentOrigin`, `parentEnd`, `parentDir`, shooter data,
  and rail colors. Madir's helix previously used a zero parent origin.
- Replaced the approximate FX update branch with Wolfcam's emitted-entity
  lifecycle, adapted for this cgame: per-entity snapshot restoration, lifetime
  interpolation, movement, bounce/impact state, distance cursors, sink behavior,
  color/alpha fade, size, rotation, direction, and light/sound/script dispatch.
- Creation snapshots remain intact. Per-frame offsets/fades no longer compound.
  The old disabled field-copy blocks were obsolete: the existing `memcpy` already
  preserved all script fields; missing copies were not the cause.
- Distance and impact children preserve their parent's model/size/color/velocity.
  Distance emitters preserve fractional spacing between updates. Child emitters
  begin at their own `lerp = 0`.
- Movement commands after a rendering command now initialize the emitter too.
  This fixes Madir's lightning `Sprite` followed by `moveBounce`.
- Gravity remains a time-based trajectory between bounces. Collision reflection
  uses the falling velocity rather than the original launch velocity; particles
  dropped from rest now rebound upward.
- Model direction/angle/axis mode is retained in cgame and its axes/scaling are
  rebuilt for animated script values. `emitterf` gets its final update.
- Increased the entity budget to Wolfcam's 14-bit capacity (16,383 renderer
  entries, 16,380 local entities). All three renderers now use 64-bit draw/lit
  sorting keys, including sorting, batching, and shader-index fixups. This costs
  additional memory but prevents a single dense long rail from evicting its core
  or being truncated at the former 512/1,023/4,095 limits.
- Allocation during emitter execution cannot evict its active parent or corrupt
  the local-entity traversal cursor when the pool fills.
- Rail-ring geometry now initializes vertex alpha in OpenGL1, OpenGL2 and Vulkan.
- FX loop-sound allocation resets each frame. Added missing compatibility
  prototypes, including the pointer-returning `String_Alloc` declaration.
- Removed unconditional per-entity per-frame debug writes from the old loop.
  FX file reads now terminate cleanly at EOF even if the filesystem leaves the
  destination byte unchanged.

### Verification

- Windows x64 Release solution build with VS 2022 / v143: passed. Existing compiler
  warnings and optional Vorbis DLL copy notices remain; compilation/linking passed.
- `git diff --check`: passed.
- `tests/fx_regression.c` links the actual cgame objects with mocked engine
  syscalls. It does not initialize an engine, map, renderer, Steam, or server.
  It checks rail parent coordinates, non-compounding offsets/fades, gravity,
  bounce, distance blood emission, impact marks/sounds, model axes/scale,
  emitter expiry, light-state isolation, commands after `sprite`, and pool
  exhaustion while an emitter is updating.
- The same harness loaded the user's existing local `madir.fx`: all four effects
  parsed; an 8,192-unit rail retained and submitted its beam plus all 10,240 rings
  at the shot coordinates; gibs created 11 models and emitted blood sprites.
- `python tests/renderer_sort_regression.py` compiles the actual radix sort and
  rail-disc functions from each renderer against minimal test data. All three
  pass high-bit sorting, stable equal-key ordering, ring geometry and vertex alpha.
- Fresh engine executables, renderer DLLs, cgame/game/UI DLLs and `iobin.pk3` are
  staged in `build/ioquakelive-windows-x64/`, with SHA-256 verification against
  `build/Release/x64/`. Optional FX asset packs and the user's script are untouched.

### Runtime follow-up and limits

Restart the staged client, keep `cg_fxCompiled 0`, and run
`fxload scripts/madir.fx`. Check rail origin/length/fade, gib movement and blood,
wall marks, and lightning particles. Automated tests validate simulation and
renderer inputs; they do not establish visual parity with Wolfcam.

JIT behavior, missing player/reward hooks, external-force support, and a standalone
`death` command remain outside this repair. The historical note suggesting a full
Wolfcam death-command port should not be read as an implemented feature. Very
large or overlapping scripts can still exhaust the finite entity budget.

The repository remains uncommitted. No proprietary assets were added or bundled.

---

## Historical implementation notes (before the repair)

## 1. Current status

### Working / verified by logs

- `.fx` files load at runtime with:
  - `fxload [path]`
  - `fxlist`
  - `fxlog`
  - `clearfx`
  - `fxmath`
  - `runfx <name>`
- `scripts/madir.fx` parses successfully:
  - 4 named effects: `player/gibbed`, `weapon/rail/trail`, `weapon/lightning/impact`, `player/quad`
  - `gibbed=1`
  - `railTrail=1`
- Rail hook runs and creates FX local entities.
- q3mme renderer entity types are being submitted (`type=10` beam, `type=11` rail rings).
- Gib model entities are created (`model=116..125`) and now pass the submission check.
- `pak-fx-assets.pk3` supplies:
  - `models/gibsq3/*.md3` aliases
  - `q3gibs`, `q3bloodMark`, `q3bloodTrail` shaders
  - `sound/player/gibsplt1.wav`, `gibimp1/2/3.wav`
- Debug logging goes to `fxdebug.log` via `FS_APPEND_SYNC`.

### Still broken / incomplete

- **Rail visual is still wrong** compared with Wolfcam/q3mme. It is no longer invisible, but the shape/rings/beam do not match the `.fx` script.
- **Gibs may now render but movement/blood are incomplete.** The physics helper was added, but it was only just staged; verify it. Blood/distance emitters are not ported.
- **Local-entity emitter lifecycle is still a simplified approximation.** This is the primary root cause of both the rail mismatch and incomplete gibs.
- JIT compiler (`cg_fxCompiled 1`) is untested.
- Several player-specific script slots are not hooked.
- `RT_MODEL_FX_*` are still aliases to `RT_MODEL` (axis is computed in cgame). This is probably acceptable but not a literal renderer port.
- OpenGL2/Vulkan q3mme surface code compiles, but visual verification is missing.

---

## 2. Critical staging gotcha (this wasted a lot of time)

There are two build trees:

- Build output: `build\Release\x64\`
- Staged game: `build\ioquakelive-windows-x64\`

The staged cgame can be **stale** even after a successful build. At one point:

- `build\Release\x64\baseq3\cgamex86_64.dll` = `1,479,168` bytes (new)
- `build\ioquakelive-windows-x64\baseq3\cgamex86_64.dll` = `1,477,632` bytes (old)

That is why "nothing changed" after adding physics and q3mme renderer types: the game was loading the old cgame.

**Always verify after staging:**

```powershell
Get-Item `
  build\Release\x64\baseq3\cgamex86_64.dll, `
  build\ioquakelive-windows-x64\baseq3\cgamex86_64.dll |
  Select-Object FullName,Length,LastWriteTime
```

Current expected cgame size after the physics build: **1,479,168 bytes**.

If the game is running, Windows locks the DLL. Do not kill the game. Either ask the user to close it, or stage as `cgamex86_64.dll.new` and move it after the process exits.

Renderer DLLs must be staged too:

- `opengl1x86_64.dll`
- `opengl2x86_64.dll`
- `vulkanx86_64.dll`
- `ioquakelive.x86_64.exe` / `ioquakelive-vulkan.x86_64.exe` when engine code changes

---

## 3. Build

Windows x64 Release with VS 2022:

```powershell
msbuild misc\msvc\ioquakelive.sln `
  /p:Configuration=Release `
  /p:Platform=x64 `
  /p:PlatformToolset=v143 `
  /m
```

Build logs are under `build\logs\`.

The cgame project is:

`misc\msvc\cgame.vcxproj`

New files were added to both MSVC and `Makefile`.

---

## 4. Architecture

### Load path

```text
fxload [file]
  -> CG_FXLoad_f
  -> CG_ReloadQ3mmeScripts(file)
     -> CG_RemoveFXLocalEntities(qtrue, 0)
     -> CG_FreeFxJitTokens()
     -> memset(EffectScripts)
     -> CG_ResetFXScripting()
     -> CG_ParseQ3mmeScripts(file)
     -> log summary (fxdebug.log)
```

`cg_fxfile` is a cvar. `CG_Init` calls:

```c
CG_FX_RegisterCommands();
CG_ReloadQ3mmeScripts(cg_fxfile.string);
```

### Execution path

```text
game event (rail / gib / impact / teleport / ...)
  -> CG_FX_RunScript / CG_FX_RunImpactScript / CG_FX_RunRailScript
     -> CG_ResetScriptVars()
     -> populate ScriptVars (origin, dir, velocity, cent data, ...)
     -> CG_RunQ3mmeScript(script, NULL)
        -> interpreter tokens
        -> CG_FX_* helpers
        -> CG_AllocFxEntity / CG_AllocLocalEntity
  -> CG_AddLocalEntities()
     -> FX local entity update / render
```

### Script data

`EffectScripts` is a large static struct in `cg_fx_scripts.c`:

- `gibbed`, `thawed`, `jumpPad`, `headShot`, `bubbles`
- `playerTeleportIn/Out`
- `playerTalk`, `playerConnection`, `playerFlight`, `playerHaste`, `playerQuad`, `playerFriend`
- player medal scripts
- player head/torso/legs trails
- `weapons[WP_*]` with:
  - fire
  - flash
  - projectile
  - trail
  - impact
  - impactFlesh
- JIT token pointers

Loading summary in `fxdebug.log` is the fastest way to confirm what the parser actually assigned.

---

## 5. File map

### New cgame files

| File | Role |
|---|---|
| `code/cgame/cg_fx_scripts.c` | Wolfcam interpreter/parser, script vars, FX helpers, local entity creation, JIT scaffolding. |
| `code/cgame/cg_fx_scripts.h` | `ScriptVars_t`, `positionData_t`, `leFXType_t`, `effectScripts_t`, prototypes. |
| `code/cgame/cg_fx_compat.c` | Wolfcam compatibility adapters, FS log, run helpers, sound wrappers, low-priority stub, math helpers. |
| `code/cgame/cg_fx_compat.h` | Adapter prototypes, cvar externs, aliases. |
| `code/cgame/cg_fx_commands.c` | `fxload`, `fxlist`, `fxlog`, `clearfx`, `fxmath`, `runfx`, gib entry point. |
| `code/cgame/cg_mem.c` / `.h` | Native allocation wrappers used by the FX code. |

### Modified cgame files

| File | Changes |
|---|---|
| `cg_local.h` | Include guard, FX include, extended `localEntity_t`, `centity_t`, `cg_t`, globals, cvar declarations. |
| `cg_main.c` | `cg_fxDebug`, `cg_fxCompiled`, `cg_fxfile`, FX init/reload calls, shutdown close log. |
| `cg_event.c` | Hooks for gib, thaw, jump pad, teleport in/out, awards/headshot. |
| `cg_weapons.c` | Fire, muzzle flash, wall/flesh impacts, bullets, rail, lightning, grapple. |
| `cg_ents.c` | Projectile and missile trail scripts. |
| `cg_players.c` | Legs/torso/head trails, flight, haste, quad. |
| `cg_effects.c` | Water-bubble script. |
| `cg_localents.c` | FX local entity update/render, physics, debug logs, emitter loop. |
| `cg_consolecmds.c` | Fallback dispatch to `CG_FX_Command`. |

### Renderer / engine changes

| File | Changes |
|---|---|
| `code/renderercommon/tr_types.h` | Added `float width` to `refEntity_t`; added q3mme entity types. |
| `code/renderergl1/tr_surface.c` | q3mme surface functions. |
| `code/renderergl1/tr_main.c` | New entity types in `R_AddRefEntityToScene`. |
| `code/renderergl2/tr_surface.c` | q3mme surface functions. |
| `code/renderergl2/tr_main.c` | New entity types in `R_AddRefEntityToScene`. |
| `code/renderervk/tr_surface.c` | q3mme surface functions. |
| `code/renderervk/tr_main.c` | New entity types in `R_AddRefEntityToScene`. |
| `code/qcommon/q_math.c` / `q_shared.h` | `PointToPlane()`. |
| `code/qcommon/cvar_help.c` | Help entries for FX cvars. |

### Build files

- `misc/msvc/cgame.vcxproj`
- `Makefile`

### Runtime content (local build only, not for redistribution)

- `build/ioquakelive-windows-x64/baseq3/scripts/*.fx`
- `build/ioquakelive-windows-x64/baseq3/pak-fx.pk3`
- `build/ioquakelive-windows-x64/baseq3/pak-fx-assets.pk3`
- `build/ioquakelive-windows-x64/baseq3/sound/player/*.wav`
- `build/Release/x64/baseq3/` equivalents

`pak-fx-assets.pk3` contains Quake Live/`dlc_gibs` assets. **Do not commit or redistribute it.**

---

## 6. Renderer entity types

Added to `refEntityType_t` after `RT_PORTALSURFACE`:

```c
RT_SPRITE_FIXED,      /* 8 */
RT_SPARK,             /* 9 */
RT_BEAM_Q3MME,        /* 10 */
RT_RAIL_RINGS_Q3MME,  /* 11 */
```

Existing values remain:

```text
RT_MODEL 0
RT_POLY 1
RT_SPRITE 2
RT_BEAM 3
RT_RAIL_CORE 4
RT_RAIL_RINGS 5
RT_LIGHTNING 6
RT_PORTALSURFACE 7
```

Ported functions (all three renderers):

- `RB_SurfaceSpriteFixed`
- `RB_SurfaceSpark`
- `RB_SurfaceBeamQ3mme`
- `RB_SurfaceRailRingsQ3mme`

`DoRailDiscs` was extended with an explicit `spanWidth` argument:

- stock rail rings pass `r_railWidth->integer`
- q3mme rings pass `e->radius`

`RB_AddQuadStampRef` adapts byte `shaderRGBA` to each renderer's expected color type.

`RT_MODEL_FX_DIR/ANGLES/AXIS` are still aliases to `RT_MODEL`. The cgame computes the model axis in `CG_FX_Model`; the renderer does not have separate model-FX types. This may be fine, but it is not a literal Wolfcam renderer port.

---

## 7. Event / weapon hooks

Implemented:

- `EV_GIB_PLAYER` -> `CG_FX_GibPlayer` when `EffectScripts.gibbed` is non-empty; skips default gibs.
- `EV_THAW_PLAYER` -> `thawed`.
- `EV_JUMP_PAD` -> `jumpPad`.
- `EV_PLAYER_TELEPORT_IN/OUT`.
- `EV_AWARD` -> player medal scripts / `headShot`.
- Weapon fire (`CG_FireWeapon`).
- Muzzle flash (`CG_AddPlayerWeapon`).
- Bullet flesh impacts (`CG_Bullet`).
- Wall impacts (`CG_MissileHitWall`).
- Flesh impacts (`CG_MissileHitPlayer`).
- Projectile + missile trails (`CG_Missile`).
- Rail trail (`CG_RailTrail`).
- Lightning beam (`CG_LightningBolt`).
- Grapple trail (`CG_GrappleTrail`).
- Water bubbles (`CG_BubbleTrail`).
- Player legs/torso/head trails (`CG_Player`).
- Player flight/haste (`CG_Player`).
- Player quad (`CG_PlayerPowerups`).

Not hooked / missing:

- `playerTalk`
- `playerConnection`
- `playerImpressive`
- `playerExcellent`
- `playerHolyshit`
- `playerFriend`
- `playerDefend`
- `playerAssist`
- `playerCapture`
- Possibly other QL-specific reward/chat events with no equivalent in this cgame.

Why: this cgame's reward/chat structure differs from Wolfcam's. These effects load but will not auto-trigger.

---

## 8. Script-variable and beam/ring semantics

Important distinction:

- `ScriptVars.dir` is used by the q3mme beam as a **direction vector whose length encodes width**.
- `ScriptVars.end` is an absolute endpoint in some call paths.
- `CG_FX_RunRailScript` sets:
  - `ScriptVars.origin = start`
  - `ScriptVars.end = end`
  - `ScriptVars.dir = end - start` **without normalizing**, so the q3mme beam gets the correct width.
- The script itself normalizes `dir` later for helix rings.

Current `CG_FX_Beam` (q3mme mode):

```c
re->reType = RT_BEAM_Q3MME;
re->origin = ScriptVars.origin;
re->oldorigin = ScriptVars.dir;   /* direction/width, NOT absolute endpoint */
```

Current `CG_FX_Rings` (q3mme mode):

```c
re->reType = RT_RAIL_RINGS_Q3MME;
re->origin = ScriptVars.origin;
re->oldorigin = ScriptVars.dir;
re->radius = ScriptVars.size;
re->rotation = ScriptVars.width;
```

If the rail looks wrong, verify these are what the renderer receives. `FX submit` logs include type, radius, origin, and oldorigin.

---

## 9. Local entities - the core incomplete area

### Current implementation

`CG_AddLocalEntities()` in `cg_localents.c` has a fast path:

```c
if (le->fxType) {
    CG_FX_UpdatePhysics(le);

    if (le->emitterScript && interval elapsed) {
        ScriptVars = le->sv;
        ScriptVars.lerp = ...;
        EmitterScript = qtrue;
        PlainScript = qtrue;
        CG_RunQ3mmeScript(le->emitterScript, NULL);
        EmitterScript = qfalse;
        PlainScript = qfalse;
        le->sv = ScriptVars;
        le->lastRunTime = cg.time;
    }

    ... light/sound/loop ...
    if (le->refEntity.reType || le->refEntity.hModel) {
        ... update origin ...
        trap_R_AddRefEntityToScene(&le->refEntity);
    }
    continue;
}
```

`CG_FX_UpdatePhysics()` currently implements:

- `deltaTime = (cg.time - le->trTimef) * 0.001`
- `newOrigin = le->pos.trBase + velocity * deltaTime`
- gravity / `moveGravity`
- `moveBounce` trace + reflection
- bounce damping (`moveBounce2`)
- impact origin/direction
- distance accumulation
- updates `le->sv.origin`, `le->refEntity.origin`, `le->pos.trBase`, `le->trTimef`

Limitations:

- No model tumble/spin axis update.
- No `sink` implementation, despite checking `hasSink`.
- No external forces.
- No `CG_GetStoredScriptVarsFromLE` fidelity for all Wolfcam fields.
- `CG_Trace(..., -1, MASK_SOLID)` is a simplification.
- Does not run distance/impact/loop sub-scripts the way Wolfcam does.
- Does not update scale/alpha/color from `ScriptVars` each frame.
- Does not implement Wolfcam's `DistanceScript`, `ScriptJustParsing`, `PlainScript` state machine.
- Does not add with `R_AddRefEntityPtrToScene` or q3mme culling ratio.
- Sprites/rings that need emitter-script-driven position changes may remain static or be placed incorrectly.

This is why gibs can render but behave wrong, and why the rail rings can look wrong.

### Wolfcam reference

`C:\Users\stupi\Documents\Repos\wolfcamql-master\code\cgame\cg_localents.c`

Important functions/lines:

- `CG_Add_FX_Emitted()` - starts around line 2042
- movement/trace/bounce - roughly lines 2290-2440
- emitter script run - lines 2440-2470
- fade/scale/render - after 2470
- `CG_Add_FX_Emitted_Light()` - ~2712
- `CG_Add_FX_Emitted_Sound()` - ~2743
- `CG_Add_FX_Emitted_LoopSound()` - ~2771
- switch dispatch in `CG_AddLocalEntities()` - ~2931

Other Wolfcam helpers:

- `CG_GetStoredScriptVarsFromLE`
- `CG_CopyStoredScriptVarsToLE`
- `CG_AddRefEntityFX` / `R_AddRefEntityPtrToScene`
- `CG_CopyPositionDataToCent` / `CG_CopyPositionDataToScript`
- `CG_UpdatePositionData`

The next agent should port `CG_Add_FX_Emitted()` nearly verbatim, adapting:

- `cg.ftime` -> `cg.time`
- `cg.freecam` -> `cgs.freecam`
- `cg.paused` -> `cg_paused.integer`
- `R_AddRefEntityPtrToScene` -> `CG_AddRefEntityFX`
- `Lock_EntList` / `Unlock_EntList` -> no-op
- `IsFxModel` -> `reType == RT_MODEL`
- `MAX_FX_EXTERNAL_FORCES` / external forces if not needed
- `le->refEntity` / `le->sv` fields that already exist

Do not keep approximating this function. It is the difference between "entities exist" and "the .fx script looks like Wolfcam."

---

## 10. Known bugs / failure modes

### 10.1 Gibs render but freeze

Cause chain:

1. Staged cgame was stale, so physics was not loaded.
2. After staging, basic physics exists but is not a full port.
3. `RT_MODEL` is enum **0**. The old render condition `if (le->refEntity.reType)` skipped all models. Fixed to:

```c
if (le->refEntity.reType || le->refEntity.hModel)
```

4. Model entities have `model=116..125` in logs, `reType=0`, `hModel` nonzero.
5. If they still freeze, inspect `FX LE pre` positions over time. If `origin` is constant and `trTimef` is updating, physics is not being called or velocity is zero. If `trTimef` is zero, the helper returns once to initialize it.

### 10.2 No blood / no gib trails

The `player/gibbed` script uses:

```text
distance 10 {
    clear velocity
    velocity2 -10
    ...
    emitter 2 {
        moveGravity 0
        alphaFade 0
        rotate rotate + 10*lerp
        size 10 + 12*lerp
        sprite
    }
}
```

The distance sub-script lifecycle is not implemented. Body-part models appear, but blood sprites from `distance`/`impact` are not emitted. q3mme renderer types alone do not fix this.

### 10.3 Rail still wrong

Likely causes, in order:

1. Local-entity emitter state machine is incomplete (see section 9). Rings are created but their per-emitter `origin`/offset updates and `rotatearound`/`addScale` application may not match Wolfcam.
2. `CG_FX_Rings` / `CG_FX_Beam` are created during the emitter block with `emitterScript` attached. When localents runs the emitter script with `EmitterScript = qtrue`, all `CG_FX_*` functions early-return. Wolfcam handles this with additional state/flags; our simplified loop does not.
3. Verify `ScriptVars.dir` length and whether the script normalized it before `Beam` vs before `rings`.
4. Verify `r_railSegmentLength` (user config was `0` at one point). It does not affect q3mme rings but breaks stock rail rings.
5. Verify `cg_fxDebug` logs show `type=10` beam and `type=11` rings, with meaningful `origin`/`oldorigin`.
6. A screenshot is required to distinguish wrong width, wrong orientation, giant quads, or no helix.

### 10.4 "Nothing changed"

Check staging first. Compare cgame lengths. A stale stage DLL has already caused this once.

### 10.5 Log file not growing

`FS_FOpenFileByMode` returns **0 on success** for `FS_WRITE`/`FS_APPEND`. The log open code originally treated `<= 0` as failure. Fixed by checking the handle, not the return value, and using `FS_APPEND_SYNC`.

`fxdebug.log` is under `fs_homepath/baseq3`; `fxlog` prints the paths.

### 10.6 JIT

`cg_fxCompiled` defaults to `0`. The JIT token array is huge and not validated. Do not enable until script behavior is correct.

---

## 11. Debugging

### Cvars / commands

```text
cg_fxDebug 0   off
cg_fxDebug 1   load summary + script runs
cg_fxDebug 2   allocations + direct ref entity submissions
cg_fxDebug 3   per-local-entity per-frame logs (huge, tens of MB)
cg_fxCompiled 0 interpreter (safe)
cg_fxCompiled 1 JIT (untested)
cg_fxfile "scripts/madir.fx"
```

Commands:

```text
fxload [path]
fxlist
fxlog
clearfx
fxmath <expr>
runfx <effect/name>
```

`fxload` opens a fresh `fxdebug.log` and writes the load summary.

### Useful log lines

```text
FX: loaded '...': N named effect(s)
FX effect[0]: ...
FX: gibbed=1 railTrail=1 ...
FX: rail script start=... end=...
FX: alloc localEntity ...
FX beam LE ... reType=10 ...
FX LE pre ... fxType=1 reType=0 model=116 ...
FX submit ... type=10 renderfx=0 shader=... radius=... origin=... old=...
FX LE freed by lifetime ...
```

### Finding the log

```powershell
Get-ChildItem C:\Users\stupi -Recurse -File -Filter fxdebug.log
```

Expected location:

`build\ioquakelive-windows-x64\baseq3\fxdebug.log`

or `%APPDATA%\quakelive\baseq3\fxdebug.log`.

---

## 12. Assets and licensing

`pak-fx-assets.pk3` contains:

- `models/gibsq3/*.md3` (copied/renamed from `content/optional/gore/dlc_gibs`)
- `dlc_gibs/gibs.jpg`
- `dlc_gibs/blood_stain.png`
- `dlc_gibs/bloodspray1.png`
- `scripts/custom_fx.shader`
- `sound/player/gibsplt1.wav`
- `sound/player/gibimp1.wav`
- `sound/player/gibimp2.wav`
- `sound/player/gibimp3.wav`

Quake Live and Steam Workshop content is **not** GPL. Do not commit these assets, do not add them to release packages, and do not redistribute without permission.

`madir.fx` is a user script and is not part of the repository. It references Wolfcam paths:

- `models/gibsq3/*.md3`
- `q3bloodMark`
- `q3bloodTrail`
- `sound/player/gibsplt1.wav`
- `sound/player/gibimp*.wav`

The asset pak provides aliases/shaders, but the visual result is still not Wolfcam-identical.

---

## 13. Recommended next steps, in order

1. **Verify the staged cgame is current.**
   - Expected: `1,479,168` bytes.
   - Confirm the game process is not loading an old DLL.

2. **Port `CG_Add_FX_Emitted()` fully from Wolfcam.**
   - Replace the simplified FX branch in `cg_localents.c`.
   - Port light/sound/loop variants.
   - Port the `DistanceScript` / `ScriptJustParsing` / `PlainScript` state machine.
   - Preserve `le->sv`, `le->pos`, `trTimef`, `lastRunTime`, `startTime/endTime`, `lifeRate`.
   - Update model axis/tumble and scale/fade/color from `ScriptVars`.
   - This is the highest-value fix for rail, gibs, blood, and trails.

3. **Port the missing script-variable copy fields.**
   - `CG_GetStoredScriptVarsFromLE` / `CG_CopyStoredScriptVarsToLE` currently have large `#if 0` blocks.
   - Ensure all movement/rotation/alpha/size/distance fields survive between the LE and `ScriptVars`.

4. **Fix rail geometry after localents is correct.**
   - Compare `CG_FX_Beam`/`CG_FX_Rings` against Wolfcam exactly.
   - Check `ScriptVars.dir` length at the moment `Beam` and `rings` execute.
   - Check `rotatearound`, `perpendicular`, `add`, `addScale`, `parentOrigin`.
   - Validate `RT_BEAM_Q3MME` width (`|oldorigin| * 0.5`) and `RT_RAIL_RINGS_Q3MME` (`rotation`, `radius`).
   - Test with `r_railSegmentLength 32`.

5. **Port `distance` / `impact` / `death` sub-scripts.**
   - Required for blood, gib trails, wall marks, and many q3mme effects.

6. **Hook the remaining player script slots** if exact Wolfcam behavior is required:
   - talk, connection, impressive, excellent, holyshit, friend, defend, assist, capture.
   - These need cgame reward/chat integration.

7. **Optimize/fix JIT only after interpreter behavior is correct.**

8. **Add a regression test script** that verifies:
   - load summary
   - rail beam/rings submissions
   - gib model allocation + movement over multiple frames
   - distance blood sprites

---

## 14. Rules / constraints

From `AGENTS.md`:

- **Do not launch the game.** Build and inspect; ask the user to test.
- **Never terminate a user process.**
- **Do not commit or push unless explicitly asked.**
- **Do not bundle proprietary assets.**
- `build/` is generated and git-ignored.

`AGENTS.md` itself is git-ignored.

No commits have been made for this FX work. The working tree contains uncommitted source changes.

---

## 15. Honest assessment

The port currently has:

- a working `.fx` loader/parser
- a working interpreter entry point
- real q3mme renderer entity types in all three renderers
- many event/weapon hooks
- basic FX local-entity movement/bounce
- asset aliases for the user's `madir.fx`

It does **not** yet have a faithful Wolfcam local-entity/emitter lifecycle. That is where the remaining visual differences and missing blood/physics behavior come from. Porting `CG_Add_FX_Emitted()` exactly is the correct next move; further renderer or hook tweaks will not compensate for it.
