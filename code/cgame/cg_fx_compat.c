/*
===========================================================================
FX scripting compatibility helpers.
Portions derived from WolfcamQL (GNU General Public License v2).
See COPYING.txt for the full license.
===========================================================================
*/

#include "cg_local.h"
#include "cg_fx_compat.h"
#include <stdarg.h>

static fileHandle_t fxLogFile;
static qboolean fxLogOpen;
static int fxLogBytes;

#define FX_LOG_MAX_BYTES (8 * 1024 * 1024)

void CG_FX_LogOpen(qboolean truncate) {
    fileHandle_t trunc = 0;

    trap_Cvar_Update(&cg_fxDebug);
    if (fxLogOpen) {
        trap_FS_FCloseFile(fxLogFile);
        fxLogOpen = qfalse;
    }
    if (cg_fxDebug.integer <= 0) {
        return;
    }
    if (truncate) {
        trap_FS_FOpenFile("fxdebug.log", &trunc, FS_WRITE);
        if (trunc) {
            trap_FS_FCloseFile(trunc);
        }
    }
    fxLogFile = 0;
    trap_FS_FOpenFile("fxdebug.log", &fxLogFile, FS_APPEND_SYNC);
    if (!fxLogFile) {
        fxLogOpen = qfalse;
        Com_Printf("FX: unable to open fxdebug.log for append\n");
        return;
    }
    fxLogBytes = 0;
    fxLogOpen = qtrue;
}

void CG_FX_LogClose(void) {
    if (fxLogOpen) {
        trap_FS_FCloseFile(fxLogFile);
        fxLogOpen = qfalse;
    }
}

void CG_FX_Log(const char* fmt, ...) {
    char buf[2048];
    va_list ap;
    int len;

    // CG_UpdateCvars refreshes this once per frame; avoid a syscall per log line.
    if (cg_fxDebug.integer <= 0) {
        return;
    }
    if (fxLogBytes >= FX_LOG_MAX_BYTES) {
        return;
    }
    if (!fxLogOpen) {
        CG_FX_LogOpen(qfalse);
    }
    if (!fxLogOpen) {
        return;
    }

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (len < 0) {
        return;
    }
    if (len >= (int)sizeof(buf)) {
        len = (int)sizeof(buf) - 1;
    }
    trap_FS_Write(buf, len, fxLogFile);
    fxLogBytes += len;
}

void CG_FXLog_f(void) {
    char home[MAX_OSPATH];
    char base[MAX_OSPATH];
    char game[MAX_QPATH];

    trap_Cvar_VariableStringBuffer("fs_homepath", home, sizeof(home));
    trap_Cvar_VariableStringBuffer("fs_basepath", base, sizeof(base));
    trap_Cvar_VariableStringBuffer("fs_game", game, sizeof(game));

    CG_FX_LogOpen(qtrue);
    CG_FX_Log("FX log start\n");
    CG_FX_Log("fs_homepath=%s\n", home);
    CG_FX_Log("fs_basepath=%s\n", base);
    CG_FX_Log("fs_game=%s\n", game);
    CG_FX_LogClose();

    Com_Printf("FX log: fxdebug.log (written under fs_homepath/baseq3)\n");
    Com_Printf("  fs_homepath: %s\n", home);
    Com_Printf("  fs_basepath: %s\n", base);
    Com_Printf("  fs_game: %s\n", game);
}


int FxLoopSounds;

void CG_MakeLowPriorityEntity(localEntity_t* le) {
    le->lowPriority = qtrue;
}

void CG_AddRefEntityFX(const refEntity_t* re) {
    if (cg_fxDebug.integer >= 4) {
        CG_FX_Log("FX: addRef type=%d model=%d shader=%d radius=%.1f origin=(%.1f %.1f %.1f) old=(%.1f %.1f %.1f)\n",
                   (int)re->reType, re->hModel, re->customShader, re->radius,
                   re->origin[0], re->origin[1], re->origin[2],
                   re->oldorigin[0], re->oldorigin[1], re->oldorigin[2]);
    }
    trap_R_AddRefEntityToScene(re);
}

