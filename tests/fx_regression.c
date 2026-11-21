/* FX regression harness. Links the real cgame objects with mocked engine syscalls.
 * No engine, renderer, server, map, Steam, or game initialization is started.
 * Build cgame Release x64, then from a VS x64 developer shell:
 * cl /nologo /MT /DWIN32 /DMISSIONPACK tests\fx_regression.c build\Release\x64\baseq3\cgame\*.obj /link /SUBSYSTEM:CONSOLE /OUT:fx-regression.exe
 * Run: fx-regression.exe [optional path to a local madir.fx]
 */
#include "../code/cgame/cg_local.h"
#include "../code/cgame/cg_fx_compat.h"
#include <stdarg.h>
#include <time.h>

extern localEntity_t cg_activeLocalEntities;
void dllEntry(intptr_t (QDECL *fn)(intptr_t, ...));
static snapshot_t snapshot;
static FILE *scriptFile;
static int submitted, lights, sounds, marks, errors;
static int debugCvarQueries;
static qboolean floorEnabled;
static refEntity_t lastEntity;
static vec3_t lastLightOrigin;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); exit(2); } } while (0)
#define NEAR(a,b) (fabs((a)-(b)) < 0.02)

static intptr_t QDECL MockSyscall(intptr_t call, ...) {
    va_list ap;
    intptr_t result = 0;
    va_start(ap, call);
    switch (call) {
    case CG_PRINT: {
        const char *msg = va_arg(ap, const char *);
        fputs(msg, stdout);
        if (strstr(msg, "error") || strstr(msg, "unknown")) errors++;
        break;
    }
    case CG_ERROR:
        fprintf(stderr, "%s", va_arg(ap, const char *)); exit(3);
    case CG_CVAR_VARIABLESTRINGBUFFER: {
        const char *name = va_arg(ap, const char *);
        char *buffer = va_arg(ap, char *);
        int size = va_arg(ap, int);
        const char *value = "";
        if (!Q_stricmp(name, "debug_fxscript")) debugCvarQueries++;
        if (!Q_stricmp(name, "cg_railTrailTime")) value = "1000";
        if (!Q_stricmp(name, "r_railCoreWidth")) value = "6";
        Q_strncpyz(buffer, value, size);
        break;
    }
    case CG_CVAR_UPDATE: break;
    case CG_FS_FOPENFILE: {
        const char *path = va_arg(ap, const char *);
        fileHandle_t *handle = va_arg(ap, fileHandle_t *);
        int mode = va_arg(ap, int);
        CHECK(mode == FS_READ);
        scriptFile = fopen(path, "rb");
        *handle = scriptFile ? 1 : 0;
        if (scriptFile) { fseek(scriptFile, 0, SEEK_END); result = ftell(scriptFile); rewind(scriptFile); }
        break;
    }
    case CG_FS_READ: {
        void *buf = va_arg(ap, void *);
        int count = va_arg(ap, int);
        memset(buf, 0, count);
        fread(buf, 1, count, scriptFile);
        break;
    }
    case CG_FS_FCLOSEFILE: fclose(scriptFile); scriptFile = NULL; break;
    case CG_R_REGISTERMODEL: case CG_R_REGISTERSHADER: case CG_S_REGISTERSOUND:
        result = 1; break;
    case CG_R_ADDREFENTITYTOSCENE:
        lastEntity = *va_arg(ap, const refEntity_t *); submitted++; break;
    case CG_R_ADDLIGHTTOSCENE: {
        const float *origin = va_arg(ap, const float *);
        VectorCopy(origin, lastLightOrigin); lights++; break;
    }
    case CG_S_STARTSOUND: sounds++; break;
    case CG_CM_MARKFRAGMENTS: marks++; break;
    case CG_CM_BOXTRACE: {
        trace_t *tr = va_arg(ap, trace_t *);
        const float *start = va_arg(ap, const float *);
        const float *end = va_arg(ap, const float *);
        memset(tr, 0, sizeof(*tr));
        tr->fraction = 1;
        VectorCopy(end, tr->endpos);
        if (floorEnabled && end[2] < 0 && start[2] >= 0) {
            tr->fraction = start[2] / (start[2] - end[2]);
            VectorSubtract(end, start, tr->endpos); VectorMA(start, tr->fraction, tr->endpos, tr->endpos);
            tr->plane.normal[2] = 1;
        }
        break;
    }
    case CG_CM_POINTCONTENTS: break;
    case CG_R_MODELBOUNDS: {
        vec_t *mins, *maxs;
        (void)va_arg(ap, int); mins = va_arg(ap, vec_t *); maxs = va_arg(ap, vec_t *);
        VectorSet(mins, -8, -8, -8); VectorSet(maxs, 8, 8, 8); break;
    }
    default: fprintf(stderr, "Unexpected syscall %d\n", (int)call); exit(4);
    }
    va_end(ap);
    return result;
}

