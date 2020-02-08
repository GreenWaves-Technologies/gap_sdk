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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "stdio.h"
#include "hal/gvsoc/gvsoc.h"
#include "pmsis.h"
#include "archi/pulp.h"

#if defined(ARCHI_HAS_CLUSTER)

#ifndef ARCHI_NB_CLUSTER
#define ARCHI_NB_CLUSTER 1
#endif

typedef enum 
{
  RT_CLUSTER_MOUNT_START,
  RT_CLUSTER_MOUNT_POWERED_ON,
  RT_CLUSTER_MOUNT_DONE,
} __rt_cluster_mount_step_e;

rt_fc_cluster_data_t __rt_fc_cluster_data[ARCHI_NB_CLUSTER];
pi_task_t *__rt_cluster_tasks[ARCHI_NB_CLUSTER];

#ifdef __RT_USE_PROFILE
RT_L1_TINY_DATA int __rt_pe_trace[ARCHI_CLUSTER_NB_PE];
#endif

/*
 * Cluster tiny data
 * They are in tiny area for fast access from cluster side. As they local
 * they will be instantiated once for each cluster and are thus
 * naturally supporting multi cluster.
 */

RT_L1_TINY_DATA rt_cluster_call_pool_t __rt_cluster_pool;
RT_L1_TINY_DATA int __rt_cluster_nb_active_pe;
RT_L1_TINY_DATA pi_cl_dma_cmd_t *__rt_dma_first_pending;
RT_L1_TINY_DATA pi_cl_dma_cmd_t *__rt_dma_last_pending;

RT_L1_DATA uint32_t __rt_cluster_fc_task_lock;


void __rt_enqueue_event();
void __rt_remote_enqueue_event();
void __rt_bridge_enqueue_event();

extern char _l1_preload_start_inL2[];
extern char _l1_preload_start[];
extern char _l1_preload_size[];

extern void _start();

static void __rt_init_cluster_data(int cid)
{

#if defined(ARCHI_HAS_FC) && ARCHI_HAS_FC == 1
  // In case the chip does not support L1 preloading, the initial L1 data are in L2, we need to copy them to L1
  int *l1_preload_start = (int *)rt_cluster_tiny_addr(cid, (void *)&_l1_preload_start);
  int *l1_preload_start_inL2 = (int *)&_l1_preload_start_inL2;
  int l1_preload_size = (int)&_l1_preload_size;

  rt_trace(RT_TRACE_INIT, "L1 preloading data copy from L2 to L1 (L2 start: 0x%x, L2 end: 0x%x, L1 start: 0x%x)\n", (int)l1_preload_start_inL2, (int)l1_preload_start_inL2 + l1_preload_size, (int)l1_preload_start);
  for (; l1_preload_size > 0; l1_preload_size-=4, l1_preload_start++, l1_preload_start_inL2++) {
    *l1_preload_start = *l1_preload_start_inL2;
  }

#endif

  int nb_cluster = rt_nb_cluster();

  __rt_fc_cluster_data[cid].stacks = NULL;
  __rt_fc_cluster_data[cid].trig_addr = eu_evt_trig_cluster_addr(cid, RT_CLUSTER_CALL_EVT);
  __rt_fc_cluster_data[cid].pool = (rt_cluster_call_pool_t *)rt_cluster_tiny_addr(cid, &__rt_cluster_pool);


#ifdef __RT_USE_PROFILE
  for (int i=0; i<ARCHI_CLUSTER_NB_PE; i++)
  {
    char str[64];
    sprintf(str, "/user/runtime/cluster_%d/pe%d", cid, i);
    *(int *)rt_cluster_tiny_addr(cid, (void *)&__rt_pe_trace[i]) = gv_vcd_open_trace(str);
  };
#endif
}



static int __rt_cluster_power_up(rt_fc_cluster_data_t *cluster)
{
  int cid = cluster->cid;

  cluster->powered_up = 0;

  // Power-up the cluster
  // For now the PMU is only supporting one cluster
  if (cid == 0)
  {
    int pending = 0;
    cluster->powered_up = __rt_pmu_cluster_power_up(cluster->mount_event, &pending);
    return pending;
  }

  return 0;
}