qboolean CG_FX_RunScript(const char* script, struct centity_s* cent, const vec3_t origin, const vec3_t dir,
                         const vec3_t velocity) {
    if (!script || !script[0]) {
        return qfalse;
    }

    if (cg_fxDebug.integer >= 1) {
        CG_FX_Log("FX: run script len=%d origin=(%.1f %.1f %.1f) '%.48s'\n", (int)strlen(script),
                   origin ? origin[0] : 0, origin ? origin[1] : 0, origin ? origin[2] : 0, script);
    }

    CG_ResetScriptVars();
    if (cent) {
        CG_CopyPlayerDataToScriptData(cent);
    }
    if (origin) {
        VectorCopy(origin, ScriptVars.origin);
        VectorCopy(origin, ScriptVars.parentOrigin);
        VectorCopy(origin, ScriptVars.lastIntervalPosition);
        VectorCopy(origin, ScriptVars.lastDistancePosition);
    }
    if (dir) {
        VectorCopy(dir, ScriptVars.dir);
        VectorCopy(dir, ScriptVars.parentDir);
    }
    if (velocity) {
        VectorCopy(velocity, ScriptVars.velocity);
        VectorCopy(velocity, ScriptVars.parentVelocity);
    }
    ScriptVars.lastIntervalTime = cg.time;
    ScriptVars.lastDistanceTime = cg.time;
    Vector4Set(ScriptVars.color, 1, 1, 1, 1);
    ScriptVars.size = 1;
    ScriptVars.width = 1;

    CG_RunQ3mmeScript(script, NULL);
    return qtrue;
}

qboolean CG_FX_RunImpactScript(const char* script, struct centity_s* cent, const vec3_t origin, const vec3_t dir) {
    if (!script || !script[0]) {
        return qfalse;
    }

    if (cg_fxDebug.integer >= 1) {
        CG_FX_Log("FX: impact script len=%d origin=(%.1f %.1f %.1f) '%.48s'\n", (int)strlen(script),
                   origin ? origin[0] : 0, origin ? origin[1] : 0, origin ? origin[2] : 0, script);
    }

    CG_ResetScriptVars();
    if (cent) {
        CG_CopyPlayerDataToScriptData(cent);
    }
    if (origin) {
        VectorCopy(origin, ScriptVars.origin);
        VectorCopy(origin, ScriptVars.parentOrigin);
        VectorCopy(origin, ScriptVars.impactOrigin);
    }
    if (dir) {
        VectorCopy(dir, ScriptVars.dir);
        VectorCopy(dir, ScriptVars.parentDir);
        VectorCopy(dir, ScriptVars.impactDir);
    }
    VectorCopy(ScriptVars.origin, ScriptVars.lastIntervalPosition);
    VectorCopy(ScriptVars.origin, ScriptVars.lastDistancePosition);
    ScriptVars.lastIntervalTime = cg.time;
    ScriptVars.lastDistanceTime = cg.time;
    ScriptVars.impacted = qtrue;
    Vector4Set(ScriptVars.color, 1, 1, 1, 1);
    ScriptVars.size = 1;
    ScriptVars.width = 1;

    CG_RunQ3mmeScript(script, NULL);
    return qtrue;
}

