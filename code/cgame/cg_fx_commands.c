/*
===========================================================================
FX scripting console commands.
Portions derived from WolfcamQL (GNU General Public License v2).
See COPYING.txt for the full license.
===========================================================================
*/

#include "cg_local.h"
#include "cg_fx_scripts.h"
#include "cg_fx_compat.h"

static void CG_FX_RunNamed(const char* name, const vec3_t origin, const vec3_t dir, const vec3_t velocity) {
    int i;

    CG_ResetScriptVars();
    VectorCopy(origin, ScriptVars.origin);
    VectorCopy(dir, ScriptVars.dir);
    VectorNormalize(ScriptVars.dir);
    VectorCopy(velocity, ScriptVars.velocity);
    VectorCopy(origin, ScriptVars.lastDistancePosition);
    VectorCopy(origin, ScriptVars.lastIntervalPosition);
    ScriptVars.lastDistanceTime = cg.time;
    ScriptVars.lastIntervalTime = cg.time;
    Vector4Set(ScriptVars.color, 1, 1, 1, 1);
    ScriptVars.size = 1;
    ScriptVars.width = 1;

    for (i = 0; i < EffectScripts.numEffects; i++) {
        if (!Q_stricmp(name, EffectScripts.names[i])) {
            CG_RunQ3mmeScript(EffectScripts.ptr[i], NULL);
            return;
        }
    }

    Com_Printf("couldn't find fx '%s'\n", name);
}

static void CG_FXLoad_f(void) {
    if (trap_Argc() > 1) {
        char name[MAX_QPATH];

        trap_Argv(1, name, sizeof(name));
        CG_ReloadQ3mmeScripts(name);
    } else {
        CG_ReloadQ3mmeScripts(cg_fxfile.string);
    }
    if (cg_fxDebug.integer > 0) {
        Com_Printf("FX: diagnostics written to fxdebug.log\n");
    }
}

static void CG_ClearFX_f(void) {
    if (trap_Argc() < 2) {
        CG_RemoveFXLocalEntities(qtrue, 0);
    } else {
        char arg[32];

        trap_Argv(1, arg, sizeof(arg));
        CG_RemoveFXLocalEntities(qfalse, (float)atof(arg));
    }
}

static void CG_FXMath_f(void) {
    char s[1024];
    float value = 0.0f;
    int i, err = 0;
    int argc = trap_Argc();

    if (argc < 2) {
        Com_Printf("usage: fxmath <math expression>\n");
        return;
    }

    s[0] = '\0';
    for (i = 1; i < argc; i++) {
        char arg[MAX_STRING_CHARS];

        trap_Argv(i, arg, sizeof(arg));
        Q_strcat(s, sizeof(s), arg);
        Q_strcat(s, sizeof(s), " ");
    }

    CG_Q3mmeMath(s, &value, &err);
    Com_Printf("%f\n", value);
}

static void CG_RunFX_f(void) {
    vec3_t origin, dir, velocity;
    char name[MAX_QPATH];
    int argc = trap_Argc();

    if (argc < 2) {
        Com_Printf("usage: runfx <fx name> [origin0] [origin1] [origin2] [dir0] [dir1] [dir2] "
                   "[velocity0] [velocity1] [velocity2]\n");
        return;
    }

    trap_Argv(1, name, sizeof(name));

    VectorCopy(cg.refdef.vieworg, origin);
    VectorCopy(cg.refdef.viewaxis[0], dir);
    VectorCopy(cg.predictedPlayerState.velocity, velocity);

    if (argc > 2) origin[0] = (float)atof(CG_Argv(2));
    if (argc > 3) origin[1] = (float)atof(CG_Argv(3));
    if (argc > 4) origin[2] = (float)atof(CG_Argv(4));
    if (argc > 5) dir[0] = (float)atof(CG_Argv(5));
    if (argc > 6) dir[1] = (float)atof(CG_Argv(6));
    if (argc > 7) dir[2] = (float)atof(CG_Argv(7));
    if (argc > 8) velocity[0] = (float)atof(CG_Argv(8));
    if (argc > 9) velocity[1] = (float)atof(CG_Argv(9));
    if (argc > 10) velocity[2] = (float)atof(CG_Argv(10));

    CG_FX_RunNamed(name, origin, dir, velocity);
}

void CG_FX_GibPlayer(centity_t* cent) {
    CG_ResetScriptVars();
    CG_CopyPlayerDataToScriptData(cent);
    VectorCopy(cent->lerpOrigin, ScriptVars.lastIntervalPosition);
    ScriptVars.lastIntervalTime = cg.time;
    VectorCopy(cent->currentState.pos.trDelta, ScriptVars.parentVelocity);
    Vector4Set(ScriptVars.color, 1, 1, 1, 1);
    CG_RunQ3mmeScript(EffectScripts.gibbed, NULL);
}

static void CG_FXList_f(void) {
    int i;

    Com_Printf("FX: %d named effect(s)\n", EffectScripts.numEffects);
    for (i = 0; i < EffectScripts.numEffects; i++) {
        Com_Printf("FX[%d]: %s\n", i, EffectScripts.names[i]);
    }
}

void CG_FX_RegisterCommands(void) {
    trap_AddCommand("fxload");
    trap_AddCommand("fxlist");
    trap_AddCommand("fxlog");
    trap_AddCommand("clearfx");
    trap_AddCommand("fxmath");
    trap_AddCommand("runfx");
}

qboolean CG_FX_Command(const char* cmd) {
    if (!Q_stricmp(cmd, "fxload")) {
        CG_FXLoad_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "fxlist")) {
        CG_FXList_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "fxlog")) {
        CG_FXLog_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "clearfx")) {
        CG_ClearFX_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "fxmath")) {
        CG_FXMath_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "runfx")) {
        CG_RunFX_f();
        return qtrue;
    }
    return qfalse;
}
