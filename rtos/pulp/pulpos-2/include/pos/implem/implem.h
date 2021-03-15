/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_IMPLEM_IMPLEM_H__
#define __POS_IMPLEM_IMPLEM_H__

#include <stdlib.h>

#define POS_CL_WAIT_TASK_EVT 1

#define   likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)

#define PI_PLATFORM_OTHER 0
#define PI_PLATFORM_FPGA  1
#define PI_PLATFORM_RTL   2
#define PI_PLATFORM_GVSOC 3
#define PI_PLATFORM_BOARD 4

static inline int pi_platform()
{
    return __PLATFORM__;
}

static inline uint32_t pi_core_id()
{
    return hal_core_id();
}

static inline uint32_t pi_cluster_id()
{
    return hal_cluster_id();
}

static inline int pi_cl_cluster_nb_cores()
{
#ifdef ARCHI_HAS_CC
#if !defined(ARCHI_CORE_HAS_PULPV2) || defined(PLP_NO_BUILTIN)
    return ((pulp_read32(ARCHI_APB_SOC_CTRL_ADDR)>>16) + 1);
#else
    return __builtin_pulp_CoreCount() + 1;
#endif
#else
#if !defined(ARCHI_CORE_HAS_PULPV2) || defined(PLP_NO_BUILTIN)
    return (pulp_read32(ARCHI_APB_SOC_CTRL_ADDR)>>16);
#else
    return __builtin_pulp_CoreCount();
#endif
#endif
}

static inline uint32_t pi_cl_cluster_nb_pe_cores()
{
#if !defined(ARCHI_CORE_HAS_PULPV2) || defined(PLP_NO_BUILTIN)
    return (pulp_read32(ARCHI_APB_SOC_CTRL_ADDR)>>16);
#else
    return __builtin_pulp_CoreCount();
#endif
}

static inline uint32_t pi_is_fc()
{
    return hal_is_fc();
}


#if defined(ARCHI_HAS_CLUSTER)

static inline int pos_nb_cluster()
{
#ifndef ARCHI_NB_CLUSTER
    return 1;
#else
    return ARCHI_NB_CLUSTER;
#endif
}

#endif

extern int pos_kernel_pmsis_exit_value;


static inline int pmsis_kickoff(void *arg)
{
    ((void (*)())arg)();
    return pos_kernel_pmsis_exit_value;
}

static inline void pmsis_exit(int err)
{
    pos_kernel_pmsis_exit_value = err;
    exit(err);
}



#include "task.h"
#if defined(UDMA_VERSION) && UDMA_VERSION == 2
#include "pos/implem/udma-v2.h"
#endif
#if defined(UDMA_VERSION) && UDMA_VERSION == 3
#include "pos/implem/udma-v3.h"
#endif
#if defined(UDMA_VERSION) && UDMA_VERSION == 4
#include "pos/implem/udma-v4.h"
#endif
#include "alloc.h"
#include "irq.h"
#include "link.h"
#include "soc_event.h"
#include "alloc.h"
#include "alloc_pool.h"
#include "trace.h"
#include "soc.h"
#include "freq.h"
#include "perf.h"
#include "cluster.h"
#include "pe.h"
#include "kernel.h"
#include "dma.h"
#include "lock.h"

#ifdef ARCHI_UDMA_HAS_HYPER
#include "pos/implem/hyperbus-v2.h"
#include "pos/implem/octospi-v2.h"
#endif

#ifdef CONFIG_PWM
#include "pos/implem/pwm-v1.h"
#endif

#ifdef CONFIG_CPI
#include "pos/implem/cpi-v1.h"
#endif

#if defined(UDMA_UART_VERSION) && UDMA_UART_VERSION == 2
#include "pos/implem/uart-v2.h"
#endif


static inline void *pos_cluster_tiny_addr(int cid, void *data)
{
  // TODO due to a compiler bug, we have to cast the tiny data to avoid the propagation of the tiny attribute
  return (void *)(ARCHI_CLUSTER_GLOBAL_ADDR(cid) + ((int)data & 0xFFF));
}


static inline void cl_wait_task(unsigned char *done)
{
    while ((*(volatile char *)done) == 0)
    {
        eu_evt_maskWaitAndClr(1<<POS_CL_WAIT_TASK_EVT);
    }
}

static inline void pi_yield()
{
    int irq = hal_irq_disable();
    pos_task_handle();
    hal_irq_restore(irq);

}

extern unsigned char __l1_heap_start;
extern unsigned char __l1_heap_size;

#define rt_platform() __PLATFORM__

#endif