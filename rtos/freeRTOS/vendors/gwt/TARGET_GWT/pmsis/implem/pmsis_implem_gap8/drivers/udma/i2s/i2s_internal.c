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

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* Special flags */
static uint32_t g_i2s_flags = 0;

/* One I2S periph with two distinct devices inside. */
struct i2s_itf_data_s *g_i2s_itf_data[UDMA_NB_I2S << 1] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_i2s_handler(void *arg);
static void __pi_i2s_resume(struct i2s_itf_data_s *itf_data);
static void __pi_i2s_suspend(struct i2s_itf_data_s *itf_data);
static uint8_t __pi_i2s_enqueue(struct i2s_itf_data_s *itf_data);
static void *__pi_i2s_pop(struct i2s_itf_data_s *itf_data);
static int32_t __pi_i2s_conf_apply(struct i2s_itf_data_s *itf_data);
static int32_t __pi_i2s_conf_set(struct i2s_itf_data_s *itf_data,
                                      struct pi_i2s_conf *conf);
static void __pi_i2s_conf_get(struct i2s_itf_data_s *itf_data,
                              struct pi_i2s_conf *conf);

static void __pi_i2s_clock_enable(struct i2s_itf_data_s *itf_data);

static void __pi_i2s_clock_disable(struct i2s_itf_data_s *itf_data);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_i2s_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2S_ID(0);

    struct i2s_itf_data_s *itf_data = g_i2s_itf_data[channel];
    uint8_t task_done = 1;
    if (itf_data->reenqueue)
    {
        task_done = __pi_i2s_enqueue(itf_data);
    }
    if (task_done)
    {
        pi_task_t *task = itf_data->fifo_head;
        if (task != NULL)
        {
            if ((itf_data->options & PI_I2S_OPT_MEM_SLAB))
            {
                task->data[1] = (uintptr_t) __pi_i2s_pop(itf_data);
            }
            itf_data->fifo_head = itf_data->fifo_head->next;
            __pi_irq_handle_end_of_task(task);
        }
        else
        {
            itf_data->nb_ready_buffer++;
        }
    }
}

static void *__pi_i2s_pop(struct i2s_itf_data_s *itf_data)
{
    void *buffer = itf_data->ring_buffer[itf_data->ring_buffer_tail++];
    if (itf_data->ring_buffer_tail == itf_data->ring_buffer_nb_elem)
    {
        itf_data->ring_buffer_tail = 0;
    }
    return buffer;
}

static uint8_t __pi_i2s_enqueue(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    uint32_t buffer = 0;
    uint32_t size = 0;
    uint32_t max_size = 0;
    uint32_t size_enqueue = 0;
    uint8_t done = 1;
    if (itf_data->pending_size)
    {
        buffer = (uint32_t) itf_data->pending_buffer;
        size = itf_data->pending_size;
        done = 0;
    }
    else
    {
        size = itf_data->block_size;
        if (!(itf_data->options & PI_I2S_OPT_MEM_SLAB))
        {
            uint32_t buffer_idx = itf_data->cur_buffer;
            buffer = (uint32_t) itf_data->pingpong_buffers[buffer_idx];
            itf_data->cur_buffer = buffer_idx ^ 1;
        }
        else
        {
            pi_mem_slab_alloc(itf_data->mem_slab, (void **) &buffer, 0);
            if (buffer)
            {
                itf_data->ring_buffer[itf_data->ring_buffer_head++] = (void *) buffer;
                if (itf_data->ring_buffer_head == itf_data->ring_buffer_nb_elem)
                {
                    itf_data->ring_buffer_head = 0;
                }
            }
        }
    }

    if (buffer)
    {
        /* Max size is 64kB on GAP8 UDMA. */
        max_size = (1 << 16) - 4;

        if (size > max_size)
        {
            size_enqueue = max_size;
            itf_data->pending_size = size - max_size;
            itf_data->pending_buffer = (void *) (buffer + max_size);
        }
        else
        {
            size_enqueue = size;
            itf_data->pending_size = 0;
        }
        /* Enqueue first in HW fifo. */
        hal_i2s_enqueue(device_id, itf_data->i2s_id, buffer, size_enqueue, itf_data->udma_cfg);
    }
    return done;
}

