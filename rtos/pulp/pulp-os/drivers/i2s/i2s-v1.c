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

extern void __pos_i2s_handle_copy_asm();


static int __pos_i2s_global_open_count;

static pos_i2s_t __pos_i2s[ARCHI_UDMA_NB_I2S];


void pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    conf->itf = 0;
    conf->frame_clk_freq = 44100;
    conf->pdm_decimation_log2 = 8;
    conf->format = PI_I2S_FMT_DATA_FORMAT_PDM;
    conf->options = 0;
    conf->word_size = 16;
    conf->channels = 1;
    conf->pingpong_buffers[0] = NULL;
    conf->pingpong_buffers[1] = NULL;
}


void __pos_i2s_handle_copy(pos_i2s_t *i2s)
{
    if (i2s->reenqueue)
    {
        unsigned int base = hal_udma_channel_base(i2s->channel);
        int buffer = i2s->current_buffer;
        plp_udma_enqueue(base, (int)i2s->conf.pingpong_buffers[buffer], i2s->conf.block_size, UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_16);
        i2s->current_buffer = buffer ^ 1;
    }

    pi_task_t *waiting = i2s->waiting_first;

    if (waiting)
    {
        i2s->waiting_first = waiting->implem.next;
        __rt_event_enqueue(waiting);
    }
    else
    {
        i2s->nb_ready_buffer++;
    }
}


int pi_i2s_open(struct pi_device *device)
{
    int irq = rt_irq_disable();

    struct pi_i2s_conf *conf = (struct pi_i2s_conf *)device->config;
    int itf_id = conf->itf;
    pos_i2s_t *i2s = &__pos_i2s[itf_id];
    int periph_id = ARCHI_UDMA_I2S_ID(itf_id >> 1);

    if (conf->pingpong_buffers[0] == NULL || conf->pingpong_buffers[1] == NULL)
        return -1;

    device->data = (void *)i2s;

    memcpy(&i2s->conf, conf, sizeof(struct pi_i2s_conf));

    __pos_i2s_global_open_count++;
    if (__pos_i2s_global_open_count == 1)
    {
        // Deactivate i2s clock-gating
        plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));        
    }

    i2s->open_count++;
    if (i2s->open_count == 1)
    {
        int sub_periph_id = itf_id & 1;
        int channel_id = UDMA_EVENT_ID(periph_id) + sub_periph_id;
        int pdm = (i2s->conf.format & PI_I2S_FMT_DATA_FORMAT_MASK) == PI_I2S_FMT_DATA_FORMAT_PDM;

        i2s->channel = channel_id;
        i2s->reenqueue = 0;
        if (conf->word_size == 16)
            i2s->udma_cfg = UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_16;
        else
            i2s->udma_cfg = UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_32;

        i2s->i2s_freq = i2s->conf.frame_clk_freq;


        // Activate routing of UDMA i2s soc events to FC to trigger interrupts
        soc_eu_fcEventMask_setEvent(channel_id);

        // Redirect all UDMA i2s events to a specific callback
        __rt_udma_register_channel_callback(channel_id, __pos_i2s_handle_copy_asm, (void *)i2s);

        i2s->clk = sub_periph_id;

        unsigned int mode = hal_i2s_chmode_get(itf_id);
        int clk = i2s->clk;

        mode &= ~(I2S_CHMODE_CH_SNAPCAM_MASK(clk) | I2S_CHMODE_CH_LSBFIRST_MASK(clk) | 
        I2S_CHMODE_CH_PDM_USEFILTER_MASK(clk) | I2S_CHMODE_CH_PDM_EN_MASK(clk) | 
        I2S_CHMODE_CH_USEDDR_MASK(clk) | I2S_CHMODE_CH_MODE_MASK(clk));

        if (pdm)
        {
            i2s->i2s_freq *= (1<<conf->pdm_decimation_log2);

            int shift = 10 - i2s->conf.pdm_decimation_log2;
            if (shift > 7) shift = 7;

            hal_i2s_filt_ch_set(i2s->conf.itf, i2s->clk, (shift << 16) | ((1<<i2s->conf.pdm_decimation_log2)-1));

            mode |= I2S_CHMODE_CH_PDM_USEFILTER_ENA(clk) | I2S_CHMODE_CH_PDM_EN_ENA(clk);

            if (conf->channels == 2)
            {
                mode |= I2S_CHMODE_CH_USEDDR_ENA(clk);
            }

        }
        else
        {
            i2s->i2s_freq *= conf->word_size;

            if (conf->channels == 2)
                i2s->i2s_freq *= 2;
        }

        mode |= I2S_CHMODE_CH_LSBFIRST_DIS(clk);

        // Configure each channel on a different clock generator to avoid activating a channel
        // when the other is first activated
        mode |= I2S_CHMODE_CH_MODE_CLK(0,0) | I2S_CHMODE_CH_MODE_CLK(1,1);

        hal_i2s_chmode_set(itf_id, mode);
    }


    rt_irq_restore(irq);

    return 0;
}


