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
   info         Libgomp support data structures declaration */

#ifndef __LIBGOMP_GLOBALS_H__
#define __LIBGOMP_GLOBALS_H__

#include "appsupport.h"

#define GOMP_WARN_NOT_SUPPORTED(what) printf("[libGOMP] " what " is not supported yet.");

/* Platform Information */
#include "libgomp_config.h"
#include "omp-lock.h"

typedef struct gomp_target_desc_s
{
    void (*fn) (void *);
    void **hostaddrs;

    unsigned int nteams;
    unsigned int threadLimit;
} gomp_target_desc_t;

gomp_target_desc_t target_desc;

/* task function type */
typedef void (*task_f)(int);

/* Threads/tasks support */
typedef struct global_infos_s
{
    /* NOTE If you change ANY dimension of these fields you have also to update macros below here */
    uint32_t thread_pool; //Busy = 1, available = 0. Big endian
    uint32_t idle_cores;
    omp_lock_t lock;
} global_infos_t;

/* Threads/tasks support */
/* This struct encapsulate a generic work-share
 * (e.g. dynamic loop, sections, etc)
 */
typedef struct gomp_work_share_s
{
    /* This is the "business" lock for the WS */
    omp_lock_t lock;
    int end;
    int next;
    int chunk_size;
    int incr;

    /* These locks are to decouple enter phase (and exit phase)
     * from the "business" phase. If only one WS is defined,
     * they are the same lock (see gomp_new_work_share()) */
    omp_lock_t enter_lock;

    //FIXME should be removed
    int checkfirst;
    uint32_t embedded;
    omp_lock_t exit_lock;
    uint32_t completed;

#ifdef OMP_NOWAIT_SUPPORT
    /* This ptr point to next_ws in case you use NOWAIT constructs */
    struct gomp_work_share_s *next_ws;
#endif
    struct gomp_work_share_s *prev_ws;


    /*This field indicates the next team descriptor on the pool of preallocated descriptor, when the current
     is not used. **/
    struct gomp_work_share_s *next_free;
} gomp_work_share_t;


/* This structure contains all of the thread-local data associated with
 * a thread team.  This is the data that must be saved when a thread
 * encounters a nested PARALLEL construct.
 */
typedef struct gomp_team_s
{
    /****************** 1) Thread Info ****************************/
    /* This is the function that any thread
     * in the team must run upon launch. */
    void (*omp_task_f) (void *data);
    void *omp_args;

    /* Nesting level.  */
    unsigned level;
    struct gomp_team_s *parent;

    /******************** 2) Team Info ****************************/
    /* This is the number of threads in the current team.  */
    unsigned nthreads;

    /* This is the team descriptor/mask */
    uint32_t team;

    /* These are the local ids assigned to processors */
    uint32_t proc_ids[MAX_PARREG_THREADS];
    uint32_t thread_ids[DEFAULT_MAXPROC];

    /******************** 3) Work Info ****************************/
    /* This is the task that the thread is currently executing.  */
    /* Team (parreg) specific locks */
    omp_lock_t critical_lock;
    omp_lock_t atomic_lock;
    uint32_t barrier_id;

    gomp_work_share_t root_ws;

#ifdef OMP_NOWAIT_SUPPORT
    gomp_work_share_t *work_share[DEFAULT_MAXPROC];
#else
    gomp_work_share_t *work_share;
#endif

    /*This field indicates the next team descriptor on the pool of preallocated descriptor, when the current
     is* not used. */
    struct gomp_team_s *next;
} gomp_team_t;

#define OMP_SLAVE_EXIT ((gomp_team_t * volatile) 0xfeedbeef)

typedef struct{
} omp_t;


#define MAX_TEAM_DESC           NUM_HW_BARRIER
#define MAX_WS                  ( 2 )