static void Reset(void) {
    CG_InitLocalEntities();
    CG_ResetScriptVars();
    cg.time = 1000;
    cg.snap = &snapshot;
    cg_fxinterval.value = 25;
    cg_addMarks.integer = 1;
    cg_impactMarkTime.integer = 10000;
    cg_fxScriptMinDistance.value = 0.001f;
    cg_fxScriptMinInterval.value = 0.001f;
    floorEnabled = qfalse;
    submitted = lights = sounds = marks = 0;
}
static int Count(int type) {
    localEntity_t *le;
    int count = 0;
    for (le = cg_activeLocalEntities.next; le != &cg_activeLocalEntities; le = le->next) {
        if (type < 0 || (le->fxType == LEFX_EMIT && le->refEntity.reType == type)) count++;
        CHECK(count < 20000);
    }
    return count;
}
static void Run(const char *script) {
    if (cg_fxCompiled.integer) {
        CG_FreeFxJitTokens();
        Q_strncpyz(EffectScripts.extra[MAX_FX_EXTRA-1], script, MAX_FX_SCRIPT_SIZE);
        script = EffectScripts.extra[MAX_FX_EXTRA-1];
    }
    CHECK(!CG_RunQ3mmeScript(script, NULL));
}
static void Frame(int time) { cg.time = time; submitted = 0; CG_AddLocalEntities(); }

