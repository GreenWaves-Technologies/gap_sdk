/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 * 
 * Additions for Windows select support, Async Event
 * Copyright (c) 2018, GreenWaves Technologies SA
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

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ae.h"
#ifdef USE_ZMALLOC
#include "zmalloc.h"
#else
#define zmalloc malloc
#define zfree free
#define zrealloc realloc
#endif

#ifndef min
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })
#endif
#ifndef max
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#endif

#include "ae_select.c"

int aeCreateAsyncSocket(aeEventLoop *eventLoop);

aeEventLoop *aeCreateEventLoop(int maxFired, void * loopdata, int flags, aeLoopStopProc *loopStopProc) {
    aeEventLoop *eventLoop;
    if ((eventLoop = zmalloc(sizeof(*eventLoop))) == NULL) goto err;
    eventLoop->events = NULL;
    eventLoop->maxFired = maxFired;
    eventLoop->fired = zmalloc(sizeof(aeEvent *) * maxFired);
    if (eventLoop->fired == NULL) goto err;
    eventLoop->numFired = 0;
    gettimeofday(&eventLoop->lastTime, NULL);
    eventLoop->timeEvents = NULL;
    eventLoop->asyncEvents = NULL;
    eventLoop->nextEventId = 1;
    eventLoop->stop = 0;
#ifndef _WIN32
    eventLoop->maxfd = -1;
#endif
    eventLoop->loopdata = loopdata;
    eventLoop->beforesleep = NULL;
    eventLoop->aftersleep = NULL;
    eventLoop->maximumWait = DEFAULT_MAXIMUM_WAIT;
    eventLoop->flags = flags;
    eventLoop->loopStopProc = loopStopProc;

    if (aeApiCreate(eventLoop) == -1) goto err;
    if (aeCreateAsyncSocket(eventLoop)) goto err;
    return eventLoop;

err:
    if (eventLoop) {
        zfree(eventLoop->fired);
        zfree(eventLoop);
    }
    return NULL;
}

void aeStopWhenEmpty(aeEventLoop *eventLoop, int yes) {
    if (yes) {
        eventLoop->flags |= AE_STOP_WHEN_EMPTY;
    } else {
        eventLoop->flags &= ~AE_STOP_WHEN_EMPTY;
    }
}

void aeAsyncSocketReadable(aeEventLoop * eventLoop, socket_t fd, void * UNUSED(state), int events) {
    if (!(events & AE_READABLE)) return;

    aeAsyncEvent * ae;
    while (recv(fd, ((char*)&ae), sizeof(void *), 0) == sizeof(void *)) {
        DL_APPEND(eventLoop->asyncEvents, ae);
    }
}

int aeSetBlocking(socket_t fd, int blocking)
{
#ifdef _WIN32
  unsigned long mode = blocking ? 0 : 1;
  return ioctlsocket(fd, FIONBIO, &mode) == 0;
#else
  if (fd < 0) {
    return 0;
  }
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    return 0;
  }

  flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

  return fcntl(fd, F_SETFL, flags) == 0;
#endif
}

int aeCreateAsyncSocket(aeEventLoop *eventLoop) {
    struct sockaddr_in si;
    socket_t sockfd;

#ifdef _WIN32
    WSAStartup(MAKEWORD(1,1), &eventLoop->wsa_data);
#endif
    if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) return 1;

#ifndef _WIN32 // see http://itamarst.org/writings/win32sockets.html
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return 1;
#endif

    memset((char *) &eventLoop->asyncSocketAddr, 0, sizeof(si));
    eventLoop->asyncSocketAddr.sin_family = AF_INET;
    eventLoop->asyncSocketAddr.sin_port = htons(0);
    eventLoop->asyncSocketAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if(bind(sockfd, (struct sockaddr*)&eventLoop->asyncSocketAddr, sizeof(eventLoop->asyncSocketAddr) ) == SOCKET_ERROR) return 1;
    
    socklen_t len = sizeof(eventLoop->asyncSocketAddr);
    if(getsockname(sockfd, (struct sockaddr*)&eventLoop->asyncSocketAddr, &len) == SOCKET_ERROR) return 1;

    if (!aeSetBlocking(sockfd, 0)) return 1;

    eventLoop->asyncSocket = sockfd;

    if (aeCreateFileEvent(eventLoop, &eventLoop->asyncSocketEv, sockfd, AE_READABLE, aeAsyncSocketReadable, NULL) != AE_OK) return 1;
    return 0;
}

