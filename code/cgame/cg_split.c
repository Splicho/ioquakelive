/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "cg_local.h"
#include "../ui/ui_shared.h"
#include "../../ui/menudef.h"

#define SP_MAX_ENTS 2048
#define SP_MAX_POLYS 4096
#define SP_MAX_VERTS 65536
#define SP_MAX_LIGHTS 64

typedef struct {
    refEntity_t ent;
    int owner;
} spEnt_t;

typedef struct {
    qhandle_t shader;
    int numVerts;
    int firstVert;
    int num;
} spPoly_t;

typedef struct {
    vec3_t org;
    float intensity;
    float r, g, b;
    qboolean additive;
} spLight_t;

static spEnt_t spEnts[SP_MAX_ENTS];
static int spNumEnts;
static spPoly_t spPolys[SP_MAX_POLYS];
static int spNumPolys;
static polyVert_t spVerts[SP_MAX_VERTS];
static int spNumVerts;
static spLight_t spLights[SP_MAX_LIGHTS];
static int spNumLights;

static qboolean spRecording;
static qboolean spFrame;
static int spOwner = -1;
static qboolean spOn;
static int spClient = -1;
static int spSent = -2;
static vec3_t spOrigin;
static vec3_t spAngles;
static int spViewHeight;
static qboolean spSeen;
static int spSeenTime;
static qboolean spHadView;

static qboolean CG_SplitAllowed(void) {
    if (!cg.snap) {
        return qfalse;
    }
    if (cg.demoPlayback) {
        return qtrue;
    }
    return cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR ? qtrue : qfalse;
}

static qboolean CG_SplitCandidate(int i, int left) {
    const clientInfo_t* ci = &cgs.clientinfo[i];

    return i != left && ci->infoValid && ci->team != TEAM_SPECTATOR ? qtrue : qfalse;
}

static int CG_SplitLeft(void) {
    if (cg.snap && (cg.snap->ps.pm_flags & PMF_FOLLOW || cg.demoPlayback)) {
        return cg.snap->ps.clientNum;
    }
    return -1;
}

static int CG_SplitPick(int after) {
    int left = CG_SplitLeft();
    int myTeam = left >= 0 ? cgs.clientinfo[left].team : TEAM_FREE;
    int pass, k, i;

    for (pass = 0; pass < 2; pass++) {
        for (k = 1; k <= MAX_CLIENTS; k++) {
            i = (after + k + MAX_CLIENTS) % MAX_CLIENTS;
            if (i >= cgs.maxclients || !CG_SplitCandidate(i, left)) {
                continue;
            }
            if (pass == 0 && cgs.gametype >= GT_TEAM && left >= 0 && cgs.clientinfo[i].team == myTeam) {
                continue;
            }
            return i;
        }
    }
    return -1;
}

static void CG_SplitPublish(void) {
    int want = spOn ? spClient : -1;

    if (want == spSent) {
        return;
    }
    spSent = want;
    trap_Cvar_Set("cg_splitClient", want >= 0 ? va("%i", want) : "");
}

static void CG_SplitSet(int client) {
    spClient = client;
    spSeen = qfalse;
    spHadView = qfalse;
    CG_SplitPublish();
}

void CG_Split_f(void) {
    int client;

    if (trap_Argc() > 1) {
        client = atoi(CG_Argv(1));
        if (client < 0 || client >= cgs.maxclients || !cgs.clientinfo[client].infoValid) {
            CG_Printf("Split screen: no player %i.\n", client);
            return;
        }
        if (!CG_SplitAllowed()) {
            CG_Printf("Split screen works while spectating or watching a demo.\n");
            return;
        }
        spOn = qtrue;
        CG_SplitSet(client);
        return;
    }
    if (spOn) {
        spOn = qfalse;
        CG_SplitSet(-1);
        return;
    }
    if (!CG_SplitAllowed()) {
        CG_Printf("Split screen works while spectating or watching a demo.\n");
        return;
    }
    client = CG_SplitPick(CG_SplitLeft());
    if (client < 0) {
        CG_Printf("Split screen needs a second player.\n");
        return;
    }
    spOn = qtrue;
    CG_SplitSet(client);
}

void CG_SplitNext_f(void) {
    int client;

    if (!spOn) {
        CG_Split_f();
        return;
    }
    client = CG_SplitPick(spClient);
    if (client >= 0) {
        CG_SplitSet(client);
    }
}

qboolean CG_SplitActive(void) {
    return spFrame;
}

void CG_SplitBeginFrame(void) {
    int left;

    spNumEnts = spNumPolys = spNumVerts = spNumLights = 0;
    spOwner = -1;
    spFrame = qfalse;
    spRecording = qfalse;
    if (!spOn) {
        CG_SplitPublish();
        return;
    }
    if (!CG_SplitAllowed()) {
        spOn = qfalse;
        CG_SplitSet(-1);
        return;
    }
    left = CG_SplitLeft();
    if (spClient < 0 || spClient == left || !CG_SplitCandidate(spClient, left)) {
        int next = CG_SplitPick(spClient < 0 ? left : spClient);
        if (next < 0) {
            return;
        }
        CG_SplitSet(next);
    }
    CG_SplitPublish();
    if (cg.snap->ps.pm_type == PM_INTERMISSION) {
        return;
    }
    spFrame = qtrue;
    spRecording = qtrue;
}

