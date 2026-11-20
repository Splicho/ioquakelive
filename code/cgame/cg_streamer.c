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

#define CS_JSON_MAX (256 * 1024)
#define CS_DEPTH 16

static char csBuf[CS_JSON_MAX];
static int csLen;
static qboolean csOverflow;
static int csDepth;
static qboolean csFirst[CS_DEPTH];

static qboolean csActive;
static int csCheckTime;
static char csLastState[16];
static int csLastRoundStart;
static int csLastWarmupRound;
static qboolean csSeen;
static qboolean csCountdownSeen;

static const char* const csGametypes[] = {"ffa", "duel", "race", "tdm", "ca", "ctf", "1fctf", "ob", "har", "ft", "dom", "ad", "rr"};
static const char* const csWeapons[] = {"none", "gauntlet", "mg", "sg", "gl", "rl", "lg", "rg", "pg", "bfg", "gh", "ng", "pl", "cg", "hmg"};
static const char* const csPowerups[] = {"none", "redflag", "blueflag", "neutralflag", "quad", "battlesuit", "haste", "invis",
                                         "regen", "flight", "invulnerability", "scout", "guard", "doubler", "armorregen", "freeze"};
static const char* const csMods[] = {"unknown", "shotgun", "gauntlet", "machinegun", "grenade", "grenade_splash", "rocket",
                                     "rocket_splash", "plasma", "plasma_splash", "railgun", "lightning", "bfg", "bfg_splash",
                                     "water", "slime", "lava", "crush", "telefrag", "falling", "suicide", "target_laser",
                                     "trigger_hurt", "nail", "chaingun", "proximity_mine", "kamikaze", "juiced", "grapple",
                                     "switch_teams", "thaw", "lightning_discharge", "hmg", "railgun_headshot"};

static void CS_Raw(const char* s, int len) {
    if (csOverflow) {
        return;
    }
    if (csLen + len >= CS_JSON_MAX) {
        csOverflow = qtrue;
        return;
    }
    memcpy(csBuf + csLen, s, len);
    csLen += len;
    csBuf[csLen] = '\0';
}

static void CS_Text(const char* s) {
    CS_Raw(s, (int)strlen(s));
}

static void CS_Quoted(const char* s) {
    char esc[8];
    const unsigned char* p;

    CS_Raw("\"", 1);
    for (p = (const unsigned char*)s; *p; p++) {
        if (*p == '"' || *p == '\\') {
            esc[0] = '\\';
            esc[1] = (char)*p;
            CS_Raw(esc, 2);
        } else if (*p < 0x20 || *p >= 0x7f) {
            Com_sprintf(esc, sizeof(esc), "\\u%04x", *p);
            CS_Raw(esc, 6);
        } else {
            CS_Raw((const char*)p, 1);
        }
    }
    CS_Raw("\"", 1);
}

static void CS_Key(const char* key) {
    if (!csFirst[csDepth]) {
        CS_Raw(",", 1);
    }
    csFirst[csDepth] = qfalse;
    if (key) {
        CS_Quoted(key);
        CS_Raw(":", 1);
    }
}

static void CS_Open(const char* key, char c) {
    CS_Key(key);
    CS_Raw(&c, 1);
    if (csDepth < CS_DEPTH - 1) {
        csDepth++;
    }
    csFirst[csDepth] = qtrue;
}

static void CS_Close(char c) {
    if (csDepth > 0) {
        csDepth--;
    }
    CS_Raw(&c, 1);
}

static void CS_Int(const char* key, int v) {
    CS_Key(key);
    CS_Text(va("%i", v));
}

static void CS_Bool(const char* key, qboolean v) {
    CS_Key(key);
    CS_Text(v ? "true" : "false");
}

static void CS_Null(const char* key) {
    CS_Key(key);
    CS_Text("null");
}

static void CS_String(const char* key, const char* v) {
    CS_Key(key);
    CS_Quoted(v ? v : "");
}