int aeFireAsyncSocket(aeEventLoop *eventLoop, aeAsyncEvent * ae) {
    int len = sizeof(void *), idx = 0, retval;
    do {
        retval = sendto(eventLoop->asyncSocket, (void *)&ae, sizeof(void *), 0,
            (struct sockaddr*) &eventLoop->asyncSocketAddr, sizeof(eventLoop->asyncSocketAddr));
        if (retval == SOCKET_ERROR) {
            int plat_err = CSOCKERRNO;
            if (plat_err == SOCKERR_WOULDBLOCK || plat_err == SOCKERR_INPROGRESS) continue;
            return 0;
        }
        idx += retval;
    } while (idx < len);
    return 1;
}

void aeSignalCallCanceled(aeAsyncCallState *state);

void aeDeleteEventLoop(aeEventLoop *eventLoop) {
    aeApiFree(eventLoop);

    aeAsyncSocketReadable(eventLoop, eventLoop->asyncSocket, NULL, AE_READABLE);
#ifdef _WIN32
    closesocket(eventLoop->asyncSocket);
    WSACleanup();
#else
    close(eventLoop->asyncSocket);
#endif

    aeAsyncEvent *ae = NULL, *tmpae = NULL;

    DL_FOREACH_SAFE(eventLoop->asyncEvents, ae, tmpae) {
        // if this async event is an async call then signal the waiting function
        if ((ae->flags&AE_EVENTFLAGS_ASYNCCALL)) {
            aeSignalCallCanceled((aeAsyncCallState *) ae->clientData);
        } else {
            ae->asyncProc(eventLoop, ae->clientData, 1);
        }
        DL_DELETE(eventLoop->asyncEvents, ae);
        zfree(ae);
    }
    zfree(eventLoop->fired);
    zfree(eventLoop);
}

void aeStop(aeEventLoop *eventLoop) {
    eventLoop->stop = 1;
}

int aeCreateFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe, socket_t fd, int mask,
        aeFileProc *proc, void *clientData)
{    
    if (aeApiAddEvent(eventLoop, fd, mask) == -1)
        return AE_ERR;

    fe->event_type = AE_TYPE_FILE;
    fe->fd = fd;
    fe->mask = mask;
    fe->rfileProc = proc;
    fe->wfileProc = proc;
    fe->clientData = clientData;
    fe->firedIdx = -1;

#ifndef _WIN32
    if (fd > eventLoop->maxfd)
        eventLoop->maxfd = fd;
#endif
    DL_APPEND(eventLoop->events, fe);
    return AE_OK;
}

int aeSetMaskFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe, int mask)
{
    if (aeApiAddEvent(eventLoop, fe->fd, mask) == -1)
        return AE_ERR;
    fe->mask = mask;
    return AE_OK;
}

int aeDeleteFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe)
{
    if (!fe->prev) return AE_ERR;
    // printf("aeDeleteFileEvent delete called %p\n", fe);
    DL_DELETE(eventLoop->events, fe);
    fe->next = fe->prev = NULL;

    if (fe->firedIdx >= 0) {
        eventLoop->fired[fe->firedIdx] = NULL;
        fe->firedIdx = -1;
    }

    if (fe->mask & AE_WRITABLE) fe->mask |= AE_BARRIER;
    aeApiDelEvent(eventLoop, fe->fd, fe->mask);
    fe->mask = AE_NONE;
#ifndef _WIN32
    if (fe->fd == eventLoop->maxfd) {
        /* Update the max fd */
        eventLoop->maxfd = -1;
        DL_FOREACH(eventLoop->events, fe) {
            if (fe->fd > eventLoop->maxfd) eventLoop->maxfd = fe->fd;
        }
    }
#endif
    return AE_OK;
}

