/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * Additions for Winsock support, Blocking Async Event
 * Copyright (c) 2018, GreenWaves Technologies SA
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__


#include <sys/time.h>
#include <pthread.h>

#include "warnings.h"
#include "platform.h"
#include "uthash.h"
#include "utlist.h"

#define DEFAULT_MAXIMUM_WAIT -1

#define AE_OK 0
#define AE_ERR -1
#define AE_FLAG(__n) (1<<__n)
#define AE_NONE 0       /* No events registered. */
#define AE_READABLE AE_FLAG(0)   /* Fire when descriptor is readable. */
#define AE_WRITABLE AE_FLAG(1)   /* Fire when descriptor is writable. */
#define AE_BARRIER AE_FLAG(2)    /* With WRITABLE, never fire the event if the
                           READABLE event already fired in the same event
                           loop iteration. Useful when you want to persist
                           things to disk before sending replies, and want
                           to do that in a group fashion. */

#define AE_FILE_EVENTS AE_FLAG(0)
#define AE_TIME_EVENTS AE_FLAG(1)
#define AE_ASYNC_EVENTS AE_FLAG(2)
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS|AE_ASYNC_EVENTS)
#define AE_DONT_WAIT AE_FLAG(8)
#define AE_CALL_AFTER_SLEEP AE_FLAG(9)
#define AE_HASASYNC_EVENTS AE_FLAG(10)
#define AE_STOP_WHEN_EMPTY AE_FLAG(11)

#define AE_DEFAULT_FLAGS (AE_ALL_EVENTS|AE_CALL_AFTER_SLEEP|AE_STOP_WHEN_EMPTY)

#define AE_EVENTFLAGS_ASYNCCALL 1

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

#define AE_CALL_COMPLETED 1
#define AE_CALL_CANCELLED 2

/* Macros */
#define AE_NOTUSED(V) ((void) V)

typedef enum aeEventType {
    AE_TYPE_FILE,
    AE_TYPE_TIME,
    AE_TYPE_ASYNC,
} aeEventType;

struct aeEventLoop;
struct aeAsyncCallState;

/* Types and data structures */
typedef void aeFileProc(struct aeEventLoop *eventLoop, socket_t fd, void *clientData, int mask);
typedef long long aeTimeProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeAsyncProc(struct aeEventLoop *eventLoop, void *clientData, int cancelled);
typedef void aeAsyncCallCompleteProc(struct aeAsyncCallState *state);
typedef void aeAsyncCallProc(struct aeEventLoop *eventLoop, void *clientData, struct aeAsyncCallState *state);
typedef void aeDeleteTimeProc(struct aeEventLoop *eventLoop, void *deleteClientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);
typedef void aeLoopStopProc(struct aeEventLoop *eventLoop);

typedef struct aeEvent {
    struct aeEvent *next, *prev;
    int firedIdx;
    aeEventType event_type;
} aeEvent;

/* File event structure */
typedef struct aeFileEvent {
    struct aeFileEvent *next, *prev;
    int firedIdx;
    aeEventType event_type;
    socket_t fd;
    int mask; /* one of AE_(READABLE|WRITABLE|BARRIER) */
    int fired_mask;
    aeFileProc *rfileProc;
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

/* Time event structure */
typedef struct aeTimeEvent {
    struct aeTimeEvent *next, *prev;
    int firedIdx;
    aeEventType event_type;
    struct timeval when;
    aeTimeProc *timeProc;
    void *clientData;
    aeDeleteTimeProc *deleteProc;
    void *deleteClientData;
} aeTimeEvent;

/* Async event structure */
typedef struct aeAsyncEvent {
    struct aeAsyncEvent *next, *prev;
    int firedIdx;
    aeEventType event_type;
    aeAsyncProc *asyncProc;
    void *clientData;
    int flags;
} aeAsyncEvent;

typedef struct aeAsyncCallState {
    aeAsyncCallProc *asyncProc;
    void *clientData;
    void **result;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int flags;
    aeAsyncCallCompleteProc *complete;
} aeAsyncCallState;

/* State of an event based program */
typedef struct aeEventLoop {
#ifndef _WIN32
    socket_t maxfd;   /* highest file descriptor currently registered */
#endif
    __attribute__((aligned(8))) long long nextEventId; // Allign to guarantee atomic access
    struct timeval lastTime;     /* Used to detect system clock skew */
    aeFileEvent *events; /* Registered events */
    aeEvent **fired; /* Fired events */
    int numFired;
    int maxFired; /* maximum number of fireable events in a single loop */
    aeTimeEvent *timeEvents;
    aeAsyncEvent *asyncEvents;
    int maximumWait;
    int stop;
    void *apidata; /* This is used for polling API specific data */
    void *loopdata; /* Added for loop specific user data */
    aeBeforeSleepProc *beforesleep;
    aeBeforeSleepProc *aftersleep;
    int flags;
    socket_t asyncSocket;
    struct sockaddr_in asyncSocketAddr;
    aeFileEvent asyncSocketEv;
    aeLoopStopProc *loopStopProc;
#ifdef _WIN32
    WSADATA wsa_data;
#endif
    struct timeval in_the_future;
} aeEventLoop;

/* Prototypes */
aeEventLoop *aeCreateEventLoop(int setsize, void * loopdata, int flags, aeLoopStopProc *loopStopProc);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe, socket_t fd, int mask,
        aeFileProc *proc, void *clientData);
int aeSetMaskFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe, int mask);
int aeDeleteFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe);
int aeCreateTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *te,
        aeTimeProc *proc, void *clientData, aeDeleteTimeProc *deleteProc, void *deleteClientData);
int aeSetTimeoutTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *te, long long milliseconds);
int aeDeleteTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *te);
int aeCreateAsyncEvent(aeEventLoop *eventLoop, aeAsyncProc *proc, void *clientData);
void aeAsyncCallComplete(aeAsyncCallState *state, void *result);
void * aeAsyncCall(aeEventLoop *eventLoop, aeAsyncCallProc *proc, void *clientData);
void aeCancelPendingCalls(aeEventLoop *eventLoop);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
// int aeWait(socket_t fd, int mask, long long milliseconds);
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
void aeSetAfterSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *aftersleep);
int aeGetSetSize(aeEventLoop *eventLoop);
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);
void aeMaximumWait(aeEventLoop *eventLoop, int maximumWait);
void aeStopWhenEmpty(aeEventLoop *eventLoop, int yes);

#endif
