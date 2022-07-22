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

static inline uint32_t pi_is_fc()
{
    return hal_is_fc();
}


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



#include "thread.h"
#include "task.h"
#if !defined(__GAP9__)
#include "alloc.h"
#include "alloc_pool.h"
#endif
#include "irq.h"
#include "link.h"
#include "soc_event.h"
#if defined(UDMA_VERSION) && UDMA_VERSION == 2
#include "pos/implem/udma-v2.h"
#endif
#if defined(UDMA_VERSION) && UDMA_VERSION == 3
#include "pos/implem/udma-v3.h"
#endif
#if defined(UDMA_VERSION) && UDMA_VERSION == 4
#include "pos/implem/udma-v4.h"
#endif
#include "trace.h"
#include "soc.h"
#include "perf.h"

#if defined(__GAP9__)
#include "chips/gap9/drivers/cluster/implem.h"
#endif

#include "kernel.h"
#include "dma.h"
#include "lock.h"

#ifdef ARCHI_UDMA_HAS_HYPER
#if defined(__GAP9__)
#include "pos/implem/hyperbus-v2.h"
#include "pos/implem/octospi-v2.h"
#include "chips/gap9/drivers/i2s/i2s.h"
#endif
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



static inline void pi_yield()
{
    int irq = hal_irq_disable();
    pos_task_handle();
    hal_irq_restore(irq);

}

#define rt_platform() __PLATFORM__

/* Define this flag to enable assert, args check. */

#if defined(DEBUG_ASSERT)
#define IMPLEM_SPECIFIC_ASSERT(test)                                    \
    if (!(test))                                                        \
    {                                                                   \
        printf("PI assertion error in func %s, %s:%d : %s.\n",          \
               __func__, __FILE__, (unsigned int)__LINE__, #test);      \
        pmsis_exit(-187);                                               \
    }
#else
#define IMPLEM_SPECIFIC_ASSERT(test) ((void) 0)
#endif  /* DEBUG_ASSERT */

#endif