static inline uint32_t __pi_i2s_clk_div(uint32_t i2s_freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_FC);
    uint32_t clk_div = 0;
    clk_div = ((periph_freq / i2s_freq) >> 1) - 1;
    I2S_TRACE("Periph_freq=%ld i2s_freq=%ld, clk_div=%ld\n",
              periph_freq, i2s_freq, clk_div);
    if (clk_div > 0xFFFF)
    {
        I2S_TRACE_ERR("Clock divider too big, SoC frequency is too high.\n");
    }
    return clk_div;
}

static void __pi_i2s_resume(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;

    I2S_TRACE("I2S(%d) Resuming: clk_use: %x, word_size: %x, format: %x, "
              "options: %x, block_size: %ld, UDMA_CFG: %lx\n",
              itf_data->i2s_id, itf_data->word_size, itf_data->format,
              itf_data->options, itf_data->block_size, itf_data->udma_cfg);

    itf_data->reenqueue = 1;
    itf_data->cur_buffer = 0;
    itf_data->cur_read_buffer = 0;
    itf_data->nb_ready_buffer = 0;
    itf_data->fifo_head = NULL;
    itf_data->fifo_tail = NULL;

    __pi_i2s_enqueue(itf_data);
    __pi_i2s_enqueue(itf_data);

    /* Enable clock. */
    __pi_i2s_clock_enable(itf_data);

    I2S_TRACE("I2S: ext %x, clkgen0 %x, clkgen1 %x, mode %x, filt0 %x, filt1 %x\n",
              i2s(device_id)->ext_clk, i2s(device_id)->cfg_clkgen0, i2s(device_id)->cfg_clkgen1,
              i2s(device_id)->chmode, i2s(device_id)->filt_ch0, i2s(device_id)->filt_ch1);
}

static void __pi_i2s_suspend(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    itf_data->reenqueue = 0;

    /* Disable clock. */
    __pi_i2s_clock_disable(itf_data);

    /* Wait end of pending samples. */
    for (volatile uint32_t i=0; i<100; i++);

    /* Clear channels. */
    hal_i2s_clear(device_id, itf_data->i2s_id);
    itf_data->nb_ready_buffer = 0;
}

static void __pi_i2s_clock_enable(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    /* Enable clock to acquire data. Enable only if clocks are internally generated. */
    /* Case i2s clock source is external but WS is generated internally. */
    if ((itf_data->clk & PI_I2S_OPT_EXT_CLK) && ((itf_data->clk & PI_I2S_OPT_EXT_WS) == 0))
    {
        hal_i2s_ext_clk_set(device_id, (itf_data->word_size - 1));
        I2S_TRACE("Enabling external clock, WS generated internally.\n");
    }
    /* Case i2s clock source and WS are generated internally from clkgen_X. */
    if ((itf_data->clk & PI_I2S_OPT_EXT_CLK) == 0)
    {
        uint16_t clk_div = __pi_i2s_clk_div(itf_data->frequency);
        I2S_TRACE("I2S(%d) clk_div = %d\n", device_id, clk_div);
        switch (itf_data->clk & 0x1)
        {
        case 0 :
            hal_i2s_cfg_clkgen0_set(device_id, (itf_data->word_size - 1), 1, clk_div);
            I2S_TRACE("Enabling clock generated by clk_gen_0.\n");
            break;

        case 1 :
            hal_i2s_cfg_clkgen1_set(device_id, (itf_data->word_size - 1), 1, clk_div);
            I2S_TRACE("Enabling clock generated by clk_gen_1.\n");
            break;

        default :
            break;
        }
    }
}

static void __pi_i2s_clock_disable(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    /* Disable clocks if they are internal. */
    if ((itf_data->clk & PI_I2S_OPT_EXT_CLK) == 0)
    {
        switch (itf_data->clk & 0x1)
        {
        case 0 :
            hal_i2s_cfg_clkgen0_set(device_id, 0, 0, 0);
            I2S_TRACE("Disabling clock generated by clk_gen_0.\n");
            break;

        case 1 :
            hal_i2s_cfg_clkgen1_set(device_id, 0, 0, 0);
            I2S_TRACE("Disabling clock generated by clk_gen_1.\n");
            break;

        default :
            break;
        }
    }
}