void CG_SplitOwner(int owner) {
    spOwner = owner;
}

void CG_SplitRecordEntity(const refEntity_t* re) {
    if (!spRecording || spNumEnts >= SP_MAX_ENTS) {
        return;
    }
    spEnts[spNumEnts].ent = *re;
    spEnts[spNumEnts].owner = spOwner;
    spNumEnts++;
}

void CG_SplitRecordPolys(qhandle_t shader, int numVerts, const polyVert_t* verts, int num) {
    int total = numVerts * num;

    if (!spRecording || spNumPolys >= SP_MAX_POLYS || total <= 0 || spNumVerts + total > SP_MAX_VERTS) {
        return;
    }
    spPolys[spNumPolys].shader = shader;
    spPolys[spNumPolys].numVerts = numVerts;
    spPolys[spNumPolys].firstVert = spNumVerts;
    spPolys[spNumPolys].num = num;
    memcpy(&spVerts[spNumVerts], verts, sizeof(polyVert_t) * total);
    spNumVerts += total;
    spNumPolys++;
}

void CG_SplitRecordLight(const vec3_t org, float intensity, float r, float g, float b, qboolean additive) {
    spLight_t* l;

    if (!spRecording || spNumLights >= SP_MAX_LIGHTS) {
        return;
    }
    l = &spLights[spNumLights++];
    VectorCopy(org, l->org);
    l->intensity = intensity;
    l->r = r;
    l->g = g;
    l->b = b;
    l->additive = additive;
}

void CG_SplitStopRecording(void) {
    spRecording = qfalse;
}

static void CG_SplitTrackSecond(void) {
    centity_t* cent = &cg_entities[spClient];
    int anim;

    spSeen = cent->currentValid && cent->currentState.eType == ET_PLAYER && cent->currentState.clientNum == spClient;
    if (!spSeen) {
        return;
    }
    VectorCopy(cent->lerpOrigin, spOrigin);
    VectorCopy(cent->lerpAngles, spAngles);
    anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
    if (cent->currentState.eFlags & EF_DEAD) {
        spViewHeight = DEAD_VIEWHEIGHT;
    } else if (anim == LEGS_IDLECR || anim == LEGS_WALKCR || anim == LEGS_BACKCR) {
        spViewHeight = CROUCH_VIEWHEIGHT;
    } else {
        spViewHeight = DEFAULT_VIEWHEIGHT;
    }
    spSeenTime = cg.time;
    spHadView = qtrue;
}

static float CG_SplitFovX(float fovY, int width, int height) {
    return atan(tan(DEG2RAD(fovY * 0.5f)) * width / (float)height) * 360.0f / M_PI;
}

qboolean CG_SplitRender(void) {
    refdef_t full, left, right;
    refEntity_t ent;
    vec3_t angles;
    int i, half;

    spRecording = qfalse;
    if (!spFrame) {
        return qfalse;
    }

    full = cg.refdef;
    half = full.width / 2;

    left = full;
    left.width = half;
    left.fov_x = CG_SplitFovX(full.fov_y, left.width, left.height);
    trap_R_RenderScene(&left);

    CG_SplitTrackSecond();
    if (!spHadView) {
        return qtrue;
    }
    right = full;
    right.x = full.x + half;
    right.width = full.width - half;
    right.fov_x = CG_SplitFovX(full.fov_y, right.width, right.height);
    VectorCopy(spOrigin, right.vieworg);
    right.vieworg[2] += spViewHeight;
    VectorCopy(spAngles, angles);
    angles[ROLL] = 0;
    AnglesToAxis(angles, right.viewaxis);
    memset(right.areamask, 0, sizeof(right.areamask));

    trap_R_ClearScene();
    for (i = 0; i < spNumEnts; i++) {
        const spEnt_t* s = &spEnts[i];

        if (s->ent.renderfx & RF_FIRST_PERSON) {
            continue;
        }
        if (s->owner == spClient && s->ent.reType != RT_LIGHTNING && s->ent.reType != RT_BEAM) {
            continue;
        }
        ent = s->ent;
        ent.renderfx &= ~RF_THIRD_PERSON;
        trap_R_AddRefEntityToScene(&ent);
    }
    for (i = 0; i < spNumPolys; i++) {
        const spPoly_t* p = &spPolys[i];

        if (p->num == 1) {
            trap_R_AddPolyToScene(p->shader, p->numVerts, &spVerts[p->firstVert]);
        } else {
            trap_R_AddPolysToScene(p->shader, p->numVerts, &spVerts[p->firstVert], p->num);
        }
    }
    for (i = 0; i < spNumLights; i++) {
        const spLight_t* l = &spLights[i];

        if (l->additive) {
            trap_R_AddAdditiveLightToScene(l->org, l->intensity, l->r, l->g, l->b);
        } else {
            trap_R_AddLightToScene(l->org, l->intensity, l->r, l->g, l->b);
        }
    }
    trap_R_RenderScene(&right);
    return qtrue;
}

