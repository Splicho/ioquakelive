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

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET stSocket_t;
#define STRM_INVALID INVALID_SOCKET
#define STRM_CLOSE closesocket
#define STRM_BLOCKED() (WSAGetLastError() == WSAEWOULDBLOCK)
#define STRM_SEND_FLAGS 0
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
typedef int stSocket_t;
#define STRM_INVALID (-1)
#define STRM_CLOSE close
#define STRM_BLOCKED() (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
#ifdef MSG_NOSIGNAL
#define STRM_SEND_FLAGS MSG_NOSIGNAL
#else
#define STRM_SEND_FLAGS 0
#endif
#endif

#include "client.h"

#define STRM_MAX_CONNS 16
#define STRM_REQ_MAX 4096
#define STRM_OUT_MAX (8 * 1024 * 1024)
#define STRM_STATE_MAX (512 * 1024)
#define STRM_READ_TIMEOUT 5000
#define STRM_PING_TIME 15000

enum { STRM_FREE, STRM_READING, STRM_EVENTS, STRM_CLOSING };

typedef struct {
    stSocket_t sock;
    int kind;
    char req[STRM_REQ_MAX];
    int reqLen;
    char* out;
    int outLen;
    int outCap;
    int opened;
} stConn_t;

static cvar_t* cl_streamerMode;
static cvar_t* cl_streamerPort;
static cvar_t* cl_streamerRate;
static cvar_t* cl_streamerClients;

static stSocket_t stListen = STRM_INVALID;
static int stListenPort;
static stConn_t stConns[STRM_MAX_CONNS];
static char stState[STRM_STATE_MAX];
static int stStateLen;
static int stNextPush;
static int stLastWrite;
static int stFailedPort;
static int stEventCount;
#ifdef _WIN32
static qboolean stWsa;
#endif

static const char stOffline[] = "{\"version\":1,\"connected\":false}";

static void CL_StreamerNonBlocking(stSocket_t s) {
#ifdef _WIN32
    u_long one = 1;
    ioctlsocket(s, FIONBIO, &one);
#else
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);
#ifdef SO_NOSIGPIPE
    {
        int one = 1;
        setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (const void*)&one, sizeof(one));
    }
#endif
#endif
}

static void CL_StreamerDrop(stConn_t* c) {
    if (c->sock != STRM_INVALID) {
        STRM_CLOSE(c->sock);
    }
    free(c->out);
    Com_Memset(c, 0, sizeof(*c));
    c->sock = STRM_INVALID;
    c->kind = STRM_FREE;
}

static void CL_StreamerCount(void) {
    int i, n = 0;

    for (i = 0; i < STRM_MAX_CONNS; i++) {
        if (stConns[i].kind == STRM_EVENTS) {
            n++;
        }
    }
    if (n != stEventCount) {
        stEventCount = n;
        Cvar_Set("cl_streamerClients", va("%i", n));
    }
}

static void CL_StreamerQueue(stConn_t* c, const char* data, int len) {
    char* grown;
    int cap;

    if (c->kind == STRM_FREE || len <= 0) {
        return;
    }
    if (c->outLen + len > STRM_OUT_MAX) {
        CL_StreamerDrop(c);
        return;
    }
    if (c->outLen + len > c->outCap) {
        cap = c->outCap ? c->outCap : 16384;
        while (cap < c->outLen + len) {
            cap *= 2;
        }
        grown = (char*)realloc(c->out, cap);
        if (!grown) {
            CL_StreamerDrop(c);
            return;
        }
        c->out = grown;
        c->outCap = cap;
    }
    Com_Memcpy(c->out + c->outLen, data, len);
    c->outLen += len;
}

static void CL_StreamerQueueText(stConn_t* c, const char* text) {
    CL_StreamerQueue(c, text, (int)strlen(text));
}

static void CL_StreamerCloseAll(void) {
    int i;

    for (i = 0; i < STRM_MAX_CONNS; i++) {
        if (stConns[i].kind != STRM_FREE) {
            CL_StreamerDrop(&stConns[i]);
        }
    }
    if (stListen != STRM_INVALID) {
        STRM_CLOSE(stListen);
        stListen = STRM_INVALID;
    }
    stListenPort = 0;
    CL_StreamerCount();
}

static qboolean CL_StreamerOpen(int port) {
    struct sockaddr_in addr;
    stSocket_t s;

#ifdef _WIN32
    if (!stWsa) {
        WSADATA data;
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            return qfalse;
        }
        stWsa = qtrue;
    }