int main(int argc, char **argv) {
    localEntity_t *le;
    vec3_t start = {100,200,300}, end = {200,200,300};
    dllEntry(MockSyscall);
    cg_fxCompiled.integer = argc > 2 && !strcmp(argv[2], "--compiled");
    Reset();
    CHECK(CG_FX_RunRailScript("copy parentOrigin origin\nemitter 1 {\nbeam\n}\n", NULL, start, end));
    le = cg_activeLocalEntities.next;
    CHECK(VectorCompare(le->refEntity.origin, start));
    CHECK(NEAR(le->refEntity.oldorigin[0],100));

    Reset();
    VectorSet(ScriptVars.origin, 100, 200, 300);
    Run("emitter 1 {\naddScale origin dir origin lerp * 10\nalphaFade 0\nsize 10 + 12 * lerp\nsprite\n}\n");
    le = cg_activeLocalEntities.next;
    VectorSet(le->sv.dir, 1, 0, 0);
    Frame(1500);
    CHECK(NEAR(lastEntity.radius,16));
    CHECK(lastEntity.shaderRGBA[3] == 127);
    CHECK(NEAR(lastEntity.origin[0],105));
    Frame(1750);
    CHECK(NEAR(lastEntity.radius,19));
    CHECK(NEAR(lastEntity.origin[0],107.5));
    CHECK(lastEntity.shaderRGBA[3] == 63);
    Frame(1750);
    CHECK(NEAR(lastEntity.origin[0],107.5));
    Frame(2000); CHECK(Count(-1) == 0);

    Reset();
    Run("emitter 2 {\nmoveGravity 800\nsprite\n}\n");
    Frame(1100); CHECK(NEAR(lastEntity.origin[2], -8));
    Frame(1200); CHECK(NEAR(lastEntity.origin[2], -32));

    Reset();
    VectorSet(ScriptVars.velocity, 100, 0, 0);
    Run("emitter 2 {\nmoveGravity 0\ndistance 10 {\nclear velocity\nsize 20\nemitter 1 {\nalphaFade 0\nsprite\n}\n}\nanglesModel\n}\n");
    le = cg_activeLocalEntities.next;
    Frame(1250);
    CHECK(Count(RT_SPRITE) == 2);
    CHECK(NEAR(le->refEntity.origin[0],25));
    CHECK(NEAR(le->refEntity.radius,1));
    CHECK(NEAR(le->sv.velocity[0],100));
    Frame(1300); CHECK(Count(RT_SPRITE) == 3);
    CHECK(NEAR(le->refEntity.origin[0],30));

    Reset(); floorEnabled = qtrue;
    ScriptVars.origin[2] = 5; ScriptVars.velocity[2] = -100;
    Run("emitter 2 {\nmoveBounce 0 0.6\nimpact 50 {\nsize 25\nshader test\ndecal alpha\nsound test\n}\nanglesModel\n}\n");
    Frame(1100); CHECK(marks == 1 && sounds == 1);
    le = cg_activeLocalEntities.next;
    CHECK(NEAR(le->sv.velocity[2],60)); CHECK(NEAR(le->refEntity.radius,1));
    Frame(1200); CHECK(marks == 1 && sounds == 1); CHECK(lastEntity.origin[2] > 0);

    Reset(); floorEnabled = qtrue;
    ScriptVars.origin[2] = 5;
    Run("emitter 2 {\nmoveBounce 800 0.6\nsprite\n}\n");
    Frame(1100);
    le = cg_activeLocalEntities.next;
    CHECK(le->sv.velocity[2] > 0); // Falling from rest must bounce upward.
    Frame(1125); CHECK(lastEntity.origin[2] > 0);

    Reset();
    Run("emitter 2 {\nyaw lerp * 180\nsize 1 + lerp\nanglesModel\n}\n");
    Frame(2000); CHECK(NEAR(VectorLength(lastEntity.axis[0]),1.5));
    CHECK(NEAR(lastEntity.axis[0][0],0)); CHECK(NEAR(lastEntity.axis[0][1],1.5));

    Reset();
    Run("emitterf 0.1 {\nsize lerp\nsprite\n}\n");
    Frame(1200); CHECK(submitted == 1); CHECK(lastEntity.radius >= 1);
    Frame(1225); CHECK(Count(-1) == 0);

    Reset();
    VectorSet(ScriptVars.origin, 10, 20, 30);
    Run("emitter 1 {\nlight\n}\n");
    Frame(1500); CHECK(lights == 1 && NEAR(lastLightOrigin[0],10));
    ScriptVars.origin[0] = 999;
    Frame(1500); CHECK(lights == 2 && NEAR(lastLightOrigin[0],10));

    Reset();
    Run("emitter 1 {\nsprite\nmoveBounce 400 0.75\n}\n");
    Frame(1100); CHECK(NEAR(lastEntity.origin[2],-4));
    Frame(1200); CHECK(NEAR(lastEntity.origin[2],-16));

    if (argc > 1) {
        Reset(); CG_ReloadQ3mmeScripts(argv[1]); CHECK(EffectScripts.numEffects == 4);
        end[0] = 8292;
        CHECK(CG_FX_RunRailScript(EffectScripts.weapons[WP_RAILGUN].trailScript, NULL, start, end));
        CHECK(Count(RT_BEAM_Q3MME) == 1);
        CHECK(Count(RT_RAIL_RINGS_Q3MME) == 10240);
        Frame(1250); CHECK(submitted == 10241);
        for (le=cg_activeLocalEntities.next; le!=&cg_activeLocalEntities; le=le->next) {
            if (le->refEntity.reType == RT_RAIL_RINGS_Q3MME && le->fxType == LEFX_EMIT) {
                CHECK(le->refEntity.origin[0] >= 99 && le->refEntity.origin[0] <= 8293);
                CHECK(fabs(le->refEntity.origin[1]-200) < 10);
                CHECK(fabs(le->refEntity.origin[2]-300) < 10);
            }
        }
        {
            clock_t begin = clock();
            int frame;
            // Measure active full-length rail updates without file logging or a renderer.
            for (frame = 0; frame < 10; frame++) Frame(1275 + frame * 25);
            printf("FX CPU check: %.3f ms/update for 10,240 rings (mock renderer)\n",
                   (double)(clock()-begin) * 1000.0 / CLOCKS_PER_SEC / 10.0);
        }
        Reset(); VectorCopy(start,cg_entities[0].lerpOrigin);
        CG_FX_GibPlayer(&cg_entities[0]); CHECK(Count(RT_MODEL) == 11);
        Frame(1100); CHECK(Count(RT_SPRITE) > 0);
        CHECK(Count(RT_MODEL) == 11);
        for (le=cg_activeLocalEntities.next; le!=&cg_activeLocalEntities; le=le->next) {
            if (le->fxType == LEFX_EMIT && le->refEntity.reType == RT_SPRITE) {
                CHECK(le->refEntity.shaderRGBA[0] == 178);
                CHECK(le->refEntity.shaderRGBA[1] == 0 && le->refEntity.shaderRGBA[2] == 0);
                CHECK(le->refEntity.shaderRGBA[3] == 255);
            }
        }
    }
    // Exhaust the pool from inside an updating emitter. The parent and list
    // cursor must survive eviction of older children.
    Reset();
    Run("emitter 2 {\nif lerp > 0 {\nrepeat 17000 {\nemitter 1 {\nsprite\n}\n}\n}\nanglesModel\n}\n");
    le = cg_activeLocalEntities.next;
    Frame(1100);
    CHECK(le->fxType == LEFX_EMIT && le->refEntity.reType == RT_MODEL);
    CHECK(Count(-1) == MAX_REFENTITIES - 3);

    CG_FreeFxJitTokens();
    CHECK(debugCvarQueries == 0); // Debug state is registered and refreshed per frame.
    CHECK(errors == 0);
    puts("PASS: FX rail, fade, motion, nested distance, impact, model axes, lifetime and light regression checks");
    return 0;
}
