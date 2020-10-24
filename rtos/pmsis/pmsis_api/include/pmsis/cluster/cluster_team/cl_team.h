/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PMSIS_CLUSTER_CLUSTER_TEAM_CL_TEAM_H__
#define __PMSIS_CLUSTER_CLUSTER_TEAM_CL_TEAM_H__

#include "pmsis/pmsis_types.h"

#ifndef PI_INLINE_CL_TEAM_0
#define PI_INLINE_CL_TEAM_0
#endif

/**
 * \ingroup clusterDriver
 *
 * \defgroup ClusterTeam Team synchronization
 *
 * Once a cluster entry point has been entered by cluster controller core, all
 * the following primitives can be used to do multi-core processing, in a
 * fork-join manner (like OMP parallel primitive).
 *
 * The execution can first be forked in order to activate more cores, and then
 * synchronized together using barriers.
 *
 * \addtogroup ClusterTeam
 * \{
 */

/**
 * \brief Return the number of cores of the cluster.
 *
 * This will return the number of slave cores present in the cluster, available
 * to share/fork a task.
 *
 * \return Number of cores.
 */
static inline int pi_cl_cluster_nb_cores();

/**
 * \brief Fork the execution of the calling core.
 *
 * Calling this function will create a team of workers and call the specified
 * entry point on each core of the team to start multi-core processing.
 * The team parameters (number of cores and stacks) are by default the ones
 * configured when sending a task to cluster from the fabric controller.
 * It is possible to use different parameters when doing a new fork. If this is
 * done the new parameters will become the new default ones.
 *
 * \param nb_cores       Number of cores to execute the task/entry point..
 * \param entry          Function entry point to be executed by team of workers.
 * \param arg            Argument of the function entry point.
 *
 * \note If *nb_cores* is zero, fork is done reusing the cores_mask of the
 *       previous fork or the default.
 * \note If the number of cores is not provided (i.e. is zero), the number of cores
 *       of the previous fork will be reused. Doing this has less runtime overhead.
 */
void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg);

/**
 * \brief Prepare a team for a task.
 *
 * This function is called to set up a team of workers.
 * This function has the same behaviour as \ref pi_cl_team_fork(), but here the
 * fork is not done, thus a call to \ref pi_cl_team_preset_fork() is needed in
 * order to dispatch the task to the team of workers.
 *
 * \param nb_cores       Number of cores to execute the entry point
 */
void pi_cl_team_prepare_fork(int nb_cores);

/**
 * \brief Fork a the execution of the calling core.
 *
 * This function is to be called after \ref pi_cl_team_prepare_fork().
 * A call to this function will fork the function entry point between a preset
 * team of workers.
 *
 * \param entry          Function entry point to be executed by team of workers.
 * \param arg            Argument of the function entry point.
 *
 * \note Calling pi_cl_team_prepare_fork() then pi_cl_team_preset_fork() is
 *       no different than calling pi_cl_team_fork() once.
 *       But when forking multiple times with the same team of workers, calling
 *       these two functions prevent from having an overhead due to barrier
 *       setups, ie first call pi_cl_team_prepare_fork() to set the team then
 *       multiple calls to pi_cl_team_preset_fork().
 * \note Workers are exclusively slave cores.
 */
void pi_cl_team_preset_fork(void (*entry)(void *), void *arg);

/**
 * \brief Fork the execution of the calling core using task.
 *
 * This function is similar to pi_cl_team_fork but takes a task as parameter, which
 * allows setting more parameters.
 * Calling this function will create a team of workers and call the specified
 * entry point on each core of the team to start multi-core processing with
 * the team parameters specified in the task.
 *
 * \param fork_task      Task to be forked on slave cores.
 */
void pi_cl_team_fork_task(struct pi_cl_team_task *fork_task);

/**
 * \brief Execute a barrier between all cores of the team.
 *
 * This will block the execution of each calling core until all cores
 * have reached the barrier.
 * The set of cores participating in the barrier is the one created with the
 * last fork.
 * Each core of the team must execute the barrier exactly once for all cores
 * to be able to go through the barrier.
 */
PI_INLINE_CL_TEAM_0 void pi_cl_team_barrier();

/**
 * \brief Enter a critical section.
 *
 * This will block the execution of the calling core until it can execute
 * the following section of code alone.
 * This will also prevent all other cores of the team to execute
 * the following code until \ref pi_cl_team_critical_exit() is called.
 *
 * \note No runtime functions should be called from within the critical section,
 *       only application code is allowed.
 */
PI_INLINE_CL_TEAM_0 void pi_cl_team_critical_enter(void);

/**
 * \brief Exit a critical section.
 *
 * This will exit the critical code and let other cores executing it.
 */
PI_INLINE_CL_TEAM_0 void pi_cl_team_critical_exit(void);

/**
 * \brief Return the number of cores in the team.
 *
 * This will return the number of cores involved in the team created by the
 * active fork operation.
 *
 * \return The number of cores of the team.
 */
int pi_cl_team_nb_cores();


/**
 * \}
 */

#endif  /* __PMSIS_CLUSTER_CLUSTER_TEAM_CL_TEAM_H__ */