static int32_t __pi_i2s_conf_apply(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    /* Use pingpong buffers. */
    if (!(itf_data->options & PI_I2S_OPT_MEM_SLAB))
    {
        if ((itf_data->pingpong_buffers[0] == NULL) ||
            (itf_data->pingpong_buffers[1] == NULL))
        {
            I2S_TRACE_ERR("Error : one or both pingpong buffers NULL !\n");
            return -13;
        }
    }
    /* Use slab. */
    else
    {
        itf_data->ring_buffer_nb_elem = itf_data->mem_slab->num_blocks;
        itf_data->ring_buffer_head = 0;
        itf_data->ring_buffer_tail = 0;
        itf_data->ring_buffer = pi_fc_l1_malloc(itf_data->ring_buffer_nb_elem * sizeof(void *));
        if (itf_data->ring_buffer == NULL)
        {
            I2S_TRACE_ERR("Error : mem_slab NULL !\n");
            return -14;
        }
    }

    uint8_t pdm = ((itf_data->format & PI_I2S_FMT_DATA_FORMAT_PDM) != 0);
    uint8_t pdm_filter_ena = (itf_data->pdm_filter_ena & pdm);
    uint8_t shift = 0;
    uint16_t decimation = 0;
    uint8_t lsb = 0;
    uint8_t ddr = 0;

    /*
     * Use internal clock for both i2s, clkgen0. On gapuino_v2, alternate pads
     * are used, so both i2s_0 and i2s_1 use i2s_0 internal clock.
     */
    itf_data->clk = (g_i2s_flags & PI_I2S_SETUP_SINGLE_CLOCK) ? 0 : itf_data->i2s_id;
    if ((g_i2s_flags & PI_I2S_SETUP_SINGLE_CLOCK) && (itf_data->options & PI_I2S_OPT_EXT_CLK))
    {
        I2S_TRACE_ERR("Error clock configuration : Single internal clock and "
                      "external clock are defined !\n");
        return -15;
    }
    /* Use INT_CLK. */
    uint8_t clk_mode = itf_data->clk;
    /* External clock source ? */
    uint8_t clk = (itf_data->options & PI_I2S_OPT_EXT_CLK);
    if (clk)
    {
        /* External WS clock ? */
        clk |= (itf_data->options & PI_I2S_OPT_EXT_WS);
        /* Use EXT_CLK. */
        /* Shift made to have EXT_CLK|EXT_WS. */
        clk_mode = (((clk & PI_I2S_OPT_EXT_CLK) >> (PI_I2S_OPT_CLK_SRC_SHIFT - 1)) |
                    ((clk & PI_I2S_OPT_EXT_WS) >> PI_I2S_OPT_WS_SRC_SHIFT));
    }
    /* | EXT_WS | EXT_CLK |...| CLKGEN | */
    itf_data->clk |= clk;
    if (pdm)
    {
        ddr = (itf_data->channels >> 1);
        /* PDM filter setup. */
        itf_data->frequency *= itf_data->pdm_decimation;
        if (itf_data->pdm_shift == -1)
        {
            shift = 4;
        }
        else
        {
            shift = itf_data->pdm_shift;
        }
        decimation = itf_data->pdm_decimation - 1;
    }
    else
    {
        itf_data->frequency *= itf_data->word_size * itf_data->channels;
    }
    if (itf_data->i2s_id)
    {
        /* Filter setup. */
        hal_i2s_filt_ch1_set(device_id, decimation, shift);
        /* Channel mode setup. */
        hal_i2s_chmode_ch1_set(device_id, lsb, pdm_filter_ena, pdm, ddr, clk_mode);
    }
    else
    {
        /* Filter setup. */
        hal_i2s_filt_ch0_set(device_id, decimation, shift);
        /* Channel mode setup. */
        hal_i2s_chmode_ch0_set(device_id, lsb, pdm_filter_ena, pdm, ddr, clk_mode);
    }
    return 0;
}

