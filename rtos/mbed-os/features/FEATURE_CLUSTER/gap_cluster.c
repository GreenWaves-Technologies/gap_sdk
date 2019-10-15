/*
 * Copyright (c) 2017, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include "gap_cluster.h"
#include "gap_l1_malloc.h"
#include "gap_dmamchan.h"
#include "gap_cluster_fc_delegate.h"

#ifdef FEATURE_CLUSTER

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Cluster each core stack size */
#define CORE_STACK_SIZE  1024

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Cluster is power on */
uint32_t cluster_is_on = 0;
/*! @brief Cluster stack is initilized */
uint32_t cluster_is_init = 0;

#ifdef _OPENMP
/*! @brief Cluster ret value in OPENMP */
uint32_t cluster_ret_value = 0;
#endif

/*! @brief Cluster Stack Variables */
uint32_t cluster_stacks_start;
uint32_t cluster_core_stack_size;
uint32_t cluster_core_mask;

extern char  __l1_preload_start;
extern char  __l1_preload_start_inL2;
extern char  __l1_preload_size;

extern char  __heapsram_start;
extern char  __heapsram_size;

GAP_L1_TINY_DATA   cluster_task_t master_task;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief All cores set stack.
 *
 * @param  stacksPtr     Overall stack pointer for all cores.
 * @param  coreStackSize Each core's stack size.
 * @note .
 */
static void CLUSTER_SetCoreStack() {
    uint32_t id = __core_ID();
    if(id) {
        uint32_t corestackPtr =  (id + 1) * cluster_core_stack_size + cluster_stacks_start;

        /* Update sp */
        asm ("add  sp, x0, %0" :: "r" (corestackPtr) );
    } else {
        cluster_is_init = 1;

        /* Cluster calls FC */
        #if defined(__GAP8__)
        EU_FC_EVT_TrigSet(CLUSTER_NOTIFY_FC_EVENT, 0);
        #elif defined(__GAP9__)
        ITC_SetIRQ(CLUSTER_NOTIFY_FC_EVENT);
        #endif
    }
}

/*!
 * @brief All cores unset stack.
 *
 * @param  stacksPtr     Overall stack pointer for all cores.
 * @param  coreStackSize Each core's stack size.
 * @note .
 */
static void CLUSTER_UnSetCoreStack() {

    if(!__core_ID()) {
        /* master core */
        cluster_core_stack_size = 1;
        /* Cluster calls FC */

        #if defined(__GAP8__)
        EU_FC_EVT_TrigSet(CLUSTER_NOTIFY_FC_EVENT, 0);
        #elif defined(__GAP9__)
        ITC_SetIRQ(CLUSTER_NOTIFY_FC_EVENT);
        #endif
    }
}

static inline void CLUSTER_FC2CL_StackInit(int cid, int nbCores, uint32_t stacksPtr, int coreStackSize)
{
    uint32_t coreMask = (1 << nbCores) - 1;

    cluster_core_mask       = coreMask;
    cluster_core_stack_size = coreStackSize;
    cluster_stacks_start    = stacksPtr;

    /* FC calls cluster */
    EU_CLUSTER_EVT_TrigSet(FC_NOTIFY_CLUSTER_EVENT, 0);
}

void CLUSTER_Start(int cid, int nbCores, int control_icache_seperation) {

    if(!cluster_is_on) {
#ifndef __PLATFORM_GVSOC__
        PMU_ClusterPowerOn();
#endif

        #if defined(__GAP9__)
        if (control_icache_seperation)
            CLUSTER_SCBC->ICACHE_SEPERATION = 0x1;
        #endif

        /*
         * Fetch all cores, they will directly jump to the PE loop
         * waiting from orders through the dispatcher
         */
        for (int i = 0; i < nbCores; i++) {
            #if defined(__GAP8__)
            SCB->BOOT_ADDR[i] = 0x1C000100;
            #elif defined(__GAP9__)
            SCB->BOOT_ADDR[i] = 0x1C008180;
            #endif
        }
        SCB->FETCH_EN = (1 << nbCores) - 1;

        /* Now do the runtime initializations */
        /* In case the chip does not support L1 preloading, the initial L1 data are in L2, we need to copy them to L1 */
        memcpy(&__l1_preload_start, &__l1_preload_start_inL2, (size_t)&__l1_preload_size);

        /* Initialize malloc heap */
        L1_MallocInit();

        CLUSTER_FC_Delegate_Init();

        cluster_is_on = 1;
    }

    if(!cluster_is_init) {
        /* Cluster Stack init*/
        uint32_t stacksPtr = (uint32_t) L1_Malloc(nbCores * CORE_STACK_SIZE);
        CLUSTER_FC2CL_StackInit(cid, nbCores, stacksPtr, CORE_STACK_SIZE);
    }
}

