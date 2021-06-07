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

#define POS_BITFIELD_ALLOC_ID 0
#define POS_SEM_ALLOC_ID      1
#define POS_BARRIER_ALLOC_ID  2
#define POS_MUTEX_ALLOC_ID    3

#define POS_BITFIELD_ALLOC_INIT (((1 << ARCHI_EU_NB_HW_BITFIELD) - 1) & ~0xf)
#define POS_SEM_ALLOC_INIT      ((1 << ARCHI_EU_NB_HW_SEMAPHORE) - 1)
#define POS_BARRIER_ALLOC_INIT  ((1 << ARCHI_EU_NB_HW_BARRIER) - 1)
#define POS_MUTEX_ALLOC_INIT    ((1 << ARCHI_EU_NB_HW_MUTEX) - 1)

// This event is used by FC to notify few events to the cluster:
//  - A remote cluster to FC event was handled and the cluster can then try to
//    post again another event in case it was working
//  - There was a remote request handled so the cluster can check iif the one is
//    waiting for has finished
#define POS_EVENT_CLUSTER_CALL_EVT 1
#define POS_EVENT_CLUSTER_SYNC     2
#define POS_EVENT_CLUSTER_TASK     3

#define POS_EVENT_FC_ENQUEUE 1

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
} __attribute__((packed));


#define CLUSTER_TASK_CUSTOM 1

struct pi_cluster_task {
    // entry function and its argument(s)
    void (*entry)(void*);
    void *arg;
    // pointer to first stack, and size for each cores
    void *stacks;
    uint32_t stack_size;
    uint32_t slave_stack_size;
    // Number of cores to be activated
    int nb_cores;
    // callback called at task completion
    struct pi_task *completion_callback;
    int stack_allocated;
    // to implement a fifo
    struct pi_cluster_task *next;

    int core_mask;
};


#define CLUSTER_TASK_IMPLEM struct pos_cluster_task_implem implem
#define PI_TASK_IMPLEM struct pi_task_implem implem

#define PI_TASK_IMPLEM_NB_DATA 8

typedef struct pi_task{
    // Warning, might be accessed inline in asm, and thus can not be moved
    struct pi_task *next;
    uintptr_t arg[2];
    uint32_t data[PI_TASK_IMPLEM_NB_DATA];

    PI_TASK_IMPLEM;

} pi_task_t;

typedef struct pi_cluster_pe_task_s
{
    void (*entry)(struct pi_cluster_pe_task_s *task, int id);
    struct pi_cluster_pe_task_s *next;
    void *stacks;
    void (*callback_entry)(void *arg);
    void *callback_arg;
    struct pi_cluster_pe_task_s *piped_task;
    uint32_t args[4];
    uint8_t nb_cores;
    uint8_t nb_cores_popped;
    uint8_t nb_done_cores;
} pi_cluster_pe_task_t;

#endif



#if defined(ARCHI_HAS_L2)
#ifdef ARCHI_HAS_L2_MULTI
#define POS_NB_ALLOC_L2 2
#else
#define POS_NB_ALLOC_L2 1
#endif
#endif


#define PI_CLUSTER_PE_TASK_T_ENTRY           (0*4)
#define PI_CLUSTER_PE_TASK_T_NEXT            (1*4)
#define PI_CLUSTER_PE_TASK_T_STACKS          (2*4)
#define PI_CLUSTER_PE_TASK_T_CALLBACK_ENTRY  (3*4)
#define PI_CLUSTER_PE_TASK_T_CALLBACK_ARG    (4*4)
#define PI_CLUSTER_PE_TASK_T_PIPED_TASK      (5*4)
#define PI_CLUSTER_PE_TASK_T_ARGS            (6*4)
#define PI_CLUSTER_PE_TASK_T_NB_CORES        (10*4)
#define PI_CLUSTER_PE_TASK_T_NB_CORES_POPPED (10*4 + 1)
#define PI_CLUSTER_PE_TASK_T_NB_DONE_CORES   (10*4 + 2)

#define PI_CALLBACK_T_NEXT       (0*4)
#define PI_CALLBACK_T_ENTRY      (1*4)
#define PI_CALLBACK_T_ARG        (2*4)

#define PI_TASK_T_NEXT           (0*4)
#define PI_TASK_T_ARG_0          (1*4)
#define PI_TASK_T_ARG_1          (2*4)
#define PI_TASK_T_DATA_0         (3*4)
#define PI_TASK_T_DATA_1         (4*4)
#define PI_TASK_T_DATA_2         (5*4)
#define PI_TASK_T_DATA_3         (6*4)
#define PI_TASK_T_DATA_4         (7*4)
#define PI_TASK_T_DATA_5         (8*4)
#define PI_TASK_T_DATA_6         (9*4)
#define PI_TASK_T_DATA_7         (10*4)


#define PI_CLUSTER_TASK_ENTRY                 (0*4)
#define PI_CLUSTER_TASK_ARG                   (1*4)
#define PI_CLUSTER_TASK_STACKS                (2*4)
#define PI_CLUSTER_TASK_STACK_SIZE            (3*4)
#define PI_CLUSTER_TASK_SLAVE_STACK_SIZE      (4*4)
#define PI_CLUSTER_TASK_NB_CORES              (5*4)
#define PI_CLUSTER_TASK_COMPLETION_CALLBACK   (6*4)
#define PI_CLUSTER_TASK_STACK_ALLOCATED       (7*4)
#define PI_CLUSTER_TASK_NEXT                  (8*4)
#define PI_CLUSTER_TASK_CORE_MASK             (9*4)





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
#include "alloc.h"
#include "pos/data/uart.h"
#include "soc_event.h"
#include "cluster.h"
#include "lock.h"

#ifndef __GAP9__
#include "pos/data/hyperbus.h"
#endif
#ifdef CONFIG_CPI
#include "pos/data/cpi-v1.h"
#endif



#endif