static void CS_Vec(const char* key, const vec3_t v) {
    CS_Open(key, '[');
    CS_Int(NULL, (int)v[0]);
    CS_Int(NULL, (int)v[1]);
    CS_Int(NULL, (int)v[2]);
    CS_Close(']');
}

static void CS_Begin(void) {
    csLen = 0;
    csBuf[0] = '\0';
    csOverflow = qfalse;
    csDepth = 0;
    csFirst[0] = qtrue;
}

static void CS_Send(const char* type) {
    if (!csOverflow && csLen > 0) {
        trap_StreamerPublish(type, csBuf);
    }
}

static const char* CS_Gametype(void) {
    if (cgs.gametype >= 0 && cgs.gametype < (int)ARRAY_LEN(csGametypes)) {
        return csGametypes[cgs.gametype];
    }
    return "unknown";
}

static const char* CS_Weapon(int w) {
    if (w >= 0 && w < (int)ARRAY_LEN(csWeapons)) {
        return csWeapons[w];
    }
    return "none";
}

static const char* CS_Team(int team) {
    switch (team) {
        case TEAM_RED:
            return "red";
        case TEAM_BLUE:
            return "blue";
        case TEAM_SPECTATOR:
            return "spectator";
        default:
            return "free";
    }
}

static int CS_ModWeapon(int mod) {
    switch (mod) {
        case MOD_SHOTGUN:
            return WP_SHOTGUN;
        case MOD_GAUNTLET:
            return WP_GAUNTLET;
        case MOD_MACHINEGUN:
            return WP_MACHINEGUN;
        case MOD_GRENADE:
        case MOD_GRENADE_SPLASH:
            return WP_GRENADE_LAUNCHER;
        case MOD_ROCKET:
        case MOD_ROCKET_SPLASH:
            return WP_ROCKET_LAUNCHER;
        case MOD_PLASMA:
        case MOD_PLASMA_SPLASH:
            return WP_PLASMAGUN;
        case MOD_RAILGUN:
        case MOD_RAILGUN_HEADSHOT:
            return WP_RAILGUN;
        case MOD_LIGHTNING:
        case MOD_LIGHTNING_DISCHARGE:
            return WP_LIGHTNING;
        case MOD_BFG:
        case MOD_BFG_SPLASH:
            return WP_BFG;
        case MOD_NAIL:
            return WP_NAILGUN;
        case MOD_CHAINGUN:
            return WP_CHAINGUN;
        case MOD_PROXIMITY_MINE:
            return WP_PROX_LAUNCHER;
        case MOD_GRAPPLE:
            return WP_GRAPPLING_HOOK;
        case MOD_HMG:
            return WP_HMG;
        default:
            return WP_NONE;
    }
}

static void CS_Name(const char* keyName, const char* keyClean, const char* name) {
    char clean[MAX_STRING_CHARS];

    Q_strncpyz(clean, name, sizeof(clean));
    Q_CleanStr(clean);
    CS_String(keyName, name);
    CS_String(keyClean, clean);
}

static const char* CS_MatchState(void) {
    int warmup;

    if (!cg.snap) {
        return "loading";
    }
    if (cg.intermissionStarted || cg.snap->ps.pm_type == PM_INTERMISSION) {
        return "intermission";
    }
    warmup = cg.warmup;
    if (warmup < 0) {
        return "warmup";
    }
    if (warmup > 0) {
        return "countdown";
    }
    return "live";
}

static const score_t* CS_Score(int client) {
    int i;

    for (i = 0; i < cg.numScores && i < MAX_CLIENTS; i++) {
        if (cg.scores[i].client == client) {
            return &cg.scores[i];
        }
    }
    return NULL;
}

static void CS_PowerupMask(const char* key, int mask) {
    int i;

    CS_Open(key, '[');
    for (i = 1; i < PW_NUM_POWERUPS && i < (int)ARRAY_LEN(csPowerups); i++) {
        if (mask & (1 << i)) {
            CS_String(NULL, csPowerups[i]);
        }
    }
    CS_Close(']');
}

