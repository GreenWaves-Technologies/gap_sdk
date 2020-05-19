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
 *          Sivarajah Sivapiriyan, GreenWaves Technologies (sivarajah.sivapiriyan@greenwaves-technologies.com)
 */

#include "pmsis.h"

#define UDMA_MEMCPY_MAX_SIZE 0x1FFFF

typedef struct
{
    pi_task_t *pending_copy;
    pi_task_t *waiting_first;
    pi_task_t *waiting_last;
    int open_count;
    int channel;
} pos_dmacpy_t;


static pos_dmacpy_t pos_dmacpy_dev[ARCHI_UDMA_NB_TCDM];

static uint32_t max_size = (uint32_t) UDMA_MEMCPY_MAX_SIZE - 4;

/*
 * Dmacpy IRQ handler, written in ASM.
 * This function will call pi_uart_handle_copy(), with event num and pos_dmacpy_t arg.
 */
extern void pos_dmacpy_handle_copy_asm();
void pos_dmacpy_handle_copy(int event, void *arg);

static inline void pos_dmacpy(pos_dmacpy_t *dev, struct pi_task *task);

void pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf)
{
    conf->id = 0;
}


void pos_dmacpy_handle_copy(int event, void *arg)
{
    pos_dmacpy_t *dev = (pos_dmacpy_t *)arg;
    int channel = event & 0x1;

    pi_task_t *task = dev->pending_copy;
    /* Pending data on current transfer. */
    if (task->implem.data[5] && channel)
    {
        return;
    }
    if (task->implem.data[4] != 0)
    {
        pos_dmacpy(dev, task);
    }
    else
    {
        dev->pending_copy = NULL;
        __rt_event_handle_end_of_task(task);
        task = dev->waiting_first;
        if (task)
        {
            dev->waiting_first = task->implem.next;
            dev->pending_copy = task;
            pos_dmacpy(dev, task);
        }
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


static inline void pos_dmacpy(pos_dmacpy_t *dev, struct pi_task *task)
{
    uint32_t l2_buf_0 = task->implem.data[0]; /* RX_SADDR for FC->L2 or L2->L2. */
    uint32_t l2_buf_1 = task->implem.data[1]; /* TX_SADDR for L2->FC or L2->L2. */
    uint32_t cpy_src  = task->implem.data[2]; /* SRC for FC->L2. */
    uint32_t cpy_dst  = task->implem.data[3]; /* DST for L2->FC. */
    int cpy_size      = task->implem.data[4]; /* Size. */
    int tcdm_cpy      = task->implem.data[5]; /* Copy from/to TCDM? */

    if (task->implem.data[4] > max_size)
    {
        cpy_size = max_size;
    }
    task->implem.data[4] -= cpy_size;

    /* Actually copy from L2 to L2. */
    if (tcdm_cpy)
    {
        task->implem.data[0] += cpy_size;
        task->implem.data[1] += cpy_size;
        plp_udma_enqueue_fromTcdm(l2_buf_0, cpy_src, tcdm_cpy, cpy_size, UDMA_CHANNEL_CFG_EN);
        plp_udma_enqueue_toTcdm(l2_buf_1, cpy_dst, tcdm_cpy, cpy_size, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
        if (cpy_src)
        {
            task->implem.data[0] += cpy_size;
            task->implem.data[2] += cpy_size;
            plp_udma_enqueue_fromTcdm(l2_buf_0, cpy_src, tcdm_cpy, cpy_size, UDMA_CHANNEL_CFG_EN);
        }
        else
        {
            task->implem.data[1] += cpy_size;
            task->implem.data[3] += cpy_size;
            plp_udma_enqueue_toTcdm(l2_buf_1, cpy_dst, tcdm_cpy, cpy_size, UDMA_CHANNEL_CFG_EN);
        }
    }
}


static inline __attribute__((always_inline)) void pos_dmacpy_enqueue_pending_copy(
    pos_dmacpy_t *dev, pi_task_t *task)
{
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
    /* Src address not aligned on 4 bytes. */
    if (((uint32_t) src % 4) != 0)
    {
        return -11;
    }
    /* Dst address not aligned on 4 bytes. */
    if (((uint32_t) dst % 4) != 0)
    {
        return -12;
    }
    /* Size not a multiple of 4 bytes. */
    if (((uint32_t) size % 4) != 0)
    {
        return -13;
    }

    task->implem.data[0] = 0;
    task->implem.data[1] = 0;
    task->implem.data[2] = (uint32_t) src;
    task->implem.data[3] = (uint32_t) dst;
    task->implem.data[4] = size;
    task->implem.data[5] = 0;

    switch (dir)
    {
    case PI_DMACPY_FC_L1_L2 :
        task->implem.data[0] = (uint32_t) dst;
        task->implem.data[3] = 0;
        break;

    case PI_DMACPY_L2_FC_L1 :
        task->implem.data[1] = (uint32_t) src;
        task->implem.data[2] = 0;
        break;

    case PI_DMACPY_L2_L2 :
        task->implem.data[0] = (uint32_t) dst;
        task->implem.data[1] = (uint32_t) src;
        task->implem.data[2] = 0;
        task->implem.data[3] = 0;
        task->implem.data[5] = 1;
        break;

    default :
        printf("Wrong param for direction !\n");
        return -14;
    }

    int irq = hal_irq_disable();
    pos_dmacpy_t *dev = (pos_dmacpy_t *)device->data;

    if (likely(!dev->pending_copy))
    {
        dev->pending_copy = task;

        hal_irq_restore(irq);

        pos_dmacpy(dev, task);

        return 0;
    }

    pos_dmacpy_enqueue_pending_copy(dev, task);

    hal_irq_restore(irq);

    return 0;
}


int pi_dmacpy_copy(struct pi_device *device, void *src, void *dst,
                   uint32_t size, pi_dmacpy_dir_e dir)
{
    int status = 0;
    pi_task_t task;
    status = pi_dmacpy_copy_async(device, src, dst, size, dir, pi_task_block(&task));
    if (status == 0)
    {
        pi_task_wait_on(&task);
    }
    return status;
}


static void __attribute__((constructor)) pos_dmacpy_init()
{
    for (int i=0; i<ARCHI_UDMA_NB_TCDM; i++)
    {
        pos_dmacpy_t *dev = &pos_dmacpy_dev[i];
        dev->open_count = 0;
    }
}