typedef struct gomp_data_s
{
    char                  barriers[SWBAR_SIZE];
    global_infos_t        thread_pool_info;
    gomp_team_t *         curr_team[DEFAULT_MAX_PE];
    gomp_team_t *         lru_team;
    omp_lock_t            team_pool_lock;
    gomp_team_t *         team_pool_list;
    omp_lock_t            ws_pool_lock;
    gomp_work_share_t *   ws_pool_list;

    gomp_team_t           team_descriptor_pool[NUM_HW_BARRIER];
    gomp_work_share_t     ws_descriptor_pool[MAX_WS];
} gomp_data_t;

gomp_data_t gomp_data __attribute__((section(".libgomp")));

#define get_barrier_offset()          ((size_t)&(((gomp_data_t *)0)->barriers))
#define get_thread_pool_offset()      ((size_t)&(((gomp_data_t *)0)->thread_pool))
#define get_curr_team_offset()        ((size_t)&(((gomp_data_t *)0)->curr_team))
#define get_team_pool_lock_offset()   ((size_t)&(((gomp_data_t *)0)->team_pool_lock))
#define get_team_pool_list_offset()   ((size_t)&(((gomp_data_t *)0)->team_pool_list))
#define get_ws_pool_lock_offset()     ((size_t)&(((gomp_data_t *)0)->ws_pool_lock))
#define get_ws_pool_list_offset()     ((size_t)&(((gomp_data_t *)0)->ws_pool_list))

/* Statically allocated global variables
 * (to avoid declaring a "real" global variable */
#define GLOBAL_INFOS_BASE       ( LIBGOMP_LBASE + SWBAR_SIZE )
#define GLOBAL_INFOS_SIZE       ( sizeof(global_infos_t) )

#define GLOBAL_THREAD_POOL      ( *((uint32_t*) ( GLOBAL_INFOS_BASE )) )
#define GLOBAL_IDLE_CORES_ADDR  ( GLOBAL_INFOS_BASE + SIZEOF_UNSIGNED)
#define GLOBAL_IDLE_CORES       ( *((uint32_t *) GLOBAL_IDLE_CORES_ADDR) )
#define GLOBAL_LOCK_ADDR        ( GLOBAL_INFOS_BASE + SIZEOF_UNSIGNED + SIZEOF_PTR )
#define GLOBAL_LOCK             ( (omp_lock_t *) GLOBAL_LOCK_ADDR )

#define GLOBAL_INFOS_WAIT()     gomp_hal_lock( GLOBAL_LOCK )
#define GLOBAL_INFOS_SIGNAL()   gomp_hal_unlock( GLOBAL_LOCK )

#define CURR_TEAM_ADDR          ( GLOBAL_INFOS_BASE + GLOBAL_INFOS_SIZE )
#define CURR_TEAM_PTR(pid)      ( (gomp_team_t *volatile*) (CURR_TEAM_ADDR + (pid << 2)) )
#define CURR_TEAM(pid)          ( *CURR_TEAM_PTR(pid) )
#define CURR_TEAM_SIZE          ( SIZEOF_PTR * DEFAULT_MAX_PE )

#define CURR_LEAGUE(team)       ( * ((gomp_team_t *volatile*)(LIBGOMP_BASE + get_cluster_offset(team) + get_curr_team_offset())))

//Team Descriptor Pre-allocated Pool
#define TEAMMEM_LOCK_ADDR       ( CURR_TEAM_ADDR + CURR_TEAM_SIZE )
#define TEAMMEM_LOCK            ( (omp_lock_t *) (TEAMMEM_LOCK_ADDR) )
#define TEAMMEM_LOCK_SIZE       ( SIZEOF_PTR)
#define TEAMMEM_LOCK_WAIT()     gomp_hal_lock( TEAMMEM_LOCK )
#define TEAMMEM_LOCK_SIGNAL()   gomp_hal_unlock( TEAMMEM_LOCK )