static int __rt_cluster_setup(rt_fc_cluster_data_t *cluster)
{
  int cid = cluster->cid;
  rt_event_t *event = cluster->mount_event;

  rt_cluster_call_pool_t *pool = (rt_cluster_call_pool_t *)rt_cluster_tiny_addr(cid, &__rt_cluster_pool);

  pool->first_call_fc = NULL;
  pool->last_call_fc = NULL;
  pool->first_call_fc_for_cl = NULL;

  __rt_cluster_fc_task_lock = 0;



#ifdef FLL_VERSION
  #if PULP_CHIP_FAMILY == CHIP_VIVOSOC3 || PULP_CHIP_FAMILY == CHIP_VIVOSOC3_1|| PULP_CHIP_FAMILY == CHIP_VIVOSOC4
    if (rt_platform() != ARCHI_PLATFORM_FPGA)
    {
      // Check if we have to restore the cluster freqeuncy
      int freq = rt_freq_get(RT_FREQ_DOMAIN_CL);

      if(freq == 0)  // freq is 0 -> set clk domain to soc fll, as this is always running 
      {
        rt_freq_config_set(RT_FREQ_DOMAIN_CL, FREQ_DOMAIN_CLK_TREE_FLL_SOC);  // freq will be updated internally
      }
    }  

  #else 
    if (rt_platform() != ARCHI_PLATFORM_FPGA)
    {
      // Setup FLL
      int init_freq = __rt_fll_init(__RT_FLL_CL);

      // Check if we have to restore the cluster frequency
      // otherwise just set it to the one returned by the fll
      int freq = rt_freq_get(RT_FREQ_DOMAIN_CL);

      if (freq)
      {
        rt_freq_set(RT_FREQ_DOMAIN_CL, freq);
      }
      else
      {
        __rt_freq_set_value(RT_FREQ_DOMAIN_CL, init_freq);
      }
    }
  #endif  
#endif

#ifdef ARCHI_HAS_CLUSTER_CLK_GATE
    /* Activate cluster top level clock gating */
    IP_WRITE(ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid), ARCHI_CLUSTER_CTRL_CLUSTER_CLK_GATE, 1);
#endif

    // Initialize cluster global variables
    __rt_init_cluster_data(cid);

    // Initialize cluster L1 memory allocator
    __rt_alloc_init_l1(cid);

    // Initialize FC data for this cluster
    if (cluster->powered_up)
    {
      //__rt_fc_cluster_data[cid].call_head = 0;
    }

    // Activate icache
    hal_icache_cluster_enable(cid);



#if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2

    // Fetch all cores, they will directly jump to the PE loop waiting from orders through the dispatcher
    for (int i=0; i<rt_nb_active_pe(); i++) {
#ifdef ARCHI_CORE_HAS_1_10
      plp_ctrl_core_bootaddr_set_remote(cid, i, (int)_start);
#else
      // On old cores, the HW is adding 0x80 to the boot address
      plp_ctrl_core_bootaddr_set_remote(cid, i, ((int)_start) & 0xffffff00);
#endif
    }
#ifndef ARCHI_HAS_NO_DISPATCH
    eoc_fetch_enable_remote(cid, (1<<rt_nb_active_pe()) - 1);
#endif

#endif

    return 0;
}



static void __rt_cluster_mount_step(void *_cluster)
{
  int end = 0;
  rt_fc_cluster_data_t *cluster = (rt_fc_cluster_data_t *)_cluster;

  while(!end)
  {
    switch (cluster->state)
    {
      case RT_CLUSTER_MOUNT_START:
        end = __rt_cluster_power_up(cluster);
        break;

      case RT_CLUSTER_MOUNT_POWERED_ON:
        end = __rt_cluster_setup(cluster);
        break;

      case RT_CLUSTER_MOUNT_DONE:
        __rt_event_restore(cluster->mount_event);
        __rt_event_enqueue(cluster->mount_event);
        end = 1;
        break;
    }

    cluster->state++;
  }
}