void aeAddMicroSeconds(long long usecs, struct timeval *a, struct timeval *result)
{
    usecs += a->tv_usec;
    result->tv_sec = a->tv_sec + (usecs / 1000000);
    result->tv_usec = (usecs % 1000000);
}

aeAsyncEvent * aeCreateAsyncEventInt(aeEventLoop *eventLoop, aeAsyncProc *proc, void *clientData, int flags)
{
    aeAsyncEvent * ae = zmalloc(sizeof(aeAsyncEvent));
    if (!ae) {
        return NULL;
    }
    memset(ae, 0, sizeof(aeAsyncEvent));
    ae->event_type = AE_TYPE_ASYNC;
    ae->asyncProc = proc;
    ae->clientData = clientData;
    ae->flags = flags;
    ae->firedIdx = -1;

    // printf("ae create %p\n", ae);
    if (!aeFireAsyncSocket(eventLoop, ae)) {
        zfree(ae);
        return NULL;
    }

    return ae;
}

int aeCreateAsyncEvent(aeEventLoop *eventLoop, aeAsyncProc *proc, void *clientData)
{
    aeAsyncEvent *ae = aeCreateAsyncEventInt(eventLoop, proc, clientData, 0);
    return(ae?AE_OK:AE_ERR);
}

void aeAsyncCallCompleteFunction(aeAsyncCallState *state)
{
    pthread_mutex_lock(&state->mutex);
    state->flags |= AE_CALL_COMPLETED;
    pthread_cond_signal(&state->cond);
    pthread_mutex_unlock(&state->mutex);
}

void aeAsyncCallComplete(aeAsyncCallState *state, void *result)
{
    state->result = result;
    state->complete(state);
}

void aeAsyncCallStart(struct aeEventLoop *eventLoop, void *clientData, int UNUSED(cancelled))
{
    aeAsyncCallState *state = (aeAsyncCallState *) clientData;
    state->asyncProc(eventLoop, state->clientData, state);
}

// Queues async call
// AsyncEvent runs aeAsyncCallStart which calls proc with a 'closure' that can be called at the end of
// execution. This signals aeAsyncCall to continue.
void * aeAsyncCall(aeEventLoop *eventLoop, aeAsyncCallProc *proc, void *clientData)
{
    aeAsyncCallState state;
    memset(&state, 0, sizeof(aeAsyncCallState));
    pthread_mutex_init(&state.mutex, NULL);
    pthread_cond_init(&state.cond, NULL);
    state.asyncProc = proc;
    state.clientData = clientData;
    state.complete = aeAsyncCallCompleteFunction;
    state.result = NULL;
    pthread_mutex_lock(&state.mutex);
    aeCreateAsyncEventInt(eventLoop, aeAsyncCallStart, (void *) &state, AE_EVENTFLAGS_ASYNCCALL);
    while (!(state.flags&AE_CALL_COMPLETED)) pthread_cond_wait(&state.cond, &state.mutex);
    pthread_mutex_unlock(&state.mutex);
    pthread_mutex_destroy(&state.mutex);
    pthread_cond_destroy(&state.cond);
    return (state.flags&AE_CALL_CANCELLED?NULL:state.result);
}

void aeSignalCallCanceled(aeAsyncCallState *state) {
    pthread_mutex_lock(&state->mutex);
    state->flags = (AE_CALL_CANCELLED|AE_CALL_COMPLETED);
    pthread_cond_signal(&state->cond);
    pthread_mutex_unlock(&state->mutex);
}

