/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

//#define DEBUG_PRINTF printf

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* One I2S periph with two distinct devices inside. */
struct i2s_driver_fifo_s *__global_i2s_driver_fifo[UDMA_NB_I2S << 1];

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_i2s_handle_end_of_task(struct pi_task *task);
static void __pi_i2s_handler(void *arg);
static void __pi_i2s_resume(uint8_t i2s_id);
static void __pi_i2s_suspend(uint8_t i2s_id);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_i2s_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    }
}

static void __pi_i2s_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2S_ID(0);

    struct i2s_driver_fifo_s *fifo = __global_i2s_driver_fifo[channel];
    if (fifo->reenqueue)
    {
        uint8_t cur_buffer = fifo->cur_buffer;
        fifo->cur_buffer = cur_buffer ^ 1;
        hal_i2s_enqueue(periph_id, channel, (uint32_t) fifo->pingpong_buffers[cur_buffer],
                        fifo->block_size, fifo->udma_cfg);
    }
    pi_task_t *task = fifo->fifo_head;
    if (task != NULL)
    {
        fifo->fifo_head = fifo->fifo_head->next;
        __pi_i2s_handle_end_of_task(task);
    }
    else
    {
        fifo->nb_ready_buffer++;
    }
}

static void __pi_i2s_resume(uint8_t i2s_id)
{
    uint32_t device_id = 0;
    struct i2s_driver_fifo_s *fifo = __global_i2s_driver_fifo[i2s_id];
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_FC);
    uint32_t clk_div = periph_freq / fifo->frequency;

    DEBUG_PRINTF("I2S(%d) Resuming: i2s_freq: %x, clk_div: %x, clk: %x, "
                 "word_size: %x, format: %x, options: %x, block_size: %x, "
                 "UDMA_CFG: %x\n",
                 i2s_id, fifo->frequency, clk_div, fifo->clk, fifo->word_size,
                 fifo->format, fifo->options, fifo->block_size, fifo->udma_cfg);

    fifo->reenqueue = 1;
    fifo->cur_buffer = 0;
    fifo->cur_read_buffer = 0;
    fifo->nb_ready_buffer = 0;
    fifo->fifo_head = NULL;
    fifo->fifo_tail = NULL;

    /* Enqueue first in HW fifo. */
    hal_i2s_enqueue(device_id, i2s_id, (uint32_t) fifo->pingpong_buffers[0],
                    fifo->block_size, fifo->udma_cfg);
    hal_i2s_enqueue(device_id, i2s_id, (uint32_t) fifo->pingpong_buffers[1],
                    fifo->block_size, fifo->udma_cfg);
    /* Enable clock to acquire data. */
    switch (fifo->clk)
    {
    case 0 :
        hal_i2s_cfg_clkgen0_set(device_id, (fifo->word_size - 1), 1, ((clk_div - 1) >> 1));
        break;
    case 1 :
        hal_i2s_cfg_clkgen1_set(device_id, (fifo->word_size - 1), 1, ((clk_div - 1) >> 1));
        break;
    default :
        break;
    }
    DEBUG_PRINTF("I2S: ext %x, clkgen0 %x, clkgen1 %x, mode %x, filt0 %x, filt1 %x\n",
                 i2s(device_id)->ext_clk, i2s(device_id)->cfg_clkgen0, i2s(device_id)->cfg_clkgen1,
                 i2s(device_id)->chmode, i2s(device_id)->filt_ch0, i2s(device_id)->filt_ch1);
}

static void __pi_i2s_suspend(uint8_t i2s_id)
{
    uint32_t device_id = 0;
    struct i2s_driver_fifo_s *fifo = __global_i2s_driver_fifo[i2s_id];
    fifo->reenqueue = 0;
    switch (fifo->clk)
    {
    case 0 :
        hal_i2s_cfg_clkgen0_set(device_id, 0, 0, 0);
        break;
    case 1 :
        hal_i2s_cfg_clkgen1_set(device_id, 0, 0, 0);
        break;
    default :
        break;
    }
    /* Wait end of pending samples. */
    for (volatile uint32_t i=0; i<100; i++);

    /* Clear channels. */
    hal_i2s_clear(device_id, i2s_id);
    fifo->nb_ready_buffer = 0;

}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    conf->pdm_decimation_log2 = 8;
    conf->word_size = 16;
    conf->channels = 1;
    conf->itf = 0;
    conf->format = PI_I2S_FMT_DATA_FORMAT_SHIFT;
    conf->options = 0;
    conf->frame_clk_freq = 44100;
    conf->block_size = 0;
    conf->pingpong_buffers[0] = NULL;
    conf->pingpong_buffers[1] = NULL;
}

