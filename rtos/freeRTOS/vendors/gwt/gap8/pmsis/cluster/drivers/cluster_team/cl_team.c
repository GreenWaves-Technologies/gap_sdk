/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#include "pmsis.h"
#include "pmsis/cluster/cluster_team/cl_team.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"

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

void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
    uint32_t team_core_mask = 0;
    uint32_t master_core_mask = (1 << ARCHI_CLUSTER_MASTER_CORE);
    uint32_t master_worker_mask = 0;
    if (nb_cores == 0)
    {
        team_core_mask = __per_cluster_data[0]->task_first->cluster_team_mask;
    }
    else
    {
        team_core_mask = ((1 << (uint32_t) nb_cores) - 1);
    }

    /* Set barriers for workers sync(application barrier), then workers + master. */
    hal_cl_eu_barrier_setup(0, team_core_mask);
    master_worker_mask = (team_core_mask | master_core_mask);
    hal_cl_eu_barrier_setup((uint32_t) ARCHI_CLUSTER_SYNC_BARR_ID, master_worker_mask);

    /* Reset team config. */
    /* Remove master core from dispatcher. */
    hal_cl_eu_dispatch_team_config(team_core_mask ^ master_core_mask);
    hal_cl_eu_dispatch_fifo_push((uint32_t) entry);
    hal_cl_eu_dispatch_fifo_push((uint32_t) arg);

    /* Is master core involved? */
    if (team_core_mask & master_core_mask)
    {
        entry(arg);
    }
    hal_cl_eu_barrier_trigger_wait_clear((uint32_t) ARCHI_CLUSTER_SYNC_BARR_ID);
}

void pi_cl_team_prepare_fork(int nb_cores)
{
    uint32_t team_core_mask = 0;
    uint32_t master_core_mask = (1 << ARCHI_CLUSTER_MASTER_CORE);
    uint32_t master_worker_mask = 0;
    if (nb_cores == 0)
    {
        team_core_mask = __per_cluster_data[0]->task_first->cluster_team_mask;
    }
    else
    {
        team_core_mask = ((1 << (uint32_t) nb_cores) - 1);
    }

    /* Set barriers for workers sync(application barrier), then workers + master. */
    hal_cl_eu_barrier_setup(0, team_core_mask);
    master_worker_mask = (team_core_mask | master_core_mask);
    hal_cl_eu_barrier_setup((uint32_t) ARCHI_CLUSTER_SYNC_BARR_ID, master_worker_mask);

    /* Reset team config. */
    hal_cl_eu_dispatch_team_config(team_core_mask ^ master_core_mask);
}

void pi_cl_team_preset_fork(void (*entry)(void *), void *arg)
{
    uint32_t master_core_mask = (1 << ARCHI_CLUSTER_MASTER_CORE);
    hal_cl_eu_dispatch_fifo_push((uint32_t) entry);
    hal_cl_eu_dispatch_fifo_push((uint32_t) arg);
    if (hal_cl_eu_barrier_team_get(0) & master_core_mask)
    {
        entry(arg);
    }
    hal_cl_eu_barrier_trigger_wait_clear((uint32_t) ARCHI_CLUSTER_SYNC_BARR_ID);
}

void pi_cl_team_barrier()
{
    hal_cl_eu_barrier_trigger_wait_clear(0);
}

void pi_cl_team_critical_enter()
{
    hal_eu_mutex_lock(0);
}

void pi_cl_team_critical_exit()
{
    hal_eu_mutex_unlock(0);
}

/* Barrier(0) used to sync team. */
int pi_cl_team_nb_cores()
{
    uint32_t barrier_team = hal_cl_eu_barrier_team_get(0);
    uint32_t team_nb_cores = 0;
    asm volatile("p.cnt %0, %1" : "=r"(team_nb_cores) : "r"(barrier_team));
    return team_nb_cores;
}
