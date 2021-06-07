/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

extern struct cluster_driver_data *__per_cluster_data[];

/*******************************************************************************
 * API implementation
 ******************************************************************************/

/* Barrier config. */
static inline uint32_t pi_cl_team_barrier_available(void)
{
    uint32_t nb_avail = 0;
    uint32_t hw_barrier_alloc = __per_cluster_data[0]->hw_barrier_alloc;
    asm volatile("p.cnt %0, %1" : "=r"(nb_avail) : "r"(hw_barrier_alloc));
    return nb_avail;
}

PI_INLINE_CL_TEAM_0 uint32_t pi_cl_team_barrier_alloc(void)
{
    int32_t barrier_id = -1;
    uint32_t nb_avail = __FF1(__per_cluster_data[0]->hw_barrier_alloc);
    if (nb_avail != 32)
    {
        barrier_id = nb_avail;
        __per_cluster_data[0]->hw_barrier_alloc &= ~(1 << barrier_id);
    }
    return barrier_id;
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_barrier_free(uint32_t barrier_id)
{
    __per_cluster_data[0]->hw_barrier_alloc |= (1 << barrier_id);
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_barrier_set(uint32_t barrier_id,
                                                uint32_t team_mask)
{
    hal_cl_eu_barrier_setup(barrier_id, team_mask);
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_barrier_wait(uint32_t barrier_id)
{
    hal_cl_eu_barrier_trigger_wait_clear(barrier_id);
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_barrier()
{
    hal_cl_eu_barrier_trigger_wait_clear((uint32_t) ARCHI_CLUSTER_LEGCY_BARR_ID);
}

static inline void __pi_cl_team_barrier_sync_set(uint32_t team_mask)
{
    hal_cl_eu_barrier_setup((uint32_t) ARCHI_CLUSTER_SYNC_BARR_ID, team_mask);
}

static inline void __pi_cl_team_barrier_sync_wait_clear(void)
{
    hal_cl_eu_barrier_trigger_wait_clear((uint32_t) ARCHI_CLUSTER_SYNC_BARR_ID);
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_barrier_cc(void)
{
    __pi_cl_team_barrier_sync_wait_clear();
}


/* Team config. */
/* Barrier(ARCHI_CLUSTER_SYNC_BARR_ID) used to sync team. */
PI_INLINE_CL_TEAM_0 int pi_cl_team_nb_cores()
{
    uint32_t barrier_team = hal_cl_eu_barrier_team_get(ARCHI_CLUSTER_SYNC_BARR_ID);
    uint32_t team_nb_cores = 0;
    asm volatile("p.cnt %0, %1" : "=r"(team_nb_cores) : "r"(barrier_team));
    return team_nb_cores;
}

static inline void __pi_cl_team_config_set(uint32_t team_mask)
{
    uint32_t master_core_mask = (1 << ARCHI_CLUSTER_MASTER_CORE);

    /* Set barriers for workers sync(application barrier). */
    hal_cl_eu_barrier_setup((uint32_t) ARCHI_CLUSTER_LEGCY_BARR_ID, team_mask | master_core_mask);

    /* Set master and worker sync barrier. */
    __pi_cl_team_barrier_sync_set(team_mask | master_core_mask);

    /* Reset team config. */
    hal_cl_eu_dispatch_team_config(team_mask ^ master_core_mask);
}

static inline void __pi_cl_team_dispatch(void (*entry)(void *), void *arg)
{
    /* Dispatch task. */
    hal_cl_eu_dispatch_fifo_push((uint32_t) entry);
    hal_cl_eu_dispatch_fifo_push((uint32_t) arg);
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
    uint32_t team_core_mask = 0;
    uint32_t master_core_mask = (1 << ARCHI_CLUSTER_MASTER_CORE);
    /* Set barrier for workers + master sync. */
    if (nb_cores != 0)
    {
        team_core_mask = ((1 << (uint32_t) nb_cores) - 1);
        __pi_cl_team_config_set(team_core_mask);
    }
    else
    {
        //team_core_mask = __per_cluster_data[0]->task_first->cluster_team_mask;
        team_core_mask = hal_cl_eu_barrier_team_get(ARCHI_CLUSTER_SYNC_BARR_ID);
    }

    /* Dispatch task to workers. */
    __pi_cl_team_dispatch(entry, arg);

    /* Is master core involved? */
    if (team_core_mask & master_core_mask)
    {
        entry(arg);
    }

    /* Master waits for workers. */
    __pi_cl_team_barrier_sync_wait_clear();
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_fork_cc(int nb_cores, void (*entry)(void *), void *arg)
{
    uint32_t team_core_mask = 0;
    /* Set barriers for workers sync(application barrier), and master workers sync. */
    if (nb_cores != 0)
    {
        team_core_mask = ((1 << (uint32_t) nb_cores) - 1);
        __pi_cl_team_config_set(team_core_mask);
    }

    /* Dispatch task to workers. */
    __pi_cl_team_dispatch(entry, arg);

    /* Master executes task. */
    entry(arg);

    /* Master waits for workers. */
    __pi_cl_team_barrier_sync_wait_clear();
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_preset_fork(void (*entry)(void *), void *arg)
{
    uint32_t master_core_mask = (1 << ARCHI_CLUSTER_MASTER_CORE);
    /* Dispatch task to workers. */
    __pi_cl_team_dispatch(entry, arg);

    if (hal_cl_eu_barrier_team_get(ARCHI_CLUSTER_LEGCY_BARR_ID) & master_core_mask)
    {
        entry(arg);
    }
    /* Master waits for workers. */
    __pi_cl_team_barrier_sync_wait_clear();
}


/* Mutex. */
PI_INLINE_CL_TEAM_0 void pi_cl_team_critical_enter()
{
    hal_cl_eu_mutex_lock(0);
}

PI_INLINE_CL_TEAM_0 void pi_cl_team_critical_exit()
{
    hal_cl_eu_mutex_unlock(0);
}