static inline __attribute__((always_inline)) int __rt_cluster_mount(rt_fc_cluster_data_t *cluster, int cid, int flags, rt_event_t *event)
{
  rt_trace(RT_TRACE_CONF, "Mounting cluster (cluster: %d)\n", cid);

  if (rt_is_fc() || (cid && !rt_has_fc()))
  {
    cluster->state = RT_CLUSTER_MOUNT_START;
    cluster->mount_event = event;

    __rt_event_save(event);
    __rt_init_event(event, rt_event_internal_sched(), __rt_cluster_mount_step, (void *)cluster);
    __rt_event_set_pending(event);

    __rt_cluster_mount_step((void *)cluster);
  }
  else
  {
    // Initialize cluster global variables
    __rt_init_cluster_data(cid);

    // Activate icache
    hal_icache_cluster_enable(cid);

    // Only initialize it if it is not our own allocator as in this case
    // it has already been initialized during start-up

#if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2

    for (int i=1; i<rt_nb_active_pe(); i++) {
#ifdef ARCHI_CORE_HAS_1_10
      plp_ctrl_core_bootaddr_set_remote(cid, i, (int)_start);
#else
      // On old cores, the HW is adding 0x80 to the boot address
      plp_ctrl_core_bootaddr_set_remote(cid, i, ((int)_start) & 0xffffff00);
#endif
    }
    eoc_fetch_enable_remote(cid, -1);    

#endif

    rt_event_push(event);

  }

  return 0;
}

static inline __attribute__((always_inline)) void __rt_cluster_unmount(int cid, int flags, rt_event_t *event)
{
  rt_trace(RT_TRACE_CONF, "Unmounting cluster (cluster: %d)\n", cid);

#ifdef FLL_VERSION
  #if PULP_CHIP_FAMILY == CHIP_VIVOSOC3 || PULP_CHIP_FAMILY == CHIP_VIVOSOC3_1 || PULP_CHIP_FAMILY == CHIP_VIVOSOC4
    if (rt_platform() != ARCHI_PLATFORM_FPGA)
    {
      // check if cl fll was used
      if(rt_freq_config_get(RT_FREQ_DOMAIN_CL) == FREQ_DOMAIN_CLK_TREE_FLL_ALT)
      {
        // change back to soc fll
        rt_freq_config_set(RT_FREQ_DOMAIN_CL, FREQ_DOMAIN_CLK_TREE_FLL_SOC);  // freq will be updated internally
        // disable cl fll
        __rt_fll_disable(HAL_FLL_CL);
      }
    }
  #else
    if (rt_platform() != ARCHI_PLATFORM_FPGA)
    {
      __rt_fll_deinit(__RT_FLL_CL);
    }
  #endif  
#endif

  // Power-up the cluster
  // For now the PMU is only supporting one cluster
    int pending = 0;
  if (cid == 0) __rt_pmu_cluster_power_down(event, &pending);

  // For now the whole sequence is blocking so we just handle the event here.
  // The power-down sequence could be done asynchronously and would then use the event
  if (event && !pending) 
    {
      rt_event_push(event);
    }
}


void pi_cluster_conf_init(struct pi_cluster_conf *conf)
{
  conf->id = 0;
}


int pi_cluster_open(struct pi_device *cluster_dev)
{
  int irq = rt_irq_disable();

  struct pi_cluster_conf *conf = (struct pi_cluster_conf *)cluster_dev->config;
  int cid = conf->id;

  cluster_dev->data = (void *)&__rt_fc_cluster_data[cid];

  rt_event_t *event = __rt_wait_event_prepare_blocking();

  if (__rt_cluster_mount(&__rt_fc_cluster_data[cid], conf->id, 0, event))
  {
    rt_irq_restore(irq);
    return -1;
  }

  __rt_wait_event(event);

  rt_irq_restore(irq);

  return 0;
}




