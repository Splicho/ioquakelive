/*
===========================================================================
GitHub release updater for ioquakelive.

Windows-only: checks the latest Splicho/ioquakelive release, asks the user,
downloads the Windows x64 release zip, then hands off to a small cmd/PowerShell
helper that expands it after this process exits.

The rest of this file is GPLv2, matching the engine.
===========================================================================
*/

#include "../qcommon/q_shared.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#pragma comment(lib, "winhttp.lib")
#endif

#define UPDATE_REPO_HOST L"api.github.com"
#define UPDATE_REPO_PATH L"/repos/Splicho/ioquakelive/releases/latest"
#define UPDATE_ASSET "ioquakelive-windows-x64.zip"

static int UpdateMessage(const char* text, UINT flags) {
    return MessageBoxA(NULL, text, "ioquakelive updater", flags | MB_TOPMOST);
}

static const char* JsonString(const char* object, const char* key) {
    const char* p = strstr(object, key);
    if (!p) {
        return NULL;
    }
    p = strchr(p, ':');
    if (!p) {
        return NULL;
    }
    p = strchr(p, '"');
    if (!p) {
        return NULL;
    }
    return p + 1;
}

static void JsonCopy(const char* object, const char* key, char* out, size_t outSize) {
    const char* p;
    const char* end;

    out[0] = '\0';
    p = JsonString(object, key);
    if (!p) {
        return;
    }
    end = strchr(p, '"');
    if (!end) {
        return;
    }
    if ((size_t)(end - p) >= outSize) {
        end = p + outSize - 1;
    }
    memcpy(out, p, (size_t)(end - p));
    out[end - p] = '\0';
}

static BOOL HttpGet(const wchar_t* host, const wchar_t* path, char** data, DWORD* dataLen) {
    HINTERNET session = NULL;
    HINTERNET connect = NULL;
    HINTERNET request = NULL;
    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    DWORD total = 0;
    DWORD cap = 0;
    char* buffer = NULL;
    BOOL ok = FALSE;

    *data = NULL;
    *dataLen = 0;

    session = WinHttpOpen(L"ioquakelive-updater/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        goto done;
    }
    WinHttpSetTimeouts(session, 5000, 5000, 10000, 10000);

    connect = WinHttpConnect(session, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connect) {
        goto done;
    }

    request = WinHttpOpenRequest(connect, L"GET", path, NULL, WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!request) {
        goto done;
    }

    if (!WinHttpAddRequestHeaders(request,
                                  L"User-Agent: ioquakelive-updater/1.0\r\n"
                                  L"Accept: application/vnd.github+json\r\n",
                                  (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
        goto done;
    }

    if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        goto done;
    }
    if (!WinHttpReceiveResponse(request, NULL)) {
        goto done;
    }
    if (!WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                             WINHTTP_NO_HEADER_INDEX) ||
        status != 200) {
        goto done;
    }

    for (;;) {
        DWORD available = 0;
        DWORD read = 0;

        if (!WinHttpQueryDataAvailable(request, &available) || available == 0) {
            break;
        }
        if (total + available + 1 > cap) {
            DWORD newCap = cap ? cap : 8192;
            char* newBuffer;

            while (newCap < total + available + 1) {
                newCap *= 2;
            }
            newBuffer = (char*)realloc(buffer, newCap);
            if (!newBuffer) {
                goto done;
            }
            buffer = newBuffer;
            cap = newCap;
        }
        if (!WinHttpReadData(request, buffer + total, available, &read) || read == 0) {
            break;
        }
        total += read;
    }

    if (!buffer || total == 0) {
        goto done;
    }
    buffer[total] = '\0';
    *data = buffer;
    *dataLen = total;
    buffer = NULL;
    ok = TRUE;

done:
    free(buffer);
    if (request) {
        WinHttpCloseHandle(request);
    }
    if (connect) {
        WinHttpCloseHandle(connect);
    }
    if (session) {
        WinHttpCloseHandle(session);
    }
    return ok;
}

static HWND updateProgressWindow;
static int updateProgressPercent;
static qboolean updateProgressCancelled;
static char updateProgressText[256];

