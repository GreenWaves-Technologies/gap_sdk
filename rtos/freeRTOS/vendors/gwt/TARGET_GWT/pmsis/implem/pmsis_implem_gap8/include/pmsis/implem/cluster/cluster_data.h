/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_CLUSTER_CLUSTER_DATA_H__
#define __PMSIS_IMPLEM_CLUSTER_CLUSTER_DATA_H__

#if !(defined(LANGUAGE_ASSEMBLY) || defined(__ASSEMBLER__))

#include <stdint.h>

struct cluster_driver_data
{
    // prepare a small fifo so that FC can pipeline tasks
    // --> need to be first for inline access
    struct pi_cluster_task *task_first;
    struct pi_cluster_task *task_last;
    // event kernel attached
    struct pmsis_event_kernel_wrap* event_kernel;
    // metadata
    uint32_t cluster_is_on;
    pmsis_mutex_t task_mutex;
    pmsis_mutex_t powerstate_mutex;
    spinlock_t fifo_access;
    void *heap_start;
    uint32_t heap_size;
};

#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */

#define PI_CLUSTER_TASK_FUNCTION_OFFSET       ( 0x00 )
#define PI_CLUSTER_TASK_FUNC_ARGS_OFFSET      ( 0x04 )
#define PI_CLUSTER_TASK_STACK_PTR_OFFSET      ( 0x08 )
#define PI_CLUSTER_TASK_STACK_MST_SIZE_OFFSET ( 0x0C )
#define PI_CLUSTER_TASK_STACK_SLV_SIZE_OFFSET ( 0x10 )
#define PI_CLUSTER_TASK_TEAM_CORES_OFFSET     ( 0x14 )
#define PI_CLUSTER_TASK_CALLBACK_OFFSET       ( 0x18 )
#define PI_CLUSTER_TASK_STACK_ALLOC_OFFSET    ( 0x1C )
#define PI_CLUSTER_TASK_NEXT_TASK_OFFSET      ( 0x20 )
/* Implem specific */
#define PI_CLUSTER_TASK_TEAM_MASK_OFFSET      ( 0x24 )


#endif  /* __PMSIS_IMPLEM_CLUSTER_CLUSTER_DATA_H__ */