void aeCancelPendingCalls(aeEventLoop *eventLoop) {
    aeAsyncEvent *ae, *tmpae;
    DL_FOREACH_SAFE(eventLoop->asyncEvents, ae, tmpae) {
        // if this async event is an async call then signal the waiting function
        if ((ae->flags&AE_EVENTFLAGS_ASYNCCALL)) {
            aeSignalCallCanceled((aeAsyncCallState *) ae->clientData);
            DL_DELETE(eventLoop->asyncEvents, ae);
            zfree(ae);
        }
    }
}

int aeCmpTimeval(struct timeval * a, struct timeval * b)
{
    if (a->tv_sec < b->tv_sec)
        return -1;
    else if (a->tv_sec > b->tv_sec)
        return 1;
    else if (a->tv_usec < b->tv_usec)
        return -1;
    else if (a->tv_usec > b->tv_usec)
        return 1;
    else
        return 0;
}

int aeTimeEventIsEarlier(aeTimeEvent *a, aeTimeEvent *b) {
    // if the two are equal we want this scheduled after so not to starve events
    // printf(">>>>%lu %lu %lu %lu %d\n", a->when.tv_sec, a->when.tv_usec, b->when.tv_sec, b->when.tv_usec, aeCmpTimeval(&a->when, &b->when));
    return ((aeCmpTimeval(&a->when, &b->when) < 0)?-1:1); 
}

int aeCreateTimeEvent(aeEventLoop * UNUSED(eventLoop), aeTimeEvent *te, aeTimeProc *proc, void *clientData, aeDeleteTimeProc *deleteProc, void *deleteClientData)
{
    if (te == NULL) return AE_ERR;

    memset(te, 0, sizeof(aeTimeEvent));

    te->timeProc = proc;
    te->deleteProc = deleteProc;
    te->clientData = clientData;
    te->deleteClientData = deleteClientData;
    te->event_type = AE_TYPE_TIME;
    te->firedIdx = -1;

    return AE_OK;
}

int aeSetTimeoutTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *te, long long usecs)
{
    struct timeval now;

    gettimeofday(&now, NULL);

    aeAddMicroSeconds(usecs, &now, &te->when);

    // If the time event is already scheduled to fire check if firing needs to be cancelled
    if (te->firedIdx != -1) {
        if (aeCmpTimeval(&now, &te->when) < 0) {
            // cancel it for this loop
            // printf("cancel te this loop\n");
            eventLoop->fired[te->firedIdx] = NULL;
            te->firedIdx = -1;
        }
    }

    // if it is not added to the loop then add it
    if (!te->prev)
        DL_APPEND(eventLoop->timeEvents, te);

    return AE_OK;
}

int aeDeleteTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *te)
{
    if (!te->prev) return AE_ERR;
    DL_DELETE(eventLoop->timeEvents, te);
    te->next = te->prev = NULL;
    // printf("aeDelete %p fired %d\n", te, te->firedIdx);
    if (te->firedIdx >= 0) {
        eventLoop->fired[te->firedIdx] = NULL;
        te->firedIdx = -1;
    }
    if (te->deleteProc) te->deleteProc(eventLoop, te->deleteClientData);
    return AE_OK;
}

static int prepareAsyncEvents(aeEventLoop *eventLoop)
{
    int numevents = 0;
    aeAsyncEvent *ae, *tmp; 
    DL_FOREACH_SAFE(eventLoop->asyncEvents, ae, tmp) {
        DL_DELETE(eventLoop->asyncEvents, ae);
        ae->firedIdx = eventLoop->numFired++;
        eventLoop->fired[ae->firedIdx] = (aeEvent *) ae;
        numevents++;
    }
    return numevents;
}

/* Process async events */
static void fireAsyncEvent(aeEventLoop *eventLoop, aeAsyncEvent *ae) {
    ae->asyncProc(eventLoop, ae->clientData, 0);
    zfree(ae);
}


