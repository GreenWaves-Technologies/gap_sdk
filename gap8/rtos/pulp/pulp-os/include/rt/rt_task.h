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

#ifndef __RT_RT_TASK_H__
#define __RT_RT_TASK_H__


/**        
 * @ingroup groupCluster        
 */

/**        
 * @defgroup Task Cluster task scheduler
 *
 * The cluster task scheduler is an alternative to the default fork/join execution
 * model used on the cluster side. It can be used when the cluster code can be
 * better parallelized using task pipelining rather than the fork/join pattern.
 * It provides the following features:
 *   - A task is a structure which contains an entry point and arguments and
 *     can be pushed to cluster side for execution.
 *   - Once popped by a core, it will be executed until completion, and the
 *     core can then proceed with another task.
 *   - Tasks are popped by cluster cores in parallel.
 *   - Tasks can be pushed to the cluster from either the fabric contoroller or
       the cluster.
 *   - A task can be multicore. Several cores will pop it and will get assigned
 *     a different ID.
 *   - Tasks can be assigned default global stacks or assigned specific stacks.
 *   - Task termination for tasks pushed from fabric controller is managed
 *     through the usual event mechanism, and through explicit wait for tasks
 *     pushed from cluster side.
 */

/**        
 * @addtogroup Task
 * @{        
 */

/**@{*/



/** \struct rt_task_conf_t
 * \brief Task scheduler configuration structure.
 *
 * This structure is used to pass the desired task scheduler configuration to the runtime when it is opened.
 */
typedef struct
{
  void *stacks;            /*!< Stacks to be used by the cluster cores when executing the tasks. If it is NULL, and the stack size is not 0, the runtime will allocate default stacks. If it is not NULL, this should point to the lowest address of the area containing the stacks for all cores. */
  unsigned int stack_size; /*!< Stack size to be used by the cluster cores when executing the tasks. The size is the size of the stack for one core. If the size is 0, no stack is given to the cores, which means the stack will have to be provided by the tasks. */
  int nb_cores;            /*!< Number of cluster cores which will pop and execute tasks. If it is zero, this will use the maximum number of cores. */
  int cid;                 /*!< Cluster ID where the task scheduler must be created. */
} rt_task_conf_t;



/** \brief Initialize a task scheduler configuration with default values.
 *
 * This function can be called to get default values for all parameters before setting some of them.
 * The structure containing the configuration must be kept alive until the task
 * scheduler is opened.
 * Calling this function will make sure that the task scheduler is created for 
 * all cores on cluster 0, with allocated stacks of 512 bytes per core.
 *
 * \param conf A pointer to the task scheduler configuration.
 */
void rt_task_conf_init(rt_task_conf_t *conf);



/** \brief Open a cluster task scheduler.
 *
 * This function can be called to setup the specified cluster so that it is
 * ready to execute tasks.
 * This function can only be called from fabric controller side.
 *
 * \param cluster A pointer to the cluster structure. This structure must be kept alive until the task scheduler is closed.
 * \param conf A pointer to the task scheduler configuration.
 * \param event The event used for managing termination.
 * \return 0 if the operation is successfull, or -1 if it failed.
 */
int rt_task_cluster_init(rt_task_cluster_t *cluster, rt_task_conf_t *conf, rt_event_t *event);



/** \brief Close a cluster task scheduler.
 *
 * This function can be called to close a cluster task scheduler and free
 * all allocated resources. Once it is called, the cluster can be used again with the default fork/join execution model.
 * This function must be called only once no task is executing or ready to be executed in the scheduler.
 * This function can only be called from fabric controller side.
 *
 * \param cluster A pointer to the cluster structure. This structure can be freed once this call is over.
 * \param event The event used for managing termination.
 */
void rt_task_cluster_deinit(rt_task_cluster_t *cluster, rt_event_t *event);


/** \brief Initialize a task.
 *
 * This function must be called before a task is pushed to initialize it.
 * This is the only mandatory function to be called on a task before it is
 * pushed.
 * If no more function is called on the task, the task is configured as a single
 * core task with no specific stack.
 *
 * \param task A pointer to the task structure. This structure must be kept alive until the task has finished execution.
 * \param entry The task entry point.
 */
static inline void rt_task_init(rt_task_t *task, void (*entry)(rt_task_t *task, int id));



/** \brief Set the number of cores of a task.
 *
 * This function can be called to set the number of cores which will execute the
 * task. The task will remain in the queue until the specified number of cores
 * has popped the queue and each core will get a different task ID.
 *
 * \param task A pointer to the task structure.
 * \param nb_cores Number of cores
 */
static inline void rt_task_cores(rt_task_t *task, int nb_cores);



/** \brief Set the stacks of a task.
 *
 * This function can be called to set the stacks that the cores will use to
 * execute the task. This must contain one stack for each core which will execute
 * the task.
 *
 * \param task A pointer to the task structure.
 * \param stacks The lowest address of the area containing the stacks for all cores.
 * \param size The size of the stack of one core.
 */
static inline void rt_task_stacks(rt_task_t *task, void *stacks, int size);



/** \brief Push a task from fabric controller side.
 *
 * This function can be called to push a task for execution from fabric controller side.
 * Once this function is called, the task must not be modified anymore.
 * This function must be called only from fabric controller side.
 *
 * \param cluster A pointer to the cluster structure of the cluster where the task must be pushed.
 * \param task A pointer to the task structure.
 * \param event     The event used for managing termination.
 */
void rt_task_fc_push(rt_task_cluster_t *cluster, rt_task_t *task, rt_event_t *event);



/** \brief Push a task from cluster side.
 *
 * This function can be called to push a task for execution from cluster side.
 * The task is pushed to the cluster from which this function is called.
 * Once this function is called, the task must not be modified anymore.
 * This function must be called only from cluster side.
 *
 * \param task A pointer to the task structure.
 */
void rt_task_cl_push(rt_task_t *task);



/** \brief Wait for a task completion.
 *
 * This function can be called to block execution until the specified task is
 * over. This can be called only for tasks which has been pushed through rt_task_cl_push.
 * This function must be called only from cluster side.
 *
 * \param task A pointer to the task structure.
 */
void rt_task_cl_wait(rt_task_t *task);



//!@}

/**        
 * @} end of Event group        
 */



/// @cond IMPLEM

static inline void rt_task_init(rt_task_t *task, void (*entry)(rt_task_t *task, int id))
{
  task->entry = entry;
  task->nb_cores = 0;
  task->stacks = NULL;
  task->pending = 1;
}

static inline void rt_task_cores(rt_task_t *task, int nb_cores)
{
  task->nb_cores = nb_cores - 1;
  task->nb_cores_to_pop = nb_cores - 1;
  task->nb_cores_to_end = nb_cores;
}

static inline void rt_task_stacks(rt_task_t *task, void *stacks, int size)
{
  task->stacks = (void *)((int)stacks + size);
  task->stack_size = size;
}

/// @endcond

#endif