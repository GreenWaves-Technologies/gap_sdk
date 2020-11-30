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

#ifndef __POS_IMPLEM_PE_H__
#define __POS_IMPLEM_PE_H__


static inline void pos_team_cc_barrier()
{
#ifdef ARCHI_CC_CORE_ID
    eu_bar_trig_wait_clr(eu_bar_addr(1));
#else
    eu_bar_trig_wait_clr(eu_bar_addr(0));
#endif
}


static inline void pos_team_config_offload(int nb_cores)
{
    unsigned int core_mask = ((1<<nb_cores) - 1);
    eu_dispatch_team_config(core_mask);
    eu_bar_setup_mask(eu_bar_addr(0), core_mask, core_mask);
    // Configure the barrier so that the rendez-vous is between slave cores but the master
    // also receives the notification so that it can wait for the offload termination.
    eu_bar_setup_mask(eu_bar_addr(1), core_mask | (1<<8), core_mask | (1<<8));
}


static inline void pos_team_offload(int nb_cores, void (*entry)(void *), void *arg)
{
    if (nb_cores)
        pos_team_config_offload(nb_cores);

    eu_dispatch_push((int)entry);
    eu_dispatch_push((int)arg);
}

static inline void pos_team_offload_wait()
{
    pos_team_cc_barrier();
}


static inline int pi_cl_team_nb_cores()
{
  return __FL1(pulp_read32(eu_bar_addr(0) + EU_HW_BARR_TRIGGER_MASK) + 1);
}


#ifdef CONFIG_PE_TASK

static inline void pi_cluster_pe_task_init(pi_cluster_pe_task_t *task, void (*entry)(pi_cluster_pe_task_t *task, int id))
{
    task->entry = entry;
    task->nb_cores = 1;
    task->stacks = NULL;
    task->callback_entry = NULL;
    task->piped_task = NULL;
}

static inline void pi_cluster_pe_task_nb_cores(pi_cluster_pe_task_t *task, unsigned int nb_cores)
{
    task->nb_cores = nb_cores;
}

static inline void pi_cluster_pe_task_callback(pi_cluster_pe_task_t *task, void (*callback)(void *arg), void *arg)
{
    task->callback_entry = callback;
    task->callback_arg = arg;
}

static inline void pi_cluster_pe_task_arg(pi_cluster_pe_task_t *task, int index, uint32_t value)
{
    task->args[index] = value;
}

static inline void pi_cluster_pe_task_push(pi_cluster_pe_task_t *task)
{
    task->next = NULL;
    task->nb_done_cores = task->nb_cores;
    task->nb_cores_popped = 0;
    eu_mutex_lock_from_id(0);
    if (pos_cluster_pool.first_pe_task)
    {
        pos_cluster_pool.last_pe_task->next = task;
    }
    else
    {
        pos_cluster_pool.first_pe_task = task;
    }

    pos_cluster_pool.last_pe_task = task;
    eu_mutex_unlock_from_id(0);

    eu_sem_inc(eu_sem_addr(15), task->nb_cores);
}


static inline void pi_cluster_pe_task_wait(pi_cluster_pe_task_t *task)
{
    while(*(volatile int8_t *)&task->nb_done_cores != 0)
    {
      eu_evt_maskWaitAndClr(1<<POS_EVENT_CLUSTER_CALL_EVT);
    }
}


static inline void pi_cluster_pe_piped_task_push(pi_cluster_pe_task_t *task, pi_cluster_pe_task_t *piped_task)
{
    piped_task->next = NULL;
    piped_task->nb_done_cores = piped_task->nb_cores;
    piped_task->nb_cores_popped = 0;

    eu_mutex_lock_from_id(0);

    int nb_free_stages = task->nb_cores - task->nb_done_cores;

    task->piped_task = piped_task;
    piped_task->nb_cores = nb_free_stages;

    if (nb_free_stages > 0)
    {
        if (pos_cluster_pool.first_pe_task)
        {
            pos_cluster_pool.last_pe_task->next = piped_task;
        }
        else
        {
            pos_cluster_pool.first_pe_task = piped_task;
        }

        pos_cluster_pool.last_pe_task = piped_task;

        eu_sem_inc(eu_sem_addr(15), nb_free_stages);
    }

    eu_mutex_unlock_from_id(0);
}

#endif


static inline void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
    pos_team_offload(nb_cores, entry, arg);

#ifndef ARCHI_CC_CORE_ID
    entry(arg);
#endif

    pos_team_offload_wait();
}


static inline void pi_cl_team_fork_cc(int nb_cores, void (*entry)(void *), void *arg)
{
    pos_team_offload(nb_cores, entry, arg);

    entry(arg);

    pos_team_offload_wait();
}


static inline void pi_cl_team_critical_enter(void)
{
    hal_compiler_barrier();
    eu_mutex_lock_from_id(0);
}


static inline void pi_cl_team_critical_exit(void)
{
    eu_mutex_unlock_from_id(0);
    hal_compiler_barrier();
}


static inline void pi_cl_team_barrier()
{
    hal_compiler_barrier();
    eu_bar_trig_wait_clr(eu_bar_addr(0));
    hal_compiler_barrier();
}



#endif