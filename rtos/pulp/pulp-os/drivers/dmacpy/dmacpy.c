/*
 * Copyright (C) 2018 GreenWaves Technologies
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


typedef struct 
{
    pi_task_t *pending_copy;
    pi_task_t *waiting_first;
    pi_task_t *waiting_last;
    int open_count;
    int channel;
} pos_dmacpy_t;


static pos_dmacpy_t pos_dmacpy_dev[ARCHI_UDMA_NB_TCDM];


void pos_dmacpy_handle_copy_asm();
static inline void pos_dmacpy(pos_dmacpy_t *dev, uint32_t l2_addr, uint32_t tcdm_addr, int size, int to_tcdm);



void pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf)
{
    conf->id = 0;
}


void pos_dmacpy_handle_copy(void *arg)
{
    pos_dmacpy_t *dev = (pos_dmacpy_t *)arg;

    pi_task_t *task = dev->pending_copy;
    dev->pending_copy = NULL;

    __rt_event_handle_end_of_task(task);

    task = dev->waiting_first;
    if (task)
    {
        dev->waiting_first = task->implem.next;
        dev->pending_copy = task;

        pos_dmacpy(dev, task->implem.data[0], task->implem.data[1], task->implem.data[2], task->implem.data[3]);
    }
}


int pi_dmacpy_open(struct pi_device *device)
{
    int periph_id = ARCHI_UDMA_TCDM_ID(conf->id);
    int channel = UDMA_EVENT_ID(periph_id);
    struct pi_dmacpy_conf *conf = (struct pi_dmacpy_conf *)device->config;

    pos_dmacpy_t *dev = &pos_dmacpy_dev[conf->id];

    device->data = (void *)dev;

    dev->open_count++;

    if (dev->open_count == 1)
    {
        dev->channel = channel;
        dev->pending_copy = NULL;
        dev->waiting_first = NULL;

        // Activate routing of UDMA cpi soc events to FC to trigger interrupts
        soc_eu_fcEventMask_setEvent(channel);
        soc_eu_fcEventMask_setEvent(channel+1);

        // Deactivate cpi clock-gating
        plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));

        // Redirect all UDMA cpi events to the standard callback
        __rt_udma_register_channel_callback(channel, pos_dmacpy_handle_copy_asm, (void *)dev);
        __rt_udma_register_channel_callback(channel+1, pos_dmacpy_handle_copy_asm, (void *)dev);
    }

    return 0;
}


void pi_dmacpy_close(struct pi_device *device)
{
    pos_dmacpy_t *dev = (pos_dmacpy_t *)device->data;

    int channel = dev->channel;
    int periph_id = UDMA_PERIPH_ID(channel);

    dev->open_count--;

    if (dev->open_count == 0)
    {
        // Deactivate event routing
        soc_eu_fcEventMask_clearEvent(channel);

        // Reactivate clock-gating
        plp_udma_cg_set(plp_udma_cg_get() & ~(1<<periph_id));
    }
}


static inline void pos_dmacpy(pos_dmacpy_t *dev, uint32_t l2_addr, uint32_t tcdm_addr, int size, int to_tcdm)
{
    if (to_tcdm)
    {
        plp_udma_enqueue_toTcdm(l2_addr, tcdm_addr, size, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
        plp_udma_enqueue_fromTcdm(l2_addr, tcdm_addr, size, UDMA_CHANNEL_CFG_EN);
    }
}


static inline __attribute__((always_inline)) void pos_dmacpy_enqueue_pending_copy(
    pos_dmacpy_t *dev, pi_task_t *task, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    task->implem.data[0] = arg0;
    task->implem.data[1] = arg1;
    task->implem.data[2] = arg2;
    task->implem.data[3] = arg3;

    if (dev->waiting_first)
        dev->waiting_last->implem.next = task;
    else
        dev->waiting_first = task;

    dev->waiting_last = task;
    task->implem.next = NULL;
}


int pi_dmacpy_copy_async(struct pi_device *device, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task)
{
    pos_dmacpy_t *dev = (pos_dmacpy_t *)device->data;
    uint32_t l2_addr;
    uint32_t tcdm_addr;
    int to_tcdm;

    if (dir == PI_DMACPY_FC_L1_L2)
    {
        l2_addr = (uint32_t)dst;
        tcdm_addr = (uint32_t)src;
        to_tcdm = 0;
    }
    else
    {
        l2_addr = (uint32_t)src;
        tcdm_addr = (uint32_t)dst;
        to_tcdm = 1;
    }
    
    int irq = hal_irq_disable();

    printf("%s %d\n", __FILE__, __LINE__);

    if (likely(!dev->pending_copy))
    {
    printf("%s %d\n", __FILE__, __LINE__);

        dev->pending_copy = task;

        hal_irq_restore(irq);

        pos_dmacpy(dev, l2_addr, tcdm_addr, size, to_tcdm);

        return 0;
    }
   
    pos_dmacpy_enqueue_pending_copy(dev, task, l2_addr, tcdm_addr, size, to_tcdm);
 
    hal_irq_restore(irq);

    return 0;
}


int pi_dmacpy_copy(struct pi_device *device, void *src, void *dst,
                   uint32_t size, pi_dmacpy_dir_e dir)
{
    pi_task_t task;
    pi_dmacpy_copy_async(device, src, dst, size, dir, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static void __attribute__((constructor)) pos_dmacpy_init()
{
    for (int i=0; i<ARCHI_UDMA_NB_TCDM; i++)
    {
        pos_dmacpy_t *dev = &pos_dmacpy_dev[i];
        dev->open_count = 0;
    }
}