static int32_t __pi_i2s_conf_set(struct i2s_itf_data_s *itf_data,
                                 struct pi_i2s_conf *conf)
{
    itf_data->i2s_id              = conf->itf;
    itf_data->channels            = conf->channels;
    itf_data->word_size           = conf->word_size;
    itf_data->frame_clk_freq      = conf->frame_clk_freq;
    itf_data->format              = conf->format;
    itf_data->options             = conf->options;
    itf_data->block_size          = conf->block_size;
    itf_data->pdm_decimation      = conf->pdm_decimation;
    itf_data->pdm_shift           = conf->pdm_shift;
    itf_data->pdm_filter_ena      = conf->pdm_filter_ena;
    itf_data->pingpong_buffers[0] = conf->pingpong_buffers[0];
    itf_data->pingpong_buffers[1] = conf->pingpong_buffers[1];
    itf_data->mem_slab            = conf->mem_slab;
    return 0;
}

static void __pi_i2s_conf_get(struct i2s_itf_data_s *itf_data,
                              struct pi_i2s_conf *conf)
{
    conf->itf                 = itf_data->i2s_id;
    conf->channels            = itf_data->channels;
    conf->word_size           = itf_data->word_size;
    conf->frame_clk_freq      = itf_data->frame_clk_freq;
    conf->format              = itf_data->format;
    conf->options             = itf_data->options;
    conf->block_size          = itf_data->block_size;
    conf->pdm_decimation      = itf_data->pdm_decimation;
    conf->pdm_shift           = itf_data->pdm_shift;
    conf->pdm_filter_ena      = itf_data->pdm_filter_ena;
    conf->pingpong_buffers[0] = itf_data->pingpong_buffers[0];
    conf->pingpong_buffers[1] = itf_data->pingpong_buffers[1];
    conf->mem_slab            = itf_data->mem_slab;
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    conf->itf                 = 0;
    conf->channels            = 1;
    conf->word_size           = 16;
    conf->frame_clk_freq      = 44100;
    conf->format              = PI_I2S_FMT_DATA_FORMAT_PDM;
    conf->options             = PI_I2S_OPT_PINGPONG;
    conf->block_size          = 0;
    conf->pingpong_buffers[0] = NULL;
    conf->pingpong_buffers[1] = NULL;
    conf->mem_slab            = NULL;
    conf->pdm_decimation      = 64;
    conf->pdm_shift           = -1;
    conf->pdm_filter_ena      = 1;
}

int32_t __pi_i2s_open(struct pi_i2s_conf *conf, struct i2s_itf_data_s **device_data)
{
    uint32_t device_id = 0;
    struct i2s_itf_data_s *itf_data = g_i2s_itf_data[conf->itf];
    if (itf_data == NULL)
    {
        /* Allocate driver data. */
        itf_data = (struct i2s_itf_data_s *) pi_l2_malloc(sizeof(struct i2s_itf_data_s));
        if (itf_data == NULL)
        {
            I2S_TRACE_ERR("Driver itf_data alloc failed !\n");
            return -11;
        }
        g_i2s_itf_data[conf->itf] = itf_data;

        itf_data->nb_open             = 1;
        itf_data->fifo_head           = NULL;
        itf_data->fifo_tail           = NULL;
        itf_data->device_id           = device_id;
        itf_data->i2s_id              = conf->itf;
        itf_data->channels            = conf->channels;
        itf_data->word_size           = conf->word_size;
        itf_data->frame_clk_freq      = conf->frame_clk_freq;
        itf_data->format              = conf->format;
        itf_data->options             = conf->options;
        itf_data->block_size          = conf->block_size;
        itf_data->pdm_decimation      = conf->pdm_decimation;
        itf_data->pdm_shift           = conf->pdm_shift;
        itf_data->pdm_filter_ena      = conf->pdm_filter_ena;
        itf_data->pingpong_buffers[0] = conf->pingpong_buffers[0];
        itf_data->pingpong_buffers[1] = conf->pingpong_buffers[1];;
        itf_data->mem_slab            = conf->mem_slab;
        itf_data->frequency           = itf_data->frame_clk_freq;
        itf_data->udma_cfg            = UDMA_CORE_RX_CFG_EN(1);
        itf_data->udma_cfg           |= UDMA_CORE_RX_CFG_DATASIZE(conf->word_size >> 4);
        itf_data->clk                 = 0;
        itf_data->cur_read_buffer     = 0;
        itf_data->cur_buffer          = 0;
        itf_data->nb_ready_buffer     = 0;
        itf_data->reenqueue           = 0;
        itf_data->pending_size        = 0;

        /* Set handlers. */
        /* Enable SOC events propagation to FC. */
        switch (itf_data->i2s_id)
        {
        case 0 :
            pi_fc_event_handler_set(SOC_EVENT_UDMA_I2S_RX(itf_data->device_id), __pi_i2s_handler);
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2S_RX(itf_data->device_id));
            break;
        case 1 :
            pi_fc_event_handler_set(SOC_EVENT_UDMA_I2S_TX(itf_data->device_id), __pi_i2s_handler);
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2S_TX(itf_data->device_id));
            break;
        default :
            return -12;
        }

        /* Disable UDMA CG. */
        udma_init_device(UDMA_I2S_ID(device_id));

        int32_t status = __pi_i2s_conf_apply(itf_data);
        if (status)
        {
            I2S_TRACE_ERR("Error applying conf : %ld\n", status);
            return status;
        }
    }
    else
    {
        /* Increment number of devices opened. */
        itf_data->nb_open++;
        I2S_TRACE("Device id=%d already opened, now open=%d\n",
                  itf_data->device_id, itf_data->nb_open);
    }
    *device_data = g_i2s_itf_data[conf->itf];
    return 0;
}