void CLUSTER_Wait(int cid)
{
    /* If Cluster has not finished previous task, wait */
    while(master_task.entry)
    {
        #if defined(__GAP8__)
        EU_EVT_MaskWaitAndClr(1 << CLUSTER_NOTIFY_FC_EVENT);
        #elif defined(__GAP9__)
        ITC_WaitEvent_NOIRQ(CLUSTER_NOTIFY_FC_EVENT);
        #endif

        CLUSTER_FC_Delegate();
    }

}

static inline void CLUSTER_FC2CL_StackDeInit()
{
    /* Wait cluster finish task */
    CLUSTER_Wait(0);

    /* Set stack size = 0 */
    cluster_core_stack_size = 0;

    /* FC calls cluster */
    EU_CLUSTER_EVT_TrigSet(FC_NOTIFY_CLUSTER_EVENT, 0);

    /* Wait response from cluster cores */
    while(cluster_core_stack_size == 0)
    {
        #if defined(__GAP8__)
        EU_EVT_MaskWaitAndClr(1 << CLUSTER_NOTIFY_FC_EVENT);
        #elif defined(__GAP9__)
        ITC_WaitEvent_NOIRQ(CLUSTER_NOTIFY_FC_EVENT);
        #endif
    }
}


/*!
 * @brief All cores do stack deinit.
 *
 * @param  coreMask      Mask for each core.
 * @note .
 */
void CLUSTER_StackDeInit() {
    CLUSTER_CoresFork(CLUSTER_UnSetCoreStack, (void *)0);
}

void CLUSTER_Stop(int cid) {
    CLUSTER_FC2CL_StackDeInit();

    SCB->FETCH_EN = 0x0;

#ifndef __PLATFORM_GVSOC__
    PMU_ClusterPowerOff();
#endif

    cluster_is_on = 0;
    cluster_is_init = 0;
}

/*!
 * @brief All cores do task init.
 * Master core 0 do stacks initilization.
 * @param.
 * @note .
 */
void CLUSTER_StackInit() {
    if (!cluster_is_init) {
        /* do_ctors */
    }

    CLUSTER_CoresFork(CLUSTER_SetCoreStack, (void *)cluster_stacks_start);
}