static int getShortestTimeEvent(aeTimeEvent *time_events, struct timeval *ptv) {
    if (!time_events) return 0;
    ptv->tv_sec = time_events->when.tv_sec;
    ptv->tv_usec = time_events->when.tv_usec;
    aeTimeEvent *te;
    DL_FOREACH(time_events, te) {
        if (aeCmpTimeval(ptv, &te->when) > 0) {
            ptv->tv_sec = te->when.tv_sec;
            ptv->tv_usec = te->when.tv_usec;
        }
    }
    return 1;
}


/* Process time events */
static int prepareTimeEvents(aeEventLoop *eventLoop) {
    int processed = 0;
    aeTimeEvent *te;
    struct timeval now;
    gettimeofday(&now, NULL);

    /* If the system clock is moved to the future, and then set back to the
     * right value, time events may be delayed in a random way. Often this
     * means that scheduled operations will not be performed soon enough.
     *
     * Here we try to detect system clock skews, and force all the time
     * events to be processed ASAP when this happens: the idea is that
     * processing events earlier is less dangerous than delaying them
     * indefinitely, and practice suggests it is. */

    int timewarp = (aeCmpTimeval(&now, &eventLoop->lastTime) < 0);
    if (timewarp) {
        DL_FOREACH(eventLoop->timeEvents, te) {
            te->when.tv_sec = 0;
        }
    }
    
    memcpy(&eventLoop->lastTime, &now, sizeof(struct timeval));
    DL_FOREACH(eventLoop->timeEvents, te) {
        if (aeCmpTimeval(&now, &te->when) >= 0) {
            te->firedIdx = eventLoop->numFired++;
            eventLoop->fired[te->firedIdx] = (aeEvent *) te;
            processed++;
        }
    }
    return processed;
}

static void fireTimeEvent(aeEventLoop *eventLoop, aeTimeEvent *te)
{
    int firedIdx = te->firedIdx;
    long long retval = te->timeProc(eventLoop, te->clientData);
    te->firedIdx = -1;
    if (eventLoop->fired[firedIdx]) { // If the event was deleted during its execution then nothing further to do
        DL_DELETE(eventLoop->timeEvents, te);

        if (retval != AE_NOMORE) {
            // event should fire again
            struct timeval now;
            gettimeofday(&now, NULL);
            aeAddMicroSeconds(retval,&now,&te->when);
            // Append to end of list to give events at start a chance
            // This still preserves order which may not be a good idea
            DL_APPEND(eventLoop->timeEvents, te);
        } else {
            te->next = te->prev = NULL;
            if (te->deleteProc) te->deleteProc(eventLoop, te->deleteClientData);
        }
    }
}

static void fireFileEvent(aeEventLoop *eventLoop, aeFileEvent *fe)
{
    int firedIdx = fe->firedIdx;
    int fired = 0;
    /* Normally we execute the readable event first, and the writable
        * event laster. This is useful as sometimes we may be able
        * to serve the reply of a query immediately after processing the
        * query.
        *
        * However if AE_BARRIER is set in the mask, our application is
        * asking us to do the reverse: never fire the writable event
        * after the readable. In such a case, we invert the calls.
        * This is useful when, for instance, we want to do things
        * in the beforeSleep() hook, like fsynching a file to disk,
        * before replying to a client. */
    int invert = fe->mask & AE_BARRIER;

    /* Note the "fe->mask & mask & ..." code: maybe an already
        * processed event removed an element that fired and we still
        * didn't processed, so we check if the event is still valid.
        *
        * Fire the readable event if the call sequence is not
        * inverted. */
    if (!invert && fe->mask & fe->fired_mask & AE_READABLE) {
        fe->rfileProc(eventLoop,fe->fd,fe->clientData,fe->fired_mask);
        fired++;
        if (!eventLoop->fired[firedIdx]) return;
    }

    /* Fire the writable event. */
    if (fe->mask & fe->fired_mask & AE_WRITABLE) {
        if ((!fired || fe->wfileProc != fe->rfileProc) && fe->wfileProc) {
            fe->wfileProc(eventLoop,fe->fd,fe->clientData,fe->fired_mask);
            if (!eventLoop->fired[firedIdx]) return;
        }
    }

    /* If we have to invert the call, fire the readable event now
        * after the writable one. */
    if (invert && fe->mask & fe->fired_mask & AE_READABLE) {
        if (!fired || fe->wfileProc != fe->rfileProc) {
            fe->rfileProc(eventLoop,fe->fd,fe->clientData,fe->fired_mask);
            fired++;
            if (!eventLoop->fired[firedIdx]) return;
        }
    }
    fe->firedIdx = -1;
}

