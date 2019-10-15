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
   info         parallel thread team implementation */

#include "libgomp.h"

// #define OMP_TEAM_DEBUG

/* Team-Cache Masks */
#define BITMASK_1THS   (0x0001U)
#define BITMASK_2THS   (0x0003U)
#define BITMASK_3THS   (0x0007U)
#define BITMASK_4THS   (0x000FU)
#define BITMASK_5THS   (0x001FU)
#define BITMASK_6THS   (0x003FU)
#define BITMASK_7THS   (0x007FU)
#define BITMASK_8THS   (0x00FFU)
#define BITMASK_9THS   (0x01FFU)
#define BITMASK_10THS  (0x03FFU)
#define BITMASK_11THS  (0x07FFU)
#define BITMASK_12THS  (0x0FFFU)
#define BITMASK_13THS  (0x1FFFU)
#define BITMASK_14THS  (0x3FFFU)
#define BITMASK_15THS  (0x7FFFU)
#define BITMASK_16THS  (0xFFFFU)

#define WS_EMBEDDED     ( 0xfeeddeadU )

/* Mask useless bits */
ALWAYS_INLINE int
__getBitmask(uint32_t nthreads)
{
    uint32_t bitmask;

    switch(nthreads)
    {
        case 1U:
            bitmask = BITMASK_1THS ;
            break;
        case 2U:
            bitmask = BITMASK_2THS ;
            break;
        case 3U:
            bitmask = BITMASK_3THS ;
            break;
        case 4U:
            bitmask = BITMASK_4THS ;
            break;
        case 5U:
            bitmask = BITMASK_5THS ;
            break;
        case 6U:
            bitmask = BITMASK_6THS ;
            break;
        case 7U:
            bitmask = BITMASK_7THS ;
            break;
        case 8U:
            bitmask = BITMASK_8THS ;
            break;
        case 9U:
            bitmask = BITMASK_9THS ;
            break;
        case 10U:
            bitmask = BITMASK_10THS;
            break;
        case 11U:
            bitmask = BITMASK_11THS;
            break;
        case 12U:
            bitmask = BITMASK_12THS;
            break;
        case 13U:
            bitmask = BITMASK_13THS;
            break;
        case 14U:
            bitmask = BITMASK_14THS;
            break;
        case 15U:
            bitmask = BITMASK_15THS;
            break;
        case 16U:
            bitmask = BITMASK_15THS;
            break;

        default:
            gomp_assert(0);
            break;
    }
    return bitmask;
}

/*** Team Descriptor Pool APIs ***/
ALWAYS_INLINE void
gomp_team_pool_lock_init ( )
{
    gomp_data.team_pool_lock = (omp_lock_t) 0x0U;
}

ALWAYS_INLINE void
gomp_team_pool_lock ( )
{
    gomp_hal_lock(&gomp_data.team_pool_lock);
}

ALWAYS_INLINE void
gomp_team_pool_unlock ( )
{
    gomp_hal_unlock(&gomp_data.team_pool_lock);
}

ALWAYS_INLINE gomp_team_t *
gomp_get_team_from_pool ( uint32_t id )
{
    return &(gomp_data.team_descriptor_pool[id]);
}

ALWAYS_INLINE void
gomp_team_pool_init ( )
{
    uint32_t i;
    gomp_team_t *team;

    /* Check Gomp Memory Integrity */
    //NOTE: in aliased architecture it has some issue...
    //gomp_assert(((uint32_t) &(gomp_data) == (uint32_t) LIBGOMP_BASE));

    gomp_team_pool_lock_init( );

    /* Get First Team */
    team = gomp_get_team_from_pool(0x0);
    team->next = (gomp_team_t *) NULL;
    team->barrier_id = 0x0U;

    /* Use the First Team as list top */
    gomp_data.team_pool_list = team;

    for(i = 1U; i < MAX_TEAM_DESC; ++i)
    {
        team = gomp_get_team_from_pool(i);
        team->barrier_id = i;
        team->root_ws.embedded = WS_EMBEDDED;
        team->next = gomp_data.team_pool_list;
        gomp_data.team_pool_list = team;
    }
}

ALWAYS_INLINE void
gomp_push_team_pool ( gomp_team_t *team )
{
    gomp_hal_lock(&gomp_data.team_pool_lock);
    #ifdef OMP_TEAM_DEBUG
    printf("[%d][%d][gomp_push_team_pool] pushing team 0x%08x; pool list head 0x%08x\n", get_cl_id(), get_proc_id(), team, gomp_data.team_pool_list);
    #endif

    team->next = gomp_data.team_pool_list;
    gomp_data.team_pool_list = team;

    #ifdef OMP_TEAM_DEBUG
    printf("[%d][%d][gomp_push_team_pool] pushed team 0x%08x; pool list head 0x%08x; pool list head next 0x%08x\n", get_cl_id(), get_proc_id(), team, gomp_data.team_pool_list, gomp_data.team_pool_list->next);
    #endif

    gomp_hal_unlock(&gomp_data.team_pool_lock);
}

