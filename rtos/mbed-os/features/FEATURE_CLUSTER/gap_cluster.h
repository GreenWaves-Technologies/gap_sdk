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

#ifndef _GAP_CLUSTER_H_
#define _GAP_CLUSTER_H_

#include <cmsis.h>
#include <gap_pmu.h>
#include <gap_handler_wrapper.h>
#include "gap_l1_malloc.h"
#include "gap_cluster_fc_delegate.h"

#ifdef FEATURE_CLUSTER
/*!
 * @addtogroup cluster
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Compilor error relocation truncated to fit: */
#define  GAP_CLUSTER_CORE0_MASK                   0x00000001
#define  GAP_CLUSTER_WITHOUT_CORE0_MASK           0xFFFFFFFE

typedef struct _cluster_task_s {
    void (*entry)(void *);
    void *arg;
    struct _cluster_task_s *end;
} cluster_task_t ;

extern GAP_L1_TINY_DATA   cluster_task_t master_task;


/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


/*!
 * @brief Activate cluster domain clock and do initialization
 *
 * @param  cid      The cluster ID - 0 if only one cluster
 * @param  nbCores  The number of cores we want to use
 * @param  int control_icache_seperation  The flag to indicate the control core's icache
                                          is seperated with other task cores' shared icache
 * @note .
 */
void CLUSTER_Start(int cid, int nbCores, int control_icache_seperation);

/*!
 * @brief Isolate cluster domain clock and do de-initialization.
 *
 * @param  cid      The cluster ID - 0 if only one cluster
 * @note .
 */
void CLUSTER_Stop(int cid);

/*!
 * @brief Wait the cluster to finish.
 *
 * @param  cid      The cluster ID - 0 if only one cluster
 * @note .
 */
void CLUSTER_Wait(int cid);

/*!
 * @brief FC push task to Cluster master core.
 *
 * @param  cid    The cluster ID - 0 if only one cluster
 * @param  entry  The task function pointer
 * @param  arg    The task function argument
 * @param  end    The task executed when the main task has finished
 * @param.
 * @note .
 */
void CLUSTER_SendTask(uint32_t cid, void *entry, void* arg, cluster_task_t *end);

/*!
 * @brief Get the core numbers in use mask, i.e. (1 << core_number_in_use) - 1
 *
 * @note .
 */
uint8_t CLUSTER_GetCoreMask();

/*!
 * @brief Called from master core on cluster (core 0) to push a task to the other cores.
 *
 * @param  entry  The task function pointer
 * @param  arg    The task function argument
 * @note .
 */
void CLUSTER_CoresFork(void (*entry)(void *), void* arg);

/*!
 * @brief Synchronization barrier for cluster cores
 *
 * @note When multiple cores in the cluster execute a task together this can be used to wait
 * for all cores to finish before continuing to a subsequent task.
 * Core is clock gated while waiting.
 */
static inline void CLUSTER_SynchBarrier()
{
    EU_BarrierTriggerWaitClear();
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /* FEATURE_CLUSTER*/
#endif /*_GAP_CLUSTER_H_*/
