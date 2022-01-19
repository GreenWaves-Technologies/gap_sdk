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

#ifndef __POS_IMPLEM_TASK_H__
#define __POS_IMPLEM_TASK_H__

#include <hal/pulp.h>


void pos_init_stop();
void pos_sched_init();
void pos_task_handle_blocking(void *arg);
void pos_task_handle();
void pos_task_remote_enqueue();
void pos_task_cancel(pi_task_t *task);


void pos_time_task_cancel(pi_task_t *task);


static inline void pi_task_destroy(pi_task_t *task)
{
}


static inline void pi_task_wait_on(struct pi_task *task)
{
    int irq = hal_irq_disable();

    while(likely(task->arg[0]))
    {
        pos_task_handle();
    }

    hal_irq_restore(irq);
}


POS_TEXT_L2 static inline void pi_task_wait_on_xip(struct pi_task *task)
{
    int irq = hal_irq_disable();

    while(likely(task->arg[0]))
    {
        hal_itc_wait_for_interrupt();
        hal_irq_enable();
        hal_irq_disable();
    }

    hal_irq_restore(irq);
}


__attribute__((always_inline)) static inline int pos_task_is_block(struct pi_task *task)
{
    return task->arg[0] == (uint32_t)pos_task_handle_blocking;
}


__attribute__((always_inline)) static inline void pos_task_handle_blocking_inlined(void *arg)
{
    pi_task_t *task = arg;
    task->arg[0] = 0;
}

__attribute__((always_inline)) static inline void pos_task_block_done(struct pi_task *task)
{
    pos_task_handle_blocking(task);
}


static inline struct pi_task *pi_task_block(struct pi_task *task)
{
    task->arg[0] = (uint32_t)pos_task_handle_blocking;
    task->arg[1] = (uint32_t)task;
    return task;
}


static inline void __attribute__((always_inline)) pos_task_push_locked_no_irq(pi_task_t *task)
{
    task->next = NULL;
    if (pos_sched_first)
    {
        pos_sched_last->next = task;
    }
    else
    {
        pos_sched_first = task;
    }
    pos_sched_last = task;
}


static inline void __attribute__((always_inline)) pos_task_push_locked(pi_task_t *task)
{
    uint32_t callback = task->arg[0];

    if (callback & 1)
    {
        void (*irq_callback)(void *) = (void (*)(void *))callback;
        irq_callback((void *)task->arg[1]);
    }
    else
    {
        pos_task_push_locked_no_irq(task);
    }
}

static inline void __attribute__((always_inline)) pi_task_push_irq_safe(pi_task_t *task)
{
    pos_task_push_locked(task);
}

static inline void __attribute__((always_inline)) pi_task_push(pi_task_t *task)
{
    int irq = hal_irq_disable();
    pos_task_push_locked(task);
    hal_irq_restore(irq);
}


static inline struct pi_task *pi_task_callback(struct pi_task *task, void (*callback)(void*), void *arg)
{
    task->arg[0] = (uint32_t)callback;
    task->arg[1] = (uint32_t)arg;
    return task;
}


static inline struct pi_task *pi_task_irq_callback(struct pi_task *task, void (*callback)(void*), void *arg)
{
    task->arg[0] = (uint32_t)callback | 1;
    task->arg[1] = (uint32_t)arg;
    return task;
}


static inline struct pi_task *pi_task_callback_rearm(struct pi_task *task)
{
    return task;
}


static inline pi_callback_t *pi_callback(pi_callback_t *callback,
                                          void (*entry)(void*), void *arg)
{
  callback->entry = entry;
  callback->arg = arg;
  return callback;
}


static inline void pos_task_cluster_notif_req_done(int cid)
{
  eu_evt_trig(eu_evt_trig_cluster_addr(cid, POS_CL_WAIT_TASK_EVT), 0);
}

static inline void cl_notify_task_done(uint8_t *done, uint8_t cluster_id)
{
    *done = 1;
    pos_task_cluster_notif_req_done(cluster_id);
}



static inline void pos_task_init_from_cluster(pi_task_t *task)
{
}


#endif