/* Process every pending time event, then every pending file event
 * (that may be registered by time event callbacks just processed).
 * Without special flags the function sleeps until some file event
 * fires, or when the next time event occurs (if any).
 *
 * If flags is 0, the function does nothing and returns.
 * if flags has AE_ALL_EVENTS set, all the kind of events are processed.
 * if flags has AE_FILE_EVENTS set, file events are processed.
 * if flags has AE_TIME_EVENTS set, time events are processed.
 * if flags has AE_DONT_WAIT set the function returns ASAP until all
 * if flags has AE_CALL_AFTER_SLEEP set, the aftersleep callback is called.
 * the events that's possible to process without to wait are processed.
 *
 * The function returns the number of events processed. */
int aeProcessEvents(aeEventLoop *eventLoop, int flags)
{
// #define AE_LOOP_DEBUG
#ifdef AE_LOOP_DEBUG
    {
        aeAsyncEvent *ae;
        aeTimeEvent *te;
        aeFileEvent *fe;
        int time_count, async_count, file_count;
        DL_COUNT(eventLoop->timeEvents, te, time_count);
        DL_COUNT(eventLoop->asyncEvents, ae, async_count);
        DL_COUNT(eventLoop->events, fe, file_count);
        printf("loop %p time %d file %d async %d\n", eventLoop, time_count, file_count, async_count);
    }
#endif

    /* Nothing to do? return ASAP */
    if (!(flags & AE_ALL_EVENTS)) return 0;

    // Start collecting new events here
    eventLoop->numFired = 0;
    
    /* Note that we want call select() even if there are no
     * file events to process as long as we want to process time
     * events, in order to sleep until the next time event is ready
     * to fire. But not if there are async events */

    if (eventLoop->events || ((flags & AE_TIME_EVENTS) && !(flags & AE_DONT_WAIT) && (eventLoop->asyncEvents == NULL))) {
        struct timeval tv, *tvp = &tv;
        memset(tvp, 0, sizeof(struct timeval));
        if (!eventLoop->asyncEvents) {
            struct timeval shortest;
            if (getShortestTimeEvent(eventLoop->timeEvents, &shortest)) {
                struct timeval now;
                gettimeofday(&now, NULL);

                long long usecs;
                long long secs;
                usecs = shortest.tv_usec - now.tv_usec;
                if (usecs < 0) {
                    usecs += 1000000;
                    secs = (shortest.tv_sec - 1) - now.tv_sec;
                } else {
                    secs = shortest.tv_sec - now.tv_sec;
                }

                if (secs >= 0) {
                    if (eventLoop->maximumWait > -1) {
                        long long total_usecs = usecs + 1000000 * secs;
                        if (total_usecs > eventLoop->maximumWait) {
                            tv.tv_sec = eventLoop->maximumWait / 1000000;
                            tv.tv_usec = eventLoop->maximumWait % 1000000;
                        } else {
                            tv.tv_sec = secs;
                            tv.tv_usec = usecs;
                        }
                    } else {
                        tv.tv_sec = secs;
                        tv.tv_usec = usecs;
                    }
                }
            } else {
                if (eventLoop->maximumWait > -1) {
                    tv.tv_sec = eventLoop->maximumWait/1000;
                    tv.tv_usec = (eventLoop->maximumWait % 1000)*1000;
                } else {
                    tvp = NULL;
                }
            }
        }

        // if (tvp) {
        //     printf("sleep for %ld:%ld\n", tvp->tv_sec, tvp->tv_usec);
        // } else {
        //     printf("sleep forever\n");
        // }
        /* Call the multiplexing API, will return only on timeout or when
         * some event fires. */

        aeApiPoll(eventLoop, tvp);
        /* After sleep callback. */
        if (eventLoop->aftersleep != NULL && flags & AE_CALL_AFTER_SLEEP)
            eventLoop->aftersleep(eventLoop);
    }

    if (flags & AE_ASYNC_EVENTS)
        prepareAsyncEvents(eventLoop);
    if (flags & AE_TIME_EVENTS)
        prepareTimeEvents(eventLoop);

    for (int idx=0; idx < eventLoop->numFired; idx++) {
        // printf("fireloop %d %p\n", idx, eventLoop->fired[idx]);
        if (!eventLoop->fired[idx]) continue;
        switch (eventLoop->fired[idx]->event_type) {
            case AE_TYPE_ASYNC:
                fireAsyncEvent(eventLoop, (aeAsyncEvent *) eventLoop->fired[idx]);
                break;
            case AE_TYPE_FILE:
                fireFileEvent(eventLoop, (aeFileEvent *) eventLoop->fired[idx]);
                break;
            case AE_TYPE_TIME:
                fireTimeEvent(eventLoop, (aeTimeEvent *) eventLoop->fired[idx]);
                break;
            /* no default */
        }
        // printf("fireloop %d event %p completed - now null\n", idx, eventLoop->fired[idx]);
        eventLoop->fired[idx] = NULL;
    }
    return eventLoop->numFired; /* return the number of processed file/time/async events */
}

