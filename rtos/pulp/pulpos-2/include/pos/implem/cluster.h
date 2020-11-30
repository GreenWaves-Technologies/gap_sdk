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

#ifndef __POS_IMPLEM_CLUSTER_H__
#define __POS_IMPLEM_CLUSTER_H__


void pos_cluster_push_fc_event(pi_task_t *event);


static inline void pos_cluster_notif_req_done(int cid)
{
    eu_evt_trig(eu_evt_trig_cluster_addr(cid, POS_EVENT_CLUSTER_CALL_EVT), 0);
}

static inline void pi_cl_send_task_to_fc(pi_task_t *task)
{
    pos_cluster_push_fc_event(task);
}

static inline void pi_cl_send_callback_to_fc(pi_callback_t *callback)
{
    pos_cluster_push_fc_event((pi_task_t *)(((unsigned int)callback) | 0x1));
}

static inline void pi_cl_cluster_task_terminate(struct pi_cluster_task *task)
{
    pos_cluster_push_fc_event(task->completion_callback);
}

static inline struct pi_cluster_task *pi_cluster_task(struct pi_cluster_task *task, void (*entry)(void*), void *arg)
{
    task->entry = entry;
    task->arg = arg;
    task->stacks = (void *)0;
    task->stack_size = 0;
    task->nb_cores = pi_cl_cluster_nb_cores();
    return task;
}

static inline struct pi_cluster_task *pi_cluster_task_rearm(struct pi_cluster_task *task)
{
    return task;
}



#endif