qboolean CG_FX_RunRailScript(const char* script, const clientInfo_t* ci, const vec3_t start, const vec3_t end) {
    vec3_t dir;

    if (!script || !script[0]) {
        return qfalse;
    }

    if (cg_fxDebug.integer >= 1) {
        CG_FX_Log("FX: rail script start=(%.1f %.1f %.1f) end=(%.1f %.1f %.1f) '%.48s'\n",
                   start[0], start[1], start[2], end[0], end[1], end[2], script);
    }

    CG_ResetScriptVars();
    if (ci) {
        int clientNum = (int)(ci - cgs.clientinfo);
        if (clientNum >= 0 && clientNum < MAX_CLIENTS) {
            CG_CopyPlayerDataToScriptData(&cg_entities[clientNum]);
        }
    }
    VectorCopy(start, ScriptVars.origin);
    VectorCopy(start, ScriptVars.parentOrigin);
    VectorCopy(end, ScriptVars.end);
    VectorCopy(end, ScriptVars.parentEnd);
    VectorSubtract(end, start, dir);
    VectorCopy(dir, ScriptVars.dir);
    VectorCopy(dir, ScriptVars.parentDir);
    VectorCopy(start, ScriptVars.lastIntervalPosition);
    VectorCopy(start, ScriptVars.lastDistancePosition);
    ScriptVars.lastIntervalTime = cg.time;
    ScriptVars.lastDistanceTime = cg.time;
    Vector4Set(ScriptVars.color, 1, 1, 1, 1);
    if (ci) {
        VectorCopy(ci->color1, ScriptVars.color);
        VectorCopy(ci->color1, ScriptVars.color1);
        VectorCopy(ci->color2, ScriptVars.color2);
    }
    ScriptVars.size = 1;
    ScriptVars.width = 1;

    CG_RunQ3mmeScript(script, NULL);
    return qtrue;
}

void CG_FX_RunPlayerScript(const char* script, struct centity_s* cent, const vec3_t origin, int animFrame) {
    if (!script || !script[0]) {
        return;
    }

    CG_ResetScriptVars();
    CG_CopyPlayerDataToScriptData(cent);
    VectorCopy(origin, ScriptVars.origin);
    VectorCopy(origin, ScriptVars.lastIntervalPosition);
    VectorCopy(origin, ScriptVars.lastDistancePosition);
    VectorCopy(cent->currentState.pos.trDelta, ScriptVars.velocity);
    ScriptVars.lastIntervalTime = cg.time;
    ScriptVars.lastDistanceTime = cg.time;
    ScriptVars.animFrame = animFrame;
    Vector4Set(ScriptVars.color, 1, 1, 1, 1);
    ScriptVars.size = 1;
    ScriptVars.width = 1;

    CG_RunQ3mmeScript(script, NULL);
}

void CG_RemoveFXLocalEntities(qboolean all, float emitterId) {
    extern localEntity_t cg_activeLocalEntities;
    localEntity_t* le;
    localEntity_t* next;

    for (le = cg_activeLocalEntities.prev; le != &cg_activeLocalEntities; le = next) {
        next = le->prev;
        if (le->fxType && (all || le->sv.emitterId == emitterId)) {
            CG_FreeLocalEntity(le);
        }
    }
}

void CG_StartSound(const vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx) {
    trap_S_StartSound((float*)origin, entityNum, entchannel, sfx);
}

void CG_StartLocalSound(sfxHandle_t sfx, int channelNum) {
    trap_S_StartLocalSound(sfx, channelNum);
}

void CG_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx) {
    trap_S_AddLoopingSound(entityNum, origin, velocity, sfx);
}

qboolean CG_IsUs(const clientInfo_t* ci) {
    return (qboolean)(cg.snap && ci == &cgs.clientinfo[cg.snap->ps.clientNum]);
}

qboolean CG_IsTeammate(const clientInfo_t* ci) {
    return (qboolean)(cg.snap && !CG_IsUs(ci) && ci->team == cgs.clientinfo[cg.snap->ps.clientNum].team);
}

qboolean CG_IsEnemy(const clientInfo_t* ci) {
    return (qboolean)(cg.snap && !CG_IsUs(ci) && ci->team != cgs.clientinfo[cg.snap->ps.clientNum].team);
}

qboolean CG_IsFirstPersonView(int clientNum) {
    return (qboolean)(cg.snap && clientNum == cg.snap->ps.clientNum && !cg.renderingThirdPerson);
}

qboolean CG_IsTeamGame(int gametype) {
    return (qboolean)(gametype == GT_TEAM || gametype == GT_CTF || gametype == GT_1FCTF ||
                      gametype == GT_OBELISK || gametype == GT_HARVESTER || gametype == GT_FREEZE);
}