int aeLoopEmpty(aeEventLoop *eventLoop) {
    // The one file event is the async socket
    return !((eventLoop->events && eventLoop->events->next)||(eventLoop->timeEvents)||(eventLoop->asyncEvents));
}

// /* Wait for milliseconds until the given file descriptor becomes
//  * writable/readable/exception */
// int aeWait(socket_t fd, int mask, long long usecs) {
//     struct fd_set rfds;
//     struct fd_set wfds;
//     int retmask = 0, retval;
//     FD_ZERO(&rfds);
//     FD_ZERO(&wfds);

//     if (mask & AE_READABLE) FD_SET(fd,&rfds);
//     if (mask & AE_WRITABLE) FD_SET(fd,&wfds);

//     struct timeval tv;

//     tv.tv_sec = usecs / 1000000;
//     tv.tv_usec = usecs % 1000000;

//     retval = select(fd+1,&rfds,&wfds,NULL,&tv);

//     if (retval > 0) {
//         if (FD_ISSET(fd,&rfds)) retmask |= AE_READABLE;
//         if (FD_ISSET(fd,&wfds)) retmask |= AE_WRITABLE;
//         return retmask;
//     } else {
//         return retval;
//     }
// }

void aeMain(aeEventLoop *eventLoop) {
    eventLoop->stop = 0;
    while (!eventLoop->stop) {
        if (eventLoop->beforesleep != NULL)
            eventLoop->beforesleep(eventLoop);
        aeProcessEvents(eventLoop, eventLoop->flags);
        if (aeLoopEmpty(eventLoop)&&(eventLoop->flags&AE_STOP_WHEN_EMPTY)) {
            eventLoop->stop = 1;
        }
    }
    if (eventLoop->loopStopProc) eventLoop->loopStopProc(eventLoop);
}

char *aeGetApiName(void) {
    return aeApiName();
}

void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep) {
    eventLoop->beforesleep = beforesleep;
}

void aeSetAfterSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *aftersleep) {
    eventLoop->aftersleep = aftersleep;
}

void aeMaximumWait(aeEventLoop *eventLoop, int maximumWait) {
    eventLoop->maximumWait = maximumWait;
}
