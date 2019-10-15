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
   info         #pragma omp single implementation */

#include "libgomp.h"
#define WS_NOT_INITED   ( 0x0U )

void *
gomp_single_copy_start(gomp_work_share_t *ws)
{
    GOMP_WARN_NOT_SUPPORTED("#pragma omp single copyprivate");
    return NULL;
}

void
gomp_single_copy_end(gomp_work_share_t *ws, void *data)
{
    GOMP_WARN_NOT_SUPPORTED("#pragma omp single copyprivate");
}

/**************** APIs **************************/

int
GOMP_single_start(void)
{
    int ret = 0;
    uint32_t pid = get_proc_id();

    gomp_team_t *team = (gomp_team_t *) CURR_TEAM(pid);
    gomp_work_share_t *ws;

    //NOTE ws return from this function already locked
    ret = gomp_work_share_start(&ws);

    /* Faster than a call to gomp_work_share_end_nowait() */
    ws->completed++;

    if (ws->completed == team->nthreads)
    {
        ws->checkfirst = WS_NOT_INITED;

        #ifdef OMP_NOWAIT_SUPPORT
        gomp_free_work_share(ws->prev_ws);
        #endif
    }
    gomp_hal_unlock(&ws->lock);
    return ret;
}

void *
GOMP_single_copy_start(void)
{
    GOMP_WARN_NOT_SUPPORTED("#pragma omp single copyprivate");
    return NULL;
}

void
GOMP_single_copy_end(void *data)
{
    GOMP_WARN_NOT_SUPPORTED("#pragma omp single copyprivate");
    return;
}
