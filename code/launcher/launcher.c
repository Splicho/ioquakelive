#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../zlib/zlib.h"
#include "../renderercommon/tr_stbtt.h"
#define VK_NO_PROTOTYPES
#include "../renderercommon/vulkan/vulkan_core.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include <windows.h>
#include <winhttp.h>
#include <dwmapi.h>
#include <objbase.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#ifdef __APPLE__
#include <limits.h>
#include <mach-o/dyld.h>
#endif

#ifndef PRODUCT_VERSION
#define PRODUCT_VERSION "dev"
#endif

#if defined(_M_ARM64) || defined(__aarch64__)
#define GAME_ARCH "arm64"
#define PACKAGE_ARCH "arm64"
#elif defined(_M_X64) || defined(__x86_64__)
#define GAME_ARCH "x86_64"
#ifdef _WIN32
#define PACKAGE_ARCH "x64"
#else
#define PACKAGE_ARCH "x86_64"
#endif
#else
#define GAME_ARCH "x86"
#define PACKAGE_ARCH "x86"
#endif

#ifdef _WIN32
#define GAME_EXT ".exe"
#define PACKAGE_OS "windows"
#define LINK_NAME "ioquakelive.lnk"
#elif defined(__APPLE__)
#define GAME_EXT ""
#define PACKAGE_OS "macos"
#define LINK_NAME "ioquakelive"
#else
#define GAME_EXT ""
#define PACKAGE_OS "linux"
#define LINK_NAME "ioquakelive.desktop"
#endif

#define UI_W 1280
#define UI_H 720
#define UI_STEP 16.0f
#define UI_FILL_W 0.76f
#define UI_FILL_H 0.80f
#define UI_GROW 1.35f
#define ART_SNAP 1.04f
#define FONT_SHIFTS 4
#define EXTRA_MAX 2048
#define EXTRA_SLOTS 4096
#define EXTRA_PAGES 4
#define EXTRA_SIDE 1024
#define NAME_RUNES 96
#define COL_X 60.0f
#define COL_W 1160.0f
#define IN_X (COL_X + 24.0f)
#define LOGO_X 24.0f
#define TOP_MID 32.0f
#define VIG_W 160
#define VIG_H 90
#define VIG_BASE 0.30f
#define VIG_EDGE 0.45f
#define CTRL_W 44.0f
#define CTRL_H 36.0f
#define CTRL_Y (TOP_MID - CTRL_H * 0.5f)
#define PANEL_LOW 300.0f
#define PANEL_HIGH 72.0f
#define PANEL_Y panelTop
#define HEAD_H 48.0f
#define BODY_TOP (PANEL_Y + HEAD_H)
#define SIDE_W 380.0f
#define SIDE_X (COL_X + COL_W - 24.0f - SIDE_W)
#define TEXT_W (SIDE_X - 64.0f - IN_X)
#define PANEL_GAP 16.0f
#define PANEL_END (UI_H - 24.0f)

#define PLAY_H 52.0f
#define PLAY_Y (PANEL_END - 24.0f - PLAY_H)
#define BUTTON_R 8.0f
#define PANEL_R 16.0f
#define ROW_R 8.0f
#define SHAPE_MAX 72
#define LOGO 28.0f
#define MORE_ICON 18.0f
#define ITEM_R 6.0f
#define MENU_PAD 6.0f
#define NOTES_LINE 27.0f
#define CHECK_ICON 16.0f
#define CLOSE_ICON 12.0f
#define OPTS_ROWS 6
#define OPTS_W 320.0f
#define OPTS_ROW 40.0f
#define OPTS_GAP 9.0f
#define OPTS_X (UI_W - 12.0f - OPTS_W)
#define OPTS_Y (CTRL_Y + CTRL_H + 6.0f)
#define OPTS_H (OPTS_ROW * OPTS_ROWS + MENU_PAD * 2.0f + OPTS_GAP)
#define GEAR_ICON 16.0f
#define SRV_MAX 40
#define SRV_SLOTS 12
#define SHOT_MAX 48
#define ROW_H 50.0f
#define ROW_STEP 54.0f
#define LIST_W (COL_W - 64.0f)
#define MODE_X (IN_X + 560.0f)
#define THUMB_W 64.0f
#define THUMB_H 36.0f
#define SIDE_ROWS 3
#define SIDE_ROW_H 66.0f
#define SIDE_GAP 6.0f
#define SIDE_LINE 17.0f
#define SIDE_THUMB_W 52.0f
#define SIDE_THUMB_H 38.0f
#define BOX_W 600.0f
#define BOX_R 14.0f
#define COVER_H 184.0f
#define BOX_BTN_H 40.0f
#define BOX_FOOT 84.0f
#define BOX_ROW 34.0f
#define LIVE_MAX 64
#define PIC_MAX 512
#define PIC_JOBS 32
#define PIC_WORKERS 8
#define PIC_BYTES 0x80000
#define FLAG_SIDE 21.0f
#define CLAN_IMAGE "https://www.quakeclub.com/_next/image?url="
#define CLAN_DEFAULT "/clans-avatars/clan-default.png"
#define LIVE_API "https://www.quakeclub.com/api/server-live/"
#define SITE_SERVER "https://www.quakeclub.com/browser/"
#define SITE "https://www.quakeclub.com"
#define PRO_W 560.0f
#define PRO_COVER 132.0f
#define PRO_AVATAR 92.0f
#define PRO_TILE_H 70.0f
#define PRO_TEXT 0x40000
#define HUB_X COL_X
#define HUB_W COL_W
#define HUB_Y PANEL_HIGH
#define HUB_BODY (HUB_Y + HEAD_H)
#define HUB_IN (HUB_X + 24.0f)
#define HUB_CW (HUB_W - 48.0f)
#define HUB_PAD 24.0f
#define HUB_SECTION 40.0f
#define HUB_R 16.0f
#define HUB_MODES 8
#define HUB_SEASONS 3
#define HUB_RANK_MODES 5
#define HUB_ELO_MAX 400
#define HUB_RECENT 5
#define HUB_PAGE 30
#define HUB_MEN 32
#define HUB_RANK_PAGE 50
#define HUB_ROWS 64
#define HUB_TEXT 0x40000
#define HUB_CHART_PTS 1100
#define TRACK_HEAD 0.4f
#define TRACK_TAB 2.0f
#define TRACK_CAPS 1.5f
#define TRACK_BUTTON 1.2f
#define TRACK_NAV 1.4f
#define NAV_ITEMS 5
#define NAV_GAP 24.0f
#define NAV_X 236.0f

#define STABLE_API "https://api.github.com/repos/foressssst/ioquakelive/releases/latest"
#define BETA_API "https://api.github.com/repos/foressssst/ioquakelive/releases?per_page=1"
#define TAG_API "https://api.github.com/repos/foressssst/ioquakelive/releases/tags/v"
#define SERVERS_API "https://www.quakeclub.com/api/servers-status"
#define RELEASE_PAGE "https://github.com/foressssst/ioquakelive/releases/tag/"
#define DOWNLOAD_ROOT "https://github.com/foressssst/ioquakelive/releases/download/"
#define PACKAGE "ioquakelive-" PACKAGE_OS "-" PACKAGE_ARCH ".zip"
#define PACKAGE_NOTE "ioquakelive "
#define PART_NAME "ioquakelive-download.part"
#ifdef __APPLE__
#define SELF_APP "ioquakelive-launcher.app"
#define SELF_NAME SELF_APP "/Contents/MacOS/ioquakelive-launcher"
#define GAME_NAME "ioquakelive.app/Contents/MacOS/ioquakelive"
#define PAK_OURS "ioquakelive.app/Contents/Resources/baseq3/pak01.pk3"
#else
#define SELF_NAME "ioquakelive-launcher" GAME_EXT
#define GAME_NAME "ioquakelive." GAME_ARCH GAME_EXT
#define PAK_OURS "baseq3/pak01.pk3"
#endif
#define VULKAN_NAME "ioquakelive-vulkan." GAME_ARCH GAME_EXT
#define FOLDER_NAME "ioquakelive"
#define ZIP_TAIL (65536 + 22)
#define ZIP_BLOCK 0x10000
#define PAK_GAME "baseq3/pak00.pk3"
#define ART_HERO "ui/assets/backscreen_classic.jpg"
#define ART_CUSTOM "baseq3/launcher.jpg"
#define PAK_MAX_FILE 0x4000000
#define HTTP_MAX 0x40000
#define NOTES_MAX 16384
#define PAK_GAME_SIZE 962052238
#define COPY_BLOCK 0x100000

#define COL_BLACK 0x000000FF
#define COL_PURE 0xFFFFFFFF
#define COL_P1 0x24211DFF
#define COL_P2 0x2F2B27FF
#define COL_P3 0x393531FF
#define COL_P4 0x44403BFF
#define COL_WHITE 0xF2F0E9FF
#define COL_W85 0xF2F0E9E6
#define COL_W70 0xA5A199FF
#define COL_W40 0xA5A19999
#define COL_DOT 0x76726BFF
#define COL_INK 0x141310FF
#define COL_OK 0x5DB872FF
#define COL_MAL 0xDC5C54FF
#define COL_LINE 0x524D47FF
#define COL_GOLD 0xC9A961FF
#define COL_RED 0xD65A52FF
#define COL_BLUE 0x4D86D9FF
#define COL_HUSH 0x00000066
#define COL_RING 0xFFFFFF14
#define COL_SHADE 0x00000008

enum { CHECK_RUNNING, CHECK_DONE, CHECK_FAILED };
enum {
    FONT_PLAY,
    FONT_BRAND,
    FONT_PANEL,
    FONT_HEAD,
    FONT_TEXT,
    FONT_BODY,
    FONT_SCORE,
    FONT_SMALL,
    FONT_SMALLB,
    FONT_BOXNAME,
    FONT_BOXBTN,
    FONT_TINY,
    FONT_SV_NAME,
    FONT_SV_SCORE,
    FONT_SV_HEAD,
    FONT_SV_LIST,
    FONT_SV_META,
    FONT_SV_COUNT,
    FONT_SV_OF,
    FONT_SV_SIDE,
    FONT_SV_MAP,
    FONT_NAV,
    FONT_HUB_NAME,
    FONT_HUB_TITLE,
    FONT_HUB_DATA,
    FONT_HUB_TAB,
    FONT_HUB_SEASON,
    FONT_HUB_SECTION,
    FONT_HUB_ASIDE,
    FONT_HUB_LABEL,
    FONT_HUB_FIGURE,
    FONT_HUB_BIO,
    FONT_HUB_AXIS,
    FONT_HUB_WEAPON,
    FONT_HUB_DATO,
    FONT_HUB_RESULT,
    FONT_HUB_MAPLINE,
    FONT_HUB_MONO,
    FONT_HUB_PAGE,
    FONT_HUB_NUM,
    FONT_HUB_NUMB,
    FONT_HUB_RIVAL,
    FONT_HUB_MARK,
    FONT_HUB_GLOBE,
    FONT_HUB_GLOBEB,
    FONT_HUB_LINK,
    FONT_HUB_RANKNAME,
    FONT_HUB_RANKNUM,
    FONT_HUB_RANKSTAT,
    FONT_HUB_MANNAME,
    FONT_HUB_SCORE,
    FONT_HUB_INITIAL,
    FONT_COUNT
};
#define FONT_HUB_FIRST FONT_HUB_NAME
enum { FACE_TEXT, FACE_BOLD, FACE_DISPLAY, FACE_REGULAR, FACE_MEDIUM, FACE_STRONG, FACE_COUNT };
enum { LINE_BLANK, LINE_HEAD, LINE_BULLET, LINE_TEXT };
enum { VULKAN_PROBING, VULKAN_USABLE, VULKAN_MISSING };
enum {
    BTN_PLAY,
    BTN_UPDATE,
    BTN_STEAM,
    BTN_SERVERS,
    BTN_NEXT,
    BTN_ALT,
    BTN_BACK,
    BTN_OPT_A,
    BTN_OPT_B,
    BTN_ME,
    BTN_OPTS,
    BTN_MIN,
    BTN_CLOSE,
    BTN_NAV_0,
    BTN_NAV_LAST = BTN_NAV_0 + NAV_ITEMS - 1,
    BTN_MENU_0,
    BTN_MENU_LAST = BTN_MENU_0 + OPTS_ROWS - 1,
    BTN_SIDE_0,
    BTN_SIDE_LAST = BTN_SIDE_0 + SIDE_ROWS - 1,
    BTN_PLR_0,
    BTN_PLR_LAST = BTN_PLR_0 + LIVE_MAX - 1,
    BTN_CONNECT,
    BTN_SEE_SERVER,
    BTN_BOX_CLOSE,
    BTN_BOX,
    BTN_VEIL,
    BTN_HUB_TAB_0,
    BTN_HUB_CLOSE = BTN_HUB_TAB_0 + 4,
    BTN_HUB_SITE,
    BTN_HUB_MODE_0,
    BTN_HUB_SEASON_0 = BTN_HUB_MODE_0 + HUB_MODES + 1,
    BTN_HUB_SUB_0 = BTN_HUB_SEASON_0 + HUB_SEASONS,
    BTN_HUB_ALL = BTN_HUB_SUB_0 + 4,
    BTN_HUB_RANKLINK,
    BTN_HUB_PREV,
    BTN_HUB_NEXT,
    BTN_HUB_BACK,
    BTN_HUB_ROW_0,
    BTN_HUB = BTN_HUB_ROW_0 + HUB_ROWS,
    BTN_COUNT
};
enum { STEP_NONE, STEP_FILES, STEP_SETTINGS, STEP_DEMOS, STEP_PREFS };
#define SETUP_STEPS 4
enum { KIND_CONFIG, KIND_HUD, KIND_DEMOS, KIND_COUNT };
enum { JOB_SCAN, JOB_FILES, JOB_SETTINGS, JOB_DEMOS, JOB_INSTALL, JOB_UPDATE, JOB_REPAIR };
enum { RUN_IDLE, RUN_BUSY, RUN_OK, RUN_FAIL };
enum { FAIL_NONE, FAIL_FOLDER, FAIL_NET, FAIL_FILE, FAIL_BUSY };
enum { PHASE_DOWNLOAD, PHASE_INSTALL };
enum {
    TXT_PLAY,
    TXT_UPTODATE,
    TXT_CHECKING,
    TXT_CHECK_FAILED,
    TXT_PATCH_OUT,
    TXT_DOWNLOADING,
    TXT_INSTALLING,
    TXT_WHATS_NEW,
    TXT_INSTALL_TITLE,
    TXT_INSTALL_WHERE,
    TXT_CHANGE_FOLDER,
    TXT_SHORTCUT,
    TXT_FROM_GITHUB,
    TXT_FROM_FILE,
    TXT_ADOPT,
    TXT_FAIL_FOLDER,
    TXT_FAIL_NET,
    TXT_FAIL_FILE,
    TXT_FAIL_BUSY,
    TXT_STEAM_OFF,
    TXT_STEAM_OPEN,
    TXT_STEAM_OPENING,
    TXT_NO_VULKAN,
    TXT_SETUP_STEP,
    TXT_TITLE_FILES,
    TXT_TITLE_SETTINGS,
    TXT_TITLE_DEMOS,
    TXT_TITLE_PREFS,
    TXT_SCANNING,
    TXT_FILES_FOUND,
    TXT_NO_SPACE,
    TXT_PAK_COPY,
    TXT_INSTALL,
    TXT_CHOOSE_OTHER,
    TXT_FILES_MISSING,
    TXT_FIND_PAK,
    TXT_SEARCH_AGAIN,
    TXT_WRONG_FILE,
    TXT_PAK_FAILED,
    TXT_SETTINGS_INTRO,
    TXT_CONFIG,
    TXT_HUD,
    TXT_FILE_ONE,
    TXT_FILE_MANY,
    TXT_UNCHANGED,
    TXT_FILES_FAILED,
    TXT_IMPORT,
    TXT_SKIP,
    TXT_NEXT,
    TXT_FINISH,
    TXT_NO_SETTINGS,
    TXT_DEMO_ONE,
    TXT_DEMO_MANY,
    TXT_DEMOS_COPY,
    TXT_DEMOS_FAILED,
    TXT_NO_DEMOS,
    TXT_WORKING,
    TXT_BACK,
    TXT_PREFS_INTRO,
    TXT_STARTUP,
    TXT_STARTUP_DETAIL,
    TXT_BETA,
    TXT_BETA_DETAIL,
    TXT_OPEN_FOLDER,
    TXT_RUN_SETUP,
    TXT_REPAIR,
    TXT_CHECK_NOW,
    TXT_SERVERS_SIDE,
    TXT_SEE_ALL,
    TXT_SERVERS_TITLE,
    TXT_SERVERS_LOADING,
    TXT_SERVERS_FAILED,
    TXT_COL_SERVER,
    TXT_COL_MODE,
    TXT_COL_PLAYERS,
    TXT_OF,
    TXT_STATE_PLAYING,
    TXT_STATE_WARMUP,
    TXT_STATE_COUNTDOWN,
    TXT_STATE_PREGAME,
    TXT_STATE_WAITING,
    TXT_RED_TEAM,
    TXT_BLUE_TEAM,
    TXT_SPECTATORS,
    TXT_NOBODY,
    TXT_JOIN,
    TXT_SEE_SERVER,
    TXT_CLOSE,
    TXT_TRAY_OPEN,
    TXT_TRAY_QUIT,
    TXT_PRO_MISSING,
    TXT_PRO_FAILED,
    TXT_HUB_PROFILE,
    TXT_HUB_MATCHES,
    TXT_HUB_RANKING,
    TXT_HUB_SERVERS,
    TXT_NAV_HOME,
    TXT_HUB_SITE,
    TXT_HUB_SINCE,
    TXT_HUB_ABOUT,
    TXT_HUB_SEASON,
    TXT_HUB_STATS,
    TXT_HUB_TIER,
    TXT_HUB_ELO,
    TXT_HUB_CALIBRATING,
    TXT_HUB_INACTIVE,
    TXT_HUB_BANNED,
    TXT_HUB_GLOBAL,
    TXT_HUB_PLAYERS,
    TXT_HUB_PLAYERS_IN,
    TXT_HUB_FEWER,
    TXT_HUB_DIDNT,
    TXT_HUB_UNRANKED,
    TXT_HUB_TO_ENTER,
    TXT_HUB_COME_BACK,
    TXT_HUB_RECORD,
    TXT_HUB_WINRATE,
    TXT_HUB_KD,
    TXT_HUB_FRAGS,
    TXT_HUB_CLUTCH,
    TXT_HUB_CLUTCH_SOON,
    TXT_HUB_CLUTCH_OF,
    TXT_HUB_FAV,
    TXT_HUB_FAV_NONE,
    TXT_HUB_HISTORY,
    TXT_HUB_HISTORY_S1,
    TXT_HUB_HISTORY_FEW,
    TXT_HUB_ELO_AT,
    TXT_HUB_UP,
    TXT_HUB_DOWN,
    TXT_HUB_ACCURACY,
    TXT_HUB_ACC,
    TXT_HUB_NO_WEAPONS_IN,
    TXT_HUB_NO_WEAPONS,
    TXT_HUB_MEDALS,
    TXT_HUB_NO_MEDALS_IN,
    TXT_HUB_NO_MEDALS,
    TXT_HUB_RECENT,
    TXT_HUB_SEE_ALL,
    TXT_HUB_COL_SERVER,
    TXT_HUB_COL_VS,
    TXT_HUB_COL_FRAGS,
    TXT_HUB_COL_ELO,
    TXT_HUB_COL_DATE,
    TXT_HUB_GAME_FRAGS,
    TXT_HUB_NO_GAMES_IN,
    TXT_HUB_NO_GAMES,
    TXT_HUB_GAMES_OF,
    TXT_HUB_ALL,
    TXT_HUB_COUNT_ALL,
    TXT_HUB_COUNT_ALL_ONE,
    TXT_HUB_COUNT_MODE,
    TXT_HUB_COUNT_MODE_ONE,
    TXT_HUB_NONE_YET,
    TXT_HUB_EMPTY_ALL,
    TXT_HUB_EMPTY_MODE,
    TXT_HUB_PREV,
    TXT_HUB_NEXT,
    TXT_HUB_PAGE,
    TXT_HUB_COL_RANK,
    TXT_HUB_COL_PLAYER,
    TXT_HUB_COL_GAMES,
    TXT_HUB_COL_WINS,
    TXT_HUB_COL_LAST,
    TXT_HUB_COL_KD,
    TXT_HUB_RANK_EMPTY,
    TXT_HUB_BACK,
    TXT_HUB_NOT_RATED,
    TXT_HUB_WHY_ABORTED,
    TXT_HUB_WHY_FEW,
    TXT_HUB_WHY_SHORT,
    TXT_HUB_WHY_RATED,
    TXT_HUB_SUMMARY,
    TXT_HUB_WEAPONS,
    TXT_HUB_FLAGS,
    TXT_HUB_RED,
    TXT_HUB_BLUE,
    TXT_HUB_WON,
    TXT_HUB_LOST,
    TXT_HUB_TIE,
    TXT_HUB_WINNER,
    TXT_HUB_COL_SCORE,
    TXT_HUB_COL_PERF,
    TXT_HUB_COL_DEATHS,
    TXT_HUB_COL_DAMAGE,
    TXT_HUB_COL_ACC,
    TXT_HUB_COL_DIFF,
    TXT_HUB_W_RL,
    TXT_HUB_W_LG,
    TXT_HUB_W_RG,
    TXT_HUB_W_SG,
    TXT_HUB_W_MG,
    TXT_HUB_UNUSED,
    TXT_HUB_HITS,
    TXT_HUB_NO_MATCH_MEDALS,
    TXT_HUB_CAPTURED,
    TXT_HUB_RETURNED,
    TXT_HUB_TOOK,
    TXT_HUB_DROPPED,
    TXT_HUB_CARRIER,
    TXT_HUB_LOADING,
    TXT_HUB_FAILED,
    TXT_COUNT
};

typedef struct {
    int ax, ay, w, h, ox, oy;
} cell_t;

typedef struct {
    cell_t cells[FONT_SHIFTS];
    float advance;
    bool color;
} glyph_t;

typedef struct {
    SDL_Texture* tex;
    glyph_t glyphs[224];
    float ascent;
    float height;
    float cap;
    float scale;
    float em;
    int face;
} font_t;

typedef struct {
    Uint32 cp;
    int font;
    int page;
    glyph_t glyph;
} extra_t;

typedef struct {
    const char* file;
    bool bold;
    bool tried;
    unsigned char* data;
    stbtt_fontinfo info;
} fallback_t;

typedef struct {
    Uint32 cp;
    Uint32 col;
} nameRune_t;

typedef struct {
    float x, y, w, h;
    bool visible;
} button_t;

typedef struct {
    SDL_Texture* tex;
    int w, h;
} image_t;

typedef struct {
    float x[SHAPE_MAX], y[SHAPE_MAX], mx[SHAPE_MAX], my[SHAPE_MAX];
    int n;
} shape_t;

typedef struct {
    SDL_Vertex v[SHAPE_MAX * 4 + 8];
    int idx[SHAPE_MAX * 12 + 24];
    int nv, ni;
} mesh_t;

typedef struct {
    int step;
    bool found, picked, canLink, failed, wrongFile;
    bool want[2];
    char ql[1024];
    char pak[1024];
    char user[1024];
    int count[KIND_COUNT];
    Sint64 bytes[KIND_COUNT];
} setup_t;

typedef struct {
    const char* to;
    int kind;
    int depth;
    bool link;
    bool ok;
} walk_t;

typedef struct {
    bool active, local, adopt, shortcut, done;
    int fail;
    char dir[1024];
    char zip[1024];
} install_t;

typedef struct {
    char version[32];
    char url[512];
    char hash[65];
    Sint64 size;
} pack_t;

unsigned char* Launcher_DecodeJpeg(const unsigned char* data, unsigned long size, int* width, int* height, int denom);
extern const unsigned char launcherFontText[];
extern const unsigned char launcherFontBold[];
extern const unsigned char launcherFontDisplay[];
extern const unsigned char launcherFontRegular[];
extern const unsigned char launcherFontMedium[];
extern const unsigned char launcherFontStrong[];
extern const unsigned char launcherLogo[];
extern const unsigned int launcherLogoSize;

static SDL_Window* window;
static SDL_Renderer* renderer;
static image_t hero, shade, logo, iconMore, iconCheck, iconSlash, iconGear;
static float uiScale = 1.0f;
static const float artBox[2][4] = {{-24.0f, -40.0f, UI_W * 1.06f, UI_H * 1.06f}, {-12.0f, -26.0f, UI_W + 24.0f, UI_H + 24.0f}};
static const char* basePath = "";
static font_t fonts[FONT_COUNT];
static bool fontsReady;
static stbtt_fontinfo faceInfo[FACE_COUNT];
static extra_t extras[EXTRA_MAX];
static Uint16 extraSlots[EXTRA_SLOTS];
static SDL_Texture* extraPages[EXTRA_PAGES];
static int extraCount, extraPageCount, extraX, extraY, extraRow;
static fallback_t fallbacks[] = {
#if defined(_WIN32)
    {"segoeui.ttf", false}, {"segoeuib.ttf", true}, {"seguisym.ttf", false}, {"YuGothM.ttc", false}, {"YuGothB.ttc", true},
    {"malgun.ttf", false},  {"malgunbd.ttf", true}, {"msyh.ttc", false},     {"msyhbd.ttc", true},  {"Nirmala.ttc", false},
    {"LeelawUI.ttf", false}, {"ebrima.ttf", false}, {"gadugi.ttf", false},   {"seguihis.ttf", false}, {"cambria.ttc", false},
    {"seguiemj.ttf", false},
#elif defined(__APPLE__)
    {"/System/Library/Fonts/Supplemental/Arial.ttf", false},
    {"/System/Library/Fonts/Supplemental/Arial Bold.ttf", true},
    {"/System/Library/Fonts/Supplemental/Arial Unicode.ttf", false},
    {"/System/Library/Fonts/Apple Symbols.ttf", false},
    {"/Library/Fonts/Arial Unicode.ttf", false},
#else
    {"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", false},
    {"/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", true},
    {"/usr/share/fonts/TTF/DejaVuSans.ttf", false},
    {"/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", true},
    {"/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf", false},
    {"/usr/share/fonts/dejavu-sans-fonts/DejaVuSans-Bold.ttf", true},
    {"/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf", false},
    {"/usr/share/fonts/truetype/noto/NotoSans-Bold.ttf", true},
    {"/usr/share/fonts/noto/NotoSans-Regular.ttf", false},
    {"/usr/share/fonts/noto/NotoSans-Bold.ttf", true},
    {"/usr/share/fonts/truetype/noto/NotoSansSymbols2-Regular.ttf", false},
    {"/usr/share/fonts/noto/NotoSansSymbols2-Regular.ttf", false},
    {"/usr/share/fonts/truetype/noto/NotoSansSymbols-Regular.ttf", false},
    {"/usr/share/fonts/noto/NotoSansSymbols-Regular.ttf", false},
    {"/usr/share/fonts/truetype/noto/NotoSansMath-Regular.ttf", false},
    {"/usr/share/fonts/noto/NotoSansMath-Regular.ttf", false},
    {"/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf", false},
    {"/usr/share/fonts/google-droid-sans-fonts/DroidSansFallbackFull.ttf", false},
    {"/usr/share/fonts/truetype/wqy/wqy-microhei.ttc", false},
    {"/usr/share/fonts/wenquanyi/wqy-microhei/wqy-microhei.ttc", false},
#endif
};
static button_t buttons[BTN_COUNT];
static float heat[BTN_COUNT];
static SDL_Cursor* cursorArrow;
static SDL_Cursor* cursorHand;
static int hotButton = -1;
static float navX, navW, navGoalX, navGoalW;
static bool navReady;
static bool gameFound, vulkanFound, heroCustom, firstRun;
static int rendererHint = -1;
static SDL_AtomicInt vulkanState;
static char** gameArgs;
static int gameArgCount;
static float introHeat, notesLow, thumbY, thumbH, dragStart, dragFrom, textTrack;
static int dragMode;
static float notesHeat, notesScroll, notesGoal, notesHeight, notesView, notesTop;
static float stepHeat = 1.0f, slideX, slideFade = 1.0f;
static int stepDir, stepFrom;
static Uint64 stepStart;
static float panelTop = PANEL_LOW;
static bool slideClip;
static bool optsOpen, betaChannel, startMinimized, startupOn, autoTried, prefStartup, prefBeta;
static float optsHeat;
static char connectTo[64];
static SDL_Thread* checkThread;
typedef struct {
    char name[96], map[32], mode[16], addr[48];
    int players, max;
} server_t;
typedef struct {
    char map[32];
    image_t img, mini;
    float heat;
} shot_t;
typedef struct {
    char name[64], raw[96], avatar[160], country[4], clanTag[24], clanPic[160], steamId[24];
    int team, score;
    bool clan;
} player_t;
typedef struct {
    char key[200];
    image_t img;
    float heat;
    Uint64 again;
    int tries;
    bool done;
} pic_t;
typedef struct {
    char steamId[24], raw[128], title[96], country[4], clanTag[24], clanName[96], avatar[200], banner[200], flag[200], thumb[200];
    Uint32 titleColor;
    int status, elo, rank, total, left, kind;
    float kd, lg, rg;
    bool clan, haveKd, haveLg, haveRg;
} profile_t;
typedef struct {
    char key[200];
    unsigned char* data;
    size_t size;
} picNew_t;
typedef struct {
    char map[32];
    unsigned char* rgba;
    int w, h;
} shotNew_t;
static shot_t shots[SHOT_MAX];
static shotNew_t shotsNew[SHOT_MAX];
static char shotsTried[SHOT_MAX][32], shotsJob[SHOT_MAX][32];
static int shotCount, shotsNewCount, shotsNewNext, shotsTriedCount, shotsJobCount;
static SDL_AtomicInt shotsState;
static SDL_Thread* shotsThread;
static server_t servers[SRV_MAX], serversNext[SRV_MAX];
static int serverCount, serversNextCount, serversTotal;
static SDL_AtomicInt serversState;
static SDL_Thread* serversThread;
static Uint64 serversWhen;
static bool serversLoaded, serversFailed;
static char serversText[65536];
static server_t boxServer;
static bool boxOpen, liveLoaded, liveFailed, liveAgain;
static float boxHeat, boxGoal, boxView;
static player_t livePlayers[LIVE_MAX], liveNext[LIVE_MAX];
static int liveCount, liveNextCount, liveScore[2], liveNextScore[2], boxWant;
static char liveGame[24], liveNextGame[24], liveNextAddr[48];
static char liveText[65536];
static SDL_AtomicInt liveState;
static SDL_Thread* liveThread;
static pic_t pics[PIC_MAX];
static picNew_t picsNew[PIC_JOBS];
static char picsJob[PIC_JOBS][200];
static int picCount, picsJobCount;
static SDL_AtomicInt picsState;
static SDL_Thread* picsThread;
static Uint64 liveWhen;
static profile_t meProfile, meNext;
static SDL_AtomicInt meState;
static SDL_Thread* meThread;
static float boxClipTop, boxClipBottom;
static bool waitLock;
#ifdef _WIN32
static HANDLE instanceLock;
#else
static int instanceLock = -1;
#endif
static bool notesDry;

static SDL_AtomicInt checkState;
static char httpText[HTTP_MAX];
static char latestTag[32];
static char latestVersion[32];
static bool updateAvailable;
static char latestNotes[NOTES_MAX];

static setup_t setup;
static int setupSaved;
static bool wrapPlain;
static SDL_AtomicInt jobRun, jobKb, jobStop, pickState, jobTotal, jobPhase, jobFail;
static SDL_Thread* jobThread;
static int jobKind;
static Sint64 jobBytes;
static install_t install;
static pack_t packLatest, packJob;
static char jobText[HTTP_MAX];
static char jobVersion[32];
static char installedVersion[32] = PRODUCT_VERSION;
static char selfPath[1024];
#ifdef __APPLE__
static char selfApp[1024];
static char appRoot[1024];
#endif
static char pickPath[1024];
static Sint64 userWhen;
static bool steamMissing;
static Uint64 steamChecked, steamOpening;

static const char* const texts[TXT_COUNT] = {
    [TXT_PLAY] = "Jugar",
    [TXT_UPTODATE] = "Última versión",
    [TXT_CHECKING] = "Buscando actualizaciones...",
    [TXT_CHECK_FAILED] = "No se pudieron buscar actualizaciones",
    [TXT_PATCH_OUT] = "Actualizar a la %s",
    [TXT_DOWNLOADING] = "Descargando: %d%%",
    [TXT_INSTALLING] = "Instalando...",
    [TXT_WHATS_NEW] = "Novedades de la versión %s",
    [TXT_INSTALL_TITLE] = "Instalar ioquakelive",
    [TXT_INSTALL_WHERE] = "Se instalará en:",
    [TXT_CHANGE_FOLDER] = "Cambiar carpeta",
    [TXT_SHORTCUT] = "Crear un acceso directo en el escritorio",
    [TXT_FROM_GITHUB] = "ioquakelive se descargará desde GitHub.",
    [TXT_FROM_FILE] = "ioquakelive se instalará desde el archivo ZIP ubicado junto al launcher.",
    [TXT_ADOPT] = "Se detectó una instalación de ioquakelive. Solo se agregará el launcher.",
    [TXT_FAIL_FOLDER] = "No se puede utilizar esta carpeta. Elige otra.",
    [TXT_FAIL_NET] = "La descarga falló. Comprueba tu conexión a internet e inténtalo de nuevo.",
    [TXT_FAIL_FILE] = "No se pudieron instalar los archivos. Comprueba que haya suficiente espacio en disco.",
    [TXT_FAIL_BUSY] = "Cierra ioquakelive e inténtalo de nuevo.",
    [TXT_STEAM_OFF] = "Steam no está abierto.",
    [TXT_STEAM_OPEN] = "Abrir Steam",
    [TXT_STEAM_OPENING] = "Abriendo Steam...",
    [TXT_NO_VULKAN] = "Tu equipo no es compatible con Vulkan, así que por ahora jugarás con OpenGL. Actualizar los drivers de tu tarjeta de video puede solucionarlo.",
    [TXT_SETUP_STEP] = "Configuración inicial · Paso %d de %d",
    [TXT_TITLE_FILES] = "Archivos del juego",
    [TXT_TITLE_SETTINGS] = "Configuración y HUD",
    [TXT_TITLE_DEMOS] = "Demos",
    [TXT_SCANNING] = "Buscando Quake Live...",
    [TXT_FILES_FOUND] = "ioquakelive utiliza los archivos de tu instalación original de Quake Live. Se encontraron en:",
    [TXT_NO_SPACE] = "No ocupará espacio adicional en el disco.",
    [TXT_PAK_COPY] = "Se copiará pak00.pk3 (917 MB).",
    [TXT_INSTALL] = "Instalar",
    [TXT_CHOOSE_OTHER] = "Seleccionar manualmente",
    [TXT_FILES_MISSING] = "Para utilizar ioquakelive necesitas una copia legítima de Quake Live en Steam. Instálalo desde "
                          "tu biblioteca de Steam o selecciona manualmente el archivo pak00.pk3.",
    [TXT_FIND_PAK] = "Buscar pak00.pk3",
    [TXT_SEARCH_AGAIN] = "Buscar de nuevo",
    [TXT_WRONG_FILE] = "El archivo seleccionado no corresponde al pak00.pk3 de Quake Live.",
    [TXT_PAK_FAILED] = "No se pudo instalar pak00.pk3. Comprueba que haya suficiente espacio en disco.",
    [TXT_SETTINGS_INTRO] = "Importa tu configuración y HUD de Quake Live.",
    [TXT_CONFIG] = "Configuración",
    [TXT_HUD] = "HUD",
    [TXT_FILE_ONE] = "%d archivo",
    [TXT_FILE_MANY] = "%d archivos",
    [TXT_UNCHANGED] = "Tu instalación original de Quake Live no se modificará.",
    [TXT_FILES_FAILED] = "No se pudieron copiar algunos archivos.",
    [TXT_IMPORT] = "Importar",
    [TXT_SKIP] = "Omitir",
    [TXT_NEXT] = "Siguiente",
    [TXT_FINISH] = "Terminar",
    [TXT_NO_SETTINGS] = "No se encontró ninguna configuración ni HUD en tu instalación de Quake Live.",
    [TXT_DEMO_ONE] = "Se encontró %d demo en tu instalación de Quake Live (%s).",
    [TXT_DEMO_MANY] = "Se encontraron %d demos en tu instalación de Quake Live (%s).",
    [TXT_DEMOS_COPY] = "Se copiarán a ioquakelive.",
    [TXT_DEMOS_FAILED] = "No se pudieron copiar algunas demos.",
    [TXT_NO_DEMOS] = "No se encontraron demos en tu instalación de Quake Live.",
    [TXT_WORKING] = "Copiando: %d%%",
    [TXT_BACK] = "Atrás",
    [TXT_TITLE_PREFS] = "Preferencias",
    [TXT_PREFS_INTRO] = "Puedes cambiarlas cuando quieras desde las opciones.",
    [TXT_STARTUP] = "Abrir al iniciar el equipo",
    [TXT_STARTUP_DETAIL] = "Minimizado, para actualizarse solo",
    [TXT_BETA] = "Versiones experimentales",
    [TXT_BETA_DETAIL] = "Lo nuevo antes, aún en prueba",
    [TXT_OPEN_FOLDER] = "Abrir la carpeta del juego",
    [TXT_RUN_SETUP] = "Configuración inicial",
    [TXT_REPAIR] = "Reparar la instalación",
    [TXT_CHECK_NOW] = "Buscar actualizaciones",
    [TXT_SERVERS_SIDE] = "Servidores",
    [TXT_SEE_ALL] = "Ver todos",
    [TXT_SERVERS_TITLE] = "Servidores de Quake Club",
    [TXT_SERVERS_LOADING] = "Cargando servidores...",
    [TXT_SERVERS_FAILED] = "No se pudo cargar la lista de servidores.",
    [TXT_COL_SERVER] = "Servidor",
    [TXT_COL_MODE] = "Modo y mapa",
    [TXT_COL_PLAYERS] = "Jugadores",
    [TXT_OF] = "de",
    [TXT_STATE_PLAYING] = "En juego",
    [TXT_STATE_WARMUP] = "Calentamiento",
    [TXT_STATE_COUNTDOWN] = "Cuenta atrás",
    [TXT_STATE_PREGAME] = "Previa",
    [TXT_STATE_WAITING] = "Esperando",
    [TXT_RED_TEAM] = "Red Team",
    [TXT_BLUE_TEAM] = "Blue Team",
    [TXT_SPECTATORS] = "Espectadores",
    [TXT_NOBODY] = "No hay nadie jugando ahora.",
    [TXT_JOIN] = "Conectar",
    [TXT_SEE_SERVER] = "Ver el servidor",
    [TXT_CLOSE] = "Cerrar",
    [TXT_TRAY_OPEN] = "Abrir ioquakelive",
    [TXT_TRAY_QUIT] = "Salir",
    [TXT_PRO_MISSING] = "Todavía no tiene perfil en Quake Club.",
    [TXT_PRO_FAILED] = "No se pudo cargar el perfil.",
    [TXT_HUB_PROFILE] = "Perfil",
    [TXT_HUB_MATCHES] = "Partidas",
    [TXT_HUB_RANKING] = "Ranking",
    [TXT_HUB_SERVERS] = "Servidores",
    [TXT_NAV_HOME] = "Inicio",
    [TXT_HUB_SITE] = "Ver en la web",
    [TXT_HUB_SINCE] = "Miembro desde %s",
    [TXT_HUB_ABOUT] = "Sobre mí",
    [TXT_HUB_SEASON] = "Season %d",
    [TXT_HUB_STATS] = "Estadísticas",
    [TXT_HUB_TIER] = "Tier",
    [TXT_HUB_ELO] = "%s ELO",
    [TXT_HUB_CALIBRATING] = "Calibrando",
    [TXT_HUB_INACTIVE] = "Inactivo",
    [TXT_HUB_BANNED] = "Baneado",
    [TXT_HUB_GLOBAL] = "Global Ranking",
    [TXT_HUB_PLAYERS] = "%s jugadores",
    [TXT_HUB_PLAYERS_IN] = "%s jugadores en %s",
    [TXT_HUB_FEWER] = "menos de %d partidas en %s",
    [TXT_HUB_DIDNT] = "no jugó %s",
    [TXT_HUB_UNRANKED] = "Sin rango",
    [TXT_HUB_TO_ENTER] = "partidas para entrar",
    [TXT_HUB_COME_BACK] = "vuelve a jugar para reaparecer",
    [TXT_HUB_RECORD] = "Registro de partidas",
    [TXT_HUB_WINRATE] = "%d%% winrate",
    [TXT_HUB_KD] = "K/D Ratio",
    [TXT_HUB_FRAGS] = "%s frags / %s muertes",
    [TXT_HUB_CLUTCH] = "Clutch",
    [TXT_HUB_CLUTCH_SOON] = "se mide desde el 17 de noviembre",
    [TXT_HUB_CLUTCH_OF] = "de %s veces último en pie",
    [TXT_HUB_FAV] = "Mapa más jugado",
    [TXT_HUB_FAV_NONE] = "Ninguno todavía",
    [TXT_HUB_HISTORY] = "Rendimiento histórico",
    [TXT_HUB_HISTORY_S1] = "El historial de ELO se guarda desde la Season 2.",
    [TXT_HUB_HISTORY_FEW] = "Todavía no tiene suficientes partidas para dibujar su ELO.",
    [TXT_HUB_ELO_AT] = "%s de ELO",
    [TXT_HUB_UP] = "Subió %d",
    [TXT_HUB_DOWN] = "Bajó %d",
    [TXT_HUB_ACCURACY] = "Accuracy",
    [TXT_HUB_ACC] = "%s %% ACC",
    [TXT_HUB_NO_WEAPONS_IN] = "No registró armas en %s en la %s.",
    [TXT_HUB_NO_WEAPONS] = "Todavía no tiene armas registradas en %s.",
    [TXT_HUB_MEDALS] = "Medallas",
    [TXT_HUB_NO_MEDALS_IN] = "No ganó medallas en %s en la %s.",
    [TXT_HUB_NO_MEDALS] = "Todavía no tiene medallas en %s.",
    [TXT_HUB_RECENT] = "Últimas partidas",
    [TXT_HUB_SEE_ALL] = "Ver todas",
    [TXT_HUB_COL_SERVER] = "Servidor",
    [TXT_HUB_COL_VS] = "Contra",
    [TXT_HUB_COL_FRAGS] = "Frags",
    [TXT_HUB_COL_ELO] = "ELO",
    [TXT_HUB_COL_DATE] = "Fecha",
    [TXT_HUB_GAME_FRAGS] = "%d frags / %d muertes",
    [TXT_HUB_NO_GAMES_IN] = "No jugó %s en la %s.",
    [TXT_HUB_NO_GAMES] = "Todavía no tiene partidas de %s.",
    [TXT_HUB_GAMES_OF] = "Partidas de",
    [TXT_HUB_ALL] = "Todos",
    [TXT_HUB_COUNT_ALL] = "%s partidas jugadas",
    [TXT_HUB_COUNT_ALL_ONE] = "%s partida jugada",
    [TXT_HUB_COUNT_MODE] = "%s partidas de %s",
    [TXT_HUB_COUNT_MODE_ONE] = "%s partida de %s",
    [TXT_HUB_NONE_YET] = "Todavía no tiene partidas registradas.",
    [TXT_HUB_EMPTY_ALL] = "Todavía no hay partidas.",
    [TXT_HUB_EMPTY_MODE] = "No hay partidas de %s.",
    [TXT_HUB_PREV] = "Anterior",
    [TXT_HUB_NEXT] = "Siguiente",
    [TXT_HUB_PAGE] = "Página %d de %d",
    [TXT_HUB_COL_RANK] = "#",
    [TXT_HUB_COL_PLAYER] = "Jugador",
    [TXT_HUB_COL_GAMES] = "Partidos jugados",
    [TXT_HUB_COL_WINS] = "Victorias",
    [TXT_HUB_COL_LAST] = "Último en pie",
    [TXT_HUB_COL_KD] = "K/D",
    [TXT_HUB_RANK_EMPTY] = "Todavía no hay jugadores clasificados en %s.",
    [TXT_HUB_BACK] = "Volver",
    [TXT_HUB_NOT_RATED] = "No cuenta para el ranking",
    [TXT_HUB_WHY_ABORTED] = ": la partida se abortó.",
    [TXT_HUB_WHY_FEW] = ": faltaron jugadores.",
    [TXT_HUB_WHY_SHORT] = ": duró demasiado poco.",
    [TXT_HUB_WHY_RATED] = ": no es una partida puntuada.",
    [TXT_HUB_SUMMARY] = "Resumen",
    [TXT_HUB_WEAPONS] = "Armas",
    [TXT_HUB_FLAGS] = "Banderas",
    [TXT_HUB_RED] = "Red Team",
    [TXT_HUB_BLUE] = "Blue Team",
    [TXT_HUB_WON] = "Ganó",
    [TXT_HUB_LOST] = "Perdió",
    [TXT_HUB_TIE] = "Empate",
    [TXT_HUB_WINNER] = "Ganó %s",
    [TXT_HUB_COL_SCORE] = "Puntos",
    [TXT_HUB_COL_PERF] = "Performance",
    [TXT_HUB_COL_DEATHS] = "Muertes",
    [TXT_HUB_COL_DAMAGE] = "Daño",
    [TXT_HUB_COL_ACC] = "ACC",
    [TXT_HUB_COL_DIFF] = "+/-",
    [TXT_HUB_W_RL] = "RL",
    [TXT_HUB_W_LG] = "LG",
    [TXT_HUB_W_RG] = "RG",
    [TXT_HUB_W_SG] = "SG",
    [TXT_HUB_W_MG] = "MG",
    [TXT_HUB_UNUSED] = "No la usó",
    [TXT_HUB_HITS] = "%d de %d",
    [TXT_HUB_NO_MATCH_MEDALS] = "Sin medallas en esta partida",
    [TXT_HUB_CAPTURED] = "Capturó",
    [TXT_HUB_RETURNED] = "Devolvió",
    [TXT_HUB_TOOK] = "Tomó",
    [TXT_HUB_DROPPED] = "Soltó",
    [TXT_HUB_CARRIER] = "Bajas al portador",
    [TXT_HUB_LOADING] = "Cargando...",
    [TXT_HUB_FAILED] = "No se pudo cargar.",
};

#define T(id) texts[id]

static unsigned Rd16(const unsigned char* p) {
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

static unsigned Rd32(const unsigned char* p) {
    return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24);
}

static bool Pak_ReadAt(SDL_IOStream* io, Sint64 offset, void* out, size_t size) {
    return SDL_SeekIO(io, offset, SDL_IO_SEEK_SET) == offset && SDL_ReadIO(io, out, size) == size;
}

static unsigned char* Zip_Dir(SDL_IOStream* io, unsigned* count, unsigned* dirSize, char* note, size_t noteSize) {
    Sint64 total = SDL_GetIOSize(io);
    size_t tailSize = total < ZIP_TAIL ? (size_t)(total < 0 ? 0 : total) : ZIP_TAIL;
    unsigned char* tail = tailSize >= 22 ? malloc(tailSize) : NULL;
    unsigned char* dir = NULL;
    int i;

    if (note && noteSize) {
        note[0] = '\0';
    }
    if (!tail || !Pak_ReadAt(io, total - (Sint64)tailSize, tail, tailSize)) {
        free(tail);
        return NULL;
    }
    for (i = (int)tailSize - 22; i >= 0; i--) {
        if (Rd32(tail + i) == 0x06054b50) {
            break;
        }
    }
    if (i >= 0) {
        size_t n = Rd16(tail + i + 20);

        *count = Rd16(tail + i + 10);
        *dirSize = Rd32(tail + i + 12);
        if (note && noteSize) {
            n = SDL_min(n, tailSize - (size_t)i - 22);
            n = SDL_min(n, noteSize - 1);
            memcpy(note, tail + i + 22, n);
            note[n] = '\0';
        }
        if (*dirSize && *dirSize < PAK_MAX_FILE) {
            dir = malloc(*dirSize);
        }
        if (dir && !Pak_ReadAt(io, Rd32(tail + i + 16), dir, *dirSize)) {
            free(dir);
            dir = NULL;
        }
    }
    free(tail);
    return dir;
}

static unsigned char* Pak_Read(const char* pak, const char* name, size_t* outSize) {
    char path[1024];
    unsigned char local[30];
    unsigned char* dir;
    unsigned char* packed = NULL;
    unsigned char* out = NULL;
    unsigned char* p;
    SDL_IOStream* io;
    size_t nameLen = strlen(name);
    unsigned count = 0, dirSize = 0, method = 0, packedSize = 0, size = 0, header = 0, k;
    bool found = false;

    SDL_snprintf(path, sizeof(path), "%s%s", pak[0] == '/' || (pak[0] && pak[1] == ':') ? "" : basePath, pak);
    io = SDL_IOFromFile(path, "rb");
    if (!io) {
        return NULL;
    }
    dir = Zip_Dir(io, &count, &dirSize, NULL, 0);
    if (!dir) {
        SDL_CloseIO(io);
        return NULL;
    }
    p = dir;
    for (k = 0; k < count && p + 46 <= dir + dirSize && Rd32(p) == 0x02014b50; k++) {
        unsigned n = Rd16(p + 28);

        if (p + 46 + n > dir + dirSize) {
            break;
        }
        if (n == nameLen && !SDL_strncasecmp((const char*)p + 46, name, n)) {
            method = Rd16(p + 10);
            packedSize = Rd32(p + 20);
            size = Rd32(p + 24);
            header = Rd32(p + 42);
            found = true;
            break;
        }
        p += 46 + n + Rd16(p + 30) + Rd16(p + 32);
    }
    free(dir);
    if (found && size && size < PAK_MAX_FILE && packedSize < PAK_MAX_FILE && Pak_ReadAt(io, header, local, sizeof(local)) &&
        Rd32(local) == 0x04034b50) {
        packed = malloc(packedSize ? packedSize : 1);
        if (packed && Pak_ReadAt(io, (Sint64)header + 30 + Rd16(local + 26) + Rd16(local + 28), packed, packedSize)) {
            if (method == 0 && packedSize == size) {
                out = packed;
                packed = NULL;
            } else if (method == 8) {
                z_stream zs;

                memset(&zs, 0, sizeof(zs));
                out = malloc(size);
                if (out && inflateInit2(&zs, -MAX_WBITS) == Z_OK) {
                    int result;

                    zs.next_in = packed;
                    zs.avail_in = packedSize;
                    zs.next_out = out;
                    zs.avail_out = size;
                    result = inflate(&zs, Z_FINISH);
                    inflateEnd(&zs);
                    if (result != Z_STREAM_END) {
                        free(out);
                        out = NULL;
                    }
                } else {
                    free(out);
                    out = NULL;
                }
            }
        }
    }
    free(packed);
    SDL_CloseIO(io);
    if (out && outSize) {
        *outSize = size;
    }
    return out;
}

static unsigned char* Font_Raster(font_t* font, const stbtt_fontinfo* info, float pixels, bool em, int last, int* outH) {
    const int atlasW = 1024;
    float scale = em ? R_ScaleForMappingEmToPixels(info, pixels) : R_ScaleForPixelHeight(info, pixels);
    int ascent, descent, gap, c, s, x = 1, y = 1, rowH = 0, atlasH;
    unsigned char* alpha;

    R_GetFontLineMetrics(info, &ascent, &descent, &gap);
    font->scale = scale;
    font->em = scale * (float)R_GetFontUnitsPerEm(info);
    font->ascent = ascent * scale;
    font->height = (ascent - descent + gap) * scale;
    for (c = 32; c <= last; c++) {
        glyph_t* g = &font->glyphs[c - 32];
        int index = R_GetFontGlyphIndex(info, c);
        int advance, bearing;

        R_GetGlyphHMetrics(info, index, &advance, &bearing);
        g->advance = advance * scale;
        for (s = 0; s < FONT_SHIFTS; s++) {
            cell_t* cell = &g->cells[s];
            int x0, y0, x1, y1;

            R_GetGlyphBitmapBoxSubpixel(info, index, scale, scale, (float)s / FONT_SHIFTS, 0.0f, &x0, &y0, &x1, &y1);
            cell->w = x1 - x0;
            cell->h = y1 - y0;
            cell->ox = x0;
            cell->oy = y0;
            if (x + cell->w + 1 > atlasW) {
                x = 1;
                y += rowH + 1;
                rowH = 0;
            }
            cell->ax = x;
            cell->ay = y;
            x += cell->w + 1;
            if (cell->h > rowH) {
                rowH = cell->h;
            }
        }
    }
    font->cap = (float)-font->glyphs['H' - 32].cells[0].oy;
    atlasH = y + rowH + 1;
    alpha = calloc((size_t)atlasW, (size_t)atlasH);
    if (!alpha) {
        return NULL;
    }
    for (c = 32; c <= last; c++) {
        glyph_t* g = &font->glyphs[c - 32];
        int index = R_GetFontGlyphIndex(info, c);

        for (s = 0; s < FONT_SHIFTS; s++) {
            const cell_t* cell = &g->cells[s];

            if (cell->w > 0 && cell->h > 0) {
                R_MakeGlyphBitmapSubpixel(info, alpha + (size_t)cell->ay * atlasW + cell->ax, cell->w, cell->h, atlasW, scale, scale,
                                          (float)s / FONT_SHIFTS, 0.0f, index);
            }
        }
    }
    *outH = atlasH;
    return alpha;
}

static bool Font_Upload(font_t* font, unsigned char* alpha, int atlasH) {
    const int atlasW = 1024;
    SDL_Surface* surface;
    int i;

    surface = SDL_CreateSurface(atlasW, atlasH, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        free(alpha);
        return false;
    }
    for (i = 0; i < atlasH; i++) {
        Uint8* row = (Uint8*)surface->pixels + (size_t)i * surface->pitch;
        int j;

        for (j = 0; j < atlasW; j++) {
            row[j * 4 + 0] = 255;
            row[j * 4 + 1] = 255;
            row[j * 4 + 2] = 255;
            row[j * 4 + 3] = alpha[(size_t)i * atlasW + j];
        }
    }
    free(alpha);
    font->tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!font->tex) {
        return false;
    }
    SDL_SetTextureBlendMode(font->tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(font->tex, SDL_SCALEMODE_NEAREST);
    return true;
}

static bool Font_Bake(font_t* font, const stbtt_fontinfo* info, float pixels, bool em) {
    int h = 0;
    unsigned char* alpha = Font_Raster(font, info, pixels, em, 255, &h);

    return alpha && Font_Upload(font, alpha, h);
}

static const float fontSizes[FONT_COUNT] = {19.0f, 19.0f, 20.0f, 18.0f, 18.0f, 17.0f, 38.0f, 15.0f, 16.0f, 18.0f,
                                            17.0f, 11.0f, 12.5f, 13.0f, 11.0f, 14.0f, 10.0f, 14.0f, 12.0f, 11.0f, 12.0f,
                                         14.0f, 30.0f, 14.0f, 12.0f, 16.0f, 13.0f, 20.0f, 12.0f, 10.5f, 28.0f, 15.0f, 10.0f, 11.0f, 13.0f, 15.0f, 11.5f, 10.0f, 26.0f, 14.0f, 14.0f, 12.0f, 10.0f, 12.0f, 13.0f, 13.0f, 14.875f, 14.875f, 11.0f, 13.0f, 68.0f, 40.0f};
static const int fontFaces[FONT_COUNT] = {FACE_DISPLAY, FACE_DISPLAY, FACE_DISPLAY, FACE_BOLD,    FACE_TEXT,    FACE_TEXT,    FACE_DISPLAY,
                                         FACE_TEXT,    FACE_BOLD,    FACE_DISPLAY, FACE_DISPLAY, FACE_DISPLAY, FACE_STRONG,  FACE_STRONG,
                                         FACE_STRONG,  FACE_REGULAR, FACE_MEDIUM,  FACE_DISPLAY, FACE_REGULAR, FACE_DISPLAY, FACE_DISPLAY,
                                        FACE_DISPLAY, FACE_STRONG, FACE_STRONG, FACE_DISPLAY, FACE_STRONG, FACE_STRONG, FACE_DISPLAY, FACE_STRONG, FACE_STRONG, FACE_STRONG, FACE_TEXT, FACE_TEXT, FACE_STRONG, FACE_TEXT, FACE_STRONG, FACE_DISPLAY, FACE_MEDIUM, FACE_DISPLAY, FACE_TEXT, FACE_BOLD, FACE_DISPLAY, FACE_DISPLAY, FACE_TEXT, FACE_BOLD, FACE_DISPLAY, FACE_REGULAR, FACE_DISPLAY, FACE_REGULAR, FACE_DISPLAY, FACE_STRONG, FACE_STRONG};

static void Fonts_Load(void) {
    static const unsigned char* const data[FACE_COUNT] = {launcherFontText,    launcherFontBold,   launcherFontDisplay,
                                                          launcherFontRegular, launcherFontMedium, launcherFontStrong};
    int i;

    fontsReady = true;
    for (i = 0; fontsReady && i < FACE_COUNT; i++) {
        fontsReady = R_LoadFontTables(&faceInfo[i], data[i], 0) != 0;
    }
    for (i = 0; fontsReady && i < FONT_HUB_FIRST; i++) {
        fonts[i].face = fontFaces[i];
        fontsReady = Font_Bake(&fonts[i], &faceInfo[fontFaces[i]], fontSizes[i] * uiScale, i >= FONT_SV_NAME);
    }
}

static bool Fallback_Load(fallback_t* fb) {
    char path[1100];
    size_t size = 0;
    int offset = 0;

    if (fb->tried) {
        return fb->data != NULL;
    }
    fb->tried = true;
#ifdef _WIN32
    SDL_snprintf(path, sizeof(path), "%s\\Fonts\\%s", SDL_getenv("WINDIR") ? SDL_getenv("WINDIR") : "C:\\Windows", fb->file);
#else
    SDL_strlcpy(path, fb->file, sizeof(path));
#endif
    fb->data = SDL_LoadFile(path, &size);
    if (!fb->data) {
        return false;
    }
    if (size > 16 && !SDL_memcmp(fb->data, "ttcf", 4)) {
        offset = (int)(((Uint32)fb->data[12] << 24) | ((Uint32)fb->data[13] << 16) | ((Uint32)fb->data[14] << 8) | fb->data[15]);
        if ((size_t)offset >= size) {
            offset = -1;
        }
    }
    if (size < 64 || offset < 0 || !R_LoadFontTables(&fb->info, fb->data, offset)) {
        SDL_free(fb->data);
        fb->data = NULL;
        return false;
    }
    return true;
}

static bool Glyph_Ignored(Uint32 c) {
    return (c >= 0x200b && c <= 0x200f) || (c >= 0x202a && c <= 0x202e) || (c >= 0x2060 && c <= 0x2064) ||
           (c >= 0xfe00 && c <= 0xfe0f) || c == 0xfeff || (c >= 0xe0000 && c <= 0xe007f) || (c >= 0xe0100 && c <= 0xe01ef);
}

static bool Extra_Page(void) {
    unsigned char* zero;
    SDL_Texture* tex;

    if (extraPageCount >= EXTRA_PAGES) {
        return false;
    }
    tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, EXTRA_SIDE, EXTRA_SIDE);
    if (!tex) {
        return false;
    }
    zero = calloc((size_t)EXTRA_SIDE * EXTRA_SIDE, 4);
    if (!zero) {
        SDL_DestroyTexture(tex);
        return false;
    }
    SDL_UpdateTexture(tex, NULL, zero, EXTRA_SIDE * 4);
    free(zero);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    extraPages[extraPageCount++] = tex;
    extraX = extraY = 1;
    extraRow = 0;
    return true;
}

static bool Extra_Place(const stbtt_fontinfo* info, int index, float scale, glyph_t* g) {
    int s;

    for (s = 0; s < FONT_SHIFTS; s++) {
        cell_t* cell = &g->cells[s];
        int x0, y0, x1, y1;

        R_GetGlyphBitmapBoxSubpixel(info, index, scale, scale, (float)s / FONT_SHIFTS, 0.0f, &x0, &y0, &x1, &y1);
        cell->w = SDL_clamp(x1 - x0, 0, 255);
        cell->h = SDL_clamp(y1 - y0, 0, 255);
        cell->ox = x0;
        cell->oy = y0;
        if (extraX + cell->w + 1 > EXTRA_SIDE) {
            extraX = 1;
            extraY += extraRow + 1;
            extraRow = 0;
        }
        if (extraY + cell->h + 1 > EXTRA_SIDE) {
            return false;
        }
        cell->ax = extraX;
        cell->ay = extraY;
        extraX += cell->w + 1;
        if (cell->h > extraRow) {
            extraRow = cell->h;
        }
    }
    return true;
}

static int Glyph_Bake(const font_t* font, Uint32 cp, glyph_t* g) {
    const stbtt_fontinfo* info = &faceInfo[font->face];
    float scale = font->scale;
    int index = R_GetFontGlyphIndex(info, (int)cp), pass, i, s, advance, bearing, page;
    bool bold = font->face == FACE_BOLD || font->face == FACE_DISPLAY || font->face == FACE_STRONG;
    unsigned char* alpha;
    unsigned char* rgba;

    for (pass = 0; !index && pass < 2; pass++) {
        for (i = 0; !index && i < (int)SDL_arraysize(fallbacks); i++) {
            fallback_t* fb = &fallbacks[i];

            if ((pass == 0) != (fb->bold == bold) || !Fallback_Load(fb)) {
                continue;
            }
            index = R_GetFontGlyphIndex(&fb->info, (int)cp);
            if (index) {
                info = &fb->info;
                scale = R_ScaleForMappingEmToPixels(info, font->em);
            }
        }
    }
    if (!index || (!extraPageCount && !Extra_Page())) {
        return -1;
    }
    R_GetGlyphHMetrics(info, index, &advance, &bearing);
    g->advance = advance * scale;
    if (!Extra_Place(info, index, scale, g) && (!Extra_Page() || !Extra_Place(info, index, scale, g))) {
        return -1;
    }
    page = extraPageCount - 1;
    alpha = malloc(255 * 255);
    rgba = malloc(255 * 255 * 4);
    for (s = 0; alpha && rgba && s < FONT_SHIFTS; s++) {
        const cell_t* cell = &g->cells[s];
        SDL_Rect r;

        if (cell->w <= 0 || cell->h <= 0) {
            continue;
        }
        SDL_memset(alpha, 0, (size_t)cell->w * cell->h);
        R_MakeGlyphBitmapSubpixel(info, alpha, cell->w, cell->h, cell->w, scale, scale, (float)s / FONT_SHIFTS, 0.0f, index);
        for (i = 0; i < cell->w * cell->h; i++) {
            rgba[i * 4 + 0] = rgba[i * 4 + 1] = rgba[i * 4 + 2] = 255;
            rgba[i * 4 + 3] = alpha[i];
        }
        r.x = cell->ax;
        r.y = cell->ay;
        r.w = cell->w;
        r.h = cell->h;
        SDL_UpdateTexture(extraPages[page], &r, rgba, cell->w * 4);
    }
    free(alpha);
    free(rgba);
    return page;
}

static void Pixels_Halve(unsigned char* p, int* w, int* h);
static unsigned char* Pixels_Fit(const unsigned char* rgba, int w, int h, int outW, int outH);

static bool Emoji_Default(Uint32 cp) {
    static const Uint32 ranges[][2] = {
        {0x231a, 0x231b}, {0x23e9, 0x23ec}, {0x23f0, 0x23f0}, {0x23f3, 0x23f3}, {0x25fd, 0x25fe}, {0x2614, 0x2615},
        {0x2648, 0x2653}, {0x267f, 0x267f}, {0x2693, 0x2693}, {0x26a1, 0x26a1}, {0x26aa, 0x26ab}, {0x26bd, 0x26be},
        {0x26c4, 0x26c5}, {0x26ce, 0x26ce}, {0x26d4, 0x26d4}, {0x26ea, 0x26ea}, {0x26f2, 0x26f3}, {0x26f5, 0x26f5},
        {0x26fa, 0x26fa}, {0x26fd, 0x26fd}, {0x2705, 0x2705}, {0x270a, 0x270b}, {0x2728, 0x2728}, {0x274c, 0x274c},
        {0x274e, 0x274e}, {0x2753, 0x2755}, {0x2757, 0x2757}, {0x2795, 0x2797}, {0x27b0, 0x27b0}, {0x27bf, 0x27bf},
        {0x2b1b, 0x2b1c}, {0x2b50, 0x2b50}, {0x2b55, 0x2b55},
    };
    int i;

    if (cp >= 0x1f000 && cp < 0x1fb00) {
        return true;
    }
    for (i = 0; i < (int)SDL_arraysize(ranges); i++) {
        if (cp >= ranges[i][0] && cp <= ranges[i][1]) {
            return true;
        }
    }
    return false;
}

static int Glyph_Emoji(const font_t* font, Uint32 cp, glyph_t* g) {
    char name[32];
    size_t size = 0;
    unsigned char* file;
    unsigned char* rgba = NULL;
    unsigned char* fit = NULL;
    SDL_IOStream* io;
    SDL_Surface* png;
    SDL_Surface* surface;
    int n, w, h, y, s, page = -1;
    SDL_Rect r;

    if (!Emoji_Default(cp)) {
        return -1;
    }
    SDL_snprintf(name, sizeof(name), "emoji/%04x.png", (unsigned)cp);
    file = Pak_Read(PAK_OURS, name, &size);
    if (!file) {
        return -1;
    }
    io = SDL_IOFromConstMem(file, size);
    png = io ? SDL_LoadPNG_IO(io, true) : NULL;
    surface = png ? SDL_ConvertSurface(png, SDL_PIXELFORMAT_RGBA32) : NULL;
    free(file);
    if (png) {
        SDL_DestroySurface(png);
    }
    if (!surface) {
        return -1;
    }
    n = SDL_clamp((int)SDL_roundf(font->em), 1, 254);
    w = surface->w;
    h = surface->h;
    rgba = malloc((size_t)w * h * 4);
    if (rgba) {
        for (y = 0; y < h; y++) {
            memcpy(rgba + (size_t)y * w * 4, (Uint8*)surface->pixels + (size_t)y * surface->pitch, (size_t)w * 4);
        }
        while (w / 2 >= n && h / 2 >= n) {
            Pixels_Halve(rgba, &w, &h);
        }
        fit = Pixels_Fit(rgba, w, h, n, n);
    }
    SDL_DestroySurface(surface);
    free(rgba);
    if (!fit || (!extraPageCount && !Extra_Page())) {
        free(fit);
        return -1;
    }
    if (extraX + n + 1 > EXTRA_SIDE) {
        extraX = 1;
        extraY += extraRow + 1;
        extraRow = 0;
    }
    if (extraY + n + 1 > EXTRA_SIDE && !Extra_Page()) {
        free(fit);
        return -1;
    }
    page = extraPageCount - 1;
    for (s = 0; s < FONT_SHIFTS; s++) {
        g->cells[s].ax = extraX;
        g->cells[s].ay = extraY;
        g->cells[s].w = n;
        g->cells[s].h = n;
        g->cells[s].ox = 0;
        g->cells[s].oy = -(int)SDL_roundf((font->cap + (float)n) * 0.5f);
    }
    r.x = extraX;
    r.y = extraY;
    r.w = n;
    r.h = n;
    SDL_UpdateTexture(extraPages[page], &r, fit, n * 4);
    free(fit);
    extraX += n + 1;
    if (n > extraRow) {
        extraRow = n;
    }
    g->advance = (float)n * 1.1f;
    g->color = true;
    return page;
}

static const glyph_t* Glyph_Get(int fontIndex, Uint32 cp, SDL_Texture** tex) {
    static const glyph_t none;
    font_t* font = &fonts[fontIndex];
    unsigned slot;
    extra_t* e;

    *tex = font->tex;
    if (cp >= 32 && cp < 256) {
        return &font->glyphs[cp - 32];
    }
    if (Glyph_Ignored(cp)) {
        return &none;
    }
    slot = (cp * 2654435761u + (unsigned)fontIndex * 40503u) & (EXTRA_SLOTS - 1);
    while (extraSlots[slot]) {
        e = &extras[extraSlots[slot] - 1];
        if (e->cp == cp && e->font == fontIndex) {
            if (e->page < 0) {
                return &font->glyphs['?' - 32];
            }
            *tex = extraPages[e->page];
            return &e->glyph;
        }
        slot = (slot + 1) & (EXTRA_SLOTS - 1);
    }
    if (extraCount >= EXTRA_MAX) {
        return &font->glyphs['?' - 32];
    }
    e = &extras[extraCount];
    e->cp = cp;
    e->font = fontIndex;
    e->page = Glyph_Emoji(font, cp, &e->glyph);
    if (e->page < 0) {
        e->page = Glyph_Bake(font, cp, &e->glyph);
    }
    extraSlots[slot] = (Uint16)++extraCount;
    if (e->page < 0) {
        return &font->glyphs['?' - 32];
    }
    *tex = extraPages[e->page];
    return &e->glyph;
}

static void Pixels_Halve(unsigned char* p, int* w, int* h) {
    int sw = *w, nw = *w / 2, nh = *h / 2, x, y, c;

    for (y = 0; y < nh; y++) {
        for (x = 0; x < nw; x++) {
            const unsigned char* s0 = p + ((size_t)(y * 2) * sw + x * 2) * 4;
            const unsigned char* s1 = s0 + 4;
            const unsigned char* s2 = s0 + (size_t)sw * 4;
            const unsigned char* s3 = s2 + 4;
            unsigned a = (unsigned)s0[3] + s1[3] + s2[3] + s3[3];
            unsigned char px[4];

            for (c = 0; c < 3; c++) {
                px[c] = a ? (unsigned char)(((unsigned)s0[c] * s0[3] + (unsigned)s1[c] * s1[3] + (unsigned)s2[c] * s2[3] +
                                             (unsigned)s3[c] * s3[3] + a / 2) /
                                            a)
                          : 0;
            }
            px[3] = (unsigned char)((a + 2) / 4);
            memcpy(p + ((size_t)y * nw + x) * 4, px, 4);
        }
    }
    *w = nw;
    *h = nh;
}

static float Kernel(float x) {
    if (x < 0.0f) {
        x = -x;
    }
    if (x < 0.0001f) {
        return 1.0f;
    }
    if (x >= 3.0f) {
        return 0.0f;
    }
    x *= SDL_PI_F;
    return 3.0f * SDL_sinf(x) * SDL_sinf(x / 3.0f) / (x * x);
}

static bool Pixels_Stretch(const unsigned char* from, int fromN, size_t fromStep, size_t fromLine, unsigned char* to, int toN, size_t toStep,
                           size_t toLine, int lines, float start, float span) {
    float scale = span / (float)toN, wide = scale > 1.0f ? scale : 1.0f;
    int taps = (int)(wide * 6.0f) + 2, i, j, k, c;
    float* weights = malloc((size_t)toN * taps * sizeof(float));
    int* first = malloc((size_t)toN * sizeof(int));

    if (!weights || !first) {
        free(weights);
        free(first);
        return false;
    }
    for (j = 0; j < toN; j++) {
        float center = start + ((float)j + 0.5f) * scale - 0.5f, sum = 0.0f;

        first[j] = (int)SDL_floorf(center - wide * 3.0f) + 1;
        for (k = 0; k < taps; k++) {
            float w = Kernel(((float)(first[j] + k) - center) / wide);

            weights[(size_t)j * taps + k] = w;
            sum += w;
        }
        for (k = 0; k < taps; k++) {
            weights[(size_t)j * taps + k] /= sum;
        }
    }
    for (i = 0; i < lines; i++) {
        const unsigned char* src = from + (size_t)i * fromLine;
        unsigned char* dst = to + (size_t)i * toLine;

        for (j = 0; j < toN; j++) {
            float acc[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            for (k = 0; k < taps; k++) {
                const unsigned char* px = src + (size_t)SDL_clamp(first[j] + k, 0, fromN - 1) * fromStep;
                float w = weights[(size_t)j * taps + k];

                for (c = 0; c < 4; c++) {
                    acc[c] += (float)px[c] * w;
                }
            }
            for (c = 0; c < 4; c++) {
                dst[(size_t)j * toStep + c] = (unsigned char)SDL_clamp(acc[c] + 0.5f, 0.0f, 255.0f);
            }
        }
    }
    free(weights);
    free(first);
    return true;
}

static unsigned char* Pixels_Fit(const unsigned char* rgba, int w, int h, int outW, int outH) {
    float want, cw = (float)w, ch = (float)h, x0, y0;
    unsigned char* mid;
    unsigned char* out;

    if (outW < 1 || outH < 1) {
        return NULL;
    }
    want = (float)outW / (float)outH;
    if (cw / ch > want) {
        cw = ch * want;
    } else {
        ch = cw / want;
    }
    x0 = ((float)w - cw) * 0.5f;
    y0 = ((float)h - ch) * 0.5f;
    if (w >= outW && h >= outH && cw <= (float)outW * ART_SNAP) {
        cw = (float)outW;
        ch = (float)outH;
        x0 = (float)((w - outW) / 2);
        y0 = (float)((h - outH) / 2);
    }
    mid = malloc((size_t)outW * h * 4);
    out = malloc((size_t)outW * outH * 4);
    if (!mid || !out || !Pixels_Stretch(rgba, w, 4, (size_t)w * 4, mid, outW, 4, (size_t)outW * 4, h, x0, cw) ||
        !Pixels_Stretch(mid, h, (size_t)outW * 4, 4, out, outH, (size_t)outW * 4, 4, outW, y0, ch)) {
        free(out);
        out = NULL;
    }
    free(mid);
    return out;
}

static void Image_Make(image_t* image, unsigned char* rgba, int w, int h, float target) {
    int want = (int)(target * uiScale + 0.5f);
    SDL_Surface* surface;

    while (w / 2 >= want && h / 2 >= 1) {
        Pixels_Halve(rgba, &w, &h);
    }
    surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, rgba, w * 4);
    if (!surface) {
        return;
    }
    image->tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (image->tex) {
        SDL_SetTextureBlendMode(image->tex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(image->tex, SDL_SCALEMODE_LINEAR);
        image->w = w;
        image->h = h;
    }
}

static void Image_FromJpeg(image_t* image, const unsigned char* file, size_t size, const float* box) {
    int outW = (int)(SDL_roundf((box[0] + box[2]) * uiScale) - SDL_roundf(box[0] * uiScale));
    int outH = (int)(SDL_roundf((box[1] + box[3]) * uiScale) - SDL_roundf(box[1] * uiScale));
    unsigned char* rgb;
    unsigned char* rgba;
    unsigned char* fit;
    int w = 0, h = 0, i, n;

    rgb = Launcher_DecodeJpeg(file, (unsigned long)size, &w, &h, 1);
    if (!rgb) {
        return;
    }
    n = w * h;
    rgba = malloc((size_t)n * 4);
    if (rgba) {
        for (i = 0; i < n; i++) {
            rgba[i * 4 + 0] = rgb[i * 3 + 0];
            rgba[i * 4 + 1] = rgb[i * 3 + 1];
            rgba[i * 4 + 2] = rgb[i * 3 + 2];
            rgba[i * 4 + 3] = 255;
        }
        while (w / 2 >= outW && h / 2 >= outH) {
            Pixels_Halve(rgba, &w, &h);
        }
        fit = Pixels_Fit(rgba, w, h, outW, outH);
        if (fit) {
            Image_Make(image, fit, outW, outH, (float)outW);
            free(fit);
        } else {
            Image_Make(image, rgba, w, h, box[2]);
        }
        free(rgba);
    }
    free(rgb);
}

static void Image_LoadHero(void) {
    char path[1024];
    size_t size = 0;
    unsigned char* file;

    SDL_snprintf(path, sizeof(path), "%s%s", basePath, ART_CUSTOM);
    file = SDL_LoadFile(path, &size);
    if (file) {
        Image_FromJpeg(&hero, file, size, artBox[1]);
        SDL_free(file);
        heroCustom = hero.tex != NULL;
    }
    if (!hero.tex && install.local) {
        unsigned char* art = Pak_Read(install.zip, ART_CUSTOM, &size);

        if (art) {
            Image_FromJpeg(&hero, art, size, artBox[1]);
            free(art);
            heroCustom = hero.tex != NULL;
        }
    }
    if (!hero.tex) {
        file = Pak_Read(PAK_OURS, ART_HERO, &size);
        if (file) {
            Image_FromJpeg(&hero, file, size, artBox[0]);
            free(file);
        }
    }
}

static void Image_Vignette(void) {
    unsigned char* rgba = malloc(VIG_W * VIG_H * 4);
    int x, y;

    if (!rgba) {
        return;
    }
    for (y = 0; y < VIG_H; y++) {
        for (x = 0; x < VIG_W; x++) {
            float u = ((float)x + 0.5f) / (float)VIG_W * 2.0f - 1.0f, v = ((float)y + 0.5f) / (float)VIG_H * 2.0f - 1.0f;
            float t = SDL_clamp((SDL_sqrtf((u * u + v * v) * 0.5f) - 0.25f) / 0.75f, 0.0f, 1.0f);
            unsigned char* p = rgba + (y * VIG_W + x) * 4;

            p[0] = p[1] = p[2] = 0;
            p[3] = (unsigned char)(255.0f * (VIG_BASE + VIG_EDGE * t * t * (3.0f - 2.0f * t)) + 0.5f);
        }
    }
    Image_Make(&shade, rgba, VIG_W, VIG_H, UI_W);
    free(rgba);
}

static void Image_FromPng(image_t* image, const unsigned char* data, unsigned int size, float target) {
    SDL_IOStream* io = SDL_IOFromConstMem(data, size);
    SDL_Surface* file = io ? SDL_LoadPNG_IO(io, true) : NULL;
    SDL_Surface* surface = file ? SDL_ConvertSurface(file, SDL_PIXELFORMAT_RGBA32) : NULL;
    unsigned char* rgba;
    int y;

    if (file) {
        SDL_DestroySurface(file);
    }
    if (!surface) {
        return;
    }
    rgba = malloc((size_t)surface->w * surface->h * 4);
    if (rgba) {
        for (y = 0; y < surface->h; y++) {
            memcpy(rgba + (size_t)y * surface->w * 4, (Uint8*)surface->pixels + (size_t)y * surface->pitch, (size_t)surface->w * 4);
        }
        Image_Make(image, rgba, surface->w, surface->h, target);
        free(rgba);
    }
    SDL_DestroySurface(surface);
}

static float Seg_Dist(float px, float py, float ax, float ay, float bx, float by) {
    float dx = bx - ax, dy = by - ay;
    float s = ((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy);

    s = s < 0.0f ? 0.0f : s > 1.0f ? 1.0f : s;
    dx = ax + dx * s - px;
    dy = ay + dy * s - py;
    return SDL_sqrtf(dx * dx + dy * dy);
}

static void Icon_Lines(image_t* image, float size, const float* pts, int count, float weight) {
    int n = (int)(size * uiScale + 0.5f), x, y, sx, sy, i;
    float half = n * weight;
    unsigned char* rgba = n > 0 ? malloc((size_t)n * n * 4) : NULL;

    if (!rgba) {
        return;
    }
    for (y = 0; y < n; y++) {
        for (x = 0; x < n; x++) {
            int hits = 0;

            for (sy = 0; sy < 4; sy++) {
                for (sx = 0; sx < 4; sx++) {
                    float px = x + (sx + 0.5f) * 0.25f, py = y + (sy + 0.5f) * 0.25f;

                    for (i = 0; i + 1 < count; i++) {
                        if (Seg_Dist(px, py, pts[i * 2] * n, pts[i * 2 + 1] * n, pts[i * 2 + 2] * n, pts[i * 2 + 3] * n) <= half) {
                            hits++;
                            break;
                        }
                    }
                }
            }
            rgba[(y * n + x) * 4 + 0] = 255;
            rgba[(y * n + x) * 4 + 1] = 255;
            rgba[(y * n + x) * 4 + 2] = 255;
            rgba[(y * n + x) * 4 + 3] = (unsigned char)(hits * 255 / 16);
        }
    }
    Image_Make(image, rgba, n, n, size);
    free(rgba);
}

static void Icon_Gear(image_t* image, float size) {
    int n = (int)(size * uiScale + 0.5f), x, y, sx, sy;
    unsigned char* rgba = n > 0 ? malloc((size_t)n * n * 4) : NULL;

    if (!rgba) {
        return;
    }
    for (y = 0; y < n; y++) {
        for (x = 0; x < n; x++) {
            int hits = 0;

            for (sy = 0; sy < 4; sy++) {
                for (sx = 0; sx < 4; sx++) {
                    float dx = x + (sx + 0.5f) * 0.25f - n * 0.5f, dy = y + (sy + 0.5f) * 0.25f - n * 0.5f;
                    float d = SDL_sqrtf(dx * dx + dy * dy) / (float)n;
                    float f = (SDL_atan2f(dy, dx) / (2.0f * SDL_PI_F) + 1.0f) * 8.0f;
                    float edge;

                    f -= SDL_floorf(f);
                    edge = (f > 0.25f && f < 0.75f) ? 0.49f : 0.37f;
                    if (d <= edge && d >= 0.15f) {
                        hits++;
                    }
                }
            }
            rgba[(y * n + x) * 4 + 0] = 255;
            rgba[(y * n + x) * 4 + 1] = 255;
            rgba[(y * n + x) * 4 + 2] = 255;
            rgba[(y * n + x) * 4 + 3] = (unsigned char)(hits * 255 / 16);
        }
    }
    Image_Make(image, rgba, n, n, size);
    free(rgba);
}

static void Icons_Load(void) {
    static const float more[6] = {0.2f, 0.64f, 0.5f, 0.36f, 0.8f, 0.64f};
    static const float check[6] = {0.2f, 0.52f, 0.42f, 0.74f, 0.8f, 0.3f};
    static const float slash[4] = {0.1f, 0.1f, 0.9f, 0.9f};

    Icon_Lines(&iconMore, MORE_ICON, more, 3, 0.065f);
    Icon_Lines(&iconCheck, CHECK_ICON, check, 3, 0.09f);
    Icon_Lines(&iconSlash, CLOSE_ICON, slash, 2, 0.05f);
    Icon_Gear(&iconGear, GEAR_ICON);
}

static void Draw_Color(Uint32 rgba) {
    SDL_SetRenderDrawColor(renderer, (Uint8)(rgba >> 24), (Uint8)(rgba >> 16), (Uint8)(rgba >> 8), (Uint8)rgba);
}

static Uint32 Col_Fade(Uint32 rgba) {
    return slideFade >= 1.0f ? rgba : (rgba & 0xFFFFFF00u) | (Uint32)((float)(rgba & 255) * slideFade + 0.5f);
}

static void Draw_Rect(float x, float y, float w, float h, Uint32 rgba) {
    SDL_FRect r;

    x += slideX;
    rgba = Col_Fade(rgba);
    r.x = SDL_roundf(x * uiScale);
    r.y = SDL_roundf(y * uiScale);
    r.w = SDL_roundf((x + w) * uiScale) - r.x;
    r.h = SDL_roundf((y + h) * uiScale) - r.y;
    Draw_Color(rgba);
    SDL_RenderFillRect(renderer, &r);
}

static Uint32 Col_Mix(Uint32 a, Uint32 b, float t) {
    Uint32 out = 0;
    int shift;

    for (shift = 24; shift >= 0; shift -= 8) {
        float ca = (float)((a >> shift) & 255), cb = (float)((b >> shift) & 255);

        out |= (Uint32)(ca + (cb - ca) * t + 0.5f) << shift;
    }
    return out;
}

static Uint32 Col_Alpha(Uint32 rgba, float alpha) {
    return (rgba & 0xFFFFFF00u) | (Uint32)((float)(rgba & 255) * alpha + 0.5f);
}

static void Shape_Make(shape_t* s, float x, float y, float w, float h, float radius) {
    static const float start[4] = {180.0f, 270.0f, 0.0f, 90.0f};
    float x0 = SDL_roundf((x + slideX) * uiScale), y0 = SDL_roundf(y * uiScale);
    float x1 = SDL_roundf((x + slideX + w) * uiScale), y1 = SDL_roundf((y + h) * uiScale);
    float r = SDL_min(radius * uiScale, SDL_min(x1 - x0, y1 - y0) * 0.5f);
    float nx[SHAPE_MAX], ny[SHAPE_MAX];
    int steps, corner, i;

    if (r < 1.0f) {
        r = 0.0f;
    }
    steps = r > 0.0f ? SDL_clamp((int)SDL_ceilf(r * 0.6f), 2, (SHAPE_MAX / 4) - 1) : 0;
    s->n = 0;
    for (corner = 0; corner < 4; corner++) {
        float cx = corner == 0 || corner == 3 ? x0 + r : x1 - r;
        float cy = corner < 2 ? y0 + r : y1 - r;

        for (i = 0; i <= steps; i++) {
            float a = (start[corner] + (steps ? 90.0f * (float)i / (float)steps : 0.0f)) * SDL_PI_F / 180.0f;
            float px = cx + r * SDL_cosf(a), py = cy + r * SDL_sinf(a);

            if (s->n > 0 && SDL_fabsf(px - s->x[s->n - 1]) < 0.01f && SDL_fabsf(py - s->y[s->n - 1]) < 0.01f) {
                continue;
            }
            s->x[s->n] = px;
            s->y[s->n] = py;
            s->n++;
        }
    }
    if (s->n > 2 && SDL_fabsf(s->x[s->n - 1] - s->x[0]) < 0.01f && SDL_fabsf(s->y[s->n - 1] - s->y[0]) < 0.01f) {
        s->n--;
    }
    for (i = 0; i < s->n; i++) {
        int j = (i + 1) % s->n;
        float dx = s->x[j] - s->x[i], dy = s->y[j] - s->y[i], len = SDL_sqrtf(dx * dx + dy * dy);

        nx[i] = len > 0.0f ? dy / len : 0.0f;
        ny[i] = len > 0.0f ? -dx / len : 0.0f;
    }
    for (i = 0; i < s->n; i++) {
        int k = (i + s->n - 1) % s->n;
        float d = 1.0f + nx[k] * nx[i] + ny[k] * ny[i];

        s->mx[i] = d > 0.01f ? (nx[k] + nx[i]) / d : nx[i];
        s->my[i] = d > 0.01f ? (ny[k] + ny[i]) / d : ny[i];
    }
}

static void Mesh_Point(mesh_t* m, float x, float y, Uint32 rgba, float alpha) {
    SDL_Vertex* v = &m->v[m->nv++];

    v->position.x = x;
    v->position.y = y;
    v->color.r = (float)((rgba >> 24) & 255) / 255.0f;
    v->color.g = (float)((rgba >> 16) & 255) / 255.0f;
    v->color.b = (float)((rgba >> 8) & 255) / 255.0f;
    v->color.a = (float)(rgba & 255) / 255.0f * alpha;
    v->tex_coord.x = 0.0f;
    v->tex_coord.y = 0.0f;
}

static void Mesh_Tri(mesh_t* m, int a, int b, int c) {
    m->idx[m->ni++] = a;
    m->idx[m->ni++] = b;
    m->idx[m->ni++] = c;
}

static void Mesh_Ring(mesh_t* m, const shape_t* s, float da, float aa, float db, float ab, Uint32 rgba) {
    int base = m->nv, i;

    for (i = 0; i < s->n; i++) {
        Mesh_Point(m, s->x[i] + s->mx[i] * da, s->y[i] + s->my[i] * da, rgba, aa);
        Mesh_Point(m, s->x[i] + s->mx[i] * db, s->y[i] + s->my[i] * db, rgba, ab);
    }
    for (i = 0; i < s->n; i++) {
        int a = base + i * 2, b = base + (i + 1) % s->n * 2;

        Mesh_Tri(m, a, b, b + 1);
        Mesh_Tri(m, a, b + 1, a + 1);
    }
}

static void Mesh_Fill(mesh_t* m, const shape_t* s, Uint32 rgba) {
    float cx = 0.0f, cy = 0.0f;
    int i;

    m->nv = m->ni = 0;
    for (i = 0; i < s->n; i++) {
        cx += s->x[i];
        cy += s->y[i];
    }
    Mesh_Point(m, cx / (float)s->n, cy / (float)s->n, rgba, 1.0f);
    for (i = 0; i < s->n; i++) {
        Mesh_Point(m, s->x[i] - s->mx[i] * 0.5f, s->y[i] - s->my[i] * 0.5f, rgba, 1.0f);
    }
    for (i = 0; i < s->n; i++) {
        Mesh_Tri(m, 0, 1 + i, 1 + (i + 1) % s->n);
    }
    Mesh_Ring(m, s, -0.5f, 1.0f, 0.5f, 0.0f, rgba);
}

static void Draw_Round(float x, float y, float w, float h, float radius, Uint32 rgba) {
    shape_t s;
    mesh_t m;

    rgba = Col_Fade(rgba);
    if (w <= 0.0f || h <= 0.0f || !(rgba & 255)) {
        return;
    }
    Shape_Make(&s, x, y, w, h, radius);
    Mesh_Fill(&m, &s, rgba);
    SDL_RenderGeometry(renderer, NULL, m.v, m.nv, m.idx, m.ni);
}

static void Draw_RoundLine(float x, float y, float w, float h, float radius, Uint32 rgba) {
    shape_t s;
    mesh_t m;

    rgba = Col_Fade(rgba);
    if (w <= 0.0f || h <= 0.0f || !(rgba & 255)) {
        return;
    }
    Shape_Make(&s, x, y, w, h, radius);
    m.nv = m.ni = 0;
    Mesh_Ring(&m, &s, 0.5f, 0.0f, -0.5f, 1.0f, rgba);
    Mesh_Ring(&m, &s, -0.5f, 1.0f, -1.5f, 0.0f, rgba);
    SDL_RenderGeometry(renderer, NULL, m.v, m.nv, m.idx, m.ni);
}

static void Draw_Image(const image_t* image, float x, float y, float w, float h, float alpha) {
    SDL_FRect src, dst;
    float want, have;

    if (!image->tex || w <= 0.0f || h <= 0.0f) {
        return;
    }
    want = w / h;
    have = (float)image->w / (float)image->h;
    if (have > want) {
        src.h = (float)image->h;
        src.w = src.h * want;
        src.x = ((float)image->w - src.w) * 0.5f;
        src.y = 0.0f;
    } else {
        src.w = (float)image->w;
        src.h = src.w / want;
        src.x = 0.0f;
        src.y = ((float)image->h - src.h) * 0.5f;
    }
    x += slideX;
    alpha *= slideFade;
    dst.x = SDL_roundf(x * uiScale);
    dst.y = SDL_roundf(y * uiScale);
    dst.w = SDL_roundf((x + w) * uiScale) - dst.x;
    dst.h = SDL_roundf((y + h) * uiScale) - dst.y;
    SDL_SetTextureAlphaMod(image->tex, (Uint8)(alpha * 255.0f + 0.5f));
    SDL_RenderTexture(renderer, image->tex, image->w == (int)dst.w && image->h == (int)dst.h ? NULL : &src, &dst);
}

static void Draw_RoundImage(const image_t* image, float x, float y, float w, float h, float radius, Uint32 rgba) {
    shape_t s;
    mesh_t m;
    float want, have, u0 = 0.0f, v0 = 0.0f, uw = 1.0f, vh = 1.0f, x0, y0, x1, y1;
    int i;

    rgba = Col_Fade(rgba);
    if (!image->tex || w <= 0.0f || h <= 0.0f || !(rgba & 255)) {
        return;
    }
    want = w / h;
    have = (float)image->w / (float)image->h;
    if (have > want) {
        uw = want / have;
        u0 = (1.0f - uw) * 0.5f;
    } else {
        vh = have / want;
        v0 = (1.0f - vh) * 0.5f;
    }
    Shape_Make(&s, x, y, w, h, radius);
    Mesh_Fill(&m, &s, rgba);
    x0 = SDL_roundf((x + slideX) * uiScale);
    y0 = SDL_roundf(y * uiScale);
    x1 = SDL_roundf((x + slideX + w) * uiScale);
    y1 = SDL_roundf((y + h) * uiScale);
    for (i = 0; i < m.nv; i++) {
        float fx = SDL_clamp((m.v[i].position.x - x0) / (x1 - x0), 0.0f, 1.0f);
        float fy = SDL_clamp((m.v[i].position.y - y0) / (y1 - y0), 0.0f, 1.0f);

        m.v[i].tex_coord.x = u0 + fx * uw;
        m.v[i].tex_coord.y = v0 + fy * vh;
    }
    SDL_RenderGeometry(renderer, image->tex, m.v, m.nv, m.idx, m.ni);
}

static void Draw_Spin(const image_t* image, float x, float y, float angle, Uint32 rgba) {
    SDL_FRect dst;

    if (!image->tex) {
        return;
    }
    rgba = Col_Fade(rgba);
    dst.x = SDL_roundf((x + slideX) * uiScale);
    dst.y = SDL_roundf(y * uiScale);
    dst.w = (float)image->w;
    dst.h = (float)image->h;
    SDL_SetTextureColorMod(image->tex, (Uint8)(rgba >> 24), (Uint8)(rgba >> 16), (Uint8)(rgba >> 8));
    SDL_SetTextureAlphaMod(image->tex, (Uint8)rgba);
    SDL_RenderTextureRotated(renderer, image->tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

static Uint32 Text_Code(const char** text) {
    const unsigned char* p = (const unsigned char*)*text;
    Uint32 c = *p++;

    if (c >= 0xf0 && p[0] && p[1] && p[2]) {
        c = ((c & 0x07) << 18) | ((Uint32)(p[0] & 0x3f) << 12) | ((Uint32)(p[1] & 0x3f) << 6) | (p[2] & 0x3f);
        p += 3;
    } else if (c >= 0xe0 && p[0] && p[1]) {
        c = ((c & 0x0f) << 12) | ((Uint32)(p[0] & 0x3f) << 6) | (p[1] & 0x3f);
        p += 2;
    } else if (c >= 0xc0 && p[0]) {
        c = ((c & 0x1f) << 6) | (p[0] & 0x3f);
        p += 1;
    }
    *text = (const char*)p;
    return c;
}

static int Utf8_Put(char* out, Uint32 c) {
    if (c < 0x80) {
        out[0] = (char)c;
        return 1;
    }
    if (c < 0x800) {
        out[0] = (char)(0xc0 | (c >> 6));
        out[1] = (char)(0x80 | (c & 0x3f));
        return 2;
    }
    if (c < 0x10000) {
        out[0] = (char)(0xe0 | (c >> 12));
        out[1] = (char)(0x80 | ((c >> 6) & 0x3f));
        out[2] = (char)(0x80 | (c & 0x3f));
        return 3;
    }
    out[0] = (char)(0xf0 | (c >> 18));
    out[1] = (char)(0x80 | ((c >> 12) & 0x3f));
    out[2] = (char)(0x80 | ((c >> 6) & 0x3f));
    out[3] = (char)(0x80 | (c & 0x3f));
    return 4;
}

static Uint32 Upper_Cp(Uint32 c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    if (c < 0xe0) {
        return c;
    }
    if (c <= 0xfe) {
        return c == 0xf7 ? c : c - 32;
    }
    if (c == 0xff) {
        return 0x178;
    }
    if (c <= 0x17f) {
        if (c == 0x138 || c == 0x149 || c == 0x17f) {
            return c;
        }
        if ((c >= 0x139 && c <= 0x148) || (c >= 0x179 && c <= 0x17e)) {
            return (c & 1) ? c : c - 1;
        }
        return (c & 1) ? c - 1 : c;
    }
    if (c >= 0x3b1 && c <= 0x3cb) {
        return c == 0x3c2 ? 0x3a3 : c - 0x20;
    }
    if (c == 0x3ac) {
        return 0x386;
    }
    if (c >= 0x3ad && c <= 0x3af) {
        return c - 0x25;
    }
    if (c == 0x3cc) {
        return 0x38c;
    }
    if (c == 0x3cd || c == 0x3ce) {
        return c - 0x3f;
    }
    if (c >= 0x430 && c <= 0x44f) {
        return c - 0x20;
    }
    if (c >= 0x450 && c <= 0x45f) {
        return c - 0x50;
    }
    if ((c >= 0x460 && c <= 0x481) || (c >= 0x48a && c <= 0x4bf) || (c >= 0x4d0 && c <= 0x52f) || (c >= 0x1e00 && c <= 0x1e95) ||
        (c >= 0x1ea0 && c <= 0x1eff)) {
        return (c & 1) ? c - 1 : c;
    }
    if (c >= 0x4c1 && c <= 0x4ce) {
        return (c & 1) ? c : c - 1;
    }
    if (c >= 0x561 && c <= 0x586) {
        return c - 0x30;
    }
    if (c >= 0xff41 && c <= 0xff5a) {
        return c - 0x20;
    }
    return c;
}

static int Text_Next(const char** text) {
    Uint32 c = Text_Code(text);

    switch (c) {
        case 0x2018:
        case 0x2019:
            return '\'';
        case 0x201c:
        case 0x201d:
            return '"';
        case 0x2013:
        case 0x2014:
            return '-';
        case 0x2022:
            return 0xb7;
        case 0x2026:
            return '.';
        case 0x2192:
            return '>';
        default:
            break;
    }
    return c < 32 ? '?' : (int)c;
}

static float Text_Width(int fontIndex, const char* text, int bytes) {
    const char* end = bytes < 0 ? text + strlen(text) : text + bytes;
    float w = 0.0f;
    int count = 0;

    if (!fontsReady) {
        return (float)(end - text) * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    }
    while (text < end && *text) {
        SDL_Texture* tex;

        w += Glyph_Get(fontIndex, (Uint32)Text_Next(&text), &tex)->advance;
        count++;
    }
    if (count > 1) {
        w += (float)(count - 1) * textTrack * uiScale;
    }
    return w / uiScale;
}

static const char* Text_Upper(const char* text) {
    static char out[4][192];
    static int slot;
    char* o = out[slot = (slot + 1) & 3];
    size_t n = 0;

    while (*text) {
        const char* start = text;
        Uint32 c = Text_Code(&text);
        Uint32 u = Upper_Cp(c);
        size_t len = (size_t)(text - start);
        char enc[4];

        if (n + len >= sizeof(out[0])) {
            break;
        }
        if (u != c && (size_t)Utf8_Put(enc, u) == len) {
            SDL_memcpy(o + n, enc, len);
        } else {
            SDL_memcpy(o + n, start, len);
        }
        n += len;
    }
    o[n] = 0;
    return o;
}

static const char* Text_Fit(int fontIndex, const char* text, float width) {
    static char out[512];
    int n = (int)strlen(text), cut;
    float dots;

    if (Text_Width(fontIndex, text, n) <= width) {
        return text;
    }
    dots = Text_Width(fontIndex, "...", -1);
    while (n > 0 && Text_Width(fontIndex, text, n) + dots > width) {
        n--;
        while (n > 0 && ((unsigned char)text[n] & 0xc0) == 0x80) {
            n--;
        }
    }
    cut = n;
    while (cut > n / 2 && text[cut] != ' ') {
        cut--;
    }
    if (cut > n / 2) {
        n = cut;
    }
    while (n > 0 && (text[n - 1] == ' ' || text[n - 1] == ',' || text[n - 1] == '(' || text[n - 1] == ':' || text[n - 1] == '.')) {
        n--;
    }
    if (n > (int)sizeof(out) - 4) {
        n = (int)sizeof(out) - 4;
    }
    memcpy(out, text, (size_t)n);
    memcpy(out + n, "...", 4);
    return out;
}

static void Draw_Glyph(int fontIndex, Uint32 cp, float* px, float py, Uint32 rgba, SDL_Texture** last, bool track);

static void Draw_Text(int fontIndex, float x, float y, Uint32 rgba, const char* text, int bytes) {
    const char* end = bytes < 0 ? text + strlen(text) : text + bytes;
    font_t* font = &fonts[fontIndex];
    SDL_Texture* last = NULL;
    float px, py;

    x += slideX;
    rgba = Col_Fade(rgba);
    px = SDL_roundf(x * uiScale);
    py = SDL_roundf(y * uiScale + font->ascent);

    if (!fontsReady) {
        char line[256];

        SDL_snprintf(line, sizeof(line), "%.*s", (int)(end - text), text);
        Draw_Color(rgba);
        SDL_RenderDebugText(renderer, x * uiScale, y * uiScale, line);
        return;
    }
    while (text < end && *text) {
        Uint32 cp = (Uint32)Text_Next(&text);

        Draw_Glyph(fontIndex, cp, &px, py, rgba, &last, true);
    }
}

static void Draw_Glyph(int fontIndex, Uint32 cp, float* px, float py, Uint32 rgba, SDL_Texture** last, bool track) {
    SDL_Texture* tex;
    const glyph_t* g = Glyph_Get(fontIndex, cp, &tex);
    const cell_t* cell;
    float base = SDL_floorf(*px);
    int shift = (int)((*px - base) * FONT_SHIFTS + 0.5f);

    if (shift >= FONT_SHIFTS) {
        shift = 0;
        base += 1.0f;
    }
    cell = &g->cells[shift];
    if (cell->w > 0 && cell->h > 0) {
        SDL_FRect src, dst;

        if (g->color) {
            *last = NULL;
            SDL_SetTextureColorMod(tex, 255, 255, 255);
            SDL_SetTextureAlphaMod(tex, (Uint8)rgba);
        } else if (tex != *last) {
            *last = tex;
            SDL_SetTextureColorMod(tex, (Uint8)(rgba >> 24), (Uint8)(rgba >> 16), (Uint8)(rgba >> 8));
            SDL_SetTextureAlphaMod(tex, (Uint8)rgba);
        }
        src.x = (float)cell->ax;
        src.y = (float)cell->ay;
        src.w = (float)cell->w;
        src.h = (float)cell->h;
        dst.x = base + cell->ox;
        dst.y = py + cell->oy;
        dst.w = src.w;
        dst.h = src.h;
        SDL_RenderTexture(renderer, tex, &src, &dst);
    }
    if (track || g->advance > 0.0f) {
        *px += g->advance + textTrack * uiScale;
    }
}

static int Hex_Digit(char c) {
    return c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10;
}

static Uint32 Name_Readable(int r, int g, int b) {
    float lum = (0.299f * (float)r + 0.587f * (float)g + 0.114f * (float)b) / 255.0f;

    if (lum > 0.85f) {
        return 0;
    }
    if (lum > 0.55f) {
        float f = 0.55f / lum;

        r = (int)SDL_roundf((float)r * f);
        g = (int)SDL_roundf((float)g * f);
        b = (int)SDL_roundf((float)b * f);
    }
    return ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | 0xFFu;
}

static int Name_Code(const char* p, Uint32* col) {
    static const Uint32 digits[10] = {0x5a6d7d, 0xcc0000, 0x008800, 0x998800, 0x2855cc, 0x2b8f83, 0x8a2be2, 0, 0x5a6d7d, 0x606060};
    static const Uint32 letters[26] = {0xcc0000, 0xcc3600, 0xcc6600, 0xcc9500, 0x998800, 0x7a9500, 0x559500, 0x2e9500, 0x008800,
                                       0x00954a, 0x00956b, 0x00958c, 0x2b8f83, 0x0077b8, 0x0066cc, 0x2855cc, 0x2233cc, 0x4422cc,
                                       0x6622cc, 0x8a2be2, 0x8a2be2, 0xb02299, 0xb02277, 0xcc2255, 0,        0x606060};
    unsigned char c;
    Uint32 rgb;

    if (p[0] != '^' || !p[1]) {
        return 0;
    }
    c = (unsigned char)p[1];
    if (c == 'x' || c == 'X') {
        if (c == 'x' && SDL_isxdigit((unsigned char)p[2]) && SDL_isxdigit((unsigned char)p[3]) && SDL_isxdigit((unsigned char)p[4])) {
            *col = Name_Readable(Hex_Digit(p[2]) * 17, Hex_Digit(p[3]) * 17, Hex_Digit(p[4]) * 17);
            return 5;
        }
        return 0;
    }
    if (SDL_isdigit(c)) {
        rgb = digits[c - '0'];
    } else if (SDL_isalpha(c)) {
        rgb = letters[SDL_tolower(c) - 'a'];
    } else {
        return 0;
    }
    *col = rgb ? (rgb << 8) | 0xFFu : 0;
    return 2;
}

static void Name_Plain(const char* raw, char* out, size_t size) {
    size_t n = 0;
    Uint32 col;
    int skip;

    while (*raw == ' ') {
        raw++;
    }
    while (*raw) {
        const char* start = raw;
        size_t len;

        if ((skip = Name_Code(raw, &col)) > 0) {
            raw += skip;
            continue;
        }
        Text_Code(&raw);
        len = (size_t)(raw - start);
        if (n + len >= size) {
            break;
        }
        SDL_memcpy(out + n, start, len);
        n += len;
    }
    while (n > 0 && out[n - 1] == ' ') {
        n--;
    }
    out[n] = '\0';
}

static int Arabic_Forms(Uint32 c, Uint32* base) {
    static const Uint16 starts[42] = {0xfe80, 0xfe81, 0xfe83, 0xfe85, 0xfe87, 0xfe89, 0xfe8d, 0xfe8f, 0xfe93, 0xfe95, 0xfe99,
                                      0xfe9d, 0xfea1, 0xfea5, 0xfea9, 0xfeab, 0xfead, 0xfeaf, 0xfeb1, 0xfeb5, 0xfeb9, 0xfebd,
                                      0xfec1, 0xfec5, 0xfec9, 0xfecd, 0,      0,      0,      0,      0,      0,      0xfed1,
                                      0xfed5, 0xfed9, 0xfedd, 0xfee1, 0xfee5, 0xfee9, 0xfeed, 0xfeef, 0xfef1};
    static const Uint8 forms[42] = {1, 2, 2, 2, 2, 4, 2, 4, 2, 4, 4, 4, 4, 4, 2, 2, 2, 2, 4, 4, 4,
                                    4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 4};

    if (c < 0x621 || c > 0x64a) {
        return 0;
    }
    *base = starts[c - 0x621];
    return forms[c - 0x621];
}

static bool Rune_Mark(Uint32 c) {
    return (c >= 0x300 && c <= 0x36f) || (c >= 0x483 && c <= 0x489) || (c >= 0x591 && c <= 0x5bd) || (c >= 0x610 && c <= 0x61a) ||
           (c >= 0x64b && c <= 0x65f) || c == 0x670 || (c >= 0x6d6 && c <= 0x6ed) || (c >= 0x20d0 && c <= 0x20ff);
}

static bool Rune_Rtl(Uint32 c) {
    return (c >= 0x590 && c <= 0x8ff && !Rune_Mark(c)) || (c >= 0xfb1d && c <= 0xfdff) || (c >= 0xfe70 && c <= 0xfefc);
}

static bool Rune_Neutral(Uint32 c) {
    return c == ' ' || (c < 0x80 && !SDL_isalnum((int)c)) || (c >= 0xa0 && c <= 0xbf) || (c >= 0x2000 && c <= 0x2bff);
}

static int Arabic_Shape(nameRune_t* r, int n) {
    nameRune_t src[NAME_RUNES];
    Uint32 base = 0, other;
    int i, j, k, m = 0, forms;
    bool joinPrev, joinNext;

    SDL_memcpy(src, r, (size_t)n * sizeof(*r));
    for (i = 0; i < n; i++) {
        if (!src[i].cp) {
            continue;
        }
        r[m] = src[i];
        forms = src[i].cp == 0x640 ? 4 : Arabic_Forms(src[i].cp, &base);
        if (forms) {
            for (j = i - 1; j >= 0 && Rune_Mark(src[j].cp); j--) {
            }
            for (k = i + 1; k < n && Rune_Mark(src[k].cp); k++) {
            }
            joinPrev = forms >= 2 && j >= 0 && (src[j].cp == 0x640 || Arabic_Forms(src[j].cp, &other) == 4);
            joinNext = forms == 4 && k < n && (src[k].cp == 0x640 || Arabic_Forms(src[k].cp, &other) >= 2);
            if (src[i].cp == 0x644 && k < n &&
                (src[k].cp == 0x622 || src[k].cp == 0x623 || src[k].cp == 0x625 || src[k].cp == 0x627)) {
                r[m].cp = (src[k].cp == 0x622 ? 0xfef5 : src[k].cp == 0x623 ? 0xfef7 : src[k].cp == 0x625 ? 0xfef9 : 0xfefb) +
                          (joinPrev ? 1 : 0);
                src[k].cp = 0;
            } else if (src[i].cp != 0x640 && base) {
                r[m].cp = base + (forms == 4 && joinPrev && joinNext ? 3 : forms == 4 && joinNext ? 2 : joinPrev ? 1 : 0);
            }
        }
        m++;
    }
    return m;
}

static void Runes_Reorder(nameRune_t* r, int n) {
    nameRune_t tmp[NAME_RUNES];
    int i, j, end, last, k, o;

    for (i = 0; i < n;) {
        if (!Rune_Rtl(r[i].cp)) {
            i++;
            continue;
        }
        end = i + 1;
        for (j = i + 1; j < n; j++) {
            if (Rune_Rtl(r[j].cp) || (Rune_Mark(r[j].cp) && end == j)) {
                end = j + 1;
            } else if (!Rune_Neutral(r[j].cp) && !Rune_Mark(r[j].cp)) {
                break;
            }
        }
        SDL_memcpy(tmp, r + i, (size_t)(end - i) * sizeof(*r));
        o = i;
        for (last = end - i - 1; last >= 0; last = k - 1) {
            for (k = last; k > 0 && Rune_Mark(tmp[k].cp); k--) {
            }
            SDL_memcpy(r + o, tmp + k, (size_t)(last - k + 1) * sizeof(*r));
            o += last - k + 1;
        }
        i = end;
    }
}


static int Name_Runes(const char* raw, nameRune_t* out, int max) {
    Uint32 col = 0, code;
    int n = 0, skip, i;

    while (*raw && n < max) {
        Uint32 c;

        if ((skip = Name_Code(raw, &code)) > 0) {
            col = code;
            raw += skip;
            continue;
        }
        c = (Uint32)Text_Next(&raw);
        c = Upper_Cp(c);
        if (Glyph_Ignored(c)) {
            continue;
        }
        out[n].cp = c;
        out[n].col = col;
        n++;
    }
    while (n > 0 && out[n - 1].cp == ' ') {
        n--;
    }
    for (i = 0; i < n && out[i].cp == ' '; i++) {
    }
    if (i > 0) {
        SDL_memmove(out, out + i, (size_t)(n - i) * sizeof(*out));
        n -= i;
    }
    n = Arabic_Shape(out, n);
    Runes_Reorder(out, n);
    return n;
}

static float Runes_Width(int fontIndex, const nameRune_t* r, int n) {
    float w = 0.0f;
    int i;

    for (i = 0; i < n; i++) {
        SDL_Texture* tex;
        const glyph_t* g = Glyph_Get(fontIndex, r[i].cp, &tex);

        if (g->advance > 0.0f) {
            w += g->advance + textTrack * uiScale;
        }
    }
    return w / uiScale;
}

static void Draw_Runes(int fontIndex, float x, float y, Uint32 rgba, const nameRune_t* r, int n) {
    SDL_Texture* last = NULL;
    Uint32 lastCol = 0;
    float px, py;
    int i;

    if (!fontsReady) {
        return;
    }
    rgba = Col_Fade(rgba);
    px = SDL_roundf((x + slideX) * uiScale);
    py = SDL_roundf(y * uiScale + fonts[fontIndex].ascent);
    for (i = 0; i < n; i++) {
        Uint32 col = r[i].col ? ((r[i].col & 0xFFFFFF00u) | (rgba & 0xFFu)) : rgba;

        if (col != lastCol) {
            lastCol = col;
            last = NULL;
        }
        Draw_Glyph(fontIndex, r[i].cp, &px, py, col, &last, false);
    }
}

static float Text_Mid(int fontIndex, float mid) {
    if (!fontsReady) {
        return mid - (float)SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 0.5f;
    }
    return mid + (fonts[fontIndex].cap * 0.5f - fonts[fontIndex].ascent) / uiScale;
}

static float Font_Height(int fontIndex) {
    return fontsReady ? fonts[fontIndex].height / uiScale : (float)SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE + 4.0f;
}

static float Notes_Fade(float y, float lineH) {
    float mid = y + lineH * 0.5f, a = 1.0f, d;

    if (notesScroll > 0.5f) {
        d = (mid - notesTop) / 30.0f;
        a = d < a ? d : a;
    }
    if (notesHeight - notesView - notesScroll > 0.5f) {
        d = (notesLow - mid) / 30.0f;
        a = d < a ? d : a;
    }
    return a < 0.0f ? 0.0f : a;
}

static float Draw_Wrapped(int fontIndex, float x, float y, float width, float lineH, Uint32 rgba, const char* text) {
    while (*text) {
        const char* p = text;
        const char* fit = NULL;
        float fade;

        while (*p) {
            const char* word = p;

            while (*word && *word != ' ') {
                word++;
            }
            if (Text_Width(fontIndex, text, (int)(word - text)) > width && fit) {
                break;
            }
            fit = word;
            p = word;
            while (*p == ' ') {
                p++;
            }
        }
        if (!fit) {
            fit = text + strlen(text);
        }
        fade = wrapPlain ? 1.0f : notesDry ? 0.0f : Notes_Fade(y, lineH);
        if (fade > 0.0f && (wrapPlain || (y + lineH > notesTop && y < notesLow))) {
            Draw_Text(fontIndex, x, y + (lineH - Font_Height(fontIndex)) * 0.5f, Col_Alpha(rgba, fade), text, (int)(fit - text));
        }
        y += lineH;
        text = fit;
        while (*text == ' ') {
            text++;
        }
    }
    return y;
}

static float Draw_Para(int fontIndex, float x, float y, float width, float lineH, Uint32 rgba, const char* text) {
    char part[1024];
    const char *end, *start = text;

    wrapPlain = true;
    for (;;) {
        size_t n, i, o = 0;

        end = strchr(text, '\n');
        n = end ? (size_t)(end - text) : strlen(text);
        for (i = 0; i < n && o < sizeof(part) - 1; i++) {
            if (text[i] != '\r') {
                part[o++] = text[i];
            }
        }
        part[o] = '\0';
        y = o || (!end && text == start) ? Draw_Wrapped(fontIndex, x, y, width, lineH, rgba, part) : y + lineH * 0.5f;
        if (!end) {
            break;
        }
        text = end + 1;
    }
    wrapPlain = false;
    return y;
}

static int Notes_Line(const char** cursor, char* line, size_t size, const char** text) {
    const char* p = *cursor;
    const char* end = strchr(p, '\n');
    size_t n = end ? (size_t)(end - p) : strlen(p);
    size_t i = 0, o = 0;
    int kind = LINE_TEXT;

    *cursor = p + n + (end ? 1 : 0);
    *text = line;
    while (i < n && (p[i] == ' ' || p[i] == '\t')) {
        i++;
    }
    if (i < n && p[i] == '#') {
        kind = LINE_HEAD;
        while (i < n && (p[i] == '#' || p[i] == ' ')) {
            i++;
        }
    } else if (i + 1 < n && (p[i] == '-' || p[i] == '*') && p[i + 1] == ' ') {
        kind = LINE_BULLET;
        i += 2;
    }
    for (; i < n && o < size - 1; i++) {
        if (p[i] == '[') {
            const char* close = memchr(p + i, ']', n - i);
            const char* stop = close && close + 1 < p + n && close[1] == '(' ? memchr(close, ')', (size_t)(p + n - close)) : NULL;

            if (stop) {
                for (i++; p + i < close && o < size - 1; i++) {
                    if (p[i] != '*' && p[i] != '`') {
                        line[o++] = p[i];
                    }
                }
                i = (size_t)(stop - p);
                continue;
            }
        }
        if (p[i] != '\r' && p[i] != '*' && p[i] != '`') {
            line[o++] = p[i];
        }
    }
    while (o > 0 && line[o - 1] == ' ') {
        o--;
    }
    line[o] = '\0';
    if (!o || (o >= 3 && strspn(line, "-") == o)) {
        return LINE_BLANK;
    }
    return kind;
}

static float Draw_Notes(float x, float y, float width, float alpha) {
    char line[512];
    const char* p = latestNotes;
    const char* text;
    bool first = true;

    while (*p) {
        switch (Notes_Line(&p, line, sizeof(line), &text)) {
            case LINE_BLANK:
                continue;
            case LINE_HEAD:
                if (!SDL_strcasecmp(text, "Changelog")) {
                    continue;
                }
                textTrack = TRACK_CAPS;
                y = Draw_Wrapped(FONT_BRAND, x, y + (first ? 0.0f : 22.0f), width, 28.0f, Col_Alpha(COL_WHITE, alpha), Text_Upper(text)) + 6.0f;
                textTrack = 0.0f;
                break;
            case LINE_BULLET:
                if (!notesDry) {
                    Draw_Round(x + 1.0f, y + NOTES_LINE * 0.5f - 3.0f, 6.0f, 6.0f, 3.0f, Col_Alpha(COL_W40, alpha * Notes_Fade(y, NOTES_LINE)));
                }
                y = Draw_Wrapped(FONT_BODY, x + 20.0f, y, width - 20.0f, NOTES_LINE, Col_Alpha(COL_W85, alpha), text) + 8.0f;
                break;
            default:
                y = Draw_Wrapped(FONT_BODY, x, y, width, NOTES_LINE, Col_Alpha(COL_W85, alpha), text) + 8.0f;
                break;
        }
        first = false;
    }
    return y;
}

static bool Version_Parts(const char* s, long parts[4]) {
    int i;

    parts[0] = parts[1] = parts[2] = 0;
    parts[3] = LONG_MAX;
    for (i = 0; i < 3 && *s >= '0' && *s <= '9'; i++) {
        parts[i] = strtol(s, (char**)&s, 10);
        if (*s == '.' && i < 2) {
            s++;
        }
    }
    if (i && !strncmp(s, "-preview-", 9) && s[9] >= '0' && s[9] <= '9') {
        parts[3] = strtol(s + 9, (char**)&s, 10);
    }
    return i && !*s;
}

static int Version_Compare(const char* a, const char* b) {
    long x[4], y[4];
    int i;

    Version_Parts(a, x);
    Version_Parts(b, y);
    for (i = 0; i < 4; i++) {
        if (x[i] != y[i]) {
            return x[i] < y[i] ? -1 : 1;
        }
    }
    return 0;
}

static const char* Json_Value(const char* text, const char* key) {
    const char* p = strstr(text, key);

    if (!p) {
        return NULL;
    }
    p += strlen(key);
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ':') {
        p++;
    }
    return p;
}

static void Json_Text(const char* p, char* out, int size) {
    int n = 0;

    out[0] = '\0';
    if (!p || *p != '"') {
        return;
    }
    for (p++; *p && *p != '"' && n < size - 4; p++) {
        if (*p != '\\') {
            if ((unsigned char)*p >= 0x20) {
                out[n++] = *p;
            }
            continue;
        }
        if (!*++p) {
            break;
        }
        if (*p == 'n') {
            out[n++] = '\n';
        } else if (*p == 't') {
            out[n++] = ' ';
        } else if (*p == 'u') {
            unsigned cp = 0;
            int i;

            for (i = 1; i <= 4 && p[i]; i++) {
                cp = (cp << 4) | (unsigned)(p[i] <= '9' ? p[i] - '0' : (p[i] | 0x20) - 'a' + 10);
            }
            if (i <= 4) {
                break;
            }
            p += 4;
            if (cp >= 0xd800 && cp <= 0xdbff && p[1] == '\\' && p[2] == 'u' && SDL_isxdigit((unsigned char)p[3]) &&
                SDL_isxdigit((unsigned char)p[4]) && SDL_isxdigit((unsigned char)p[5]) && SDL_isxdigit((unsigned char)p[6])) {
                unsigned low = 0;

                for (i = 3; i <= 6; i++) {
                    low = (low << 4) | (unsigned)Hex_Digit(p[i]);
                }
                if (low >= 0xdc00 && low <= 0xdfff) {
                    cp = 0x10000 + ((cp - 0xd800) << 10) + (low - 0xdc00);
                    p += 6;
                }
            }
            if (cp >= 0x20 && (cp < 0xd800 || cp > 0xdfff)) {
                n += Utf8_Put(out + n, cp);
            }
        } else if (*p != 'r' && *p != 'b' && *p != 'f') {
            out[n++] = *p;
        }
    }
    out[n] = '\0';
}

static void Job_Add(size_t bytes) {
    jobBytes += (Sint64)bytes;
    SDL_SetAtomicInt(&jobKb, (int)(jobBytes >> 10));
}

typedef bool (*sink_t)(void* ctx, const void* data, size_t n);

typedef struct {
    Uint32 h[8];
    Uint64 bits;
    unsigned char block[64];
    size_t used;
} sha_t;

static const Uint32 shaK[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be,
    0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa,
    0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85,
    0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
    0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

#define SHA_ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static void Sha_Block(sha_t* sha, const unsigned char* p) {
    Uint32 w[64], a, b, c, d, e, f, g, h, t1, t2;
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = (Uint32)p[i * 4] << 24 | (Uint32)p[i * 4 + 1] << 16 | (Uint32)p[i * 4 + 2] << 8 | (Uint32)p[i * 4 + 3];
    }
    for (i = 16; i < 64; i++) {
        w[i] = (SHA_ROR(w[i - 2], 17) ^ SHA_ROR(w[i - 2], 19) ^ (w[i - 2] >> 10)) + w[i - 7] +
               (SHA_ROR(w[i - 15], 7) ^ SHA_ROR(w[i - 15], 18) ^ (w[i - 15] >> 3)) + w[i - 16];
    }
    a = sha->h[0];
    b = sha->h[1];
    c = sha->h[2];
    d = sha->h[3];
    e = sha->h[4];
    f = sha->h[5];
    g = sha->h[6];
    h = sha->h[7];
    for (i = 0; i < 64; i++) {
        t1 = h + (SHA_ROR(e, 6) ^ SHA_ROR(e, 11) ^ SHA_ROR(e, 25)) + ((e & f) ^ (~e & g)) + shaK[i] + w[i];
        t2 = (SHA_ROR(a, 2) ^ SHA_ROR(a, 13) ^ SHA_ROR(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    sha->h[0] += a;
    sha->h[1] += b;
    sha->h[2] += c;
    sha->h[3] += d;
    sha->h[4] += e;
    sha->h[5] += f;
    sha->h[6] += g;
    sha->h[7] += h;
}

static void Sha_Init(sha_t* sha) {
    static const Uint32 start[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    memcpy(sha->h, start, sizeof(start));
    sha->bits = 0;
    sha->used = 0;
}

static void Sha_Update(sha_t* sha, const void* data, size_t n) {
    const unsigned char* p = data;

    sha->bits += (Uint64)n * 8;
    while (n > 0) {
        size_t take = 64 - sha->used;

        if (take > n) {
            take = n;
        }
        memcpy(sha->block + sha->used, p, take);
        sha->used += take;
        p += take;
        n -= take;
        if (sha->used == 64) {
            Sha_Block(sha, sha->block);
            sha->used = 0;
        }
    }
}

static void Sha_Final(sha_t* sha, unsigned char* digest) {
    int i;

    sha->block[sha->used++] = 0x80;
    if (sha->used > 56) {
        memset(sha->block + sha->used, 0, 64 - sha->used);
        Sha_Block(sha, sha->block);
        sha->used = 0;
    }
    memset(sha->block + sha->used, 0, 56 - sha->used);
    for (i = 0; i < 8; i++) {
        sha->block[56 + i] = (unsigned char)(sha->bits >> (56 - i * 8));
    }
    Sha_Block(sha, sha->block);
    for (i = 0; i < 32; i++) {
        digest[i] = (unsigned char)(sha->h[i / 4] >> (24 - (i % 4) * 8));
    }
}

#ifdef _WIN32
static HINTERNET Http_Open(const char* address, const char* body, HINTERNET* session, HINTERNET* connect, int wait) {
    URL_COMPONENTS uc;
    wchar_t url[1024], host[256], path[1024];
    HINTERNET request = NULL;
    DWORD status = 0, statusLen = sizeof(status);

    *session = NULL;
    *connect = NULL;
    if (!MultiByteToWideChar(CP_UTF8, 0, address, -1, url, 1024)) {
        return NULL;
    }
    memset(&uc, 0, sizeof(uc));
    uc.dwStructSize = sizeof(uc);
    uc.lpszHostName = host;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = 1024;
    if (!WinHttpCrackUrl(url, 0, 0, &uc) || uc.nScheme != INTERNET_SCHEME_HTTPS) {
        return NULL;
    }
    *session = WinHttpOpen(L"ioquakelive", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (*session) {
        WinHttpSetTimeouts(*session, 5000, 5000, 5000, wait);
        *connect = WinHttpConnect(*session, host, uc.nPort, 0);
    }
    if (*connect) {
        request = WinHttpOpenRequest(*connect, body ? L"POST" : L"GET", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    }
    if (request &&
        (body ? WinHttpSendRequest(request, L"Content-Type: application/json\r\n", (DWORD)-1L, (LPVOID)body, (DWORD)SDL_strlen(body),
                                   (DWORD)SDL_strlen(body), 0)
              : WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) &&
        WinHttpReceiveResponse(request, NULL) &&
        WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status,
                            &statusLen, WINHTTP_NO_HEADER_INDEX) &&
        status == 200) {
        return request;
    }
    if (request) {
        WinHttpCloseHandle(request);
    }
    return NULL;
}

static void Http_Close(HINTERNET session, HINTERNET connect, HINTERNET request) {
    if (request) {
        WinHttpCloseHandle(request);
    }
    if (connect) {
        WinHttpCloseHandle(connect);
    }
    if (session) {
        WinHttpCloseHandle(session);
    }
}

static bool Http_Send(const char* address, const char* body, int wait, sink_t sink, void* ctx) {
    HINTERNET session, connect;
    HINTERNET request = Http_Open(address, body, &session, &connect, wait);
    unsigned char* block = request ? malloc(ZIP_BLOCK) : NULL;
    DWORD got = 0;
    bool ok = block != NULL;

    while (ok) {
        if (!WinHttpReadData(request, block, ZIP_BLOCK, &got)) {
            ok = false;
        } else if (!got) {
            break;
        } else {
            ok = sink(ctx, block, got);
        }
    }
    free(block);
    Http_Close(session, connect, request);
    return ok;
}

static bool Link_Save(const char* exe, const char* dir, const char* path, const wchar_t* args, int show) {
    wchar_t target[1024], work[1024], link[1024];
    IShellLinkW* shell = NULL;
    IPersistFile* file = NULL;
    HRESULT init;
    wchar_t* c;
    bool ok = false;

    if (!MultiByteToWideChar(CP_UTF8, 0, exe, -1, target, 1024) || !MultiByteToWideChar(CP_UTF8, 0, dir, -1, work, 1024) ||
        !MultiByteToWideChar(CP_UTF8, 0, path, -1, link, 1024)) {
        return false;
    }
    for (c = target; *c; c++) {
        if (*c == L'/') {
            *c = L'\\';
        }
    }
    for (c = work; *c; c++) {
        if (*c == L'/') {
            *c = L'\\';
        }
    }
    init = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, (void**)&shell))) {
        IShellLinkW_SetPath(shell, target);
        IShellLinkW_SetWorkingDirectory(shell, work);
        if (args) {
            IShellLinkW_SetArguments(shell, args);
        }
        if (show) {
            IShellLinkW_SetShowCmd(shell, show);
        }
        if (SUCCEEDED(IShellLinkW_QueryInterface(shell, &IID_IPersistFile, (void**)&file))) {
            ok = SUCCEEDED(IPersistFile_Save(file, link, TRUE));
            IPersistFile_Release(file);
        }
        IShellLinkW_Release(shell);
    }
    if (SUCCEEDED(init)) {
        CoUninitialize();
    }
    return ok;
}

static bool Shortcut_Make(const char* exe, const char* dir, const char* desktop) {
    char path[1200];

    if (!desktop) {
        return false;
    }
    SDL_snprintf(path, sizeof(path), "%s" LINK_NAME, desktop);
    return Link_Save(exe, dir, path, NULL, 0);
}

static bool Autostart_Path(char* out, size_t size) {
    wchar_t wide[MAX_PATH];
    char dir[1024];

    if (FAILED(SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, wide)) ||
        !WideCharToMultiByte(CP_UTF8, 0, wide, -1, dir, sizeof(dir), NULL, NULL)) {
        return false;
    }
    SDL_snprintf(out, size, "%s\\" LINK_NAME, dir);
    return true;
}

static bool Autostart_Make(const char* path) {
    return Link_Save(selfPath, basePath, path, L"--startup", SW_SHOWMINNOACTIVE);
}
#else
#define CURL_WRITEDATA 10001
#define CURL_URL 10002
#define CURL_USERAGENT 10018
#define CURL_POSTFIELDS 10015
#define CURL_WRITEFUNCTION 20011
#define CURL_LOW_SPEED_LIMIT 19
#define CURL_LOW_SPEED_TIME 20
#define CURL_FAILONERROR 45
#define CURL_FOLLOWLOCATION 52
#define CURL_CONNECTTIMEOUT 78
#define CURL_NOSIGNAL 99
#define CURL_PROTOCOLS 181
#define CURL_REDIR_PROTOCOLS 182
#define CURL_PROTO_HTTPS 2L
#define CURL_RESPONSE_CODE 0x200002

typedef struct {
    sink_t sink;
    void* ctx;
} relay_t;

static void* (*curlInit)(void);
static int (*curlSetopt)(void*, int, ...);
static int (*curlPerform)(void*);
static int (*curlGetinfo)(void*, int, ...);
static void (*curlCleanup)(void*);

static void Curl_Load(void) {
    static const char* const names[] = {"libcurl.so.4", "libcurl.so", "libcurl-gnutls.so.4", "libcurl.4.dylib", "/usr/lib/libcurl.4.dylib",
                                        "libcurl.dylib"};
    SDL_SharedObject* lib = NULL;
    int (*globalInit)(long);
    int i;

    for (i = 0; i < (int)SDL_arraysize(names) && !lib; i++) {
        lib = SDL_LoadObject(names[i]);
    }
    if (!lib) {
        return;
    }
    globalInit = (int (*)(long))SDL_LoadFunction(lib, "curl_global_init");
    curlInit = (void* (*)(void))SDL_LoadFunction(lib, "curl_easy_init");
    curlSetopt = (int (*)(void*, int, ...))SDL_LoadFunction(lib, "curl_easy_setopt");
    curlPerform = (int (*)(void*))SDL_LoadFunction(lib, "curl_easy_perform");
    curlGetinfo = (int (*)(void*, int, ...))SDL_LoadFunction(lib, "curl_easy_getinfo");
    curlCleanup = (void (*)(void*))SDL_LoadFunction(lib, "curl_easy_cleanup");
    if (!globalInit || globalInit(3) != 0 || !curlInit || !curlSetopt || !curlPerform || !curlGetinfo || !curlCleanup) {
        curlInit = NULL;
    }
}

static size_t Curl_Write(char* data, size_t size, size_t count, void* user) {
    relay_t* relay = user;

    return relay->sink(relay->ctx, data, size * count) ? size * count : 0;
}

static bool Http_Send(const char* address, const char* body, int wait, sink_t sink, void* ctx) {
    relay_t relay;
    void* curl;
    long code = 0;
    bool ok;

    if (SDL_strncmp(address, "https://", 8) || !curlInit || (curl = curlInit()) == NULL) {
        return false;
    }
    relay.sink = sink;
    relay.ctx = ctx;
    curlSetopt(curl, CURL_URL, address);
    curlSetopt(curl, CURL_WRITEFUNCTION, Curl_Write);
    curlSetopt(curl, CURL_WRITEDATA, &relay);
    curlSetopt(curl, CURL_PROTOCOLS, CURL_PROTO_HTTPS);
    curlSetopt(curl, CURL_REDIR_PROTOCOLS, CURL_PROTO_HTTPS);
    curlSetopt(curl, CURL_FOLLOWLOCATION, 1L);
    curlSetopt(curl, CURL_FAILONERROR, 1L);
    curlSetopt(curl, CURL_NOSIGNAL, 1L);
    curlSetopt(curl, CURL_CONNECTTIMEOUT, 5L);
    curlSetopt(curl, CURL_LOW_SPEED_LIMIT, 1L);
    curlSetopt(curl, CURL_LOW_SPEED_TIME, (long)(wait / 1000));
    curlSetopt(curl, CURL_USERAGENT, "ioquakelive");
    if (body) {
        curlSetopt(curl, CURL_POSTFIELDS, body);
    }
    ok = curlPerform(curl) == 0 && curlGetinfo(curl, CURL_RESPONSE_CODE, &code) == 0 && code == 200;
    curlCleanup(curl);
    return ok;
}

#ifdef __APPLE__
static bool Autostart_Path(char* out, size_t size) {
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);

    if (!home) {
        return false;
    }
    SDL_snprintf(out, size, "%sLibrary/LaunchAgents/com.ioquakelive.launcher.plist", home);
    return true;
}

static bool Autostart_Make(const char* path) {
    char text[2600], dir[1200];
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
    int n;

    if (!home || strpbrk(selfPath, "<>&")) {
        return false;
    }
    SDL_snprintf(dir, sizeof(dir), "%sLibrary/LaunchAgents", home);
    SDL_CreateDirectory(dir);
    n = SDL_snprintf(text, sizeof(text),
                     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                     "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                     "<plist version=\"1.0\">\n<dict>\n<key>Label</key>\n<string>com.ioquakelive.launcher</string>\n"
                     "<key>ProgramArguments</key>\n<array>\n<string>%s</string>\n<string>--startup</string>\n</array>\n"
                     "<key>RunAtLoad</key>\n<true/>\n</dict>\n</plist>\n",
                     selfPath);
    return n > 0 && n < (int)sizeof(text) && SDL_SaveFile(path, text, (size_t)n);
}

static bool Shortcut_Make(const char* exe, const char* dir, const char* desktop) {
    char app[1200], link[1200];
    struct stat st;

    (void)exe;
    if (!desktop) {
        return false;
    }
    SDL_snprintf(app, sizeof(app), "%s/" SELF_APP, dir);
    SDL_snprintf(link, sizeof(link), "%s" LINK_NAME, desktop);
    if (lstat(link, &st) == 0) {
        if (!S_ISLNK(st.st_mode)) {
            return false;
        }
        unlink(link);
    }
    return symlink(app, link) == 0;
}
#else
static bool Desktop_Plain(const char* text) {
    return !strpbrk(text, "\n\r\"`$\\%");
}

static bool Desktop_Write(const char* path, const char* exe, const char* args, const char* dir, const char* icon) {
    char text[4200];
    int n = SDL_snprintf(text, sizeof(text),
                         "[Desktop Entry]\nType=Application\nName=ioquakelive\nExec=\"%s\"%s\nPath=%s\nIcon=%s\nTerminal=false\n"
                         "Categories=Game;\nStartupWMClass=ioquakelive\n",
                         exe, args, dir, icon);

    return n > 0 && n < (int)sizeof(text) && SDL_SaveFile(path, text, (size_t)n) && chmod(path, 0755) == 0;
}

static bool Shortcut_Make(const char* exe, const char* dir, const char* desktop) {
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
    char icon[1200], path[1200];
    bool ok;

    if (!home || !Desktop_Plain(exe) || !Desktop_Plain(dir) || !Desktop_Plain(home)) {
        return false;
    }
    SDL_snprintf(path, sizeof(path), "%s.local/share/icons", home);
    SDL_CreateDirectory(path);
    SDL_snprintf(icon, sizeof(icon), "%s/ioquakelive.png", path);
    if (!SDL_SaveFile(icon, launcherLogo, launcherLogoSize)) {
        return false;
    }
    SDL_snprintf(path, sizeof(path), "%s.local/share/applications", home);
    SDL_CreateDirectory(path);
    SDL_strlcat(path, "/" LINK_NAME, sizeof(path));
    ok = Desktop_Write(path, exe, "", dir, icon);
    if (desktop) {
        SDL_snprintf(path, sizeof(path), "%s" LINK_NAME, desktop);
        ok = Desktop_Write(path, exe, "", dir, icon) && ok;
    }
    return ok;
}

static bool Autostart_Path(char* out, size_t size) {
    const char* config = SDL_getenv("XDG_CONFIG_HOME");
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);

    if (config && config[0] == '/') {
        SDL_snprintf(out, size, "%s/autostart/" LINK_NAME, config);
    } else if (home) {
        SDL_snprintf(out, size, "%s.config/autostart/" LINK_NAME, home);
    } else {
        return false;
    }
    return true;
}

static bool Autostart_Make(const char* path) {
    char dir[1200], icon[1200];
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
    char* cut;

    if (!home || !Desktop_Plain(selfPath) || !Desktop_Plain(basePath)) {
        return false;
    }
    SDL_strlcpy(dir, path, sizeof(dir));
    cut = strrchr(dir, '/');
    if (cut) {
        *cut = '\0';
        SDL_CreateDirectory(dir);
    }
    SDL_snprintf(icon, sizeof(icon), "%s.local/share/icons/ioquakelive.png", home);
    return Desktop_Write(path, selfPath, " --startup", basePath, icon);
}
#endif
#endif

static bool Autostart_On(void) {
    char path[1200];
    SDL_PathInfo info;

    return Autostart_Path(path, sizeof(path)) && SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_FILE;
}

static bool Autostart_Set(bool on) {
    char path[1200];

    if (!Autostart_Path(path, sizeof(path))) {
        return false;
    }
    if (!on) {
        SDL_RemovePath(path);
        return true;
    }
    return Autostart_Make(path);
}

typedef struct {
    char* out;
    int size;
    int total;
} text_t;

static bool Text_Sink(void* ctx, const void* data, size_t n) {
    text_t* text = ctx;
    size_t room = (size_t)(text->size - 1 - text->total);

    if (n > room) {
        n = room;
    }
    memcpy(text->out + text->total, data, n);
    text->total += (int)n;
    return text->total < text->size - 1;
}

static bool Http_Fetch(const char* address, int wait, sink_t sink, void* ctx) {
    return Http_Send(address, NULL, wait, sink, ctx);
}

static bool Http_Post(const char* address, const char* body, char* out, int size) {
    text_t text;

    text.out = out;
    text.size = size;
    text.total = 0;
    Http_Send(address, body, 5000, Text_Sink, &text);
    out[text.total] = '\0';
    return text.total > 0;
}

static bool Http_Get(const char* address, char* out, int size) {
    return Http_Post(address, NULL, out, size);
}

typedef struct {
    SDL_IOStream* out;
    sha_t sha;
} save_t;

static bool Save_Sink(void* ctx, const void* data, size_t n) {
    save_t* save = ctx;

    Sha_Update(&save->sha, data, n);
    Job_Add(n);
    return SDL_WriteIO(save->out, data, n) == n && !SDL_GetAtomicInt(&jobStop);
}

static bool Http_File(const char* address, const char* path, char* hashHex) {
    save_t save;
    unsigned char digest[32];
    bool ok;
    int i;

    save.out = SDL_IOFromFile(path, "wb");
    if (!save.out) {
        return false;
    }
    Sha_Init(&save.sha);
    ok = Http_Fetch(address, 30000, Save_Sink, &save);
    if (!SDL_CloseIO(save.out)) {
        ok = false;
    }
    Sha_Final(&save.sha, digest);
    for (i = 0; ok && i < 32; i++) {
        SDL_snprintf(hashHex + i * 2, 3, "%02x", digest[i]);
    }
    return ok;
}

static bool Release_Tag(const char* json, char* tag, int size) {
    const char* p = Json_Value(json, "\"tag_name\"");
    long parts[4];
    int n;

    tag[0] = '\0';
    if (!p || *p++ != '"') {
        return false;
    }
    for (n = 0; n < size - 1 && p[n] && p[n] != '"'; n++) {
        if ((p[n] < '0' || p[n] > '9') && (p[n] < 'a' || p[n] > 'z') && p[n] != '.' && p[n] != '-') {
            return false;
        }
        tag[n] = p[n];
    }
    tag[n] = '\0';
    return n && p[n] == '"' && Version_Parts(tag[0] == 'v' ? tag + 1 : tag, parts);
}

static bool Package_Find(const char* json, pack_t* pack) {
    char tag[32], digest[80];
    const char* p = strstr(json, "\"" PACKAGE "\"");
    const char* v;

    memset(pack, 0, sizeof(*pack));
    if (!p || !Release_Tag(json, tag, sizeof(tag))) {
        return false;
    }
    SDL_strlcpy(pack->version, tag[0] == 'v' ? tag + 1 : tag, sizeof(pack->version));
    v = Json_Value(p, "\"size\"");
    pack->size = v ? (Sint64)SDL_strtoll(v, NULL, 10) : 0;
    Json_Text(Json_Value(p, "\"digest\""), digest, sizeof(digest));
    Json_Text(Json_Value(p, "\"browser_download_url\""), pack->url, sizeof(pack->url));
    if (strncmp(digest, "sha256:", 7) || strlen(digest + 7) != 64 || strncmp(pack->url, DOWNLOAD_ROOT, strlen(DOWNLOAD_ROOT)) || pack->size <= 0) {
        pack->url[0] = '\0';
        return false;
    }
    SDL_strlcpy(pack->hash, digest + 7, sizeof(pack->hash));
    return true;
}

static bool Release_Fetch(char* out, int size) {
#ifdef LAUNCHER_SHOT
    const char* local = SDL_getenv("LAUNCHER_JSON");

    if (local && local[0]) {
        char* text = SDL_LoadFile(local, NULL);

        if (!text) {
            return false;
        }
        SDL_strlcpy(out, text, (size_t)size);
        SDL_free(text);
        return true;
    }
#endif
    return Http_Get(betaChannel ? BETA_API : STABLE_API, out, size);
}

static int SDLCALL Check_Thread(void* unused) {
    (void)unused;
    if (!Release_Fetch(httpText, sizeof(httpText)) || !Release_Tag(httpText, latestTag, sizeof(latestTag))) {
        SDL_SetAtomicInt(&checkState, CHECK_FAILED);
        return 0;
    }
    SDL_strlcpy(latestVersion, latestTag[0] == 'v' ? latestTag + 1 : latestTag, sizeof(latestVersion));
    Json_Text(Json_Value(httpText, "\"body\""), latestNotes, sizeof(latestNotes));
    Package_Find(httpText, &packLatest);
    updateAvailable = Version_Compare(latestVersion, installedVersion) > 0;
    SDL_SetAtomicInt(&checkState, CHECK_DONE);
    return 0;
}

static bool File_Exists(const char* name) {
    char path[1024];
    SDL_PathInfo info;

    SDL_snprintf(path, sizeof(path), "%s%s", basePath, name);
    return SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_FILE;
}

static bool Path_IsFile(const char* path) {
    SDL_PathInfo info;

    return SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_FILE;
}

static bool Path_Same(const char* a, const char* b) {
    for (; *a && *b; a++, b++) {
        int x = *a == '\\' ? '/' : SDL_tolower((unsigned char)*a);
        int y = *b == '\\' ? '/' : SDL_tolower((unsigned char)*b);

        if (x != y) {
            return false;
        }
    }
    return !*a && !*b;
}

static void Settings_Path(char* path, size_t size) {
    SDL_snprintf(path, size, "%sbaseq3/launcher.cfg", basePath);
}

static void Cfg_Word(const char* text, const char* key, char* out, size_t size) {
    size_t n = strlen(key), i = 0;
    const char* p = text;

    out[0] = '\0';
    while (p && *p) {
        if (!strncmp(p, key, n) && p[n] == ' ') {
            for (p += n + 1; *p && *p != '\r' && *p != '\n' && i < size - 1; p++) {
                out[i++] = *p;
            }
            out[i] = '\0';
            return;
        }
        p = strchr(p, '\n');
        if (p) {
            p++;
        }
    }
}

static bool Game_Version(const char* root, char* out, size_t size) {
    static const char mark[] = "ioquakelive ";
    char path[1200];
    size_t length, i, n;
    char* data;

    SDL_snprintf(path, sizeof(path), "%s" GAME_NAME, root);
    data = SDL_LoadFile(path, &length);
    if (!data) {
        return false;
    }
    for (i = 0; i + sizeof(mark) < length; i++) {
        if (data[i] != 'i' || SDL_memcmp(data + i, mark, sizeof(mark) - 1) || !SDL_isdigit((unsigned char)data[i + sizeof(mark) - 1])) {
            continue;
        }
        i += sizeof(mark) - 1;
        for (n = 0; i < length && n + 1 < size && (SDL_isalnum((unsigned char)data[i]) || data[i] == '.' || data[i] == '-'); i++) {
            out[n++] = data[i];
        }
        out[n] = '\0';
        SDL_free(data);
        return true;
    }
    SDL_free(data);
    return false;
}

static void Settings_Load(void) {
    char path[1024], version[32] = "", by[32] = "", found[32];
    char* text;

    Settings_Path(path, sizeof(path));
    text = SDL_LoadFile(path, NULL);
    firstRun = !text;
    if (text) {
        Cfg_Word(text, "version", version, sizeof(version));
        Cfg_Word(text, "by", by, sizeof(by));
    }
    if (gameFound && Game_Version(basePath, found, sizeof(found))) {
        SDL_strlcpy(installedVersion, found, sizeof(installedVersion));
    } else if (version[0] && (!SDL_strcmp(by, PRODUCT_VERSION) || Version_Compare(version, PRODUCT_VERSION) >= 0)) {
        SDL_strlcpy(installedVersion, version, sizeof(installedVersion));
    } else if (gameFound) {
        SDL_strlcpy(installedVersion, "0", sizeof(installedVersion));
    }
    setupSaved = !text                   ? STEP_NONE
                 : strstr(text, "setup 1") ? STEP_FILES
                 : strstr(text, "setup 2") ? STEP_SETTINGS
                 : strstr(text, "setup 3") ? STEP_DEMOS
                 : strstr(text, "setup 4") ? STEP_PREFS
                                           : STEP_NONE;
    betaChannel = text && strstr(text, "channel beta") != NULL;
    SDL_free(text);
}

static void Settings_Write(const char* root) {
    char path[1200], text[192];

    SDL_snprintf(text, sizeof(text), "version %s\nby " PRODUCT_VERSION "\n", installedVersion);
    if (setupSaved != STEP_NONE) {
        SDL_snprintf(text + strlen(text), sizeof(text) - strlen(text), "setup %d\n", setupSaved);
    }
    if (betaChannel) {
        SDL_strlcat(text, "channel beta\n", sizeof(text));
    }
    SDL_snprintf(path, sizeof(path), "%sbaseq3/launcher.cfg", root);
    SDL_SaveFile(path, text, strlen(text));
}

static void Settings_Save(void) {
    Settings_Write(basePath);
}

static bool Pak_Valid(const char* path) {
    SDL_PathInfo info;

    return SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_FILE && info.size == PAK_GAME_SIZE;
}

static void Wake(void) {
    SDL_Event event;

    SDL_zero(event);
    event.type = SDL_EVENT_USER;
    SDL_PushEvent(&event);
}

static bool File_Link(const char* from, const char* to) {
#ifdef _WIN32
    wchar_t a[1024], b[1024];

    return MultiByteToWideChar(CP_UTF8, 0, from, -1, a, 1024) && MultiByteToWideChar(CP_UTF8, 0, to, -1, b, 1024) &&
           CreateHardLinkW(b, a, NULL);
#else
    return link(from, to) == 0;
#endif
}

static bool File_Copy(const char* from, const char* to) {
    char part[1210];
    SDL_IOStream* in = SDL_IOFromFile(from, "rb");
    SDL_IOStream* out;
    char* block;
    size_t n;
    bool ok;

    SDL_snprintf(part, sizeof(part), "%s.part", to);
    out = in ? SDL_IOFromFile(part, "wb") : NULL;
    block = out ? malloc(COPY_BLOCK) : NULL;
    ok = block != NULL;
    while (ok && (n = SDL_ReadIO(in, block, COPY_BLOCK)) > 0) {
        ok = SDL_WriteIO(out, block, n) == n && !SDL_GetAtomicInt(&jobStop);
        SDL_AddAtomicInt(&jobKb, (int)(n >> 10));
    }
    ok = ok && SDL_GetIOStatus(in) == SDL_IO_STATUS_EOF;
    free(block);
    if (in) {
        SDL_CloseIO(in);
    }
    if (out && !SDL_CloseIO(out)) {
        ok = false;
    }
    if (ok) {
        ok = SDL_RenamePath(part, to);
    }
    if (!ok && out) {
        SDL_RemovePath(part);
    }
    return ok;
}

static bool File_Place(const char* from, const char* to, Sint64 size, bool link) {
    SDL_PathInfo info;

    if (SDL_GetPathInfo(to, &info) || (link && File_Link(from, to))) {
        SDL_AddAtomicInt(&jobKb, (int)(size >> 10));
        return true;
    }
    return File_Copy(from, to);
}

static SDL_EnumerationResult SDLCALL Tree_Each(void* userdata, const char* dir, const char* name) {
    walk_t* walk = userdata;
    char from[1200], to[1200];
    SDL_PathInfo info;
    const char* ext = strrchr(name, '.');

    if (SDL_GetAtomicInt(&jobStop)) {
        return SDL_ENUM_SUCCESS;
    }
    SDL_snprintf(from, sizeof(from), "%s%s", dir, name);
    if (!SDL_GetPathInfo(from, &info)) {
        return SDL_ENUM_CONTINUE;
    }
    if (info.type == SDL_PATHTYPE_DIRECTORY) {
        if (walk->kind != KIND_CONFIG && walk->depth < 6) {
            walk_t inner = *walk;

            inner.depth++;
            if (walk->to) {
                SDL_snprintf(to, sizeof(to), "%s/%s", walk->to, name);
                inner.to = to;
            }
            SDL_EnumerateDirectory(from, Tree_Each, &inner);
            walk->ok = walk->ok && inner.ok;
        }
        return SDL_ENUM_CONTINUE;
    }
    if (info.type != SDL_PATHTYPE_FILE || (walk->kind == KIND_CONFIG && (!ext || SDL_strcasecmp(ext, ".cfg"))) ||
        (walk->kind == KIND_DEMOS && (!ext || SDL_strncasecmp(ext, ".dm_", 4)))) {
        return SDL_ENUM_CONTINUE;
    }
    if (!walk->to) {
        setup.count[walk->kind]++;
        setup.bytes[walk->kind] += (Sint64)info.size;
        return SDL_ENUM_CONTINUE;
    }
    SDL_CreateDirectory(walk->to);
    SDL_snprintf(to, sizeof(to), "%s/%s", walk->to, name);
    if (!File_Place(from, to, (Sint64)info.size, walk->link)) {
        walk->ok = false;
    }
    return SDL_ENUM_CONTINUE;
}

static bool Tree_Walk(int kind, const char* to, bool link) {
    static const char* const folders[KIND_COUNT] = {"", "/ui", "/demos"};
    char from[1200], dest[1200];
    walk_t walk;

    memset(&walk, 0, sizeof(walk));
    walk.kind = kind;
    walk.link = link;
    walk.ok = true;
    if (to) {
        SDL_snprintf(dest, sizeof(dest), "%s%s", to, folders[kind]);
        walk.to = dest;
    }
    SDL_snprintf(from, sizeof(from), "%s%s", setup.user, folders[kind]);
    SDL_EnumerateDirectory(from, Tree_Each, &walk);
    return walk.ok;
}

static bool Steam_Root(char* out, int size) {
#ifdef _WIN32
    wchar_t wide[1024];
    DWORD bytes = sizeof(wide);

    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", L"SteamPath", RRF_RT_REG_SZ, NULL, wide, &bytes) != ERROR_SUCCESS) {
        return false;
    }
    return WideCharToMultiByte(CP_UTF8, 0, wide, -1, out, size, NULL, NULL) > 0;
#elif defined(__linux__)
    static const char* const dirs[] = {".steam/steam", ".local/share/Steam", ".var/app/com.valvesoftware.Steam/.local/share/Steam"};
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
    char path[1200];
    SDL_PathInfo info;
    int i;

    for (i = 0; home && i < (int)SDL_arraysize(dirs); i++) {
        SDL_snprintf(path, sizeof(path), "%s%s/steamapps", home, dirs[i]);
        if (SDL_GetPathInfo(path, &info) && info.type == SDL_PATHTYPE_DIRECTORY) {
            SDL_snprintf(out, size, "%s%s", home, dirs[i]);
            return true;
        }
    }
    return false;
#elif defined(__APPLE__)
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
    char path[1200];
    SDL_PathInfo info;

    if (!home) {
        return false;
    }
    SDL_snprintf(path, sizeof(path), "%sLibrary/Application Support/Steam/steamapps", home);
    if (!SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_DIRECTORY) {
        return false;
    }
    SDL_snprintf(out, size, "%sLibrary/Application Support/Steam", home);
    return true;
#else
    (void)out;
    (void)size;
    return false;
#endif
}

static bool Steam_Running(void) {
#ifdef _WIN32
    wchar_t image[1024];
    DWORD pid = 0, bytes = sizeof(pid), size = 1024;
    HANDLE process;
    bool running = false;

    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", L"pid", RRF_RT_REG_DWORD, NULL, &pid, &bytes) != ERROR_SUCCESS || !pid) {
        return false;
    }
    process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    if (QueryFullProcessImageNameW(process, 0, image, &size) && size >= 10) {
        running = !_wcsicmp(image + size - 10, L"\\steam.exe");
    }
    CloseHandle(process);
    return running;
#elif defined(__linux__)
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
    char path[1200];
    char* text;
    long pid;
    bool running;

    SDL_snprintf(path, sizeof(path), "%s.steam/steam.pid", home ? home : "/");
    text = SDL_LoadFile(path, NULL);
    if (!text) {
        return true;
    }
    pid = atol(text);
    SDL_free(text);
    if (pid <= 0) {
        return true;
    }
    SDL_snprintf(path, sizeof(path), "/proc/%ld/comm", pid);
    text = SDL_LoadFile(path, NULL);
    running = text && !strncmp(text, "steam", 5);
    SDL_free(text);
    return running;
#else
    return true;
#endif
}

static void Steam_Poll(Uint64 now) {
    char root[1024];

    if (steamChecked && now - steamChecked < 2000) {
        return;
    }
    steamChecked = now ? now : 1;
    steamMissing = Steam_Root(root, sizeof(root)) && !Steam_Running();
#ifdef LAUNCHER_SHOT
    if (SDL_getenv("LAUNCHER_STEAMOFF")) {
        steamMissing = true;
    }
#endif
}

static bool QL_Try(const char* library) {
    char path[1200];

    SDL_snprintf(path, sizeof(path), "%s/steamapps/common/Quake Live/baseq3/pak00.pk3", library);
    if (!Pak_Valid(path)) {
        return false;
    }
    SDL_strlcpy(setup.pak, path, sizeof(setup.pak));
    SDL_snprintf(setup.ql, sizeof(setup.ql), "%s/steamapps/common/Quake Live", library);
    return true;
}

static bool QL_Find(void) {
    char root[1024], path[1200], library[1024];
    char* text;
    char* p;

#ifdef LAUNCHER_SHOT
    if (SDL_getenv("LAUNCHER_NOSTEAM")) {
        return false;
    }
#endif
    if (!Steam_Root(root, sizeof(root))) {
        return false;
    }
    if (QL_Try(root)) {
        return true;
    }
    SDL_snprintf(path, sizeof(path), "%s/steamapps/libraryfolders.vdf", root);
    text = SDL_LoadFile(path, NULL);
    if (!text) {
        return false;
    }
    for (p = text; (p = strstr(p, "\"path\"")) != NULL;) {
        int n = 0;

        p = strchr(p + 6, '"');
        if (!p) {
            break;
        }
        for (p++; *p && *p != '"' && n < (int)sizeof(library) - 1; p++) {
            if (p[0] == '\\' && p[1] == '\\') {
                p++;
            }
            library[n++] = *p;
        }
        library[n] = '\0';
        if (QL_Try(library)) {
            SDL_free(text);
            return true;
        }
    }
    SDL_free(text);
    return false;
}

static SDL_EnumerationResult SDLCALL User_Each(void* userdata, const char* dir, const char* name) {
    char path[1200];
    SDL_PathInfo info;
    const char* c;

    (void)userdata;
    for (c = name; *c; c++) {
        if (*c < '0' || *c > '9') {
            return SDL_ENUM_CONTINUE;
        }
    }
    SDL_snprintf(path, sizeof(path), "%s%s/baseq3/qzconfig.cfg", dir, name);
    if (SDL_GetPathInfo(path, &info) && (Sint64)info.modify_time > userWhen) {
        userWhen = (Sint64)info.modify_time;
        SDL_snprintf(setup.user, sizeof(setup.user), "%s%s/baseq3", dir, name);
    }
    return SDL_ENUM_CONTINUE;
}

static void Setup_Scan(void) {
    char test[1200];
    int kind;

    if (!setup.picked) {
        setup.found = QL_Find();
    }
    memset(setup.count, 0, sizeof(setup.count));
    memset(setup.bytes, 0, sizeof(setup.bytes));
    setup.user[0] = '\0';
    setup.canLink = false;
    if (!setup.found) {
        return;
    }
    SDL_snprintf(test, sizeof(test), "%s" PAK_GAME ".link", basePath);
    SDL_RemovePath(test);
    setup.canLink = File_Link(setup.pak, test);
    SDL_RemovePath(test);
    if (!setup.ql[0]) {
        return;
    }
    userWhen = 0;
    SDL_EnumerateDirectory(setup.ql, User_Each, NULL);
    for (kind = 0; setup.user[0] && kind < KIND_COUNT; kind++) {
        Tree_Walk(kind, NULL, false);
    }
}

static bool Zip_Safe(const char* name) {
    const char* p;

    if (!name[0] || name[0] == '/') {
        return false;
    }
    for (p = name; *p; p++) {
        if (*p == '\\' || *p == ':' || (unsigned char)*p < 0x20) {
            return false;
        }
        if (p[0] == '.' && p[1] == '.' && (p == name || p[-1] == '/') && (p[2] == '/' || !p[2])) {
            return false;
        }
    }
    return true;
}

static bool Zip_Next(const unsigned char* dir, unsigned dirSize, const unsigned char** at, char* name, size_t size) {
    const unsigned char* p = *at;
    unsigned n;

    if (p + 46 > dir + dirSize || Rd32(p) != 0x02014b50) {
        return false;
    }
    n = Rd16(p + 28);
    if (p + 46 + n > dir + dirSize || n >= size) {
        return false;
    }
    memcpy(name, p + 46, n);
    name[n] = '\0';
    *at = p + 46 + n + Rd16(p + 30) + Rd16(p + 32);
    return true;
}

static void Zip_Folders(const char* root, const char* name) {
    char path[1400];
    const char* cut;

    for (cut = strchr(name, '/'); cut; cut = strchr(cut + 1, '/')) {
        SDL_snprintf(path, sizeof(path), "%s%.*s", root, (int)(cut - name), name);
        SDL_CreateDirectory(path);
    }
}

static bool Zip_Unpack(SDL_IOStream* io, const unsigned char* entry, const char* to) {
    unsigned method = Rd16(entry + 10), crc = Rd32(entry + 16), left = Rd32(entry + 20), size = Rd32(entry + 24), header = Rd32(entry + 42);
    unsigned char local[30];
    unsigned char* in = malloc(ZIP_BLOCK);
    unsigned char* out = malloc(ZIP_BLOCK);
    SDL_IOStream* file;
    z_stream zs;
    unsigned long sum = crc32(0L, Z_NULL, 0);
    unsigned done = 0;
    int result = Z_OK;
    bool packed = false;
    bool ok = in && out && (method == 0 || method == 8) && Pak_ReadAt(io, header, local, sizeof(local)) && Rd32(local) == 0x04034b50 &&
              SDL_SeekIO(io, (Sint64)header + 30 + Rd16(local + 26) + Rd16(local + 28), SDL_IO_SEEK_SET) >= 0;

    memset(&zs, 0, sizeof(zs));
    if (ok && method == 8) {
        packed = inflateInit2(&zs, -MAX_WBITS) == Z_OK;
        ok = packed;
    }
    file = ok ? SDL_IOFromFile(to, "wb") : NULL;
    ok = file != NULL;
    while (ok && result != Z_STREAM_END && (left > 0 || method == 8)) {
        size_t n = left < ZIP_BLOCK ? left : ZIP_BLOCK;

        if (!n || SDL_ReadIO(io, in, n) != n) {
            ok = false;
            break;
        }
        left -= (unsigned)n;
        if (method == 0) {
            ok = SDL_WriteIO(file, in, n) == n;
            sum = crc32(sum, in, (uInt)n);
            done += (unsigned)n;
            Job_Add(n);
        } else {
            zs.next_in = in;
            zs.avail_in = (uInt)n;
            do {
                size_t got;

                zs.next_out = out;
                zs.avail_out = ZIP_BLOCK;
                result = inflate(&zs, Z_NO_FLUSH);
                if (result != Z_OK && result != Z_STREAM_END && result != Z_BUF_ERROR) {
                    ok = false;
                    break;
                }
                got = ZIP_BLOCK - zs.avail_out;
                if (got) {
                    ok = SDL_WriteIO(file, out, got) == got;
                    sum = crc32(sum, out, (uInt)got);
                    done += (unsigned)got;
                    Job_Add(got);
                }
            } while (ok && result != Z_STREAM_END && zs.avail_out == 0);
        }
        ok = ok && !SDL_GetAtomicInt(&jobStop);
    }
    if (packed) {
        inflateEnd(&zs);
    }
    if (file && !SDL_CloseIO(file)) {
        ok = false;
    }
    free(in);
    free(out);
    ok = ok && done == size && (unsigned)sum == crc;
    if (!ok && file) {
        SDL_RemovePath(to);
    }
    return ok;
}

static bool File_Free(const char* path) {
    SDL_IOStream* io = SDL_IOFromFile(path, "r+b");

    if (!io) {
        return false;
    }
    SDL_CloseIO(io);
    return true;
}

static bool File_Wait(const char* path) {
    int i;

    for (i = 0; i < 50 && !SDL_GetAtomicInt(&jobStop); i++) {
        if (File_Free(path)) {
            return true;
        }
        SDL_Delay(100);
    }
    return false;
}

static void Zip_Mode(const unsigned char* entry, const char* path) {
#ifdef _WIN32
    (void)entry;
    (void)path;
#else
    unsigned mode = Rd32(entry + 38) >> 16 & 0777;

    if (entry[5] == 3 && mode) {
        chmod(path, (mode_t)mode);
    }
#endif
}

static void File_Exec(const char* path) {
#ifdef _WIN32
    (void)path;
#else
    chmod(path, 0755);
#endif
}

#ifdef __APPLE__
static bool Tree_Copy(const char* from, const char* to) {
    char a[1400], b[1400];
    char** names;
    char* cut;
    struct stat st;
    int count = 0, i;
    bool ok;

    names = SDL_GlobDirectory(from, NULL, 0, &count);
    ok = names != NULL && SDL_CreateDirectory(to);
    for (i = 0; ok && i < count; i++) {
        SDL_snprintf(a, sizeof(a), "%s/%s", from, names[i]);
        SDL_snprintf(b, sizeof(b), "%s/%s", to, names[i]);
        if (lstat(a, &st) != 0) {
            ok = false;
        } else if (S_ISDIR(st.st_mode)) {
            ok = SDL_CreateDirectory(b);
        } else if (S_ISREG(st.st_mode)) {
            cut = strrchr(b, '/');
            *cut = '\0';
            ok = SDL_CreateDirectory(b);
            *cut = '/';
            ok = ok && File_Copy(a, b) && chmod(b, st.st_mode & 0777) == 0;
        }
    }
    SDL_free(names);
    return ok;
}
#endif

static bool Self_Copy(const char* root, const char* exe) {
#ifdef __APPLE__
    char to[1200];

    if (selfApp[0]) {
        SDL_snprintf(to, sizeof(to), "%s" SELF_APP, root);
        return Tree_Copy(selfApp, to);
    }
#else
    (void)root;
#endif
    return File_Copy(selfPath, exe);
}

static bool Zip_Swap(const char* from, const char* to) {
    char old[1400];

    if (Path_Same(to, selfPath)) {
        SDL_snprintf(old, sizeof(old), "%s.old", to);
        SDL_RemovePath(old);
        if (!SDL_RenamePath(to, old)) {
            return false;
        }
    }
    return SDL_RenamePath(from, to);
}

static int Zip_Extract(const char* zip, const char* root, char* version, size_t versionSize) {
    char note[64], name[260], from[1400], to[1400];
    SDL_IOStream* io = SDL_IOFromFile(zip, "rb");
    unsigned char* dir = NULL;
    const unsigned char* p;
    const unsigned char* entry;
    unsigned count = 0, dirSize = 0, k;
    Sint64 total = 0;
    int fail = FAIL_NONE, pass;

    if (io) {
        dir = Zip_Dir(io, &count, &dirSize, note, sizeof(note));
    }
    if (!dir) {
        if (io) {
            SDL_CloseIO(io);
        }
        return FAIL_FILE;
    }
    for (pass = 0; pass < 5; pass++) {
        if (fail != FAIL_NONE && pass < 4) {
            pass = 3;
            continue;
        }
        if (pass == 4 && fail == FAIL_NONE) {
            break;
        }
        if (pass == 1) {
            jobBytes = 0;
            SDL_SetAtomicInt(&jobKb, 0);
            SDL_SetAtomicInt(&jobTotal, (int)(total >> 10));
        }
        p = dir;
        for (k = 0; k < count && (fail == FAIL_NONE || pass == 4); k++) {
            entry = p;
            if (!Zip_Next(dir, dirSize, &p, name, sizeof(name))) {
                fail = FAIL_FILE;
                break;
            }
            if (!name[0] || name[strlen(name) - 1] == '/') {
                continue;
            }
            SDL_snprintf(to, sizeof(to), "%s%s", root, name);
            SDL_snprintf(from, sizeof(from), "%s.new", to);
            switch (pass) {
                case 0:
                    if (!Zip_Safe(name)) {
                        fail = FAIL_FILE;
                    }
                    total += Rd32(entry + 24);
                    break;
                case 1:
                    Zip_Folders(root, name);
                    if (!Zip_Unpack(io, entry, from)) {
                        fail = FAIL_FILE;
                    }
                    Zip_Mode(entry, from);
                    break;
                case 2:
                    if (!Path_Same(to, selfPath) && Path_IsFile(to) && !File_Wait(to)) {
                        fail = FAIL_BUSY;
                    }
                    break;
                case 3:
                    if (!Zip_Swap(from, to)) {
                        fail = FAIL_FILE;
                    }
                    break;
                default:
                    SDL_RemovePath(from);
                    break;
            }
        }
    }
    free(dir);
    SDL_CloseIO(io);
    if (fail == FAIL_NONE && version && !strncmp(note, PACKAGE_NOTE, strlen(PACKAGE_NOTE))) {
        const char* v = note + strlen(PACKAGE_NOTE);
        size_t n = 0;

        while (n < versionSize - 1 && ((v[n] >= '0' && v[n] <= '9') || (v[n] >= 'a' && v[n] <= 'z') || v[n] == '.' || v[n] == '-')) {
            n++;
        }
        if (n) {
            memcpy(version, v, n);
            version[n] = '\0';
        }
    }
    return fail;
}

static void Install_Refresh(void) {
    char path[1200];

    SDL_snprintf(path, sizeof(path), "%s/" GAME_NAME, install.dir);
    install.adopt = Path_IsFile(path);
    install.fail = FAIL_NONE;
}

static int Install_Work(bool update) {
    char root[1100], path[1200], exe[1200], hash[65];
    const char* source = install.zip;
    const char* desktop = SDL_GetUserFolder(SDL_FOLDER_DESKTOP);
    pack_t* pack = update ? &packLatest : &packJob;
    bool fetched = false, local = !update && install.local;
    int fail;
#ifdef LAUNCHER_SHOT
    const char* option = SDL_getenv("LAUNCHER_ZIP");

    if (update && option && option[0]) {
        source = option;
        local = true;
    }
#endif

    SDL_strlcpy(jobVersion, PRODUCT_VERSION, sizeof(jobVersion));
    if (update) {
        SDL_strlcpy(root, basePath, sizeof(root));
    } else {
        SDL_snprintf(root, sizeof(root), "%s/", install.dir);
        SDL_snprintf(path, sizeof(path), "%sbaseq3", root);
        SDL_snprintf(exe, sizeof(exe), "%s" PART_NAME, root);
        if (!SDL_CreateDirectory(path) || !SDL_SaveFile(exe, "", 0)) {
            return FAIL_FOLDER;
        }
        SDL_RemovePath(exe);
    }
    if (!update && install.adopt && !Game_Version(root, jobVersion, sizeof(jobVersion))) {
        SDL_strlcpy(jobVersion, "0", sizeof(jobVersion));
    }
    if (update || !install.adopt) {
        if (!local) {
            if (!update && (!Release_Fetch(jobText, sizeof(jobText)) || !Package_Find(jobText, pack))) {
                return FAIL_NET;
            }
            if (jobKind == JOB_REPAIR) {
                SDL_snprintf(path, sizeof(path), TAG_API "%s", installedVersion);
                if (Http_Get(path, jobText, sizeof(jobText)) && Package_Find(jobText, &packJob)) {
                    pack = &packJob;
                } else if (!pack->url[0]) {
                    return FAIL_NET;
                }
            }
            SDL_snprintf(path, sizeof(path), "%s" PART_NAME, root);
            SDL_SetAtomicInt(&jobTotal, (int)(pack->size >> 10));
            hash[0] = '\0';
            if (!Http_File(pack->url, path, hash) || SDL_strcasecmp(hash, pack->hash)) {
                SDL_RemovePath(path);
                return FAIL_NET;
            }
            SDL_strlcpy(jobVersion, pack->version, sizeof(jobVersion));
            source = path;
            fetched = true;
        }
        SDL_SetAtomicInt(&jobPhase, PHASE_INSTALL);
        fail = Zip_Extract(source, root, fetched ? NULL : jobVersion, sizeof(jobVersion));
        if (fetched) {
            SDL_RemovePath(path);
        }
        if (fail != FAIL_NONE) {
            return fail;
        }
    }
    if (!update) {
        SDL_snprintf(exe, sizeof(exe), "%s" SELF_NAME, root);
        if ((install.adopt || !Path_IsFile(exe)) && !Path_Same(exe, selfPath) && !Self_Copy(root, exe)) {
            return FAIL_FILE;
        }
        File_Exec(exe);
        if (install.shortcut) {
#ifdef LAUNCHER_SHOT
            option = SDL_getenv("LAUNCHER_DESKTOP");
            if (option && option[0]) {
                desktop = option;
            }
#endif
            Shortcut_Make(exe, install.dir, desktop);
        }
    }
    return FAIL_NONE;
}

static int SDLCALL Job_Thread(void* unused) {
    char to[1200];
    bool ok = true;
    int fail;

    (void)unused;
    SDL_snprintf(to, sizeof(to), "%sbaseq3", basePath);
    switch (jobKind) {
        case JOB_INSTALL:
        case JOB_UPDATE:
        case JOB_REPAIR:
            fail = Install_Work(jobKind != JOB_INSTALL);
            SDL_SetAtomicInt(&jobFail, fail);
            ok = fail == FAIL_NONE;
            break;
        case JOB_SCAN:
            Setup_Scan();
            break;
        case JOB_FILES:
            SDL_CreateDirectory(to);
            SDL_snprintf(to, sizeof(to), "%s" PAK_GAME, basePath);
            if (!Pak_Valid(to)) {
                SDL_RemovePath(to);
            }
            ok = File_Place(setup.pak, to, PAK_GAME_SIZE, setup.canLink) && Pak_Valid(to);
            break;
        case JOB_SETTINGS:
            if (setup.want[KIND_CONFIG]) {
                ok = Tree_Walk(KIND_CONFIG, to, false);
            }
            if (setup.want[KIND_HUD]) {
                ok = Tree_Walk(KIND_HUD, to, false) && ok;
            }
            break;
        default:
            ok = Tree_Walk(KIND_DEMOS, to, setup.canLink);
            break;
    }
    SDL_SetAtomicInt(&jobRun, ok && !SDL_GetAtomicInt(&jobStop) ? RUN_OK : RUN_FAIL);
    Wake();
    return 0;
}

static void Job_Start(int kind, Sint64 bytes) {
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY) {
        return;
    }
    if (jobThread) {
        SDL_WaitThread(jobThread, NULL);
    }
    jobKind = kind;
    jobBytes = 0;
    SDL_SetAtomicInt(&jobTotal, (int)(bytes >> 10));
    SDL_SetAtomicInt(&jobKb, 0);
    SDL_SetAtomicInt(&jobFail, FAIL_NONE);
    SDL_SetAtomicInt(&jobPhase, kind == JOB_INSTALL && (install.local || install.adopt) ? PHASE_INSTALL : PHASE_DOWNLOAD);
    SDL_SetAtomicInt(&jobRun, RUN_BUSY);
    jobThread = SDL_CreateThread(Job_Thread, "job", NULL);
    if (!jobThread) {
        SDL_SetAtomicInt(&jobRun, RUN_FAIL);
    }
}

static void Setup_Enter(int step) {
    if (step != STEP_NONE && setup.step != STEP_NONE && step != setup.step) {
        stepDir = step > setup.step ? 1 : -1;
        stepFrom = setup.step;
        stepHeat = 0.0f;
        stepStart = SDL_GetTicksNS();
    }
    if (step == STEP_PREFS) {
        prefStartup = Autostart_On();
        prefBeta = betaChannel;
    }
    setup.step = step;
    setup.failed = false;
    setup.wrongFile = false;
    setupSaved = step;
    Settings_Save();
    if (step == STEP_NONE) {
        notesHeat = 0.0f;
    }
}

static void Setup_Next(void) {
    Setup_Enter(setup.step >= STEP_PREFS ? STEP_NONE : setup.step + 1);
}

static void Setup_Back(void) {
    if (install.active || setup.step <= STEP_FILES || SDL_GetAtomicInt(&jobRun) == RUN_BUSY) {
        return;
    }
    Setup_Enter(setup.step - 1);
}

static float Job_Done(void) {
    int total = SDL_GetAtomicInt(&jobTotal);
    float done = (float)SDL_GetAtomicInt(&jobKb) / (float)(total > 0 ? total : 1);

    return done < 0.0f ? 0.0f : done > 1.0f ? 1.0f : done;
}

static bool Install_Finish(void) {
    char root[1100], path[1200];
    const char* args[36];
    SDL_Process* process;
    int i, n = 1;

    if (jobKind == JOB_UPDATE || jobKind == JOB_REPAIR) {
        SDL_strlcpy(root, basePath, sizeof(root));
    } else {
        SDL_snprintf(root, sizeof(root), "%s/", install.dir);
    }
    SDL_snprintf(path, sizeof(path), "%sbaseq3/launcher.cfg", root);
    if (!install.adopt || !Path_IsFile(path)) {
        SDL_strlcpy(installedVersion, jobVersion, sizeof(installedVersion));
        Settings_Write(root);
    }
    SDL_snprintf(path, sizeof(path), "%s" SELF_NAME, root);
    install.done = true;
#ifdef LAUNCHER_SHOT
    SDL_Log("install: ok, version %s, launcher %d", installedVersion, (int)Path_IsFile(path));
    (void)args;
    (void)process;
    (void)i;
    (void)n;
    return false;
#else
    args[0] = Path_IsFile(path) ? path : selfPath;
    args[n++] = "--wait";
    if (startMinimized) {
        args[n++] = "--startup";
    }
    if (rendererHint >= 0) {
        args[n++] = "--renderer";
        args[n++] = rendererHint == 1 ? "vulkan" : "opengl";
    }
    for (i = 0; i < gameArgCount && i < 31; i++) {
        args[n++] = gameArgs[i];
    }
    args[n] = NULL;
    process = SDL_CreateProcess(args, false);
    if (!process) {
        install.fail = FAIL_FILE;
        return false;
    }
    SDL_DestroyProcess(process);
    return true;
#endif
}

static bool Job_Poll(void) {
    int run = SDL_GetAtomicInt(&jobRun);

    if (run != RUN_OK && run != RUN_FAIL) {
        return false;
    }
    SDL_SetAtomicInt(&jobRun, RUN_IDLE);
    if (jobKind == JOB_SCAN) {
        return false;
    }
    if (jobKind == JOB_INSTALL || jobKind == JOB_UPDATE || jobKind == JOB_REPAIR) {
        if (run == RUN_OK) {
            return Install_Finish();
        }
        install.fail = SDL_GetAtomicInt(&jobFail);
        if (install.fail == FAIL_NONE) {
            install.fail = FAIL_FILE;
        }
#ifdef LAUNCHER_SHOT
        SDL_Log("install: fail %d", install.fail);
#endif
        return false;
    }
    if (run == RUN_OK) {
        Setup_Next();
    } else {
        setup.failed = true;
    }
    return false;
}

static void Taskbar_Update(void) {
    static int shown = -1;
    static float last = -1.0f;
    int busy = SDL_GetAtomicInt(&jobRun) == RUN_BUSY && jobKind != JOB_SCAN;
    float done;

    if (busy != shown) {
        if (!busy && shown == 1 && !(SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)) {
            SDL_FlashWindow(window, SDL_FLASH_UNTIL_FOCUSED);
        }
        SDL_SetWindowProgressState(window, busy ? SDL_PROGRESS_STATE_NORMAL : SDL_PROGRESS_STATE_NONE);
        shown = busy;
        last = -1.0f;
    }
    if (busy) {
        done = Job_Done();
        if (SDL_fabsf(done - last) >= 0.005f) {
            SDL_SetWindowProgressValue(window, done);
            last = done;
        }
    }
}

static void Install_Start(void) {
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY || install.done) {
        return;
    }
    install.fail = FAIL_NONE;
    Job_Start(JOB_INSTALL, 0);
}

static bool Update_Busy(void) {
    return (jobKind == JOB_UPDATE || jobKind == JOB_REPAIR) && SDL_GetAtomicInt(&jobRun) == RUN_BUSY;
}

static bool Play_Ready(void) {
    return gameFound && !Update_Busy() && SDL_GetAtomicInt(&checkState) != CHECK_RUNNING;
}

static void Update_Auto(void) {
    if (autoTried || SDL_GetAtomicInt(&checkState) != CHECK_DONE || !updateAvailable || !packLatest.url[0] || !gameFound || install.active ||
        setup.step != STEP_NONE || install.done || SDL_GetAtomicInt(&jobRun) == RUN_BUSY) {
        return;
    }
    autoTried = true;
    install.fail = FAIL_NONE;
    Job_Start(JOB_UPDATE, packLatest.size);
}

static void Update_Start(void) {
    char url[128];

    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY || install.done) {
        return;
    }
    install.fail = FAIL_NONE;
#ifdef LAUNCHER_SHOT
    if (SDL_getenv("LAUNCHER_ZIP") && SDL_getenv("LAUNCHER_ZIP")[0]) {
        Job_Start(JOB_UPDATE, 0);
        return;
    }
#endif
    if (packLatest.url[0]) {
        Job_Start(JOB_UPDATE, packLatest.size);
        return;
    }
    SDL_snprintf(url, sizeof(url), RELEASE_PAGE "%s", latestTag);
    SDL_OpenURL(url);
}

static void Check_Again(void) {
    if (SDL_GetAtomicInt(&checkState) == CHECK_RUNNING || SDL_GetAtomicInt(&jobRun) == RUN_BUSY || install.active) {
        return;
    }
    if (checkThread) {
        SDL_WaitThread(checkThread, NULL);
    }
    autoTried = false;
    updateAvailable = false;
    SDL_SetAtomicInt(&checkState, CHECK_RUNNING);
    checkThread = SDL_CreateThread(Check_Thread, "check", NULL);
    if (!checkThread) {
        SDL_SetAtomicInt(&checkState, CHECK_FAILED);
    }
}

static void Repair_Start(void) {
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY || install.done || !gameFound) {
        return;
    }
    install.fail = FAIL_NONE;
    Job_Start(JOB_REPAIR, 0);
}

static void Beta_Set(bool on) {
    if (on == betaChannel) {
        return;
    }
    betaChannel = on;
    Settings_Save();
    Check_Again();
}

static void Folder_Game(void) {
    char path[1200];

    SDL_snprintf(path, sizeof(path), "%sbaseq3", basePath);
    SDL_OpenURL(path);
}

static void Setup_Rerun(void) {
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY || Update_Busy()) {
        return;
    }
    setup.picked = false;
    setup.want[KIND_CONFIG] = setup.want[KIND_HUD] = true;
    stepHeat = 1.0f;
    Setup_Enter(STEP_FILES);
    Job_Start(JOB_SCAN, 0);
}

static void Server_Name(const char* full, char* out, size_t size) {
    size_t n = 0;

    while (*full == ' ') {
        full++;
    }
    while (*full && n + 1 < size) {
        if (full[0] == '^' && full[1] && full[1] != '^') {
            full += 2;
            continue;
        }
        out[n++] = *full++;
    }
    while (n > 0 && out[n - 1] == ' ') {
        n--;
    }
    out[n] = '\0';
}

static int SDLCALL Servers_Thread(void* unused) {
    char seg[2048], raw[160];
    const char* p;
    const char* next;
    int n = 0, total = 0, i, j;
    bool local = false;

    (void)unused;
#ifdef LAUNCHER_SHOT
    if (SDL_getenv("LAUNCHER_SERVERS")) {
        size_t size = 0;
        char* file = SDL_LoadFile(SDL_getenv("LAUNCHER_SERVERS"), &size);

        if (file) {
            SDL_strlcpy(serversText, file, sizeof(serversText));
            SDL_free(file);
            local = true;
        }
    }
#endif
    if (!local && !Http_Get(SERVERS_API, serversText, sizeof(serversText))) {
        SDL_SetAtomicInt(&serversState, 3);
        Wake();
        return 0;
    }
    for (p = strstr(serversText, "\"ip\""); p && n < SRV_MAX; p = next) {
        server_t* s = &serversNext[n];
        size_t len;

        next = strstr(p + 4, "\"ip\"");
        len = next ? (size_t)(next - p) : strlen(p);
        if (len >= sizeof(seg)) {
            len = sizeof(seg) - 1;
        }
        SDL_memcpy(seg, p, len);
        seg[len] = '\0';
        Json_Text(Json_Value(seg, "\"status\""), raw, sizeof(raw));
        if (SDL_strcmp(raw, "online")) {
            continue;
        }
        Json_Text(Json_Value(seg, "\"ip\""), raw, sizeof(raw));
        p = Json_Value(seg, "\"port\"");
        if (!raw[0] || !p) {
            continue;
        }
        SDL_snprintf(s->addr, sizeof(s->addr), "%s:%d", raw, SDL_atoi(p));
        Json_Text(Json_Value(seg, "\"name\""), raw, sizeof(raw));
        Server_Name(raw, s->name, sizeof(s->name));
        Json_Text(Json_Value(seg, "\"map\""), s->map, sizeof(s->map));
        Json_Text(Json_Value(seg, "\"gameType\""), s->mode, sizeof(s->mode));
        p = Json_Value(seg, "\"players\"");
        s->players = p ? SDL_atoi(p) : 0;
        p = Json_Value(seg, "\"maxplayers\"");
        s->max = p ? SDL_atoi(p) : 0;
        total += s->players;
        n++;
    }
    for (i = 1; i < n; i++) {
        server_t key = serversNext[i];

        for (j = i - 1; j >= 0 && serversNext[j].players < key.players; j--) {
            serversNext[j + 1] = serversNext[j];
        }
        serversNext[j + 1] = key;
    }
    serversNextCount = n;
    serversTotal = total;
    SDL_SetAtomicInt(&serversState, n > 0 ? 2 : 3);
    Wake();
    return 0;
}

static void Servers_Fetch(void) {
    int state = SDL_GetAtomicInt(&serversState);

    if (state == 1) {
        return;
    }
    if (serversThread) {
        SDL_WaitThread(serversThread, NULL);
        serversThread = NULL;
    }
    SDL_SetAtomicInt(&serversState, 1);
    serversWhen = SDL_GetTicks();
    serversThread = SDL_CreateThread(Servers_Thread, "servers", NULL);
    if (!serversThread) {
        SDL_SetAtomicInt(&serversState, 3);
    }
}

static const char* Json_End(const char* p) {
    int depth = 0;
    bool text = false;

    for (; *p; p++) {
        if (text) {
            if (*p == '\\' && p[1]) {
                p++;
            } else if (*p == '"') {
                text = false;
            }
        } else if (*p == '"') {
            text = true;
        } else if (*p == '{' || *p == '[') {
            depth++;
        } else if ((*p == '}' || *p == ']') && --depth == 0) {
            return p;
        }
    }
    return NULL;
}

static void Live_Extras(player_t* pl, char* seg) {
    char clan[512], raw[96];
    char* v = (char*)Json_Value(seg, "\"clan\"");
    const char* end;
    char* full;
    int i;

    pl->clan = false;
    pl->clanTag[0] = pl->clanPic[0] = '\0';
    if (v && *v == '{' && (end = Json_End(v)) != NULL) {
        size_t len = SDL_min((size_t)(end - v + 1), sizeof(clan) - 1);

        SDL_memcpy(clan, v, len);
        clan[len] = '\0';
        Json_Text(Json_Value(clan, "\"tag\""), raw, sizeof(raw));
        Server_Name(raw, pl->clanTag, sizeof(pl->clanTag));
        Json_Text(Json_Value(clan, "\"avatarUrl\""), pl->clanPic, sizeof(pl->clanPic));
        pl->clan = true;
        SDL_memset(v, ' ', (size_t)(end - v + 1));
    }
    Json_Text(Json_Value(seg, "\"avatar\""), pl->avatar, sizeof(pl->avatar));
    full = strstr(pl->avatar, "_full.");
    if (full && strlen(pl->avatar) + 3 < sizeof(pl->avatar)) {
        SDL_memmove(full + 8, full + 6, strlen(full + 6) + 1);
        SDL_memcpy(full, "_medium.", 8);
    }
    Json_Text(Json_Value(seg, "\"country\""), raw, sizeof(raw));
    pl->country[0] = '\0';
    if (SDL_strlen(raw) == 2) {
        for (i = 0; i < 2; i++) {
            pl->country[i] = (char)SDL_toupper((unsigned char)raw[i]);
        }
        pl->country[2] = '\0';
        if (pl->country[0] < 'A' || pl->country[0] > 'Z' || pl->country[1] < 'A' || pl->country[1] > 'Z') {
            pl->country[0] = '\0';
        }
    }
}

enum { PRO_LOADING, PRO_READY, PRO_MISSING, PRO_FAILED };
enum { PRO_NONE, PRO_RANKED, PRO_PLACEMENT, PRO_INACTIVE };

static void Json_Cut(char* text, const char* key, char* out, size_t size) {
    char* v = (char*)Json_Value(text, key);
    const char* end;
    size_t len;

    out[0] = '\0';
    if (!v || *v != '{' || (end = Json_End(v)) == NULL) {
        return;
    }
    len = SDL_min((size_t)(end - v + 1), size - 1);
    SDL_memcpy(out, v, len);
    out[len] = '\0';
    SDL_memset(v, ' ', (size_t)(end - v + 1));
}

static float Json_Number(const char* text, const char* key, bool* have) {
    const char* p = Json_Value(text, key);

    if (p && *p == '"') {
        p++;
    }
    *have = p && ((*p >= '0' && *p <= '9') || *p == '-' || *p == '.');
    return *have ? (float)SDL_atof(p) : 0.0f;
}

static Uint32 Color_Hex(const char* text, Uint32 fallback) {
    Uint32 v = 0;
    int i;

    if (text[0] != '#' || SDL_strlen(text) < 7) {
        return fallback;
    }
    for (i = 1; i < 7; i++) {
        char c = (char)SDL_tolower((unsigned char)text[i]);

        if (c >= '0' && c <= '9') {
            v = v * 16 + (Uint32)(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            v = v * 16 + (Uint32)(c - 'a' + 10);
        } else {
            return fallback;
        }
    }
    return (v << 8) | 0xFF;
}

static void Site_Url(char* out, size_t size, const char* path) {
    if (path[0] == '/') {
        SDL_snprintf(out, size, SITE "%s", path);
    } else if (!SDL_strncmp(path, "https://", 8)) {
        SDL_strlcpy(out, path, size);
    } else {
        out[0] = '\0';
    }
}

static bool Steam_IdValid(const char* id) {
    int i;

    for (i = 0; id[i]; i++) {
        if (id[i] < '0' || id[i] > '9') {
            return false;
        }
    }
    return i == 17;
}

static void Identity_Line(const char* text, const char* steamId, char* avatar, size_t avatarSize, char* flag, size_t flagSize) {
    size_t n = SDL_strlen(steamId);
    const char* p = text;

    while (p && *p) {
        if (!SDL_strncmp(p, steamId, n) && p[n] == '\t') {
            char a[200], f[200];
            const char* q = p + n + 1;
            int i = 0;

            while (*q && *q != '\t' && i < (int)sizeof(a) - 1) {
                a[i++] = *q++;
            }
            a[i] = '\0';
            if (*q == '\t') {
                q++;
            }
            i = 0;
            while (*q && *q != '\t' && *q != '\r' && *q != '\n' && i < (int)sizeof(f) - 1) {
                f[i++] = *q++;
            }
            f[i] = '\0';
            if (a[0] == '/') {
                Site_Url(avatar, avatarSize, a);
            }
            if (flag && f[0] == '/') {
                Site_Url(flag, flagSize, f);
            }
            return;
        }
        p = SDL_strchr(p, '\n');
        if (p) {
            p++;
        }
    }
}

static void Live_Identity(player_t* list, int n) {
    char url[1600];
    char* text;
    size_t len;
    int i, ids = 0;

    len = SDL_strlcpy(url, SITE "/api/ioql/identidad?steamIds=", sizeof(url));
    for (i = 0; i < n; i++) {
        if (list[i].steamId[0] && len + 20 < sizeof(url)) {
            len += (size_t)SDL_snprintf(url + len, sizeof(url) - len, "%s%s", ids ? "," : "", list[i].steamId);
            ids++;
        }
    }
    text = ids ? malloc(16384) : NULL;
    if (!text) {
        return;
    }
    if (Http_Get(url, text, 16384)) {
        for (i = 0; i < n; i++) {
            if (list[i].steamId[0]) {
                Identity_Line(text, list[i].steamId, list[i].avatar, sizeof(list[i].avatar), list[i].clan ? list[i].clanPic : NULL,
                              sizeof(list[i].clanPic));
            }
        }
    }
    free(text);
}

enum { HUB_PROFILE, HUB_MATCHES, HUB_RANKING, HUB_SERVERS };
enum { WHY_NONE, WHY_PLACEMENT, WHY_INACTIVE, WHY_BANNED };
enum { HIT_NONE, HIT_MATCH, HIT_PLAYER, HIT_SERVER };
enum { SUB_SUMMARY, SUB_WEAPONS, SUB_MEDALS, SUB_FLAGS };

typedef struct {
    float acc;
    int hits, shots, damage;
    bool used;
} hubWeapon_t;

typedef struct {
    char id[48], map[32], mode[8], server[128], rival[96], date[32];
    int kills, deaths, elo;
    bool win, draw, counts;
} hubGame_t;

typedef struct {
    int status;
    char steamId[24], raw[128], avatar[200], banner[200], flag[200], clanName[96], title[128], since[48], country[4];
    char bio[600];
    Uint32 titleColor;
    int hue;
    bool clan, registered;
    int rating[HUB_MODES], wins[HUB_MODES], losses[HUB_MODES], draws[HUB_MODES], games[HUB_MODES];
    bool rated[HUB_MODES];
} hubPlayer_t;

typedef struct {
    int status;
    int elo, wins, losses, draws, rank, total, left, minGames, played, why;
    bool haveRating, closed, haveRank;
    int kills, deaths, lastAlive, clutches;
    bool haveClutch;
    char favMap[32];
    int weaponCount, weaponOrder[9];
    hubWeapon_t weapons[9];
    int medalCount, medalOrder[16], medal[16];
    int eloCount;
    short eloValue[HUB_ELO_MAX], eloChange[HUB_ELO_MAX];
    char eloDate[HUB_ELO_MAX][12];
    int gameCount;
    hubGame_t games[HUB_RECENT];
} hubStats_t;

typedef struct {
    int status;
    int page, pages, total, count;
    hubGame_t games[HUB_PAGE];
} hubGames_t;

typedef struct {
    char steamId[24], raw[96], avatar[200], clanPic[200], country[4], clanTag[24];
    int team, kills, deaths, score, damage, elo, eloChange, aim;
    int medal[16], flags[5];
    float perf;
    bool havePerf, clan, switched;
    hubWeapon_t weapons[9];
} hubMan_t;

typedef struct {
    int status;
    char map[32], mode[8], title[200], why[8];
    int red, blue, winner, count;
    bool teams, counts, aborted;
    hubMan_t men[HUB_MEN];
} hubMatch_t;

typedef struct {
    char steamId[24], raw[96], avatar[200], clanPic[200], country[4], clanTag[24];
    int rating, games, winRate, rank, lastAlive, clutches;
    float kd;
    bool inactive, clan, haveWin, haveLast, haveClutch;
} hubRank_t;

typedef struct {
    int status;
    int total, count;
    hubRank_t rows[HUB_RANK_PAGE];
} hubRanking_t;

typedef struct {
    SDL_Thread* thread;
    SDL_AtomicInt state;
    char want[128], busy[128], have[128];
    void (*load)(const char* key, void* out);
    void* next;
    void* shown;
    size_t size;
    const char* name;
} slot_t;

typedef struct {
    int kind, index;
    char id[48];
} hubHit_t;

typedef struct {
    Uint32 p1, p2, p3, p4, ink, ink2, gold;
} tone_t;

static const char* const hubModeId[HUB_MODES] = {"ca", "duel", "ctf", "ffa", "tdm", "ad", "ft", "dom"};
static const char* const hubModeName[HUB_MODES] = {"CA", "Duel", "CTF", "FFA", "TDM", "AD", "FT", "DOM"};
static const int hubRankModes[HUB_RANK_MODES] = {0, 2, 1, 4, 3};
static const char* const hubSeasonId[HUB_SEASONS] = {"s1", "s2", "s3"};
static const char* const hubWeaponKey[9] = {"RG", "RL", "PG", "SG", "LG", "GL", "MG", "HMG", "GT"};
static const char* const hubWeaponName[9] = {"Railgun",          "Rocket Launcher", "Plasma Gun",        "Shotgun", "Lightning Gun",
                                             "Grenade Launcher", "Machine Gun",     "Heavy Machine Gun", "Gauntlet"};
static const char* const hubWeaponIcon[9] = {"/weapons/railgun.png", "/weapons/rocket.png",     "/weapons/plasma.png",
                                             "/weapons/shotgun.png", "/weapons/lightning.png",  "/weapons/grenade.png",
                                             "/weapons/machinegun.png", "/weapons/hmg.png",     "/gauntlet.png"};
static const int hubMatchWeapons[5] = {1, 4, 0, 3, 6};
static const char* const hubMedalKey[16] = {"accuracy",  "assists", "captures", "combokill", "defends", "excellent",  "firstfrag", "headshot",
                                            "humiliation", "impressive", "midair", "perfect", "perforated", "quadgod", "rampage", "revenge"};
static const char* const hubMedalName[16] = {"Accuracy", "Assist", "Capture", "Combo Kill", "Defense", "Excellent", "First Frag", "Headshot",
                                             "Humiliation", "Impressive", "Midair", "Perfect", "Perforated", "Quad God", "Rampage", "Revenge"};
static const char* const hubMedalIcon[16] = {
    "/medals/medal_accuracy.png",  "/medals/medal_assist.png",     "/medals/medal_capture.png", "/medals/medal_combokill.png",
    "/medals/medal_defense.png",   "/medals/medal_excellent.png",  "/medals/medal_firstfrag.png", "/medals/medal_headshot.png",
    "/medals/medal_gauntlet.png",  "/medals/medal_impressive.png", "/medals/medal_midair.png",  "/medals/medal_perfect.png",
    "/medals/medal_perforated.png", "/medals/medal_quadgod.png",   "/medals/medal_rampage.png", "/medals/medal_revenge.png"};
static const int hubMatchMedals[16] = {9, 5, 0, 3, 7, 10, 15, 6, 11, 12, 8, 1, 2, 4, 13, 14};
static const char* const hubTierImage[8] = {"/tiers/tier_gran_maestro@128.png", "/tiers/tier_elite@128.png", "/tiers/tier_1@128.png",
                                            "/tiers/tier_2@128.png", "/tiers/tier_3@128.png", "/tiers/tier_4@128.png",
                                            "/tiers/tier_5@128.png", "/tiers/tier_6@128.png"};
static const int hubTierMin[8] = {2500, 2250, 2000, 1800, 1600, 1400, 1200, 0};
static const char* const hubMonths[12] = {"enero", "febrero", "marzo", "abril", "mayo", "junio",
                                          "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre"};
static const char* const hubCountries[][2] = {
    {"CL", "Chile"},          {"AR", "Argentina"},       {"BR", "Brasil"},         {"PE", "Perú"},           {"CO", "Colombia"},
    {"MX", "México"},         {"VE", "Venezuela"},       {"EC", "Ecuador"},        {"UY", "Uruguay"},        {"PY", "Paraguay"},
    {"BO", "Bolivia"},        {"CR", "Costa Rica"},      {"PA", "Panamá"},         {"GT", "Guatemala"},      {"HN", "Honduras"},
    {"SV", "El Salvador"},    {"NI", "Nicaragua"},       {"DO", "República Dominicana"}, {"CU", "Cuba"},     {"PR", "Puerto Rico"},
    {"ES", "España"},         {"US", "Estados Unidos"},  {"CA", "Canadá"},         {"GB", "Reino Unido"},    {"DE", "Alemania"},
    {"FR", "Francia"},        {"IT", "Italia"},          {"PT", "Portugal"},       {"RU", "Rusia"},          {"CN", "China"},
    {"JP", "Japón"},          {"KR", "Corea del Sur"},   {"AU", "Australia"},      {"NZ", "Nueva Zelanda"},  {"SE", "Suecia"},
    {"NO", "Noruega"},        {"FI", "Finlandia"},       {"DK", "Dinamarca"},      {"NL", "Países Bajos"},   {"BE", "Bélgica"},
    {"PL", "Polonia"},        {"UA", "Ucrania"},         {"CZ", "República Checa"}, {"AT", "Austria"},       {"CH", "Suiza"},
    {"IE", "Irlanda"},        {"ZA", "Sudáfrica"},       {"IN", "India"},          {"PH", "Filipinas"},      {"TH", "Tailandia"},
    {"MY", "Malasia"},        {"SG", "Singapur"},        {"ID", "Indonesia"},      {"VN", "Vietnam"},        {"TR", "Turquía"},
    {"IL", "Israel"},         {"SA", "Arabia Saudita"},  {"AE", "Emiratos Árabes"}, {"EG", "Egipto"}};
static const char* const hubMaps[][2] = {
    {"theedge", "The Edge"},           {"hellsgateredux", "Hellsgate Redux"}, {"hellsgate", "Hellsgate"},
    {"hiddenfortress", "Hidden Fortress"}, {"campgrounds", "Campgrounds"},  {"wintercampgrounds", "Winter Campgrounds"},
    {"almostlost", "Almost Lost"},     {"furiousheights", "Furious Heights"}, {"houseofdecay", "House of Decay"},
    {"deepinside", "Deep Inside"},     {"wintersedge", "Winter's Edge"},   {"bloodrun", "Blood Run"},
    {"longestyard", "Longest Yard"},   {"arenagate", "Arena Gate"},        {"blackcathedral", "Black Cathedral"},
    {"brimstoneabbey", "Brimstone Abbey"}, {"cannedheat", "Canned Heat"},  {"coldwar", "Cold War"},
    {"deadandgone", "Dead and Gone"},  {"devilish", "Devilish"},           {"dreadfulplace", "Dreadful Place"},
    {"electrichead", "Electric Head"}, {"evilplayground", "Evil Playground"}, {"falloutbunker", "Fallout Bunker"},
    {"focalpoint", "Focal Point"},     {"grimdungeons", "Grim Dungeons"},  {"industrialrevolution", "Industrial Revolution"},
    {"innersanctums", "Inner Sanctums"}, {"japanesecastles", "Japanese Castles"}, {"lostworld", "Lost World"},
    {"midlifecrisis", "Midlife Crisis"}, {"namelessplace", "Nameless Place"}, {"powerstation", "Power Station"},
    {"provinggrounds", "Proving Grounds"}, {"seamsandbolts", "Seams and Bolts"}, {"shiningforces", "Shining Forces"},
    {"silentnight", "Silent Night"},   {"spacechamber", "Space Chamber"},  {"spillway", "Spillway"},
    {"terminalheights", "Terminal Heights"}, {"theatreofpain", "Theatre of Pain"}, {"thedukesgarden", "The Duke's Garden"},
    {"theepicenter", "The Epicenter"}, {"threestory", "Three Story"},      {"troubledwaters", "Troubled Waters"},
    {"useandabuse", "Use and Abuse"},  {"verticalvengeance", "Vertical Vengeance"}, {"windowpain", "Window Pain"}};

static void Hub_LoadPlayer(const char* key, void* out);
static void Hub_LoadStats(const char* key, void* out);
static void Hub_LoadGames(const char* key, void* out);
static void Hub_LoadMatch(const char* key, void* out);
static void Hub_LoadRanking(const char* key, void* out);

static hubPlayer_t hubPlayer, hubPlayerNext;
static hubStats_t hubStats, hubStatsNext;
static hubGames_t hubGames, hubGamesNext;
static hubMatch_t hubMatch, hubMatchNext;
static hubRanking_t hubRank, hubRankNext;
static slot_t slotPlayer = {NULL, {0}, "", "", "", Hub_LoadPlayer, &hubPlayerNext, &hubPlayer, sizeof(hubPlayer), "hubplayer"};
static slot_t slotStats = {NULL, {0}, "", "", "", Hub_LoadStats, &hubStatsNext, &hubStats, sizeof(hubStats), "hubstats"};
static slot_t slotGames = {NULL, {0}, "", "", "", Hub_LoadGames, &hubGamesNext, &hubGames, sizeof(hubGames), "hubgames"};
static slot_t slotMatch = {NULL, {0}, "", "", "", Hub_LoadMatch, &hubMatchNext, &hubMatch, sizeof(hubMatch), "hubmatch"};
static slot_t slotRank = {NULL, {0}, "", "", "", Hub_LoadRanking, &hubRankNext, &hubRank, sizeof(hubRank), "hubrank"};
static slot_t* const hubSlots[5] = {&slotPlayer, &slotStats, &slotGames, &slotMatch, &slotRank};
static bool hubOpen, hubFonts;
static int hubTab, hubMode, hubSeason = HUB_SEASONS - 1, hubGamesMode, hubPage = 1, hubRankMode, hubRankPage = 1, hubSub, hubHitCount;
static float hubHeat, hubGoal[5], hubView[5], hubMost[5];
static char hubWho[24], hubMatchId[48];
static hubHit_t hubHits[HUB_ROWS];
static tone_t tone;

static void Hub_Pics(void);
static void Hub_Shots(void);
static void Box_Open(const server_t* s);

static SDL_Thread* hubFontThread;
static SDL_AtomicInt hubFontState;
static bool hubFontsTried;
static unsigned char* hubAlpha[FONT_COUNT - FONT_HUB_FIRST];
static int hubAlphaH[FONT_COUNT - FONT_HUB_FIRST];

static int SDLCALL Hub_FontThread(void* unused) {
    int i;

    (void)unused;
    for (i = FONT_HUB_FIRST; i < FONT_COUNT; i++) {
        fonts[i].face = fontFaces[i];
        hubAlpha[i - FONT_HUB_FIRST] =
            Font_Raster(&fonts[i], &faceInfo[fontFaces[i]], fontSizes[i] * uiScale, true, i >= FONT_HUB_SCORE ? 127 : 255, &hubAlphaH[i - FONT_HUB_FIRST]);
    }
    SDL_SetAtomicInt(&hubFontState, 2);
    Wake();
    return 0;
}

static void Hub_FontsStart(void) {
    if (!fontsReady || hubFontThread || hubFontsTried || SDL_GetAtomicInt(&hubFontState)) {
        return;
    }
    SDL_SetAtomicInt(&hubFontState, 1);
    hubFontThread = SDL_CreateThread(Hub_FontThread, "hubfonts", NULL);
    if (!hubFontThread) {
        Hub_FontThread(NULL);
    }
}

static bool Hub_FontsReady(void) {
    int i;

    if (hubFonts || hubFontsTried) {
        return hubFonts;
    }
    Hub_FontsStart();
    if (hubFontThread) {
        SDL_WaitThread(hubFontThread, NULL);
        hubFontThread = NULL;
    }
    if (SDL_GetAtomicInt(&hubFontState) != 2) {
        return false;
    }
    hubFontsTried = true;
    hubFonts = true;
    for (i = FONT_HUB_FIRST; i < FONT_COUNT; i++) {
        unsigned char* alpha = hubAlpha[i - FONT_HUB_FIRST];

        hubAlpha[i - FONT_HUB_FIRST] = NULL;
        if (!alpha || !Font_Upload(&fonts[i], alpha, hubAlphaH[i - FONT_HUB_FIRST])) {
            hubFonts = false;
        }
    }
    return hubFonts;
}

static const char* Json_Array(const char* text, const char* key, const char** end) {
    const char* v = Json_Value(text, key);

    if (!v || *v != '[') {
        return NULL;
    }
    *end = Json_End(v);
    return *end ? v + 1 : NULL;
}

static const char* Json_Item(const char* p, const char* end, char* out, size_t size) {
    const char* e;
    size_t len;

    while (p && p < end && *p != '{') {
        p++;
    }
    if (!p || p >= end || (e = Json_End(p)) == NULL || e > end) {
        return NULL;
    }
    len = SDL_min((size_t)(e - p + 1), size - 1);
    SDL_memcpy(out, p, len);
    out[len] = '\0';
    return e + 1;
}

static int Json_Int(const char* text, const char* key, bool* have) {
    bool dummy;
    float v = Json_Number(text, key, have ? have : &dummy);

    return (int)SDL_floorf(v + 0.5f);
}

static bool Json_True(const char* text, const char* key) {
    const char* v = Json_Value(text, key);

    return v && !SDL_strncmp(v, "true", 4);
}

static void Country_Code(const char* text, char* out) {
    char value[16];
    int i;

    Json_Text(Json_Value(text, "\"countryCode\""), value, sizeof(value));
    out[0] = '\0';
    if (SDL_strlen(value) == 2 && SDL_isalpha((unsigned char)value[0]) && SDL_isalpha((unsigned char)value[1])) {
        for (i = 0; i < 2; i++) {
            out[i] = (char)SDL_toupper((unsigned char)value[i]);
        }
        out[2] = '\0';
    }
}

static bool Iso_Local(const char* iso, SDL_DateTime* out) {
    SDL_DateTime dt;
    SDL_Time t;

    SDL_zero(dt);
    if (SDL_sscanf(iso, "%d-%d-%dT%d:%d:%d", &dt.year, &dt.month, &dt.day, &dt.hour, &dt.minute, &dt.second) != 6) {
        return false;
    }
    return SDL_DateTimeToTime(&dt, &t) && SDL_TimeToDateTime(t, out, true);
}

static void Hub_Game(const char* item, hubGame_t* g) {
    char value[64];
    SDL_DateTime dt;

    Json_Text(Json_Value(item, "\"matchId\""), g->id, sizeof(g->id));
    Json_Text(Json_Value(item, "\"map\""), g->map, sizeof(g->map));
    Json_Text(Json_Value(item, "\"gameType\""), g->mode, sizeof(g->mode));
    Json_Text(Json_Value(item, "\"serverName\""), g->server, sizeof(g->server));
    if (!g->server[0]) {
        SDL_strlcpy(g->server, "Quake Club", sizeof(g->server));
    }
    Json_Text(Json_Value(item, "\"opponent\""), g->rival, sizeof(g->rival));
    g->kills = Json_Int(item, "\"kills\"", NULL);
    g->deaths = Json_Int(item, "\"deaths\"", NULL);
    g->elo = Json_Int(item, "\"eloChange\"", NULL);
    Json_Text(Json_Value(item, "\"result\""), value, sizeof(value));
    g->win = !SDL_strcasecmp(value, "win");
    g->draw = !SDL_strcasecmp(value, "draw");
    Json_Text(Json_Value(item, "\"gameStatus\""), value, sizeof(value));
    g->counts = !Json_Value(item, "\"isRated\":false") && !Json_True(item, "\"isAborted\"") && (!value[0] || !SDL_strcmp(value, "SUCCESS"));
    Json_Text(Json_Value(item, "\"playedAt\""), value, sizeof(value));
    g->date[0] = '\0';
    if (Iso_Local(value, &dt) && dt.month >= 1 && dt.month <= 12) {
        SDL_snprintf(g->date, sizeof(g->date), "%d de %s", dt.day, hubMonths[dt.month - 1]);
    }
}

static int Mode_Index(const char* id) {
    int i;

    for (i = 0; i < HUB_MODES; i++) {
        if (!SDL_strcasecmp(id, hubModeId[i])) {
            return i;
        }
    }
    return -1;
}

static void Hub_LoadPlayer(const char* key, void* out) {
    hubPlayer_t* pl = out;
    char url[256], sub[2048], item[1024], value[256];
    char* text = malloc(HUB_TEXT);
    const char *p, *end;
    SDL_DateTime dt;
    bool have;
    int m;

    pl->status = PRO_FAILED;
    pl->hue = -1;
    SDL_strlcpy(pl->steamId, key, sizeof(pl->steamId));
    if (!text) {
        return;
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/player/%s", key);
    if (!Http_Get(url, text, HUB_TEXT) || !Json_Value(text, "\"steamId\"")) {
        pl->status = text[0] && SDL_strcasestr(text, "not found") ? PRO_MISSING : PRO_FAILED;
        free(text);
        return;
    }
    Json_Cut(text, "\"clan\"", sub, sizeof(sub));
    pl->clan = sub[0] != '\0';
    if (pl->clan) {
        Json_Text(Json_Value(sub, "\"name\""), pl->clanName, sizeof(pl->clanName));
        Json_Text(Json_Value(sub, "\"avatarUrl\""), value, sizeof(value));
        Site_Url(pl->flag, sizeof(pl->flag), value);
    }
    Json_Cut(text, "\"title\"", sub, sizeof(sub));
    pl->titleColor = COL_WHITE;
    if (sub[0]) {
        Json_Text(Json_Value(sub, "\"title\""), pl->title, sizeof(pl->title));
        Json_Text(Json_Value(sub, "\"titleColor\""), value, sizeof(value));
        pl->titleColor = Color_Hex(value, COL_WHITE);
    }
    Json_Cut(text, "\"profileExtras\"", sub, sizeof(sub));
    if (sub[0]) {
        Json_Text(Json_Value(sub, "\"bio\""), pl->bio, sizeof(pl->bio));
    }
    p = Json_Array(text, "\"ratings\"", &end);
    while ((p = Json_Item(p, end, item, sizeof(item))) != NULL) {
        Json_Text(Json_Value(item, "\"gameType\""), value, sizeof(value));
        m = Mode_Index(value);
        if (m < 0) {
            continue;
        }
        pl->rating[m] = Json_Int(item, "\"rating\"", &have);
        pl->rated[m] = have;
        pl->games[m] = Json_Int(item, "\"games\"", NULL);
        pl->wins[m] = Json_Int(item, "\"wins\"", NULL);
        pl->losses[m] = Json_Int(item, "\"losses\"", NULL);
        pl->draws[m] = Json_Int(item, "\"draws\"", NULL);
    }
    Json_Text(Json_Value(text, "\"coloredName\""), pl->raw, sizeof(pl->raw));
    if (!pl->raw[0]) {
        Json_Text(Json_Value(text, "\"username\""), pl->raw, sizeof(pl->raw));
    }
    Json_Text(Json_Value(text, "\"avatar\""), value, sizeof(value));
    Site_Url(pl->avatar, sizeof(pl->avatar), value);
    Json_Text(Json_Value(text, "\"banner\""), value, sizeof(value));
    Site_Url(pl->banner, sizeof(pl->banner), value);
    Country_Code(text, pl->country);
    if (!pl->country[0]) {
        SDL_strlcpy(pl->country, "CL", sizeof(pl->country));
    }
    Json_Text(Json_Value(text, "\"createdAt\""), value, sizeof(value));
    if (Iso_Local(value, &dt) && dt.month >= 1 && dt.month <= 12) {
        SDL_snprintf(pl->since, sizeof(pl->since), "%s de %d", hubMonths[dt.month - 1], dt.year);
    }
    pl->registered = !Json_Value(text, "\"isRegistered\":false");
    m = Json_Int(text, "\"profileHueEfectivo\"", &have);
    if (!have) {
        m = Json_Int(text, "\"profileHue\"", &have);
    }
    pl->hue = have ? m : -1;
    SDL_snprintf(url, sizeof(url), SITE "/api/ioql/identidad?steamIds=%s", key);
    if (Http_Get(url, text, HUB_TEXT)) {
        Identity_Line(text, key, pl->avatar, sizeof(pl->avatar), pl->clan ? pl->flag : NULL, sizeof(pl->flag));
    }
    pl->status = PRO_READY;
    free(text);
}

static void Hub_LoadStats(const char* key, void* out) {
    hubStats_t* st = out;
    char id[24], mode[8], season[8], url[256], sub[8192], item[4096], value[64];
    char* text = malloc(HUB_TEXT);
    const char *p, *end;
    bool have;
    int i, n = 0;

    st->status = PRO_FAILED;
    if (!text || SDL_sscanf(key, "%23[^|]|%7[^|]|%7s", id, mode, season) != 3) {
        free(text);
        return;
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/rankings/player/%s?gameType=%s&season=%s", id, mode, season);
    if (Http_Get(url, text, HUB_TEXT)) {
        st->rank = Json_Int(text, "\"rank\"", &st->haveRank);
        st->total = Json_Int(text, "\"totalPlayers\"", NULL);
        st->minGames = Json_Int(text, "\"minGames\"", &have);
        if (!have) {
            st->minGames = 5;
        }
        st->left = Json_Int(text, "\"gamesRemaining\"", NULL);
        st->played = Json_Int(text, "\"totalGames\"", NULL);
        st->closed = Json_True(text, "\"seasonCerrada\"");
        st->elo = Json_Int(text, "\"elo\"", &st->haveRating);
        st->haveRating = st->haveRating && st->closed;
        st->wins = Json_Int(text, "\"wins\"", NULL);
        st->losses = Json_Int(text, "\"losses\"", NULL);
        st->draws = Json_Int(text, "\"draws\"", NULL);
        Json_Text(Json_Value(text, "\"motivo\""), value, sizeof(value));
        st->why = !SDL_strcmp(value, "posicionamiento") ? WHY_PLACEMENT
                  : !SDL_strcmp(value, "inactivo")      ? WHY_INACTIVE
                  : !SDL_strcmp(value, "baneado")       ? WHY_BANNED
                                                        : WHY_NONE;
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/stats/%s?gameType=%s&season=%s", id, mode, season);
    if (!Http_Get(url, text, HUB_TEXT)) {
        free(text);
        return;
    }
    if (Json_True(text, "\"success\"")) {
        Json_Cut(text, "\"player\"", sub, sizeof(sub));
        Json_Text(Json_Value(sub, "\"favoriteMap\""), st->favMap, sizeof(st->favMap));
        if (!SDL_strcmp(st->favMap, "N/A")) {
            st->favMap[0] = '\0';
        }
        Json_Cut(text, "\"overallStats\"", sub, sizeof(sub));
        st->kills = Json_Int(sub, "\"totalKills\"", NULL);
        st->deaths = Json_Int(sub, "\"totalDeaths\"", NULL);
        st->lastAlive = Json_Int(sub, "\"ultimoEnPie\"", &st->haveClutch);
        st->clutches = Json_Int(sub, "\"clutches\"", NULL);
        Json_Cut(text, "\"weaponStats\"", sub, sizeof(sub));
        p = sub[0] ? sub + 1 : NULL;
        while (p && *p) {
            const char* q = SDL_strchr(p, '"');
            const char* e;
            char name[8];
            int k = 0;

            if (!q) {
                break;
            }
            q++;
            while (*q && *q != '"' && k < (int)sizeof(name) - 1) {
                name[k++] = *q++;
            }
            name[k] = '\0';
            q = SDL_strchr(q, '{');
            if (!q || (e = Json_End(q)) == NULL) {
                break;
            }
            for (i = 0; i < 9; i++) {
                if (!SDL_strcmp(name, hubWeaponKey[i]) && st->weaponCount < 9) {
                    size_t len = SDL_min((size_t)(e - q + 1), sizeof(item) - 1);

                    SDL_memcpy(item, q, len);
                    item[len] = '\0';
                    st->weapons[i].acc = Json_Number(item, "\"accuracy\"", &have);
                    st->weapons[i].shots = Json_Int(item, "\"totalShots\"", NULL);
                    st->weapons[i].used = true;
                    st->weaponOrder[st->weaponCount++] = i;
                }
            }
            p = e + 1;
        }
        Json_Cut(text, "\"medals\"", sub, sizeof(sub));
        for (i = 0; sub[0] && i < 16; i++) {
            char k[24];

            SDL_snprintf(k, sizeof(k), "\"%s\"", hubMedalKey[i]);
            st->medal[i] = Json_Int(sub, k, NULL);
            if (st->medal[i] > 0) {
                int j = st->medalCount++;

                while (j > 0 && st->medal[st->medalOrder[j - 1]] < st->medal[i]) {
                    st->medalOrder[j] = st->medalOrder[j - 1];
                    j--;
                }
                st->medalOrder[j] = i;
            }
        }
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/players/%s/matches?gameType=%s&season=%s", id, mode, season);
    if (Http_Get(url, text, HUB_TEXT) && Json_True(text, "\"success\"")) {
        p = Json_Array(text, "\"matches\"", &end);
        while (st->gameCount < HUB_RECENT && (p = Json_Item(p, end, item, sizeof(item))) != NULL) {
            Hub_Game(item, &st->games[st->gameCount++]);
        }
    }
    if (SDL_strcmp(season, "s1")) {
        SDL_snprintf(url, sizeof(url), SITE "/api/players/%s/elo-history?gameType=%s&all=true&season=%s", id, mode, season);
        if (Http_Get(url, text, HUB_TEXT)) {
            p = Json_Array(text, "\"history\"", &end);
            while ((p = Json_Item(p, end, item, sizeof(item))) != NULL) {
                if (n == HUB_ELO_MAX) {
                    SDL_memmove(st->eloValue, st->eloValue + 1, sizeof(st->eloValue) - sizeof(st->eloValue[0]));
                    SDL_memmove(st->eloChange, st->eloChange + 1, sizeof(st->eloChange) - sizeof(st->eloChange[0]));
                    SDL_memmove(st->eloDate, st->eloDate + 1, sizeof(st->eloDate) - sizeof(st->eloDate[0]));
                    n--;
                }
                st->eloValue[n] = (short)Json_Int(item, "\"eloAfter\"", NULL);
                st->eloChange[n] = (short)Json_Int(item, "\"change\"", NULL);
                Json_Text(Json_Value(item, "\"date\""), st->eloDate[n], sizeof(st->eloDate[n]));
                n++;
            }
            st->eloCount = n;
        }
    }
    st->status = PRO_READY;
    free(text);
}

static void Hub_LoadGames(const char* key, void* out) {
    hubGames_t* gs = out;
    char id[24], mode[8], url[256], item[4096];
    char* text = malloc(HUB_TEXT);
    const char *p, *end;
    int page;

    gs->status = PRO_FAILED;
    if (!text || SDL_sscanf(key, "%23[^|]|%7[^|]|%d", id, mode, &page) != 3) {
        free(text);
        return;
    }
    if (SDL_strcmp(mode, "todo")) {
        SDL_snprintf(url, sizeof(url), SITE "/api/players/%s/matches?page=%d&gameType=%s", id, page, mode);
    } else {
        SDL_snprintf(url, sizeof(url), SITE "/api/players/%s/matches?page=%d", id, page);
    }
    if (Http_Get(url, text, HUB_TEXT) && Json_Value(text, "\"success\"")) {
        p = Json_Array(text, "\"matches\"", &end);
        while (gs->count < HUB_PAGE && (p = Json_Item(p, end, item, sizeof(item))) != NULL) {
            Hub_Game(item, &gs->games[gs->count++]);
        }
        gs->total = Json_Int(text, "\"total\"", NULL);
        gs->pages = Json_Int(text, "\"totalPages\"", NULL);
        gs->page = page;
        gs->status = PRO_READY;
    }
    free(text);
}

static void Hub_Identities(char* text, int size, int count, char (*ids)[24], char (*avatars)[200], char (*flags)[200], const bool* clans,
                           size_t stride) {
    char url[2048];
    size_t len = SDL_strlcpy(url, SITE "/api/ioql/identidad?steamIds=", sizeof(url));
    int i, n = 0;

    for (i = 0; i < count; i++) {
        const char* id = (const char*)ids + stride * (size_t)i;

        if (id[0] && len + 20 < sizeof(url)) {
            len += (size_t)SDL_snprintf(url + len, sizeof(url) - len, "%s%s", n ? "," : "", id);
            n++;
        }
    }
    if (!n || !Http_Get(url, text, size)) {
        return;
    }
    for (i = 0; i < count; i++) {
        const char* id = (const char*)ids + stride * (size_t)i;
        char* avatar = (char*)avatars + stride * (size_t)i;
        char* flag = (char*)flags + stride * (size_t)i;
        const bool* clan = (const bool*)((const char*)clans + stride * (size_t)i);

        if (id[0]) {
            Identity_Line(text, id, avatar, 200, *clan ? flag : NULL, 200);
        }
    }
}

static void Hub_Faces(char* text, int size, hubMatch_t* mt) {
    char body[2048], key[40], value[300];
    size_t len = SDL_strlcpy(body, "{\"steamIds\":[", sizeof(body));
    int i, n = 0;

    for (i = 0; i < mt->count; i++) {
        if (Steam_IdValid(mt->men[i].steamId) && len + 24 < sizeof(body)) {
            len += (size_t)SDL_snprintf(body + len, sizeof(body) - len, "%s\"%s\"", n ? "," : "", mt->men[i].steamId);
            n++;
        }
    }
    SDL_strlcat(body, "]}", sizeof(body));
    if (!n || !Http_Post(SITE "/api/players/batch", body, text, size)) {
        return;
    }
    for (i = 0; i < mt->count; i++) {
        const char* p;

        if (!Steam_IdValid(mt->men[i].steamId)) {
            continue;
        }
        SDL_snprintf(key, sizeof(key), "\"%s\":{", mt->men[i].steamId);
        p = SDL_strstr(text, key);
        if (!p) {
            continue;
        }
        Json_Text(Json_Value(p + SDL_strlen(key), "\"avatar\""), value, sizeof(value));
        if (value[0] == '/' || !SDL_strncmp(value, "https://", 8)) {
            Site_Url(mt->men[i].avatar, sizeof(mt->men[i].avatar), value);
        }
    }
}

static void Hub_LoadMatch(const char* key, void* out) {
    hubMatch_t* mt = out;
    char url[256], sub[16384], item[16384], part[4096], value[160], server[128], status[32];
    char* text = malloc(HUB_TEXT);
    const char *p, *end;
    SDL_DateTime dt;
    bool have;
    int i;

    mt->status = PRO_FAILED;
    if (!text) {
        return;
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/matches/%s", key);
    if (!Http_Get(url, text, HUB_TEXT) || !Json_True(text, "\"success\"")) {
        mt->status = text[0] && SDL_strcasestr(text, "not found") ? PRO_MISSING : PRO_FAILED;
        free(text);
        return;
    }
    Json_Cut(text, "\"match\"", sub, sizeof(sub));
    Json_Text(Json_Value(sub, "\"map\""), mt->map, sizeof(mt->map));
    Json_Text(Json_Value(sub, "\"gameType\""), mt->mode, sizeof(mt->mode));
    Json_Text(Json_Value(sub, "\"serverName\""), server, sizeof(server));
    mt->red = Json_Int(sub, "\"team1Score\"", NULL);
    mt->blue = Json_Int(sub, "\"team2Score\"", NULL);
    mt->winner = Json_Int(sub, "\"winner\"", &have);
    if (!have) {
        mt->winner = -1;
    }
    mt->aborted = Json_True(sub, "\"isAborted\"");
    Json_Text(Json_Value(sub, "\"gameStatus\""), status, sizeof(status));
    mt->counts = !Json_Value(sub, "\"isRated\":false") && !mt->aborted && (!status[0] || !SDL_strcmp(status, "SUCCESS"));
    SDL_strlcpy(mt->why, mt->aborted ? "aborted" : !SDL_strcmp(status, "TOO_FEW_PLAYERS") ? "few" : !SDL_strcmp(status, "TOO_SHORT") ? "short" : "rated",
                sizeof(mt->why));
    Json_Text(Json_Value(sub, "\"playedAt\""), value, sizeof(value));
    {
        const char* name = mt->map;
        char clock[16] = "";

        for (i = 0; i < (int)SDL_arraysize(hubMaps); i++) {
            if (!SDL_strcasecmp(mt->map, hubMaps[i][0])) {
                name = hubMaps[i][1];
            }
        }
        if (Iso_Local(value, &dt)) {
            SDL_snprintf(clock, sizeof(clock), "%02d:%02d", dt.hour, dt.minute);
        }
        SDL_snprintf(mt->title, sizeof(mt->title), "%s%s%s%s%s", server, server[0] && name[0] ? " | " : "", name, clock[0] ? " | " : "", clock);
    }
    mt->teams = SDL_strstr(text, "\"team1\":[") && SDL_strstr(text, "\"team2\":[");
    p = Json_Array(text, "\"all\"", &end);
    while (mt->count < HUB_MEN && (p = Json_Item(p, end, item, sizeof(item))) != NULL) {
        hubMan_t* man = &mt->men[mt->count++];
        const char *w, *wend;
        int hits = 0, shots = 0, sum = 0;

        Json_Cut(item, "\"clan\"", part, sizeof(part));
        man->clan = part[0] != '\0';
        if (man->clan) {
            Json_Text(Json_Value(part, "\"tag\""), man->clanTag, sizeof(man->clanTag));
            Json_Text(Json_Value(part, "\"avatarUrl\""), value, sizeof(value));
            Site_Url(man->clanPic, sizeof(man->clanPic), value);
        }
        Json_Cut(item, "\"medals\"", part, sizeof(part));
        for (i = 0; part[0] && i < 16; i++) {
            char k[24];

            SDL_snprintf(k, sizeof(k), "\"%s\"", hubMedalKey[i]);
            man->medal[i] = Json_Int(part, k, NULL);
        }
        w = Json_Array(item, "\"weapons\"", &wend);
        while ((w = Json_Item(w, wend, part, sizeof(part))) != NULL) {
            Json_Text(Json_Value(part, "\"weapon\""), value, sizeof(value));
            for (i = 0; i < 9; i++) {
                if (!SDL_strcmp(value, hubWeaponKey[i])) {
                    man->weapons[i].hits = Json_Int(part, "\"hits\"", NULL);
                    man->weapons[i].shots = Json_Int(part, "\"shots\"", NULL);
                    man->weapons[i].damage = Json_Int(part, "\"damage\"", NULL);
                    man->weapons[i].acc = Json_Number(part, "\"accuracy\"", &have);
                    man->weapons[i].used = man->weapons[i].shots > 0;
                    hits += man->weapons[i].hits;
                    shots += man->weapons[i].shots;
                    sum += man->weapons[i].damage;
                }
            }
        }
        Json_Text(Json_Value(item, "\"steamId\""), man->steamId, sizeof(man->steamId));
        Json_Text(Json_Value(item, "\"playerName\""), man->raw, sizeof(man->raw));
        man->team = Json_Int(item, "\"team\"", NULL);
        man->kills = Json_Int(item, "\"kills\"", NULL);
        man->deaths = Json_Int(item, "\"deaths\"", NULL);
        man->score = Json_Int(item, "\"score\"", NULL);
        man->damage = sum ? sum : Json_Int(item, "\"damageDealt\"", NULL);
        man->aim = shots ? (int)SDL_floorf((float)hits * 100.0f / (float)shots + 0.5f) : 0;
        man->perf = Json_Number(item, "\"performance\"", &man->havePerf);
        man->eloChange = Json_Int(item, "\"eloChange\"", NULL);
        man->elo = Json_Int(item, "\"eloAfter\"", &have);
        if (!have || !man->elo) {
            man->elo = Json_Int(item, "\"eloBefore\"", NULL);
        }
        man->flags[0] = Json_Int(item, "\"flagsCaptured\"", NULL);
        man->flags[1] = Json_Int(item, "\"flagsReturned\"", NULL);
        man->flags[2] = Json_Int(item, "\"flagPicks\"", NULL);
        man->flags[3] = Json_Int(item, "\"flagDrops\"", NULL);
        man->flags[4] = Json_Int(item, "\"carrierTakedowns\"", NULL);
        man->switched = SDL_strstr(item, "switchedTeam\\\":true") != NULL;
        Country_Code(item, man->country);
    }
    Hub_Faces(text, HUB_TEXT, mt);
    Hub_Identities(text, HUB_TEXT, mt->count, &mt->men[0].steamId, &mt->men[0].avatar, &mt->men[0].clanPic, &mt->men[0].clan, sizeof(hubMan_t));
    mt->status = PRO_READY;
    free(text);
}

static void Hub_LoadRanking(const char* key, void* out) {
    hubRanking_t* rk = out;
    char mode[8], url[256], item[4096], value[200], raw[96];
    char* text = malloc(HUB_TEXT);
    const char *p, *end;
    bool have;
    int page;

    rk->status = PRO_FAILED;
    if (!text || SDL_sscanf(key, "%7[^|]|%d", mode, &page) != 2) {
        free(text);
        return;
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/rankings?gameType=%s&perPage=%d&offset=%d", mode, HUB_RANK_PAGE, (page - 1) * HUB_RANK_PAGE);
    if (!Http_Get(url, text, HUB_TEXT) || !Json_True(text, "\"success\"")) {
        free(text);
        return;
    }
    p = Json_Array(text, "\"rankings\"", &end);
    while (rk->count < HUB_RANK_PAGE && (p = Json_Item(p, end, item, sizeof(item))) != NULL) {
        hubRank_t* r = &rk->rows[rk->count++];

        Json_Text(Json_Value(item, "\"steamId\""), r->steamId, sizeof(r->steamId));
        Json_Text(Json_Value(item, "\"username\""), r->raw, sizeof(r->raw));
        Json_Text(Json_Value(item, "\"avatar\""), value, sizeof(value));
        Site_Url(r->avatar, sizeof(r->avatar), value);
        r->rating = Json_Int(item, "\"rating\"", NULL);
        r->games = Json_Int(item, "\"totalGames\"", NULL);
        r->winRate = Json_Int(item, "\"winRate\"", &r->haveWin);
        r->rank = Json_Int(item, "\"rank\"", NULL);
        r->inactive = Json_True(item, "\"isInactive\"");
        r->kd = Json_Number(item, "\"avgKD\"", &have);
        r->lastAlive = Json_Int(item, "\"ultimoEnPie\"", &r->haveLast);
        r->clutches = Json_Int(item, "\"clutches\"", &r->haveClutch);
        Country_Code(item, r->country);
        Json_Text(Json_Value(item, "\"clanTag\""), raw, sizeof(raw));
        Server_Name(raw, r->clanTag, sizeof(r->clanTag));
        r->clan = r->clanTag[0] != '\0';
        Json_Text(Json_Value(item, "\"clanAvatarUrl\""), value, sizeof(value));
        Site_Url(r->clanPic, sizeof(r->clanPic), value);
    }
    rk->total = Json_Int(text, "\"totalCount\"", NULL);
    Hub_Identities(text, HUB_TEXT, rk->count, &rk->rows[0].steamId, &rk->rows[0].avatar, &rk->rows[0].clanPic, &rk->rows[0].clan, sizeof(hubRank_t));
    rk->status = PRO_READY;
    free(text);
}

static int SDLCALL Slot_Thread(void* data) {
    slot_t* s = data;

    SDL_memset(s->next, 0, s->size);
    s->load(s->busy, s->next);
    SDL_SetAtomicInt(&s->state, 2);
    Wake();
    return 0;
}

static void Slot_Poll(slot_t* s) {
    if (SDL_GetAtomicInt(&s->state) == 2) {
        SDL_WaitThread(s->thread, NULL);
        s->thread = NULL;
        SDL_memcpy(s->shown, s->next, s->size);
        SDL_strlcpy(s->have, s->busy, sizeof(s->have));
        SDL_SetAtomicInt(&s->state, 0);
    }
    if (SDL_GetAtomicInt(&s->state) == 0 && s->want[0] && SDL_strcmp(s->want, s->have)) {
        SDL_strlcpy(s->busy, s->want, sizeof(s->busy));
        SDL_SetAtomicInt(&s->state, 1);
        s->thread = SDL_CreateThread(Slot_Thread, s->name, s);
        if (!s->thread) {
            SDL_SetAtomicInt(&s->state, 0);
            SDL_strlcpy(s->have, s->want, sizeof(s->have));
            *(int*)s->shown = PRO_FAILED;
        }
    }
}

static bool Slot_Ready(const slot_t* s) {
    return s->want[0] && !SDL_strcmp(s->want, s->have);
}

static bool Slot_Shows(const slot_t* s) {
    return s->have[0] && *(const int*)s->shown == PRO_READY;
}

static void Slot_Clear(slot_t* s) {
    s->want[0] = '\0';
    s->have[0] = '\0';
    SDL_memset(s->shown, 0, s->size);
}

static bool Hub_Busy(void) {
    int i;

    for (i = 0; i < (int)SDL_arraysize(hubSlots); i++) {
        if (SDL_GetAtomicInt(&hubSlots[i]->state) != 0 || (hubSlots[i]->want[0] && !Slot_Ready(hubSlots[i]))) {
            return true;
        }
    }
    return false;
}

static void Hub_Want(void) {
    int i;

    for (i = 0; i < (int)SDL_arraysize(hubSlots); i++) {
        hubSlots[i]->want[0] = '\0';
    }
    if (!hubOpen) {
        return;
    }
    if (hubWho[0]) {
        SDL_strlcpy(slotPlayer.want, hubWho, sizeof(slotPlayer.want));
        if (hubTab == HUB_PROFILE) {
            SDL_snprintf(slotStats.want, sizeof(slotStats.want), "%s|%s|%s", hubWho, hubModeId[hubMode], hubSeasonId[hubSeason]);
        }
        if (hubTab == HUB_MATCHES && !hubMatchId[0]) {
            SDL_snprintf(slotGames.want, sizeof(slotGames.want), "%s|%s|%d", hubWho, hubGamesMode ? hubModeId[hubGamesMode - 1] : "todo", hubPage);
        }
    }
    if (hubTab == HUB_MATCHES && hubMatchId[0]) {
        SDL_strlcpy(slotMatch.want, hubMatchId, sizeof(slotMatch.want));
    }
    if (hubTab == HUB_RANKING) {
        SDL_snprintf(slotRank.want, sizeof(slotRank.want), "%s|%d", hubModeId[hubRankModes[hubRankMode]], hubRankPage);
    }
}

static void Hub_Poll(void) {
    int i;

    Hub_Want();
    for (i = 0; i < (int)SDL_arraysize(hubSlots); i++) {
        Slot_Poll(hubSlots[i]);
    }
}

static void Hub_Open(int tab, const char* steamId) {
    int i;

    if (install.active || setup.step != STEP_NONE || !Hub_FontsReady()) {
        return;
    }
    if (steamId && Steam_IdValid(steamId) && SDL_strcmp(steamId, hubWho)) {
        SDL_strlcpy(hubWho, steamId, sizeof(hubWho));
        hubMode = 0;
        hubSeason = HUB_SEASONS - 1;
        hubGamesMode = 0;
        hubPage = 1;
        hubMatchId[0] = '\0';
        hubGoal[HUB_PROFILE] = hubView[HUB_PROFILE] = 0.0f;
        hubGoal[HUB_MATCHES] = hubView[HUB_MATCHES] = 0.0f;
        Slot_Clear(&slotPlayer);
        Slot_Clear(&slotStats);
        Slot_Clear(&slotGames);
    }
    if (!hubWho[0] && meProfile.status == PRO_READY) {
        SDL_strlcpy(hubWho, meProfile.steamId, sizeof(hubWho));
    }
    for (i = 0; i < (int)SDL_arraysize(hubSlots); i++) {
        if (Slot_Ready(hubSlots[i]) && *(int*)hubSlots[i]->shown == PRO_FAILED) {
            hubSlots[i]->have[0] = '\0';
        }
    }
    if (tab == HUB_MATCHES && hubTab != HUB_MATCHES) {
        hubMatchId[0] = '\0';
    }
    hubTab = tab;
    hubOpen = true;
    boxOpen = false;
    optsOpen = false;
    if (tab == HUB_SERVERS && SDL_GetTicks() - serversWhen > 10000) {
        Servers_Fetch();
    }
}

static void Hub_Match(const char* id) {
    if (!id[0]) {
        return;
    }
    SDL_strlcpy(hubMatchId, id, sizeof(hubMatchId));
    hubSub = SUB_SUMMARY;
    hubGoal[4] = hubView[4] = 0.0f;
    hubTab = HUB_MATCHES;
}

static void Hub_Stop(void) {
    int i;

    for (i = 0; i < (int)SDL_arraysize(hubSlots); i++) {
        if (hubSlots[i]->thread) {
            SDL_DetachThread(hubSlots[i]->thread);
            hubSlots[i]->thread = NULL;
        }
    }
    if (hubFontThread) {
        SDL_WaitThread(hubFontThread, NULL);
        hubFontThread = NULL;
    }
}

static int SDLCALL Live_Thread(void* unused) {
    char url[192], seg[2048], raw[96];
    const char* colon = strrchr(liveNextAddr, ':');
    const char* p;
    const char* end;
    int n = 0, i, j;

    (void)unused;
    if (colon) {
        SDL_snprintf(url, sizeof(url), LIVE_API "%.*s%%3A%s", (int)(colon - liveNextAddr), liveNextAddr, colon + 1);
    }
#ifdef LAUNCHER_SHOT
    if (SDL_getenv("LAUNCHER_LIVE")) {
        size_t size = 0;
        char* file = SDL_LoadFile(SDL_getenv("LAUNCHER_LIVE"), &size);

        if (file) {
            SDL_strlcpy(liveText, file, sizeof(liveText));
            SDL_free(file);
            colon = NULL;
        }
    }
    if (colon && !Http_Get(url, liveText, sizeof(liveText))) {
#else
    if (!colon || !Http_Get(url, liveText, sizeof(liveText))) {
#endif
        SDL_SetAtomicInt(&liveState, 3);
        Wake();
        return 0;
    }
    Json_Text(Json_Value(liveText, "\"gameState\""), liveNextGame, sizeof(liveNextGame));
    p = Json_Value(liveText, "\"team1Score\"");
    liveNextScore[0] = p ? SDL_atoi(p) : -1;
    p = Json_Value(liveText, "\"team2Score\"");
    liveNextScore[1] = p ? SDL_atoi(p) : -1;
    p = Json_Value(liveText, "\"players\"");
    if (p && *p == '[') {
        for (p++;;) {
            player_t* pl = &liveNext[n];
            const char* q;
            size_t len;

            while (*p == ' ' || *p == ',' || *p == '\r' || *p == '\n' || *p == '\t') {
                p++;
            }
            if (*p != '{' || n >= LIVE_MAX || !(end = Json_End(p))) {
                break;
            }
            len = SDL_min((size_t)(end - p + 1), sizeof(seg) - 1);
            SDL_memcpy(seg, p, len);
            seg[len] = '\0';
            Live_Extras(pl, seg);
            Json_Text(Json_Value(seg, "\"username\""), raw, sizeof(raw));
            if (!raw[0]) {
                Json_Text(Json_Value(seg, "\"name\""), raw, sizeof(raw));
            }
            SDL_strlcpy(pl->raw, raw, sizeof(pl->raw));
            Name_Plain(raw, pl->name, sizeof(pl->name));
            Json_Text(Json_Value(seg, "\"steamId\""), pl->steamId, sizeof(pl->steamId));
            if (!Steam_IdValid(pl->steamId)) {
                pl->steamId[0] = '\0';
            }
            q = Json_Value(seg, "\"team\"");
            pl->team = q ? SDL_atoi(q) : 0;
            q = Json_Value(seg, "\"score\"");
            pl->score = q ? SDL_atoi(q) : 0;
            if (pl->name[0]) {
                n++;
            }
            p = end + 1;
        }
    }
    for (i = 1; i < n; i++) {
        player_t key = liveNext[i];

        for (j = i - 1; j >= 0 && liveNext[j].score < key.score; j--) {
            liveNext[j + 1] = liveNext[j];
        }
        liveNext[j + 1] = key;
    }
    Live_Identity(liveNext, n);
    liveNextCount = n;
    SDL_SetAtomicInt(&liveState, 2);
    Wake();
    return 0;
}

static void Live_Fetch(void) {
    if (SDL_GetAtomicInt(&liveState) != 0) {
        return;
    }
    if (liveThread) {
        SDL_WaitThread(liveThread, NULL);
        liveThread = NULL;
    }
    SDL_strlcpy(liveNextAddr, boxServer.addr, sizeof(liveNextAddr));
    SDL_SetAtomicInt(&liveState, 1);
    liveWhen = SDL_GetTicks();
    liveThread = SDL_CreateThread(Live_Thread, "live", NULL);
    if (!liveThread) {
        SDL_SetAtomicInt(&liveState, 3);
    }
}

static void Live_Poll(void) {
    int state = SDL_GetAtomicInt(&liveState);
    bool same = !SDL_strcmp(liveNextAddr, boxServer.addr);

    if (state == 2 || state == 3) {
        if (same && state == 2) {
            SDL_memcpy(livePlayers, liveNext, sizeof(livePlayers));
            liveCount = liveNextCount;
            liveScore[0] = liveNextScore[0];
            liveScore[1] = liveNextScore[1];
            SDL_strlcpy(liveGame, liveNextGame, sizeof(liveGame));
            liveLoaded = true;
            liveFailed = false;
        } else if (same) {
            liveFailed = !liveLoaded;
        } else {
            liveAgain = true;
        }
        SDL_SetAtomicInt(&liveState, 0);
    }
    if (boxOpen && SDL_GetAtomicInt(&liveState) == 0 && (liveAgain || SDL_GetTicks() - liveWhen > 5000)) {
        liveAgain = false;
        Live_Fetch();
    }
}

static int Live_State(const char* game) {
    if (!SDL_strcasecmp(game, "in_progress")) {
        return TXT_STATE_PLAYING;
    }
    if (!SDL_strcasecmp(game, "warmup")) {
        return TXT_STATE_WARMUP;
    }
    if (!SDL_strcasecmp(game, "countdown") || !SDL_strcasecmp(game, "count_down")) {
        return TXT_STATE_COUNTDOWN;
    }
    if (!SDL_strcasecmp(game, "pre_game")) {
        return TXT_STATE_PREGAME;
    }
    return TXT_STATE_WAITING;
}

static bool Mode_Teams(const char* mode) {
    static const char* const teams[] = {"ca", "ctf", "tdm", "ft", "ad", "dom"};
    int i;

    for (i = 0; i < (int)SDL_arraysize(teams); i++) {
        if (!SDL_strcasecmp(mode, teams[i])) {
            return true;
        }
    }
    return false;
}

static bool Shot_Name(const char* map) {
    int i;

    if (!map[0] || SDL_strlen(map) > 30) {
        return false;
    }
    for (i = 0; map[i]; i++) {
        if (!SDL_isalnum((unsigned char)map[i]) && map[i] != '_' && map[i] != '-') {
            return false;
        }
    }
    return true;
}

static const shot_t* Shot_Find(const char* map) {
    int i;

    for (i = 0; i < shotCount; i++) {
        if (!SDL_strcasecmp(shots[i].map, map)) {
            return &shots[i];
        }
    }
    return NULL;
}

static bool Shot_Pending(const char* map) {
    int i;

    if (SDL_GetAtomicInt(&shotsState) == 0) {
        return false;
    }
    for (i = 0; i < shotsJobCount; i++) {
        if (!SDL_strcasecmp(shotsJob[i], map)) {
            return true;
        }
    }
    return false;
}

typedef struct {
    unsigned char* out;
    size_t size, total;
} bytes_t;

static bool Bytes_Sink(void* ctx, const void* data, size_t n) {
    bytes_t* bytes = ctx;

    if (n > bytes->size - bytes->total) {
        return false;
    }
    SDL_memcpy(bytes->out + bytes->total, data, n);
    bytes->total += n;
    return true;
}

static bool Shot_Bytes(const char* map, const unsigned char* data, size_t size, int denom) {
    shotNew_t* n = &shotsNew[shotsNewCount];
    unsigned char* rgb;
    int w = 0, h = 0, i;

    if (shotsNewCount >= SHOT_MAX) {
        return false;
    }
    rgb = Launcher_DecodeJpeg(data, (unsigned long)size, &w, &h, denom);
    if (!rgb || w < 2 || h < 2) {
        free(rgb);
        return false;
    }
    n->w = w;
    n->h = h;
    n->rgba = malloc((size_t)w * (size_t)h * 4);
    for (i = 0; n->rgba && i < w * h; i++) {
        n->rgba[i * 4 + 0] = rgb[i * 3 + 0];
        n->rgba[i * 4 + 1] = rgb[i * 3 + 1];
        n->rgba[i * 4 + 2] = rgb[i * 3 + 2];
        n->rgba[i * 4 + 3] = 255;
    }
    if (n->rgba) {
        SDL_strlcpy(n->map, map, sizeof(n->map));
        shotsNewCount++;
    }
    free(rgb);
    return n->rgba != NULL;
}

static bool Shot_Decode(const char* pak, const char* map, const char* file, int denom) {
    unsigned char* data;
    size_t size = 0;
    bool ok;

    data = Pak_Read(pak, file, &size);
    if (!data) {
        return false;
    }
    ok = Shot_Bytes(map, data, size, denom);
    free(data);
    return ok;
}

static bool Shot_Web(const char* map, const char* file) {
    static const int widths[] = {640, 750, 828, 1080, 1200, 1920};
    char url[256], lower[64];
    bytes_t bytes;
    bool ok = false;
    int i, w = widths[0];

    for (i = 0; file[i] && i < (int)sizeof(lower) - 1; i++) {
        lower[i] = (char)SDL_tolower((unsigned char)file[i]);
    }
    lower[i] = '\0';
    for (i = 0; i < (int)SDL_arraysize(widths); i++) {
        w = widths[i];
        if ((float)w >= BOX_W * uiScale) {
            break;
        }
    }
    SDL_snprintf(url, sizeof(url), CLAN_IMAGE "%%2Flevelshots%%2F%s.jpg&w=%d&q=75", lower, w);
    bytes.out = malloc(PIC_BYTES);
    bytes.size = PIC_BYTES;
    bytes.total = 0;
    if (bytes.out && Http_Fetch(url, 5000, Bytes_Sink, &bytes) && bytes.total > 8) {
        ok = Shot_Bytes(map, bytes.out, bytes.total, 1);
    }
    free(bytes.out);
    return ok;
}

static int SDLCALL Shots_Thread(void* unused) {
    char pak[1100], file[64];
    int i;

    (void)unused;
    SDL_snprintf(pak, sizeof(pak), "%s" PAK_GAME, basePath);
    for (i = 0; i < shotsJobCount; i++) {
        if (shotsJob[i][0] == '*') {
            if (!Shot_Web(shotsJob[i], "default")) {
                Shot_Decode(pak, shotsJob[i], "levelshots/preview/default.jpg", 1);
            }
        } else {
            SDL_snprintf(file, sizeof(file), "levelshots/%s.jpg", shotsJob[i]);
            if (!Shot_Decode(pak, shotsJob[i], file, 4)) {
                Shot_Web(shotsJob[i], shotsJob[i]);
            }
        }
    }
    SDL_SetAtomicInt(&shotsState, 2);
    Wake();
    return 0;
}

static void Shots_Want(const char* map) {
    int i;

    for (i = 0; i < shotsTriedCount; i++) {
        if (!SDL_strcasecmp(shotsTried[i], map)) {
            return;
        }
    }
    if (shotsJobCount < SHOT_MAX && shotsTriedCount < SHOT_MAX) {
        SDL_strlcpy(shotsJob[shotsJobCount++], map, sizeof(shotsJob[0]));
        SDL_strlcpy(shotsTried[shotsTriedCount++], map, sizeof(shotsTried[0]));
    }
}

static void Shots_Poll(void) {
    int state = SDL_GetAtomicInt(&shotsState), i;

    if (state == 1) {
        return;
    }
    if (state == 2) {
        for (i = 0; i < 2 && shotsNewNext < shotsNewCount; i++) {
            shotNew_t* n = &shotsNew[shotsNewNext++];

            if (shotCount < SHOT_MAX) {
                shot_t* s = &shots[shotCount];
                size_t bytes = (size_t)n->w * (size_t)n->h * 4;
                unsigned char* copy = malloc(bytes);

                SDL_zerop(s);
                if (copy) {
                    SDL_memcpy(copy, n->rgba, bytes);
                    Image_Make(&s->mini, copy, n->w, n->h, THUMB_W);
                    free(copy);
                }
                Image_Make(&s->img, n->rgba, n->w, n->h, BOX_W);
                if (s->img.tex) {
                    SDL_strlcpy(s->map, n->map, sizeof(s->map));
                    shotCount++;
                }
            }
            free(n->rgba);
            n->rgba = NULL;
        }
        if (shotsNewNext < shotsNewCount) {
            return;
        }
        shotsNewCount = shotsNewNext = 0;
        SDL_SetAtomicInt(&shotsState, 0);
    }
    if (!gameFound) {
        return;
    }
    shotsJobCount = 0;
    Shots_Want("*default");
    for (i = 0; serversLoaded && i < (hubOpen && hubTab == HUB_SERVERS ? serverCount : SDL_min(serverCount, SIDE_ROWS)); i++) {
        if (Shot_Name(servers[i].map)) {
            Shots_Want(servers[i].map);
        }
    }
    Hub_Shots();
    if (boxOpen && Shot_Name(boxServer.map)) {
        Shots_Want(boxServer.map);
    }
    if (!shotsJobCount) {
        return;
    }
    if (shotsThread) {
        SDL_WaitThread(shotsThread, NULL);
        shotsThread = NULL;
    }
    SDL_SetAtomicInt(&shotsState, 1);
    shotsThread = SDL_CreateThread(Shots_Thread, "shots", NULL);
    if (!shotsThread) {
        SDL_SetAtomicInt(&shotsState, 0);
    }
}

static void Url_Escape(const char* in, char* out, size_t size) {
    static const char hex[] = "0123456789ABCDEF";
    size_t n = 0;

    for (; *in && n + 4 < size; in++) {
        unsigned char c = (unsigned char)*in;

        if (SDL_isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out[n++] = (char)c;
        } else {
            out[n++] = '%';
            out[n++] = hex[c >> 4];
            out[n++] = hex[c & 15];
        }
    }
    out[n] = '\0';
}


static void Profile_Load(profile_t* pr) {
    char url[256], sub[1024], value[200];
    char* text = malloc(PRO_TEXT);
    const char* p;
    float v;
    bool have, shots;
    int i;

    pr->status = PRO_FAILED;
    pr->kind = PRO_NONE;
    if (!text) {
        return;
    }
    SDL_snprintf(url, sizeof(url), SITE "/api/players/%s/hover-info", pr->steamId);
    if (!Http_Get(url, text, PRO_TEXT)) {
        free(text);
        return;
    }
    if (!Json_Value(text, "\"username\"")) {
        pr->status = SDL_strcasestr(text, "not found") ? PRO_MISSING : PRO_FAILED;
        free(text);
        return;
    }
    Json_Cut(text, "\"clan\"", sub, sizeof(sub));
    pr->clan = sub[0] != '\0';
    if (pr->clan) {
        Json_Text(Json_Value(sub, "\"tag\""), pr->clanTag, sizeof(pr->clanTag));
        Json_Text(Json_Value(sub, "\"name\""), pr->clanName, sizeof(pr->clanName));
    }
    Json_Cut(text, "\"title\"", sub, sizeof(sub));
    pr->titleColor = COL_W70;
    if (sub[0]) {
        Json_Text(Json_Value(sub, "\"name\""), pr->title, sizeof(pr->title));
        Json_Text(Json_Value(sub, "\"color\""), value, sizeof(value));
        pr->titleColor = Color_Hex(value, COL_W70);
    }
    Json_Text(Json_Value(text, "\"coloredName\""), pr->raw, sizeof(pr->raw));
    if (!pr->raw[0]) {
        Json_Text(Json_Value(text, "\"username\""), pr->raw, sizeof(pr->raw));
    }
    Json_Text(Json_Value(text, "\"avatar\""), value, sizeof(value));
    Site_Url(pr->avatar, sizeof(pr->avatar), value);
    Json_Text(Json_Value(text, "\"banner\""), pr->banner, sizeof(pr->banner));
    if (pr->banner[0] != '/' && SDL_strncmp(pr->banner, "https://", 8)) {
        pr->banner[0] = '\0';
    }
    Json_Text(Json_Value(text, "\"countryCode\""), value, sizeof(value));
    pr->country[0] = '\0';
    if (SDL_strlen(value) == 2 && SDL_isalpha((unsigned char)value[0]) && SDL_isalpha((unsigned char)value[1])) {
        for (i = 0; i < 2; i++) {
            pr->country[i] = (char)SDL_toupper((unsigned char)value[i]);
        }
        pr->country[2] = '\0';
    }
    pr->kd = Json_Number(text, "\"kd\"", &pr->haveKd);

    SDL_snprintf(url, sizeof(url), SITE "/api/ioql/identidad?steamIds=%s", pr->steamId);
    if (Http_Get(url, text, PRO_TEXT)) {
        Identity_Line(text, pr->steamId, pr->avatar, sizeof(pr->avatar), pr->flag, sizeof(pr->flag));
    }

    SDL_snprintf(url, sizeof(url), SITE "/api/rankings/player/%s?gameType=ca", pr->steamId);
    if (Http_Get(url, text, PRO_TEXT) && !SDL_strstr(text, "\"isBanned\":true")) {
        v = Json_Number(text, "\"elo\"", &have);
        pr->elo = (int)v;
        if (!have) {
            pr->kind = PRO_NONE;
        } else if (SDL_strstr(text, "\"isInactive\":true")) {
            pr->kind = PRO_INACTIVE;
        } else if (SDL_strstr(text, "\"isPlacement\":true")) {
            pr->kind = PRO_PLACEMENT;
            pr->left = (int)Json_Number(text, "\"gamesRemaining\"", &have);
        } else {
            v = Json_Number(text, "\"rank\"", &have);
            if (have) {
                pr->kind = PRO_RANKED;
                pr->rank = (int)v;
                pr->total = (int)Json_Number(text, "\"totalPlayers\"", &have);
            }
        }
    }

    SDL_snprintf(url, sizeof(url), SITE "/api/stats/%s?gameType=ca", pr->steamId);
    if (Http_Get(url, text, PRO_TEXT) && SDL_strstr(text, "\"success\":true")) {
        v = Json_Number(text, "\"averageKD\"", &have);
        if (have) {
            pr->kd = v;
            pr->haveKd = true;
        }
        p = SDL_strstr(text, "\"LG\":{");
        if (p) {
            shots = Json_Number(p, "\"totalShots\"", &have) > 0.0f && have;
            pr->lg = Json_Number(p, "\"accuracy\"", &have);
            pr->haveLg = shots && have;
        }
        p = SDL_strstr(text, "\"RG\":{");
        if (p) {
            shots = Json_Number(p, "\"totalShots\"", &have) > 0.0f && have;
            pr->rg = Json_Number(p, "\"accuracy\"", &have);
            pr->haveRg = shots && have;
        }
    }
    pr->status = PRO_READY;
    free(text);
}

static Uint64 Vdf_Number(const char* from, const char* limit, const char* key) {
    const char* k = SDL_strcasestr(from, key);

    if (!k || (limit && k > limit)) {
        return 0;
    }
    k += SDL_strlen(key);
    while (*k == ' ' || *k == '\t') {
        k++;
    }
    return *k == '"' ? SDL_strtoull(k + 1, NULL, 10) : 0;
}

static bool Me_Find(char* out, size_t size) {
    char root[1024], path[1100];
    char* text;
    const char* p;
    const char* best = NULL;
    Uint64 bestScore = 0;
    size_t n = 0;
#ifdef _WIN32
    DWORD account = 0, bytes = sizeof(account);

    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", L"ActiveUser", RRF_RT_REG_DWORD, NULL, &account, &bytes) ==
            ERROR_SUCCESS &&
        account) {
        SDL_snprintf(out, size, "%" SDL_PRIu64, (Uint64)76561197960265728ULL + account);
        if (Steam_IdValid(out)) {
            return true;
        }
        out[0] = '\0';
    }
#endif
    if (!Steam_Root(root, sizeof(root))) {
        return false;
    }
    SDL_snprintf(path, sizeof(path), "%s/config/loginusers.vdf", root);
    text = SDL_LoadFile(path, &n);
    if (!text) {
        return false;
    }
    for (p = text; (p = SDL_strstr(p, "\"7656")) != NULL;) {
        const char* end = SDL_strchr(p + 1, '"');
        const char* next;
        Uint64 score;

        if (!end || end - p - 1 != 17) {
            p++;
            continue;
        }
        next = SDL_strstr(end, "\"7656");
        score = 1 + Vdf_Number(end, next, "\"timestamp\"");
        if (Vdf_Number(end, next, "\"AutoLogin\"") == 1) {
            score += (Uint64)1 << 61;
        }
        if (Vdf_Number(end, next, "\"MostRecent\"") == 1) {
            score += (Uint64)1 << 62;
        }
        if (score > bestScore) {
            best = p + 1;
            bestScore = score;
        }
        p = end;
    }
    if (best) {
        SDL_strlcpy(out, best, SDL_min(size, (size_t)18));
    }
    SDL_free(text);
    return Steam_IdValid(out);
}

static int SDLCALL Me_Thread(void* unused) {
    (void)unused;
    if (Me_Find(meNext.steamId, sizeof(meNext.steamId))) {
        Profile_Load(&meNext);
    } else {
        meNext.status = PRO_MISSING;
    }
    SDL_SetAtomicInt(&meState, 2);
    Wake();
    return 0;
}

static void Me_Start(void) {
    SDL_zero(meNext);
    SDL_SetAtomicInt(&meState, 1);
    meThread = SDL_CreateThread(Me_Thread, "me", NULL);
    if (!meThread) {
        SDL_SetAtomicInt(&meState, 0);
    }
}

static void Profile_Poll(void) {
    if (SDL_GetAtomicInt(&meState) == 2) {
        SDL_WaitThread(meThread, NULL);
        meThread = NULL;
        meProfile = meNext;
        SDL_SetAtomicInt(&meState, 0);
    }
}
static void Pic_Fetch(int i) {
    char url[640], escaped[512], file[64];
    picNew_t* n = &picsNew[i];
    const char* key = picsJob[i];
    bytes_t bytes;

    SDL_strlcpy(n->key, key, sizeof(n->key));
    n->data = NULL;
    n->size = 0;
    if (key[0] == 'f') {
        SDL_snprintf(file, sizeof(file), "emoji/%x-%x.png", 0x1f1e6 + (key[2] - 'A'), 0x1f1e6 + (key[3] - 'A'));
        n->data = Pak_Read(PAK_OURS, file, &n->size);
        return;
    }
    if ((key[0] == 'c' || key[0] == 'B') && SDL_strncmp(key + 2, "https://", 8)) {
        Url_Escape(key + 2, escaped, sizeof(escaped));
        SDL_snprintf(url, sizeof(url), CLAN_IMAGE "%s&w=%d&q=75", escaped, key[0] == 'B' ? (HUB_CW * uiScale > 1200.0f ? 1920 : 1200) : 64);
    } else if (key[2] == '/') {
        SDL_snprintf(url, sizeof(url), SITE "%s", key + 2);
    } else {
        SDL_strlcpy(url, key + 2, sizeof(url));
    }
    bytes.out = malloc(PIC_BYTES);
    bytes.size = PIC_BYTES;
    bytes.total = 0;
    if (bytes.out && Http_Fetch(url, 5000, Bytes_Sink, &bytes) && bytes.total > 8) {
        n->data = bytes.out;
        n->size = bytes.total;
    } else {
        free(bytes.out);
    }
}

static int SDLCALL Pic_Worker(void* data) {
    int i;

    for (i = (int)(intptr_t)data; i < picsJobCount; i += PIC_WORKERS) {
        Pic_Fetch(i);
    }
    return 0;
}

static int SDLCALL Pics_Thread(void* unused) {
    SDL_Thread* workers[PIC_WORKERS];
    int i;

    (void)unused;
    for (i = 0; i < PIC_WORKERS; i++) {
        workers[i] = i < picsJobCount ? SDL_CreateThread(Pic_Worker, "pic", (void*)(intptr_t)i) : NULL;
    }
    for (i = 0; i < PIC_WORKERS; i++) {
        if (workers[i]) {
            SDL_WaitThread(workers[i], NULL);
        } else if (i < picsJobCount) {
            Pic_Worker((void*)(intptr_t)i);
        }
    }
    SDL_SetAtomicInt(&picsState, 2);
    Wake();
    return 0;
}

static pic_t* Pic_Find(const char* key) {
    int i;

    for (i = 0; i < picCount; i++) {
        if (!SDL_strcmp(pics[i].key, key)) {
            return &pics[i];
        }
    }
    return NULL;
}

static void Pic_Want(const char* kind, const char* value) {
    char key[sizeof(pics[0].key)];
    pic_t* pic;

    if (!value[0] || picsJobCount >= PIC_JOBS) {
        return;
    }
    SDL_snprintf(key, sizeof(key), "%s%s", kind, value);
    pic = Pic_Find(key);
    if (pic) {
        if (!pic->done && pic->again && SDL_GetTicks() >= pic->again) {
            pic->again = 0;
            SDL_strlcpy(picsJob[picsJobCount++], key, sizeof(picsJob[0]));
        }
        return;
    }
    if (picCount >= PIC_MAX) {
        return;
    }
    SDL_zero(pics[picCount]);
    SDL_strlcpy(pics[picCount].key, key, sizeof(pics[0].key));
    picCount++;
    SDL_strlcpy(picsJob[picsJobCount++], key, sizeof(picsJob[0]));
}

static void Pic_Decode(pic_t* pic, const unsigned char* data, size_t size, float target) {
    if (size > 8 && data[0] == 0x89 && data[1] == 'P' && data[2] == 'N' && data[3] == 'G') {
        Image_FromPng(&pic->img, data, (unsigned int)size, target);
    } else if (size > 3 && data[0] == 0xFF && data[1] == 0xD8) {
        int w = 0, h = 0, i;
        unsigned char* rgb = Launcher_DecodeJpeg(data, (unsigned long)size, &w, &h, 1);
        unsigned char* rgba = rgb && w > 0 && h > 0 ? malloc((size_t)w * (size_t)h * 4) : NULL;

        for (i = 0; rgba && i < w * h; i++) {
            rgba[i * 4 + 0] = rgb[i * 3 + 0];
            rgba[i * 4 + 1] = rgb[i * 3 + 1];
            rgba[i * 4 + 2] = rgb[i * 3 + 2];
            rgba[i * 4 + 3] = 255;
        }
        if (rgba) {
            Image_Make(&pic->img, rgba, w, h, target);
            free(rgba);
        }
        free(rgb);
    }
}

static void Pics_Poll(void) {
    int state = SDL_GetAtomicInt(&picsState), i;

    if (state == 1) {
        return;
    }
    if (state == 2) {
        for (i = 0; i < picsJobCount; i++) {
            picNew_t* n = &picsNew[i];
            pic_t* pic = Pic_Find(n->key);

            if (pic) {
                if (n->data) {
                    Pic_Decode(pic, n->data, n->size,
                               n->key[0] == 'f'   ? FLAG_SIDE
                               : n->key[0] == 'A' ? 104.0f
                               : n->key[0] == 'B' ? HUB_CW
                               : n->key[0] == 'i' ? 56.0f
                               : n->key[0] == 'c' ? 44.0f
                                                  : 28.0f);
                }
                if (pic->img.tex || n->data || ++pic->tries >= 3) {
                    pic->done = true;
                } else {
                    pic->again = SDL_GetTicks() + 3000;
                }
            }
            free(n->data);
            n->data = NULL;
        }
        picsJobCount = 0;
        SDL_SetAtomicInt(&picsState, 0);
    }
    picsJobCount = 0;
    for (i = 0; boxOpen && liveLoaded && i < liveCount; i++) {
        const player_t* pl = &livePlayers[i];

        Pic_Want("a:", pl->avatar);
        Pic_Want("f:", pl->country);
        Pic_Want("c:", pl->clan ? (pl->clanPic[0] ? pl->clanPic : CLAN_DEFAULT) : "");
    }
    if (meProfile.status == PRO_READY) {
        Pic_Want("a:", meProfile.avatar);
    }
    Hub_Pics();
    if (!picsJobCount) {
        return;
    }
    if (picsThread) {
        SDL_WaitThread(picsThread, NULL);
        picsThread = NULL;
    }
    SDL_SetAtomicInt(&picsState, 1);
    picsThread = SDL_CreateThread(Pics_Thread, "pics", NULL);
    if (!picsThread) {
        picsJobCount = 0;
        SDL_SetAtomicInt(&picsState, 0);
    }
}

static void Servers_Poll(void) {
    int state = SDL_GetAtomicInt(&serversState), i;

    if (state == 2) {
        SDL_memcpy(servers, serversNext, sizeof(servers));
        serverCount = serversNextCount;
        serversLoaded = true;
        serversFailed = false;
        for (i = 0; i < serverCount; i++) {
            if (boxOpen && !SDL_strcmp(servers[i].addr, boxServer.addr)) {
                boxServer = servers[i];
            }
        }
        SDL_SetAtomicInt(&serversState, 0);
    } else if (state == 3) {
        serversFailed = true;
        SDL_SetAtomicInt(&serversState, 0);
    }
    if (gameFound && !install.active && state != 1 && SDL_GetTicks() - serversWhen > (hubOpen && hubTab == HUB_SERVERS ? 20000u : 60000u)) {
        Servers_Fetch();
    }
    Profile_Poll();
    Hub_Poll();
    Live_Poll();
    Shots_Poll();
    Pics_Poll();
}

static void Install_Pick(void) {
    char path[1200];
    const char* leaf;
    size_t n;

    SDL_strlcpy(install.dir, pickPath, sizeof(install.dir) - 16);
    for (n = 0; install.dir[n]; n++) {
        if (install.dir[n] == '\\') {
            install.dir[n] = '/';
        }
    }
    while (n > 0 && install.dir[n - 1] == '/') {
        install.dir[--n] = '\0';
    }
    leaf = strrchr(install.dir, '/');
    leaf = leaf ? leaf + 1 : install.dir;
    SDL_snprintf(path, sizeof(path), "%s/" GAME_NAME, install.dir);
    if (SDL_strcasecmp(leaf, FOLDER_NAME) && !Path_IsFile(path)) {
        SDL_strlcat(install.dir, "/" FOLDER_NAME, sizeof(install.dir));
    }
    Install_Refresh();
}

static void Install_Init(void) {
    const char* home = SDL_GetUserFolder(SDL_FOLDER_HOME);
#ifdef LAUNCHER_SHOT
    const char* option;
#endif
#ifdef _WIN32
    char drive = 'C';
#endif

    install.active = true;
    install.shortcut = true;
    SDL_snprintf(install.zip, sizeof(install.zip), "%s" PACKAGE, basePath);
#ifdef _WIN32
    if (QL_Find() && setup.pak[1] == ':') {
        drive = setup.pak[0];
    } else if (home && home[1] == ':') {
        drive = home[0];
    }
    SDL_snprintf(install.dir, sizeof(install.dir), "%c:/Games/" FOLDER_NAME, drive);
#else
    SDL_snprintf(install.dir, sizeof(install.dir), "%sGames/" FOLDER_NAME, home ? home : "");
#endif
#ifdef LAUNCHER_SHOT
    option = SDL_getenv("LAUNCHER_ZIP");
    if (option && option[0]) {
        SDL_strlcpy(install.zip, option, sizeof(install.zip));
    }
    option = SDL_getenv("LAUNCHER_DIR");
    if (option && option[0]) {
        SDL_strlcpy(install.dir, option, sizeof(install.dir));
    }
#endif
    install.local = Path_IsFile(install.zip);
    Install_Refresh();
}

static void SDLCALL Pick_Done(void* userdata, const char* const* files, int filter) {
    (void)userdata;
    (void)filter;
    if (files && files[0] && !SDL_GetAtomicInt(&pickState)) {
        SDL_strlcpy(pickPath, files[0], sizeof(pickPath));
        SDL_SetAtomicInt(&pickState, 1);
        Wake();
    }
}

static void Pick_Open(void) {
    static const SDL_DialogFileFilter filters[] = {{"Quake Live pak00.pk3", "pk3"}};

    SDL_ShowOpenFileDialog(Pick_Done, NULL, window, filters, 1, NULL, false);
}

static void Folder_Open(void) {
    SDL_ShowOpenFolderDialog(Pick_Done, NULL, window, NULL, false);
}

static void Pick_Poll(void) {
    char* cut;

    if (!SDL_GetAtomicInt(&pickState)) {
        return;
    }
    if (install.active) {
        if (SDL_GetAtomicInt(&jobRun) != RUN_BUSY && !install.done) {
            Install_Pick();
        }
    } else if (setup.step == STEP_FILES && SDL_GetAtomicInt(&jobRun) != RUN_BUSY) {
        setup.failed = false;
        setup.wrongFile = !Pak_Valid(pickPath);
        if (!setup.wrongFile) {
            setup.picked = true;
            setup.found = true;
            SDL_strlcpy(setup.pak, pickPath, sizeof(setup.pak));
            for (cut = setup.pak; *cut; cut++) {
                if (*cut == '\\') {
                    *cut = '/';
                }
            }
            SDL_strlcpy(setup.ql, setup.pak, sizeof(setup.ql));
            cut = strrchr(setup.ql, '/');
            if (cut) {
                *cut = '\0';
            }
            cut = strrchr(setup.ql, '/');
            if (cut && !SDL_strcasecmp(cut + 1, "baseq3")) {
                *cut = '\0';
            } else {
                setup.ql[0] = '\0';
            }
            Job_Start(JOB_SCAN, 0);
        }
    }
    SDL_SetAtomicInt(&pickState, 0);
}

static bool Setup_Wants(int kind) {
    return setup.want[kind] && setup.count[kind] > 0;
}

static void Setup_Primary(void) {
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY) {
        return;
    }
    setup.failed = false;
    switch (setup.step) {
        case STEP_FILES:
            if (setup.found) {
                Job_Start(JOB_FILES, PAK_GAME_SIZE);
            } else {
                Pick_Open();
            }
            break;
        case STEP_SETTINGS:
            if (Setup_Wants(KIND_CONFIG) || Setup_Wants(KIND_HUD)) {
                Job_Start(JOB_SETTINGS, (Setup_Wants(KIND_CONFIG) ? setup.bytes[KIND_CONFIG] : 0) +
                                            (Setup_Wants(KIND_HUD) ? setup.bytes[KIND_HUD] : 0));
            } else {
                Setup_Next();
            }
            break;
        case STEP_DEMOS:
            if (setup.count[KIND_DEMOS]) {
                Job_Start(JOB_DEMOS, setup.bytes[KIND_DEMOS]);
            } else {
                Setup_Next();
            }
            break;
        case STEP_PREFS:
            Autostart_Set(prefStartup);
            Beta_Set(prefBeta);
            Setup_Next();
            break;
        default:
            break;
    }
}

static void Setup_Alt(void) {
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY) {
        return;
    }
    setup.failed = false;
    setup.wrongFile = false;
    if (setup.step != STEP_FILES) {
        Setup_Next();
    } else if (setup.found) {
        Pick_Open();
    } else {
        setup.picked = false;
        Job_Start(JOB_SCAN, 0);
    }
}

static SDL_Tray* tray;
static SDL_TrayEntry* trayOpen;
static SDL_TrayEntry* trayQuit;
static int trayAction;

static void SDLCALL Tray_Clicked(void* userdata, SDL_TrayEntry* entry) {
    SDL_Event wake;

    (void)entry;
    trayAction = (int)(intptr_t)userdata;
    SDL_zero(wake);
    wake.type = SDL_EVENT_USER;
    SDL_PushEvent(&wake);
}

static void Tray_Create(void) {
    SDL_TrayMenu* menu;
    SDL_Surface* icon = NULL;

#ifdef _WIN32
    SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "1");
#else
    SDL_IOStream* io = SDL_IOFromConstMem(launcherLogo, launcherLogoSize);

    icon = io ? SDL_LoadPNG_IO(io, true) : NULL;
#endif
    tray = SDL_CreateTray(icon, "ioquakelive");
    if (icon) {
        SDL_DestroySurface(icon);
    }
    if (!tray) {
        return;
    }
    menu = SDL_CreateTrayMenu(tray);
    if (!menu) {
        SDL_DestroyTray(tray);
        tray = NULL;
        return;
    }
    trayOpen = SDL_InsertTrayEntryAt(menu, -1, T(TXT_TRAY_OPEN), SDL_TRAYENTRY_BUTTON);
    trayQuit = SDL_InsertTrayEntryAt(menu, -1, T(TXT_TRAY_QUIT), SDL_TRAYENTRY_BUTTON);
    if (trayOpen) {
        SDL_SetTrayEntryCallback(trayOpen, Tray_Clicked, (void*)(intptr_t)1);
    }
    if (trayQuit) {
        SDL_SetTrayEntryCallback(trayQuit, Tray_Clicked, (void*)(intptr_t)2);
    }
}

static void Tray_Labels(void) {
    if (trayOpen) {
        SDL_SetTrayEntryLabel(trayOpen, T(TXT_TRAY_OPEN));
    }
    if (trayQuit) {
        SDL_SetTrayEntryLabel(trayQuit, T(TXT_TRAY_QUIT));
    }
}

static void Window_ToTray(void) {
    if (tray) {
        SDL_HideWindow(window);
    } else {
        SDL_MinimizeWindow(window);
    }
}

static void Window_FromTray(void) {
    SDL_ShowWindow(window);
    SDL_RestoreWindow(window);
    SDL_RaiseWindow(window);
}

static bool Vulkan_Swapchain(PFN_vkEnumerateDeviceExtensionProperties extensions, VkPhysicalDevice device) {
    VkExtensionProperties* list;
    uint32_t count = 0, i;
    bool found = false;

    if (extensions(device, NULL, &count, NULL) != VK_SUCCESS || !count) {
        return false;
    }
    list = SDL_malloc(count * sizeof(*list));
    if (list && extensions(device, NULL, &count, list) >= VK_SUCCESS) {
        for (i = 0; i < count && !found; i++) {
            found = !SDL_strcmp(list[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }
    }
    SDL_free(list);
    return found;
}

static bool Vulkan_Device(PFN_vkGetInstanceProcAddr proc, VkInstance instance) {
    PFN_vkEnumeratePhysicalDevices devices = (PFN_vkEnumeratePhysicalDevices)proc(instance, "vkEnumeratePhysicalDevices");
    PFN_vkGetPhysicalDeviceProperties properties = (PFN_vkGetPhysicalDeviceProperties)proc(instance, "vkGetPhysicalDeviceProperties");
    PFN_vkGetPhysicalDeviceQueueFamilyProperties families =
        (PFN_vkGetPhysicalDeviceQueueFamilyProperties)proc(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    PFN_vkEnumerateDeviceExtensionProperties extensions =
        (PFN_vkEnumerateDeviceExtensionProperties)proc(instance, "vkEnumerateDeviceExtensionProperties");
    VkPhysicalDevice list[16];
    uint32_t count = SDL_arraysize(list), i, j;

    if (!devices || !properties || !families || !extensions || devices(instance, &count, list) < VK_SUCCESS) {
        return false;
    }
    for (i = 0; i < count; i++) {
        VkPhysicalDeviceProperties props;
        VkQueueFamilyProperties queues[32];
        uint32_t queueCount = SDL_arraysize(queues);

        properties(list[i], &props);
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
            continue;
        }
        families(list[i], &queueCount, queues);
        for (j = 0; j < queueCount; j++) {
            if ((queues[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) && Vulkan_Swapchain(extensions, list[i])) {
                return true;
            }
        }
    }
    return false;
}

static int SDLCALL Vulkan_Thread(void* unused) {
    static const char* const names[] = {"vulkan-1.dll", "libvulkan.so.1", "libvulkan.so"};
    VkApplicationInfo app = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    VkInstanceCreateInfo info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    SDL_SharedObject* lib = NULL;
    PFN_vkGetInstanceProcAddr proc = NULL;
    PFN_vkCreateInstance create = NULL;
    VkInstance instance;
    bool usable = false;
    int i;

    (void)unused;
    for (i = 0; i < (int)SDL_arraysize(names) && !lib; i++) {
        lib = SDL_LoadObject(names[i]);
    }
    if (lib) {
        proc = (PFN_vkGetInstanceProcAddr)SDL_LoadFunction(lib, "vkGetInstanceProcAddr");
    }
    if (proc) {
        create = (PFN_vkCreateInstance)proc(VK_NULL_HANDLE, "vkCreateInstance");
    }
    app.apiVersion = VK_API_VERSION_1_0;
    info.pApplicationInfo = &app;
    if (create && create(&info, NULL, &instance) == VK_SUCCESS) {
        PFN_vkDestroyInstance destroy = (PFN_vkDestroyInstance)proc(instance, "vkDestroyInstance");

        usable = Vulkan_Device(proc, instance);
        if (destroy) {
            destroy(instance, NULL);
        }
    }
    if (lib) {
        SDL_UnloadObject(lib);
    }
    SDL_SetAtomicInt(&vulkanState, usable ? VULKAN_USABLE : VULKAN_MISSING);
    return 0;
}

static bool Game_Start(void) {
    char path[1024];
    const char* args[36];
    SDL_Process* process;
    bool vulkan = rendererHint >= 0 ? rendererHint == 1 : SDL_GetAtomicInt(&vulkanState) != VULKAN_MISSING;
    int i;

    SDL_snprintf(path, sizeof(path), "%s%s", basePath, (vulkan && vulkanFound) ? VULKAN_NAME : GAME_NAME);
    File_Exec(path);
    args[0] = path;
    for (i = 0; i < gameArgCount && i < 32; i++) {
        args[i + 1] = gameArgs[i];
    }
    if (connectTo[0]) {
        args[++i] = "+connect";
        args[++i] = connectTo;
    }
    args[i + 1] = NULL;
    SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "IOQUAKELIVE_LAUNCHER", "return", true);
    process = SDL_CreateProcess(args, false);
    if (!process) {
        return false;
    }
    SDL_DestroyProcess(process);
    return true;
}

static void Button_Set(int index, float x, float y, float w, float h, bool visible) {
    buttons[index].x = x;
    buttons[index].y = y;
    buttons[index].w = w;
    buttons[index].h = h;
    buttons[index].visible = visible;
}

static int Button_At(float x, float y) {
    int i;

    for (i = 0; i < BTN_COUNT; i++) {
        const button_t* b = &buttons[i];

        if (b->visible && x >= b->x && x < b->x + b->w && y >= b->y && y < b->y + b->h) {
            return i;
        }
    }
    return -1;
}

static void Buttons_Hide(int first, int last) {
    int i;

    for (i = first; i <= last; i++) {
        buttons[i].visible = false;
    }
}

static void Draw_Page(void) {
    Draw_Color(COL_P1);
    SDL_RenderClear(renderer);
}

static void Draw_Art(void) {
    const float* box = artBox[heroCustom ? 1 : 0];

    if (hero.tex) {
        Draw_Image(&hero, box[0], box[1], box[2], box[3], 1.0f);
        Draw_Image(&shade, 0.0f, 0.0f, UI_W, UI_H, 1.0f);
    }
}

static void Draw_Shadow(float x, float y, float w, float h, float radius) {
    int i;

    for (i = 1; i <= 6; i++) {
        float g = (float)i * 2.5f;

        Draw_Round(x - g, y - g + 3.0f, w + g * 2.0f, h + g * 2.0f, radius + g, COL_SHADE);
    }
}

static void Draw_Sheet(float x, float y, float w, float h) {
    Draw_Shadow(x, y, w, h, PANEL_R);
    Draw_Round(x - 1.0f, y - 1.0f, w + 2.0f, h + 2.0f, PANEL_R + 1.0f, COL_RING);
    Draw_Round(x, y, w, h, PANEL_R, COL_P2);
}

static void Draw_Panel(void) {
    float side = SIDE_X - 24.0f, h = PANEL_END - PANEL_Y;

    Draw_Sheet(COL_X, PANEL_Y, side - PANEL_GAP - COL_X, h);
    Draw_Sheet(side, PANEL_Y, COL_X + COL_W - side, h);
}

static float Caps_Width(int fontIndex, const char* text, float track) {
    float w;

    textTrack = track;
    w = Text_Width(fontIndex, Text_Upper(text), -1) - track;
    textTrack = 0.0f;
    return w;
}

static void Draw_Caps(int fontIndex, float x, float mid, Uint32 rgba, const char* text, float track) {
    textTrack = track;
    Draw_Text(fontIndex, x, Text_Mid(fontIndex, mid), rgba, Text_Upper(text), -1);
    textTrack = 0.0f;
}

static void Draw_Label(float x, float width, Uint32 rgba, const char* text) {
    textTrack = TRACK_HEAD;
    Draw_Text(FONT_PANEL, x, Text_Mid(FONT_PANEL, PANEL_Y + HEAD_H * 0.5f), rgba, Text_Fit(FONT_PANEL, Text_Upper(text), width), -1);
    textTrack = 0.0f;
}

static float Draw_Big(int index, const char* text, bool ready, float done, const char* work) {
    float x = SIDE_X, y = PLAY_Y, h = PLAY_H, mid = PLAY_Y + PLAY_H * 0.5f, w, tw;
    float hot = ready && !work ? heat[index] : 0.0f;

    tw = Caps_Width(FONT_PLAY, text, TRACK_BUTTON);
    w = SDL_max(SIDE_W, tw + 64.0f);
    Button_Set(index, x, y, w, h, ready && !work);
    if (work) {
        done = SDL_clamp(done, 0.0f, 1.0f);
        Draw_Round(x, y, w, h, BUTTON_R, COL_P3);
        Draw_Round(x + 18.0f, y + h - 13.0f, w - 36.0f, 4.0f, 2.0f, COL_P4);
        if ((w - 36.0f) * done >= 4.0f) {
            Draw_Round(x + 18.0f, y + h - 13.0f, (w - 36.0f) * done, 4.0f, 2.0f, COL_WHITE);
        }
        Draw_Caps(FONT_PLAY, x + (w - Caps_Width(FONT_PLAY, work, TRACK_BUTTON)) * 0.5f, mid - 5.0f, COL_WHITE, work, TRACK_BUTTON);
        return w;
    }
    Draw_Round(x, y, w, h, BUTTON_R, ready ? Col_Mix(COL_WHITE, COL_PURE, hot) : COL_P3);
    Draw_Caps(FONT_PLAY, x + (w - tw) * 0.5f, mid, ready ? COL_INK : COL_W70, text, TRACK_BUTTON);
    return w;
}

static float Draw_Title(const char* over, const char* title) {
    Draw_Label(IN_X, TEXT_W, COL_WHITE, title);
    if (over) {
        Draw_Text(FONT_BODY, IN_X, Text_Mid(FONT_BODY, BODY_TOP + 34.0f), COL_W70, over, -1);
        return BODY_TOP + 60.0f;
    }
    return BODY_TOP + 26.0f;
}

static const int navNames[NAV_ITEMS] = {TXT_NAV_HOME, TXT_HUB_SERVERS, TXT_HUB_RANKING, TXT_HUB_MATCHES, TXT_HUB_PROFILE};
static const int navTabs[NAV_ITEMS] = {-1, HUB_SERVERS, HUB_RANKING, HUB_MATCHES, HUB_PROFILE};

static bool Nav_Shown(void) {
    return gameFound && !install.active && setup.step == STEP_NONE;
}

static int Nav_Count(void) {
    return meProfile.status == PRO_READY ? NAV_ITEMS : NAV_ITEMS - 2;
}

static int Nav_Active(void) {
    int i;

    for (i = 1; hubOpen && i < NAV_ITEMS; i++) {
        if (navTabs[i] == hubTab) {
            return i;
        }
    }
    return 0;
}

static void Draw_Nav(void) {
    float x = NAV_X, hotX = -1.0f, hotW = 0.0f;
    bool live = Nav_Shown() && !boxOpen;
    int i, count = Nav_Count(), active = Nav_Active();

    Buttons_Hide(BTN_NAV_0, BTN_NAV_LAST);
    if (!Nav_Shown()) {
        navReady = false;
        return;
    }
    for (i = 0; i < count; i++) {
        const char* text = T(navNames[i]);
        float tw = Caps_Width(FONT_NAV, text, TRACK_NAV);
        Uint32 c = i == active ? COL_WHITE : Col_Mix(COL_W70, COL_WHITE, heat[BTN_NAV_0 + i]);

        Draw_Caps(FONT_NAV, x, TOP_MID, c, text, TRACK_NAV);
        Button_Set(BTN_NAV_0 + i, x - NAV_GAP * 0.5f, TOP_MID - 18.0f, tw + NAV_GAP, 36.0f, live);
        if (i == active || (hotButton == BTN_NAV_0 + i && live)) {
            if (hotButton == BTN_NAV_0 + i || hotX < 0.0f) {
                hotX = x;
                hotW = tw;
            }
        }
        x += tw + NAV_GAP;
    }
    navGoalX = hotX;
    navGoalW = hotW;
    if (!navReady) {
        navX = navGoalX;
        navW = navGoalW;
        navReady = true;
    }
    Draw_Round(navX, TOP_MID + 14.0f, navW, 2.0f, 1.0f, COL_GOLD);
}

static void Draw_Top(void) {
    const button_t* b;
    int i;

    Draw_Image(&logo, LOGO_X, TOP_MID - LOGO * 0.5f, LOGO, LOGO, 1.0f);
    Draw_Caps(FONT_BRAND, LOGO_X + LOGO + 14.0f, TOP_MID, COL_WHITE, "ioquakelive", TRACK_CAPS);
    Draw_Nav();

    Button_Set(BTN_OPTS, UI_W - CTRL_W * 3.0f, CTRL_Y, CTRL_W, CTRL_H, gameFound && !install.active && setup.step == STEP_NONE);
    Button_Set(BTN_MIN, UI_W - CTRL_W * 2.0f, CTRL_Y, CTRL_W, CTRL_H, true);
    Button_Set(BTN_CLOSE, UI_W - CTRL_W, CTRL_Y, CTRL_W, CTRL_H, true);
    for (i = BTN_OPTS; i <= BTN_CLOSE; i++) {
        float lit = i == BTN_OPTS && optsOpen ? 1.0f : heat[i];

        if (lit <= 0.0f || !buttons[i].visible) {
            continue;
        }
        b = &buttons[i];
        Draw_Round(b->x, b->y, b->w, b->h, ITEM_R, Col_Alpha(COL_HUSH, lit));
    }
    b = &buttons[BTN_OPTS];
    if (b->visible) {
        Draw_Spin(&iconGear, b->x + (b->w - GEAR_ICON) * 0.5f, b->y + (b->h - GEAR_ICON) * 0.5f, 0.0f, COL_WHITE);
    }
    b = &buttons[BTN_MIN];
    Draw_Rect(b->x + 16.0f, b->y + 18.0f, 12.0f, 1.0f, COL_WHITE);
    b = &buttons[BTN_CLOSE];
    Draw_Spin(&iconSlash, b->x + (b->w - CLOSE_ICON) * 0.5f, b->y + (b->h - CLOSE_ICON) * 0.5f, 0.0f, COL_WHITE);
    Draw_Spin(&iconSlash, b->x + (b->w - CLOSE_ICON) * 0.5f, b->y + (b->h - CLOSE_ICON) * 0.5f, 90.0f, COL_WHITE);
}

static void Draw_Link(int index, float x, float mid, const char* text) {
    float w = Text_Width(FONT_TEXT, text, -1);
    Uint32 color = Col_Mix(COL_WHITE, COL_PURE, heat[index]);

    Button_Set(index, x - 6.0f, mid - 16.0f, w + 12.0f, 32.0f, true);
    Draw_Text(FONT_TEXT, x, Text_Mid(FONT_TEXT, mid), color, text, -1);
    Draw_Rect(x, mid + 10.0f, w, 1.0f, color);
}

static void Draw_Feed(float alpha) {
    char line[96];
    float width = TEXT_W - 20.0f;
    SDL_Rect clip;

    notesTop = BODY_TOP + 20.0f;
    notesLow = PANEL_END - 16.0f;
    if (alpha <= 0.0f) {
        notesHeight = notesView = 0.0f;
        return;
    }
    SDL_snprintf(line, sizeof(line), T(TXT_WHATS_NEW), latestVersion);
    Draw_Label(IN_X, TEXT_W, Col_Alpha(COL_WHITE, alpha), line);
    notesDry = true;
    notesHeight = Draw_Notes(0.0f, 0.0f, width, 0.0f) - 8.0f;
    notesDry = false;
    notesView = SDL_min(notesHeight, notesLow - notesTop);
    clip.x = (int)(COL_X * uiScale);
    clip.y = (int)(notesTop * uiScale);
    clip.w = (int)((SIDE_X - 24.0f - COL_X) * uiScale);
    clip.h = (int)((notesLow - notesTop) * uiScale) + 1;
    SDL_SetRenderClipRect(renderer, &clip);
    Draw_Notes(IN_X, notesTop - notesScroll, width, alpha);
    SDL_SetRenderClipRect(renderer, NULL);
    if (notesHeight > notesView + 0.5f) {
        thumbH = notesView * notesView / notesHeight;
        thumbY = notesTop + (notesView - thumbH) * notesScroll / (notesHeight - notesView);
        Draw_Round(IN_X + TEXT_W - 4.0f, thumbY, 4.0f, thumbH, 2.0f, Col_Alpha(dragMode == 2 ? COL_W70 : COL_DOT, alpha));
    }
}

static void Draw_Row(float x, float mid, Uint32 dot, Uint32 rgba, const char* text, float width) {
    Draw_Round(x, mid - 4.0f, 8.0f, 8.0f, 4.0f, dot);
    Draw_Text(FONT_TEXT, x + 18.0f, Text_Mid(FONT_TEXT, mid), rgba, Text_Fit(FONT_TEXT, text, width - 18.0f), -1);
}

static Uint32 Col_Gray(float level, float alpha) {
    Uint32 v = (Uint32)(SDL_clamp(level, 0.0f, 1.0f) * 255.0f + 0.5f);

    return (v << 24) | (v << 16) | (v << 8) | (Uint32)(SDL_clamp(alpha, 0.0f, 1.0f) * 255.0f + 0.5f);
}

static int Draw_Clamp(int fontIndex, float x, float top, float first, float rest, float lineH, int lines, Uint32 rgba, const char* text, bool draw) {
    const char* p = text;
    int line;

    for (line = 0; line < lines && *p; line++) {
        const char* end = p;
        float mid = top + lineH * ((float)line + 0.5f), width = line ? rest : first;

        for (;;) {
            const char* q = end;

            while (*q == ' ') {
                q++;
            }
            while (*q && *q != ' ') {
                q++;
            }
            if (q == end || (end != p && Text_Width(fontIndex, p, (int)(q - p)) > width)) {
                break;
            }
            end = q;
        }
        if (line == lines - 1 || Text_Width(fontIndex, p, (int)(end - p)) > width) {
            if (draw) {
                Draw_Text(fontIndex, x, Text_Mid(fontIndex, mid), rgba, Text_Fit(fontIndex, p, width), -1);
            }
            return line + 1;
        }
        if (draw) {
            Draw_Text(fontIndex, x, Text_Mid(fontIndex, mid), rgba, p, (int)(end - p));
        }
        p = end;
        while (*p == ' ') {
            p++;
        }
    }
    return line;
}

static void Draw_CapsFit(int fontIndex, float x, float mid, Uint32 rgba, const char* text, float track, float width) {
    const char* fit;

    textTrack = track;
    fit = Text_Fit(fontIndex, Text_Upper(text), width);
    Draw_Text(fontIndex, x, Text_Mid(fontIndex, mid), rgba, fit, -1);
    textTrack = 0.0f;
}

static void Draw_NameFit(int fontIndex, float x, float mid, Uint32 rgba, const char* raw, float track, float width) {
    nameRune_t r[NAME_RUNES + 3];
    int n, i;

    textTrack = track;
    n = Name_Runes(raw, r, NAME_RUNES);
    if (Runes_Width(fontIndex, r, n) > width) {
        float dots = Text_Width(fontIndex, "...", -1) + track;

        while (n > 0 && Runes_Width(fontIndex, r, n) + dots > width) {
            n--;
        }
        while (n > 0 && (r[n - 1].cp == ' ' || Rune_Mark(r[n - 1].cp))) {
            n--;
        }
        for (i = 0; i < 3; i++) {
            r[n].cp = '.';
            r[n].col = 0;
            n++;
        }
    }
    Draw_Runes(fontIndex, x, Text_Mid(fontIndex, mid), rgba, r, n);
    textTrack = 0.0f;
}

static void Draw_Thumb(const char* map, float x, float y, float w, float h) {
    const shot_t* shot = Shot_Find(map);
    const shot_t* fallback = Shot_Find("*default");

    Draw_Round(x, y, w, h, 4.0f, COL_P4);
    if (shot) {
        Draw_RoundImage(&shot->mini, x, y, w, h, 4.0f, Col_Gray(1.0f, shot->heat));
    } else if (fallback && !Shot_Pending(map)) {
        Draw_RoundImage(&fallback->mini, x, y, w, h, 4.0f, Col_Gray(1.0f, fallback->heat));
    }
}

static float Draw_Count(float right, float mid, const server_t* s, bool draw, bool compact) {
    char num[16], rest[32];
    int bold = FONT_SV_COUNT, thin = compact ? FONT_SV_COUNT : FONT_SV_OF;
    float nw, rw;

    SDL_snprintf(num, sizeof(num), "%d", s->players);
    SDL_snprintf(rest, sizeof(rest), " %s %d", T(TXT_OF), s->max);
    nw = Text_Width(bold, num, -1);
    rw = Text_Width(thin, rest, -1);
    if (draw) {
        Draw_Text(bold, right - rw - nw, Text_Mid(bold, mid), COL_WHITE, num, -1);
        Draw_Text(thin, right - rw, Text_Mid(thin, mid), COL_W70, rest, -1);
    }
    return nw + rw;
}

static void Side_Row(const server_t* s, float x, float y, float w, float hot) {
    char line[64], head[sizeof(s->name)];
    float tx = x + 12.0f + SIDE_THUMB_W + 12.0f, tw = x + w - 12.0f - tx, block, top;
    float first = tw - Draw_Count(0.0f, 0.0f, s, false, true) - 12.0f;
    const char *rest = NULL, *p = s->name;
    int lines;

    textTrack = 0.88f;
    if (Text_Width(FONT_SV_SIDE, s->name, -1) > first) {
        while ((p = SDL_strstr(p, " | ")) != NULL) {
            if (Text_Width(FONT_SV_SIDE, s->name, (int)(p - s->name)) <= first) {
                rest = p + 3;
                SDL_strlcpy(head, s->name, (size_t)(p - s->name) + 1);
            }
            p += 3;
        }
    }
    lines = rest ? 2 : Draw_Clamp(FONT_SV_SIDE, 0.0f, 0.0f, first, tw, SIDE_LINE, 2, 0, s->name, false);
    textTrack = 0.0f;
    Draw_Round(x, y, w, SIDE_ROW_H, BUTTON_R, Col_Mix(COL_P3, COL_P4, hot));
    Draw_Thumb(s->map, x + 12.0f, y + (SIDE_ROW_H - SIDE_THUMB_H) * 0.5f, SIDE_THUMB_W, SIDE_THUMB_H);
    block = SIDE_LINE * (float)lines + 19.0f;
    top = y + (SIDE_ROW_H - block) * 0.5f;
    textTrack = 0.88f;
    if (rest) {
        Draw_Clamp(FONT_SV_SIDE, tx, top, first, first, SIDE_LINE, 1, COL_WHITE, head, true);
        Draw_Clamp(FONT_SV_SIDE, tx, top + SIDE_LINE, tw, tw, SIDE_LINE, 1, COL_WHITE, rest, true);
    } else {
        Draw_Clamp(FONT_SV_SIDE, tx, top, first, tw, SIDE_LINE, 2, COL_WHITE, s->name, true);
    }
    textTrack = 0.0f;
    Draw_Count(x + w - 12.0f, top + SIDE_LINE * 0.5f, s, true, true);
    SDL_snprintf(line, sizeof(line), "%s - %s", s->mode, s->map);
    Draw_CapsFit(FONT_SV_META, tx, top + block - 9.0f, COL_W70, line, 1.2f, tw);
}

static void Draw_SideHead(void) {
    float mid = PANEL_Y + HEAD_H * 0.5f, x = SIDE_X, w = SIDE_W, sw = Text_Width(FONT_BODY, T(TXT_SEE_ALL), -1), hot = heat[BTN_SERVERS];
    Uint32 link = Col_Mix(COL_W70, COL_WHITE, hot);

    Draw_Label(x, w - sw - MORE_ICON - 24.0f, COL_WHITE, T(TXT_SERVERS_SIDE));
    if (!serversLoaded) {
        return;
    }
    Button_Set(BTN_SERVERS, x + w - sw - MORE_ICON - 12.0f, mid - 16.0f, sw + MORE_ICON + 12.0f, 32.0f, true);
    Draw_Text(FONT_BODY, x + w - sw - MORE_ICON - 4.0f, Text_Mid(FONT_BODY, mid), link, T(TXT_SEE_ALL), -1);
    Draw_Spin(&iconMore, x + w - MORE_ICON + 2.0f * hot, mid - MORE_ICON * 0.5f, 90.0f, link);
}

static void Draw_SideServers(float y) {
    float x = SIDE_X, w = SIDE_W;
    int i, n;

    n = (int)((PLAY_Y - 12.0f - y + SIDE_GAP) / (SIDE_ROW_H + SIDE_GAP));
    n = SDL_min(n, SDL_min(serverCount, SIDE_ROWS));
    for (i = 0; i < n; i++) {
        float ry = y + (float)i * (SIDE_ROW_H + SIDE_GAP);

        Button_Set(BTN_SIDE_0 + i, x, ry, w, SIDE_ROW_H, true);
        Side_Row(&servers[i], x, ry, w, heat[BTN_SIDE_0 + i]);
    }
}

static void Draw_Card(void) {
    char work[64];
    float x = SIDE_X, row = BODY_TOP + 32.0f;
    bool busy = Update_Busy(), ready = Play_Ready();

    if (gameFound) {
        Draw_SideHead();
    }

    if (busy) {
        if (SDL_GetAtomicInt(&jobPhase) == PHASE_DOWNLOAD) {
            SDL_snprintf(work, sizeof(work), T(TXT_DOWNLOADING), (int)(Job_Done() * 100.0f));
        } else {
            SDL_strlcpy(work, T(TXT_INSTALLING), sizeof(work));
        }
    }

    Button_Set(BTN_STEAM, 0.0f, 0.0f, 0.0f, 0.0f, false);
    if (!busy) {
        if (install.fail != FAIL_NONE) {
            Draw_Row(x, row, COL_MAL, COL_WHITE, T(TXT_FAIL_FOLDER + install.fail - 1), SIDE_W);
            row += 36.0f;
        }
        if (gameFound && steamMissing) {
            bool opening = steamOpening && SDL_GetTicks() - steamOpening < 20000;
            const char* off = T(opening ? TXT_STEAM_OPENING : TXT_STEAM_OFF);

            Draw_Row(x, row, COL_DOT, COL_W70, off, SIDE_W);
            if (!opening) {
                Draw_Link(BTN_STEAM, x + 18.0f + Text_Width(FONT_TEXT, off, -1) + 14.0f, row, T(TXT_STEAM_OPEN));
            }
            row += 36.0f;
        }
        if (gameFound && vulkanFound && SDL_GetAtomicInt(&vulkanState) == VULKAN_MISSING) {
            Draw_Round(x, row - 4.0f, 8.0f, 8.0f, 4.0f, COL_DOT);
            row = Draw_Para(FONT_TEXT, x + 18.0f, row - 12.0f, SIDE_W - 18.0f, 24.0f, COL_W70, T(TXT_NO_VULKAN)) + 24.0f;
        }
    }
    if (serversLoaded && gameFound) {
        Draw_SideServers(row - 18.0f);
    } else if (gameFound) {
        Draw_Text(FONT_SMALL, x, Text_Mid(FONT_SMALL, row), COL_W70, Text_Fit(FONT_SMALL, T(serversFailed ? TXT_SERVERS_FAILED : TXT_SERVERS_LOADING), SIDE_W), -1);
    }

    Draw_Big(BTN_PLAY, T(TXT_PLAY), ready, busy ? Job_Done() : 0.0f, busy ? work : NULL);
}

static void Draw_Status(void) {
    char line[96];
    const char* text = line;
    int state = SDL_GetAtomicInt(&checkState);
    float x = (buttons[BTN_ME].visible ? buttons[BTN_ME].x : UI_W - CTRL_W * 3.0f) - 14.0f;

    Button_Set(BTN_UPDATE, 0.0f, 0.0f, 0.0f, 0.0f, false);
    if (!gameFound || install.active || setup.step != STEP_NONE) {
        return;
    }
    if (Update_Busy()) {
        if (SDL_GetAtomicInt(&jobPhase) == PHASE_DOWNLOAD) {
            SDL_snprintf(line, sizeof(line), T(TXT_DOWNLOADING), (int)(Job_Done() * 100.0f));
        } else {
            text = T(TXT_INSTALLING);
        }
    } else if (state == CHECK_DONE && updateAvailable) {
        float w;

        SDL_snprintf(line, sizeof(line), T(TXT_PATCH_OUT), latestVersion);
        w = Text_Width(FONT_BODY, line, -1) + 28.0f;
        x -= w;
        Button_Set(BTN_UPDATE, x, TOP_MID - 15.0f, w, 30.0f, true);
        Draw_Round(x, TOP_MID - 15.0f, w, 30.0f, ITEM_R, Col_Mix(COL_P3, COL_P4, heat[BTN_UPDATE]));
        Draw_Text(FONT_BODY, x + 14.0f, Text_Mid(FONT_BODY, TOP_MID), COL_WHITE, line, -1);
        return;
    } else {
        text = T(state == CHECK_RUNNING ? TXT_CHECKING : state == CHECK_FAILED ? TXT_CHECK_FAILED : TXT_UPTODATE);
    }
    x -= Text_Width(FONT_BODY, text, -1);
    Draw_Text(FONT_BODY, x, Text_Mid(FONT_BODY, TOP_MID), COL_W70, text, -1);
}

static void Draw_Path(float x, float y, float width, Uint32 rgba, const char* path) {
    char shown[1024];
    const char* p = shown;
    float dots = Text_Width(FONT_BODY, "...", -1);
#ifdef _WIN32
    char* c;
#endif

    SDL_strlcpy(shown, path, sizeof(shown));
#ifdef _WIN32
    for (c = shown; *c; c++) {
        if (*c == '/') {
            *c = '\\';
        }
    }
#endif
    if (Text_Width(FONT_BODY, shown, -1) <= width) {
        Draw_Text(FONT_BODY, x, y, rgba, shown, -1);
        return;
    }
    while (*p && (Text_Width(FONT_BODY, p, -1) > width - dots || (*p != '\\' && *p != '/'))) {
        Text_Next(&p);
    }
    Draw_Text(FONT_BODY, x, y, rgba, "...", -1);
    Draw_Text(FONT_BODY, x + dots, y, rgba, p, -1);
}

static void Size_Text(Sint64 bytes, char* out, size_t size) {
    double mb = (double)bytes / 1048576.0;

    if (mb >= 1024.0) {
        SDL_snprintf(out, size, "%.1f GB", mb / 1024.0);
    } else if (mb >= 10.0) {
        SDL_snprintf(out, size, "%.0f MB", mb);
    } else {
        SDL_snprintf(out, size, "%.1f MB", mb);
    }
}

static void Draw_Check(int index, float x, float y, float w, bool on, const char* label, const char* detail) {
    Button_Set(index, x, y, w, 44.0f, true);
    if (heat[index] > 0.0f) {
        Draw_Round(x, y, w, 44.0f, ROW_R, Col_Alpha(COL_P3, heat[index]));
    }
    if (on) {
        Draw_Round(x + 12.0f, y + 11.0f, 22.0f, 22.0f, 6.0f, COL_WHITE);
        Draw_Spin(&iconCheck, x + 15.0f, y + 14.0f, 0.0f, COL_INK);
    } else {
        Draw_Round(x + 12.0f, y + 11.0f, 22.0f, 22.0f, 6.0f, COL_P1);
        Draw_RoundLine(x + 12.0f, y + 11.0f, 22.0f, 22.0f, 6.0f, COL_W40);
    }
    Draw_Text(FONT_HEAD, x + 48.0f, Text_Mid(FONT_HEAD, y + 22.0f), COL_WHITE, label, -1);
    Draw_Text(FONT_TEXT, x + 62.0f + Text_Width(FONT_HEAD, label, -1), Text_Mid(FONT_TEXT, y + 22.0f), COL_W70, detail, -1);
}

static bool Instance_Take(void) {
    Uint32 hash = 2166136261u;
    const char* p;
    char name[128];

    for (p = basePath; *p; p++) {
        hash = (hash ^ (Uint32)SDL_tolower((unsigned char)*p)) * 16777619u;
    }
#ifdef _WIN32
    {
        wchar_t wide[128];
        DWORD got;

        SDL_snprintf(name, sizeof(name), "Local\\ioquakelive-launcher-%08x", hash);
        if (!MultiByteToWideChar(CP_UTF8, 0, name, -1, wide, 128)) {
            return true;
        }
        instanceLock = CreateMutexW(NULL, FALSE, wide);
        if (!instanceLock) {
            return true;
        }
        got = WaitForSingleObject(instanceLock, waitLock ? 15000 : 0);
        if (got == WAIT_OBJECT_0 || got == WAIT_ABANDONED) {
            return true;
        }
        {
            HWND other = FindWindowW(NULL, L"ioquakelive Launcher");

            if (other) {
                if (IsIconic(other)) {
                    ShowWindow(other, SW_RESTORE);
                } else if (!IsWindowVisible(other)) {
                    ShowWindow(other, SW_SHOW);
                }
                SetForegroundWindow(other);
            }
        }
        return false;
    }
#else
    {
        int i;

        SDL_snprintf(name, sizeof(name), "/tmp/ioquakelive-launcher-%u-%08x.lock", (unsigned)getuid(), hash);
        instanceLock = open(name, O_RDWR | O_CREAT, 0600);
        if (instanceLock < 0) {
            return true;
        }
        for (i = 0;; i++) {
            if (flock(instanceLock, LOCK_EX | LOCK_NB) == 0) {
                return true;
            }
            if (!waitLock || i >= 300) {
                return false;
            }
            SDL_Delay(50);
        }
    }
#endif
}

static float Step_Ease(void) {
    float t = 1.0f - stepHeat;

    return 1.0f - t * t * t * t * t;
}

static void Slide_Begin(float x, float fade) {
    SDL_Rect clip;

    if (stepHeat >= 1.0f || !stepDir) {
        return;
    }
    slideX = x;
    slideFade = fade;
    clip.x = (int)(COL_X * uiScale);
    clip.y = (int)(PANEL_Y * uiScale);
    clip.w = (int)(COL_W * uiScale);
    clip.h = (int)((UI_H - PANEL_Y) * uiScale) + 1;
    SDL_SetRenderClipRect(renderer, &clip);
    slideClip = true;
}

static void Slide_End(void) {
    if (slideClip) {
        SDL_SetRenderClipRect(renderer, NULL);
        slideClip = false;
    }
    slideX = 0.0f;
    slideFade = 1.0f;
}

static bool Setup_Body(int step, bool scanning, const char** next, const char** alt, const char** note) {
    char line[256], size[32];
    float x = IN_X, w = TEXT_W, y;
    int i;

    *next = *alt = *note = NULL;
    SDL_snprintf(line, sizeof(line), T(TXT_SETUP_STEP), step, SETUP_STEPS);
    y = Draw_Title(line, T(TXT_TITLE_FILES + step - 1));
    if (scanning) {
        Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_SCANNING));
        return false;
    }
    switch (step) {
        case STEP_FILES:
            if (setup.found) {
                y = Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_FILES_FOUND));
                Draw_Path(x, y + 8.0f, w, COL_W70, setup.pak);
                *note = T(setup.canLink ? TXT_NO_SPACE : TXT_PAK_COPY);
                *next = T(TXT_INSTALL);
                *alt = T(TXT_CHOOSE_OTHER);
            } else {
                Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_FILES_MISSING));
                *next = T(TXT_FIND_PAK);
                *alt = T(TXT_SEARCH_AGAIN);
            }
            if (setup.wrongFile) {
                *note = T(TXT_WRONG_FILE);
            }
            if (setup.failed) {
                *note = T(TXT_PAK_FAILED);
            }
            break;
        case STEP_SETTINGS:
            if (!setup.count[KIND_CONFIG] && !setup.count[KIND_HUD]) {
                Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_NO_SETTINGS));
                *next = T(TXT_NEXT);
                break;
            }
            y = Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_SETTINGS_INTRO)) + 12.0f;
            for (i = KIND_CONFIG; i <= KIND_HUD; i++) {
                if (setup.count[i]) {
                    SDL_snprintf(line, sizeof(line), T(setup.count[i] == 1 ? TXT_FILE_ONE : TXT_FILE_MANY), setup.count[i]);
                    Draw_Check(BTN_OPT_A + i, x - 12.0f, y, w + 24.0f, setup.want[i], T(i == KIND_CONFIG ? TXT_CONFIG : TXT_HUD), line);
                    y += 48.0f;
                }
            }
            *note = T(setup.failed ? TXT_FILES_FAILED : TXT_UNCHANGED);
            if (Setup_Wants(KIND_CONFIG) || Setup_Wants(KIND_HUD)) {
                *next = T(TXT_IMPORT);
                *alt = T(TXT_SKIP);
            } else {
                *next = T(TXT_NEXT);
            }
            break;
        case STEP_PREFS:
            y = Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_PREFS_INTRO)) + 12.0f;
            Draw_Check(BTN_OPT_A, x - 12.0f, y, w + 24.0f, prefStartup, T(TXT_STARTUP), T(TXT_STARTUP_DETAIL));
            Draw_Check(BTN_OPT_B, x - 12.0f, y + 48.0f, w + 24.0f, prefBeta, T(TXT_BETA), T(TXT_BETA_DETAIL));
            *next = T(TXT_FINISH);
            break;
        default:
            if (!setup.count[KIND_DEMOS]) {
                Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_NO_DEMOS));
                *next = T(TXT_NEXT);
                break;
            }
            Size_Text(setup.bytes[KIND_DEMOS], size, sizeof(size));
            SDL_snprintf(line, sizeof(line), T(setup.count[KIND_DEMOS] == 1 ? TXT_DEMO_ONE : TXT_DEMO_MANY), setup.count[KIND_DEMOS], size);
            Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, line);
            *note = T(setup.failed ? TXT_DEMOS_FAILED : setup.canLink ? TXT_NO_SPACE : TXT_DEMOS_COPY);
            *next = T(TXT_IMPORT);
            *alt = T(TXT_SKIP);
            break;
    }
    return true;
}

static void Setup_Note(const char* note) {
    if (note) {
        Draw_Para(FONT_BODY, SIDE_X, BODY_TOP + 26.0f, SIDE_W, 25.0f, setup.failed || setup.wrongFile ? COL_WHITE : COL_W70, note);
    }
}

static void Draw_Setup(void) {
    char work[64];
    int run = SDL_GetAtomicInt(&jobRun);
    bool scanning = run == RUN_BUSY && jobKind == JOB_SCAN;
    bool working = run == RUN_BUSY && !scanning;
    float seg = (SIDE_W - 8.0f * (SETUP_STEPS - 1)) / SETUP_STEPS, ease = Step_Ease(), fade = SDL_min(1.0f, stepHeat * 5.0f);
    const char* next;
    const char* alt;
    const char* note;
    int i;

    Buttons_Hide(BTN_PLAY, BTN_OPT_B);
    for (i = 0; i < SETUP_STEPS; i++) {
        float bx = SIDE_X + (seg + 8.0f) * (float)i, by = PANEL_Y + HEAD_H * 0.5f - 2.0f, fill = i < setup.step ? 1.0f : 0.0f;

        if (stepHeat < 1.0f && i == (stepDir > 0 ? setup.step - 1 : setup.step)) {
            fill = stepDir > 0 ? ease : 1.0f - ease;
        }
        Draw_Round(bx, by, seg, 4.0f, 2.0f, COL_DOT);
        if (fill > 0.0f) {
            Draw_Round(bx, by, seg * fill, 4.0f, 2.0f, COL_WHITE);
        }
    }
    if (stepHeat < 1.0f && fade < 1.0f && stepDir && stepFrom != STEP_NONE && stepFrom != setup.step) {
        button_t keep[BTN_COUNT];

        SDL_memcpy(keep, buttons, sizeof(keep));
        Slide_Begin(-(float)stepDir * 100.0f * ease, 1.0f - fade);
        Setup_Body(stepFrom, false, &next, &alt, &note);
        Setup_Note(note);
        SDL_memcpy(buttons, keep, sizeof(keep));
    }
    Slide_Begin((float)stepDir * 100.0f * (1.0f - ease), fade);
    if (!Setup_Body(setup.step, scanning, &next, &alt, &note)) {
        Slide_End();
        return;
    }
    if (working) {
        Slide_End();
        SDL_snprintf(work, sizeof(work), T(TXT_WORKING), (int)(Job_Done() * 100.0f));
        Draw_Big(BTN_NEXT, next, false, Job_Done(), work);
        Buttons_Hide(BTN_NEXT, BTN_OPT_B);
        return;
    }
    Setup_Note(note);
    Slide_End();
    Draw_Big(BTN_NEXT, next, true, 0.0f, NULL);
    if (alt) {
        Draw_Link(BTN_ALT, SIDE_X, PLAY_Y - 30.0f, alt);
    }
    if (setup.step > STEP_FILES) {
        Draw_Link(BTN_BACK, IN_X, PLAY_Y + PLAY_H * 0.5f, T(TXT_BACK));
    }
}

static void Draw_Install(void) {
    char line[128];
    float x = IN_X, w = TEXT_W, y;
    const char* note;

    Buttons_Hide(BTN_PLAY, BTN_OPT_B);
    y = Draw_Title(NULL, T(TXT_INSTALL_TITLE));
    y = Draw_Para(FONT_TEXT, x, y, w, 28.0f, COL_W85, T(TXT_INSTALL_WHERE));
    Draw_Path(x, y + 6.0f, w, COL_WHITE, install.dir);
    y += 44.0f;
    if (SDL_GetAtomicInt(&jobRun) == RUN_BUSY || install.done) {
        float done = install.done ? 1.0f : Job_Done();

        if (!install.done && SDL_GetAtomicInt(&jobPhase) == PHASE_DOWNLOAD) {
            SDL_snprintf(line, sizeof(line), T(TXT_DOWNLOADING), (int)(done * 100.0f));
        } else {
            SDL_strlcpy(line, T(TXT_INSTALLING), sizeof(line));
        }
        Draw_Big(BTN_NEXT, T(TXT_INSTALL), false, done, line);
        return;
    }
    Draw_Link(BTN_ALT, x, y + 16.0f, T(TXT_CHANGE_FOLDER));
    Draw_Check(BTN_OPT_A, x - 12.0f, y + 46.0f, w + 24.0f, install.shortcut, T(TXT_SHORTCUT), "");
    note = install.fail != FAIL_NONE ? T(TXT_FAIL_FOLDER + install.fail - 1) : T(install.adopt ? TXT_ADOPT : install.local ? TXT_FROM_FILE : TXT_FROM_GITHUB);
    Draw_Para(FONT_BODY, SIDE_X, BODY_TOP + 26.0f, SIDE_W, 25.0f, install.fail != FAIL_NONE ? COL_WHITE : COL_W70, note);
    Draw_Big(BTN_NEXT, T(TXT_INSTALL), true, 0.0f, NULL);
}

static void Draw_Options(void) {
    static const int labels[OPTS_ROWS] = {TXT_OPEN_FOLDER, TXT_RUN_SETUP, TXT_REPAIR, TXT_CHECK_NOW, TXT_BETA, TXT_STARTUP};
    float open = optsHeat * optsHeat * (3.0f - 2.0f * optsHeat);
    float drop = (open - 1.0f) * 6.0f;
    int i;

    for (i = 0; i < OPTS_ROWS; i++) {
        float y = OPTS_Y + MENU_PAD + OPTS_ROW * (float)i + (i >= 4 ? OPTS_GAP : 0.0f);

        Button_Set(BTN_MENU_0 + i, OPTS_X + MENU_PAD, y, OPTS_W - MENU_PAD * 2.0f, OPTS_ROW, optsOpen);
    }
    if (open <= 0.0f) {
        return;
    }
    Draw_Round(OPTS_X, OPTS_Y + drop, OPTS_W, OPTS_H, BUTTON_R, Col_Alpha(COL_P3, open));
    Draw_Rect(OPTS_X + MENU_PAD + 10.0f, OPTS_Y + drop + MENU_PAD + OPTS_ROW * 4.0f + OPTS_GAP * 0.5f, OPTS_W - MENU_PAD * 2.0f - 20.0f, 1.0f,
              Col_Alpha(COL_LINE, open));
    for (i = 0; i < OPTS_ROWS; i++) {
        const button_t* b = &buttons[BTN_MENU_0 + i];
        float mid = b->y + drop + b->h * 0.5f, hot = heat[BTN_MENU_0 + i];
        bool on = i == 4 ? betaChannel : i == 5 ? startupOn : false;

        if (hot > 0.0f) {
            Draw_Round(b->x, b->y + drop, b->w, b->h, ITEM_R, Col_Alpha(COL_P4, open * hot));
        }
        Draw_Text(FONT_TEXT, b->x + 14.0f, Text_Mid(FONT_TEXT, mid), Col_Alpha(on ? COL_WHITE : Col_Mix(COL_W70, COL_WHITE, hot), open),
                  T(labels[i]), -1);
        if (on) {
            Draw_Spin(&iconCheck, b->x + b->w - 14.0f - CHECK_ICON, mid - CHECK_ICON * 0.5f, 0.0f, Col_Alpha(COL_WHITE, open));
        }
    }
}

static float Button_Width(const char* text) {
    return Caps_Width(FONT_BOXBTN, text, TRACK_BUTTON) + 40.0f;
}

static float Draw_Button(int index, float x, float y, const char* text, bool primary, bool ready) {
    float w = Button_Width(text), hot = ready ? heat[index] : 0.0f;
    Uint32 fill = primary ? (ready ? Col_Mix(COL_WHITE, COL_PURE, hot) : COL_P3) : Col_Mix(COL_P3, COL_P4, hot);

    Button_Set(index, x, y, w, BOX_BTN_H, ready);
    Draw_Round(x, y, w, BOX_BTN_H, BUTTON_R, fill);
    Draw_Caps(FONT_BOXBTN, x + 20.0f, y + BOX_BTN_H * 0.5f, primary && ready ? COL_INK : COL_W85, text, TRACK_BUTTON);
    return w;
}

static Uint32 Col_Hue(const char* name) {
    float c = (1.0f - SDL_fabsf(2.0f * 0.22f - 1.0f)) * 0.32f, m = 0.22f - c * 0.5f, h, x, r, g, b;
    unsigned hue = 0;
    int k;

    for (; *name; name++) {
        hue = (hue * 31u + (unsigned char)*name) % 360u;
    }
    h = (float)hue / 60.0f;
    x = c * (1.0f - SDL_fabsf(SDL_fmodf(h, 2.0f) - 1.0f));
    k = (int)h;
    r = k == 0 || k == 5 ? c : k == 1 || k == 4 ? x : 0.0f;
    g = k == 1 || k == 2 ? c : k == 0 || k == 3 ? x : 0.0f;
    b = k == 3 || k == 4 ? c : k == 2 || k == 5 ? x : 0.0f;
    return ((Uint32)((r + m) * 255.0f + 0.5f) << 24) | ((Uint32)((g + m) * 255.0f + 0.5f) << 16) | ((Uint32)((b + m) * 255.0f + 0.5f) << 8) | 0xFFu;
}

static float Player_Badges(const player_t* pl, float x, float mid) {
    char key[sizeof(pics[0].key)], initial[2] = {'?', '\0'};
    const pic_t* pic;
    const char* c;

    SDL_snprintf(key, sizeof(key), "a:%s", pl->avatar);
    pic = pl->avatar[0] ? Pic_Find(key) : NULL;
    if (pic && pic->img.tex) {
        Draw_RoundImage(&pic->img, x, mid - 12.0f, 24.0f, 24.0f, 6.0f, Col_Gray(1.0f, pic->heat));
    } else {
        for (c = pl->name; *c; c++) {
            if (SDL_isalnum((unsigned char)*c)) {
                initial[0] = (char)SDL_toupper((unsigned char)*c);
                break;
            }
        }
        Draw_Round(x, mid - 12.0f, 24.0f, 24.0f, 6.0f, Col_Hue(pl->name));
        Draw_Text(FONT_TINY, x + (24.0f - Text_Width(FONT_TINY, initial, -1)) * 0.5f, Text_Mid(FONT_TINY, mid), Col_Alpha(COL_WHITE, 0.7f), initial, -1);
    }
    x += 32.0f;
    if (pl->country[0]) {
        SDL_snprintf(key, sizeof(key), "f:%s", pl->country);
        pic = Pic_Find(key);
        if (pic && pic->img.tex) {
            Draw_Image(&pic->img, x, mid - FLAG_SIDE * 0.5f, FLAG_SIDE, FLAG_SIDE, pic->heat);
            x += FLAG_SIDE + 8.0f;
        }
    }
    if (pl->clan) {
        SDL_snprintf(key, sizeof(key), "c:%s", pl->clanPic[0] ? pl->clanPic : CLAN_DEFAULT);
        pic = Pic_Find(key);
        Draw_Round(x, mid - 7.0f, 28.0f, 14.0f, 4.0f, COL_P3);
        if (pic && pic->img.tex) {
            Draw_RoundImage(&pic->img, x, mid - 7.0f, 28.0f, 14.0f, 4.0f, Col_Gray(1.0f, pic->heat));
        } else if (pic && pic->done) {
            const char* tag = Text_Fit(FONT_TINY, Text_Upper(pl->clanTag), 24.0f);

            Draw_Text(FONT_TINY, x + (28.0f - Text_Width(FONT_TINY, tag, -1)) * 0.5f, Text_Mid(FONT_TINY, mid), COL_W85, tag, -1);
        }
        x += 36.0f;
    }
    return x;
}

static float Box_List(float x, float y, float w, int which, bool draw) {
    static const int titles[4] = {TXT_COL_PLAYERS, TXT_RED_TEAM, TXT_BLUE_TEAM, TXT_SPECTATORS};
    char num[16];
    float row = y + 30.0f;
    int i, n = 0;

    for (i = 0; i < liveCount; i++) {
        const player_t* pl = &livePlayers[i];

        if (which ? pl->team != which : pl->team == 3) {
            continue;
        }
        if (draw) {
            float mid = row + BOX_ROW * 0.5f, sw = 0.0f, nx, top = SDL_max(row, boxClipTop), bottom = SDL_min(row + BOX_ROW, boxClipBottom);
            bool can = pl->steamId[0] && boxOpen && bottom > top;

            Button_Set(BTN_PLR_0 + i, x, top, w, bottom - top, can);
            if (can && heat[BTN_PLR_0 + i] > 0.0f) {
                Draw_Round(x, row + 2.0f, w, BOX_ROW - 4.0f, 6.0f, Col_Alpha(COL_P3, heat[BTN_PLR_0 + i]));
            }
            if (which != 3) {
                SDL_snprintf(num, sizeof(num), "%d", pl->score);
                sw = Text_Width(FONT_SV_SCORE, num, -1) + 12.0f;
                Draw_Text(FONT_SV_SCORE, x + w - 6.0f - sw + 12.0f, Text_Mid(FONT_SV_SCORE, mid), COL_WHITE, num, -1);
            }
            nx = Player_Badges(pl, x + 6.0f, mid);
            Draw_NameFit(FONT_SV_NAME, nx, mid, which == 3 ? COL_W70 : COL_WHITE, pl->raw, 0.5f, x + w - 6.0f - sw - nx);
        }
        row += BOX_ROW;
        n++;
    }
    if (draw) {
        float mid = y + 12.0f, hx = x + 6.0f;

        if (which == 1 || which == 2) {
            Draw_Round(hx, mid - 5.0f, 10.0f, 10.0f, 2.0f, which == 1 ? COL_RED : COL_BLUE);
            hx += 18.0f;
        }
        Draw_Caps(FONT_SV_HEAD, hx, mid, COL_W70, T(titles[which]), 1.1f);
        SDL_snprintf(num, sizeof(num), "%d", n);
        Draw_Text(FONT_SV_HEAD, x + w - 6.0f - Text_Width(FONT_SV_HEAD, num, -1), Text_Mid(FONT_SV_HEAD, mid), COL_W70, num, -1);
    }
    return row;
}

static float Box_Body(float x, float y, float w, bool draw) {
    float top = y, half = (w - 24.0f) * 0.5f;
    int i, red = 0, blue = 0, spec = 0, play = 0;

    if (!liveLoaded && !liveFailed) {
        if (draw) {
            for (i = 0; i < 2; i++) {
                float cx = x + 6.0f + (float)i * (half + 24.0f);
                int k;

                Draw_Round(cx, y + 6.0f, 90.0f, 12.0f, 6.0f, COL_P3);
                for (k = 0; k < 4; k++) {
                    float ky = y + 30.0f + (float)k * BOX_ROW + BOX_ROW * 0.5f;

                    Draw_Round(cx, ky - 12.0f, 24.0f, 24.0f, 6.0f, COL_P3);
                    Draw_Round(cx + 32.0f, ky - 6.0f, 90.0f + (float)((k * 31) % 60), 12.0f, 6.0f, COL_P3);
                }
            }
        }
        return 30.0f + 4.0f * BOX_ROW;
    }
    if (liveFailed) {
        return 8.0f;
    }
    for (i = 0; i < liveCount; i++) {
        int team = livePlayers[i].team;

        red += team == 1;
        blue += team == 2;
        spec += team == 3;
        play += team != 3;
    }
    if (!play && !spec) {
        if (draw) {
            const char* none = T(TXT_NOBODY);

            Draw_Text(FONT_SMALL, x + (w - Text_Width(FONT_SMALL, none, -1)) * 0.5f, Text_Mid(FONT_SMALL, y + 30.0f), COL_W70, none, -1);
        }
        return 56.0f;
    }
    if (Mode_Teams(boxServer.mode) && (red || blue)) {
        y = SDL_max(Box_List(x, y, half, 1, draw), Box_List(x + half + 24.0f, y, half, 2, draw));
    } else if (play) {
        y = Box_List(x, y, w, 0, draw);
    }
    if (spec) {
        y = Box_List(x, y > top ? y + 18.0f : y, w, 3, draw);
    }
    return y - top;
}

static void Score_Side(float center, float mid, int team, int score) {
    char num[16];
    float lw = 16.0f + Caps_Width(FONT_SV_HEAD, T(team == 1 ? TXT_RED_TEAM : TXT_BLUE_TEAM), 0.92f), nw;

    SDL_snprintf(num, sizeof(num), "%d", score);
    nw = Text_Width(FONT_SCORE, num, -1);
    Draw_Round(center - lw * 0.5f, mid - 40.5f, 9.0f, 9.0f, 2.0f, team == 1 ? COL_RED : COL_BLUE);
    Draw_Caps(FONT_SV_HEAD, center - lw * 0.5f + 16.0f, mid - 36.0f, COL_W85, T(team == 1 ? TXT_RED_TEAM : TXT_BLUE_TEAM), 0.92f);
    Draw_Text(FONT_SCORE, center - nw * 0.5f, Text_Mid(FONT_SCORE, mid), COL_WHITE, num, -1);
}

static void Box_Score(float cx, float mid) {
    char num[16];
    float dw = Text_Width(FONT_SCORE, "-", -1), aw, bw;

    SDL_snprintf(num, sizeof(num), "%d", liveScore[0]);
    aw = SDL_max(Text_Width(FONT_SCORE, num, -1), 16.0f + Caps_Width(FONT_SV_HEAD, T(TXT_RED_TEAM), 0.92f));
    SDL_snprintf(num, sizeof(num), "%d", liveScore[1]);
    bw = SDL_max(Text_Width(FONT_SCORE, num, -1), 16.0f + Caps_Width(FONT_SV_HEAD, T(TXT_BLUE_TEAM), 0.92f));
    Score_Side(cx - dw * 0.5f - 22.0f - aw * 0.5f, mid, 1, liveScore[0]);
    Draw_Text(FONT_SCORE, cx - dw * 0.5f, Text_Mid(FONT_SCORE, mid), Col_Alpha(COL_WHITE, 0.5f), "-", -1);
    Score_Side(cx + dw * 0.5f + 22.0f + bw * 0.5f, mid, 2, liveScore[1]);
}

static void Draw_Box(void) {
    char line[160];
    const shot_t* shot = Shot_Find(boxServer.map);
    const shot_t* fallback = Shot_Find("*default");
    float e = boxHeat * boxHeat * (3.0f - 2.0f * boxHeat), content, bodyH, h, x, y, top, fy, cw, most;
    SDL_Rect clip;

    if (e <= 0.0f) {
        return;
    }
    Draw_Rect(0.0f, 0.0f, UI_W, UI_H, Col_Alpha(COL_BLACK, 0.6f * e));
    content = Box_Body(0.0f, 0.0f, BOX_W - 48.0f, false);
    bodyH = SDL_min(content, UI_H - 48.0f - COVER_H - 18.0f - BOX_FOOT);
    h = COVER_H + 18.0f + bodyH + BOX_FOOT;
    x = (UI_W - BOX_W) * 0.5f;
    y = (UI_H - h) * 0.5f + (1.0f - e) * 16.0f;
    slideFade = e;
    Draw_Round(x, y, BOX_W, h, BOX_R, COL_P2);
    Draw_Round(x, y, BOX_W, COVER_H + BOX_R, BOX_R, COL_P3);
    if (shot) {
        Draw_RoundImage(&shot->img, x, y, BOX_W, COVER_H + BOX_R, BOX_R, Col_Gray(0.45f, shot->heat));
    } else if (fallback && !Shot_Pending(boxServer.map)) {
        Draw_RoundImage(&fallback->img, x, y, BOX_W, COVER_H + BOX_R, BOX_R, Col_Gray(0.45f, fallback->heat));
    }
    Draw_Rect(x, y + COVER_H, BOX_W, BOX_R, COL_P2);
    if (liveLoaded) {
        SDL_snprintf(line, sizeof(line), "%s \xc2\xb7 %d %s %d", T(Live_State(liveGame)), boxServer.players, T(TXT_OF), boxServer.max);
    } else {
        SDL_snprintf(line, sizeof(line), "%d %s %d", boxServer.players, T(TXT_OF), boxServer.max);
    }
    Draw_Caps(FONT_SV_HEAD, x + 24.0f, y + 28.0f, COL_W85, line, 1.1f);
    if (liveLoaded && Mode_Teams(boxServer.mode) && Live_State(liveGame) == TXT_STATE_PLAYING && liveScore[0] >= 0 && liveScore[1] >= 0) {
        Box_Score(x + BOX_W * 0.5f, y + COVER_H * 0.5f);
    }
    Draw_CapsFit(FONT_BOXNAME, x + 24.0f, y + COVER_H - 46.0f, COL_WHITE, boxServer.name, TRACK_CAPS, BOX_W - 48.0f);
    SDL_snprintf(line, sizeof(line), "%s - %s", boxServer.mode, boxServer.map);
    Draw_CapsFit(FONT_SV_MAP, x + 24.0f, y + COVER_H - 22.0f, COL_W70, line, 0.72f, BOX_W - 48.0f);
    top = y + COVER_H + 18.0f;
    most = SDL_max(0.0f, content - bodyH);
    boxGoal = SDL_clamp(boxGoal, 0.0f, most);
    boxView = SDL_clamp(boxView, 0.0f, most);
    clip.x = (int)(x * uiScale);
    clip.y = (int)(top * uiScale);
    clip.w = (int)(BOX_W * uiScale);
    clip.h = (int)(bodyH * uiScale) + 1;
    SDL_SetRenderClipRect(renderer, &clip);
    boxClipTop = top;
    boxClipBottom = top + bodyH;
    Box_Body(x + 24.0f, top - boxView, BOX_W - 48.0f, true);
    SDL_SetRenderClipRect(renderer, NULL);
    if (most > 0.0f) {
        float thumb = SDL_max(30.0f, bodyH * bodyH / content);

        Draw_Round(x + BOX_W - 12.0f, top + (bodyH - thumb) * boxView / most, 3.0f, thumb, 1.5f, COL_DOT);
    }
    fy = y + h - 24.0f - BOX_BTN_H;
    cw = Draw_Button(BTN_CONNECT, x + 24.0f, fy, T(TXT_JOIN), true, boxOpen && Play_Ready());
    Draw_Button(BTN_SEE_SERVER, x + 24.0f + cw + 10.0f, fy, T(TXT_SEE_SERVER), false, boxOpen);
    Draw_Button(BTN_BOX_CLOSE, x + BOX_W - 24.0f - Button_Width(T(TXT_CLOSE)), fy, T(TXT_CLOSE), false, boxOpen);
    Button_Set(BTN_BOX, x, y, BOX_W, h, boxOpen);
    Button_Set(BTN_VEIL, 0.0f, 0.0f, UI_W, UI_H, boxOpen);
    slideFade = 1.0f;
}


static void Draw_Me(void) {
    if (meProfile.status == PRO_READY && gameFound && !install.active && setup.step == STEP_NONE) {
        nameRune_t r[NAME_RUNES + 3];
        char key[sizeof(pics[0].key)];
        const pic_t* pic;
        float nw, w, x, mid = TOP_MID;

        textTrack = TRACK_NAV;
        nw = SDL_min(Runes_Width(FONT_NAV, r, Name_Runes(meProfile.raw, r, NAME_RUNES)), 150.0f);
        textTrack = 0.0f;
        w = 8.0f + 24.0f + 10.0f + nw + 12.0f;
        x = UI_W - CTRL_W * 3.0f - 8.0f - w;
        Button_Set(BTN_ME, x, CTRL_Y + 4.0f, w, CTRL_H - 8.0f, true);
        if (heat[BTN_ME] > 0.0f) {
            Draw_Round(x, CTRL_Y + 4.0f, w, CTRL_H - 8.0f, 6.0f, Col_Alpha(COL_HUSH, heat[BTN_ME]));
        }
        SDL_snprintf(key, sizeof(key), "a:%s", meProfile.avatar);
        pic = meProfile.avatar[0] ? Pic_Find(key) : NULL;
        if (pic && pic->img.tex) {
            Draw_RoundImage(&pic->img, x + 8.0f, mid - 12.0f, 24.0f, 24.0f, 5.0f, Col_Gray(1.0f, pic->heat));
        } else {
            Draw_Round(x + 8.0f, mid - 12.0f, 24.0f, 24.0f, 5.0f, COL_P3);
        }
        Draw_NameFit(FONT_NAV, x + 42.0f, mid, COL_WHITE, meProfile.raw, TRACK_NAV, nw + 1.0f);
    } else {
        Button_Set(BTN_ME, 0.0f, 0.0f, 0.0f, 0.0f, false);
    }
}

static float hubClipTop, hubClipBottom = UI_H, hubMouseX = -1.0f, hubMouseY = -1.0f;
static bool hubLive;
static int hubFrom = HUB_PROFILE;
static SDL_Vertex chartV[HUB_CHART_PTS * 4];
static int chartI[HUB_CHART_PTS * 18];

static Uint32 Hsl(float h, float s, float l) {
    float c = (1.0f - SDL_fabsf(2.0f * l - 1.0f)) * s, hp = SDL_fmodf(h, 360.0f) / 60.0f;
    float x = c * (1.0f - SDL_fabsf(SDL_fmodf(hp, 2.0f) - 1.0f)), m = l - c * 0.5f, r = 0.0f, g = 0.0f, b = 0.0f;

    if (hp < 1.0f) {
        r = c;
        g = x;
    } else if (hp < 2.0f) {
        r = x;
        g = c;
    } else if (hp < 3.0f) {
        g = c;
        b = x;
    } else if (hp < 4.0f) {
        g = x;
        b = c;
    } else if (hp < 5.0f) {
        r = x;
        b = c;
    } else {
        r = c;
        b = x;
    }
    return ((Uint32)SDL_lroundf((r + m) * 255.0f) << 24) | ((Uint32)SDL_lroundf((g + m) * 255.0f) << 16) |
           ((Uint32)SDL_lroundf((b + m) * 255.0f) << 8) | 0xFF;
}

static void Hub_Tone(int hue) {
    if (hue < 0) {
        tone.p1 = COL_P1;
        tone.p2 = COL_P2;
        tone.p3 = COL_P3;
        tone.p4 = COL_P4;
        tone.ink = COL_WHITE;
        tone.ink2 = COL_W70;
        tone.gold = Hsl(42.0f, 0.49f, 0.58f);
        return;
    }
    tone.p1 = Hsl((float)hue, 0.14f, 0.125f);
    tone.p2 = Hsl((float)hue, 0.12f, 0.165f);
    tone.p3 = Hsl((float)hue, 0.11f, 0.205f);
    tone.p4 = Hsl((float)hue, 0.10f, 0.245f);
    tone.ink = Hsl((float)hue, 0.24f, 0.93f);
    tone.ink2 = Hsl((float)hue, 0.12f, 0.64f);
    tone.gold = Hsl((float)hue, 0.62f, 0.62f);
}

static void View_Shift(float dy) {
    SDL_Rect r;
    int w, h;

    if (dy == 0.0f || !SDL_GetCurrentRenderOutputSize(renderer, &w, &h)) {
        SDL_SetRenderViewport(renderer, NULL);
        return;
    }
    r.x = 0;
    r.y = (int)SDL_roundf(dy * uiScale);
    r.w = w;
    r.h = h;
    SDL_SetRenderViewport(renderer, &r);
}

static void Hub_Plain(int font, float x, float mid, Uint32 rgba, const char* text, float track) {
    textTrack = track;
    Draw_Text(font, x, Text_Mid(font, mid), rgba, text, -1);
    textTrack = 0.0f;
}

static float Plain_Width(int font, const char* text, float track) {
    float w;

    textTrack = track;
    w = Text_Width(font, text, -1) - (text[0] ? track : 0.0f);
    textTrack = 0.0f;
    return w;
}

static void Hub_Fit(int font, float x, float mid, Uint32 rgba, const char* text, float width) {
    Draw_Text(font, x, Text_Mid(font, mid), rgba, Text_Fit(font, text, width), -1);
}

static void Hub_Center(int font, float cx, float mid, Uint32 rgba, const char* text) {
    Draw_Text(font, cx - Text_Width(font, text, -1) * 0.5f, Text_Mid(font, mid), rgba, text, -1);
}

static void Hub_CapsCenter(int font, float cx, float mid, Uint32 rgba, const char* text, float track) {
    Draw_Caps(font, cx - Caps_Width(font, text, track) * 0.5f, mid, rgba, text, track);
}

static void Hub_Set(int index, float x, float y, float w, float h) {
    float y0 = SDL_max(y, hubClipTop), y1 = SDL_min(y + h, hubClipBottom);

    Button_Set(index, x, y0, w, y1 - y0, hubLive && y1 > y0);
}

static float Hub_Hit(int kind, int index, const char* id, float x, float y, float w, float h) {
    int slot = hubHitCount;

    if (slot >= HUB_ROWS) {
        return 0.0f;
    }
    hubHits[slot].kind = kind;
    hubHits[slot].index = index;
    SDL_strlcpy(hubHits[slot].id, id ? id : "", sizeof(hubHits[slot].id));
    hubHitCount++;
    Hub_Set(BTN_HUB_ROW_0 + slot, x, y, w, h);
    return heat[BTN_HUB_ROW_0 + slot];
}

static bool Hub_Visible(float y, float h) {
    return y + h > hubClipTop && y < hubClipBottom;
}

static void Num_Text(int n, char* out, size_t size) {
    char digits[24];
    int len, i, k = 0;

    SDL_snprintf(digits, sizeof(digits), "%d", n < 0 ? -n : n);
    len = (int)SDL_strlen(digits);
    if (n < 0 && size > 1) {
        out[k++] = '-';
    }
    for (i = 0; i < len && k < (int)size - 2; i++) {
        if (i && (len - i) % 3 == 0) {
            out[k++] = '.';
        }
        out[k++] = digits[i];
    }
    out[k] = '\0';
}

static void Acc_Text(float acc, char* out, size_t size) {
    char* comma;

    SDL_snprintf(out, size, "%.1f", acc);
    comma = SDL_strchr(out, '.');
    if (comma) {
        *comma = ',';
        if (comma[1] == '0' && !comma[2]) {
            *comma = '\0';
        }
    }
}

static int Tier_Of(int elo) {
    int i;

    for (i = 0; i < 7; i++) {
        if (elo >= hubTierMin[i]) {
            return i;
        }
    }
    return 7;
}

static const char* Map_Title(const char* map) {
    int i;

    for (i = 0; i < (int)SDL_arraysize(hubMaps); i++) {
        if (!SDL_strcasecmp(map, hubMaps[i][0])) {
            return hubMaps[i][1];
        }
    }
    return map;
}

static const char* Country_Name(const char* code) {
    int i;

    for (i = 0; i < (int)SDL_arraysize(hubCountries); i++) {
        if (!SDL_strcmp(code, hubCountries[i][0])) {
            return hubCountries[i][1];
        }
    }
    return code;
}

static const pic_t* Pic_Get(const char* kind, const char* value) {
    char key[sizeof(pics[0].key)];
    const pic_t* pic;

    if (!value[0]) {
        return NULL;
    }
    SDL_snprintf(key, sizeof(key), "%s%s", kind, value);
    pic = Pic_Find(key);
    return pic && pic->img.tex ? pic : NULL;
}

static void Hub_Icon(const char* path, float cx, float cy, float side) {
    const pic_t* pic = Pic_Get("i:", path);
    float w = side, h = side;

    if (!pic) {
        return;
    }
    if (pic->img.w > pic->img.h) {
        h = side * (float)pic->img.h / (float)pic->img.w;
    } else if (pic->img.h > pic->img.w) {
        w = side * (float)pic->img.w / (float)pic->img.h;
    }
    Draw_Image(&pic->img, cx - w * 0.5f, cy - h * 0.5f, w, h, pic->heat);
}

static void Hub_Shot(const char* map, float x, float y, float w, float h, float radius, float gray) {
    const shot_t* shot = map[0] ? Shot_Find(map) : NULL;
    const shot_t* fallback = Shot_Find("*default");

    Draw_Round(x, y, w, h, radius, tone.p4);
    if (!shot && fallback && (!map[0] || !Shot_Pending(map))) {
        shot = fallback;
    }
    if (shot) {
        Draw_RoundImage(w > THUMB_W * 1.2f ? &shot->img : &shot->mini, x, y, w, h, radius, Col_Gray(gray, shot->heat));
    }
}

static float Hub_Initial(const char* raw, float x, float y, float side, float radius, int font) {
    char clean[96], initial[2] = {'?', '\0'};
    const char* c;

    Server_Name(raw, clean, sizeof(clean));
    for (c = clean; *c; c++) {
        if (SDL_isalnum((unsigned char)*c)) {
            initial[0] = (char)SDL_toupper((unsigned char)*c);
            break;
        }
    }
    Draw_Round(x, y, side, side, radius, Col_Hue(clean));
    Draw_Text(font, x + (side - Text_Width(font, initial, -1)) * 0.5f, Text_Mid(font, y + side * 0.5f), Col_Alpha(COL_WHITE, 0.7f), initial, -1);
    return side;
}

static float Hub_Badges(const char* raw, const char* avatar, const char* country, bool clan, const char* clanPic, const char* clanTag, float x,
                        float mid) {
    player_t pl;

    SDL_zero(pl);
    Server_Name(raw, pl.name, sizeof(pl.name));
    SDL_strlcpy(pl.avatar, avatar, sizeof(pl.avatar));
    SDL_strlcpy(pl.country, country, sizeof(pl.country));
    SDL_strlcpy(pl.clanPic, clanPic, sizeof(pl.clanPic));
    SDL_strlcpy(pl.clanTag, clanTag, sizeof(pl.clanTag));
    pl.clan = clan;
    return Player_Badges(&pl, x, mid);
}

static float Hub_Button(int index, float x, float y, const char* text, Uint32 fill, Uint32 hotFill) {
    float w = Caps_Width(FONT_BOXBTN, text, TRACK_BUTTON) + 36.0f;

    Hub_Set(index, x, y, w, 40.0f);
    Draw_Round(x, y, w, 40.0f, 6.0f, Col_Mix(fill, hotFill, hubLive ? heat[index] : 0.0f));
    Draw_Caps(FONT_BOXBTN, x + 18.0f, y + 20.0f, Col_Mix(tone.ink2, tone.ink, hubLive ? heat[index] : 0.0f), text, TRACK_BUTTON);
    return w;
}

static float Hub_Tabs(int first, int count, const char* const* names, int selected, float x, float mid, int font, float track, float gap) {
    int i;

    for (i = 0; i < count; i++) {
        float w = Caps_Width(font, names[i], track);
        Uint32 c = i == selected ? tone.ink : Col_Mix(tone.ink2, tone.ink, heat[first + i] * 0.7f);

        Draw_Caps(font, x, mid, c, names[i], track);
        Hub_Set(first + i, x - 6.0f, mid - 14.0f, w + 12.0f, 28.0f);
        x += w + gap;
    }
    return x;
}

static float Hub_Card(float y, float h, const char* title, const char* aside) {
    Draw_Round(HUB_IN, y, HUB_CW, h, HUB_R, tone.p2);
    if (title) {
        Draw_Caps(FONT_HUB_SECTION, HUB_IN + HUB_PAD, y + HUB_PAD + 12.0f, tone.ink, title, 0.8f);
    }
    if (aside) {
        Hub_Plain(FONT_HUB_ASIDE, HUB_IN + HUB_CW - HUB_PAD - Plain_Width(FONT_HUB_ASIDE, aside, 1.2f), y + HUB_PAD + 12.0f, tone.ink2, aside, 1.2f);
    }
    return y + HUB_PAD + (title ? 24.0f : 0.0f);
}

static float Para_Height(int font, float width, float lineH, const char* text) {
    return Draw_Para(font, 0.0f, -100000.0f, width, lineH, 0, text) + 100000.0f;
}

static float Hub_Empty(float x, float y, float width, const char* text, bool draw) {
    float h = Para_Height(FONT_HUB_BIO, width, 24.0f, text);

    if (draw) {
        Draw_Para(FONT_HUB_BIO, x, y, width, 24.0f, tone.ink2, text);
    }
    return h;
}

static float Hub_Note(float y, const char* text) {
    Draw_Para(FONT_HUB_BIO, HUB_IN, y, HUB_CW, 24.0f, tone.ink2, text);
    return y + 24.0f;
}

static const char* Season_Name(int season) {
    static char name[24];

    SDL_snprintf(name, sizeof(name), T(TXT_HUB_SEASON), season + 1);
    return name;
}

static float Hub_Header(float y) {
    const hubPlayer_t* pl = &hubPlayer;
    const pic_t* pic;
    char line[200], clean[96];
    float cover = SDL_floorf(HUB_CW * 215.0f / 1000.0f), strip, top, tx, mid, bw, ax, ay, x;
    bool title = pl->title[0] != '\0';

    strip = 16.0f + 36.0f + (title ? 24.0f : 0.0f) + 10.0f + 16.0f + 18.0f;
    Draw_Round(HUB_IN, y, HUB_CW, cover + strip, HUB_R, tone.p3);
    pic = Pic_Get("B:", pl->banner);
    if (pic) {
        Draw_RoundImage(&pic->img, HUB_IN, y, HUB_CW, cover + HUB_R, HUB_R, Col_Gray(1.0f, pic->heat));
        Draw_Rect(HUB_IN, y + cover, HUB_CW, HUB_R, tone.p3);
    }
    top = y + cover;
    ax = HUB_IN + 28.0f;
    ay = top + 16.0f - 54.0f;
    pic = Pic_Get("A:", pl->avatar);
    if (pic) {
        Draw_RoundImage(&pic->img, ax, ay, 104.0f, 104.0f, 8.0f, Col_Gray(1.0f, pic->heat));
    } else {
        Hub_Initial(pl->raw, ax, ay, 104.0f, 8.0f, FONT_HUB_INITIAL);
    }
    tx = ax + 104.0f + 18.0f;
    bw = Caps_Width(FONT_BOXBTN, T(TXT_HUB_SITE), TRACK_BUTTON) + 36.0f;
    Hub_Button(BTN_HUB_SITE, HUB_IN + HUB_CW - 28.0f - bw, top + (strip - 40.0f) * 0.5f, T(TXT_HUB_SITE), tone.p4, Col_Mix(tone.p4, tone.ink, 0.12f));
    mid = top + 16.0f + 18.0f;
    Draw_NameFit(FONT_HUB_NAME, tx, mid, tone.ink, Text_Upper(pl->raw), 1.2f, HUB_IN + HUB_CW - 28.0f - bw - 24.0f - tx);
    mid += 18.0f;
    if (title) {
        Draw_CapsFit(FONT_HUB_TITLE, tx, mid + 6.0f + 9.0f, pl->titleColor, pl->title, 0.42f, HUB_IN + HUB_CW - 28.0f - bw - 24.0f - tx);
        mid += 24.0f;
    }
    mid += 10.0f + 8.0f;
    x = tx;
    pic = Pic_Get("f:", pl->country);
    if (pic) {
        Draw_Image(&pic->img, x, mid - 8.0f, 16.0f, 16.0f, pic->heat);
        x += 22.0f;
    }
    Draw_Caps(FONT_HUB_DATA, x, mid, tone.ink, Country_Name(pl->country), 0.48f);
    x += Caps_Width(FONT_HUB_DATA, Country_Name(pl->country), 0.48f) + 16.0f;
    if (pl->clan) {
        pic = Pic_Get("c:", pl->flag[0] ? pl->flag : CLAN_DEFAULT);
        Draw_Round(x, mid - 7.0f, 28.0f, 14.0f, 4.0f, tone.p4);
        if (pic) {
            Draw_RoundImage(&pic->img, x, mid - 7.0f, 28.0f, 14.0f, 4.0f, Col_Gray(1.0f, pic->heat));
        }
        x += 34.0f;
        Server_Name(pl->clanName, clean, sizeof(clean));
        Draw_Caps(FONT_HUB_DATA, x, mid, tone.ink, clean, 0.48f);
        x += Caps_Width(FONT_HUB_DATA, clean, 0.48f) + 16.0f;
    }
    if (pl->since[0]) {
        SDL_snprintf(line, sizeof(line), T(TXT_HUB_SINCE), pl->since);
        Draw_Caps(FONT_HUB_DATA, x, mid, tone.ink2, line, 0.48f);
    }
    return top + strip;
}

static int Caps_Wrap(int font, float x, float mid, float step, Uint32 rgba, const char* text, float track, float width, bool draw) {
    char part[128];
    const char* start = text;
    int lines = 0;

    while (*start) {
        const char* end = NULL;
        const char* p;

        for (p = start; *p; p++) {
            if (*p == ' ') {
                SDL_strlcpy(part, start, SDL_min(sizeof(part), (size_t)(p - start + 1)));
                if (Caps_Width(font, part, track) + track > width) {
                    break;
                }
                end = p;
            }
        }
        if (!*p && Caps_Width(font, start, track) + track <= width) {
            end = p;
        }
        if (!end) {
            end = SDL_strchr(start, ' ');
            if (!end) {
                end = start + SDL_strlen(start);
            }
        }
        SDL_strlcpy(part, start, SDL_min(sizeof(part), (size_t)(end - start + 1)));
        if (draw) {
            Draw_CapsFit(font, x, mid + step * (float)lines, rgba, part, track, width);
        }
        lines++;
        start = *end ? end + 1 : end;
    }
    return SDL_max(lines, 1);
}

static float Hub_Tile(float x, float y, float w, float h, const char* label, Uint32 fill) {
    Draw_Round(x, y, w, h, 8.0f, fill);
    return 15.75f * (float)(Caps_Wrap(FONT_HUB_LABEL, x + 18.0f, y + 24.3f, 15.75f, tone.ink2, label, 1.05f, w - 36.0f, true) - 1);
}

static void Hub_Mark(float x, float mid, int why, int left) {
    char line[48];
    Uint32 c = why == WHY_INACTIVE ? 0x8D867CFF : why == WHY_BANNED ? 0xCD6448FF : tone.gold;
    float w;

    if (why == WHY_NONE) {
        return;
    }
    if (why == WHY_PLACEMENT && left > 0) {
        SDL_snprintf(line, sizeof(line), "%s %d", T(TXT_HUB_CALIBRATING), left);
    } else {
        SDL_strlcpy(line, T(why == WHY_PLACEMENT ? TXT_HUB_CALIBRATING : why == WHY_INACTIVE ? TXT_HUB_INACTIVE : TXT_HUB_BANNED), sizeof(line));
    }
    c = Col_Alpha(c, 0.85f);
    w = Caps_Width(FONT_HUB_MARK, line, 0.7f) + 8.0f;
    Draw_Rect(x, mid - 8.5f, w, 1.0f, c);
    Draw_Rect(x, mid + 7.5f, w, 1.0f, c);
    Draw_Rect(x, mid - 7.5f, 1.0f, 15.0f, c);
    Draw_Rect(x + w - 1.0f, mid - 7.5f, 1.0f, 15.0f, c);
    Draw_Caps(FONT_HUB_MARK, x + 4.0f, mid, c, line, 0.7f);
}

static float Hub_StatsCard(float y) {
    const hubStats_t* st = &hubStats;
    const hubPlayer_t* pl = &hubPlayer;
    const char* mode = hubModeName[hubMode];
    char aside[96], line[96], a[24], b[24];
    bool closed = hubSeason < HUB_SEASONS - 1, clutch = hubMode == 0 && !closed;
    int elo, wins, losses, draws, games, rate, n = clutch ? 6 : 5;
    float inner = HUB_CW - HUB_PAD * 2.0f, unit = (inner - 10.0f * (float)(n - 1)) / (1.55f + (float)(n - 1)), x, w, ty, o, h;

    if (closed) {
        elo = st->elo;
        wins = st->wins;
        losses = st->losses;
        draws = st->draws;
    } else {
        elo = pl->rating[hubMode];
        wins = pl->wins[hubMode];
        losses = pl->losses[hubMode];
        draws = pl->draws[hubMode];
    }
    games = wins + losses + draws;
    rate = games > 0 ? (int)SDL_floorf((float)wins * 100.0f / (float)games + 0.5f) : 0;
    SDL_snprintf(aside, sizeof(aside), "%s \xc2\xb7 %s", Text_Upper(mode), Season_Name(hubSeason));
    h = 150.0f + 15.75f * (float)(Caps_Wrap(FONT_HUB_LABEL, 0.0f, 0.0f, 0.0f, 0, T(TXT_HUB_FAV), 1.05f, unit - 36.0f, false) - 1);
    ty = Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 16.0f + h, T(TXT_HUB_STATS), aside) + 16.0f;
    x = HUB_IN + HUB_PAD;

    w = unit * 1.55f;
    Hub_Tile(x, ty, w, h, T(TXT_HUB_TIER), tone.p3);
    ty += (h - 150.0f) * 0.5f;
    Hub_Icon(hubTierImage[Tier_Of(elo)], x + 18.0f + 28.0f, ty + 88.3f, 56.0f);
    SDL_snprintf(line, sizeof(line), "%d", elo);
    Hub_Plain(FONT_HUB_FIGURE, x + 18.0f + 56.0f + 16.0f, ty + 81.0f, tone.ink, line, 0.28f);
    SDL_snprintf(line, sizeof(line), T(TXT_HUB_ELO), Text_Upper(mode));
    Draw_Caps(FONT_HUB_LABEL, x + 18.0f + 56.0f + 16.0f, ty + 103.0f, tone.ink2, line, 0.84f);
    Hub_Mark(x + 18.0f + 56.0f + 16.0f + Caps_Width(FONT_HUB_LABEL, line, 0.84f) + 6.0f, ty + 103.0f, st->why, st->left);
    ty -= (h - 150.0f) * 0.5f;
    x += w + 10.0f;

    w = unit;
    if (!closed && st->haveRank && st->rank > 0) {
        float hot = hubLive ? heat[BTN_HUB_RANKLINK] : 0.0f;

        o = Hub_Tile(x, ty, w, h, T(TXT_HUB_GLOBAL), Col_Mix(tone.p3, tone.p4, hot));
        Hub_Set(BTN_HUB_RANKLINK, x, ty, w, h);
        SDL_snprintf(line, sizeof(line), "#%d", st->rank);
        Num_Text(st->total, a, sizeof(a));
        SDL_snprintf(aside, sizeof(aside), T(TXT_HUB_PLAYERS), a);
    } else if (closed) {
        o = Hub_Tile(x, ty, w, h, T(TXT_HUB_GLOBAL), tone.p3);
        if (st->haveRank && st->rank > 0) {
            SDL_snprintf(line, sizeof(line), "#%d", st->rank);
            Num_Text(st->total, a, sizeof(a));
            SDL_snprintf(aside, sizeof(aside), T(TXT_HUB_PLAYERS_IN), a, Season_Name(hubSeason));
        } else {
            SDL_strlcpy(line, T(TXT_HUB_UNRANKED), sizeof(line));
            if (st->played > 0) {
                SDL_snprintf(aside, sizeof(aside), T(TXT_HUB_FEWER), st->minGames, Season_Name(hubSeason));
            } else {
                SDL_snprintf(aside, sizeof(aside), T(TXT_HUB_DIDNT), Season_Name(hubSeason));
            }
        }
    } else {
        o = Hub_Tile(x, ty, w, h, T(TXT_HUB_GLOBAL), tone.p3);
        if (st->why == WHY_PLACEMENT) {
            SDL_snprintf(line, sizeof(line), "%d", st->left);
        } else {
            SDL_strlcpy(line, T(TXT_HUB_UNRANKED), sizeof(line));
        }
        SDL_strlcpy(aside, T(st->why == WHY_PLACEMENT ? TXT_HUB_TO_ENTER : TXT_HUB_COME_BACK), sizeof(aside));
    }
    Hub_Plain(FONT_HUB_FIGURE, x + 18.0f, ty + o + 59.3f, tone.ink, Text_Fit(FONT_HUB_FIGURE, line, w - 36.0f), 0.28f);
    Caps_Wrap(FONT_HUB_LABEL, x + 18.0f, ty + o + 85.4f, 14.7f, tone.ink2, aside, 0.84f, w - 36.0f, true);
    x += w + 10.0f;

    o = Hub_Tile(x, ty, w, h, T(TXT_HUB_RECORD), tone.p3);
    SDL_snprintf(line, sizeof(line), "%d-%d", wins, losses);
    Hub_Plain(FONT_HUB_FIGURE, x + 18.0f, ty + o + 59.3f, tone.ink, line, 0.28f);
    SDL_snprintf(line, sizeof(line), T(TXT_HUB_WINRATE), rate);
    o += 14.7f * (float)(Caps_Wrap(FONT_HUB_LABEL, x + 18.0f, ty + o + 85.4f, 14.7f, tone.ink2, line, 0.84f, w - 36.0f, true) - 1);
    Draw_Round(x + 18.0f, ty + o + 106.7f, w - 36.0f, 4.0f, 2.0f, tone.p1);
    if (rate > 0) {
        Draw_Round(x + 18.0f, ty + o + 106.7f, SDL_max(4.0f, (w - 36.0f) * (float)rate / 100.0f), 4.0f, 2.0f, tone.ink);
    }
    x += w + 10.0f;

    o = Hub_Tile(x, ty, w, h, T(TXT_HUB_KD), tone.p3);
    SDL_snprintf(line, sizeof(line), "%.2f", st->kills > 0 ? (float)st->kills / (float)SDL_max(st->deaths, 1) : 0.0f);
    Hub_Plain(FONT_HUB_FIGURE, x + 18.0f, ty + o + 59.3f, tone.ink, line, 0.28f);
    Num_Text(st->kills, a, sizeof(a));
    Num_Text(st->deaths, b, sizeof(b));
    SDL_snprintf(line, sizeof(line), T(TXT_HUB_FRAGS), a, b);
    Caps_Wrap(FONT_HUB_LABEL, x + 18.0f, ty + o + 85.4f, 14.7f, tone.ink2, line, 0.84f, w - 36.0f, true);
    x += w + 10.0f;

    if (clutch) {
        o = Hub_Tile(x, ty, w, h, T(TXT_HUB_CLUTCH), tone.p3);
        if (st->haveClutch) {
            Num_Text(st->clutches, line, sizeof(line));
            Num_Text(st->lastAlive, a, sizeof(a));
            SDL_snprintf(aside, sizeof(aside), T(TXT_HUB_CLUTCH_OF), a);
        } else {
            SDL_strlcpy(line, "\xe2\x80\x94", sizeof(line));
            SDL_strlcpy(aside, T(TXT_HUB_CLUTCH_SOON), sizeof(aside));
        }
        Hub_Plain(FONT_HUB_FIGURE, x + 18.0f, ty + o + 59.3f, tone.ink, line, 0.28f);
        Caps_Wrap(FONT_HUB_LABEL, x + 18.0f, ty + o + 85.4f, 14.7f, tone.ink2, aside, 0.84f, w - 36.0f, true);
        x += w + 10.0f;
    }

    o = Hub_Tile(x, ty, w, h, T(TXT_HUB_FAV), tone.p3);
    Hub_Shot(st->favMap[0] ? st->favMap : "", x + 18.0f, ty + o + 44.6f, w - 36.0f, 64.0f, 6.0f, 1.0f);
    Draw_CapsFit(FONT_HUB_LABEL, x + 18.0f, ty + o + 128.0f, tone.ink2, st->favMap[0] ? Map_Title(st->favMap) : T(TXT_HUB_FAV_NONE), 0.84f, w - 36.0f);
    return y + HUB_PAD * 2.0f + 24.0f + 16.0f + h;
}

static int Chart_Points(const hubStats_t* st, float x, float y, float w, float h, float* px, float* py, float* lo, float* hi) {
    float minE = 1e9f, maxE = -1e9f, range, pad, m[HUB_ELO_MAX], d[HUB_ELO_MAX], vy[HUB_ELO_MAX], step;
    int n = st->eloCount, i, k, sub, count = 0;

    for (i = 0; i < n; i++) {
        minE = SDL_min(minE, (float)st->eloValue[i]);
        maxE = SDL_max(maxE, (float)st->eloValue[i]);
    }
    range = maxE - minE > 0.0f ? maxE - minE : 100.0f;
    pad = range * 0.15f;
    *lo = minE - pad;
    *hi = maxE + pad;
    step = w / (float)(n - 1);
    for (i = 0; i < n; i++) {
        vy[i] = y + 8.0f + (1.0f - ((float)st->eloValue[i] - *lo) / (*hi - *lo)) * (h - 16.0f);
    }
    for (i = 0; i < n - 1; i++) {
        d[i] = (vy[i + 1] - vy[i]) / step;
    }
    m[0] = d[0];
    m[n - 1] = d[n - 2];
    for (i = 1; i < n - 1; i++) {
        m[i] = d[i - 1] * d[i] <= 0.0f ? 0.0f : (d[i - 1] + d[i]) * 0.5f;
    }
    for (i = 0; i < n - 1; i++) {
        if (d[i] == 0.0f) {
            m[i] = m[i + 1] = 0.0f;
        } else {
            float a = m[i] / d[i], b = m[i + 1] / d[i], s = a * a + b * b;

            if (s > 9.0f) {
                float t = 3.0f / SDL_sqrtf(s);

                m[i] = t * a * d[i];
                m[i + 1] = t * b * d[i];
            }
        }
    }
    sub = n < 50 ? 8 : n < 200 ? 3 : 1;
    while ((n - 1) * sub + 1 > HUB_CHART_PTS && sub > 1) {
        sub--;
    }
    for (i = 0; i < n - 1 && count < HUB_CHART_PTS - 1; i++) {
        for (k = 0; k < sub && count < HUB_CHART_PTS - 1; k++) {
            float t = (float)k / (float)sub, t2 = t * t, t3 = t2 * t;

            px[count] = x + step * ((float)i + t);
            py[count] = (2.0f * t3 - 3.0f * t2 + 1.0f) * vy[i] + (t3 - 2.0f * t2 + t) * step * m[i] + (-2.0f * t3 + 3.0f * t2) * vy[i + 1] +
                        (t3 - t2) * step * m[i + 1];
            count++;
        }
    }
    px[count] = x + w;
    py[count] = vy[n - 1];
    return count + 1;
}

static SDL_FColor Chart_Color(Uint32 rgba, float alpha) {
    SDL_FColor c;

    rgba = Col_Fade(rgba);
    c.r = (float)((rgba >> 24) & 255) / 255.0f;
    c.g = (float)((rgba >> 16) & 255) / 255.0f;
    c.b = (float)((rgba >> 8) & 255) / 255.0f;
    c.a = (float)(rgba & 255) / 255.0f * alpha;
    return c;
}

static void Chart_Vertex(int i, float x, float y, SDL_FColor c) {
    chartV[i].position.x = SDL_roundf(x * uiScale * 4.0f) / 4.0f;
    chartV[i].position.y = SDL_roundf(y * uiScale * 4.0f) / 4.0f;
    chartV[i].color = c;
    chartV[i].tex_coord.x = 0.0f;
    chartV[i].tex_coord.y = 0.0f;
}

static void Hub_Chart(float x, float y, float w, float h) {
    static float px[HUB_CHART_PTS], py[HUB_CHART_PTS];
    const hubStats_t* st = &hubStats;
    SDL_FColor fill = Chart_Color(tone.gold, 0.08f), line = Chart_Color(tone.gold, 1.0f), clear = Chart_Color(tone.gold, 0.0f);
    float lo, hi, half = 1.25f, feather = 0.6f / uiScale;
    char text[48], num[24];
    int n, i, nv = 0, ni = 0;

    n = Chart_Points(st, x, y, w, h, px, py, &lo, &hi);
    for (i = 0; i < n; i++) {
        Chart_Vertex(nv++, px[i], py[i], fill);
        Chart_Vertex(nv++, px[i], y + h, fill);
    }
    for (i = 0; i < n - 1; i++) {
        chartI[ni++] = i * 2;
        chartI[ni++] = i * 2 + 2;
        chartI[ni++] = i * 2 + 1;
        chartI[ni++] = i * 2 + 1;
        chartI[ni++] = i * 2 + 2;
        chartI[ni++] = i * 2 + 3;
    }
    SDL_RenderGeometry(renderer, NULL, chartV, nv, chartI, ni);
    nv = ni = 0;
    for (i = 0; i < n; i++) {
        float dx = px[SDL_min(i + 1, n - 1)] - px[SDL_max(i - 1, 0)], dy = py[SDL_min(i + 1, n - 1)] - py[SDL_max(i - 1, 0)];
        float len = SDL_sqrtf(dx * dx + dy * dy), nx = len > 0.0f ? -dy / len : 0.0f, ny = len > 0.0f ? dx / len : 1.0f;

        Chart_Vertex(nv++, px[i] + nx * (half + feather), py[i] + ny * (half + feather), clear);
        Chart_Vertex(nv++, px[i] + nx * (half - feather), py[i] + ny * (half - feather), line);
        Chart_Vertex(nv++, px[i] - nx * (half - feather), py[i] - ny * (half - feather), line);
        Chart_Vertex(nv++, px[i] - nx * (half + feather), py[i] - ny * (half + feather), clear);
    }
    for (i = 0; i < n - 1; i++) {
        int k, a = i * 4, b = i * 4 + 4;

        for (k = 0; k < 3; k++) {
            chartI[ni++] = a + k;
            chartI[ni++] = b + k;
            chartI[ni++] = a + k + 1;
            chartI[ni++] = a + k + 1;
            chartI[ni++] = b + k;
            chartI[ni++] = b + k + 1;
        }
    }
    SDL_RenderGeometry(renderer, NULL, chartV, nv, chartI, ni);
    Num_Text((int)SDL_roundf(hi), num, sizeof(num));
    Hub_Plain(FONT_HUB_AXIS, x + 8.0f, y + 6.0f + 6.0f, tone.ink2, num, 0.0f);
    Num_Text((int)SDL_roundf(lo), num, sizeof(num));
    Hub_Plain(FONT_HUB_AXIS, x + 8.0f, y + h - 6.0f - 6.0f, tone.ink2, num, 0.0f);
    if (hubLive && hubMouseX >= x && hubMouseX <= x + w && hubMouseY >= y && hubMouseY <= y + h && hubMouseY >= hubClipTop &&
        hubMouseY <= hubClipBottom) {
        int k = (int)SDL_floorf((hubMouseX - x) / w * (float)(st->eloCount - 1) + 0.5f);
        float dx, dy, gw, gh, gx, w1, w2, w3 = 0.0f;
        const char* change = NULL;
        char up[32];

        k = SDL_clamp(k, 0, st->eloCount - 1);
        dx = x + w * (float)k / (float)(st->eloCount - 1);
        dy = y + 8.0f + (1.0f - ((float)st->eloValue[k] - lo) / (hi - lo)) * (h - 16.0f);
        Draw_Round(dx - 5.0f, dy - 5.0f, 10.0f, 10.0f, 5.0f, tone.p2);
        Draw_Round(dx - 3.0f, dy - 3.0f, 6.0f, 6.0f, 3.0f, tone.gold);
        Num_Text(st->eloValue[k], num, sizeof(num));
        SDL_snprintf(text, sizeof(text), T(TXT_HUB_ELO_AT), num);
        w1 = Text_Width(FONT_HUB_GLOBEB, text, -1);
        w2 = Text_Width(FONT_HUB_GLOBE, st->eloDate[k], -1);
        if (st->eloChange[k] != 0) {
            SDL_snprintf(up, sizeof(up), T(st->eloChange[k] > 0 ? TXT_HUB_UP : TXT_HUB_DOWN), SDL_abs(st->eloChange[k]));
            change = up;
            w3 = Text_Width(FONT_HUB_GLOBE, up, -1);
        }
        gw = SDL_max(w1, SDL_max(w2, w3)) + 20.0f;
        gh = 12.0f + 16.0f + 1.0f + 15.0f + (change ? 16.0f : 0.0f);
        gx = (dx - x) / w > 0.75f ? dx - gw : dx;
        Draw_Round(gx, y + 8.0f, gw, gh, 6.0f, tone.p4);
        Hub_Plain(FONT_HUB_GLOBEB, gx + 10.0f, y + 8.0f + 6.0f + 8.0f, tone.ink, text, 0.0f);
        Hub_Plain(FONT_HUB_GLOBE, gx + 10.0f, y + 8.0f + 6.0f + 17.0f + 7.5f, tone.ink2, st->eloDate[k], 0.0f);
        if (change) {
            Hub_Plain(FONT_HUB_GLOBE, gx + 10.0f, y + 8.0f + 6.0f + 33.0f + 7.5f, st->eloChange[k] > 0 ? COL_OK : COL_MAL, change, 0.0f);
        }
    }
}

static float Hub_HistoryCard(float y) {
    const char* note = NULL;
    float body = 140.0f, ty;

    if (hubSeason == 0) {
        note = T(TXT_HUB_HISTORY_S1);
    } else if (hubStats.eloCount < 2) {
        note = T(TXT_HUB_HISTORY_FEW);
    }
    if (note) {
        body = Hub_Empty(0.0f, 0.0f, HUB_CW - HUB_PAD * 2.0f, note, false);
    }
    ty = Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 16.0f + body, T(TXT_HUB_HISTORY), NULL) + 16.0f;
    if (note) {
        Hub_Empty(HUB_IN + HUB_PAD, ty, HUB_CW - HUB_PAD * 2.0f, note, true);
    } else if (Hub_Visible(ty, body)) {
        Hub_Chart(HUB_IN + HUB_PAD, ty, HUB_CW - HUB_PAD * 2.0f, body);
    }
    return y + HUB_PAD * 2.0f + 24.0f + 16.0f + body;
}

static int Name_Lines(int font, const char* text, float width, char* first, size_t size, const char** rest) {
    const char* p;
    const char* cut = NULL;
    char probe[96];

    *rest = NULL;
    if (Caps_Width(font, text, 0.66f) <= width) {
        SDL_strlcpy(first, text, size);
        return 1;
    }
    for (p = text; *p; p++) {
        if (*p == ' ') {
            SDL_strlcpy(probe, text, SDL_min(sizeof(probe), (size_t)(p - text + 1)));
            if (Caps_Width(font, probe, 0.66f) <= width) {
                cut = p;
            }
        }
    }
    if (!cut) {
        SDL_strlcpy(first, text, size);
        return 1;
    }
    SDL_strlcpy(first, text, SDL_min(size, (size_t)(cut - text + 1)));
    *rest = cut + 1;
    return 2;
}

static float Hub_Grid(float x, float y, float w, int count, const int* order, const char* const* names, const char* const* icons, const char* const* data,
                      bool draw) {
    float col = (w - 10.0f * 7.0f) / 8.0f, rowH = 0.0f, top = y;
    int i;

    for (i = 0; i < count; i++) {
        char first[96];
        const char* rest;
        int lines = Name_Lines(FONT_HUB_WEAPON, names[order[i]], col, first, sizeof(first), &rest);
        float h = 36.0f + 6.0f + 13.75f * (float)lines + 6.0f + 16.0f, cx = x + (col + 10.0f) * (float)(i % 8) + col * 0.5f;

        if (i % 8 == 0 && i) {
            y += rowH + 22.0f;
            rowH = 0.0f;
        }
        rowH = SDL_max(rowH, h);
        if (draw && Hub_Visible(y, h)) {
            Hub_Icon(icons[order[i]], cx, y + 18.0f, 36.0f);
            Hub_CapsCenter(FONT_HUB_WEAPON, cx, y + 42.0f + 6.9f, tone.ink, first, 0.66f);
            if (rest) {
                Hub_CapsCenter(FONT_HUB_WEAPON, cx, y + 42.0f + 13.75f + 6.9f, tone.ink, rest, 0.66f);
            }
            Hub_Center(FONT_HUB_DATO, cx, y + 42.0f + 13.75f * (float)lines + 6.0f + 8.0f, tone.ink2, data[i]);
        }
    }
    return count ? y + rowH - top : 0.0f;
}

static float Hub_IconsCard(float y, bool medals) {
    static char data[16][32];
    const char* list[16];
    const hubStats_t* st = &hubStats;
    const char* note;
    char text[160];
    int order[16], count = 0, i;
    float body, ty, inner = HUB_CW - HUB_PAD * 2.0f;

    if (medals) {
        for (i = 0; i < st->medalCount; i++) {
            order[count] = st->medalOrder[i];
            Num_Text(st->medal[order[count]], data[count], sizeof(data[0]));
            list[count] = data[count];
            count++;
        }
    } else {
        for (i = 0; i < st->weaponCount; i++) {
            char acc[16];
            int k = st->weaponOrder[i];

            if (k == 8) {
                continue;
            }
            Acc_Text(st->weapons[k].acc, acc, sizeof(acc));
            SDL_snprintf(data[count], sizeof(data[0]), T(TXT_HUB_ACC), acc);
            list[count] = data[count];
            order[count++] = k;
        }
    }
    if (!count) {
        if (hubSeason < HUB_SEASONS - 1) {
            SDL_snprintf(text, sizeof(text), T(medals ? TXT_HUB_NO_MEDALS_IN : TXT_HUB_NO_WEAPONS_IN), hubModeName[hubMode], Season_Name(hubSeason));
        } else {
            SDL_snprintf(text, sizeof(text), T(medals ? TXT_HUB_NO_MEDALS : TXT_HUB_NO_WEAPONS), hubModeName[hubMode]);
        }
        note = text;
        body = Hub_Empty(0.0f, 0.0f, inner, note, false);
        ty = Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 16.0f + body, T(medals ? TXT_HUB_MEDALS : TXT_HUB_ACCURACY), NULL) + 16.0f;
        Hub_Empty(HUB_IN + HUB_PAD, ty, inner, note, true);
        return y + HUB_PAD * 2.0f + 24.0f + 16.0f + body;
    }
    body = Hub_Grid(0.0f, 0.0f, inner, count, order, medals ? hubMedalName : hubWeaponName, medals ? hubMedalIcon : hubWeaponIcon, list, false);
    ty = Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 18.0f + body, T(medals ? TXT_HUB_MEDALS : TXT_HUB_ACCURACY), NULL) + 18.0f;
    Hub_Grid(HUB_IN + HUB_PAD, ty, inner, count, order, medals ? hubMedalName : hubWeaponName, medals ? hubMedalIcon : hubWeaponIcon, list, true);
    return y + HUB_PAD * 2.0f + 24.0f + 18.0f + body;
}

static void Game_Columns(float x, float w, float* col) {
    float fr = (w - 432.0f - 84.0f) / 2.4f;

    col[0] = x;
    col[1] = col[0] + 40.0f + 14.0f;
    col[2] = col[1] + 64.0f + 14.0f;
    col[3] = col[2] + fr * 1.4f + 14.0f;
    col[4] = col[3] + fr + 14.0f;
    col[5] = col[4] + 144.0f + 14.0f;
    col[6] = col[5] + 72.0f + 14.0f;
    col[7] = col[6] + 112.0f;
}

static void Hub_GameHead(float x, float w, float mid) {
    static const int labels[5] = {TXT_HUB_COL_SERVER, TXT_HUB_COL_VS, TXT_HUB_COL_FRAGS, TXT_HUB_COL_ELO, TXT_HUB_COL_DATE};
    float col[8];
    int i;

    Game_Columns(x + 8.0f, w - 16.0f, col);
    for (i = 0; i < 5; i++) {
        Draw_Caps(FONT_HUB_MONO, col[2 + i], mid, tone.ink2, T(labels[i]), 1.6f);
    }
}

static void Hub_GameRow(const hubGame_t* g, int index, float x, float y, float w) {
    char line[96];
    float col[8], mid = y + 27.0f, hot;
    Uint32 c;

    hot = Hub_Hit(HIT_MATCH, index, g->id, x, y, w, 54.0f);
    if (hot > 0.0f) {
        Draw_Round(x, y, w, 54.0f, 8.0f, Col_Alpha(tone.p3, hot));
    }
    Game_Columns(x + 8.0f, w - 16.0f, col);
    c = g->win ? COL_OK : g->draw ? tone.ink2 : COL_MAL;
    Hub_Center(FONT_HUB_RESULT, col[0] + 20.0f, mid, c, g->win ? "W" : g->draw ? "E" : "L");
    Hub_Shot(g->map, col[1], mid - 20.0f, 64.0f, 40.0f, 6.0f, 1.0f);
    Draw_CapsFit(FONT_HUB_SEASON, col[2], mid - 8.0f, tone.ink, g->server, 0.4f, col[3] - col[2] - 14.0f);
    SDL_snprintf(line, sizeof(line), "%s - %s", Text_Upper(g->mode), Map_Title(g->map));
    Draw_CapsFit(FONT_HUB_MAPLINE, col[2], mid + 9.0f, tone.ink2, line, 0.69f, col[3] - col[2] - 14.0f);
    if (g->rival[0]) {
        Draw_NameFit(FONT_HUB_RIVAL, col[3], mid, tone.ink, g->rival, 0.6f, col[4] - col[3] - 14.0f);
    } else {
        Hub_Plain(FONT_HUB_RIVAL, col[3], mid, tone.ink2, "-", 0.0f);
    }
    SDL_snprintf(line, sizeof(line), T(TXT_HUB_GAME_FRAGS), g->kills, g->deaths);
    Hub_Fit(FONT_HUB_DATO, col[4], mid, tone.ink2, line, col[5] - col[4] - 14.0f);
    if (g->counts && g->elo != 0) {
        SDL_snprintf(line, sizeof(line), "%+d", g->elo);
        Hub_Plain(FONT_HUB_GLOBEB, col[5], mid, g->elo > 0 ? COL_OK : COL_MAL, line, 0.0f);
    } else {
        Hub_Plain(FONT_HUB_GLOBEB, col[5], mid, tone.ink2, "-", 0.0f);
    }
    Hub_Fit(FONT_HUB_DATO, col[6], mid, tone.ink2, g->date, col[7] - col[6]);
}

static float Hub_RecentCard(float y) {
    const hubStats_t* st = &hubStats;
    char text[160];
    float body, ty, x = HUB_IN + HUB_PAD, w = HUB_CW - HUB_PAD * 2.0f, bw;
    int i;

    if (!st->gameCount) {
        if (hubSeason < HUB_SEASONS - 1) {
            SDL_snprintf(text, sizeof(text), T(TXT_HUB_NO_GAMES_IN), hubModeName[hubMode], Season_Name(hubSeason));
        } else {
            SDL_snprintf(text, sizeof(text), T(TXT_HUB_NO_GAMES), hubModeName[hubMode]);
        }
        body = Hub_Empty(0.0f, 0.0f, w, text, false);
        ty = Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 16.0f + body, T(TXT_HUB_RECENT), NULL) + 16.0f;
        Hub_Empty(x, ty, w, text, true);
        return y + HUB_PAD * 2.0f + 24.0f + 16.0f + body;
    }
    body = 28.0f + (float)st->gameCount * 54.0f;
    ty = Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 16.0f + body, T(TXT_HUB_RECENT), NULL) + 16.0f;
    bw = Caps_Width(FONT_BOXBTN, T(TXT_HUB_SEE_ALL), TRACK_BUTTON) + 36.0f;
    Hub_Button(BTN_HUB_ALL, HUB_IN + HUB_CW - HUB_PAD - bw, y + HUB_PAD - 8.0f, T(TXT_HUB_SEE_ALL), tone.p3, tone.p4);
    Hub_GameHead(x, w, ty + 10.0f);
    for (i = 0; i < st->gameCount; i++) {
        float ry = ty + 28.0f + 54.0f * (float)i;

        if (Hub_Visible(ry, 54.0f)) {
            Hub_GameRow(&st->games[i], i, x, ry, w);
        }
    }
    return y + HUB_PAD * 2.0f + 24.0f + 16.0f + body;
}

static float Hub_Status(float y, const slot_t* s, int failText) {
    int status = *(const int*)s->shown;

    if (!Slot_Ready(s)) {
        return Hub_Note(y, T(TXT_HUB_LOADING));
    }
    return Hub_Note(y, T(status == PRO_MISSING ? TXT_PRO_MISSING : failText));
}

static float Hub_ProfileView(float y) {
    static const char* modes[HUB_MODES];
    static const char* seasons[HUB_SEASONS];
    static char seasonText[HUB_SEASONS][24];
    float bio;
    int i;

    if (!hubWho[0]) {
        return Hub_Note(y, T(TXT_PRO_MISSING));
    }
    if (!Slot_Ready(&slotPlayer) || hubPlayer.status != PRO_READY) {
        return Hub_Status(y, &slotPlayer, TXT_PRO_FAILED);
    }
    for (i = 0; i < HUB_MODES; i++) {
        modes[i] = hubModeName[i];
    }
    for (i = 0; i < HUB_SEASONS; i++) {
        SDL_strlcpy(seasonText[i], Season_Name(i), sizeof(seasonText[i]));
        seasons[i] = seasonText[i];
    }
    y = Hub_Header(y) + HUB_SECTION;
    if (hubPlayer.bio[0]) {
        bio = Para_Height(FONT_HUB_BIO, HUB_CW - HUB_PAD * 2.0f, 24.0f, hubPlayer.bio);
        Hub_Card(y, HUB_PAD * 2.0f + 24.0f + 12.0f + bio, T(TXT_HUB_ABOUT), NULL);
        Draw_Para(FONT_HUB_BIO, HUB_IN + HUB_PAD, y + HUB_PAD + 24.0f + 12.0f, HUB_CW - HUB_PAD * 2.0f, 24.0f, tone.ink, hubPlayer.bio);
        y += HUB_PAD * 2.0f + 24.0f + 12.0f + bio + HUB_SECTION;
    }
    Hub_Tabs(BTN_HUB_MODE_0, HUB_MODES, modes, hubMode, HUB_IN, y + 10.0f, FONT_HUB_TAB, 0.96f, 30.0f);
    Hub_Tabs(BTN_HUB_SEASON_0, HUB_SEASONS, seasons, hubSeason, HUB_IN, y + 44.0f, FONT_HUB_SEASON, 0.78f, 20.0f);
    y += 54.0f + 24.0f;
    if (!Slot_Ready(&slotStats) || hubStats.status != PRO_READY) {
        return Hub_Status(y, &slotStats, TXT_HUB_FAILED);
    }
    y = Hub_StatsCard(y) + HUB_SECTION;
    y = Hub_HistoryCard(y) + HUB_SECTION;
    y = Hub_IconsCard(y, false) + HUB_SECTION;
    y = Hub_IconsCard(y, true) + HUB_SECTION;
    return Hub_RecentCard(y);
}

static float Hub_Pages(float y, int page, int pages) {
    char line[48];
    float pw, nw, mid = y + 20.0f;

    if (pages <= 1) {
        return y;
    }
    pw = Hub_Button(BTN_HUB_PREV, HUB_IN + HUB_PAD, y, T(TXT_HUB_PREV), tone.p3, tone.p4);
    nw = Caps_Width(FONT_BOXBTN, T(TXT_HUB_NEXT), TRACK_BUTTON) + 36.0f;
    Hub_Button(BTN_HUB_NEXT, HUB_IN + HUB_CW - HUB_PAD - nw, y, T(TXT_HUB_NEXT), tone.p3, tone.p4);
    if (page <= 1) {
        buttons[BTN_HUB_PREV].visible = false;
    }
    if (page >= pages) {
        buttons[BTN_HUB_NEXT].visible = false;
    }
    (void)pw;
    SDL_snprintf(line, sizeof(line), T(TXT_HUB_PAGE), page, pages);
    Hub_Center(FONT_HUB_DATO, HUB_IN + HUB_CW * 0.5f, mid, tone.ink2, line);
    return y + 40.0f;
}

static float Hub_PageTitle(float y, const char* text, const char* raw) {
    float w = Caps_Width(FONT_HUB_PAGE, text, 0.52f);

    Draw_Caps(FONT_HUB_PAGE, HUB_IN, y + 16.0f, tone.ink, text, 0.52f);
    if (raw) {
        Draw_NameFit(FONT_HUB_PAGE, HUB_IN + w + 10.0f, y + 16.0f, tone.ink, Text_Upper(raw), 0.52f, HUB_CW - w - 10.0f);
    }
    return y + 32.0f;
}

static float Hub_GamesView(float y) {
    static const char* modes[HUB_MODES + 1];
    const hubGames_t* gs = &hubGames;
    const char* modeName = hubGamesMode ? hubModeName[hubGamesMode - 1] : NULL;
    char line[128], num[24];
    float x = HUB_IN + HUB_PAD, w = HUB_CW - HUB_PAD * 2.0f, ty, body;
    int i;

    if (!hubWho[0]) {
        return Hub_Note(y, T(TXT_PRO_MISSING));
    }
    modes[0] = T(TXT_HUB_ALL);
    for (i = 0; i < HUB_MODES; i++) {
        modes[i + 1] = hubModeName[i];
    }
    y = Hub_PageTitle(y, T(TXT_HUB_GAMES_OF), Slot_Ready(&slotPlayer) && hubPlayer.status == PRO_READY ? hubPlayer.raw : "");
    if (Slot_Ready(&slotGames) && gs->status == PRO_READY) {
        Num_Text(gs->total, num, sizeof(num));
        if (!gs->total) {
            if (modeName) {
                SDL_snprintf(line, sizeof(line), T(TXT_HUB_NO_GAMES), modeName);
            } else {
                SDL_strlcpy(line, T(TXT_HUB_NONE_YET), sizeof(line));
            }
        } else if (modeName) {
            SDL_snprintf(line, sizeof(line), T(gs->total == 1 ? TXT_HUB_COUNT_MODE_ONE : TXT_HUB_COUNT_MODE), num, modeName);
        } else {
            SDL_snprintf(line, sizeof(line), T(gs->total == 1 ? TXT_HUB_COUNT_ALL_ONE : TXT_HUB_COUNT_ALL), num);
        }
        Hub_Plain(FONT_HUB_BIO, HUB_IN, y + 22.0f, tone.ink2, line, 0.0f);
    }
    y += 40.0f;
    Hub_Tabs(BTN_HUB_MODE_0, HUB_MODES + 1, modes, hubGamesMode, HUB_IN, y + 10.0f, FONT_HUB_TAB, 0.96f, 30.0f);
    y += 20.0f + 24.0f;
    if (!Slot_Ready(&slotGames) || gs->status != PRO_READY) {
        return Hub_Status(y, &slotGames, TXT_HUB_FAILED);
    }
    if (!gs->count) {
        if (modeName) {
            SDL_snprintf(line, sizeof(line), T(TXT_HUB_EMPTY_MODE), modeName);
        } else {
            SDL_strlcpy(line, T(TXT_HUB_EMPTY_ALL), sizeof(line));
        }
        body = Hub_Empty(0.0f, 0.0f, w, line, false);
        Hub_Card(y, HUB_PAD * 2.0f + body, NULL, NULL);
        Hub_Empty(x, y + HUB_PAD, w, line, true);
        return y + HUB_PAD * 2.0f + body;
    }
    body = 28.0f + (float)gs->count * 54.0f + (gs->pages > 1 ? 24.0f + 40.0f : 0.0f);
    ty = Hub_Card(y, HUB_PAD * 2.0f + body, NULL, NULL);
    Hub_GameHead(x, w, ty + 10.0f);
    for (i = 0; i < gs->count; i++) {
        float ry = ty + 28.0f + 54.0f * (float)i;

        if (Hub_Visible(ry, 54.0f)) {
            Hub_GameRow(&gs->games[i], i, x, ry, w);
        }
    }
    Hub_Pages(ty + 28.0f + 54.0f * (float)gs->count + 24.0f, gs->page, gs->pages);
    return y + HUB_PAD * 2.0f + body;
}

static void Rank_Columns(float x, float w, bool clutch, float* col, float* width) {
    static const float fixedCa[9] = {52.0f, 0.0f, 144.0f, 80.0f, 104.0f, 64.0f, 48.0f, 64.0f, 56.0f};
    static const float fixed[7] = {52.0f, 0.0f, 144.0f, 80.0f, 48.0f, 64.0f, 56.0f};
    const float* f = clutch ? fixedCa : fixed;
    int n = clutch ? 9 : 7, i;
    float sum = 0.0f;

    for (i = 0; i < n; i++) {
        sum += f[i];
    }
    for (i = 0; i < n; i++) {
        width[i] = i == 1 ? w - sum - 10.0f * (float)(n - 1) : f[i];
        col[i] = i ? col[i - 1] + width[i - 1] + 10.0f : x;
    }
}

static float Hub_RankView(float y) {
    static const char* modes[HUB_RANK_MODES];
    const hubRanking_t* rk = &hubRank;
    int mode = hubRankModes[hubRankMode], labels[9], n, i, pages;
    bool clutch = mode == 0;
    float col[9], width[9], x = HUB_IN + HUB_PAD, w = HUB_CW - HUB_PAD * 2.0f, ty, body;
    char line[128], num[24];

    for (i = 0; i < HUB_RANK_MODES; i++) {
        modes[i] = hubModeName[hubRankModes[i]];
    }
    y = Hub_PageTitle(y, T(TXT_HUB_RANKING), NULL) + 16.0f;
    Hub_Tabs(BTN_HUB_MODE_0, HUB_RANK_MODES, modes, hubRankMode, HUB_IN, y + 10.0f, FONT_HUB_TAB, 0.96f, 30.0f);
    y += 20.0f + 24.0f;
    if (!Slot_Ready(&slotRank) || rk->status != PRO_READY) {
        return Hub_Status(y, &slotRank, TXT_HUB_FAILED);
    }
    if (!rk->count) {
        SDL_snprintf(line, sizeof(line), T(TXT_HUB_RANK_EMPTY), hubModeName[mode]);
        body = Hub_Empty(0.0f, 0.0f, w, line, false);
        Hub_Card(y, HUB_PAD * 2.0f + body, NULL, NULL);
        Hub_Empty(x, y + HUB_PAD, w, line, true);
        return y + HUB_PAD * 2.0f + body;
    }
    pages = (rk->total + HUB_RANK_PAGE - 1) / HUB_RANK_PAGE;
    body = 28.0f + (float)rk->count * 44.0f + (pages > 1 ? 24.0f + 40.0f : 0.0f);
    ty = Hub_Card(y, HUB_PAD * 2.0f + body, NULL, NULL);
    Rank_Columns(x + 16.0f, w - 32.0f, clutch, col, width);
    n = 0;
    labels[n++] = TXT_HUB_COL_RANK;
    labels[n++] = TXT_HUB_COL_PLAYER;
    labels[n++] = TXT_HUB_COL_GAMES;
    labels[n++] = TXT_HUB_COL_WINS;
    if (clutch) {
        labels[n++] = TXT_HUB_COL_LAST;
        labels[n++] = TXT_HUB_CLUTCH;
    }
    labels[n++] = TXT_HUB_TIER;
    labels[n++] = TXT_HUB_COL_ELO;
    labels[n++] = TXT_HUB_COL_KD;
    for (i = 0; i < n; i++) {
        if (i < 2) {
            Draw_Caps(FONT_HUB_MONO, col[i], ty + 10.0f, tone.ink2, T(labels[i]), 1.6f);
        } else {
            Hub_CapsCenter(FONT_HUB_MONO, col[i] + width[i] * 0.5f, ty + 10.0f, tone.ink2, T(labels[i]), 1.6f);
        }
    }
    for (i = 0; i < rk->count; i++) {
        const hubRank_t* r = &rk->rows[i];
        float ry = ty + 28.0f + 44.0f * (float)i, mid = ry + 22.0f, hot, nx, fade = slideFade;
        int c = 2;

        if (!Hub_Visible(ry, 44.0f)) {
            continue;
        }
        hot = Hub_Hit(HIT_PLAYER, i, r->steamId, x, ry, w, 44.0f);
        if (r->inactive) {
            slideFade = fade * (0.45f + 0.45f * hot);
        }
        if (hot > 0.0f) {
            Draw_Round(x, ry, w, 44.0f, 8.0f, Col_Alpha(tone.p3, hot));
        }
        if (r->rank > 0) {
            SDL_snprintf(line, sizeof(line), "#%d", r->rank);
            Hub_Plain(FONT_HUB_RANKNUM, col[0], mid, tone.ink, line, 0.0f);
        } else {
            Hub_Plain(FONT_HUB_RANKNUM, col[0], mid, tone.ink2, "\xe2\x80\x94", 0.0f);
        }
        nx = Hub_Badges(r->raw, r->avatar, r->country, r->clan, r->clanPic, r->clanTag, col[1], mid);
        Draw_NameFit(FONT_HUB_RANKNAME, nx, mid, tone.ink, r->raw, 0.0f, col[1] + width[1] - nx);
        SDL_snprintf(line, sizeof(line), "%d", r->games);
        Hub_Center(FONT_HUB_RANKSTAT, col[c] + width[c] * 0.5f, mid, tone.ink2, line);
        c++;
        if (r->haveWin) {
            SDL_snprintf(line, sizeof(line), "%d %%", r->winRate);
        } else {
            SDL_strlcpy(line, "\xe2\x80\x94", sizeof(line));
        }
        Hub_Center(FONT_HUB_RANKSTAT, col[c] + width[c] * 0.5f, mid, tone.ink2, line);
        c++;
        if (clutch) {
            if (r->haveLast) {
                SDL_snprintf(line, sizeof(line), "%d", r->lastAlive);
            } else {
                SDL_strlcpy(line, "\xe2\x80\x94", sizeof(line));
            }
            Hub_Center(FONT_HUB_RANKSTAT, col[c] + width[c] * 0.5f, mid, tone.ink2, line);
            c++;
            if (r->haveClutch) {
                SDL_snprintf(line, sizeof(line), "%d", r->clutches);
            } else {
                SDL_strlcpy(line, "\xe2\x80\x94", sizeof(line));
            }
            Hub_Center(FONT_HUB_RANKSTAT, col[c] + width[c] * 0.5f, mid, tone.ink2, line);
            c++;
        }
        Hub_Icon(hubTierImage[Tier_Of(r->rating)], col[c] + width[c] * 0.5f, mid, 24.0f);
        c++;
        SDL_snprintf(line, sizeof(line), "%d", r->rating);
        Hub_Center(FONT_HUB_RANKNUM, col[c] + width[c] * 0.5f, mid, tone.ink, line);
        c++;
        SDL_snprintf(num, sizeof(num), "%.2f", r->kd);
        Hub_Center(FONT_HUB_RANKSTAT, col[c] + width[c] * 0.5f, mid, tone.ink2, num);
        slideFade = fade;
    }
    Hub_Pages(ty + 28.0f + 44.0f * (float)rk->count + 24.0f, hubRankPage, pages);
    return y + HUB_PAD * 2.0f + body;
}

static float Hub_ServersView(float y) {
    float x = HUB_IN + HUB_PAD, w = HUB_CW - HUB_PAD * 2.0f, ty, head, modeX = x + 64.0f + 16.0f + (w - 80.0f) * 0.52f;
    int i;

    y = Hub_PageTitle(y, T(TXT_SERVERS_TITLE), NULL) + 24.0f;
    if (!serversLoaded) {
        return Hub_Note(y, T(serversFailed ? TXT_SERVERS_FAILED : TXT_SERVERS_LOADING));
    }
    ty = Hub_Card(y, HUB_PAD * 2.0f + 28.0f + (float)serverCount * ROW_STEP, NULL, NULL);
    head = ty + 10.0f;
    Draw_Caps(FONT_SV_META, x + 64.0f + 16.0f, head, COL_DOT, T(TXT_COL_SERVER), 1.6f);
    Draw_Caps(FONT_SV_META, modeX, head, COL_DOT, T(TXT_COL_MODE), 1.6f);
    Draw_Caps(FONT_SV_META, x + w - 8.0f - Caps_Width(FONT_SV_META, T(TXT_COL_PLAYERS), 1.6f), head, COL_DOT, T(TXT_COL_PLAYERS), 1.6f);
    for (i = 0; i < serverCount; i++) {
        const server_t* s = &servers[i];
        float ry = ty + 28.0f + ROW_STEP * (float)i, mid = ry + ROW_H * 0.5f, nx = x + 8.0f + THUMB_W + 16.0f, cw, hot;
        char line[64];

        if (!Hub_Visible(ry, ROW_H)) {
            continue;
        }
        hot = Hub_Hit(HIT_SERVER, i, NULL, x, ry, w, ROW_H);
        if (hot > 0.0f) {
            Draw_Round(x, ry, w, ROW_H, BUTTON_R, Col_Alpha(COL_P3, hot));
        }
        Draw_Thumb(s->map, x + 8.0f, mid - THUMB_H * 0.5f, THUMB_W, THUMB_H);
        Draw_Text(FONT_SV_LIST, nx, Text_Mid(FONT_SV_LIST, mid), COL_WHITE, Text_Fit(FONT_SV_LIST, s->name, modeX - nx - 20.0f), -1);
        cw = Draw_Count(x + w - 8.0f, mid, s, true, false);
        SDL_snprintf(line, sizeof(line), "%s - %s", s->mode, s->map);
        Draw_CapsFit(FONT_SV_META, modeX, mid, COL_W70, line, 1.6f, x + w - 8.0f - cw - 24.0f - modeX);
    }
    return ty + 28.0f + (float)serverCount * ROW_STEP + HUB_PAD;
}

static void Hub_ManName(const hubMan_t* man, float x, float mid, float width) {
    float nx = Hub_Badges(man->raw, man->avatar, man->country, man->clan, man->clanPic, man->clanTag, x, mid);

    Draw_NameFit(FONT_HUB_MANNAME, nx, mid, tone.ink, man->raw, 0.65f, x + width - nx);
}

static int Match_Team(const hubMatch_t* mt, int team, int* list) {
    int i, n = 0;

    for (i = 0; i < mt->count; i++) {
        if (!mt->teams || mt->men[i].team == team) {
            int k = n++;

            while (k > 0 && mt->men[list[k - 1]].score < mt->men[i].score) {
                list[k] = list[k - 1];
                k--;
            }
            list[k] = i;
        }
    }
    return n;
}

static float Match_Table(float y, int team, const int* labels, int columns, const float* widths, int kind) {
    const hubMatch_t* mt = &hubMatch;
    int list[HUB_MEN], n = Match_Team(mt, team, list), i, k;
    float x = HUB_IN + HUB_PAD, w = HUB_CW - HUB_PAD * 2.0f, col[12], fixed = 0.0f, nameW, ty, rowH = kind == SUB_WEAPONS ? 52.0f : 44.0f;
    char line[64];
    const char* title = NULL;
    const char* mark = NULL;

    if (!n) {
        return y - HUB_SECTION;
    }
    for (i = 0; i < columns; i++) {
        fixed += widths[i] + 10.0f;
    }
    nameW = w - fixed;
    col[0] = x;
    for (i = 0; i < columns; i++) {
        col[i + 1] = (i ? col[i] + widths[i - 1] + 10.0f : x + nameW + 10.0f);
    }
    if (mt->teams) {
        title = T(team == 1 ? TXT_HUB_RED : TXT_HUB_BLUE);
        mark = mt->winner == 0 ? T(TXT_HUB_TIE) : mt->winner == team ? T(TXT_HUB_WON) : mt->winner > 0 ? T(TXT_HUB_LOST) : NULL;
    }
    ty = Hub_Card(y, HUB_PAD * 2.0f + (title ? 24.0f + 16.0f : 0.0f) + 28.0f + (float)n * rowH, title, mark);
    if (title) {
        ty += 16.0f;
        Draw_Rect(HUB_IN + HUB_PAD + Caps_Width(FONT_HUB_SECTION, title, 0.8f) + 10.0f, y + HUB_PAD + 7.0f, 10.0f, 10.0f, team == 1 ? COL_RED : COL_BLUE);
    }
    Draw_Caps(FONT_HUB_MONO, x, ty + 10.0f, tone.ink2, T(TXT_HUB_COL_PLAYER), 1.6f);
    for (i = 0; i < columns; i++) {
        Hub_CapsCenter(FONT_HUB_MONO, col[i + 1] + widths[i] * 0.5f, ty + 10.0f, tone.ink2, T(labels[i]), 1.6f);
    }
    for (k = 0; k < n; k++) {
        const hubMan_t* man = &mt->men[list[k]];
        float ry = ty + 28.0f + rowH * (float)k, mid = ry + rowH * 0.5f, hot;

        if (!Hub_Visible(ry, rowH)) {
            continue;
        }
        hot = Hub_Hit(HIT_PLAYER, list[k], man->steamId, x, ry, w, rowH);
        if (hot > 0.0f) {
            Draw_Round(x, ry, w, rowH, 8.0f, Col_Alpha(tone.p3, hot));
        }
        Hub_ManName(man, x + 8.0f, mid, nameW - 8.0f);
        for (i = 0; i < columns; i++) {
            float cx = col[i + 1] + widths[i] * 0.5f;
            Uint32 c = tone.ink;

            line[0] = '\0';
            if (kind == SUB_SUMMARY) {
                switch (i) {
                    case 0:
                        SDL_snprintf(line, sizeof(line), "%d", man->score);
                        break;
                    case 1:
                        if (man->havePerf) {
                            SDL_snprintf(line, sizeof(line), "%.2f", man->perf);
                        } else {
                            SDL_strlcpy(line, "-", sizeof(line));
                        }
                        break;
                    case 2:
                        SDL_snprintf(line, sizeof(line), "%d", man->kills);
                        break;
                    case 3:
                        SDL_snprintf(line, sizeof(line), "%d", man->deaths);
                        break;
                    case 4:
                        Num_Text(man->damage, line, sizeof(line));
                        break;
                    case 5:
                        SDL_snprintf(line, sizeof(line), "%d%%", man->aim);
                        break;
                    case 6:
                        SDL_snprintf(line, sizeof(line), "%d", man->elo);
                        break;
                    default:
                        if (mt->counts && man->eloChange) {
                            SDL_snprintf(line, sizeof(line), "%+d", man->eloChange);
                            c = man->eloChange > 0 ? COL_OK : COL_MAL;
                        } else {
                            SDL_strlcpy(line, "-", sizeof(line));
                            c = tone.ink2;
                        }
                        break;
                }
                Hub_Center(FONT_HUB_NUM, cx, mid, c, line);
            } else if (kind == SUB_WEAPONS) {
                if (i == columns - 1) {
                    Num_Text(man->damage, line, sizeof(line));
                    Hub_Center(FONT_HUB_NUM, cx, mid, tone.ink, line);
                } else {
                    const hubWeapon_t* wp = &man->weapons[hubMatchWeapons[i]];

                    if (!wp->used) {
                        Hub_Center(FONT_HUB_GLOBE, cx, mid, tone.ink2, T(TXT_HUB_UNUSED));
                    } else {
                        SDL_snprintf(line, sizeof(line), "%d%%", (int)SDL_floorf(wp->acc + 0.5f));
                        Hub_Center(FONT_HUB_NUMB, cx, mid - 8.0f, tone.ink, line);
                        SDL_snprintf(line, sizeof(line), T(TXT_HUB_HITS), wp->hits, wp->shots);
                        Hub_Center(FONT_HUB_GLOBE, cx, mid + 9.0f, tone.ink2, line);
                    }
                }
            } else {
                SDL_snprintf(line, sizeof(line), "%d", man->flags[i]);
                Hub_Center(FONT_HUB_NUM, cx, mid, man->flags[i] ? tone.ink : tone.ink2, line);
            }
        }
    }
    return ty + 28.0f + rowH * (float)n + HUB_PAD;
}

static float Match_Medals(float y, int team) {
    const hubMatch_t* mt = &hubMatch;
    int list[HUB_MEN], n = Match_Team(mt, team, list), i, k;
    float x = HUB_IN + HUB_PAD, w = HUB_CW - HUB_PAD * 2.0f, nameW = 280.0f, h = 0.0f, ty;
    float rows[HUB_MEN];
    const char* title = mt->teams ? T(team == 1 ? TXT_HUB_RED : TXT_HUB_BLUE) : NULL;
    char line[48];

    if (!n) {
        return y - HUB_SECTION;
    }
    for (k = 0; k < n; k++) {
        const hubMan_t* man = &mt->men[list[k]];
        float px = 0.0f, lines = 1.0f;
        bool any = false;

        for (i = 0; i < 16; i++) {
            int m = hubMatchMedals[i];
            float pw;

            if (man->medal[m] <= 0) {
                continue;
            }
            SDL_snprintf(line, sizeof(line), "%s %d", hubMedalName[m], man->medal[m]);
            pw = 10.0f + 22.0f + 6.0f + Text_Width(FONT_HUB_GLOBE, line, -1) + 12.0f;
            if (px + pw > w - nameW && px > 0.0f) {
                px = 0.0f;
                lines += 1.0f;
            }
            px += pw + 6.0f;
            any = true;
        }
        rows[k] = SDL_max(44.0f, lines * 36.0f + 8.0f);
        (void)any;
        h += rows[k];
    }
    ty = Hub_Card(y, HUB_PAD * 2.0f + (title ? 24.0f + 16.0f : 0.0f) + h, title, NULL) + (title ? 16.0f : 0.0f);
    if (title) {
        Draw_Rect(HUB_IN + HUB_PAD + Caps_Width(FONT_HUB_SECTION, title, 0.8f) + 10.0f, y + HUB_PAD + 7.0f, 10.0f, 10.0f, team == 1 ? COL_RED : COL_BLUE);
    }
    for (k = 0; k < n; k++) {
        const hubMan_t* man = &mt->men[list[k]];
        float px = x + nameW, py = ty + 4.0f, hot;
        bool any = false;

        if (!Hub_Visible(ty, rows[k])) {
            ty += rows[k];
            continue;
        }
        hot = Hub_Hit(HIT_PLAYER, list[k], man->steamId, x, ty, w, rows[k]);
        if (hot > 0.0f) {
            Draw_Round(x, ty, w, rows[k], 8.0f, Col_Alpha(tone.p3, hot));
        }
        Hub_ManName(man, x + 8.0f, ty + 22.0f, nameW - 20.0f);
        for (i = 0; i < 16; i++) {
            int m = hubMatchMedals[i];
            float pw;

            if (man->medal[m] <= 0) {
                continue;
            }
            SDL_snprintf(line, sizeof(line), "%s %d", hubMedalName[m], man->medal[m]);
            pw = 10.0f + 22.0f + 6.0f + Text_Width(FONT_HUB_GLOBE, line, -1) + 12.0f;
            if (px + pw > x + w && px > x + nameW) {
                px = x + nameW;
                py += 36.0f;
            }
            Draw_Round(px, py + 4.0f, pw, 28.0f, 14.0f, tone.p3);
            Hub_Icon(hubMedalIcon[m], px + 10.0f + 11.0f, py + 18.0f, 22.0f);
            Hub_Plain(FONT_HUB_GLOBE, px + 38.0f, py + 18.0f, tone.ink, line, 0.0f);
            px += pw + 6.0f;
            any = true;
        }
        if (!any) {
            Hub_Plain(FONT_HUB_GLOBE, x + nameW, ty + 22.0f, tone.ink2, T(TXT_HUB_NO_MATCH_MEDALS), 0.0f);
        }
        ty += rows[k];
    }
    return ty + HUB_PAD;
}

static float Hub_MatchView(float y) {
    static const int sumLabels[8] = {TXT_HUB_COL_SCORE, TXT_HUB_COL_PERF, TXT_HUB_COL_FRAGS, TXT_HUB_COL_DEATHS, TXT_HUB_COL_DAMAGE, TXT_HUB_COL_ACC, TXT_HUB_COL_ELO, TXT_HUB_COL_DIFF};
    static const float sumWidths[8] = {72.0f, 104.0f, 64.0f, 72.0f, 72.0f, 80.0f, 72.0f, 104.0f};
    static const int gunLabels[6] = {TXT_HUB_W_RL, TXT_HUB_W_LG, TXT_HUB_W_RG, TXT_HUB_W_SG, TXT_HUB_W_MG, TXT_HUB_COL_DAMAGE};
    static const float gunWidths[6] = {104.0f, 104.0f, 104.0f, 104.0f, 104.0f, 80.0f};
    static const int flagLabels[5] = {TXT_HUB_CAPTURED, TXT_HUB_RETURNED, TXT_HUB_TOOK, TXT_HUB_DROPPED, TXT_HUB_CARRIER};
    static const float flagWidths[5] = {96.0f, 96.0f, 96.0f, 96.0f, 144.0f};
    static const char* subs[4];
    const hubMatch_t* mt = &hubMatch;
    char line[200];
    float cover = SDL_floorf(HUB_CW * 250.0f / 1000.0f), cx = HUB_IN + HUB_CW * 0.5f, cy, bw;
    int nsubs, team, list[HUB_MEN];

    bw = Caps_Width(FONT_HUB_LINK, T(TXT_HUB_BACK), 0.6f);
    Draw_Caps(FONT_HUB_LINK, HUB_IN, y + 8.0f, Col_Mix(tone.ink2, tone.ink, hubLive ? heat[BTN_HUB_BACK] : 0.0f), T(TXT_HUB_BACK), 0.6f);
    Hub_Set(BTN_HUB_BACK, HUB_IN - 6.0f, y - 6.0f, bw + 12.0f, 28.0f);
    y += 32.0f;
    if (!Slot_Ready(&slotMatch) || mt->status != PRO_READY) {
        return Hub_Status(y, &slotMatch, TXT_HUB_FAILED);
    }
    Draw_CapsFit(FONT_HUB_PAGE, HUB_IN, y + 16.0f, tone.ink, mt->title, 0.52f, HUB_CW);
    y += 32.0f + 16.0f;
    if (!mt->counts) {
        int why = !SDL_strcmp(mt->why, "aborted") ? TXT_HUB_WHY_ABORTED : !SDL_strcmp(mt->why, "few") ? TXT_HUB_WHY_FEW : !SDL_strcmp(mt->why, "short") ? TXT_HUB_WHY_SHORT : TXT_HUB_WHY_RATED;
        float tw = Text_Width(FONT_HUB_GLOBEB, T(TXT_HUB_NOT_RATED), -1);

        Draw_Round(HUB_IN, y, HUB_CW, 40.0f, 8.0f, tone.p2);
        Hub_Plain(FONT_HUB_GLOBEB, HUB_IN + 16.0f, y + 20.0f, tone.ink, T(TXT_HUB_NOT_RATED), 0.0f);
        Hub_Plain(FONT_HUB_GLOBE, HUB_IN + 16.0f + tw, y + 20.0f, tone.ink2, T(why), 0.0f);
        y += 40.0f + 16.0f;
    }
    Hub_Shot(mt->map, HUB_IN, y, HUB_CW, cover, HUB_R, 1.0f);
    Draw_Round(HUB_IN, y, HUB_CW, cover, HUB_R, 0x00000094);
    cy = y + cover * 0.5f;
    if (mt->teams) {
        SDL_snprintf(line, sizeof(line), "%d", mt->red);
        Draw_Text(FONT_HUB_SCORE, cx - 40.0f - Text_Width(FONT_HUB_SCORE, line, -1), Text_Mid(FONT_HUB_SCORE, cy + 12.0f),
                  Col_Alpha(COL_WHITE, mt->winner == 1 ? 1.0f : 0.5f), line, -1);
        SDL_snprintf(line, sizeof(line), "%d", mt->blue);
        Draw_Text(FONT_HUB_SCORE, cx + 40.0f, Text_Mid(FONT_HUB_SCORE, cy + 12.0f), Col_Alpha(COL_WHITE, mt->winner == 2 ? 1.0f : 0.5f), line, -1);
        bw = Caps_Width(FONT_HUB_SEASON, T(TXT_HUB_RED), 0.78f);
        Draw_Rect(cx - 40.0f - bw - 16.0f, cy - 42.0f - 5.0f, 10.0f, 10.0f, COL_RED);
        Draw_Caps(FONT_HUB_SEASON, cx - 40.0f - bw, cy - 42.0f, COL_WHITE, T(TXT_HUB_RED), 0.78f);
        Draw_Rect(cx + 40.0f, cy - 42.0f - 5.0f, 10.0f, 10.0f, COL_BLUE);
        Draw_Caps(FONT_HUB_SEASON, cx + 56.0f, cy - 42.0f, COL_WHITE, T(TXT_HUB_BLUE), 0.78f);
    } else if (mt->count == 2) {
        int a = mt->men[0].score >= mt->men[1].score ? 0 : 1;
        char clean[96];

        Server_Name(mt->men[0].raw, clean, sizeof(clean));

        SDL_snprintf(line, sizeof(line), "%d", mt->men[0].score);
        Draw_Text(FONT_HUB_SCORE, cx - 40.0f - Text_Width(FONT_HUB_SCORE, line, -1), Text_Mid(FONT_HUB_SCORE, cy + 12.0f),
                  Col_Alpha(COL_WHITE, a == 0 ? 1.0f : 0.5f), line, -1);
        SDL_snprintf(line, sizeof(line), "%d", mt->men[1].score);
        Draw_Text(FONT_HUB_SCORE, cx + 40.0f, Text_Mid(FONT_HUB_SCORE, cy + 12.0f), Col_Alpha(COL_WHITE, a == 1 ? 1.0f : 0.5f), line, -1);
        bw = SDL_min(HUB_CW * 0.5f - 80.0f, 320.0f);
        Draw_NameFit(FONT_HUB_SEASON, cx - 40.0f - SDL_min(bw, Text_Width(FONT_HUB_SEASON, clean, -1)), cy - 42.0f, COL_WHITE, mt->men[0].raw, 1.04f, bw);
        Draw_NameFit(FONT_HUB_SEASON, cx + 40.0f, cy - 42.0f, COL_WHITE, mt->men[1].raw, 1.04f, bw);
    } else if (Match_Team(mt, 0, list) > 0) {
        char clean[96];

        Server_Name(mt->men[list[0]].raw, clean, sizeof(clean));
        SDL_snprintf(line, sizeof(line), T(TXT_HUB_WINNER), clean);
        Hub_Center(FONT_HUB_PAGE, cx, cy, COL_WHITE, line);
    }
    y += cover + 24.0f;
    nsubs = 0;
    subs[nsubs++] = T(TXT_HUB_SUMMARY);
    subs[nsubs++] = T(TXT_HUB_WEAPONS);
    subs[nsubs++] = T(TXT_HUB_MEDALS);
    if (!SDL_strcasecmp(mt->mode, "ctf")) {
        subs[nsubs++] = T(TXT_HUB_FLAGS);
    }
    if (hubSub >= nsubs) {
        hubSub = SUB_SUMMARY;
    }
    Hub_Tabs(BTN_HUB_SUB_0, nsubs, subs, hubSub, HUB_IN, y + 10.0f, FONT_HUB_TAB, 0.96f, 30.0f);
    y += 20.0f + 24.0f;
    for (team = mt->teams ? 1 : 0; team <= (mt->teams ? 2 : 0); team++) {
        if (hubSub == SUB_SUMMARY) {
            y = Match_Table(y, team, sumLabels, 8, sumWidths, SUB_SUMMARY) + HUB_SECTION;
        } else if (hubSub == SUB_WEAPONS) {
            y = Match_Table(y, team, gunLabels, 6, gunWidths, SUB_WEAPONS) + HUB_SECTION;
        } else if (hubSub == SUB_MEDALS) {
            y = Match_Medals(y, team) + HUB_SECTION;
        } else {
            y = Match_Table(y, team, flagLabels, 5, flagWidths, SUB_FLAGS) + HUB_SECTION;
        }
    }
    return y - HUB_SECTION;
}

static int Hub_View(void) {
    return hubTab == HUB_MATCHES && hubMatchId[0] ? 4 : hubTab;
}

static void Hub_Head(void) {
    static const int names[4] = {TXT_HUB_PROFILE, TXT_HUB_MATCHES, TXT_HUB_RANKING, TXT_HUB_SERVERS};
    float x = HUB_X + 24.0f, mid = HUB_Y + HEAD_H * 0.5f, w = Text_Width(FONT_TEXT, T(TXT_CLOSE), -1);
    int i;

    for (i = 0; i < 4; i++) {
        float tw = Caps_Width(FONT_PANEL, T(names[i]), TRACK_TAB);
        Uint32 c = i == hubTab ? COL_WHITE : Col_Mix(COL_W70, COL_WHITE, hubLive ? heat[BTN_HUB_TAB_0 + i] : 0.0f);

        Draw_Caps(FONT_PANEL, x, mid, c, T(names[i]), TRACK_TAB);
        if (i == hubTab) {
            Draw_Rect(x, HUB_Y + HEAD_H - 3.0f, tw, 3.0f, COL_WHITE);
        }
        Button_Set(BTN_HUB_TAB_0 + i, x - 10.0f, HUB_Y, tw + 20.0f, HEAD_H, hubLive);
        x += tw + 34.0f;
    }
    Draw_Link(BTN_HUB_CLOSE, HUB_X + HUB_W - 24.0f - w, mid, T(TXT_CLOSE));
    if (!hubLive) {
        buttons[BTN_HUB_CLOSE].visible = false;
    }
}

static void Hub_Draw(void) {
    float k = hubHeat, e, rise, top, bottom, y, content = 0.0f;
    int v = Hub_View();
    SDL_Rect clip;

    hubHitCount = 0;
    hubLive = false;
    if (k <= 0.4f || !hubFonts) {
        return;
    }
    e = (k - 0.4f) / 0.6f;
    e = 1.0f - (1.0f - e) * (1.0f - e) * (1.0f - e) * (1.0f - e);
    rise = (1.0f - e) * (UI_H - HUB_Y + 24.0f);
    hubLive = k >= 1.0f && hubOpen && !boxOpen && !optsOpen;
    View_Shift(rise);
    Hub_Tone(v == HUB_PROFILE && Slot_Ready(&slotPlayer) && hubPlayer.status == PRO_READY ? hubPlayer.hue : -1);
    Draw_Shadow(HUB_X, HUB_Y, HUB_W, UI_H - HUB_Y + PANEL_R * 2.0f, PANEL_R);
    Draw_Round(HUB_X, HUB_Y, HUB_W, UI_H - HUB_Y + PANEL_R * 2.0f, PANEL_R, tone.p1);
    Draw_Round(HUB_X, HUB_Y, HUB_W, HEAD_H + PANEL_R * 2.0f, PANEL_R, COL_P3);
    Draw_Rect(HUB_X, HUB_BODY, HUB_W, PANEL_R * 2.0f, tone.p1);
    Draw_Rect(HUB_X, HUB_BODY, HUB_W, 2.0f, COL_LINE);
    Button_Set(BTN_HUB, HUB_X, HUB_Y, HUB_W, UI_H - HUB_Y, hubLive);
    hubClipTop = 0.0f;
    hubClipBottom = UI_H;
    Hub_Head();
    top = HUB_BODY + 2.0f;
    bottom = UI_H;
    clip.x = (int)(HUB_X * uiScale);
    clip.y = (int)(top * uiScale);
    clip.w = (int)(HUB_W * uiScale);
    clip.h = (int)((bottom - top) * uiScale) + 1;
    SDL_SetRenderClipRect(renderer, &clip);
    hubClipTop = top;
    hubClipBottom = bottom;
    y = top + 24.0f - hubView[v];
    switch (v) {
        case HUB_PROFILE:
            content = Hub_ProfileView(y) - y;
            break;
        case HUB_MATCHES:
            content = Hub_GamesView(y) - y;
            break;
        case HUB_RANKING:
            content = Hub_RankView(y) - y;
            break;
        case HUB_SERVERS:
            content = Hub_ServersView(y) - y;
            break;
        default:
            content = Hub_MatchView(y) - y;
            break;
    }
    SDL_SetRenderClipRect(renderer, NULL);
    hubClipTop = 0.0f;
    hubClipBottom = UI_H;
    hubMost[v] = SDL_max(0.0f, content + 48.0f - (bottom - top));
    if (hubMost[v] > 0.0f) {
        float track = bottom - top - 16.0f, thumb = SDL_max(40.0f, track * track / (track + hubMost[v]));

        Draw_Round(HUB_X + HUB_W - 10.0f, top + 8.0f + (track - thumb) * SDL_clamp(hubView[v] / hubMost[v], 0.0f, 1.0f), 3.0f, thumb, 1.5f, COL_DOT);
    }
    View_Shift(0.0f);
}

static void Hub_Pics(void) {
    int i;

    if (!hubOpen) {
        return;
    }
    if (Hub_View() == HUB_PROFILE && Slot_Ready(&slotPlayer) && hubPlayer.status == PRO_READY) {
        const hubPlayer_t* pl = &hubPlayer;

        Pic_Want("A:", pl->avatar);
        Pic_Want("f:", pl->country);
        Pic_Want("c:", pl->clan ? (pl->flag[0] ? pl->flag : CLAN_DEFAULT) : "");
        Pic_Want("B:", pl->banner);
        if (Slot_Ready(&slotStats) && hubStats.status == PRO_READY) {
            const hubStats_t* st = &hubStats;
            int elo = hubSeason < HUB_SEASONS - 1 ? st->elo : pl->rating[hubMode];

            Pic_Want("i:", hubTierImage[Tier_Of(elo)]);
            for (i = 0; i < st->weaponCount; i++) {
                Pic_Want("i:", hubWeaponIcon[st->weaponOrder[i]]);
            }
            for (i = 0; i < st->medalCount; i++) {
                Pic_Want("i:", hubMedalIcon[st->medalOrder[i]]);
            }
        }
    }
    if (Hub_View() == HUB_RANKING && Slot_Ready(&slotRank) && hubRank.status == PRO_READY) {
        float first = hubView[HUB_RANKING] - 260.0f;
        int start = SDL_max(0, (int)(first / 44.0f));

        for (i = 0; i < 8; i++) {
            Pic_Want("i:", hubTierImage[i]);
        }
        for (i = start; i < SDL_min(hubRank.count, start + 16); i++) {
            const hubRank_t* r = &hubRank.rows[i];

            Pic_Want("a:", r->avatar);
            Pic_Want("f:", r->country);
            Pic_Want("c:", r->clan ? (r->clanPic[0] ? r->clanPic : CLAN_DEFAULT) : "");
        }
    }
    if (Hub_View() == 4 && Slot_Ready(&slotMatch) && hubMatch.status == PRO_READY) {
        for (i = 0; i < hubMatch.count; i++) {
            const hubMan_t* man = &hubMatch.men[i];

            Pic_Want("a:", man->avatar);
            Pic_Want("f:", man->country);
            Pic_Want("c:", man->clan ? (man->clanPic[0] ? man->clanPic : CLAN_DEFAULT) : "");
        }
        for (i = 0; i < 16; i++) {
            Pic_Want("i:", hubMedalIcon[i]);
        }
    }
}

static void Hub_WantShot(const char* map) {
    if (map[0] && Shot_Name(map)) {
        Shots_Want(map);
    }
}

static void Hub_Shots(void) {
    int i;

    if (!hubOpen) {
        return;
    }
    if (Hub_View() == HUB_PROFILE && Slot_Ready(&slotStats) && hubStats.status == PRO_READY) {
        if (hubStats.favMap[0]) {
            Hub_WantShot(hubStats.favMap);
        }
        for (i = 0; i < hubStats.gameCount; i++) {
            Hub_WantShot(hubStats.games[i].map);
        }
    }
    if (Hub_View() == HUB_MATCHES && Slot_Ready(&slotGames) && hubGames.status == PRO_READY) {
        for (i = 0; i < hubGames.count; i++) {
            Hub_WantShot(hubGames.games[i].map);
        }
    }
    if (Hub_View() == 4 && Slot_Ready(&slotMatch) && hubMatch.status == PRO_READY) {
        Hub_WantShot(hubMatch.map);
    }
}

static void Hub_Close(void) {
    hubOpen = false;
}

static bool Hub_Press(int index) {
    int v = Hub_View();

    if (index >= BTN_HUB_TAB_0 && index < BTN_HUB_TAB_0 + 4) {
        int tab = index - BTN_HUB_TAB_0;

        if (tab == HUB_MATCHES && hubTab == HUB_MATCHES && hubMatchId[0]) {
            hubMatchId[0] = '\0';
        }
        Hub_Open(tab, NULL);
        return true;
    }
    if (index >= BTN_HUB_MODE_0 && index < BTN_HUB_MODE_0 + HUB_MODES + 1) {
        int i = index - BTN_HUB_MODE_0;

        if (v == HUB_PROFILE && i < HUB_MODES) {
            hubMode = i;
        } else if (v == HUB_MATCHES) {
            hubGamesMode = i;
            hubPage = 1;
            hubGoal[HUB_MATCHES] = 0.0f;
        } else if (v == HUB_RANKING && i < HUB_RANK_MODES) {
            hubRankMode = i;
            hubRankPage = 1;
            hubGoal[HUB_RANKING] = 0.0f;
        }
        return true;
    }
    if (index >= BTN_HUB_SEASON_0 && index < BTN_HUB_SEASON_0 + HUB_SEASONS) {
        hubSeason = index - BTN_HUB_SEASON_0;
        return true;
    }
    if (index >= BTN_HUB_SUB_0 && index < BTN_HUB_SUB_0 + 4) {
        hubSub = index - BTN_HUB_SUB_0;
        return true;
    }
    if (index >= BTN_HUB_ROW_0 && index < BTN_HUB_ROW_0 + HUB_ROWS) {
        const hubHit_t* hit = &hubHits[index - BTN_HUB_ROW_0];

        if (index - BTN_HUB_ROW_0 >= hubHitCount) {
            return true;
        }
        if (hit->kind == HIT_MATCH) {
            hubFrom = v == HUB_PROFILE ? HUB_PROFILE : HUB_MATCHES;
            Hub_Match(hit->id);
        } else if (hit->kind == HIT_PLAYER && Steam_IdValid(hit->id)) {
            Hub_Open(HUB_PROFILE, hit->id);
            hubGoal[HUB_PROFILE] = hubView[HUB_PROFILE] = 0.0f;
        } else if (hit->kind == HIT_SERVER && hit->index < serverCount) {
            Box_Open(&servers[hit->index]);
        }
        return true;
    }
    switch (index) {
        case BTN_HUB_CLOSE:
            Hub_Close();
            return true;
        case BTN_HUB_SITE: {
            char url[128];

            SDL_snprintf(url, sizeof(url), SITE "/perfil/%s", hubWho);
            SDL_OpenURL(url);
            return true;
        }
        case BTN_HUB_ALL:
            hubGamesMode = hubMode + 1;
            hubPage = 1;
            hubGoal[HUB_MATCHES] = hubView[HUB_MATCHES] = 0.0f;
            hubMatchId[0] = '\0';
            hubTab = HUB_MATCHES;
            return true;
        case BTN_HUB_RANKLINK: {
            int i;

            for (i = 0; i < HUB_RANK_MODES; i++) {
                if (hubRankModes[i] == hubMode) {
                    hubRankMode = i;
                    hubRankPage = 1;
                    hubGoal[HUB_RANKING] = hubView[HUB_RANKING] = 0.0f;
                }
            }
            hubTab = HUB_RANKING;
            return true;
        }
        case BTN_HUB_PREV:
        case BTN_HUB_NEXT:
            if (v == HUB_MATCHES) {
                hubPage = SDL_max(1, hubPage + (index == BTN_HUB_NEXT ? 1 : -1));
                hubGoal[HUB_MATCHES] = hubView[HUB_MATCHES] = 0.0f;
            } else if (v == HUB_RANKING) {
                hubRankPage = SDL_max(1, hubRankPage + (index == BTN_HUB_NEXT ? 1 : -1));
                hubGoal[HUB_RANKING] = hubView[HUB_RANKING] = 0.0f;
            }
            return true;
        case BTN_HUB_BACK:
            hubMatchId[0] = '\0';
            hubTab = hubFrom;
            return true;
        case BTN_HUB:
            return true;
        default:
            return false;
    }
}

static void Hub_Key(SDL_Keycode key) {
    int v = Hub_View();

    if (key == SDLK_ESCAPE) {
        if (v == 4) {
            hubMatchId[0] = '\0';
            hubTab = hubFrom;
        } else {
            Hub_Close();
        }
    } else if (key == SDLK_DOWN || key == SDLK_UP) {
        hubGoal[v] = SDL_max(0.0f, hubGoal[v] + (key == SDLK_DOWN ? 72.0f : -72.0f));
    } else if (key == SDLK_PAGEDOWN || key == SDLK_PAGEUP) {
        hubGoal[v] = SDL_max(0.0f, hubGoal[v] + (key == SDLK_PAGEDOWN ? 400.0f : -400.0f));
    } else if (key == SDLK_HOME) {
        hubGoal[v] = 0.0f;
    } else if (key == SDLK_END) {
        hubGoal[v] = hubMost[v];
    }
}

static void Hub_Wheel(float amount) {
    int v = Hub_View();

    hubGoal[v] = SDL_clamp(hubGoal[v] - amount * 72.0f, 0.0f, SDL_max(hubMost[v], 0.0f));
}

static void Draw_Content(void) {
    Buttons_Hide(BTN_NEXT, BTN_OPT_B);
    Draw_Feed(notesHeat);
    Draw_Card();
}

static void Draw_Frame(void) {
    float veil = (1.0f - introHeat) * (1.0f - introHeat), drop = SDL_min(1.0f, hubHeat / 0.45f);

    Draw_Page();
    Draw_Art();
    Buttons_Hide(BTN_SIDE_0, BTN_COUNT - 1);
    buttons[BTN_SERVERS].visible = false;
    if (drop < 1.0f) {
        View_Shift(drop * drop * (UI_H - PANEL_LOW + 30.0f));
        Draw_Panel();
        if (install.active || setup.step != STEP_NONE) {
            notesHeight = notesView = 0.0f;
            if (install.active) {
                Draw_Install();
            } else {
                Draw_Setup();
            }
        } else {
            Draw_Content();
        }
        View_Shift(0.0f);
    }
    if (hubHeat > 0.0f) {
        Buttons_Hide(BTN_PLAY, BTN_OPT_B);
        Buttons_Hide(BTN_SIDE_0, BTN_SIDE_LAST);
    }
    Hub_Draw();
    Draw_Top();
    Draw_Me();
    Draw_Status();
    Draw_Options();
    if (boxOpen) {
        Buttons_Hide(BTN_PLAY, BTN_OPTS);
        Buttons_Hide(BTN_MENU_0, BTN_SIDE_LAST);
    }
    Draw_Box();
    if (veil > 0.0f) {
        Draw_Rect(0.0f, 0.0f, UI_W, UI_H, Col_Alpha(COL_BLACK, veil));
    }
}

static bool Ease(float* value, float goal, float step) {
    float d = goal - *value;

    if (SDL_fabsf(d) <= step) {
        *value = goal;
        return false;
    }
    *value += d > 0.0f ? step : -step;
    return true;
}

static bool Hub_Update(float dt) {
    bool moving = Ease(&hubHeat, hubOpen ? 1.0f : 0.0f, dt * 1.7f);
    int i;

    for (i = 0; i < 5; i++) {
        float d;

        hubGoal[i] = SDL_clamp(hubGoal[i], 0.0f, SDL_max(hubMost[i], 0.0f));
        d = hubGoal[i] - hubView[i];
        if (SDL_fabsf(d) < 0.5f) {
            hubView[i] = hubGoal[i];
        } else {
            hubView[i] += d * SDL_min(1.0f, dt * 16.0f);
            moving = true;
        }
    }
    return moving;
}

static bool Heat_Update(float dt, bool notes) {
    bool moving = false;
    float d;
    int i;

    if (dt > 0.05f) {
        dt = 0.05f;
    }
    for (i = 0; i < BTN_COUNT; i++) {
        moving |= Ease(&heat[i], hotButton == i ? 1.0f : 0.0f, dt * 9.0f);
    }
    if (navReady && (SDL_fabsf(navGoalX - navX) > 0.25f || SDL_fabsf(navGoalW - navW) > 0.25f)) {
        float k = SDL_min(1.0f, dt * 14.0f);

        navX += (navGoalX - navX) * k;
        navW += (navGoalW - navW) * k;
        moving = true;
    } else {
        navX = navGoalX;
        navW = navGoalW;
    }
    moving |= Ease(&optsHeat, optsOpen ? 1.0f : 0.0f, dt * 7.0f);
    moving |= Hub_Update(dt);
    for (i = 0; i < shotCount; i++) {
        moving |= Ease(&shots[i].heat, 1.0f, dt * 4.0f);
    }
    for (i = 0; i < picCount; i++) {
        if (pics[i].img.tex) {
            moving |= Ease(&pics[i].heat, 1.0f, dt * 5.0f);
        }
    }
    moving |= Ease(&boxHeat, boxOpen ? 1.0f : 0.0f, dt * 6.0f);
    d = boxGoal - boxView;
    if (SDL_fabsf(d) < 0.5f) {
        boxView = boxGoal;
    } else {
        boxView += d * SDL_min(1.0f, dt * 16.0f);
        moving = true;
    }
    moving |= Ease(&introHeat, 1.0f, dt * 3.0f);
    moving |= Ease(&notesHeat, notes ? 1.0f : 0.0f, dt * 4.0f);
    if (stepHeat < 1.0f) {
        stepHeat = SDL_min(1.0f, (float)(SDL_GetTicksNS() - stepStart) / 500000000.0f);
        moving = true;
    }
    d = notesGoal - notesScroll;
    if (SDL_fabsf(d) < 0.5f) {
        notesScroll = notesGoal;
    } else {
        notesScroll += d * SDL_min(1.0f, dt * 16.0f);
        moving = true;
    }
    return moving;
}

static void Hot_Set(int index) {
    if (index == hotButton) {
        return;
    }
    hotButton = index;
    if (cursorArrow && cursorHand) {
        SDL_SetCursor(index >= 0 && index != BTN_HUB ? cursorHand : cursorArrow);
    }
}

static bool Hover_Poll(void) {
    float gx, gy;
    int wx, wy, ww, wh;

    SDL_GetGlobalMouseState(&gx, &gy);
    SDL_GetWindowPosition(window, &wx, &wy);
    SDL_GetWindowSize(window, &ww, &wh);
    if (gx < wx || gy < wy || gx >= wx + ww || gy >= wy + wh ||
        !(SDL_GetWindowFlags(window) & (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS)) ||
        (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
        hubMouseX = hubMouseY = -1.0f;
        Hot_Set(-1);
        return false;
    }
    hubMouseX = (gx - wx) / uiScale;
    hubMouseY = (gy - wy) / uiScale;
    Hot_Set(Button_At((gx - wx) / uiScale, (gy - wy) / uiScale));
    return true;
}

#ifdef _WIN32
static void Window_Round(void) {
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    int corner = 2;

    if (hwnd) {
        DwmSetWindowAttribute(hwnd, 33, &corner, sizeof(corner));
    }
}
#endif

static SDL_HitTestResult SDLCALL Window_HitTest(SDL_Window* win, const SDL_Point* point, void* data) {
    float x = point->x / uiScale, y = point->y / uiScale;

    (void)win;
    (void)data;
    if (optsOpen || Button_At(x, y) >= 0) {
        return SDL_HITTEST_NORMAL;
    }
    if (notesHeight > notesView + 0.5f && x >= COL_X && x < SIDE_X - 24.0f && y >= BODY_TOP) {
        return SDL_HITTEST_NORMAL;
    }
    return SDL_HITTEST_DRAGGABLE;
}

static void Notes_Scroll(float amount) {
    float max = notesHeight - notesView;

    notesGoal += amount;
    if (notesGoal > max) {
        notesGoal = max;
    }
    if (notesGoal < 0.0f) {
        notesGoal = 0.0f;
    }
}

static bool Feed_Grab(float x, float y) {
    if (setup.step != STEP_NONE || install.active || notesHeight <= notesView + 0.5f || x < COL_X ||
        x >= SIDE_X - 24.0f || y < notesTop || y >= notesLow) {
        return false;
    }
    dragMode = x >= IN_X + TEXT_W - 20.0f && y >= thumbY && y < thumbY + thumbH ? 2 : 1;
    dragStart = y;
    dragFrom = notesGoal;
    SDL_CaptureMouse(true);
    return true;
}

static void Feed_Drag(float y) {
    float d = y - dragStart;

    if (dragMode == 2) {
        d *= (notesHeight - notesView) / SDL_max(1.0f, notesView - thumbH);
    } else {
        d = -d;
    }
    notesGoal = dragFrom;
    Notes_Scroll(d);
    notesScroll = notesGoal;
}

static void Option_Pick(int i) {
    switch (i) {
        case 0:
            Folder_Game();
            break;
        case 1:
            hubOpen = false;
            hubHeat = 0.0f;
            Setup_Rerun();
            break;
        case 2:
            Repair_Start();
            break;
        case 3:
            Check_Again();
            break;
        case 4:
            Beta_Set(!betaChannel);
            optsOpen = true;
            break;
        default:
            Autostart_Set(!startupOn);
            startupOn = Autostart_On();
            optsOpen = true;
            break;
    }
}

static void Box_Open(const server_t* s) {
    boxServer = *s;
    boxOpen = true;
    boxGoal = boxView = 0.0f;
    liveLoaded = liveFailed = false;
    liveCount = 0;
    liveAgain = true;
    Live_Poll();
}

static bool Box_Connect(void) {
    if (!Play_Ready()) {
        return false;
    }
    SDL_strlcpy(connectTo, boxServer.addr, sizeof(connectTo));
    if (Game_Start()) {
        return true;
    }
    connectTo[0] = '\0';
    return false;
}

static void Box_See(void) {
    char url[128];
    size_t i;

    SDL_snprintf(url, sizeof(url), SITE_SERVER "%s", boxServer.addr);
    for (i = SDL_strlen(SITE_SERVER); url[i]; i++) {
        if (url[i] == '.' || url[i] == ':') {
            url[i] = '-';
        }
    }
    SDL_OpenURL(url);
}

static bool Button_Press(int index) {
    bool wasOpts = optsOpen;

    optsOpen = false;
    if (index >= BTN_MENU_0 && index <= BTN_MENU_LAST) {
        if (wasOpts) {
            Option_Pick(index - BTN_MENU_0);
        }
        return false;
    }
    if (index >= BTN_HUB_TAB_0 && index <= BTN_HUB) {
        Hub_Press(index);
        return false;
    }
    if (index >= BTN_NAV_0 && index <= BTN_NAV_LAST) {
        int tab = navTabs[index - BTN_NAV_0];

        if (tab < 0) {
            Hub_Close();
        } else if (tab == HUB_PROFILE || tab == HUB_MATCHES) {
            if (tab == HUB_MATCHES && hubTab == HUB_MATCHES) {
                hubMatchId[0] = '\0';
            }
            Hub_Open(tab, meProfile.steamId);
        } else {
            Hub_Open(tab, NULL);
        }
        return false;
    }
    if (index >= BTN_SIDE_0 && index <= BTN_SIDE_LAST) {
        if (index - BTN_SIDE_0 < serverCount) {
            Box_Open(&servers[index - BTN_SIDE_0]);
        }
        return false;
    }
    if (index >= BTN_PLR_0 && index <= BTN_PLR_LAST) {
        if (index - BTN_PLR_0 < liveCount) {
            Hub_Open(HUB_PROFILE, livePlayers[index - BTN_PLR_0].steamId);
        }
        return false;
    }
    switch (index) {
        case BTN_ME:
            Hub_Open(HUB_PROFILE, meProfile.steamId);
            break;
        case BTN_CONNECT:
            return Box_Connect();
        case BTN_SEE_SERVER:
            Box_See();
            break;
        case BTN_BOX_CLOSE:
        case BTN_VEIL:
            boxOpen = false;
            break;
        case BTN_OPTS:
            optsOpen = !wasOpts;
            if (optsOpen) {
                startupOn = Autostart_On();
            }
            break;
        case BTN_SERVERS:
            hubGoal[HUB_SERVERS] = hubView[HUB_SERVERS] = 0.0f;
            Hub_Open(HUB_SERVERS, NULL);
            break;
        case BTN_PLAY:
            if (Play_Ready()) {
                return Game_Start();
            }
            break;
        case BTN_UPDATE:
            Update_Start();
            break;
        case BTN_STEAM:
            SDL_OpenURL("steam://open/main");
            steamOpening = SDL_GetTicks();
            break;
        case BTN_NEXT:
            if (install.active) {
                Install_Start();
            } else {
                Setup_Primary();
            }
            break;
        case BTN_ALT:
            if (install.active) {
                Folder_Open();
            } else {
                Setup_Alt();
            }
            break;
        case BTN_BACK:
            Setup_Back();
            break;
        case BTN_OPT_A:
        case BTN_OPT_B:
            if (install.active) {
                install.shortcut = !install.shortcut;
            } else if (setup.step == STEP_PREFS) {
                if (index == BTN_OPT_A) {
                    prefStartup = !prefStartup;
                } else {
                    prefBeta = !prefBeta;
                }
            } else {
                setup.want[index - BTN_OPT_A] = !setup.want[index - BTN_OPT_A];
            }
            break;
        case BTN_MIN:
            Window_ToTray();
            break;
        case BTN_CLOSE:
            return true;
        default:
            break;
    }
    return false;
}

static bool Key_Press(SDL_Keycode key) {
    if (boxOpen) {
        if (key == SDLK_ESCAPE) {
            boxOpen = false;
        } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            return Box_Connect();
        } else if (key == SDLK_DOWN || key == SDLK_UP) {
            boxGoal = SDL_max(0.0f, boxGoal + (key == SDLK_DOWN ? BOX_ROW : -BOX_ROW));
        }
        return false;
    }
    if (optsOpen) {
        optsOpen = false;
        return false;
    }
    if (hubOpen) {
        Hub_Key(key);
        return false;
    }
    if (install.active) {
        if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            Install_Start();
        }
        return key == SDLK_ESCAPE;
    }
    if (setup.step != STEP_NONE) {
        if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            Setup_Primary();
        }
        if (key == SDLK_ESCAPE && setup.step > STEP_FILES) {
            Setup_Back();
            return false;
        }
        return key == SDLK_ESCAPE;
    }
    switch (key) {
        case SDLK_ESCAPE:
            return true;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            return Button_Press(BTN_PLAY);
        case SDLK_UP:
        case SDLK_DOWN:
            Notes_Scroll(key == SDLK_DOWN ? 72.0f : -72.0f);
            break;
        case SDLK_PAGEUP:
        case SDLK_PAGEDOWN:
            Notes_Scroll((key == SDLK_PAGEDOWN ? 1.0f : -1.0f) * (notesView - 48.0f));
            break;
        default:
            break;
    }
    return false;
}

static float Scale_Pick(void) {
    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    float scale = SDL_GetDisplayContentScale(display);
    SDL_Rect bounds;

    if (scale < 1.0f) {
        scale = 1.0f;
    }
    if (SDL_GetDisplayUsableBounds(display, &bounds)) {
        float room = SDL_min((float)bounds.w / UI_W, (float)bounds.h / UI_H);
        float grow = SDL_min((float)bounds.w * UI_FILL_W / UI_W, (float)bounds.h * UI_FILL_H / UI_H);

        scale = SDL_clamp(grow, scale, scale * UI_GROW);
        if (scale > room) {
            scale = room;
        }
    }
    return SDL_floorf(scale * UI_W / UI_STEP) * UI_STEP / UI_W;
}

static int Launcher_Fail(const char* why) {
#ifdef LAUNCHER_SHOT
    SDL_Log("launcher: %s", why);
#else
    if (gameFound) {
        Settings_Load();
        if (Game_Start()) {
            SDL_Quit();
            return 0;
        }
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ioquakelive", why, window);
#endif
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return 1;
}

#ifdef LAUNCHER_SHOT
static Uint64 probeChange, probeFrame, probeIdle, probeDraw;
static int probeLeft = -1;

static bool Probe_Tick(void) {
    Uint64 t = SDL_GetTicksNS();
    int to = setup.step == STEP_SETTINGS ? STEP_DEMOS : STEP_SETTINGS;

    if (probeLeft < 0 || setup.step == STEP_NONE || SDL_GetAtomicInt(&jobRun) == RUN_BUSY) {
        return true;
    }
    if (!probeIdle) {
        probeIdle = t;
    }
    if (t - probeIdle < 1500000000u) {
        return true;
    }
    if (!probeLeft) {
        return false;
    }
    probeLeft--;
    probeIdle = probeChange = t;
    SDL_Log("probe: paso %d -> %d (%s)", setup.step, to, SDL_GetRendererName(renderer));
    Setup_Enter(to);
    return true;
}

static void Probe_Frame(void) {
    Uint64 t = SDL_GetTicksNS();

    if (probeChange && t - probeChange < 650000000u) {
        SDL_Log("probe: %6.1f ms, cuadro %5.1f ms, dibujo %5.2f ms, avance %.3f, corrimiento %5.1f", (double)(t - probeChange) / 1e6,
                (double)(t - probeFrame) / 1e6, (double)probeDraw / 1e6, (double)stepHeat,
                (double)((float)stepDir * 100.0f * (1.0f - Step_Ease())));
    }
    probeFrame = t;
}
#endif

static int hubShotTab = -1;
static bool hubShotDone;
static float hubShotY = -1.0f;

static void Hub_Shot_Open(void) {
    const char* id = SDL_getenv("LAUNCHER_HUB_ID");
    const char* option;
    const char* who = id && id[0] && SDL_strcmp(id, "yo") && Steam_IdValid(id) ? id : meProfile.steamId;

    hubShotDone = true;
    Hub_Open(hubShotTab, Steam_IdValid(who) ? who : NULL);
    option = SDL_getenv("LAUNCHER_HUB");
    if (option && !SDL_strcmp(option, "partida") && id && id[0]) {
        hubFrom = HUB_MATCHES;
        Hub_Match(id);
    }
    option = SDL_getenv("LAUNCHER_HUB_MODE");
    if (option && option[0]) {
        hubMode = SDL_clamp(SDL_atoi(option), 0, HUB_MODES - 1);
        hubRankMode = SDL_clamp(SDL_atoi(option), 0, HUB_RANK_MODES - 1);
        hubGamesMode = SDL_clamp(SDL_atoi(option), 0, HUB_MODES);
    }
    option = SDL_getenv("LAUNCHER_HUB_SEASON");
    if (option && option[0]) {
        hubSeason = SDL_clamp(SDL_atoi(option), 0, HUB_SEASONS - 1);
    }
    option = SDL_getenv("LAUNCHER_HUB_SUB");
    if (option && option[0]) {
        hubSub = SDL_clamp(SDL_atoi(option), 0, 3);
    }
    option = SDL_getenv("LAUNCHER_HUB_PAGE");
    if (option && option[0]) {
        hubPage = hubRankPage = SDL_max(1, SDL_atoi(option));
    }
    option = SDL_getenv("LAUNCHER_HUB_Y");
    if (option && option[0]) {
        hubShotY = (float)SDL_atof(option);
    }
}

int main(int argc, char** argv) {
    const char* shot = NULL;
    bool running = true, moving = false, inside = false, shown = false, failed = false;
    int lastState = CHECK_RUNNING, forced = -1, press = -1, autoSteps = 0;
    Uint64 last, now;
    char stale[1100], failure[256];

#ifdef LAUNCHER_SHOT
    const char* option;
    bool autoUpdate = false, autoRepair = false;

    if (argc > 1) {
        shot = argv[1];
    }
    gameArgs = argv + 2;
    gameArgCount = argc > 2 ? argc - 2 : 0;
    option = SDL_getenv("LAUNCHER_HOT");
    if (option && option[0]) {
        forced = atoi(option);
    }
    option = SDL_getenv("LAUNCHER_TAB");
    optsOpen = option && option[0] == '4';
    hubShotTab = option && (option[0] == '5' || option[0] == '6' || option[0] == '8') ? HUB_SERVERS : -1;
    hubShotY = option && option[0] == '6' ? 10000.0f : -1.0f;
    boxWant = option && (option[0] == '7' || option[0] == '8') ? 1 : 0;
    option = SDL_getenv("LAUNCHER_HUB");
    if (option && option[0]) {
        hubShotTab = !SDL_strcmp(option, "partidas") || !SDL_strcmp(option, "partida") ? HUB_MATCHES
                     : !SDL_strcmp(option, "ranking")                                    ? HUB_RANKING
                     : !SDL_strcmp(option, "servidores")                                 ? HUB_SERVERS
                                                                                         : HUB_PROFILE;
    }
    notesGoal = option && option[0] == '3' ? 260.0f : 0.0f;
    option = SDL_getenv("LAUNCHER_STEPS");
    autoSteps = option ? atoi(option) : 0;
    option = SDL_getenv("LAUNCHER_PRESS");
    if (option && option[0]) {
        press = atoi(option);
    }
    option = SDL_getenv("LAUNCHER_BOX");
    if (boxWant && option && option[0]) {
        boxWant = 1 + atoi(option);
    }
    option = SDL_getenv("LAUNCHER_PROBE");
    if (option && option[0]) {
        probeLeft = atoi(option);
    }
#else
    gameArgs = argv + 1;
    gameArgCount = argc > 1 ? argc - 1 : 0;
#endif
    if (gameArgCount >= 1 && !SDL_strcmp(gameArgs[0], "--wait")) {
        waitLock = true;
        gameArgs++;
        gameArgCount--;
    }
    if (gameArgCount >= 1 && !SDL_strcmp(gameArgs[0], "--startup")) {
        startMinimized = true;
        gameArgs++;
        gameArgCount--;
    }
    if (gameArgCount >= 2 && !SDL_strcmp(gameArgs[0], "--renderer")) {
        rendererHint = !SDL_strcmp(gameArgs[1], "vulkan") ? 1 : 0;
        gameArgs += 2;
        gameArgCount -= 2;
    }
#ifndef _WIN32
    SDL_SetAppMetadata("ioquakelive Launcher", PRODUCT_VERSION, "ioquakelive");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
        failed = true;
    }
    basePath = SDL_GetBasePath();
    if (!basePath) {
        basePath = "";
    }
    SDL_snprintf(selfPath, sizeof(selfPath), "%s" SELF_NAME, basePath);
#ifdef _WIN32
    {
        wchar_t wide[1024];

        if (GetModuleFileNameW(NULL, wide, 1024)) {
            WideCharToMultiByte(CP_UTF8, 0, wide, -1, selfPath, sizeof(selfPath), NULL, NULL);
        }
    }
#elif defined(__APPLE__)
    {
        char raw[1024], real[PATH_MAX];
        uint32_t size = sizeof(raw);
        const char* app;
        char* slash;

        if (_NSGetExecutablePath(raw, &size) == 0 && realpath(raw, real)) {
            SDL_strlcpy(selfPath, real, sizeof(selfPath));
        }
        app = strstr(selfPath, ".app/Contents/MacOS/");
        if (app) {
            SDL_strlcpy(selfApp, selfPath, (size_t)(app - selfPath) + 5);
            SDL_strlcpy(appRoot, selfApp, sizeof(appRoot));
            slash = strrchr(appRoot, '/');
            if (slash) {
                slash[1] = '\0';
                basePath = appRoot;
            }
        }
    }
    Curl_Load();
#else
    {
        char real[1024];
        ssize_t n = readlink("/proc/self/exe", real, sizeof(real) - 1);

        if (n > 0) {
            real[n] = '\0';
            SDL_strlcpy(selfPath, real, sizeof(selfPath));
        }
    }
    Curl_Load();
#endif
    SDL_snprintf(stale, sizeof(stale), "%s" SELF_NAME ".old", basePath);
    SDL_RemovePath(stale);
#ifndef LAUNCHER_SHOT
    if (!Instance_Take()) {
        SDL_Quit();
        return 0;
    }
#endif
    gameFound = File_Exists(GAME_NAME);
    vulkanFound = File_Exists(VULKAN_NAME);
    if (failed) {
        return Launcher_Fail(failure);
    }
    if (!gameFound) {
        Install_Init();
    }
    uiScale = Scale_Pick();
#ifdef LAUNCHER_SHOT
    option = SDL_getenv("LAUNCHER_SCALE");
    if (option && option[0]) {
        uiScale = (float)atof(option);
    }
#endif
    window = SDL_CreateWindow("ioquakelive Launcher", (int)(UI_W * uiScale + 0.5f), (int)(UI_H * uiScale + 0.5f),
                              SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN | (startMinimized ? SDL_WINDOW_MINIMIZED : 0));
    renderer = window ? SDL_CreateRenderer(window, NULL) : NULL;
    if (!renderer) {
        SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
        return Launcher_Fail(failure);
    }
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetWindowHitTest(window, Window_HitTest, NULL);
#ifdef _WIN32
    Window_Round();
#endif
    cursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    cursorHand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);

    Fonts_Load();
    Hub_FontsStart();
    Image_FromPng(&logo, launcherLogo, launcherLogoSize, LOGO);
    Icons_Load();
    Image_LoadHero();
    Image_Vignette();
    Settings_Load();
    startupOn = Autostart_On();
#ifdef LAUNCHER_SHOT
    option = SDL_getenv("LAUNCHER_UPDATE");
    autoUpdate = option && option[0] == '1';
    autoRepair = option && option[0] == '2';
    option = SDL_getenv("LAUNCHER_VULKAN");
    if (option && option[0]) {
        SDL_SetAtomicInt(&vulkanState, option[0] == '1' ? VULKAN_USABLE : VULKAN_MISSING);
    }
#endif
    if (vulkanFound && SDL_GetAtomicInt(&vulkanState) == VULKAN_PROBING) {
        SDL_DetachThread(SDL_CreateThread(Vulkan_Thread, "vulkan", NULL));
    }
    setup.want[KIND_CONFIG] = setup.want[KIND_HUD] = true;
    if (gameFound) {
        char pak[1024];

        SDL_snprintf(pak, sizeof(pak), "%s" PAK_GAME, basePath);
        setup.step = Pak_Valid(pak) ? setupSaved : STEP_FILES;
        prefStartup = startupOn;
        prefBeta = betaChannel;
        if (setup.step != STEP_NONE) {
            Job_Start(JOB_SCAN, 0);
        }
    }
#ifndef LAUNCHER_SHOT
    if (gameFound && firstRun) {
        Shortcut_Make(selfPath, basePath, SDL_GetUserFolder(SDL_FOLDER_DESKTOP));
        Settings_Save();
    }
#endif

    SDL_SetAtomicInt(&checkState, install.active ? CHECK_FAILED : CHECK_RUNNING);
    checkThread = install.active ? NULL : SDL_CreateThread(Check_Thread, "check", NULL);
    if (!checkThread) {
        SDL_SetAtomicInt(&checkState, CHECK_FAILED);
    }
    if (gameFound && !install.active) {
        Servers_Fetch();
        Me_Start();
    }

#ifndef LAUNCHER_SHOT
    Tray_Create();
#endif

    last = SDL_GetTicks();
    while (running) {
        SDL_Event event;
        bool busy = SDL_GetAtomicInt(&jobRun) == RUN_BUSY;
        bool have = SDL_WaitEventTimeout(&event, !shown || moving ? 1 : lastState == CHECK_RUNNING || busy ? 33 : inside ? 40 : 500);

        while (have) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_MINIMIZED:
                    if (tray) {
                        SDL_HideWindow(window);
                        SDL_RestoreWindow(window);
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int hit = Button_At(event.button.x / uiScale, event.button.y / uiScale);

                        if ((hit >= 0 || !Feed_Grab(event.button.x / uiScale, event.button.y / uiScale)) && Button_Press(hit)) {
                            running = false;
                        }
                    } else if (event.button.button == SDL_BUTTON_X1) {
                        Setup_Back();
                    }
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (dragMode) {
                        Feed_Drag(event.motion.y / uiScale);
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button == SDL_BUTTON_LEFT && dragMode) {
                        dragMode = 0;
                        SDL_CaptureMouse(false);
                    }
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    if (boxOpen) {
                        boxGoal = SDL_max(0.0f, boxGoal - event.wheel.y * 48.0f);
                    } else if (hubOpen) {
                        Hub_Wheel(event.wheel.y);
                    } else if (setup.step == STEP_NONE && !install.active) {
                        Notes_Scroll(-event.wheel.y * 72.0f);
                    }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (Key_Press(event.key.key)) {
                        running = false;
                    }
                    break;
                case SDL_EVENT_DROP_FILE:
                    if (setup.step == STEP_FILES && !install.active && event.drop.data && !SDL_GetAtomicInt(&pickState)) {
                        SDL_strlcpy(pickPath, event.drop.data, sizeof(pickPath));
                        SDL_SetAtomicInt(&pickState, 1);
                    }
                    break;
                default:
                    break;
            }
            have = SDL_PollEvent(&event);
        }
        if (trayAction == 1) {
            Window_FromTray();
        } else if (trayAction == 2) {
            running = false;
        }
        trayAction = 0;
        lastState = SDL_GetAtomicInt(&checkState);
        Pick_Poll();
        if (Job_Poll()) {
            running = false;
        }
        Taskbar_Update();
        Servers_Poll();
        if (!shot) {
            Update_Auto();
        }
        Steam_Poll(SDL_GetTicks());
        busy = SDL_GetAtomicInt(&jobRun) == RUN_BUSY;
        if (!shot) {
            inside = Hover_Poll();
        }
#ifdef LAUNCHER_SHOT
        if (!Probe_Tick()) {
            running = false;
        }
#endif
        now = SDL_GetTicks();
        moving = Heat_Update((float)(now - last) / 1000.0f, lastState == CHECK_DONE && latestNotes[0]);
        last = now;
#ifdef LAUNCHER_SHOT
        probeDraw = SDL_GetTicksNS();
#endif
        Draw_Frame();
#ifdef LAUNCHER_SHOT
        probeDraw = SDL_GetTicksNS() - probeDraw;
        if (shot && lastState != CHECK_RUNNING && !busy && autoSteps > 0 && install.active && !install.done && install.fail == FAIL_NONE) {
            autoSteps--;
            Install_Start();
            busy = true;
        } else if (shot && lastState == CHECK_DONE && !busy && autoUpdate && updateAvailable) {
            autoUpdate = false;
            Update_Start();
            busy = true;
        } else if (shot && lastState == CHECK_DONE && !busy && autoRepair) {
            autoRepair = false;
            Repair_Start();
            busy = true;
        }
#endif
        if (shot && lastState != CHECK_RUNNING && !busy && autoSteps > 0 && setup.step != STEP_NONE && !install.active) {
            autoSteps--;
            Setup_Primary();
        } else if (shot && hubShotTab >= 0 && !hubShotDone && SDL_GetAtomicInt(&meState) != 1 && (meProfile.status != PRO_LOADING || SDL_GetTicks() > 10000)) {
            Hub_Shot_Open();
        } else if (shot && boxWant && serversLoaded && serverCount > 0 && !boxOpen) {
            Box_Open(&servers[SDL_min(boxWant - 1, serverCount - 1)]);
        } else if (shot && !moving && lastState != CHECK_RUNNING && !busy && SDL_GetAtomicInt(&serversState) == 0 && SDL_GetAtomicInt(&shotsState) == 0 &&
                   SDL_GetAtomicInt(&liveState) == 0 && SDL_GetAtomicInt(&picsState) == 0 && (!boxWant || liveLoaded || liveFailed) &&
                   SDL_GetAtomicInt(&meState) != 1 && !Hub_Busy() && (hubShotTab < 0 || hubShotDone)) {
            SDL_Surface* surface;
            int k;

            if (forced >= 0 && forced < BTN_COUNT) {
                hotButton = forced;
                heat[forced] = 1.0f;
            }
            if (press >= 0 && press < BTN_COUNT) {
                Button_Press(press);
            }
#ifdef LAUNCHER_SHOT
            option = SDL_getenv("LAUNCHER_SLIDE");
            if (option && option[0]) {
                stepHeat = (float)SDL_atof(option);
            }
#endif
            optsHeat = optsOpen ? 1.0f : 0.0f;
            for (k = 0; k < shotCount; k++) {
                shots[k].heat = 1.0f;
            }
            boxHeat = boxOpen ? 1.0f : 0.0f;
            boxView = boxGoal;
            hubHeat = hubOpen ? 1.0f : 0.0f;
            if (hubShotY >= 0.0f) {
                hubGoal[Hub_View()] = SDL_min(hubShotY, hubMost[Hub_View()]);
            }
            for (k = 0; k < 5; k++) {
                hubView[k] = hubGoal[k];
            }
#ifdef LAUNCHER_SHOT
            option = SDL_getenv("LAUNCHER_PANEL");
            if (option && option[0]) {
                hubHeat = (float)SDL_atof(option);
            }
            option = SDL_getenv("LAUNCHER_HUB_MOUSE");
            if (option && option[0]) {
                SDL_sscanf(option, "%f,%f", &hubMouseX, &hubMouseY);
            }
#endif
            introHeat = 1.0f;
            notesHeat = lastState == CHECK_DONE && latestNotes[0] ? 1.0f : 0.0f;
            notesScroll = notesGoal;
            Draw_Frame();
            Draw_Frame();
            surface = SDL_RenderReadPixels(renderer, NULL);
            if (surface) {
                SDL_SavePNG(surface, shot);
                SDL_DestroySurface(surface);
            }
            if (gameArgCount && setup.step == STEP_NONE) {
                SDL_Log("play: %d", (int)Button_Press(BTN_PLAY));
            }
            running = false;
        }
        SDL_RenderPresent(renderer);
#ifdef LAUNCHER_SHOT
        Probe_Frame();
#endif
        if (!shown && !shot) {
            if (!startMinimized || !tray) {
                SDL_ShowWindow(window);
                if (startMinimized) {
                    SDL_MinimizeWindow(window);
                }
            }
            shown = true;
        }
    }

    SDL_SetAtomicInt(&jobStop, 1);
    if (jobThread) {
        SDL_WaitThread(jobThread, NULL);
    }
    if (serversThread) {
        SDL_DetachThread(serversThread);
    }
    if (liveThread) {
        SDL_DetachThread(liveThread);
    }
    if (picsThread) {
        SDL_DetachThread(picsThread);
    }
    Hub_Stop();
    if (shotsThread) {
        SDL_WaitThread(shotsThread, NULL);
    }
    if (checkThread) {
        SDL_WaitThread(checkThread, NULL);
    }
    if (tray) {
        SDL_DestroyTray(tray);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