ALWAYS_INLINE gomp_team_t *
gomp_pull_team_pool ( )
{
    gomp_team_t *team = (gomp_team_t *) NULL;;

    gomp_hal_lock(&gomp_data.team_pool_lock);
    team = gomp_data.team_pool_list;
    if(team != (gomp_team_t *) NULL)
        gomp_data.team_pool_list = team->next;
    gomp_hal_unlock(&gomp_data.team_pool_lock);

#ifdef OMP_TEAM_DEBUG
    printf("[%d][%d][gomp_pull_team_pool] pulled team 0x%08x\n", get_cl_id(), get_proc_id(), team);
#endif
    return team;
}

//FIXME
#if 0
ALWAYS_INLINE gomp_team_t *
gomp_malloc_team()
{
    gomp_team_t *new_team
    new_team = (gomp_team_t *) shmalloc( sizeof(gomp_team_t) );
    new_team->next = NULL;
    return new_team;
}
#endif

ALWAYS_INLINE gomp_team_t *
gomp_new_team ( )
{
    gomp_team_t *new_team;

    new_team = gomp_pull_team_pool();
    //NOTE: in aliased architecture it has some issue...
    // gomp_assert(new_team != (gomp_team_t *) NULL);
    return new_team;
}

//FIXME Actually this method do not free the exeeded teams. In future
ALWAYS_INLINE void
gomp_free_team ( gomp_team_t * team )
{
    return gomp_push_team_pool(team);
}

/* Safely get-and-decrement at most 'specified' free processors */
ALWAYS_INLINE int
gomp_resolve_num_threads ( int specified )
{
  int nthr;

  nthr = GLOBAL_IDLE_CORES + 1;

  /* If a number of threads has been specified by the user
   * and it is not bigger than max idle threads use that number
   */
  if (specified && (specified < nthr))
    nthr = specified;

  GLOBAL_IDLE_CORES -= (nthr - 1);

  return nthr;
}

ALWAYS_INLINE void
gomp_master_region_start ( __attribute__((unused)) void *fn,
                           __attribute__((unused)) void *data,
                           __attribute__((unused)) int specified,
                                                   gomp_team_t **team)
{
    uint32_t i, nprocs;
    gomp_team_t *new_team;

    nprocs    = get_num_procs();

    /* Create the team descriptor for current parreg */
    new_team = gomp_new_team();

    new_team->nthreads = nprocs;
    new_team->team = 0xFFU;

    for (i = 0; i < nprocs; ++i)
        new_team->proc_ids[i] = i;

    /* Update Team */
    new_team->level  = 0x0U;
    new_team->parent = ( gomp_team_t * ) NULL;
    gomp_set_curr_team(0, new_team);

    #ifdef OMP_TEAM_DEBUG
    printf("[%d][%d][gomp_master_region_start] New Team: 0x%08x (%d threads, Level %d), check 0x%08x\n", get_cl_id(),
           get_proc_id(), new_team, nprocs, new_team->level, CURR_TEAM(0));
    #endif
    *team = new_team;
}