static void CS_Pov(const playerState_t* ps) {
    int i, left, held;
    float speed;

    CS_Open("pov", '{');
    CS_Int("id", ps->clientNum);
    CS_Bool("following", (ps->pm_flags & PMF_FOLLOW) ? qtrue : qfalse);
    CS_Int("health", ps->stats[STAT_HEALTH]);
    CS_Int("armor", ps->stats[STAT_ARMOR]);
    CS_String("weapon", CS_Weapon(ps->weapon));
    CS_Open("weapons", '[');
    for (i = WP_GAUNTLET; i < WP_NUM_WEAPONS; i++) {
        if (ps->stats[STAT_WEAPONS] & (1 << i)) {
            CS_String(NULL, CS_Weapon(i));
        }
    }
    CS_Close(']');
    CS_Open("ammo", '{');
    for (i = WP_GAUNTLET; i < WP_NUM_WEAPONS; i++) {
        if (ps->stats[STAT_WEAPONS] & (1 << i)) {
            CS_Int(CS_Weapon(i), ps->ammo[i]);
        }
    }
    CS_Close('}');
    CS_Open("powerups", '{');
    for (i = 1; i < PW_NUM_POWERUPS && i < (int)ARRAY_LEN(csPowerups); i++) {
        if (ps->powerups[i]) {
            left = ps->powerups[i] - cg.time;
            CS_Int(csPowerups[i], (left < 0 || left > 3600000) ? -1 : left);
        }
    }
    CS_Close('}');
    held = ps->stats[STAT_HOLDABLE_ITEM];
    if (held > 0 && held < bg_numItems && bg_itemlist[held].classname) {
        const char* name = bg_itemlist[held].classname;
        if (!Q_stricmpn(name, "holdable_", 9)) {
            name += 9;
        }
        CS_String("holdable", name);
    } else {
        CS_Null("holdable");
    }
    speed = sqrt(ps->velocity[0] * ps->velocity[0] + ps->velocity[1] * ps->velocity[1]);
    CS_Int("speed", (int)speed);
    CS_Vec("origin", ps->origin);
    CS_Vec("angles", ps->viewangles);
    CS_Close('}');
}

