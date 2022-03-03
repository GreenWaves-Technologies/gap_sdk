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

static inline void pi_cl_team_barrier();

static inline void pos_team_cc_barrier()
{
    eu_bar_trig_wait_clr(eu_bar_addr(0));
}


static inline void pos_team_config_offload(int nb_cores)
{
    unsigned int core_mask = ((1<<nb_cores) - 1);
    unsigned int dispatch_core_mask = __BITCLR(core_mask, 1, 0);
    eu_dispatch_team_config(dispatch_core_mask);
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

static inline void pos_team_offload_preset(void (*entry)(void *), void *arg)
{
    eu_dispatch_push((int)entry);
    eu_dispatch_push((int)arg);
}

static inline void pos_team_offload_wait()
{
    pi_cl_team_barrier();
}


static inline int pi_cl_team_nb_cores()
{
  return __FL1(pulp_read32(eu_bar_addr(0) + EU_HW_BARR_TRIGGER_MASK) + 1);
}


#ifdef CONFIG_PE_TASK

extern void pi_cl_workitem_empty_callback(void *arg);

static inline void pi_cl_workitem_init(pi_cl_workitem_t *task, void (*entry)(pi_cl_workitem_t *task, int id))
{
    task->entry = entry;
    task->nb_cores = 1;
    task->callback_entry = pi_cl_workitem_empty_callback;
    task->piped_task = NULL;
}

static inline void pi_cl_workitem_set_nb_tasks(pi_cl_workitem_t *task, unsigned int nb_cores)
{
    task->nb_cores = nb_cores;
}

static inline void pi_cl_workitem_set_callback(pi_cl_workitem_t *task, void (*callback)(void *arg), void *arg)
{
    task->callback_entry = callback;
    task->callback_arg = arg;
}

static inline void pi_cl_workitem_set_arg(pi_cl_workitem_t *task, int index, uint32_t value)
{
    task->args[index] = value;
}

static inline uint32_t pi_cl_workitem_get_arg(pi_cl_workitem_t *task, int index)
{
    return task->args[index];
}

static inline void pi_cl_workitem_push(pi_cl_workitem_t *task)
{
    task->next = NULL;
    task->nb_done_cores = task->nb_cores;
    task->nb_cores_popped = 0;
    eu_mutex_lock_from_id(0);
    pos_pending_task++;
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


static inline void pi_cl_workitem_wait(pi_cl_workitem_t *task)
{
    while(*(volatile int8_t *)&task->nb_done_cores != 0)
    {
      eu_evt_maskWaitAndClr(1<<POS_EVENT_CLUSTER_CALL_EVT);
    }
}


static inline void pi_cluster_pe_piped_task_push(pi_cl_workitem_t *task, pi_cl_workitem_t *piped_task)
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


static inline void pi_cl_team_prepare_fork(int nb_cores)
{
    pos_team_config_offload(nb_cores);
}


static inline void pi_cl_team_preset_fork(void (*entry)(void *), void *arg)
{
    pos_team_offload_preset(entry, arg);

    entry(arg);

    pos_team_offload_wait();
}

static inline void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
    pos_team_offload(nb_cores, entry, arg);

    entry(arg);

    pos_team_offload_wait();
}

static inline void pi_cl_team_fork_pe_entry(pi_cl_workitem_t *task, int id)
{
    void (*entry)(void *) = (void (*)(void *))pi_cl_workitem_get_arg(task, 0);
    void *arg = (void *)pi_cl_workitem_get_arg(task, 1);
    entry(arg);
}

static inline void pi_cl_team_fork_cc(int nb_cores, void (*entry)(void *), void *arg)
{
    pi_cl_workitem_t workitem;

    if (nb_cores == 0)
    {
        nb_cores = pi_cl_team_nb_cores();
    }
    if ((unsigned int)nb_cores == pi_cl_cluster_nb_pe_cores()+1)
    {
        nb_cores = pi_cl_cluster_nb_pe_cores();
    }

    uint32_t core_mask = ((1<<nb_cores)-1) | (1<<ARCHI_CC_CORE_ID);
    eu_bar_setup_mask(eu_bar_addr(0), core_mask, core_mask);

    pi_cl_workitem_init(&workitem, pi_cl_team_fork_pe_entry);
    pi_cl_workitem_set_arg(&workitem, 0, (uint32_t)entry);
    pi_cl_workitem_set_arg(&workitem, 1, (uint32_t)arg);
    pi_cl_workitem_set_nb_tasks(&workitem, nb_cores);
    pi_cl_workitem_push(&workitem);

    entry(arg);

    pi_cl_workitem_wait(&workitem);

    core_mask = (1 << pi_cluster_get_task_nb_cores()) - 1;
    eu_bar_setup_mask(eu_bar_addr(0), core_mask, core_mask);

    // Since we used workitems in a non-standard way, same workitem was pushed several times
    // for callbacks, now we need to undo it
    pos_cluster_pool.first_call_from_pe = NULL;
    pos_pending_task = 0;
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


static inline void pi_cl_team_barrier_wait(uint32_t barrier)
{
    eu_bar_trig_wait_clr(barrier);
}

static inline void pi_cl_team_barrier_wait_safe(uint32_t barrier)
{
    eu_bar_trig_wait_clr_safe(barrier);
}


static inline uint32_t pi_cl_team_barrier_nb_available(void)
{
    return __builtin_popcount(eu_bitfield_value_get(POS_BARRIER_ALLOC_ID));
}

static inline uint32_t pi_cl_team_barrier_id(uint32_t barrier)
{
    return eu_bar_id(barrier);
}

static inline uint32_t pi_cl_team_barrier_alloc(void)
{
    int id = eu_bitfield_alloc_from_id(POS_BARRIER_ALLOC_ID);
    if (id == 32)
        return 0;
    else
        return eu_bar_addr(id);

}

static inline void pi_cl_team_barrier_free(uint32_t barrier)
{
    eu_bitfield_set_from_id(POS_BARRIER_ALLOC_ID, 1 << eu_bar_id(barrier));
}

static inline void pi_cl_team_barrier_set(uint32_t barrier, uint32_t team_mask)
{
    eu_bar_setup_mask(barrier, team_mask, team_mask);
}



static inline void pi_cl_team_barrier()
{
    hal_compiler_barrier();
    eu_bar_trig_wait_clr(eu_bar_addr(0));
    hal_compiler_barrier();
}

static inline void pi_cl_team_barrier_cc()
{
    pos_team_cc_barrier();
}

#ifdef __GAP9__
static inline int pi_cl_bitfield_nb_available()
{
    return __builtin_popcount(eu_bitfield_value_get(POS_BITFIELD_ALLOC_ID));
}


static inline uint32_t pi_cl_bitfield_alloc()
{
    int id = eu_bitfield_alloc_from_id(POS_BITFIELD_ALLOC_ID);
    if (id == 32)
        return 0;
    else
        return eu_bitfield_addr(id);
}


static inline void pi_cl_bitfield_free(uint32_t bitfield)
{
    eu_bitfield_set_from_id(POS_BITFIELD_ALLOC_ID, 1 << eu_bitfield_id(bitfield));
}


static inline int pi_cl_bitfield_id(uint32_t bitfield)
{
    return eu_bitfield_id(bitfield);
}


static inline void pi_cl_bitfield_value_set(uint32_t bitfield, uint32_t value)
{
    eu_bitfield_value_set(bitfield, value);
}


static inline uint32_t pi_cl_bitfield_value_get(uint32_t bitfield)
{
    return eu_bitfield_value_get(bitfield);
}


static inline void pi_cl_bitfield_bits_set(uint32_t bitfield, uint32_t value)
{
    eu_bitfield_set(bitfield, value);
}


static inline void pi_cl_bitfield_bits_clr(uint32_t bitfield, uint32_t value)
{
    eu_bitfield_clear(bitfield, value);
}


static inline uint32_t pi_cl_bitfield_bit_alloc(uint32_t bitfield)
{
    return eu_bitfield_alloc(bitfield);
}


static inline int pi_cl_sem_nb_available()
{
    return __builtin_popcount(eu_bitfield_value_get(POS_SEM_ALLOC_ID));
}


static inline uint32_t pi_cl_sem_alloc()
{
    int id = eu_bitfield_alloc_from_id(POS_SEM_ALLOC_ID);
    if (id == 32)
        return 0;
    else
        return eu_sem_addr(id);
}


static inline void pi_cl_sem_free(uint32_t sem)
{
    eu_bitfield_set_from_id(POS_SEM_ALLOC_ID, 1 << eu_sem_id(sem));
}


static inline int pi_cl_sem_id(uint32_t sem)
{
    return eu_sem_id(sem);
}


static inline void pi_cl_sem_set(uint32_t sem, uint32_t value)
{
    eu_sem_set(sem, value);
}


static inline uint32_t pi_cl_sem_get(uint32_t sem)
{
    return eu_sem_get(sem);
}


static inline void pi_cl_sem_inc(uint32_t sem, uint32_t value)
{
    eu_sem_inc(sem, value);
}


static inline uint32_t pi_cl_sem_dec(uint32_t sem)
{
    return eu_sem_dec(sem);
}


static inline uint32_t pi_cl_sem_dec_cc(uint32_t sem)
{
    return eu_sem_dec_irq_safe(sem);
}


static inline uint32_t pi_cl_sem_load_inc(uint32_t sem)
{
    return eu_sem_load_inc(sem);
}
#endif


#endif
