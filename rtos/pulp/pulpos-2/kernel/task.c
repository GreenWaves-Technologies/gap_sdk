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

#include "pmsis.h"
#include <stdio.h>


PI_FC_TINY pi_task_t *pos_sched_first;
PI_FC_TINY pi_task_t *pos_sched_last;



POS_TEXT_L2 void pos_task_handle_blocking(void *arg)
{
    pi_task_t *task = arg;
    task->arg[0] = 0;
}

POS_TEXT_L2 void pos_task_handle()
{
    pi_task_t *task = pos_sched_first;

    if (unlikely(task == NULL))
    {
        // Pop first event from the queue. Loop until we pop a null event
        // We must always read again the queue head, as the executed
        // callback can modify the queue 
        pos_irq_wait_for_interrupt();
        hal_irq_enable();
        hal_irq_disable();
        task = *((pi_task_t * volatile *)&pos_sched_first);
    }

    while (likely(task != NULL))
    {
        pos_sched_first = task->next;

        // Read event information and put it back in the scheduler

        void (*callback)(void *) = (void (*)(void *))task->arg[0];
        void *arg = (void *)task->arg[1];

        // Finally execute the event with interrupts enabled
        hal_irq_enable();
        callback(arg);
        hal_irq_disable();

        task = pos_sched_first;

    }
}


POS_TEXT_L2 void pos_task_flush()
{
    while(pos_sched_first)
    {
        pos_task_handle();
    }
}


void pos_task_cancel(pi_task_t *task)
{
    pi_task_t *current = pos_sched_first, *prev = NULL;

    while (current && current != task)
    {
        prev = current;
        current = current->next;
    }

    if (current)
    {
        if (prev)
        {
            prev->next = current->next;
        }
        else
        {
            pos_sched_first = current->next;
        }
    }

    pos_time_task_cancel(task);
}

void pi_task_abort(pi_task_t *task)
{
    pos_task_cancel(task);
}

void pos_sched_init()
{
    pos_sched_first = NULL;
}
