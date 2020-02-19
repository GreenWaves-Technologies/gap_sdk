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

#ifndef __PMSIS_IMPLEM_PMSIS_TYPES_H__
#define __PMSIS_IMPLEM_PMSIS_TYPES_H__

#include "pmsis/pmsis_types.h"

#ifndef PI_TASK_IMPLEM
#define PI_TASK_IMPLEM \
    int8_t destroy;
#endif

#ifndef PI_TASK_IMPLEM_NB_DATA
#define PI_TASK_IMPLEM_NB_DATA 8
#endif  /* PI_TASK_IMPLEM_NB_DATA */

typedef struct pi_task
{
    // Warning, might be accessed inline in asm, and thus can not be moved
    uintptr_t arg[4];
    int32_t id;
    uint32_t data[PI_TASK_IMPLEM_NB_DATA];
    pi_sem_t wait_on;
    struct pi_task *next;
    volatile int8_t done;
    int8_t core_id;
    int8_t cluster_id;
    PI_TASK_IMPLEM;
} pi_task_t;

#endif  /* __PMSIS_IMPLEM_PMSIS_TYPES_H__ */
