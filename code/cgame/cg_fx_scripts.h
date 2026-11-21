/*
===========================================================================
Portions derived from WolfcamQL's cg_fx_scripts.h.
WolfcamQL is distributed under the GNU General Public License v2.
See COPYING.txt for the full license.
===========================================================================
*/

#ifndef cg_fx_scripts_h_included
#define cg_fx_scripts_h_included

#define MAX_SCRIPTED_VARS_VECTOR_STACK 64
#define MAX_FX_SOUND_LIST 32
#define MAX_FX_MODEL_LIST 32
#define MAX_FX_SHADER_LIST 32

typedef struct positionData_s {
    vec3_t intervalPosition;
    double intervalTime;

    vec3_t distancePosition;
    double distanceTime;
} positionData_t;

typedef struct ScriptVars_s {
    float size;
    float width;
    float angle;
    float t0;
    float t1;
    float t2;
    float t3;

    float t4;
    float t5;
    float t6;
    float t7;
    float t8;
    float t9;

    float rotate;
    float vibrate;

    vec3_t origin;
    vec3_t end;
    vec3_t velocity;
    vec3_t dir;
    vec3_t angles;
    vec3_t axis[3];

    float time;
    float loop;
    int loopCount;

    float rand;
    float crand;
    float lerp;
    float life;

    vec3_t v0, v1, v2, v3;
    vec3_t v4, v5, v6, v7, v8, v9;

    vec3_t tmpVector;

    vec4_t color;

    vec3_t color1;
    vec3_t color2;

    int team;
    int clientNum;
    qboolean enemy;
    qboolean teamMate;
    qboolean inEyes;
    int powerups;

    int surfaceType;

    char shader[MAX_QPATH];
    char model[MAX_QPATH];
    char loopSound[MAX_QPATH];
    char sound[MAX_QPATH];

    float animFrame;

    struct centity_s* parentCent;
    vec3_t parentOrigin;
    vec3_t parentVelocity;
    vec3_t parentAngles;
    vec3_t parentDir;
    vec3_t parentEnd;
    int parentSize;
    float parentAngle;

    vec3_t vecStack[MAX_SCRIPTED_VARS_VECTOR_STACK];
    int vecStackCount;

    qboolean hasAlphaFade;
    float alphaFade;
    qboolean hasColorFade;
    float colorFade;
    qboolean hasMoveGravity;
    float moveGravity;
    qboolean hasShaderTime;
    float shaderTime;

    qboolean firstPerson;
    qboolean thirdPerson;
    qboolean shadow;
    qboolean cullNear;
    qboolean cullRadius;
    qboolean cullDistance;
    float cullDistanceValue;
    qboolean depthHack;
    qboolean stencil;

    qboolean decalEnergy;
    qboolean decalAlpha;

    char soundList[MAX_FX_SOUND_LIST][MAX_QPATH];
    int numSoundList;
    int selectedSoundList;

    char modelList[MAX_FX_MODEL_LIST][MAX_QPATH];
    int numModelList;
    int selectedModelList;

    char shaderList[MAX_FX_SHADER_LIST][MAX_QPATH];
    int numShaderList;
    int selectedShaderList;

    qboolean hasMoveBounce;
    float moveBounce1;
    float moveBounce2;
    qboolean hasSink;
    float sink1;
    float sink2;
    float emitterTime;
    qboolean emitterFullLerp;
    qboolean emitterKill;

    float distance;

    qboolean spin;
    vec3_t lastIntervalPosition;
    double lastIntervalTime;
    vec3_t lastDistancePosition;
    double lastDistanceTime;

    const char* emitterScriptStart;
    const char* emitterScriptEnd;

    vec3_t impactOrigin;
    vec3_t impactDir;
    qboolean impacted;

    float emitterId;
} ScriptVars_t;

typedef enum {
    FX_MODEL_DIR,
    FX_MODEL_ANGLES,
    FX_MODEL_AXIS
} fxModelType_t;

typedef enum {
    LEFX_NONE,
    LEFX_EMIT,
    LEFX_EMIT_LIGHT,
    LEFX_EMIT_SOUND,
    LEFX_EMIT_LOOPSOUND,
    LEFX_SCRIPT
} leFXType_t;

extern ScriptVars_t ScriptVars;
extern ScriptVars_t EmitterScriptVars;
extern qboolean EmitterScript;
extern qboolean DistanceScript;
extern qboolean PlainScript;
extern int FxLoopSounds;

extern qboolean DebugInterval;

void CG_ResetScriptVars(void);
struct localEntity_s;
void CG_GetStoredScriptVarsFromLE(const struct localEntity_s* le);
void CG_FX_ModelAxis(refEntity_t* re, fxModelType_t ftype);
void CG_CopyPlayerDataToScriptData(struct centity_s* cent);
void CG_CopyStaticScriptDataToCent(struct centity_s* cent);
void CG_CopyStaticCentDataToScript(struct centity_s* cent);