static void UpdateProgressPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC dc;
    RECT client;
    RECT textRect;
    RECT barOuter;
    RECT barInner;
    HBRUSH background;
    HBRUSH border;
    HBRUSH fill;
    int fillWidth;

    dc = BeginPaint(hwnd, &ps);
    GetClientRect(hwnd, &client);

    background = CreateSolidBrush(RGB(24, 24, 24));
    FillRect(dc, &client, background);
    DeleteObject(background);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(240, 240, 240));
    textRect.left = 20;
    textRect.right = client.right - 20;
    textRect.top = 16;
    textRect.bottom = 58;
    DrawTextA(dc, updateProgressText, -1, &textRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    barOuter.left = 30;
    barOuter.right = client.right - 30;
    barOuter.top = 76;
    barOuter.bottom = 98;
    border = CreateSolidBrush(RGB(90, 90, 90));
    FrameRect(dc, &barOuter, border);
    DeleteObject(border);

    barInner = barOuter;
    InflateRect(&barInner, -2, -2);
    fillWidth = (barInner.right - barInner.left) * (updateProgressPercent < 0 ? 0 : updateProgressPercent) / 100;
    barInner.right = barInner.left + fillWidth;
    if (barInner.right > barInner.left) {
        fill = CreateSolidBrush(RGB(45, 170, 75));
        FillRect(dc, &barInner, fill);
        DeleteObject(fill);
    }

    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK UpdateProgressProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CLOSE:
            updateProgressCancelled = qtrue;
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            UpdateProgressPaint(hwnd);
            return 0;
        default:
            return DefWindowProcA(hwnd, message, wParam, lParam);
    }
}

static void UpdateProgressPump(void) {
    MSG message;

    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

static void UpdateProgressSet(int percent, const char* text) {
    if (percent < 0) {
        percent = 0;
    }
    if (percent > 100) {
        percent = 100;
    }
    updateProgressPercent = percent;
    Q_strncpyz(updateProgressText, text, sizeof(updateProgressText));
    if (updateProgressWindow) {
        InvalidateRect(updateProgressWindow, NULL, FALSE);
        UpdateWindow(updateProgressWindow);
    }
    UpdateProgressPump();
}

static BOOL UpdateProgressCreate(void) {
    WNDCLASSA windowClass;
    RECT workArea;
    RECT windowRect;
    int width = 460;
    int height = 130;
    int x;
    int y;

    if (updateProgressWindow) {
        return TRUE;
    }

    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.lpfnWndProc = UpdateProgressProc;
    windowClass.hInstance = GetModuleHandleA(NULL);
    windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    windowClass.lpszClassName = "ioquakeliveUpdateProgress";
    RegisterClassA(&windowClass);

    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = width;
    windowRect.bottom = height;
    AdjustWindowRectEx(&windowRect, WS_POPUP | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);

    SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0);
    x = workArea.left + ((workArea.right - workArea.left) - (windowRect.right - windowRect.left)) / 2;
    y = workArea.top + ((workArea.bottom - workArea.top) - (windowRect.bottom - windowRect.top)) / 2;

    updateProgressWindow = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        "ioquakeliveUpdateProgress",
        "ioquakelive update",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    if (!updateProgressWindow) {
        return FALSE;
    }

    updateProgressCancelled = qfalse;
    updateProgressPercent = 0;
    Q_strncpyz(updateProgressText, "Preparing update...", sizeof(updateProgressText));
    ShowWindow(updateProgressWindow, SW_SHOWNORMAL);
    UpdateWindow(updateProgressWindow);
    UpdateProgressPump();
    return TRUE;
}

static void UpdateProgressClose(void) {
    if (updateProgressWindow) {
        DestroyWindow(updateProgressWindow);
        updateProgressWindow = NULL;
    }
    UpdateProgressPump();
}