#endif

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == STRM_INVALID) {
        return qfalse;
    }
#ifndef _WIN32
    {
        int one = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const void*)&one, sizeof(one));
    }
#endif
    Com_Memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) != 0 || listen(s, 8) != 0) {
        STRM_CLOSE(s);
        return qfalse;
    }
    CL_StreamerNonBlocking(s);
    stListen = s;
    stListenPort = port;
    return qtrue;
}

static qboolean CL_StreamerGameReady(void) {
    return cgvm && cls.cgameStarted && clc.state == CA_ACTIVE;
}

static void CL_StreamerBuildState(void) {
    stStateLen = 0;
    if (CL_StreamerGameReady()) {
        VM_Call(cgvm, CG_STREAMER_STATE);
    }
    if (!stStateLen) {
        Q_strncpyz(stState, stOffline, sizeof(stState));
        stStateLen = (int)strlen(stState);
    }
}

static void CL_StreamerQueueEvent(stConn_t* c, const char* type, const char* json, int len) {
    CL_StreamerQueueText(c, "event: ");
    CL_StreamerQueueText(c, type);
    CL_StreamerQueueText(c, "\ndata: ");
    CL_StreamerQueue(c, json, len);
    CL_StreamerQueueText(c, "\n\n");
}

static void CL_StreamerBroadcast(const char* type, const char* json, int len) {
    int i;

    for (i = 0; i < STRM_MAX_CONNS; i++) {
        if (stConns[i].kind == STRM_EVENTS) {
            CL_StreamerQueueEvent(&stConns[i], type, json, len);
        }
    }
    stLastWrite = Sys_Milliseconds();
}

static void CL_StreamerHeader(const char* req, const char* name, char* out, int size) {
    const char* line = strchr(req, '\n');
    int len = (int)strlen(name);
    int i;

    out[0] = '\0';
    while (line && line[1] && line[1] != '\r' && line[1] != '\n') {
        line++;
        if (!Q_stricmpn(line, name, len) && line[len] == ':') {
            line += len + 1;
            while (*line == ' ' || *line == '\t') {
                line++;
            }
            for (i = 0; line[i] && line[i] != '\r' && line[i] != '\n' && i < size - 1; i++) {
                out[i] = line[i];
            }
            while (i > 0 && (out[i - 1] == ' ' || out[i - 1] == '\t')) {
                i--;
            }
            out[i] = '\0';
            return;
        }
        line = strchr(line, '\n');
    }
}

static qboolean CL_StreamerLocalName(const char* host) {
    const char* colon = strchr(host, ':');
    int len = colon ? (int)(colon - host) : (int)strlen(host);

    if (len != 9) {
        return qfalse;
    }
    return (!Q_stricmpn(host, "127.0.0.1", 9) || !Q_stricmpn(host, "localhost", 9)) ? qtrue : qfalse;
}

static qboolean CL_StreamerOriginAllowed(const char* origin) {
    if (!Q_stricmp(origin, "http://absolute")) {
        return qtrue;
    }
    if (!Q_stricmpn(origin, "http://", 7)) {
        return CL_StreamerLocalName(origin + 7);
    }
    if (!Q_stricmpn(origin, "https://", 8)) {
        return CL_StreamerLocalName(origin + 8);
    }
    return qfalse;
}

static void CL_StreamerCors(const char* origin, char* out, int size) {
    out[0] = '\0';
    if (origin[0]) {
        Com_sprintf(out, size,
                    "Access-Control-Allow-Origin: %s\r\n"
                    "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                    "Vary: Origin\r\n",
                    origin);
    }
}

static void CL_StreamerRespond(stConn_t* c, const char* status, const char* type, const char* body, int len,
                               const char* origin) {
    char head[1024];
    char cors[384];

    CL_StreamerCors(origin, cors, sizeof(cors));
    Com_sprintf(head, sizeof(head),
                "HTTP/1.1 %s\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %i\r\n"
                "%s"
                "Cache-Control: no-store\r\n"
                "Connection: close\r\n\r\n",
                status, type, len, cors);
    CL_StreamerQueueText(c, head);
    CL_StreamerQueue(c, body, len);
    c->kind = STRM_CLOSING;
}

