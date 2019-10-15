/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __RT_RT_CLUSTER_H__
#define __RT_RT_CLUSTER_H__


/**        
 * @ingroup groupKernel        
 */


/**        
 * @defgroup Cluster Cluster management
 *
 * Clusters must be mounted before they can be used.
 *
 * Once mounted, remote function executions can be enqueued to offload computation to the cluster.
 * A remote execution is a multi-core execution. This starts execution only on the master core, however
 * the other cores of the cluster are available for parallel computation through synchronization primitives.
 * Events can be used to receive a notification on the fabric controller when an execution is finished.
 */

/**        
 * @addtogroup Cluster
 * @{        
 */

/**@{*/

/** \brief Cluster power-up and power-down.
 *
 * By default the cluster is powered down and cannot be used. Before using the cluster this function must be used
 * to power it up.
 *
 * \param mount   Must be set to 1 for powering-up the cluster, or 0 for powering-down.
 * \param cid     The identifier of the cluster to configure. This must be an integer from 0 to the number of clusters minus 1.
 * \param flags   Flags to configure the cluster. Not currently used.
 * \param event   An event to specify how to be notified when the new cluster configuration has been applied. If NULL, the function will only return when this is done. Otherwise the event can be used to either call a callback or to wait on the event afterwards.
 */
void rt_cluster_mount(int mount, int cid, int flags, rt_event_t *event);



/** \brief Call a function that will execute on the cluster.
 *
 * This will wake-up the master core (core 0) of the specified cluster and make it execute the specified function.
 * This function is intended to be used for coarse-grain job delegation to the cluster side, and thus the stack
 * used to execute this function must also be specified.
 * When the function starts executing on the cluster, the other cores of the cluster are also available
 * for parallel computation. Thus the stacks for the other cores (called slave cores) can also be specified,
 * as well as the number of cores which can be used by the function on the cluster (including the master).
 *
 * Note that this enqueues a function execution. To allow cluster executions to be pipelined 2 can be enqueued at the same time.
 * If 2 functions are queued, as soon as the first is finished, the master core immediately continues with the next one,
 * while the fabric controller receives the termination notification and can enqueue a new execution, in order
 * to keep the cluster busy.
 *
 * \param call    A pointer to the cluster call structure, needed by the runtime to manage the call. This is only needed if more than 2 calls are enqueued at the same time, otherwise it can be NULL.
 * \param cid     The identifier of the cluster on which to call the function.
 * \param entry   The function to be called.
 * \param arg     The argument of the function to be called.
 * \param stacks  The stacks of the cores. This must contain the master stack and one stack for each slave core. If NULL the stack will be allocated and freed automatically. The free occurs the next time that rt_cluster_call is invoked.
 * \param master_stack_size  Stack size of the master core which will execute the function. If stacks is NULL can be 0 to indicate to use the default master stack size. See rt_cl_master_stack_size_get().
 * \param slave_stack_size   Stack size of each core associated to the execution of the function. If stacks is NULL can be 0 to indicate to use the default slave core stack size. See rt_cl_slave_stack_size_get().
 * \param nb_pe   Number of cores involved in the execution of the function. Can be 0 to select the maximum number of cores.
 * \param event   An event to specify how to be notified when the cluster function has returned. If NULL, the function will only return when this is done. Otherwise the event can be used to either call a callback or to wait on the event afterwards.
 */
int rt_cluster_call(rt_cluster_call_t *call, int cid, void (*entry)(void *arg), void *arg, void *stacks, int master_stack_size, int slave_stack_size, int nb_pe, rt_event_t *event);


/** \brief Can be used to trigger a notification to all cluster cores */
#define RT_TRIGGER_ALL_CORE 0


/** \brief Initialize a notification structure.
 *
 * Notifications can be used to make a cluster core go to sleep until a notification is sent to him. This is useful for example for polling a variable without consuming power for nothing.  
 * Can only be called from fabric controller.
 *
 * \param notif    A pointer to the notification structure, needed by the runtime to manage the notification. This must be allocated by the caller and must be kept alive until the notification is closed.
 * \param cid      The identifier of the cluster on which the the notification will be triggered.
 */
void rt_cluster_notif_init(rt_notif_t *notif, int cid);