#define TEAMMEM_FREE_LIST_ADDR  ( TEAMMEM_LOCK_ADDR + TEAMMEM_LOCK_SIZE )
#define TEAMMEM_FREE_LIST       ( *((gomp_team_t  **) ( TEAMMEM_FREE_LIST_ADDR )) )
#define TEAMMEM_FREE_LIST_SIZE  ( SIZEOF_PTR)

//Workshare Pool Pre-allocated Pool
#define WSMEM_LOCK_ADDR         ( TEAMMEM_FREE_LIST_ADDR + TEAMMEM_FREE_LIST_SIZE )
#define WSMEM_LOCK              ( (omp_lock_t *) (WSMEM_LOCK_ADDR) )
#define WSMEM_LOCK_SIZE         ( SIZEOF_PTR )
#define WSMEM_LOCK_WAIT()       gomp_hal_lock( WSMEM_LOCK )
#define WSMEM_LOCK_SIGNAL()     gomp_hal_unlock( WSMEM_LOCK )

#define WSMEM_FREE_LIST_ADDR    ( WSMEM_LOCK_ADDR + WSMEM_LOCK_SIZE )
#define WSMEM_FREE_LIST         ( *((gomp_work_share_t  **) ( WSMEM_FREE_LIST_ADDR )) )
#define WSMEM_FREE_LIST_SIZE    ( SIZEOF_PTR )

#ifdef OMP_NOWAIT_SUPPORT
#define CURR_WS(pid)            ( ((gomp_team_t *) CURR_TEAM(pid))->work_share[( pid )] )
#else
#define CURR_WS(pid)            ( ((gomp_team_t *) CURR_TEAM(pid))->work_share )
#endif

// #define OMP_LIBGOMP_DEBUG

/*** Gloabal Threads Pool APIs ***/
ALWAYS_INLINE void
gomp_print_thread_info ( )
{
#ifdef OMP_LIBGOMP_DEBUG
    printf("---- Thread Pool ---\n");
    printf("Thread Pool Bitmask:\t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.thread_pool_info.thread_pool,
           &(gomp_data.thread_pool_info.thread_pool),
           GLOBAL_INFOS_BASE);
    printf("Thread Pool #Idles: \t  %8d\t(0x%08x == 0x%08x)\n" , gomp_data.thread_pool_info.idle_cores,
           &(gomp_data.thread_pool_info.idle_cores ),
           GLOBAL_IDLE_CORES_ADDR);
    printf("Thread Pool Lock:   \t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.thread_pool_info.lock,
           &(gomp_data.thread_pool_info.lock),
           GLOBAL_LOCK_ADDR);

    printf("---- Team Descriptor Pool ---\n");
    printf("Team Pointers:      \t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.curr_team,
           &gomp_data.curr_team,
           CURR_TEAM_ADDR);
    printf("Team Desc Pool Lock:\t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.team_pool_lock,
           &gomp_data.team_pool_lock,
           TEAMMEM_LOCK_ADDR);
    printf("Team Desc Pool List:\t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.team_pool_list,
           &gomp_data.team_pool_list,
           TEAMMEM_FREE_LIST_ADDR);
    printf("Team Desc Pool Addr:\t0x%08x to 0x%08x (Last 0x%08x)\n",
           &gomp_data.team_descriptor_pool ,
           ((int32_t) &gomp_data.team_descriptor_pool[NUM_HW_BARRIER-1]) + sizeof(gomp_team_t),
           (int32_t) &gomp_data.team_descriptor_pool[NUM_HW_BARRIER-1]);

    printf("---- WS Descriptor Pool ---\n");
    printf("WS Pool Lock:       \t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.ws_pool_lock,
           &gomp_data.ws_pool_lock,
           WSMEM_LOCK_ADDR);
    printf("WS Desc Pool List:  \t0x%08x\t(0x%08x == 0x%08x)\n", gomp_data.ws_pool_list,
           &gomp_data.ws_pool_list,
           WSMEM_FREE_LIST_ADDR);
    printf("WS Desc Pool Addr:  \t0x%08x to 0x%08x (Last 0x%08x)\n", &gomp_data.ws_descriptor_pool,
           ((int32_t) &gomp_data.ws_descriptor_pool[MAX_WS-1]) + sizeof(gomp_work_share_t),
           (int32_t) &gomp_data.ws_descriptor_pool[MAX_WS-1]);
#endif
}