static void CL_StreamerRoute(stConn_t* c) {
    char method[16];
    char path[256];
    char host[128];
    char origin[192];
    char cors[384];
    char head[640];
    char* p;
    int i;

    c->req[c->reqLen] = '\0';
    p = c->req;
    for (i = 0; *p && *p != ' ' && i < (int)sizeof(method) - 1; i++) {
        method[i] = *p++;
    }
    method[i] = '\0';
    while (*p == ' ') {
        p++;
    }
    for (i = 0; *p && *p != ' ' && *p != '?' && *p != '#' && i < (int)sizeof(path) - 1; i++) {
        path[i] = *p++;
    }
    path[i] = '\0';
    if (i > 1 && path[i - 1] == '/') {
        path[i - 1] = '\0';
    }

    CL_StreamerHeader(c->req, "Host", host, sizeof(host));
    CL_StreamerHeader(c->req, "Origin", origin, sizeof(origin));
    if ((host[0] && !CL_StreamerLocalName(host)) || (origin[0] && !CL_StreamerOriginAllowed(origin))) {
        CL_StreamerRespond(c, "403 Forbidden", "application/json", "{\"error\":\"forbidden\"}", 21, "");
        return;
    }

    if (!Q_stricmp(method, "OPTIONS")) {
        CL_StreamerRespond(c, "204 No Content", "text/plain", "", 0, origin);
        return;
    }
    if (Q_stricmp(method, "GET")) {
        CL_StreamerRespond(c, "405 Method Not Allowed", "application/json", "{\"error\":\"method\"}", 18, origin);
        return;
    }
    if (!strcmp(path, "/") || !Q_stricmp(path, "/state")) {
        CL_StreamerBuildState();
        CL_StreamerRespond(c, "200 OK", "application/json; charset=utf-8", stState, stStateLen, origin);
        return;
    }
    if (!Q_stricmp(path, "/events")) {
        CL_StreamerCors(origin, cors, sizeof(cors));
        Com_sprintf(head, sizeof(head),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/event-stream; charset=utf-8\r\n"
                    "%s"
                    "Cache-Control: no-store\r\n"
                    "Connection: keep-alive\r\n\r\n"
                    "retry: 2000\n\n",
                    cors);
        CL_StreamerQueueText(c, head);
        if (c->kind == STRM_FREE) {
            return;
        }
        c->kind = STRM_EVENTS;
        CL_StreamerBuildState();
        CL_StreamerQueueEvent(c, "state", stState, stStateLen);
        CL_StreamerCount();
        return;
    }
    CL_StreamerRespond(c, "404 Not Found", "application/json", "{\"error\":\"not found\"}", 21, origin);
}

static void CL_StreamerAccept(void) {
    stSocket_t s;
    int i;

    for (;;) {
        s = accept(stListen, NULL, NULL);
        if (s == STRM_INVALID) {
            return;
        }
        for (i = 0; i < STRM_MAX_CONNS; i++) {
            if (stConns[i].kind == STRM_FREE) {
                break;
            }
        }
        if (i == STRM_MAX_CONNS) {
            STRM_CLOSE(s);
            continue;
        }
        CL_StreamerNonBlocking(s);
        Com_Memset(&stConns[i], 0, sizeof(stConns[i]));
        stConns[i].sock = s;
        stConns[i].kind = STRM_READING;
        stConns[i].opened = Sys_Milliseconds();
    }
}

static void CL_StreamerRead(stConn_t* c) {
    char junk[1024];
    int n;

    if (c->kind != STRM_READING) {
        n = recv(c->sock, junk, sizeof(junk), 0);
        if (n == 0 || (n < 0 && !STRM_BLOCKED())) {
            CL_StreamerDrop(c);
        }
        return;
    }
    n = recv(c->sock, c->req + c->reqLen, STRM_REQ_MAX - 1 - c->reqLen, 0);
    if (n == 0 || (n < 0 && !STRM_BLOCKED())) {
        CL_StreamerDrop(c);
        return;
    }
    if (n > 0) {
        c->reqLen += n;
        c->req[c->reqLen] = '\0';
        if (strstr(c->req, "\r\n\r\n") || strstr(c->req, "\n\n")) {
            CL_StreamerRoute(c);
            return;
        }
        if (c->reqLen >= STRM_REQ_MAX - 1) {
            CL_StreamerRespond(c, "400 Bad Request", "application/json", "{\"error\":\"request\"}", 19, "");
            return;
        }
    }
    if (Sys_Milliseconds() - c->opened > STRM_READ_TIMEOUT) {
        CL_StreamerDrop(c);
    }
}

