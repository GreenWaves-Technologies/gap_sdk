/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"
#include "pmsis/cluster/cluster_team/cl_team.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis_hal/pmsis_hal.h"
/**
 * @ingroup groupCluster
 */

/**
 * @defgroup Team Team synchronization primitives
 *
 * Once a cluster entry point has been entered, after it has been enqueued by the fabric controller, all the following primitives can be used
 * to do multi-core processing, in a fork-join manner (like OMP parallel primitive).
 *
 * The execution can first be forked in order to activate more cores, and then synchronized together using barriers.
 */

/**
 * @addtogroup Team
 * @{
 */

/**@{*/

static uint32_t team_nb_cores = 0;

/** \brief Fork the execution of the calling core.
 *
 * Calling this function will create a team of workers and call the specified entry point
 * to start multi-core processing.
 * The team parameters (number of cores and stacks) are by default the ones
 * configured when calling
 * the cluster entry point from the fabric controller. It is possible to use
 * different parameters
 * when doing a new fork. If this is done the new parameters will become the
 * new default ones.
 * If the number of cores is not provided (i.e. is zero), the number of cores
 * of the previous
 * fork will be reused. Doing this has less runtime overhead.
 *
 * \param        cores_mask The mask of cores which will enter the entry point.
 * If it is zero, this will reuse the cores_mask of the previous fork or the default.
 * \param        entry The function entry point to be executed by all cores of the team.
 * \param        arg    The argument of the function entry point.
 */
void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
    uint32_t core_mask = 0;
    if (!nb_cores || ((uint32_t) nb_cores == team_nb_cores))
    {
        if (!team_nb_cores)
        {
            team_nb_cores = (uint32_t) ARCHI_CLUSTER_NB_PE;
            core_mask = ((1 << team_nb_cores)-1);
            hal_eu_dispatch_team_config(core_mask & GAP_CLUSTER_WITHOUT_CORE0_MASK);
        }
        else
        {
            core_mask = ((1 << team_nb_cores)-1);
        }
    }
    else
    {
        team_nb_cores = (uint32_t) nb_cores;
        core_mask = ((1 << team_nb_cores)-1);
        hal_eu_dispatch_team_config(core_mask & GAP_CLUSTER_WITHOUT_CORE0_MASK);
    }
    hal_eu_barrier_setup(core_mask);
    hal_eu_dispatch_push((uint32_t) entry);
    hal_eu_dispatch_push((uint32_t) arg);
    entry(arg);
    hal_eu_barrier_trigger_wait_clear();
}


/** \brief Execute a barrier between all cores of the team.
 *
 * This will block the execution of each calling core until all cores have reached the barrier.
 * The set of cores participating in the barrier is the one created with the last fork.
 * Each core of the team must execute the barrier exactly once for all cores to be able to go through the barrier.
 */
void pi_cl_team_barrier(int barrier_id)
{
    hal_evt_read32((uint32_t)&EU_BARRIER_DEMUX(0)->TRIGGER_WAIT_CLEAR, 0);
}



/** \brief Enter a critical section.
 *
 * This will block the execution of the calling core until it can execute the following section of code alone.
 * This will also prevent all other cores of the team to execute the following code until
 * rt_team_critical_exit is called.
 */
void pi_cl_team_critical_enter()
{
    hal_eu_mutex_lock(0);
}



/** \brief Exit a critical section.
 *
 * This will exit the critical code and let other cores executing it..
 */
void pi_cl_team_critical_exit()
{
    hal_eu_mutex_unlock(0);
}

int pi_cl_team_nb_cores()
{
    return team_nb_cores;
}

//!@}

/**
 * @} end of Team group
 */