void CG_CopyPositionDataToCent(positionData_t* pd);
void CG_CopyPositionDataToScript(const positionData_t* pd);
void CG_UpdatePositionData(const struct centity_s* cent, positionData_t* pd);
void CG_ResetFXIntervalAndDistance(struct centity_s* cent);

#ifndef WP_MAX_NUM_WEAPONS_ALL_PROTOCOLS
#define WP_MAX_NUM_WEAPONS_ALL_PROTOCOLS 128
#endif

#define MAX_FX_SCRIPT_SIZE (1024 * 32)
#define MAX_FX_EXTRA 128
#define MAX_FX 256

typedef struct weaponEffects_s {
    qboolean hasFireScript;
    qboolean hasFlashScript;
    qboolean hasProjectileScript;
    qboolean hasTrailScript;
    qboolean hasImpactScript;
    qboolean hasImpactFleshScript;

    char fireScript[MAX_FX_SCRIPT_SIZE];
    char flashScript[MAX_FX_SCRIPT_SIZE];
    char projectileScript[MAX_FX_SCRIPT_SIZE];
    char trailScript[MAX_FX_SCRIPT_SIZE];
    char impactScript[MAX_FX_SCRIPT_SIZE];
    char impactFleshScript[MAX_FX_SCRIPT_SIZE];
} weaponEffects_t;

// A bounded hash table avoids indexing a huge array by script byte offset.
#define FX_TOKEN_CACHE_BUCKETS 4096

typedef struct jitFxToken_s {
    struct jitFxToken_s* next;
    const char* start;
    qboolean isFilename;
    qboolean valid;
    const char* end;
    qboolean hitNewLine;
    char token[MAX_QPATH];
    int tokenId;
    float value;
} jitFxToken_t;

typedef struct effectScripts_s {
    weaponEffects_t weapons[WP_MAX_NUM_WEAPONS_ALL_PROTOCOLS];

    char playerTalk[MAX_FX_SCRIPT_SIZE];
    char playerConnection[MAX_FX_SCRIPT_SIZE];
    char playerImpressive[MAX_FX_SCRIPT_SIZE];
    char playerExcellent[MAX_FX_SCRIPT_SIZE];
    char playerHolyshit[MAX_FX_SCRIPT_SIZE];
    char playerAccuracy[MAX_FX_SCRIPT_SIZE];
    char playerGauntlet[MAX_FX_SCRIPT_SIZE];

    char playerMedalComboKill[MAX_FX_SCRIPT_SIZE];
    char playerMedalMidAir[MAX_FX_SCRIPT_SIZE];
    char playerMedalRevenge[MAX_FX_SCRIPT_SIZE];
    char playerMedalFirstFrag[MAX_FX_SCRIPT_SIZE];
    char playerMedalRampage[MAX_FX_SCRIPT_SIZE];
    char playerMedalPerforated[MAX_FX_SCRIPT_SIZE];
    char playerMedalAccuracy[MAX_FX_SCRIPT_SIZE];
    char playerMedalHeadshot[MAX_FX_SCRIPT_SIZE];
    char playerMedalPerfect[MAX_FX_SCRIPT_SIZE];
    char playerMedalQuadGod[MAX_FX_SCRIPT_SIZE];

    char playerHaste[MAX_FX_SCRIPT_SIZE];
    char playerDefend[MAX_FX_SCRIPT_SIZE];
    char playerAssist[MAX_FX_SCRIPT_SIZE];
    char playerCapture[MAX_FX_SCRIPT_SIZE];
    char playerQuad[MAX_FX_SCRIPT_SIZE];
    char playerFlight[MAX_FX_SCRIPT_SIZE];
    char playerFriend[MAX_FX_SCRIPT_SIZE];
    char playerHeadTrail[MAX_FX_SCRIPT_SIZE];
    char playerTorsoTrail[MAX_FX_SCRIPT_SIZE];
    char playerLegsTrail[MAX_FX_SCRIPT_SIZE];

    char playerTeleportIn[MAX_FX_SCRIPT_SIZE];
    char playerTeleportOut[MAX_FX_SCRIPT_SIZE];

    char jumpPad[MAX_FX_SCRIPT_SIZE];
    char headShot[MAX_FX_SCRIPT_SIZE];

    char bubbles[MAX_FX_SCRIPT_SIZE];
    char gibbed[MAX_FX_SCRIPT_SIZE];
    char thawed[MAX_FX_SCRIPT_SIZE];
    char impactFlesh[MAX_FX_SCRIPT_SIZE];

    char extra[MAX_FX_EXTRA][MAX_FX_SCRIPT_SIZE];
    int numExtra;

    char names[MAX_FX][MAX_QPATH];
    char* ptr[MAX_FX];
    int numEffects;

    jitFxToken_t* jitToken[FX_TOKEN_CACHE_BUCKETS];
} effectScripts_t;

extern effectScripts_t EffectScripts;

const char* CG_Q3mmeMath(const char* script, float* val, int* error);
qboolean CG_RunQ3mmeScript(const char* script, const char* emitterEnd);
void CG_ReloadQ3mmeScripts(const char* fileName);
void CG_FreeFxJitTokens(void);

#endif // cg_fx_scripts_h_included