static inline void __pos_i2s_suspend(pos_i2s_t *i2s)
{
    unsigned int base = hal_udma_channel_base(i2s->channel);

    i2s->reenqueue = 0;

    // Stop the clock now
    hal_i2s_cfg_clkgen_set(i2s->conf.itf, i2s->clk, 0);

    // And wait a bit so that the pending samples within UDMA internal FIFOs
    // are moved to the buffers.
    // Also reactivate the interrupts in case these pending samples trigger an
    // end of transfer
    rt_irq_enable();

    volatile int i;
    for (i=0; i<20; i++);

    // Then clear the channels so that we start from a clean state the next time.
    rt_irq_disable();
    plp_udma_clr(base);
    i2s->nb_ready_buffer = 0;
}


void pi_i2s_close(struct pi_device *device)
{
    pos_i2s_t *i2s = (pos_i2s_t *)device->data;

    int irq = rt_irq_disable();

    i2s->open_count--;
    if (i2s->open_count == 0)
    {
        // Deactivate event routing
        soc_eu_fcEventMask_clearEvent(i2s->channel);
    }

    __pos_i2s_global_open_count--;
    if (__pos_i2s_global_open_count == 0)
    {
        int periph_id = ARCHI_UDMA_I2S_ID(i2s->conf.itf >> 1);

        // Reactivate clock-gating
        plp_udma_cg_set(plp_udma_cg_get() & ~(1<<periph_id));
    }


    rt_irq_restore(irq);
}


static inline void __pos_i2s_resume(pos_i2s_t *i2s)
{
    int periph_freq = __rt_freq_periph_get();
    int div = periph_freq / i2s->i2s_freq;

    unsigned int base = hal_udma_channel_base(i2s->channel);

    rt_trace(RT_TRACE_CAM, "[I2S] Resuming (i2s: %d, clk: %d, periph_freq: %d, i2s_freq: %d, div: %d)\n", i2s->conf.itf, itf->clk, periph_freq, itf->conf.frame_clk_freq, div);
  
    i2s->reenqueue = 1;
    i2s->current_buffer = 0;
    i2s->current_read_buffer = 0;
    i2s->nb_ready_buffer = 0;
    i2s->waiting_first = NULL;

    plp_udma_enqueue(base, (int)i2s->conf.pingpong_buffers[0], i2s->conf.block_size, i2s->udma_cfg);
    plp_udma_enqueue(base, (int)i2s->conf.pingpong_buffers[1], i2s->conf.block_size, i2s->udma_cfg);

    unsigned int conf = 
        I2S_CFG_CLKGEN_BITS_WORD(i2s->conf.word_size) | 
        I2S_CFG_CLKGEN_CLK_EN |
        I2S_CFG_CLKGEN0_CLKDIV((div-1)>>1);

    hal_i2s_cfg_clkgen_set(i2s->conf.itf, i2s->clk, conf);
}


int pi_i2s_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    pos_i2s_t *i2s = (pos_i2s_t *)device->data;

    int irq = rt_irq_disable();

    switch (cmd)
    {
        case PI_I2S_IOCTL_START:
            __pos_i2s_resume(i2s);
            break;

        case PI_I2S_IOCTL_STOP:
            __pos_i2s_suspend(i2s);
            break;

        default:
            rt_irq_restore(irq);
            return -1;
    }

    rt_irq_restore(irq);

    return 0;
}



int pi_i2s_read_async(struct pi_device *device, pi_task_t *task)
{
    pos_i2s_t *i2s = (pos_i2s_t *)device->data;

    int irq = rt_irq_disable();

    // Prepare now the task results, the IRQ handler can still overwrite
    // them if they are different.
    task->implem.data[0] = 0;
    task->implem.data[1] = (int)i2s->conf.pingpong_buffers[i2s->current_read_buffer];
    task->implem.data[2] = i2s->conf.block_size;
    i2s->current_read_buffer ^= 1;

    if (i2s->nb_ready_buffer)
    {
        i2s->nb_ready_buffer--;
        __rt_event_enqueue(task);
    }
    else
    {
        if (i2s->waiting_first)
            i2s->waiting_last->implem.next = task;
        else
            i2s->waiting_first = task;

        task->implem.next = NULL;
        i2s->waiting_last = task;
    }

    rt_irq_restore(irq);

    return 0;
}


int pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size)
{  
    if (mem_block)
        *mem_block = (void *)task->implem.data[1];
    if (size)
        *size = task->implem.data[2];

    return task->implem.data[0];
}


int pi_i2s_read(struct pi_device *dev, void **mem_block, size_t *size)
{
    pi_task_t task;
    pi_i2s_read_async(dev, pi_task_block(&task));
    pi_task_wait_on(&task);

    return pi_i2s_read_status(&task, mem_block, size);
}


int pi_i2s_write(struct pi_device *dev, void *mem_block, size_t size)
{
    return 0;
}

static void __attribute__((constructor)) __pos_i2s_init()
{
    __pos_i2s_global_open_count = 0;

    for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
    {
        __pos_i2s[i].open_count = 0;
    }
}