static void CS_Player(int i, const playerState_t* ps) {
    const clientInfo_t* ci = &cgs.clientinfo[i];
    const score_t* sc = CS_Score(i);
    const centity_t* cent = &cg_entities[i];
    qboolean visible = cent->currentValid && cent->currentState.eType == ET_PLAYER;

    CS_Open(NULL, '{');
    CS_Int("id", i);
    CS_Name("name", "cleanName", ci->name);
    CS_Name("clan", "cleanClan", ci->clanTag);
    CS_String("country", ci->country);
    CS_String("team", CS_Team(ci->team));
    CS_Bool("bot", ci->botSkill > 0 ? qtrue : qfalse);
    CS_Bool("ready", ci->ready);
    CS_Bool("followed", ps->clientNum == i ? qtrue : qfalse);
    CS_Bool("visible", (visible || ps->clientNum == i) ? qtrue : qfalse);
    if (ps->clientNum == i) {
        CS_String("source", "pov");
        CS_Bool("alive", ps->stats[STAT_HEALTH] > 0 ? qtrue : qfalse);
        CS_Int("health", ps->stats[STAT_HEALTH]);
        CS_Int("armor", ps->stats[STAT_ARMOR]);
        CS_String("weapon", CS_Weapon(ps->weapon));
        CS_Vec("origin", ps->origin);
    } else if (visible) {
        CS_String("source", "view");
        CS_Bool("alive", !(cent->currentState.eFlags & EF_DEAD) && (short)cent->currentState.health > 0 ? qtrue : qfalse);
        CS_Int("health", (short)cent->currentState.health);
        CS_Int("armor", (short)cent->currentState.armor);
        CS_String("weapon", CS_Weapon(cent->currentState.weapon));
        CS_Vec("origin", cent->lerpOrigin);
    } else if ((ci->team == TEAM_RED || ci->team == TEAM_BLUE) && ci->team == ps->persistant[PERS_TEAM] &&
               (ci->health > 0 || ci->armor > 0)) {
        CS_String("source", "team");
        CS_Bool("alive", qtrue);
        CS_Int("health", ci->health);
        CS_Int("armor", ci->armor);
        CS_String("weapon", CS_Weapon(ci->curWeapon));
        CS_Null("origin");
    } else {
        CS_Null("source");
        if (sc) {
            CS_Bool("alive", sc->alive);
        } else {
            CS_Null("alive");
        }
        CS_Null("health");
        CS_Null("armor");
        CS_Null("weapon");
        CS_Null("origin");
    }
    if (visible) {
        CS_PowerupMask("powerups", cent->currentState.powerups);
    } else if (ps->clientNum == i) {
        int k, mask = 0;
        for (k = 1; k < PW_NUM_POWERUPS; k++) {
            if (ps->powerups[k]) {
                mask |= 1 << k;
            }
        }
        CS_PowerupMask("powerups", mask);
    } else {
        CS_PowerupMask("powerups", ci->powerups);
    }
    if (sc) {
        CS_Int("score", sc->score);
        CS_Int("roundScore", sc->roundScore);
        CS_Int("kills", sc->frags);
        CS_Int("deaths", sc->deaths);
        CS_Int("net", sc->net);
        CS_Int("damageDone", sc->damageDone);
        CS_Int("damageTaken", sc->damageReceived);
        CS_Int("accuracy", sc->accuracy);
        CS_Int("ping", sc->ping);
        CS_Int("time", sc->time);
        CS_Int("captures", sc->captures);
        CS_Int("assists", sc->assistCount);
        CS_Int("defends", sc->defendCount);
        CS_Int("impressives", sc->impressiveCount);
        CS_Int("excellents", sc->excellentCount);
        CS_Int("humiliations", sc->guantletCount);
        CS_Int("thaws", sc->thaws);
        CS_String("bestWeapon", CS_Weapon(sc->bestWeapon));
    } else {
        CS_Int("score", ci->score);
    }
    CS_Close('}');
}

