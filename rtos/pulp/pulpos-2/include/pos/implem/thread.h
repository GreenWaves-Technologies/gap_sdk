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

#pragma once

#include "pmsis.h"


typedef struct pi_thread_s
{
    struct {
        int ra;
        int s0;
        int s1;
        int s2;
        int s3;
        int s4;
        int s5;
        int s6;
        int s7;
        int s8;
        int s9;
        int s10;
        int s11;
        int sp;
    } regs;
    struct pi_thread_s *next;
    struct pi_thread_s *waiting;
    void *status;
    char finished;
    char priority;
} pi_thread_t;

typedef struct pi_thread_queue_s
{
  struct pi_thread_s *first;
  struct pi_thread_s *last;
} pi_thread_queue_t;


#define PI_THREAD_MAX_PRIORITIES 3


extern PI_FC_L1_TINY pi_thread_queue_t __pi_thread_ready_queues[PI_THREAD_MAX_PRIORITIES];
extern PI_FC_L1_TINY uint32_t __pi_thread_ready;
extern PI_FC_L1_TINY pi_thread_t *__pi_thread_current;


int pi_thread_create(pi_thread_t *thread, void *(*entry)(void *), void *arg, int priority, void *stack, unsigned int stack_size);

void __pi_thread_switch(pi_thread_t *current, pi_thread_t *new);

void __pi_thread_switch_to_next();

void __pi_thread_sched_init();

static inline pi_thread_t *__pi_thread_dequeue_first(pi_thread_queue_t *queue)
{
    pi_thread_t *result = queue->first;
    queue->first = result->next;
    return result;
}


static inline pi_thread_t *__pi_thread_dequeue_ready()
{
    int priority = __FF1(__pi_thread_ready);
    pi_thread_t *thread = __pi_thread_dequeue_first(&__pi_thread_ready_queues[priority]);
    if (__pi_thread_ready_queues[priority].first == NULL)
    {
        __pi_thread_ready = __BITCLR_R(__pi_thread_ready, 1, priority);
    }
    return thread;
}


void __pi_thread_enqueue_list(pi_thread_t *thread);
