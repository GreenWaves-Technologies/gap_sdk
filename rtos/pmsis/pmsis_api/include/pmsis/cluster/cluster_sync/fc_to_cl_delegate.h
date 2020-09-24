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

#ifndef __FC_TO_CL_H__
#define __FC_TO_CL_H__

#include "pmsis/pmsis_types.h"
#include "pmsis/cluster/cl_pmsis_types.h"

/**
 * @ingroup groupDrivers
 */

/**
 * @defgroup clusterDriver Cluster
 *
 * The cluster driver provides support for offload code execution to the cluster
 * side.
 *
 * Clusters must be mounted before they can be used. Once mounted, remote
 * function executions can be enqueued to offload computation to the cluster.
 * A remote execution is a multi-core execution. This starts execution only on
 * the cluster controller core, however the other cores of the cluster are
 * available for parallel computation through synchronization primitives.
 */


/**
 * @addtogroup clusterDriver
 * @{
 */

/**
 * @defgroup FcClusterSync FC/Cluster synchronization
 *
 * This set of functions provide support for controlling clusters from
 * fabric-controller side.
 */


/**
 * @addtogroup FcClusterSync
 * @{
 */

/**@{*/

/** \brief Initialize a cluster configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them.
 * The structure containing the configuration must be kept alive until the SPI
 * device is opened.
 *
 * \param conf A pointer to the SPI master configuration.
 */
void pi_cluster_conf_init(struct pi_cluster_conf *conf);

/** \brief Open and power-up the cluster.
 * 
 * This function must be called before the cluster device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * By default the cluster is powered down and cannot be used. Calling this
 * function will power it up.
 * At the end of the call, the cluster is ready to execute a task.
 * The caller is blocked until the operation is finished.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_cluster_open(struct pi_device *device);

/** \brief Close an opened cluster device.
 *
 * This function can be called to close an opened cluster device once it is not
 * needed anymore, in order to free all allocated resources. Once this function
 * is called, the device is not accessible anymore and must be opened
 * again before being used. This will power-down the cluster.
 * The caller is blocked until the operation is finished.
 *
 * \param device  A pointer to the structure describing the device.
 */
int pi_cluster_close(struct pi_device *device);

/** \brief Prepare a cluster task for execution.
 *
 * This initializes a cluster task before it can be sent to the cluster side
 * for execution.
 * If the same task is re-used for several executions, it must be reinitialized
 * everytime by calling this function.
 *
 * \param task  A pointer to the structure describing the task.
 *   This structure is allocated by the caller and must be kept alive until the
 *   task has finished execution.
 * \param entry  The task entry point that the cluster controller will execute.
 * \param arg The argument to the entry point.
 */
static inline struct pi_cluster_task *pi_cluster_task(
        struct pi_cluster_task *task, void (*entry)(void*), void *arg);

/** \brief Enqueue a task for execution on the cluster.
 *
 * This will enqueue the task at the end of the queue of tasks, ready to be
 * executed by the specified cluster.
 * Once the task gets scheduled, the cluster-controller core is waken-up and
 * starts executing the task entry point.
 * This function is intended to be used for coarse-grain job delegation to the
 * cluster side, and thus the stack used to execute this function can be
 * specified.
 * When the function starts executing on the cluster, the other cores of the
 * cluster are also available for parallel computation. Thus the stacks for the
 * other cores (called slave cores) can also be specified, as well as the number
 * of cores which can be used by the function on the cluster (including the
 * cluster controller).
 * The caller is blocked until the task has finished execution.
 *
 * Note that this enqueues a function execution. To allow cluster executions
 * to be pipelined, several tasks can be enqueued at the same time.
 * If more than two tasks are enqueued, as soon as the first is finished,
 * the cluster-controller core immediately continues with the next one,
 * while the fabric controller receives the termination notification and can
 * enqueue a new execution, in order to keep the cluster busy.
 *
 * \param device    A pointer to the structure describing the device.
 * \param task Cluster task structure containing task and its parameters.
 */
static inline int pi_cluster_send_task(struct pi_device *device,
        struct pi_cluster_task *task);

