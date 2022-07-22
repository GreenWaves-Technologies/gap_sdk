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


#ifndef __POS_DATA_DATA_H__
#define __POS_DATA_DATA_H__

#include <archi/pulp.h>

#ifndef LANGUAGE_ASSEMBLY

// We cannot use tiny attribute if we use a generic riscv toolchain or LLVM or we there is fc specific memeory (TCDM or L2)
#if (defined(ARCHI_HAS_FC_TCDM) || defined(ARCHI_HAS_L2_ALIAS)) && !defined(__LLVM__) && !defined(__RISCV_GENERIC__) && !defined(__PULP_TOOLCHAIN__)
#define POS_FC_TINY_ATTRIBUTE __attribute__ ((tiny))
#else
#define POS_FC_TINY_ATTRIBUTE
#endif

#define POS_HANDLER __attribute__((section(".text_l2")))
#define POS_TEXT_L2 __attribute__((section(".text_l2")))
#define PI_LOCAL_CODE POS_TEXT_L2
#define PI_TEXT_L2 POS_TEXT_L2

#define PI_FC_L1       __attribute__((section(".data_fc")))
#define PI_FC_L1_TINY  __attribute__((section(".data_tiny_fc"))) POS_FC_TINY_ATTRIBUTE

#define PI_FC_TINY     PI_FC_L1_TINY

#define PI_L2 __attribute__((section(".l2_data")))

#if defined(ARCHI_HAS_L1_ALIAS) && !defined(__PULP_TOOLCHAIN__)
#define PI_CL_L1_TINY __attribute__ ((tiny)) __attribute__((section(".data_tiny_l1")))
#else
#define PI_CL_L1_TINY __attribute__((section(".data_tiny_l1")))
#endif

#define PI_CL_L1 __attribute__((section(".data_l1")))
#define PI_L1 PI_CL_L1


struct pi_task_implem
{
    unsigned int time;
#if defined(CONFIG_MULTI_THREADING)
    void *waiting;
#endif
} __attribute__((packed));



#define CLUSTER_TASK_IMPLEM
#define PI_TASK_IMPLEM struct pi_task_implem implem

#define PI_TASK_IMPLEM_NB_DATA 8

typedef struct pi_task{
    // Warning, might be accessed inline in asm, and thus can not be moved
    struct pi_task *next;
    uintptr_t arg[4];
    uint32_t data[PI_TASK_IMPLEM_NB_DATA];

    PI_TASK_IMPLEM;

} pi_task_t;


struct pi_mem_slab {
    uint32_t num_blocks;
    size_t block_size;
    char *buffer;
    char *free_list;
    uint32_t num_used;
};

#endif



#if defined(ARCHI_HAS_L2)
#ifdef ARCHI_HAS_L2_MULTI
#define POS_NB_ALLOC_L2 2
#else
#define POS_NB_ALLOC_L2 1
#endif
#endif


#define PI_CALLBACK_T_NEXT       (0*4)
#define PI_CALLBACK_T_ENTRY      (1*4)
#define PI_CALLBACK_T_ARG        (2*4)

#define PI_TASK_T_NEXT           (0*4)
#define PI_TASK_T_ARG_0          (1*4)
#define PI_TASK_T_ARG_1          (2*4)
#define PI_TASK_T_ARG_2          (3*4)
#define PI_TASK_T_ARG_3          (4*4)
#define PI_TASK_T_DATA_0         (5*4)
#define PI_TASK_T_DATA_1         (6*4)
#define PI_TASK_T_DATA_2         (7*4)
#define PI_TASK_T_DATA_3         (8*4)
#define PI_TASK_T_DATA_4         (9*4)
#define PI_TASK_T_DATA_5         (10*4)
#define PI_TASK_T_DATA_6         (11*4)
#define PI_TASK_T_DATA_7         (12*4)






#include "kernel.h"

#if defined(UDMA_VERSION) && UDMA_VERSION == 2
#include "pos/data/udma-v2.h"
#endif
#if defined(UDMA_VERSION) && UDMA_VERSION == 3
#include "pos/data/udma-v3.h"
#endif
#if defined(UDMA_VERSION) && UDMA_VERSION == 4
#include "pos/data/udma-v4.h"
#endif
#include "sched.h"
#include "pos/data/irq.h"
#include "pos/data/spim.h"

#ifndef __GAP9__
#include "alloc.h" // given by pmsis implem for GAP9
#endif

#include "pos/data/uart.h"
#include "soc_event.h"
#include "lock.h"

#ifndef LANGUAGE_ASSEMBLY
#include "pmsis/pmsis_types.h"
#include "pmsis/cluster/cl_pmsis_types.h"
#endif

#ifdef __GAP9__
#include "chips/gap9/drivers/cluster/data.h"
#endif

#ifndef __GAP9__
#include "pos/data/hyperbus.h"
#endif
#ifdef CONFIG_CPI
#include "pos/data/cpi-v1.h"
#endif



#endif
