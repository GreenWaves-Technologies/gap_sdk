/* Copyright (C) 2005-2014 Free Software Foundation, Inc.
 C ontributed by Richard Henderson <r*th@redhat.com>.

 This file is part of the GNU OpenMP Library (libgomp).

 Libgomp is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details.

 Under Section 7 of GPL version 3, you are granted additional
 permissions described in the GCC Runtime Library Exception, version
 3.1, as published by the Free Software Foundation.

 You should have received a copy of the GNU General Public License and
 a copy of the GCC Runtime Library Exception along with this program;
 see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 <http://www.gnu.org/licenses/>.  */

/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         workshare descriptor creation */

#include "libgomp.h"

#ifndef OMP_NOWAIT_SUPPORT
#warning You compiled the library without OMP_NOWAIT_SUPPORT, so nowait clause is not safe to be used.
#endif

// #define OMP_WORK_DEBUG

#define WS_INITED       ( 0xfeeddeadU )
#define WS_NOT_INITED   ( 0x0U )

#define WS_NOT_EMBEDDED ( 0x0U )

/*** Workshare Pool APIs ***/

ALWAYS_INLINE void
gomp_ws_pool_lock_init ( )
{
    gomp_data.ws_pool_lock = (omp_lock_t) 0x0U;
}

ALWAYS_INLINE void
gomp_ws_pool_lock ( )
{
    gomp_hal_lock(&gomp_data.ws_pool_lock);
}

ALWAYS_INLINE void
gomp_ws_pool_unlock ( )
{
    gomp_hal_unlock(&gomp_data.ws_pool_lock);
}

ALWAYS_INLINE gomp_work_share_t *
gomp_get_ws_from_pool ( uint32_t id )
{
    return &(gomp_data.ws_descriptor_pool[id]);
}

ALWAYS_INLINE void
gomp_ws_pool_init ( )
{
    uint32_t i;
    gomp_work_share_t *ws;

    gomp_ws_pool_lock_init( );

    /* Get First Team */
    ws = gomp_get_ws_from_pool(0x0U);
    ws->embedded = WS_NOT_EMBEDDED;
    ws->next_free = NULL;

    /* Use the First Team as list top */
    gomp_data.ws_pool_list = ws;

    for(i = 1U; i < MAX_WS; ++i)
    {
        ws = gomp_get_ws_from_pool(i);
        ws->embedded = WS_NOT_EMBEDDED;
        ws->next_free = gomp_data.ws_pool_list;
        gomp_data.ws_pool_list = ws;
    }
}

ALWAYS_INLINE void
gomp_push_ws_pool ( gomp_work_share_t *ws )
{
    gomp_hal_lock(&gomp_data.ws_pool_lock);
    ws->next_free = gomp_data.ws_pool_list;
    gomp_data.ws_pool_list = ws;
    gomp_hal_unlock(&gomp_data.ws_pool_lock);
}

ALWAYS_INLINE gomp_work_share_t *
gomp_pull_ws_pool ( )
{
    gomp_work_share_t *ws = (gomp_work_share_t *) NULL;

    gomp_hal_lock(&gomp_data.ws_pool_lock);
    ws = gomp_data.ws_pool_list;
    if(ws != NULL)
      gomp_data.ws_pool_list = ws->next_free;
    gomp_hal_unlock(&gomp_data.ws_pool_lock);

    return ws;
}

/* Mallocate a work share descriptor */
ALWAYS_INLINE gomp_work_share_t *
gomp_malloc_ws ( )
{
    gomp_work_share_t *ws = (gomp_work_share_t *)shmalloc(sizeof(gomp_work_share_t));
    ws->next_free = (gomp_work_share_t *) NULL;
    ws->embedded = WS_NOT_EMBEDDED;
    return ws;
}

/* Free a work share descriptor */
ALWAYS_INLINE void
gomp_free_ws ( gomp_work_share_t *ws )
{
    shfree(ws);
}