/** \brief Enqueue asynchronously a task for execution on the cluster.
 *
 * This will enqueue the task at the end of the queue of tasks, ready to be
 * executed by the specified cluster.
 * Once the task gets scheduled, the cluster-controller core is waken-up and
 * starts executing the task entry point.
 * This function is intended to be used for coarse-grain job delegation to the
 * cluster side, and thus the stack used to execute this function can be
 * specified.
 * When the function starts executing on the cluster, the other cores of the
 * cluster are also available for parallel computation. Thus the stacks for the
 * other cores (called slave cores) can also be specified, as well as the number
 * of cores which can be used by the function on the cluster (including the
 * cluster controller).
 * The task is just enqueued and the caller continues execution.
 * A task must be specified in order to specify how the caller should be
 * notified when the task has finished execution.
 *
 * Note that this enqueues a function execution. To allow cluster executions
 * to be pipelined, several tasks can be enqueued at the same time.
 * If more than two tasks are enqueued, as soon as the first is finished,
 * the cluster-controller core immediately continues with the next one,
 * while the fabric controller receives the termination notification and can
 * enqueue a new execution, in order to keep the cluster busy.
 *
 * \param device    A pointer to the structure describing the device.
 * \param task Cluster task structure containing task and its parameters.
 * \param end_task        The task used to notify the end of execution.
 */
static inline int pi_cluster_send_task_async(struct pi_device *device,
        struct pi_cluster_task *cluster_task,
        pi_task_t *task);

//!@}

/**
 * @}
 */

/**
 * @}
 */


/// @cond IMPLEM

/**
 * Defines for IOCTL
 */
#define SEND_TASK_ID 0
#define WAIT_FREE_ID 1
#define OPEN_ID 2
#define CLOSE_ID 3
#define SEND_TASK_ASYNC_ID 4
#define WAIT_FREE_ASYNC_ID 5
#define OPEN_ASYNC_ID 6
#define CLOSE_ASYNC_ID 7

int pi_cluster_send_task_to_cl(struct pi_device *device, struct pi_cluster_task *task);
int pi_cluster_send_task_to_cl_async(struct pi_device *device, struct pi_cluster_task *cluster_task, pi_task_t *task);
int pi_cluster_send_tasklet_to_cl_async(struct pi_device *device, struct pi_cluster_task *cluster_task, pi_task_t *task);

static inline int pi_cluster_send_task(struct pi_device *device, struct pi_cluster_task *task)
{
    return pi_cluster_send_task_to_cl(device, task);
}

static inline int pi_cluster_send_task_async(struct pi_device *device, struct pi_cluster_task *cluster_task, pi_task_t *task)
{
    return pi_cluster_send_task_to_cl_async(device, cluster_task, task);
}

/** \brief check if any cluster is on
*/
uint8_t pi_cluster_is_on(void);

// --- Useful defines to manipulate cluster objects
//
#define  GAP_CLUSTER_TINY_DATA(id, addr) (CLUSTER_BASE + 0x400000*(id) + (addr & 0xFFF))
#define  GAP_CLUSTER_CORE0_MASK 0x00000001
#define  GAP_CLUSTER_WITHOUT_CORE0_MASK           0xFFFFFFFE

/** \brief poweroff the cluster (activate clock gating) - async version
 * Stops the cluster and activate clock gating
 * will wait until cluster has finished executing all its tasks
 * If multiple threads are using the cluster, will only decrement a semaphore
 * \param cluster_id ID of the cluster to poweroff
 * \param async_task asynchronous task to be executed at the end of operation
 */
int pi_cluster_close_async(struct pi_device *device, pi_task_t *async_task);

int pi_cluster_ioctl(struct pi_device *device, uint32_t func_id, void *arg);

int pi_cluster_ioctl_async(struct pi_device *device, uint32_t func_id,
        void *arg, pi_task_t *async_task);

/** \brief Wait for the cluster to be free i.e. nothing executes on it
 * Wait until no task is executed on cluster
 * \param cluster_id ID of the cluster to wait on
 */
void pi_cluster_wait_free(struct pi_device *device);

/** \brief Wait for the cluster to be free i.e. nothing executes on it
 * Wait until no task is executed on cluster
 * \param cluster_id ID of the cluster to wait on
 * \param async_task asynchronous task to be executed at the end of operation
 */
void pi_cluster_wait_free_async(struct pi_device *device, pi_task_t *async_task);

/** \brief power on the cluster (deactivate clock gating) - async version
 * Calling this function will power on the cluster
 * At the end of the call, cluster is ready to execute a task
 * Function is thread safe and reentrant
 * \param cluster_id ID of the cluster to power on
 * \param async_task asynchronous task to be executed at the end of operation
 */
int pi_cluster_open_async(struct pi_device *device,
        pi_task_t *async_task);


#if defined(PMSIS_DRIVERS) || !defined(__PULPOS2__)

static inline struct pi_cluster_task *pi_cluster_task(struct pi_cluster_task *task, void (*entry)(void*), void *arg)
{
    #if defined(PMSIS_DRIVERS)
    memset(task, 0, sizeof(struct pi_cluster_task));
    #endif  /* PMSIS_DRIVERS */
    task->entry = entry;
    task->arg = arg;
    task->stacks = (void *)0;
    task->stack_size = 0;
    task->nb_cores = 0;
    return task;
}

#endif

/// @endcond

#endif