int32_t __pi_i2s_open(struct pi_i2s_conf *conf)
{
    uint32_t irq = disable_irq();

    uint32_t device_id = 0;
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) pi_l2_malloc(sizeof(struct i2s_driver_fifo_s));
    if (fifo == NULL)
    {
        return -11;
    }
    __global_i2s_driver_fifo[conf->itf] = fifo;
    fifo->fifo_head = NULL;
    fifo->fifo_tail = NULL;
    fifo->i2s_id = conf->itf;
    fifo->frequency = conf->frame_clk_freq;
    fifo->word_size = conf->word_size;
    fifo->channels = conf->channels;
    fifo->format = conf->format;
    fifo->options = conf->options;
    fifo->frame_clk_freq = conf->frame_clk_freq;
    fifo->block_size = conf->block_size;
    fifo->clk = conf->itf;
    fifo->pingpong_buffers[0] = conf->pingpong_buffers[0];
    fifo->pingpong_buffers[1] = conf->pingpong_buffers[1];
    fifo->cur_read_buffer = 0;
    fifo->cur_buffer = 0;
    fifo->nb_ready_buffer = 0;
    fifo->reenqueue = 0;
    fifo->udma_cfg = UDMA_CORE_RX_CFG_EN(1);
    fifo->udma_cfg |= UDMA_CORE_RX_CFG_DATASIZE(conf->word_size >> 4);

    /* Set handlers. */
    /* Enable SOC events propagation to FC. */
    switch (conf->itf)
    {
    case 0 :
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2S_RX(device_id), __pi_i2s_handler);
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2S_RX(device_id));
        break;
    case 1 :
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2S_TX(device_id), __pi_i2s_handler);
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2S_TX(device_id));
        break;
    default :
        return -12;
    }

    /* Disable UDMA CG. */
    udma_init_device(UDMA_I2S_ID(device_id));

    uint8_t pdm = (conf->format & PI_I2S_FMT_DATA_FORMAT_MASK) == PI_I2S_FMT_DATA_FORMAT_PDM;
    uint8_t shift = 0;
    uint16_t decimation = 0;
    uint8_t lsb = 0;
    uint8_t ddr = 0;
    /* Use internal clock for both(i2s0->clkgen0, i2s1->clkgen1). */
    //uint8_t ch_mode = I2S_CHMODE_CH0_MODE(0) | I2S_CHMODE_CH1_MODE(1);
    if (pdm)
    {
        fifo->frequency *= (1 << conf->pdm_decimation_log2);
        /* Filter setup. */
        shift = 10 - conf->pdm_decimation_log2;
        shift = (shift > 7) ? 7 : shift;
        decimation = (1 << conf->pdm_decimation_log2) - 1;
        ddr = (conf->channels == 2) ? 1 : 0;
    }
    else
    {
        fifo->frequency *= conf->word_size;
        if (conf->channels == 2)
        {
            fifo->frequency *= 2;
        }
    }
    if (conf->itf)
    {
        /* Filter setup. */
        hal_i2s_filt_ch1_set(device_id, decimation, shift);
        /* Channel mode setup. */
        hal_i2s_chmode_ch1_set(device_id, lsb, pdm, pdm, ddr, 0);
    }
    else
    {
        /* Filter setup. */
        hal_i2s_filt_ch0_set(device_id, decimation, shift);
        /* Channel mode setup. */
        hal_i2s_chmode_ch0_set(device_id, lsb, pdm, pdm, ddr, 0);
    }
    hal_i2s_chmode_ch0_mode_set(device_id, 0);
    hal_i2s_chmode_ch1_mode_set(device_id, 1);

    restore_irq(irq);
    return 0;
}

void __pi_i2s_close(uint8_t i2s_id)
{
    uint32_t device_id = 0;
    /* Free allocated fifo. */
    pi_l2_free(__global_i2s_driver_fifo[i2s_id], sizeof(struct i2s_driver_fifo_s));
    __global_i2s_driver_fifo[i2s_id] = NULL;

    /* Clear handlers. */
    /* Disable SOC events propagation to FC. */
    switch (i2s_id)
    {
    case 0 :
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2S_RX(device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2S_RX(device_id));
        break;
    case 1 :
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2S_TX(device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2S_TX(device_id));
        break;
    default :
        return;
    }

    /* Enable UDMA CG. */
    /* Disable only if both I2S are not used. */
    uint8_t i2s_itf = !i2s_id;
    if (__global_i2s_driver_fifo[i2s_itf] == NULL)
    {
        udma_deinit_device(UDMA_I2S_ID(device_id));
    }
}

int32_t __pi_i2s_ioctl(uint8_t i2s_id, uint32_t cmd, void *arg)
{
    uint32_t irq = disable_irq();
    switch (cmd)
    {
    case PI_I2S_IOCTL_START :
        __pi_i2s_resume(i2s_id);
        break;
    case PI_I2S_IOCTL_STOP :
        __pi_i2s_suspend(i2s_id);
        break;
    default :
        restore_irq(irq);
        return -1;
    }
    restore_irq(irq);
    return 0;
}

int32_t __pi_i2s_read_async(uint8_t i2s_id, pi_task_t *task)
{
    uint32_t irq = disable_irq();
    struct i2s_driver_fifo_s *fifo = __global_i2s_driver_fifo[i2s_id];
    task->data[0] = 0;
    task->data[1] = (uint32_t) fifo->pingpong_buffers[fifo->cur_read_buffer];
    task->data[2] = fifo->block_size;
    task->next = NULL;
    fifo->cur_read_buffer ^= 1;

    if (fifo->nb_ready_buffer)
    {
        fifo->nb_ready_buffer--;
        __pi_i2s_handle_end_of_task(task);
    }
    else
    {
        if (fifo->fifo_head != NULL)
        {
            fifo->fifo_tail->next = task;
        }
        else
        {
            fifo->fifo_head = task;
        }
        fifo->fifo_tail = task;
    }
    restore_irq(irq);
    return 0;
}

int32_t __pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size)
{
    if (mem_block != NULL)
    {
        *mem_block = (void *) task->data[1];
    }
    if (size)
    {
        *size = task->data[2];
    }
    return task->data[0];
}

int32_t __pi_i2s_write(uint8_t i2s_id, void *mem_block, size_t size)
{
    /* Not used in GAP8. */
    return 0;
}