static void CL_StreamerWrite(stConn_t* c) {
    int n;

    if (c->outLen > 0) {
        n = send(c->sock, c->out, c->outLen, STRM_SEND_FLAGS);
        if (n < 0) {
            if (!STRM_BLOCKED()) {
                CL_StreamerDrop(c);
            }
            return;
        }
        if (n > 0) {
            memmove(c->out, c->out + n, c->outLen - n);
            c->outLen -= n;
        }
    }
    if (c->kind == STRM_CLOSING && c->outLen == 0) {
        CL_StreamerDrop(c);
    }
}

void CL_StreamerPublish(const char* type, const char* json) {
    int len;

    if (!type || !json || stListen == STRM_INVALID) {
        return;
    }
    len = (int)strlen(json);
    if (!Q_stricmp(type, "state")) {
        if (len >= STRM_STATE_MAX) {
            return;
        }
        Com_Memcpy(stState, json, len + 1);
        stStateLen = len;
        return;
    }
    if (strpbrk(type, "\r\n") || strpbrk(json, "\r\n")) {
        return;
    }
    CL_StreamerBroadcast(type, json, len);
}

void CL_StreamerFrame(void) {
    int i, now, rate, port;

    if (com_demoWindowParent || !cl_streamerMode) {
        return;
    }
    port = cl_streamerPort->integer;
    if (!cl_streamerMode->integer || port < 1 || port > 65535) {
        if (stListen != STRM_INVALID) {
            CL_StreamerCloseAll();
        }
        stFailedPort = 0;
        return;
    }
    if (stListen != STRM_INVALID && stListenPort != port) {
        CL_StreamerCloseAll();
    }
    if (stListen == STRM_INVALID) {
        if (stFailedPort == port) {
            return;
        }
        if (!CL_StreamerOpen(port)) {
            stFailedPort = port;
            Com_Printf(S_COLOR_YELLOW "Streamer Mode: could not open port %i. It may already be in use. Change it with cl_streamerPort.\n", port);
            return;
        }
        stFailedPort = 0;
        stNextPush = 0;
        Com_Printf("Streamer Mode: live data at http://127.0.0.1:%i/events\n", port);
    }

    CL_StreamerAccept();
    for (i = 0; i < STRM_MAX_CONNS; i++) {
        if (stConns[i].kind != STRM_FREE) {
            CL_StreamerRead(&stConns[i]);
        }
    }
    CL_StreamerCount();

    now = Sys_Milliseconds();
    if (stEventCount > 0) {
        rate = cl_streamerRate->integer;
        if (rate < 1) {
            rate = 1;
        } else if (rate > 60) {
            rate = 60;
        }
        if (now >= stNextPush) {
            stNextPush = now + 1000 / rate;
            CL_StreamerBuildState();
            CL_StreamerBroadcast("state", stState, stStateLen);
        } else if (now - stLastWrite > STRM_PING_TIME) {
            for (i = 0; i < STRM_MAX_CONNS; i++) {
                if (stConns[i].kind == STRM_EVENTS) {
                    CL_StreamerQueueText(&stConns[i], ": ping\n\n");
                }
            }
            stLastWrite = now;
        }
    }

    for (i = 0; i < STRM_MAX_CONNS; i++) {
        if (stConns[i].kind != STRM_FREE) {
            CL_StreamerWrite(&stConns[i]);
        }
    }
    CL_StreamerCount();
}

void CL_StreamerInit(void) {
    int i;

    for (i = 0; i < STRM_MAX_CONNS; i++) {
        stConns[i].sock = STRM_INVALID;
        stConns[i].kind = STRM_FREE;
    }
    cl_streamerMode = Cvar_Get("cl_streamerMode", "0", CVAR_ARCHIVE);
    cl_streamerPort = Cvar_Get("cl_streamerPort", "27970", CVAR_ARCHIVE);
    Cvar_CheckRange(cl_streamerPort, 1, 65535, qtrue);
    cl_streamerRate = Cvar_Get("cl_streamerRate", "10", CVAR_ARCHIVE);
    Cvar_CheckRange(cl_streamerRate, 1, 60, qtrue);
    cl_streamerClients = Cvar_Get("cl_streamerClients", "0", CVAR_ROM);
}

void CL_StreamerShutdown(void) {
    CL_StreamerCloseAll();
}
