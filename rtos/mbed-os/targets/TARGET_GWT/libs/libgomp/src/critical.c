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
   info         #pragma omp critical-atomic implemetation */

#include "libgomp.h"

void
GOMP_atomic_start (void)
{
    uint32_t pid = get_proc_id();
    gomp_hal_lock(&(CURR_TEAM(pid)->atomic_lock));
}

void
GOMP_atomic_end (void)
{
    uint32_t pid = get_proc_id();
    gomp_hal_unlock(&(CURR_TEAM(pid)->atomic_lock));
}

void
GOMP_critical_start (void)
{
    uint32_t pid = get_proc_id();
    gomp_hal_lock(&(CURR_TEAM(pid)->critical_lock));
}

void
GOMP_critical_end (void)
{
    uint32_t pid = get_proc_id();
    gomp_hal_unlock(&(CURR_TEAM(pid)->critical_lock));
}