static float CG_SplitScreenToVirtual(float sx) {
    float xscale43;

    if (cgs.widescreenBias <= 0.0f) {
        return sx / cgs.screenXScale;
    }
    xscale43 = (4.0f * cgs.glconfig.vidHeight / 3.0f) / 640.0f;
    return (sx - cgs.widescreenBias) / xscale43;
}

static void CG_SplitLabel(int client, float centerX, qboolean seen, int health, int armor, int weapon) {
    static const vec4_t back = {0.0f, 0.0f, 0.0f, 0.55f};
    static const vec4_t healthColor = {1.0f, 1.0f, 1.0f, 1.0f};
    static const vec4_t armorColor = {1.0f, 0.85f, 0.25f, 1.0f};
    const clientInfo_t* ci = &cgs.clientinfo[client];
    char name[MAX_QPATH];
    char hp[16], ap[16];
    float nameW, hpW, apW, total, x, y = 446.0f;
    qhandle_t icon = 0;
    static qhandle_t healthIcon;

    if (!healthIcon) {
        healthIcon = trap_R_RegisterShaderNoMip("icons/iconh_yellow");
    }
    Q_strncpyz(name, ci->name, sizeof(name));
    nameW = CG_DrawTextWidth(name, 0.22f, 0, 1);
    if (seen) {
        Com_sprintf(hp, sizeof(hp), "%i", health > 0 ? health : 0);
        Com_sprintf(ap, sizeof(ap), "%i", armor > 0 ? armor : 0);
        hpW = CG_DrawTextWidth(hp, 0.26f, 0, 1);
        apW = CG_DrawTextWidth(ap, 0.26f, 0, 1);
        if (weapon > WP_NONE && weapon < WP_NUM_WEAPONS) {
            icon = cg_weapons[weapon].weaponIcon;
        }
    } else {
        hpW = apW = 0.0f;
    }
    total = MAX(nameW, hpW + apW + 46.0f + (icon ? 24.0f : 0.0f));
    x = centerX - total * 0.5f - 8.0f;
    CG_FillRect(x, y - 16.0f, total + 16.0f, seen ? 40.0f : 22.0f, back);
    CG_DrawText(centerX - nameW * 0.5f, y, 1, 0.22f, (float*)colorWhite, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
    if (!seen) {
        return;
    }
    x = centerX - (hpW + apW + 46.0f + (icon ? 24.0f : 0.0f)) * 0.5f;
    CG_DrawPic(x, y + 6.0f, 12.0f, 12.0f, healthIcon);
    x += 16.0f;
    CG_DrawText(x, y + 18.0f, 1, 0.26f, (float*)healthColor, hp, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
    x += hpW + 14.0f;
    CG_DrawPic(x, y + 6.0f, 12.0f, 12.0f, cgs.media.armorIcon);
    x += 16.0f;
    CG_DrawText(x, y + 18.0f, 1, 0.26f, (float*)armorColor, ap, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
    x += apW + 10.0f;
    if (icon) {
        CG_DrawPic(x, y + 4.0f, 16.0f, 16.0f, icon);
    }
}

void CG_SplitDraw2D(void) {
    static const vec4_t line = {0.0f, 0.0f, 0.0f, 1.0f};
    static const vec4_t panel = {0.06f, 0.06f, 0.06f, 1.0f};
    float w = (float)cgs.glconfig.vidWidth;
    int left = CG_SplitLeft();
    const centity_t* cent;

    CG_SetWidescreen(WIDESCREEN_STRETCH);
    if (!spHadView) {
        trap_R_SetColor(panel);
        trap_R_DrawStretchPic(cg.refdef.x + cg.refdef.width / 2, cg.refdef.y, cg.refdef.width - cg.refdef.width / 2,
                              cg.refdef.height, 0, 0, 0, 0, cgs.media.whiteShader);
    }
    trap_R_SetColor(line);
    trap_R_DrawStretchPic(cg.refdef.x + cg.refdef.width / 2 - 1, cg.refdef.y, 2, cg.refdef.height, 0, 0, 0, 0,
                          cgs.media.whiteShader);
    trap_R_SetColor(NULL);

    CG_DrawSplitCrosshairs(spHadView);

    CG_SetWidescreen(WIDESCREEN_CENTER);
    if (left >= 0) {
        CG_SplitLabel(left, CG_SplitScreenToVirtual(w * 0.25f), qtrue, cg.snap->ps.stats[STAT_HEALTH],
                      cg.snap->ps.stats[STAT_ARMOR], cg.snap->ps.weapon);
    }
    if (spClient >= 0) {
        cent = &cg_entities[spClient];
        CG_SplitLabel(spClient, CG_SplitScreenToVirtual(w * 0.75f), spSeen, (short)cent->currentState.health,
                      (short)cent->currentState.armor, cent->currentState.weapon);
    }
    CG_SetWidescreen(WIDESCREEN_STRETCH);
    CG_DrawChat(5, 287, 634, 120, 1.0f);
}
