#ifndef cg_fx_compat_h_included
#define cg_fx_compat_h_included

#include "cg_local.h"

#ifndef PW_ARMORREGEN
#define PW_ARMORREGEN PW_AMMOREGEN
#endif
#ifndef PW_FROZEN
#define PW_FROZEN PW_FREEZE
#endif
#ifndef PWEX_SPAWNPROTECTION
#define PWEX_SPAWNPROTECTION 0x40000000
#endif

#ifndef RT_MODEL_FX_DIR
#define RT_MODEL_FX_DIR RT_MODEL
#endif
#ifndef RT_MODEL_FX_ANGLES
#define RT_MODEL_FX_ANGLES RT_MODEL
#endif
#ifndef RT_MODEL_FX_AXIS
#define RT_MODEL_FX_AXIS RT_MODEL
#endif
#ifndef WP_HEAVY_MACHINEGUN
#define WP_HEAVY_MACHINEGUN WP_MACHINEGUN
#endif
#ifndef MAX_LOOP_SOUNDS
#define MAX_LOOP_SOUNDS 256
#endif

const char* String_Alloc(const char* p);
qboolean trap_Cvar_Exists(const char* name);
void trap_autoWriteConfig(qboolean enable);
void trap_SendConsoleCommandNow(const char* text);
void CG_StartSound(const vec3_t origin, int entityNum, int channel, sfxHandle_t sfx);
void CG_StartLocalSound(sfxHandle_t sfx, int channel);
void CG_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);

void CG_MakeLowPriorityEntity(localEntity_t* le);
void CG_AddRefEntityFX(const refEntity_t* re);
void CG_RemoveFXLocalEntities(qboolean all, float emitterId);
extern int FxLoopSounds;

void SC_Lowercase(char* p);
int SC_Cvar_Get_Int(const char* cvar);
float SC_Cvar_Get_Float(const char* cvar);
const char* SC_Cvar_Get_String(const char* cvar);

char* CG_FS_ReadLine(fileHandle_t f, int* len);
const char* CG_GetToken(const char* inputString, char* token, qboolean isFilename, qboolean* newLine);
void CG_LStrip(char** cp);
void CG_StripSlashComments(char* s);
void CG_CleanString(char* s);
void CG_Abort(void);
void CG_ScaleModel(refEntity_t* re, float size);

qboolean CG_IsEnemy(const clientInfo_t* ci);
qboolean CG_IsTeammate(const clientInfo_t* ci);
qboolean CG_IsUs(const clientInfo_t* ci);
qboolean CG_IsFirstPersonView(int clientNum);
qboolean CG_IsTeamGame(int gametype);
qboolean CG_IsFlagGame(int gametype);
qboolean CG_IsDuelGame(int gametype);

extern vmCvar_t cg_fxCompiled;
extern vmCvar_t cg_fxDebug;
extern vmCvar_t cg_fxScriptDebug;

void CG_FX_LogOpen(qboolean truncate);
void CG_FX_LogClose(void);
void CG_FX_Log(const char* fmt, ...);
void CG_FXLog_f(void);
extern vmCvar_t cg_fxfile;
extern vmCvar_t cg_fxinterval;
extern vmCvar_t cg_fxScriptMinDistance;
extern vmCvar_t cg_fxScriptMinEmitter;
extern vmCvar_t cg_fxScriptMinInterval;
extern vmCvar_t cg_vibrateMaxDistance;

void CG_FX_RegisterCommands(void);
qboolean CG_FX_Command(const char* cmd);
void CG_FX_GibPlayer(struct centity_s* cent);
qboolean CG_FX_RunScript(const char* script, struct centity_s* cent, const vec3_t origin, const vec3_t dir,
                         const vec3_t velocity);
qboolean CG_FX_RunImpactScript(const char* script, struct centity_s* cent, const vec3_t origin, const vec3_t dir);
qboolean CG_FX_RunRailScript(const char* script, const clientInfo_t* ci, const vec3_t start, const vec3_t end);
void CG_FX_RunPlayerScript(const char* script, struct centity_s* cent, const vec3_t origin, int animFrame);
void Vector4Set(vec4_t v, float x, float y, float z, float w);
void CG_FreeLocalEntity(struct localEntity_s* le);

#endif // cg_fx_compat_h_included
