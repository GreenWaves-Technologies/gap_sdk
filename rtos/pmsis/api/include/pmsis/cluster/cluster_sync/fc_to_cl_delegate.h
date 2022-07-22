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

#ifdef __cplusplus
extern "C" {
#endif

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

/** \brief Specify cluster task stack information.
 *
 * This can be called to configure the size of the stacks and to specify stacks
 * allocated by the caller.
 *
 * \param task The task for which the stack information is being speficied.
 * \param stacks Pointer to the memory which should be used for the stacks.
 * \param arg The argument to the entry point.
 */
static inline pi_cluster_task_t *pi_cluster_task_stacks(pi_cluster_task_t *task, void *stacks,
    int stack_size);

/** \brief Specify the cluster task priority
 *
 * This sets the priority of the specified cluster task.
 * Only priorities 0 and 1 are currently supported.
 * The cluster driver provides on cluster side a cooperative priority-based scheduler.
 * A currently running task is in charge of periodically checking if it must release the 
 * cluster to let a higher priority task execute.
 * \param task The cluster task
 * \param priority The cluster task priority, can be 0 or 1.
 */
static inline void pi_cluster_task_priority(pi_cluster_task_t *task, uint8_t priority);

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
 * This function only supports task with priority 0. pi_cluster_enqueue should be used
 * instead to use priority 1.
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
 * This function only supports task with priority 0. pi_cluster_enqueue_task_async should be used
 * instead to use priority 1.
 *
 * Note that this enqueues a function execution. To allow cluster executions
 * to be pipelined, several tasks can be enqueued at the same time.
 * If more than two tasks are enqueued, as soon as the first is finished,
 * the cluster-controller core immediately continues with the next one,
 * while the fabric controller receives the termination notification and can
 * enqueue a new execution, in order to keep the cluster busy.
 *
 * \param device         A pointer to the structure describing the device.
 * \param cluster_task   Cluster task structure containing task and its parameters.
 * \param task           The task used to notify the end of execution.
 */
static inline int pi_cluster_send_task_async(struct pi_device *device,
        struct pi_cluster_task *cluster_task,
        pi_task_t *task);

/** \brief Enqueue a task for execution on the cluster.
 *
 * This function is similar to pi_cluster_send_task but supports priority 0 and 1 and do not 
 * support automatic stack allocation. Stacks but always be allocated by the caller.
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
int pi_cluster_enqueue_task(struct pi_device *device, struct pi_cluster_task *task);

/** \brief Enqueue asynchronously a task for execution on the cluster.
 *
 * This function is similar to pi_cluster_send_task_async but supports priority 0 and 1 and do not 
 * support automatic stack allocation. Stacks but always be allocated by the caller.
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
 * \param device         A pointer to the structure describing the device.
 * \param cluster_task   Cluster task structure containing task and its parameters.
 * \param task           The task used to notify the end of execution.
 */
int pi_cluster_enqueue_task_async(
    struct pi_device *device,
    struct pi_cluster_task *task,
    pi_task_t *async_task);

/** \brief Check if the current task should release the cluster.
 *
 * Since there is no preemption on cluster side, a low priority running task must periodically
 * check if the cluster should be released by calling this function.
 * If so the task should return, and the entry point will be called again later on to resume
 * execution.
 * Note that if this funtion tells that the task should return, this will force the entry point
 * to be called again later on.
 *
 * \return 1 if the current task should release the cluster or 0 if it can keep executing.
 */
static inline int pi_cl_task_yield();

/** \brief Allocate L1 memory from the task scratch area.
 *
 * The scratch area is an area within the L1 memory reserved during cluster configuration for
 * data which does not need to be kept when a cluster task ends or is suspended to let another
 * higher priority task execute.
 * Each task is having its own linear scratch allocator so that such memory area can be reused
 * from one task to another.
 * Calling this function will allocate scratch data from the specified task in a linear way,
 * which means the allocated data must be freed in reverse order.
 * Allocated data do not need to be freed when the task is over since it is just a pointer
 * being increased or decreased.
 * This allocator is reset every time the task is reset, for example by calling pi_cluster_task.
 * The amount of scratch data allocated for the specified task can not exceed the size of the
 * scratch area specified when the cluster was opened (0 by default).
 *
 * \param device The cluster device where the data is being allocated.
 * \param task The cluster task on which the scratch data must be allocated.
 * \param size Size of the allocated data.
 *
 * \return pointer The allocated area if the allocation succeeded.
 * \return NULL In case the allocation failed.
 */
void *pi_cl_l1_scratch_alloc(pi_device_t *device, pi_cluster_task_t *task, int size);

/** \brief Free L1 memory from the task scratch area.
 *
 * Calling this function will free the specified amount of data in the scrath allocator.
 * No data needs to be provided since the allocator works linearly.
 *
 * \param device The cluster device where the data is being freed.
 * \param task The cluster task on which the scratch data must be freed.
 * \param size Size of the freed data.
 */
void pi_cl_l1_scratch_free(pi_device_t *device, pi_cluster_task_t *task, int size);


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
void pi_cluster_send_tasklet(struct pi_device *device, struct pi_cluster_task *cluster_task);

static inline int pi_cluster_send_task(struct pi_device *device, struct pi_cluster_task *task)
{
    return pi_cluster_send_task_to_cl(device, task);
}

static inline int pi_cluster_send_task_async(struct pi_device *device, struct pi_cluster_task *cluster_task, pi_task_t *task)
{
    return pi_cluster_send_task_to_cl_async(device, cluster_task, task);
}

static inline int pi_cluster_send_workgroup(pi_device_t *device, pi_cluster_task_t *cluster_task);

#ifndef __GAP8__
/** \brief check if any cluster is on
*/
static inline uint8_t pi_cluster_is_on(void);
#else
uint8_t pi_cluster_is_on(void);
#endif

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


static inline pi_cluster_task_t *pi_cluster_task_stacks(pi_cluster_task_t *task, void *stacks, int stack_size)
{
    task->stacks = stacks;
    task->slave_stack_size = stack_size;
    return task;
}

#if !defined(__GAP9__)

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
    task->slave_stack_size = 0;
    task->nb_cores = 0;
    return task;
}

#endif

#endif

/// @endcond

#ifdef __cplusplus
}
#endif
#endif