void CG_StreamerState(void) {
    const playerState_t* ps;
    const char* info;
    char map[MAX_QPATH];
    int i;

    if (!cg.snap) {
        return;
    }
    ps = &cg.snap->ps;
    COM_StripExtension(COM_SkipPath(cgs.mapname), map, sizeof(map));

    CS_Begin();
    CS_Open(NULL, '{');
    CS_Int("version", 1);
    CS_Bool("connected", qtrue);
    CS_Bool("demo", cg.demoPlayback);
    CS_Int("time", cg.time);
    CS_String("map", map);
    CS_String("gametype", CS_Gametype());
    CS_Int("clientNum", cg.clientNum);
    CS_Bool("spectator", (ps->pm_flags & PMF_FOLLOW) || ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ? qtrue : qfalse);
    CS_String("state", CS_MatchState());
    CS_Bool("paused", cgs.pauseEnd ? qtrue : qfalse);
    CS_Int("pauseEnd", cgs.pauseEnd);
    CS_Int("levelStartTime", cgs.levelStartTime);
    CS_Int("elapsed", cg.time - cgs.levelStartTime);

    CS_Open("limits", '{');
    CS_Int("time", cgs.timelimit);
    CS_Int("frag", cgs.fraglimit);
    CS_Int("capture", cgs.capturelimit);
    CS_Int("score", cgs.scorelimit);
    CS_Int("round", cgs.roundlimit);
    CS_Int("roundTime", cgs.roundtimelimit);
    CS_Int("mercy", cgs.mercylimit);
    CS_Close('}');

    info = CG_ConfigString(CS_ROUND_WARMUP);
    CS_Open("round", '{');
    CS_Int("number", atoi(Info_ValueForKey(info, "round")));
    CS_Int("countdownEnd", atoi(Info_ValueForKey(info, "time")));
    CS_Int("startTime", atoi(CG_ConfigString(CS_ROUND_START_TIME)));
    CS_Close('}');

    CS_Open("teams", '{');
    CS_Open("red", '{');
    CS_Int("score", cgs.scores1);
    CS_Int("players", cgs.teamCountRed);
    CS_Int("alive", cgs.teamAliveCount[TEAM_RED]);
    CS_Close('}');
    CS_Open("blue", '{');
    CS_Int("score", cgs.scores2);
    CS_Int("players", cgs.teamCountBlue);
    CS_Int("alive", cgs.teamAliveCount[TEAM_BLUE]);
    CS_Close('}');
    CS_Close('}');

    CS_Open("leaders", '[');
    CS_Open(NULL, '{');
    CS_Int("id", cgs.clientNum1stPlayer);
    CS_Name("name", "cleanName", cgs.name1stPlayer);
    CS_Int("score", cgs.score1stPlayer);
    CS_Close('}');
    CS_Open(NULL, '{');
    CS_Int("id", cgs.clientNum2ndPlayer);
    CS_Name("name", "cleanName", cgs.name2ndPlayer);
    CS_Int("score", cgs.score2ndPlayer);
    CS_Close('}');
    CS_Close(']');

    CS_Pov(ps);

    CS_Open("players", '[');
    for (i = 0; i < MAX_CLIENTS && i < cgs.maxclients; i++) {
        if (cgs.clientinfo[i].infoValid) {
            CS_Player(i, ps);
        }
    }
    CS_Close(']');
    CS_Close('}');
    CS_Send("state");
}

static void CS_Event(const char* type) {
    CS_Send(type);
}

void CG_StreamerFrag(const entityState_t* es) {
    int target, attacker, mod, weapon;
    const char* modName;

    if (!csActive || cg.seeking) {
        return;
    }
    target = es->otherEntityNum;
    attacker = es->otherEntityNum2;
    mod = es->eventParm;
    if (target < 0 || target >= MAX_CLIENTS) {
        return;
    }
    if (attacker < 0 || attacker >= MAX_CLIENTS) {
        attacker = -1;
    }
    modName = (mod >= 0 && mod < (int)ARRAY_LEN(csMods)) ? csMods[mod] : "unknown";
    weapon = CS_ModWeapon(mod);

    CS_Begin();
    CS_Open(NULL, '{');
    CS_Int("time", cg.time);
    CS_Int("killer", attacker);
    if (attacker >= 0) {
        CS_Name("killerName", "killerCleanName", cgs.clientinfo[attacker].name);
        CS_String("killerTeam", CS_Team(cgs.clientinfo[attacker].team));
    } else {
        CS_Null("killerName");
        CS_Null("killerCleanName");
        CS_Null("killerTeam");
    }
    CS_Int("victim", target);
    CS_Name("victimName", "victimCleanName", cgs.clientinfo[target].name);
    CS_String("victimTeam", CS_Team(cgs.clientinfo[target].team));
    CS_String("mod", modName);
    if (weapon != WP_NONE) {
        CS_String("weapon", CS_Weapon(weapon));
    } else {
        CS_Null("weapon");
    }
    CS_Bool("suicide", attacker < 0 || attacker == target ? qtrue : qfalse);
    CS_Bool("teamkill", attacker >= 0 && attacker != target && cgs.gametype >= GT_TEAM &&
                            cgs.clientinfo[attacker].team == cgs.clientinfo[target].team
                        ? qtrue
                        : qfalse);
    CS_Close('}');
    CS_Event("frag");
}

