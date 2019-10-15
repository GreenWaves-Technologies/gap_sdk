/* Select()-based ae.c module.
 *
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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


#include <string.h>

typedef struct aeApiState {
    fd_set rfds, wfds;
    /* We need to have a copy of the fd sets as it's not safe to reuse
     * FD sets after select(). */
    fd_set _rfds, _wfds;
} aeApiState;

static int aeApiCreate(aeEventLoop *eventLoop) {
    aeApiState *state = zmalloc(sizeof(aeApiState));

    if (!state) return -1;
    FD_ZERO(&state->rfds);
    FD_ZERO(&state->wfds);
    eventLoop->apidata = state;
    return 0;
}

static void aeApiFree(aeEventLoop *eventLoop) {
    zfree(eventLoop->apidata);
}

#define YESNO(__mask, __flag) ((__mask & __flag)?"yes":"no")

static int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;
    // printf("set fd %d read %s write %s\n", fd, YESNO(mask, AE_READABLE), YESNO(mask, AE_WRITABLE));
    if (mask & AE_READABLE) FD_SET(fd,&state->rfds); else FD_CLR(fd,&state->rfds);
    if (mask & AE_WRITABLE) FD_SET(fd,&state->wfds); else FD_CLR(fd,&state->wfds);
    return 0;
}

static void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;

    // printf("clear fd %d read %s write %s\n", fd, YESNO(mask, AE_READABLE), YESNO(mask, AE_WRITABLE));
    if (mask & AE_READABLE) FD_CLR(fd,&state->rfds);
    if (mask & AE_WRITABLE) FD_CLR(fd,&state->wfds);
}

static int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp) {
    aeApiState *state = eventLoop->apidata;
    int retval, numevents = 0;

    memcpy(&state->_rfds,&state->rfds,sizeof(fd_set));
    memcpy(&state->_wfds,&state->wfds,sizeof(fd_set));

#ifndef _WIN32
    retval = select(eventLoop->maxfd+1,
                &state->_rfds,&state->_wfds,NULL,tvp);
#else
    retval = select(0,
                &state->_rfds,&state->_wfds,NULL,tvp);
#endif

    if (retval > 0) {
        aeFileEvent *fe, *tmp;
        DL_FOREACH_SAFE(eventLoop->events, fe, tmp) {
            int mask = 0;
            // if (FD_ISSET(fe->fd,&state->_rfds) || FD_ISSET(fe->fd,&state->_wfds))
            //     printf("fd %d read %d write %d\n", fe->fd, FD_ISSET(fe->fd,&state->_rfds), FD_ISSET(fe->fd,&state->_wfds));
            if (fe->mask == AE_NONE) continue;
            if (fe->mask & AE_READABLE && FD_ISSET(fe->fd,&state->_rfds))
                mask |= AE_READABLE;
            if (fe->mask & AE_WRITABLE && FD_ISSET(fe->fd,&state->_wfds))
                mask |= AE_WRITABLE;
            
            if (mask) {
                // add them all in sequence
                fe->firedIdx = eventLoop->numFired++;
                eventLoop->fired[fe->firedIdx] = (aeEvent *) fe;
                fe->fired_mask = mask;
                // printf("fired fd %d read %s write %s\n", fe->fd, YESNO(mask, AE_READABLE), YESNO(mask, AE_WRITABLE));
                numevents++;
            }
        }
    }
    return numevents;
}

static char *aeApiName(void) {
    return "select";
}
