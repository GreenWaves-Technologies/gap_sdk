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
   info         iterations managent on parallel loops */

#include "libgomp.h"

ALWAYS_INLINE int
gomp_iter_dynamic_next_locked (gomp_work_share_t *ws, int * pstart, int * pend)
{
    int start, end, chunk, left;

    start = ws->next;
    if (start == ws->end)
        return 0;

    chunk = ws->chunk_size * ws->incr;

    left = ws->end - start;
    if (ws->incr < 0)
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
    *pstart = start;
    *pend = end;

    return 1;
}

ALWAYS_INLINE int
gomp_iter_dynamic_next (gomp_work_share_t *ws, int * pstart, int * pend)
{
    int start, end, chunk, left;

    gomp_hal_lock(&ws->lock);

    start = ws->next;
    if (start == ws->end)
    {
        gomp_hal_unlock (&ws->lock);
        return 0;
    }

    chunk = ws->chunk_size * ws->incr;

    left = ws->end - start;
    if (ws->incr < 0)
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

    gomp_hal_unlock (&ws->lock);

    *pstart = start;
    *pend = end;

    return 1;
}