void CG_StreamerTeamSound(int sound) {
    const char* winner = NULL;
    const char* type = "round_end";

    if (!csActive || cg.seeking) {
        return;
    }
    switch (sound) {
        case GTS_RED_WINS_ROUND:
            winner = "red";
            break;
        case GTS_BLUE_WINS_ROUND:
            winner = "blue";
            break;
        case GTS_DRAW_ROUND:
            winner = "draw";
            break;
        case GTS_REDTEAM_WON:
            winner = "red";
            type = "match_winner";
            break;
        case GTS_BLUETEAM_WON:
            winner = "blue";
            type = "match_winner";
            break;
        default:
            return;
    }
    CS_Begin();
    CS_Open(NULL, '{');
    CS_Int("time", cg.time);
    CS_Int("round", atoi(Info_ValueForKey(CG_ConfigString(CS_ROUND_WARMUP), "round")));
    CS_String("winner", winner);
    CS_Int("red", cgs.scores1);
    CS_Int("blue", cgs.scores2);
    CS_Close('}');
    CS_Event(type);
}

void CG_StreamerChat(int sender, qboolean team, const char* text) {
    if (!csActive || cg.seeking) {
        return;
    }
    CS_Begin();
    CS_Open(NULL, '{');
    CS_Int("time", cg.time);
    CS_Int("from", (sender >= 0 && sender < MAX_CLIENTS) ? sender : -1);
    CS_Bool("team", team);
    CS_Name("text", "cleanText", text);
    CS_Close('}');
    CS_Event("chat");
}

void CG_StreamerFrame(void) {
    const char* state;
    const char* info;
    int warmupRound;
    int roundStart;

    if (cg.time < csCheckTime || cg.time - csCheckTime >= 250) {
        char buf[16];
        csCheckTime = cg.time;
        trap_Cvar_VariableStringBuffer("cl_streamerClients", buf, sizeof(buf));
        csActive = atoi(buf) > 0 ? qtrue : qfalse;
    }
    if (!csActive || !cg.snap || cg.seeking) {
        csSeen = qfalse;
        return;
    }

    if (!cg.demoPlayback && cg.scoresRequestTime + 2000 < cg.time) {
        cg.scoresRequestTime = cg.time;
        trap_SendClientCommand("score");
    }

    state = CS_MatchState();
    info = CG_ConfigString(CS_ROUND_WARMUP);
    warmupRound = atoi(Info_ValueForKey(info, "round"));
    if (!csSeen) {
        csSeen = qtrue;
        Q_strncpyz(csLastState, state, sizeof(csLastState));
        csLastRoundStart = atoi(CG_ConfigString(CS_ROUND_START_TIME));
        csLastWarmupRound = warmupRound;
        return;
    }
    if (strcmp(state, csLastState)) {
        CS_Begin();
        CS_Open(NULL, '{');
        CS_Int("time", cg.time);
        CS_String("state", state);
        CS_String("previous", csLastState);
        CS_Int("red", cgs.scores1);
        CS_Int("blue", cgs.scores2);
        CS_Close('}');
        CS_Event("match_state");
        Q_strncpyz(csLastState, state, sizeof(csLastState));
    }
    if (warmupRound != csLastWarmupRound) {
        csLastWarmupRound = warmupRound;
        if (warmupRound > 0) {
            csCountdownSeen = qtrue;
            CS_Begin();
            CS_Open(NULL, '{');
            CS_Int("time", cg.time);
            CS_Int("round", warmupRound);
            CS_Int("countdownEnd", atoi(Info_ValueForKey(info, "time")));
            CS_Close('}');
            CS_Event("round_countdown");
        }
    }
    roundStart = atoi(CG_ConfigString(CS_ROUND_START_TIME));
    if (roundStart != csLastRoundStart) {
        qboolean fresh = roundStart && (!csLastRoundStart || csCountdownSeen);
        csLastRoundStart = roundStart;
        if (fresh) {
            csCountdownSeen = qfalse;
            CS_Begin();
            CS_Open(NULL, '{');
            CS_Int("time", cg.time);
            CS_Int("round", warmupRound);
            CS_Int("startTime", roundStart);
            CS_Close('}');
            CS_Event("round_start");
        }
    }
}