qboolean CG_IsFlagGame(int gametype) {
    return (qboolean)(gametype == GT_CTF || gametype == GT_1FCTF);
}

qboolean CG_IsDuelGame(int gametype) {
    return (qboolean)(gametype == GT_DUEL);
}

qboolean trap_Cvar_Exists(const char* name) {
    char buf[1024];

    trap_Cvar_VariableStringBuffer(name, buf, sizeof(buf));
    return (qboolean)(buf[0] != 0);
}

void trap_autoWriteConfig(qboolean enable) {
    (void)enable;
}

void trap_SendConsoleCommandNow(const char* text) {
    trap_SendConsoleCommand(text);
}

void Vector4Set(vec4_t v, float x, float y, float z, float w) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

void CG_Abort(void) {
    CG_Error("FX script aborted");
}

void SC_Lowercase (char *p)
{
    while (*p) {
        int c = *p;
        *p = (char)tolower(c);
        p++;
    }
}


int SC_Cvar_Get_Int (const char *cvar)
{
    char buff[MAX_STRING_CHARS];

    //memset(buff, 0, sizeof(buff));  //FIXME why are you doing this again??
    buff[0] = '\0';
    trap_Cvar_VariableStringBuffer(cvar, buff, sizeof(buff));

    return atoi(buff);
}


float SC_Cvar_Get_Float (const char *cvar)
{
    char buff[MAX_STRING_CHARS];

    buff[0] = '\0';
    trap_Cvar_VariableStringBuffer(cvar, buff, sizeof(buff));

    return atof(buff);
}


const char *SC_Cvar_Get_String (const char *cvar)
{
    static char buff[MAX_STRING_CHARS];

    buff[0] = '\0';
    trap_Cvar_VariableStringBuffer(cvar, buff, sizeof(buff));

    return (const char *)buff;
}


char *CG_FS_ReadLine (qhandle_t f, int *len)
{
    static char buffer[1024];
    int i;
    //int r;

    //trap_FS_Read( void *buffer, int len, fileHandle_t f );
    i = 0;
    *len = 0;
    buffer[0] = '\0';
    while (1) {
        if (i == 1022) {
            buffer[i + 1] = '\0';
            Com_Printf("CG_FS_ReadLine() buffer filled\n");
            return buffer;
        }
        buffer[i] = '\0';  // FS_Read leaves the destination unchanged at EOF.
        trap_FS_Read((void *)&buffer[i], 1, f);
        //Com_Printf("%d %c", (int)buffer[i], buffer[i]);
        //Com_Printf("\n");
        if (buffer[i] == '\0') {
            //Com_Printf("...\n");
            return buffer;
        }
        if (buffer[i] == '\n'  ||  buffer[i] == '\f'  ||  buffer[i] == '\r') {
            buffer[i] = '\n';
            buffer[i + 1] = '\0';
            (*len)++;
            return buffer;
        }

        (*len)++;
        i++;
    }

    *len = 0;
    return NULL;
}