void __pi_i2s_close(struct i2s_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    /* Decrement number of devices opened. */
    itf_data->nb_open--;
    /* Free device and structure opened. */
    if (itf_data->nb_open == 0)
    {
        /* Clear handlers. */
        /* Disable SOC events propagation to FC. */
        switch (itf_data->i2s_id)
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
        uint8_t i2s_itf = !(itf_data->i2s_id);
        if (g_i2s_itf_data[i2s_itf] == NULL)
        {
            udma_deinit_device(UDMA_I2S_ID(device_id));
        }

        /* Free allocated driver data. */
        pi_l2_free(itf_data, sizeof(struct i2s_itf_data_s));
        g_i2s_itf_data[itf_data->i2s_id] = NULL;
    }
}

int32_t __pi_i2s_ioctl(struct i2s_itf_data_s *itf_data, uint32_t cmd, void *arg)
{
    uint32_t irq = disable_irq();
    I2S_TRACE("i2s(%ld) ioctl command=%ld, arg=%lx\n", itf_data->i2s_id, cmd, arg);
    switch (cmd)
    {
    case PI_I2S_IOCTL_START :
        __pi_i2s_resume(itf_data);
        break;

    case PI_I2S_IOCTL_STOP :
        __pi_i2s_suspend(itf_data);
        break;

    case PI_I2S_IOCTL_CONF_SET :
        __pi_i2s_conf_set(itf_data, (struct pi_i2s_conf *) arg);
        break;

    case PI_I2S_IOCTL_CONF_GET :
        __pi_i2s_conf_get(itf_data, (struct pi_i2s_conf *) arg);
        break;

    case PI_I2S_IOCTL_CLOCK_ENABLE :
        __pi_i2s_clock_enable(itf_data);
        break;

    case PI_I2S_IOCTL_CLOCK_DISABLE :
        __pi_i2s_clock_disable(itf_data);
        break;

    default :
        restore_irq(irq);
        return -1;
    }
    restore_irq(irq);
    return 0;
}

int32_t __pi_i2s_read_async(struct i2s_itf_data_s *itf_data, pi_task_t *task)
{
    uint32_t irq = disable_irq();

    /* Fill rest of arguments for i2s driver. */
    task->data[0] = 0;
    if (!(itf_data->options & PI_I2S_OPT_MEM_SLAB))
    {
        task->data[1] = (uint32_t) itf_data->pingpong_buffers[itf_data->cur_read_buffer];
    }
    task->data[2] = itf_data->block_size;
    task->next = NULL;
    itf_data->cur_read_buffer ^= 1;

    if (itf_data->nb_ready_buffer)
    {
        if ((itf_data->options & PI_I2S_OPT_MEM_SLAB))
        {
            task->data[1] = (uintptr_t) __pi_i2s_pop(itf_data);
        }
        itf_data->nb_ready_buffer--;
        __pi_irq_handle_end_of_task(task);
    }
    else
    {
        if (itf_data->fifo_head != NULL)
        {
            itf_data->fifo_tail->next = task;
        }
        else
        {
            itf_data->fifo_head = task;
        }
        itf_data->fifo_tail = task;
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

void __pi_i2s_setup(uint32_t flags)
{
    g_i2s_flags = flags;
}