static BOOL HttpDownload(const wchar_t* url, const char* filename) {    URL_COMPONENTS components;
    wchar_t host[256];
    wchar_t path[4096];
    HINTERNET session = NULL;
    HINTERNET connect = NULL;
    HINTERNET request = NULL;
    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    FILE* file = NULL;
    BOOL ok = FALSE;

    memset(&components, 0, sizeof(components));
    components.dwStructSize = sizeof(components);
    components.lpszHostName = host;
    components.dwHostNameLength = (DWORD)ARRAY_LEN(host);
    components.lpszUrlPath = path;
    components.dwUrlPathLength = (DWORD)ARRAY_LEN(path);

    if (!WinHttpCrackUrl(url, 0, 0, &components)) {
        return FALSE;
    }
    if (components.dwHostNameLength >= ARRAY_LEN(host) || components.dwUrlPathLength >= ARRAY_LEN(path)) {
        return FALSE;
    }
    host[components.dwHostNameLength] = '\0';
    path[components.dwUrlPathLength] = '\0';

    session = WinHttpOpen(L"ioquakelive-updater/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        goto done;
    }
    WinHttpSetTimeouts(session, 5000, 5000, 60000, 60000);

    connect = WinHttpConnect(session, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connect) {
        goto done;
    }

    request = WinHttpOpenRequest(connect, L"GET", path, NULL, WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!request) {
        goto done;
    }

    if (!WinHttpAddRequestHeaders(request, L"User-Agent: ioquakelive-updater/1.0\r\n",
                                  (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
        goto done;
    }

    if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        goto done;
    }
    if (!WinHttpReceiveResponse(request, NULL)) {
        goto done;
    }
    if (!WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                             WINHTTP_NO_HEADER_INDEX) ||
        status != 200) {
        goto done;
    }

    {
        DWORD contentLength = 0;
        DWORD contentLengthSize = sizeof(contentLength);
        DWORD downloaded = 0;

        WinHttpQueryHeaders(request, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &contentLength, &contentLengthSize,
                            WINHTTP_NO_HEADER_INDEX);
        UpdateProgressSet(0, "Downloading update...");

        file = fopen(filename, "wb");
        if (!file) {
            goto done;
        }

        for (;;) {
            DWORD available = 0;
            DWORD read = 0;
            char buffer[16384];

            if (updateProgressCancelled) {
                goto done;
            }
            if (!WinHttpQueryDataAvailable(request, &available) || available == 0) {
                break;
            }
            if (available > sizeof(buffer)) {
                available = sizeof(buffer);
            }
            if (!WinHttpReadData(request, buffer, available, &read) || read == 0) {
                break;
            }
            if (fwrite(buffer, 1, read, file) != read) {
                goto done;
            }
            downloaded += read;
            if (contentLength > 0) {
                char statusText[128];
                int percent = (int)((ULONGLONG)downloaded * 100 / contentLength);
                if (percent > 100) {
                    percent = 100;
                }
                snprintf(statusText, sizeof(statusText), "Downloading update... %d%%", percent);
                UpdateProgressSet(percent, statusText);
            } else {
                char statusText[128];
                snprintf(statusText, sizeof(statusText), "Downloading update... %.1f MB",
                         downloaded / (1024.0 * 1024.0));
                UpdateProgressSet(0, statusText);
            }
        }
    }

    ok = TRUE;

done:
    if (file) {
        fclose(file);
    }
    if (request) {
        WinHttpCloseHandle(request);
    }
    if (connect) {
        WinHttpCloseHandle(connect);
    }
    if (session) {
        WinHttpCloseHandle(session);
    }
    return ok;
}

static int VersionPart(const char** text) {
    int value = 0;

    while (**text == ' ') {
        (*text)++;
    }
    while (**text >= '0' && **text <= '9') {
        value = value * 10 + (**text - '0');
        (*text)++;
    }
    if (**text == '.') {
        (*text)++;
    }
    return value;
}

static BOOL VersionIsNewer(const char* latest, const char* current) {
    int latestMajor, latestMinor, latestPatch;
    int currentMajor, currentMinor, currentPatch;
    const char* p = latest;

    while (*p && (*p < '0' || *p > '9')) {
        p++;
    }
    latestMajor = VersionPart(&p);
    latestMinor = VersionPart(&p);
    latestPatch = VersionPart(&p);

    p = current;
    while (*p && (*p < '0' || *p > '9')) {
        p++;
    }
    currentMajor = VersionPart(&p);
    currentMinor = VersionPart(&p);
    currentPatch = VersionPart(&p);

    if (latestMajor != currentMajor) {
        return latestMajor > currentMajor;
    }
    if (latestMinor != currentMinor) {
        return latestMinor > currentMinor;
    }
    return latestPatch > currentPatch;
}

static BOOL PathDirectory(const char* path, char* dir, size_t dirSize, char* name, size_t nameSize) {
    const char* slash;
    size_t len;

    if (!path || !path[0]) {
        return FALSE;
    }
    slash = strrchr(path, '\\');
    if (!slash) {
        slash = strrchr(path, '/');
    }
    if (!slash) {
        return FALSE;
    }
    len = (size_t)(slash - path);
    if (len + 1 >= dirSize) {
        return FALSE;
    }
    memcpy(dir, path, len);
    dir[len] = '\0';
    Q_strncpyz(name, slash + 1, (int)nameSize);
    return TRUE;
}