void CLUSTER_SleepWait() {

    /* Cluster Icache Enable*/
    SCBC->ICACHE_ENABLE = 0xFFFFFFFF;

    /* Initialization for the task dispatch loop */
    EU_EVT_MaskSet((1 << EU_DISPATCH_EVENT)
                   | (1 << EU_MUTEX_EVENT)
                   | (1 << EU_HW_BARRIER_EVENT)
                   | (1 << EU_LOOP_EVENT));

    /* s1 : CORE_EU_DISPATCH_DEMUX_BASE  0x204080 */
    /* s2 : CORE_EU_BARRIER_DEMUX_BASE   0x204200 */
    /* s3 : CORE_EU_CORE_DEMUX_BASE      0x204000 */
    /* asm volatile ("add    s1,  x0,  %0" :: "r" (CORE_EU_DISPATCH_DEMUX_BASE)); */
    /* asm volatile ("add    s2,  x0,  %0" :: "r" (CORE_EU_BARRIER_DEMUX_BASE)); */
    /* asm volatile ("add    s3,  x0,  %0" :: "r" (CORE_EU_CORE_DEMUX_BASE)); */

    if(__core_ID())
    {
        while(1) {
            asm volatile (
                "1:\n\t"
#ifdef _OPENMP
                    "lw     t0,  cluster_is_init \n\t"
                    "beqz   t0,  2f              \n\t"
                    "jal    x1,  omp_init        \n\t"
#endif

                "2:\n\t"
                    "li     s1,  0x204080      \n\t"
                    "li     s2,  0x204200      \n\t"
                    "p.elw  t0,  0(s1)         \n\t"
                    "p.elw  a0,  0(s1)         \n\t"
                    "andi   t1,  t0,   1       \n\t"
                    "bne    t1,  zero, 3f      \n\t"
                    "jalr   t0                 \n\t"
                    "p.elw  t0,  0x1c(s2)      \n\t"
                    "j      4f                 \n"
                "3:\n\t"
                    "p.bclr t0, t0, 0,0        \n\t"
                    "jalr   t0                 \n"
                "4:"
                );
        } // slaveloop
    } else {
        /*
         * Core 0 will wait for tasks from FC side
         */

        __enable_irq();

        while(1) {
            // MasterLoop
            asm volatile (
            "1:\n\t"
                "li      s2,  0x204200 \n\t"
                "li      s3,  0x204000 \n\t"
                // EU_EVT_MaskWaitAndClr(1 << FC_NOTIFY_CLUSTER_EVENT);
                "p.bset  t1, x0, 0, %0 \n\t"
                "sw      t1, 0x8(s3)   \n\t"
                "p.elw   t0, 0x3C(s3)  \n\t"
                "sw      t1, 0x4(s3)   \n\t"

                "lw      t0, cluster_is_init               \n\t"
                "beqz    t0, 2f                            \n\t"

                "lw      t0, (cluster_core_stack_size)     \n\t"
                "bne     t0, zero, 3f                      \n\t"
                "jal     CLUSTER_StackDeInit               \n\t"
                "j       3f                                \n\t"
            "2:\n\t"
                "lw      a0, (cluster_stacks_start)        \n\t"
                "lw      a1, (cluster_core_stack_size)     \n\t"
                "add     sp, a0, a1                        \n\t"
                "jal     CLUSTER_StackInit                 \n\t"
#ifdef _OPENMP
                "jal    x1,  omp_init                      \n\t"
                "la     t1,  cluster_ret_value             \n\t"
                "sw     a0, 0(t1)                          \n\t"
                // Notify FC to exit
#if defined(__GAP8__)
                // EU_FC_EVT_TrigSet(CLUSTER_NOTIFY_FC_EVENT, 0);
                "li     a0, 0x1B200e04                     \n\t"
                "sw     x0, 0(a0)                          \n\t"
#elif defined(__GAP9__)
                // ITC_SetIRQ(CLUSTER_NOTIFY_FC_EVENT);
                "li     a0, 0x1A109010                     \n\t"
                "addi   a1, x0, 0x2                        \n\t"
                "sw     a1, 0(a0)                          \n\t"
#endif
                "li     s2,  0x204200      \n\t"
                "p.elw  t0,  0x1c(s2)                      \n\t"
#endif
            "3:\n\t"
                "lw      t0, (master_task)                 \n\t"
                "beqz    t0, 4f                            \n\t"
                "p.bclr  t0, t0, 0, 0                      \n\t"
                "la      a0, master_task                   \n\t"
                "lw      a0, 4(a0)                         \n\t"
                "jalr    t0                                \n\t"
                "la      a0, master_task                   \n\t"
                "sw      x0, (a0)                          \n\t"
                "jal     CLUSTER_TaskFinish                \n\t"
                "j       4f                                \n"
            "4:"
            :: "I" (FC_NOTIFY_CLUSTER_EVENT)
                );
        }
    } // masterloop
}

void CLUSTER_TaskFinish(){
    /* Clear master task */
    master_task.entry = NULL;

    /* Notify FC the current task is finished */
    #if defined(__GAP8__)
    EU_FC_EVT_TrigSet(CLUSTER_NOTIFY_FC_EVENT, 0);
    #elif defined(__GAP9__)
    ITC_SetIRQ(CLUSTER_NOTIFY_FC_EVENT);
    #endif
}

uint8_t CLUSTER_GetCoreMask() {
    return cluster_core_mask;
}

void CLUSTER_CoresFork(void (*entry)(void *), void* arg) {
    EU_BarrierSetup(cluster_core_mask);

    if (cluster_core_mask != GAP_CLUSTER_CORE0_MASK)
    {
        EU_Dispatch_TeamConfig(cluster_core_mask & GAP_CLUSTER_WITHOUT_CORE0_MASK );
        EU_DispatchPush((int)entry);
        EU_DispatchPush((int)arg);
    }

    entry(arg);

    EU_BarrierTriggerWaitClear();
}

void CLUSTER_SendTask(uint32_t cid, void *entry, void* arg, cluster_task_t *end) {

    /* If Cluster has not finished previous task, wait */
    while(!cluster_is_init || master_task.entry)
    {
        #if defined(__GAP8__)
        EU_EVT_MaskWaitAndClr(1 << CLUSTER_NOTIFY_FC_EVENT);
        #elif defined(__GAP9__)
        ITC_WaitEvent_NOIRQ(CLUSTER_NOTIFY_FC_EVENT);
        #endif
    }

    master_task.entry = entry;
    master_task.arg   = arg;
    master_task.end   = end;

    EU_CLUSTER_EVT_TrigSet(FC_NOTIFY_CLUSTER_EVENT, 0);
}

#endif /* FEATURE_CLUSTER */