/* Get a work share from the pool of ws*/
ALWAYS_INLINE gomp_work_share_t *
alloc_work_share ( )
{
    gomp_work_share_t *ws = (gomp_work_share_t * ) NULL;

    ws = gomp_pull_ws_pool();
    if(ws == NULL)
        ws = gomp_malloc_ws();
    return ws;
}

/* Dealloc a work share to the pool of ws*/
ALWAYS_INLINE void
gomp_free_work_share ( gomp_work_share_t *ws )
{
    if(ws->embedded == WS_NOT_EMBEDDED)
        gomp_push_ws_pool(ws);
}

ALWAYS_INLINE gomp_work_share_t *
gomp_new_work_share ( )
{
    gomp_work_share_t *new_ws;

    new_ws = alloc_work_share();

    /*Reset the locks */

    //gomp_hal_unlock(&new_ws->lock);
    //gomp_hal_unlock(&new_ws->enter_lock);
    //gomp_hal_unlock(&new_ws->exit_lock);
    new_ws->lock       = 0x0;
    new_ws->enter_lock = 0x0;
    new_ws->exit_lock  = 0x0;

    new_ws->next_ws = NULL;
    new_ws->prev_ws = NULL;

    return new_ws;
}

ALWAYS_INLINE int
gomp_work_share_start ( gomp_work_share_t **new_ws )
{
    int ret = 0;
    uint32_t pid = get_proc_id();

    #ifndef OMP_NOWAIT_SUPPORT
    *new_ws = (gomp_work_share_t *) CURR_WS(pid);

    gomp_hal_lock(&(*new_ws)->lock); //Acquire the ws lock
    if ((*new_ws)->checkfirst != WS_INITED)
    {
        /* This section is performed only by first thread of next new_ws*/
        (*new_ws)->checkfirst = WS_INITED;

        (*new_ws)->completed = 0;
        ret = 1;
    }
    #else

    /*Multiple WS*/
    gomp_work_share_t *curr_ws = (gomp_work_share_t *) CURR_WS(pid);

#ifdef OMP_WORK_DEBUG
    printf("[%d-%d][gomp_work_share_start] curr_ws 0x%x (next 0x%x)\n", get_proc_id(), get_cl_id(), curr_ws, curr_ws->next_ws);
#endif

    gomp_hal_lock(&curr_ws->lock); //Acquire the curr_ws lock
    *new_ws = curr_ws->next_ws;

    /* Check if new ws is already allocated */
    if(*new_ws == NULL)
    {
        /* This section is performed only by first thread of next ws*/
        *new_ws = gomp_new_work_share();
        (*new_ws)->prev_ws = curr_ws;
        curr_ws->next_ws = *new_ws;
        (*new_ws)->completed = 0;
        ret = 1;
    }

    gomp_hal_lock(&(*new_ws)->lock); //acquire new ws lock
    gomp_hal_unlock(&curr_ws->lock); //release curr ws lock

    CURR_WS(pid) = *new_ws; //update curr ws pointer
    #endif

#ifdef OMP_WORK_DEBUG
    printf("[%d-%d][gomp_work_share_start] NewWS 0x%x (Prev 0x%x)\n", get_proc_id(), get_cl_id(), *new_ws, (*new_ws)->prev_ws);
#endif
    return ret;
}

ALWAYS_INLINE void
gomp_work_share_end_nowait ()
{
    uint32_t pid = get_proc_id();
    gomp_team_t *team = (gomp_team_t *) CURR_TEAM(pid);
    gomp_work_share_t *ws;

    # ifndef OMP_NOWAIT_SUPPORT
    ws = team->work_share;
    # else
    ws = team->work_share[pid];
    # endif

    gomp_hal_lock(&ws->lock);
    ws->completed++;

    if (ws->completed == team->nthreads)
    {
        #ifdef OMP_NOWAIT_SUPPORT
        //NOTE at this point no more threads point to ws->prev_ws because you
        // are sure that everyone point to ws. Thus you can free ws->prev_ws
        if(ws->prev_ws)
            gomp_free_work_share(ws->prev_ws);

        #else
        ws->checkfirst = WS_NOT_INITED;
        #endif
    }

    gomp_hal_unlock(&ws->lock);
}