ALWAYS_INLINE void
gomp_team_start (void *fn, void *data, int specified, gomp_team_t **team)
{
    uint32_t i, nprocs, pid, local_id_gen, num_threads;
    uint32_t curr_team_ptr, mask;
    gomp_team_t *new_team, *lru_team, *parent_team;

    pid = get_proc_id();

    /* Fetch free processor(s) */
    GLOBAL_INFOS_WAIT();

    num_threads = gomp_resolve_num_threads (specified);

    /* Create the team descriptor for current parreg */
    new_team = gomp_new_team();
    gomp_assert(new_team != (gomp_team_t *) NULL);

    /* Check Level */
    parent_team      = gomp_get_curr_team(pid);
    new_team->level  = parent_team->level + 1;
    new_team->parent = parent_team;

    new_team->omp_task_f = (void *)(fn);
    new_team->omp_args   = data;

    #ifdef OMP_TEAM_DEBUG
    printf("[%d][%d][gomp_team_start] New Team: 0x%08x (%d threads, Level %d), Fn 0x%08x, Args 0x%08x, parent 0x%08x (Level %d)\n", get_cl_id(),
           get_proc_id(), new_team, num_threads, new_team->level, new_team->omp_task_f, new_team->omp_args, parent_team, parent_team->level);
    #endif

    /*Reset the locks */
    //gomp_hal_unlock(&new_team->critical_lock);
    //gomp_hal_unlock(&new_team->atomic_lock);
    //gomp_hal_unlock(&new_team->barrier_lock);
    new_team->critical_lock = 0x0;
    new_team->atomic_lock   = 0x0;

    /* Init default work share */
    gomp_work_share_t *root_ws = &(new_team->root_ws);
    root_ws->lock       = 0x0;
    root_ws->enter_lock = 0x0;
    root_ws->exit_lock  = 0x0;

    root_ws->next_ws = NULL;
    root_ws->prev_ws = NULL;

    #ifdef OMP_NOWAIT_SUPPORT
    new_team->work_share[pid] = root_ws;
    #else
    new_team->work_share = root_ws;
    #endif

    lru_team = gomp_get_lru_team();
    if(new_team->level == 0x1U &&
       lru_team == new_team   &&
       lru_team->nthreads >= num_threads)
    {
        //LRU team hit
        new_team->team = __getBitmask(num_threads);
        new_team->nthreads = num_threads;
        gomp_alloc_thread_pool(new_team->team);

        #ifdef OMP_TEAM_DEBUG
        printf("[%d][%d][gomp_team_start] HIT: Level: %d, LRU: 0x%08x (%d threads), NewTeam: 0x%08x (%d threads - bitmask 0x%08x)\n", get_cl_id(),
               get_proc_id(), new_team->level, lru_team, lru_team->nthreads, new_team, new_team->nthreads, new_team->team);
        #endif
    }
    else
    {
        //LRU team miss
        new_team->team = 0x0U;
        new_team->nthreads = num_threads;
        nprocs = get_num_procs();

        /* Use the global array to fetch idle cores */
        local_id_gen = 1; // '0' is master

        num_threads--; // Decrease NUM_THREADS to account for the master
        new_team->team |= (1 << pid);
        new_team->thread_ids[pid] = 0;
        new_team->proc_ids[0] = pid;

        unsigned int *gtpool = (unsigned int *) (GLOBAL_INFOS_BASE);
        for( i=1, mask = 2, curr_team_ptr = (uint32_t) CURR_TEAM_PTR(1); /* skip p0 (global master) */
             i<nprocs && num_threads;
             i++, mask <<= 1, curr_team_ptr += 4)
        {
            if(!( *gtpool & mask))
            {
                *gtpool |= mask;

                new_team->team |= mask;
                new_team->proc_ids[local_id_gen] = i;
                new_team->thread_ids[i] = local_id_gen++;

                /* Update local team structure pointers of all processors of the team */
                *((gomp_team_t **) curr_team_ptr) = new_team;

                /* How many left? */
                num_threads--;
            } // if
        } // for

        #ifdef OMP_TEAM_DEBUG
        printf("[%d][%d][gomp_team_start] MISS: Level: %d, LRU: 0x%08x (%d threads), NewTeam: 0x%08x (%d threads - bitmask 0x%08x)\n", get_cl_id(),
               get_proc_id(), new_team->level, lru_team, lru_team->nthreads, new_team, new_team->nthreads, new_team->team);
        #endif
    }

    gomp_hal_set_hwBarrier(new_team->barrier_id, new_team->nthreads, new_team->team);
    GLOBAL_INFOS_SIGNAL();

    /* Update the parent team field */
    gomp_set_curr_team(pid, new_team);
    *team = new_team;
}

/* End team and destroy team descriptor */
ALWAYS_INLINE void
gomp_team_end()
{
    unsigned int i, neg_mask, pid, nthreads;
    gomp_team_t *the_team;

    pid = get_proc_id();
    the_team = (gomp_team_t *) CURR_TEAM(pid);
    neg_mask =~ the_team->team;

    neg_mask |= (1 << pid);
    nthreads = the_team->nthreads;

    GLOBAL_INFOS_WAIT();
    GLOBAL_IDLE_CORES += (nthreads - 1); // free all but master
    GLOBAL_THREAD_POOL &= neg_mask;
    GLOBAL_INFOS_SIGNAL();

    /* After this, the current parallel thread will be lost. Free...if we had any for MPARM */
    gomp_set_curr_team(pid, the_team->parent);

#ifndef OMP_NOWAIT_SUPPORT
    gomp_free_work_share(the_team->work_share);
#else
    gomp_free_work_share(the_team->work_share[pid]);
#endif

    gomp_set_lru_team(the_team);
    gomp_free_team(the_team);

} // gomp_team_end

/* End team and destroy team descriptor */
ALWAYS_INLINE void
gomp_init_lru_team( )
{
    uint32_t i, nprocs, pid;
    gomp_team_t *new_team;

    /* Create the team descriptor for current parreg */
    new_team = gomp_new_team();
    gomp_assert(new_team != (gomp_team_t *) NULL);

    nprocs = get_num_procs();
    pid = get_proc_id();

    new_team->nthreads = nprocs;
    new_team->thread_ids[pid] = 0;
    new_team->proc_ids[0] = pid;

    new_team->team = __getBitmask(nprocs);

    for (i = 1; i < nprocs; ++i)
    {
        gomp_set_curr_team(i, new_team);
        new_team->proc_ids[i] = i;
        new_team->thread_ids[i] = i;
    }

    gomp_set_lru_team(new_team);
    gomp_free_team(new_team);
}

/* End team and destroy team descriptor */
ALWAYS_INLINE void
gomp_init_thread(uint32_t pid, gomp_team_t *team)
{
    team->work_share[pid] = &(team->root_ws);
}