/** \brief Send a notification.
 *
 * Notifications can be used to make a cluster core go to sleep until a notification is sent to him. This is useful for example for polling a variable without consuming power for nothing.  
 * Can only be called from fabric controller.
 *
 * \param notif    A pointer to the notification structure, this will tell the runtime which event must be used and to which cluster it must be sent.
 * \param core_mask The mask of cores which will receive the notification. There is 1 bit per core, bit 0 (least signigicant bit) is for core 0. A bit set to 1 means the core will receive the notification. The special value RT_TRIGGER_ALL_CORE can also be used to send to all cores.
 */
static inline void rt_cluster_notif_trigger(rt_notif_t *notif, unsigned core_mask);



/** \brief Get the event associated to the notification.
 *
 * The event is the information used by cluster cores to go to sleep. As the notification structure is usually stored close to the fabric controller, calling this function let cluster cores store the event information closer, e.g. on the stack.
 * Can only be called from cluster.
 *
 * \param notif    A pointer to the notification structure.
 * \return   The event. This can then be used when calling rt_cluster_notif_wait.
 */
static inline int rt_cluster_notif_event(rt_notif_t *notif);



/** \brief Put the calling core to sleep until a notification is received.
 *
 * Notifications are using hardware events which can be multiplexed between several notifications. 
 * So a core should not only rely on the notification to get an information, he should 
 * always check a condition variable and use the notification just as a way to sleep until
 * something happens.
 * Can only be called from cluster.
 *
 * \param event The event returned by rt_cluster_notif_event.
 */
static inline void rt_cluster_notif_wait(int event);



/** \brief Close a notification structure.
 *
 * This will free all allocated resources.
 * Can only be called from fabric controller.
 *
 * \param notif    A pointer to the notification structure.
 */
void rt_cluster_notif_deinit(rt_notif_t *notif);



//!@}

/**        
 * @} end of Cluster group        
 */




/// @cond IMPLEM

extern RT_L1_TINY_DATA rt_cluster_call_pool_t __rt_cluster_pool;
extern RT_L1_TINY_DATA int __rt_cluster_nb_active_pe;


static inline int rt_cluster_nb_pe()
{
  return __rt_cluster_nb_active_pe;
}

int rt_cluster_fetch_all(int cid);

#if defined(ARCHI_HAS_CLUSTER)

#if defined(EU_VERSION) && EU_VERSION == 1
void __rt_cluster_pe_init(void *stacks, int stacks_size);
#endif


extern void __rt_set_slave_stack();

#if defined(PULP_CHIP_FAMILY) && (PULP_CHIP_FAMILY == CHIP_VEGA || PULP_CHIP_FAMILY == CHIP_DEVCHIP || PULP_CHIP_FAMILY == CHIP_WOLFE || PULP_CHIP_FAMILY == CHIP_GAP) || PULP_CHIP_FAMILY == CHIP_GAP9

int __rt_pmu_cluster_power_up(rt_event_t *event, int *pending);

void __rt_pmu_cluster_power_down(rt_event_t *event, int *pending);
  
#else

static inline int __rt_pmu_cluster_power_up(rt_event_t *event, int *pending)
{
  return 0;
}

static inline void __rt_pmu_cluster_power_down(rt_event_t *event, int *pending)
{
}
  
#endif

extern RT_L1_GLOBAL_DATA void (*__rt_cluster_entry)(void *);
extern RT_L1_GLOBAL_DATA void *__rt_cluster_entry_arg;

static inline rt_event_sched_t *__rt_cluster_sched_get()
{
  return rt_event_internal_sched();
}


// This function will push an event from cluster to FC and will be enqueued
// into the scheduler like a normal event
void __rt_cluster_push_fc_event(rt_event_t *event);

// This function will push an event from cluster to FC and the event callback
// will be executed directly from within the interrupt handler
static inline void __rt_cluster_push_fc_irq_event(rt_event_t *event)
{
  __rt_cluster_push_fc_event((rt_event_t *)(((unsigned int)event) | 0x1));
}

static inline void __rt_cluster_notif_req_done(int cid)
{
  eu_evt_trig(eu_evt_trig_cluster_addr(cid, RT_CLUSTER_CALL_EVT), 0);
}

static inline int rt_cluster_notif_event(rt_notif_t *notif)
{
  return notif->event_id;
}

static inline void rt_cluster_notif_trigger(rt_notif_t *notif, unsigned core_mask)
{
  eu_evt_trig(notif->trig_addr, core_mask);
}

static inline void rt_cluster_notif_wait(int event)
{
  eu_evt_maskWaitAndClr(event);
}

extern rt_fc_cluster_data_t __rt_fc_cluster_data[];

#endif



/// @endcond

#endif
