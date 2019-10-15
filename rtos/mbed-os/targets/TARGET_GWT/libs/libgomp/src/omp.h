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
   info         Standard public OpenMP APIs declaration */

#ifndef __OMP_H__
#define __OMP_H__

/* Standard public APIs */

/* env.c */
extern void omp_set_num_threads (int);
extern void omp_set_dynamic (int);
extern int omp_get_dynamic (void);
extern void omp_set_nested (int);
extern int omp_get_nested (void);

/* parallel.c */
extern int omp_get_num_procs(void);
extern int omp_get_num_threads(void);
extern int omp_get_max_threads(void);
extern int omp_get_thread_num(void);
extern int omp_in_parallel(void);

/* target.c */
extern int omp_get_num_teams(void);
extern int omp_get_team_num(void);

extern double omp_get_wtime(void);
extern int omp_get_wtick(void);

#include "omp-lock.h"
#include "appsupport.h"

#define MAIN            _app_main
#define PRINTF          printf
#define START_TIMER     start_timer
#define RESET_TIMER     reset_timer
#define STOP_TIMER      stop_timer
#define GET_TIME        get_time

#endif /* __OMP_H__ */