int pi_cluster_close(struct pi_device *cluster_dev)
{
  rt_fc_cluster_data_t *data = (rt_fc_cluster_data_t *)cluster_dev->data;

  __rt_cluster_unmount(data->cid, 0, NULL);

  return 0;
}


void __rt_dma_2d();

static RT_FC_BOOT_CODE int __rt_cluster_init(void *arg)
{
  int nb_cluster = rt_nb_cluster();
  int data_size = sizeof(rt_fc_cluster_data_t)*nb_cluster;

  memset(__rt_fc_cluster_data, 0, data_size);

  for (int i=0; i<nb_cluster; i++)
  {
    __rt_fc_cluster_data[i].cid = i;
  }

#ifndef ARCHI_NO_L1_TINY
  rt_irq_set_handler(ARCHI_CL_EVT_DMA1, __rt_dma_2d);
#endif

  rt_irq_set_handler(RT_FC_ENQUEUE_EVENT, __rt_remote_enqueue_event);
  rt_irq_mask_set(1<<RT_FC_ENQUEUE_EVENT);

  rt_irq_set_handler(RT_BRIDGE_ENQUEUE_EVENT, __rt_bridge_enqueue_event);
  rt_irq_mask_set(1<<RT_BRIDGE_ENQUEUE_EVENT);

  return 0;
}



#if defined(ARCHI_HAS_FC)

void __rt_cluster_push_fc_event(rt_event_t *event)
{

  while (rt_tas_lock_32((int)&__rt_cluster_fc_task_lock) == -1)
  {
    eu_evt_maskWaitAndClr(1<<RT_CL_SYNC_EVENT);
  }

  rt_fc_cluster_data_t *data = &__rt_fc_cluster_data[rt_cluster_id()];

  // First wait until the slot to post events is free
  while(data->events != NULL)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }

  // Push the event and notify the FC with a HW evet in case it
  // is sleeping
  data->events = event;
#ifdef ITC_VERSION
  hal_itc_status_set(1<<RT_FC_ENQUEUE_EVENT);
#else
  eu_evt_trig(eu_evt_trig_fc_addr(RT_FC_ENQUEUE_EVENT), 0);
#endif

  rt_tas_unlock_32((int)&__rt_cluster_fc_task_lock, 0);
  eu_evt_trig(eu_evt_trig_addr(RT_CL_SYNC_EVENT), 0);
}

#endif


RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_cluster_new()
{
  int err = 0;

  err |= __rt_cbsys_add(RT_CBSYS_START, __rt_cluster_init, NULL);

  if (err) rt_fatal("Unable to initialize time driver\n");
}

#endif


#if defined(ARCHI_HAS_CLUSTER)

void rt_cluster_notif_init(rt_notif_t *notif, int cluster_id)
{
  notif->trig_addr = eu_evt_trig_cluster_addr(cluster_id, RT_CLUSTER_TRIGGER_EVT);
  notif->event_id = 1 << RT_CLUSTER_TRIGGER_EVT;
}
void rt_cluster_notif_deinit(rt_notif_t *notif)
{
}

#endif


#if defined(ARCHI_HAS_CLUSTER)


extern int main();

static void cluster_pe_start(void *arg)
{
  rt_irq_enable();
  main();
}

static void __rt_cluster_start(void *arg)
{
#if defined(EU_VERSION) && EU_VERSION >= 3
  eu_evt_maskSet((1<<PULP_DISPATCH_EVENT) | (1<<PULP_HW_BAR_EVENT) | (1<<PULP_MUTEX_EVENT));
  rt_team_fork(rt_nb_pe(), cluster_pe_start, NULL);
#endif
}

int rt_cluster_fetch_all(int cid)
{
  rt_cluster_mount(1, cid, 0, NULL);
  return rt_cluster_call(NULL, cid, __rt_cluster_start, NULL, NULL, 0, 0, rt_nb_pe(), NULL);
}

#else

int rt_cluster_fetch_all(int cid)
{
  return 0;
}

#endif