ALWAYS_INLINE uint32_t *
gomp_get_thread_pool_ptr ( )
{
    return &gomp_data.thread_pool_info.thread_pool;
}

ALWAYS_INLINE uint32_t
gomp_get_thread_pool ( )
{
    return gomp_data.thread_pool_info.thread_pool;
}

ALWAYS_INLINE void
gomp_set_thread_pool ( uint32_t thread_pool)
{
    gomp_data.thread_pool_info.thread_pool = thread_pool;
}

ALWAYS_INLINE void
gomp_alloc_thread_pool ( uint32_t thread_pool)
{
    gomp_data.thread_pool_info.thread_pool |= thread_pool;
}

ALWAYS_INLINE void
gomp_dealloc_thread_pool ( uint32_t thread_pool)
{
    gomp_data.thread_pool_info.thread_pool &= ~thread_pool;
}

ALWAYS_INLINE uint32_t
gomp_get_thread_pool_idle_cores ( )
{
    return gomp_data.thread_pool_info.idle_cores;
}

ALWAYS_INLINE void
gomp_set_thread_pool_idle_cores ( uint32_t idle_cores)
{
    gomp_data.thread_pool_info.idle_cores = idle_cores;
}

ALWAYS_INLINE void
gomp_atomic_add_thread_pool_idle_cores ( uint32_t nbCores)
{
    gomp_hal_lock(&gomp_data.thread_pool_info.lock);
    gomp_data.thread_pool_info.idle_cores += nbCores;
    gomp_hal_unlock(&gomp_data.thread_pool_info.lock);
}

ALWAYS_INLINE void
gomp_atomic_del_thread_pool_idle_cores ( uint32_t nbCores)
{
    gomp_hal_lock(&gomp_data.thread_pool_info.lock);
    gomp_data.thread_pool_info.idle_cores -= nbCores;
    gomp_hal_unlock(&gomp_data.thread_pool_info.lock);
}

ALWAYS_INLINE void
gomp_thread_pool_lock ( )
{
    gomp_hal_lock(&gomp_data.thread_pool_info.lock);
}

ALWAYS_INLINE void
gomp_thread_pool_unlock ( )
{
    gomp_hal_unlock(&gomp_data.thread_pool_info.lock);
}

ALWAYS_INLINE void
gomp_thread_pool_lock_init ( )
{
    gomp_data.thread_pool_info.lock = (omp_lock_t) 0x0U;
}

ALWAYS_INLINE void
gomp_set_curr_team ( uint32_t pid,
                gomp_team_t * new_team)
{
    gomp_data.curr_team[pid] = new_team;
#ifdef OMP_LIBGOMP_GLOBALS_DEBUG
    printf("[%d][%d][gomp_set_curr_team] 0x%x (@0x%x)\n", get_cl_id(), get_proc_id(), gomp_data.curr_team[pid], &gomp_data.curr_team[pid]);
#endif
}

ALWAYS_INLINE gomp_team_t *
gomp_get_curr_team ( uint32_t pid )
{
#ifdef OMP_LIBGOMP_GLOBALS_DEBUG
    printf("[%d][%d][gomp_get_curr_team] 0x%x (@0x%x)\n", get_cl_id(), get_proc_id(), gomp_data.curr_team[pid], &gomp_data.curr_team[pid]);
#endif
    return gomp_data.curr_team[pid];
}


ALWAYS_INLINE gomp_team_t *
gomp_get_lru_team ( )
{
    return gomp_data.lru_team;
}

ALWAYS_INLINE void
gomp_set_lru_team ( gomp_team_t * team)
{
    gomp_data.lru_team = team;
}

#endif // __LIBGOMP_GLOBALS_H__