const char *CG_GetToken (const char *inputString, char *token, qboolean isFilename, qboolean *newLine)
{
    //int i;
    const char *p;
    char c;
    qboolean gotFirstToken;
    //char *start;
    //char *tokenOrig;

    //tokenOrig = token;

    //start = token;

    if (1) {  //(isFilename) {
        //Com_Printf("inputstring '%s'\n", inputString);
    }

	if (!inputString) {
		Com_Printf("CG_GetToken inputString == NULL\n");
		return NULL;
	}


	//if (inputString >= EffectScripts.weapons[0]
    *newLine = qfalse;
    p = inputString;
    token[0] = '\0';

    if (p[0] == '\0') {  //  ||  (p[0] != '\t'  &&  (p[0] < ' '  ||  p[0] > '~'))) {
        //Com_Printf("xx\n");
        *newLine = qtrue;

#if 0
		if (jt  &&  !jt->valid) {
			jt->valid = qtrue;
			jt->end = p;
			Q_strncpyz(jt->token, tokenStart, sizeof(jt->token));
			jt->hitNewLine = *newLine;
		}
#endif
        //Com_Printf("CG_GetToken() '%s'\n", tokenOrig);
        return p;
    }

#if 0
    if (p[0] == '\0'  ||  (p[0] != '\t'  &&  (p[0] < ' '  ||  p[0] > '~'))) {
        Com_Printf("xx\n");
        *newLine = qtrue;

#if 0
		if (jt  &&  !jt->valid) {
			jt->valid = qtrue;
			jt->end = p;
			Q_strncpyz(jt->token, tokenStart, sizeof(jt->token));
			jt->hitNewLine = *newLine;
		}
#endif

        return p;
    }
#endif

    gotFirstToken = qfalse;

    while (1) {
        c = p[0];
        if (c == '\0') {
            //Com_Printf("end\n");
            *newLine = qtrue;
            break;
        }
        if (c != '\t'  &&  (c < ' '  ||  c > '~')) {
            *newLine = qtrue;
            if (gotFirstToken) {
                p--;
            }
            break;
        }

        if (c == ' '  ||  c == '\t'  ||  c == '\''  ||  c == '"') {
            if (!gotFirstToken) {
                p++;
                continue;
            } else {
                break;
            }
        }

        if (!isFilename  &&  (c == '/'  ||  c == '-'  ||  c == '+'  ||  c == '*'  ||  c  == '('  ||  c  == ')'  ||  c == ',')) {
            if (gotFirstToken) {
                p--;
                break;
            } else {
                token[0] = c;
                token++;
                break;
            }
        }

        if (c < '!'  ||  c > '~') {
            break;
        }

        if (c == '{'  ||  c == '}') {
            if (gotFirstToken) {
                p--;
                break;
            }
        }

        token[0] = c;
        token++;
        p++;
        gotFirstToken = qtrue;
    }

    p++;
    token[0] = '\0';

#if 0
    if (isFilename) {
        Com_Printf("^6filename token: '%s'\n", start);
    } else {
        Com_Printf("^5token: '%s'\n", start);
    }
#endif

    //Com_Printf("CG_GetToken() '%s'\n", tokenOrig);

    return p;
}


void CG_LStrip (char **cp)
{
    char c;

    while (1) {
        c = *cp[0];
        if (c == '\0') {
            return;
        }
        if (c >= '!'  &&  c <= '~') {
            return;
        }

        (*cp)++;
    }
}


void CG_StripSlashComments (char *s)
{
    char *start;
    int len;
    char c;

    start = s;

    while (1) {
        if (s[0] == '\0') {
            break;
        } else if (s[1] == '\0') {
            break;
        } else if (s[0] == '/'  &&  s[1] == '/') {
            s[0] = '\n';
            s[1] = '\0';
            //return;
            break;
        }

        s++;
    }

    len = strlen(start);
    if (len < 2) {
        return;
    }

    s--;
    while (s > start) {
        c = s[0];

        if (c >= '!'  &&  c <= '~') {
            break;
        }

        s[1] = '\0';
        s[0] = '\n';
        s--;
    }

    //Com_Printf("^3new line %d '%s'\n", strlen(start), start);
}


void CG_CleanString (char *s)
{
    while (1) {
        if (s[0] == '\0') {
            break;
        } else if (s[0] == '\t') {
            s[0] = ' ';
        } else if (s[0] == '\f'  ||  s[0] == '\r'  ||  s[0] == '\n') {
            s[0] = '\n';
        } else if (s[0] < ' '  ||  s[0] > '~') {
            s[0] = ' ';
        }
        s++;
    }
}


void CG_ScaleModel (refEntity_t *re, float size)
{
    re->radius = size;
    VectorScale(re->axis[0], re->radius, re->axis[0]);
    VectorScale(re->axis[1], re->radius, re->axis[1]);
    VectorScale(re->axis[2], re->radius, re->axis[2]);
    re->nonNormalizedAxes = qtrue;
}