static void RunUpdaterAndExit(const char* zipPath, const char* installDir, const char* exeName) {
    char scriptPath[MAX_PATH];
    char command[MAX_PATH * 3];
    char comspec[MAX_PATH];
    FILE* script;
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;

    if (!GetTempPathA(MAX_PATH, scriptPath)) {
        return;
    }
    Q_strcat(scriptPath, sizeof(scriptPath), "ioquakelive-update.cmd");

    script = fopen(scriptPath, "w");
    if (!script) {
        UpdateMessage("Could not create the update helper.", MB_OK | MB_ICONERROR);
        return;
    }

    fprintf(script,
            "@echo off\r\n"
            "ping 127.0.0.1 -n 3 >nul\r\n"
            "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Expand-Archive -LiteralPath '%s' -DestinationPath '%s' -Force\"\r\n"
            "start \"\" \"%s\\%s\"\r\n"
            "del \"%%~f0\"\r\n",
            zipPath, installDir, installDir, exeName);
    fclose(script);

    if (!GetEnvironmentVariableA("COMSPEC", comspec, sizeof(comspec))) {
        Q_strncpyz(comspec, "cmd.exe", sizeof(comspec));
    }
    snprintf(command, sizeof(command), "\"%s\" /c \"\"%s\"\"", comspec, scriptPath);

    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    if (CreateProcessA(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup, &process)) {
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
        ExitProcess(0);
    }

    UpdateMessage("Could not start the update helper.", MB_OK | MB_ICONERROR);
}

void Sys_CheckForGithubUpdate(void) {
    char* json = NULL;
    DWORD jsonLen = 0;
    char tag[64];
    char assetUrl[2048];
    char prompt[512];
    char modulePath[MAX_PATH];
    char installDir[MAX_PATH];
    char exeName[MAX_PATH];
    char tempPath[MAX_PATH];
    char zipPath[MAX_PATH];
    wchar_t assetUrlWide[2048];

    if (!HttpGet(UPDATE_REPO_HOST, UPDATE_REPO_PATH, &json, &jsonLen)) {
        return;
    }

    JsonCopy(json, "\"tag_name\"", tag, sizeof(tag));
    if (!tag[0]) {
        free(json);
        return;
    }

    {
        const char* asset = strstr(json, UPDATE_ASSET);
        const char* url = asset ? strstr(asset, "\"browser_download_url\"") : NULL;

        assetUrl[0] = '\0';
        if (url) {
            JsonCopy(url, "\"browser_download_url\"", assetUrl, sizeof(assetUrl));
        }
    }
    free(json);

    if (!assetUrl[0] || !VersionIsNewer(tag, PRODUCT_VERSION)) {
        return;
    }

    snprintf(prompt, sizeof(prompt),
             "A newer ioquakelive is available.\n\n"
             "Installed: %s\nLatest: %s\n\n"
             "Download and install it now?",
             PRODUCT_VERSION, tag);
    if (UpdateMessage(prompt, MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }

    if (!GetModuleFileNameA(NULL, modulePath, sizeof(modulePath)) ||
        !PathDirectory(modulePath, installDir, sizeof(installDir), exeName, sizeof(exeName))) {
        UpdateMessage("Could not determine the install directory.", MB_OK | MB_ICONERROR);
        return;
    }

    if (!GetTempPathA(MAX_PATH, tempPath)) {
        UpdateMessage("Could not determine the temporary directory.", MB_OK | MB_ICONERROR);
        return;
    }
    snprintf(zipPath, sizeof(zipPath), "%sioquakelive-%s.zip", tempPath, tag);
    DeleteFileA(zipPath);

    if (MultiByteToWideChar(CP_ACP, 0, assetUrl, -1, assetUrlWide, ARRAY_LEN(assetUrlWide)) <= 0) {
        UpdateMessage("Could not prepare the update download.", MB_OK | MB_ICONERROR);
        return;
    }

    UpdateProgressCreate();
    if (!HttpDownload(assetUrlWide, zipPath)) {
        BOOL cancelled = updateProgressCancelled;
        UpdateProgressClose();
        DeleteFileA(zipPath);
        if (!cancelled) {
            UpdateMessage("Could not download the update. Try again later.", MB_OK | MB_ICONERROR);
        }
        return;
    }

    UpdateProgressSet(100, "Download complete. Installing...");
    Sleep(500);
    UpdateProgressClose();

    RunUpdaterAndExit(zipPath, installDir, exeName);
}

#else

void Sys_CheckForGithubUpdate(void) {
}

#endif
