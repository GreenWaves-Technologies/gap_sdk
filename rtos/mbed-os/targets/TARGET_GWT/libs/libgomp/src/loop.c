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
   info         parallel loop management */

#include "libgomp.h"

ALWAYS_INLINE void
gomp_loop_init(gomp_work_share_t *ws, int start, int end, int incr, int chunk_size)
{
    ws->chunk_size = chunk_size;
    /* Canonicalize loops that have zero iterations to ->next == ->end.  */
    ws->end = ((incr > 0 && start > end) || (incr < 0 && start < end)) ? start : end;
    ws->incr = incr;
    ws->next = start;
}

ALWAYS_INLINE int
gomp_loop_dynamic_next (gomp_work_share_t *ws, int *istart, int *iend)
{
    int ret;
    ret = gomp_iter_dynamic_next (ws, istart, iend);

    return ret;
}

/* The GOMP_loop_end* routines are called after the thread is told that
 *  all loop iterations are complete.  This first version synchronizes
 *  all threads; the nowait version does not.  */

ALWAYS_INLINE void
gomp_loop_end_nowait()
{
    gomp_work_share_end_nowait();
}


ALWAYS_INLINE void
gomp_loop_end()
{
    gomp_work_share_end_nowait();
    gomp_hal_hwBarrier( (gomp_get_curr_team ( get_proc_id( ) ))->barrier_id );
}


/*********************** APIs *****************************/

int
GOMP_loop_dynamic_start(int start, int end, int incr, int chunk_size,
                        int *istart, int *iend)
{
    int chunk, left;
    gomp_work_share_t *ws;
    int ret = 1;

    //NOTE ws return from this function already locked
    if (gomp_work_share_start(&ws))
        gomp_loop_init(ws, start, end, incr, chunk_size);

    start = ws->next;

    if (start == ws->end)
        ret = 0;

    if(ret)
    {
        chunk = chunk_size * incr;

        left = ws->end - start;
        if (incr < 0)
        {
            if (chunk < left)
                chunk = left;
        }
        else
        {
            if (chunk > left)
                chunk = left;
        }

        end = start + chunk;
        ws->next = end;
    }

    *istart = start;
    *iend   = end;

    gomp_hal_unlock (&ws->lock);

    return ret;
}

int
GOMP_loop_dynamic_next (int *istart, int *iend)
{
    int ret;
    uint32_t pid = get_proc_id();
    ret = gomp_loop_dynamic_next ((gomp_work_share_t *) CURR_WS(pid), istart, iend);

    return ret;
}

void
GOMP_parallel_loop_dynamic_start (void (*fn) (void *), void *data,
                                  unsigned num_threads, long start, long end,
                                  long incr, long chunk_size)
{
    gomp_team_t *new_team;

    gomp_team_start (fn, data, num_threads, &new_team);
    #ifdef OMP_NOWAIT_SUPPORT
    gomp_loop_init(new_team->work_share[get_proc_id()], start, end, incr, chunk_size);
    #else
    gomp_loop_init(new_team->work_share, start, end, incr, chunk_size);
    #endif
    MSGBarrier_hwRelease( new_team->team^(0x1<<new_team->proc_ids[0]) );
}

void
GOMP_parallel_loop_dynamic (void (*fn) (void *), void *data,
                            unsigned num_threads, long start, long end,
                            long incr, long chunk_size, unsigned flags)
{
    gomp_team_t *new_team;

    gomp_team_start (fn, data, num_threads, &new_team);
    #ifdef OMP_NOWAIT_SUPPORT
    gomp_loop_init(new_team->work_share[get_proc_id()], start, end, incr, chunk_size);
    #else
    gomp_loop_init(new_team->work_share, start, end, incr, chunk_size);
    #endif
    MSGBarrier_hwRelease( new_team->team^(0x1<<new_team->proc_ids[0]) );
    fn(data);
    GOMP_parallel_end();
}

void
GOMP_loop_end()
{
    gomp_loop_end();
}

void
GOMP_loop_end_nowait()
{
    gomp_loop_end_nowait();
}
