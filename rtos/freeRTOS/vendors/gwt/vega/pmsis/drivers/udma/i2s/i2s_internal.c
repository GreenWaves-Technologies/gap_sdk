/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#define I2S_DOUBLE_BUFFER 1

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* One I2S periph with two distinct devices inside. */
static struct i2s_itf_data_s *g_i2s_itf_data[UDMA_NB_I2S << 1] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_i2s_handler_tx(void *arg);
static void __pi_i2s_handler_rx(void *arg);
static void __pi_i2s_resume(struct i2s_itf_data_s *itf_data);
static void __pi_i2s_suspend(struct i2s_itf_data_s *itf_data);
static uint8_t __pi_i2s_enqueue(struct i2s_itf_data_s *itf_data);
static void *__pi_i2s_pop(struct i2s_itf_data_s *itf_data);
static void __pi_i2s_copy_exec(struct i2s_itf_data_s *itf_data,
                               struct pi_task *task, uint8_t hw_buff_idx);
static int32_t __pi_i2s_conf_apply(struct i2s_itf_data_s *itf_data);
static int32_t __pi_i2s_conf_set(struct i2s_itf_data_s *itf_data,
                                 struct pi_i2s_conf *conf);
static void __pi_i2s_clock_enable(struct i2s_itf_data_s *itf_data);
static void __pi_i2s_clock_disable(struct i2s_itf_data_s *itf_data);


static void __pi_ring_buffer_head_pointer_update(pi_ring_buff_t *rb)
{
    rb->head = (rb->head + 1) % rb->nb_elem;
    if (rb->tail == rb->head)
    {
        rb->tail = (rb->tail + 1) % rb->nb_elem;
    }
}

static void __pi_ring_buffer_tail_pointer_update(pi_ring_buff_t *rb)
{
    rb->tail = (rb->tail + 1) % rb->nb_elem;
}

static void *__pi_ring_buffer_buffer_get(pi_ring_buff_t *rb)
{
    uint8_t *buffer = rb->buffer[rb->tail];
    __pi_ring_buffer_tail_pointer_update(rb);
    return buffer;
}

static void *__pi_ring_buffer_buffer_put(pi_ring_buff_t *rb)
{
    uint8_t *buffer = rb->buffer[rb->head];
    __pi_ring_buffer_head_pointer_update(rb);
    return buffer;
}

static void __pi_i2s_nb_buffer_ready_update(struct i2s_itf_data_s *itf_data)
{
    itf_data->nb_ready_buffer++;
}

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_i2s_handler_tx(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint8_t channel = TX_CHANNEL;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2S_ID(0);
    I2S_TRACE("I2S(%ld) IRQ: event=%ld channel=%d\n", periph_id, event, channel);

    struct i2s_itf_data_s *itf_data = g_i2s_itf_data[channel];
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    struct pi_task *task = pi_udma_fifo_hw_fifo_task_pop(udma_chan);

    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Pending data on current transfer ? */
    if (task->data[1] != 0)
    {
        I2S_TRACE("Reenqueue pending data on current transfer : "
                  "l2_buf=%lx, size=%ld, chan=%d.\n",
                  task->data[0], task->data[1], task->data[2]);
        __pi_i2s_copy_exec(itf_data, task, hw_buff_idx);
        #if I2S_DOUBLE_BUFFER
        if ((task->data[1]) && (hw_buff_idx == 0))
        {
            __pi_i2s_copy_exec(itf_data, task, 1);
        }
        #endif  /* I2S_DOUBLE_BUFFER */
    }
    else
    {
        #if I2S_DOUBLE_BUFFER
        if (task != pi_udma_fifo_hw_fifo_current_task_get(udma_chan, 0))
        #endif  /* I2S_DOUBLE_BUFFER */
        {
            //I2S_TRACE("I2S(%d) : pending transfer ongoing, return.\n", periph_id);
            I2S_TRACE("Handle end of current transfer, pop and start a new transfer if there is.\n");
            __pi_irq_handle_end_of_task(task);
        }
        task = pi_udma_fifo_sw_fifo_task_pop(udma_chan);
        I2S_TRACE("I2S(%d) : pop new task=%lx from SW fifo and handle it.\n",
                  itf_data->device_id, task);
        if (task != NULL)
        {
            __pi_i2s_copy_exec(itf_data, task, hw_buff_idx);
        }
    }
}

static void __pi_i2s_handler_rx(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint8_t channel = RX_CHANNEL;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_I2S_ID(0);
    I2S_TRACE("I2S(%d) IRQ: event=%ld channel=%d\n", periph_id, event, channel);

    struct i2s_itf_data_s *itf_data = g_i2s_itf_data[channel];
    //struct pi_task *task = itf_data->fifo_head;
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    struct pi_task *task = pi_udma_fifo_sw_fifo_task_pop(udma_chan);
    /* Reenqueue and signal data are available. */
    __pi_i2s_enqueue(itf_data);
    if (task != NULL)
    {
        task->data[1] = (uint32_t) __pi_i2s_pop(itf_data);
        //itf_data->fifo_head = itf_data->fifo_head->next;
        __pi_irq_handle_end_of_task(task);
    }
    else
    {
        __pi_i2s_nb_buffer_ready_update(itf_data);
    }
}

static uint8_t __pi_i2s_enqueue(struct i2s_itf_data_s *itf_data)
{
    uint8_t device_id = itf_data->device_id;
    uint32_t buff = 0, size = 0;
    uint32_t udma_cfg = (uint32_t) UDMA_CORE_CFG_DATA_SIZE(itf_data->word_size >> 4);
    uint8_t is_rx = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_RX_TX_SHIFT);
    //udma_channel_e channel = (itf_data->i2s_id == 0) ? RX_CHANNEL : TX_CHANNEL;
    udma_channel_e channel = !is_rx;

    size = itf_data->block_size;
    buff = (uint32_t) __pi_ring_buffer_buffer_put(&(itf_data->ring_buffer));
    I2S_TRACE("I2S(%d) HW enqueue: buf=%lx size=%ld, channel=%d\n",
              device_id, buff, size, channel);
    hal_udma_i2s_enqueue(device_id, buff, size, udma_cfg, channel);
    return 0;
}

static void *__pi_i2s_pop(struct i2s_itf_data_s *itf_data)
{
    uint32_t *buffer = NULL;
    buffer = __pi_ring_buffer_buffer_get(&(itf_data->ring_buffer));
    I2S_TRACE("I2S(%d): mem_slab pop buffer=%lx\n", itf_data->device_id, buffer);
    return buffer;
}

static void __pi_i2s_copy_exec(struct i2s_itf_data_s *itf_data,
                               struct pi_task *task, uint8_t hw_buff_idx)
{
    uint8_t device_id = itf_data->device_id;
    uint32_t buff = 0, size = 0;
    uint32_t max_size = (uint32_t) UDMA_MAX_SIZE - 4;
    uint32_t udma_cfg = task->data[3];
    udma_channel_e channel = task->data[2];
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    pi_udma_fifo_buffer_set(task, &buff, &size, max_size);
    pi_udma_fifo_hw_fifo_task_enqueue(udma_chan, task, hw_buff_idx);
    I2S_TRACE("I2S(%d) : HW enqueue single task=%lx, buf=%lx size=%ld, "
              "channel=%d, size_remain=%ld.\n",
              device_id, task, buff, size, channel, task->data[1]);
    hal_udma_i2s_enqueue(device_id, buff, size, udma_cfg, channel);
}

static inline uint32_t __pi_i2s_clk_div(uint32_t i2s_freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    uint32_t clk_div = 0;
    clk_div = ((periph_freq / i2s_freq) - 1) >> 1;
    I2S_TRACE("Periph_freq=%ld i2s_freq=%ld, clk_div=%ld\n",
              periph_freq, i2s_freq, clk_div);
    if (clk_div > 0xFFFF)
    {
        I2S_TRACE_ERR("Clock divider too big, SoC frequency is too high.\n");
    }
    return clk_div;
}

static void __pi_i2s_clock_enable(struct i2s_itf_data_s *itf_data)
{
    uint32_t base = (uint32_t) i2s(itf_data->device_id);
    //uint8_t ext_clk = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_CLK_SRC_SHIFT);
    udma_i2s_i2s_clkcfg_setup_t clk_setup = { .raw = itf_data->clk_setup };
    /* In case the itf is already opened. */
    clk_setup.raw |= udma_i2s_i2s_clkcfg_setup_get(base);
    uint32_t clk_div = __pi_i2s_clk_div(itf_data->frequency);

    /* Enable clock to acquire data. Enable only if clocks are internally generated. */
    clk_setup.common_clk_div = clk_div >> 8;
    clk_setup.master_clk_div = clk_div & 0xFF;
    clk_setup.slave_clk_div = clk_div & 0xFF;

    I2S_TRACE("I2S(%d) : setting CLKCFG=%lx\n", itf_data->device_id, clk_setup.raw);
    udma_i2s_i2s_clkcfg_setup_set(base, clk_setup.raw);
}

static void __pi_i2s_clock_disable(struct i2s_itf_data_s *itf_data)
{
    uint32_t base = (uint32_t) i2s(itf_data->device_id);
    /* Enable clock to acquire data. Enable only if clocks are internally generated. */
    udma_i2s_i2s_clkcfg_setup_t clk_setup = { .raw = 0 };
    I2S_TRACE("I2S(%d) : setting CLKCFG=%lx\n", itf_data->device_id, clk_setup.raw);
    udma_i2s_i2s_clkcfg_setup_set(base, clk_setup.raw);
}

static void __pi_i2s_resume(struct i2s_itf_data_s *itf_data)
{
    I2S_TRACE("I2S(%d) : resuming.\n", itf_data->i2s_id);

    /* Enable clock. */
    __pi_i2s_clock_enable(itf_data);
}

static void __pi_i2s_suspend(struct i2s_itf_data_s *itf_data)
{
    /* Disable clock. */
    __pi_i2s_clock_disable(itf_data);
    I2S_TRACE("I2S(%d) : suspending.\n", itf_data->i2s_id);

    #if 0
    /* Wait end of pending samples. */
    for (volatile uint32_t i=0; i<100; i++);

    /* Clear channels. */
    hal_udma_i2s_clear(device_id, itf_data->i2s_id);
    #endif
}


static int32_t __pi_i2s_conf_apply(struct i2s_itf_data_s *itf_data)
{
    uint32_t base = (uint32_t) i2s(itf_data->device_id);

    uint8_t lsb = ARCHI_BEXTRACTU((itf_data->flags >> 16), 1, PI_I2S_CH_FMT_DATA_ORDER_SHIFT);
    uint8_t ext_clk = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_CLK_SRC_SHIFT);
    uint8_t is_rx = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_RX_TX_SHIFT);
    uint8_t fdx_ena = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_FULL_DUPLEX_SHIFT);
    uint8_t clk_src = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_CLK_SRC_SEL_SHIFT);
    uint8_t dual = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_DUAL_MODE_SHIFT);
    // CLK enable should be 1, no matter clk is internal or external.
    uint8_t slv_ena = 1;//((!ext_clk) && ((clk_src == PI_I2S_OPT_CLK_SLV_OFFSET)));
    uint8_t mst_ena = 1;//((!ext_clk) && ((clk_src == PI_I2S_OPT_CLK_MST_OFFSET)));
    I2S_TRACE("EXT_CLK=%d, IS_RX=%d, FDX_ENA=%d, CLK_SRC=%d, SLV_ENA=%d, "
              "MST_ENA=%d, LSB=%d, DUAL=%d\n",
              ext_clk, is_rx, fdx_ena, clk_src, slv_ena, mst_ena, lsb, dual);

    /* Clock settings. */
    udma_i2s_i2s_clkcfg_setup_t clk_setup = { .raw = itf_data->clk_setup };
    clk_setup.slave_clk_en = slv_ena;
    clk_setup.master_clk_en = mst_ena;
    clk_setup.pdm_clk_en = itf_data->pdm;

    /* Slave setup. */
    if (is_rx)
    {
        clk_setup.slave_ext = ext_clk;
        clk_setup.slave_num = clk_src;
        udma_i2s_i2s_slv_setup_t slave_setup = { .raw = 0 };
        slave_setup.slave_words = (itf_data->channels - 1) >> 1;
        slave_setup.slave_bits = itf_data->word_size - 1;
        slave_setup.slave_lsb = lsb;
        slave_setup.slave_2ch = dual;
        slave_setup.slave_en = 1;
        I2S_TRACE("I2S(%d) : setting SLAVE=%lx\n", itf_data->device_id, slave_setup.raw);
        udma_i2s_i2s_slv_setup_set(base, slave_setup.raw);
    }
    else
    {
        clk_setup.master_ext = ext_clk;
        clk_setup.master_num = clk_src;
        /* Master setup. */
        udma_i2s_i2s_mst_setup_t master_setup = { .raw = 0 };
        master_setup.master_words = (itf_data->channels - 1) >> 1;
        master_setup.master_bits = itf_data->word_size - 1;
        master_setup.master_lsb = lsb;
        master_setup.master_2ch = dual;
        master_setup.master_en = 1;
        I2S_TRACE("I2S(%d) : setting MASTER=%lx\n", itf_data->device_id, master_setup.raw);
        udma_i2s_i2s_mst_setup_set(base, master_setup.raw);
    }
    itf_data->clk_setup = clk_setup.raw;

    /* PDM setup. */
    udma_i2s_i2s_pdm_setup_t pdm_setup = { .raw = 0 };
    if (itf_data->pdm)
    {
        uint8_t shift = (10 - itf_data->pdm_decimation) & 7;
        uint8_t pdm_mode = ARCHI_BEXTRACTU(itf_data->flags, __RT_FLAGS_MUX_WIDTH, __RT_FLAGS_MUX_BIT);
        pdm_mode |= dual;

        pdm_setup.pdm_shift = shift;
        pdm_setup.pdm_decimation = (1 << itf_data->pdm_decimation) - 1;
        pdm_setup.pdm_mode = pdm_mode;
        pdm_setup.pdm_en = 1;

        /* Update i2s frequency. */
        itf_data->frequency *= (1 << itf_data->pdm_decimation);
    }
    else
    {
        /* Update i2s frequency. */
        itf_data->frequency *= (itf_data->word_size * itf_data->channels);
        // NOT SURE WHY HAVE TO DO THIS FOR EVEN CHANNEL NUMBER. HW BUG?
        if ((itf_data->channels % 2) == 0)
        {
            //itf_data->frequency *= 2;
        }
    }
    I2S_TRACE("I2S(%d) : setting PDM=%lx\n", itf_data->device_id, pdm_setup.raw);
    udma_i2s_i2s_pdm_setup_set(base, pdm_setup.raw);
    return 0;
}

static int32_t __pi_i2s_conf_set(struct i2s_itf_data_s *itf_data,
                                 struct pi_i2s_conf *conf)
{
    itf_data->word_size           = conf->word_size;
    itf_data->channels            = conf->channels;
    itf_data->flags               = ((conf->format << 16) | conf->options);
    itf_data->frequency           = conf->frame_clk_freq;
    itf_data->block_size          = conf->block_size;
    itf_data->ring_buffer.nb_elem = conf->mem_slab->num_blocks;
    itf_data->ring_buffer.head    = 0;
    itf_data->ring_buffer.tail    = 0;
    itf_data->pdm                 = conf->pdm_filter_ena;
    itf_data->pdm_decimation      = conf->pdm_decimation;
    itf_data->nb_ready_buffer     = 0;

    uint8_t is_rx = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_RX_TX_SHIFT);
    /* RX : check block size and prepare buffers.
       When receiving data, continous mode is used.
       When sending data, normal mode is used. */
    if (is_rx)
    {
        if (itf_data->block_size > (uint32_t) UDMA_MAX_SIZE - 4)
        {
            I2S_TRACE_ERR("Block size error: udma_max_size=%ld < block_size=%ld\n",
                          (uint32_t) UDMA_MAX_SIZE - 4, itf_data->block_size);
            return -13;
        }

        if ((itf_data->flags & PI_I2S_OPT_MEM_SLAB) == 0)
        {
            itf_data->ring_buffer.nb_elem = 2;
            itf_data->ring_buffer.buffer = pi_fc_l1_malloc(sizeof(void *) * itf_data->ring_buffer.nb_elem);
            if (itf_data->ring_buffer.buffer == NULL)
            {
                I2S_TRACE_ERR("Error alloc buffers\n");
                return -14;
            }
            itf_data->ring_buffer.buffer[0] = conf->pingpong_buffers[0];
            itf_data->ring_buffer.buffer[1] = conf->pingpong_buffers[1];
            __pi_i2s_enqueue(itf_data);
        }
        else
        {
            itf_data->ring_buffer.buffer = pi_fc_l1_malloc(sizeof(void *) * itf_data->ring_buffer.nb_elem);
            if (itf_data->ring_buffer.buffer == NULL)
            {
                I2S_TRACE_ERR("Error alloc buffers\n");
                return -15;
            }
            for (uint32_t buf=0; buf<itf_data->ring_buffer.nb_elem; buf++)
            {
                itf_data->ring_buffer.buffer[buf] = (void *) &(conf->mem_slab->buffer[buf * conf->mem_slab->block_size]);
                I2S_TRACE("Ring buffers: buf_%d=%lx\n", buf, itf_data->ring_buffer.buffer[buf]);
            }
            #if I2S_DOUBLE_BUFFER
            if (itf_data->ring_buffer.nb_elem > 2)
            {
                __pi_i2s_enqueue(itf_data);
            }
            #endif  /* I2S_DOUBLE_BUFFER */
        }
        /* Double buffering. */
        #if I2S_DOUBLE_BUFFER
        __pi_i2s_enqueue(itf_data);
        #endif  /* I2S_DOUBLE_BUFFER */
    }

    return __pi_i2s_conf_apply(itf_data);
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    conf->itf                 = 0;
    conf->word_size           = 32;
    conf->channels            = 1;
    conf->format              = ( PI_I2S_CH_FMT_DATA_ORDER_MSB );
    conf->options             = ( PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_RX );//| PI_I2S_OPT_ENABLED );
    conf->frame_clk_freq      = 44100;
    conf->block_size          = 0;
    conf->pingpong_buffers[0] = NULL;
    conf->pingpong_buffers[1] = NULL;
    conf->mem_slab            = NULL;
    conf->pdm_decimation      = 8; /* Decimation factor given in log2. */
    conf->pdm_filter_ena      = 0;
    /* Not used. */
    conf->pdm_shift           = -1;
    conf->mem_word_size       = 0;
    conf->channel_id          = 0;
    conf->asrc_channel        = -1;
    conf->ref_clk_freq        = 0;
    conf->ws_delay            = 0;
    conf->pdm_polarity        = 0;
    conf->pdm_diff            = 0;
    conf->ts_evt_id           = 0xFF;
}

int32_t __pi_i2s_open(struct pi_i2s_conf *conf, struct i2s_itf_data_s **device_data)
{
    uint32_t irq = __disable_irq();
    if ((uint8_t) (UDMA_NB_I2S << 1) < conf->itf)
    {
        I2S_TRACE_ERR("Error : wrong interface ID, itf=%d !\n", conf->itf);
        __restore_irq(irq);
        return -11;
    }

    struct i2s_itf_data_s *itf_data = g_i2s_itf_data[conf->itf];
    if (itf_data == NULL)
    {
        /* Allocate driver data. */
        itf_data = pi_fc_l1_malloc(sizeof(struct i2s_itf_data_s));
        if (itf_data == NULL)
        {
            I2S_TRACE_ERR("Driver itf_data alloc failed !\n");
            __restore_irq(irq);
            return -12;
        }
        g_i2s_itf_data[conf->itf] = itf_data;
        itf_data->nb_open   = 0;
        itf_data->i2s_id    = conf->itf;
        itf_data->device_id = (conf->itf >> 1); /* 2 i2s periph(RX & TX) per dev. */
        itf_data->clk_setup = 0;
        pi_udma_fifo_init(&(itf_data->udma_chan));
        //itf_data->fifo_head = NULL;
        //itf_data->fifo_tail = NULL;

        /* Init once for both RX and TX. */
        uint8_t i2s_itf = !(itf_data->i2s_id);
        if (g_i2s_itf_data[i2s_itf] == NULL)
        {
            /* Set handlers. */
            pi_fc_event_handler_set(SOC_EVENT_UDMA_I2S_RX(itf_data->device_id), __pi_i2s_handler_rx);
            pi_fc_event_handler_set(SOC_EVENT_UDMA_I2S_TX(itf_data->device_id), __pi_i2s_handler_tx);
            /* Enable SOC events propagation to FC. */
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2S_RX(itf_data->device_id));
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2S_TX(itf_data->device_id));

            /* Open device for first time. */
            I2S_TRACE("Opening I2S(%d) udma_id=%ld @ %lx\n",
                      itf_data->device_id, UDMA_I2S_ID(itf_data->device_id),
                      i2s(itf_data->device_id));

            /* Disable UDMA CG. */
            hal_udma_ctrl_cg_disable(UDMA_I2S_ID(itf_data->device_id));
            I2S_TRACE("I2S(%d) : disable CG for udma_id=%ld %p %lx\n",
                      itf_data->device_id, UDMA_I2S_ID(itf_data->device_id),
                      &(udma_ctrl->cg), udma_ctrl_cg_get());
        }

        int32_t status = __pi_i2s_conf_set(itf_data, conf);
        if (status)
        {
            I2S_TRACE_ERR("Error applying conf : %ld\n", status);
            __restore_irq(irq);
            return status;
        }
    }
    *device_data = g_i2s_itf_data[conf->itf];
    /* Increment number of devices opened. */
    itf_data->nb_open++;
    I2S_TRACE("Device id=%d (i2s_%d) opened=%ld\n",
              itf_data->device_id, itf_data->i2s_id, itf_data->nb_open);
    __restore_irq(irq);
    return 0;
}

void __pi_i2s_close(struct i2s_itf_data_s *itf_data)
{
    uint32_t irq = __disable_irq();
    uint32_t device_id = itf_data->device_id;
    /* Decrement number of devices opened. */
    itf_data->nb_open--;
    /* Free device and structure opened. */
    if (itf_data->nb_open == 0)
    {
        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2S_RX(device_id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_I2S_TX(device_id));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2S_RX(device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_I2S_TX(device_id));

        /* Enable UDMA CG. */
        /* Disable only if both I2S are not used. */
        uint8_t i2s_itf = !(itf_data->i2s_id);
        if (g_i2s_itf_data[i2s_itf] == NULL)
        {
            hal_udma_ctrl_cg_enable(UDMA_I2S_ID(device_id));
        }

        /* Free allocated driver data. */
        g_i2s_itf_data[itf_data->i2s_id] = NULL;
        uint8_t is_rx = ARCHI_BEXTRACTU(itf_data->flags, 1, PI_I2S_OPT_RX_TX_SHIFT);
        if (is_rx)
        {
            pi_fc_l1_free(itf_data->ring_buffer.buffer, sizeof(void *) * itf_data->ring_buffer.nb_elem);
        }
        pi_fc_l1_free(itf_data, sizeof(struct i2s_itf_data_s));
    }
    __restore_irq(irq);
}

int32_t __pi_i2s_ioctl(struct i2s_itf_data_s *itf_data, uint32_t cmd, void *arg)
{
    int32_t status = 0;
    uint32_t irq = __disable_irq();
    I2S_TRACE("I2S(%d) ioctl command=%ld, arg=%lx\n", itf_data->device_id, cmd, arg);
    switch (cmd)
    {
    case PI_I2S_IOCTL_START :
        __pi_i2s_resume(itf_data);
        break;

    case PI_I2S_IOCTL_STOP :
        __pi_i2s_suspend(itf_data);
        break;

    case PI_I2S_IOCTL_CONF_SET :
        status = __pi_i2s_conf_set(itf_data, (struct pi_i2s_conf *) arg);
        break;

        #if 0
    case PI_I2S_IOCTL_CLOCK_ENABLE :
        __pi_i2s_clock_enable(itf_data);
        break;

    case PI_I2S_IOCTL_CLOCK_DISABLE :
        __pi_i2s_clock_disable(itf_data);
        break;
        #endif

    default :
        __restore_irq(irq);
        return -1;
    }
    __restore_irq(irq);
    return status;
}

int32_t __pi_i2s_read_async(struct i2s_itf_data_s *itf_data, pi_task_t *task)
{
    uint32_t irq = disable_irq();
    I2S_TRACE("Read i2s(%d).\n", itf_data->device_id);
    /* Fill rest of arguments for i2s driver. */
    task->data[0] = 0;
    task->data[2] = itf_data->block_size;
    task->next = NULL;
    if (itf_data->nb_ready_buffer)
    {
        I2S_TRACE("Read i2s(%d) nb_ready_buffer=%ld\n",
                  itf_data->device_id, itf_data->nb_ready_buffer);
        task->data[1] = (uint32_t) __pi_i2s_pop(itf_data);
        itf_data->nb_ready_buffer--;
        __pi_irq_handle_end_of_task(task);
    }
    else
    {
        pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
        pi_udma_fifo_sw_fifo_task_enqueue(udma_chan, task);
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
    if (size != NULL)
    {
        *size = task->data[2];
    }
    return task->data[0];
}

int32_t __pi_i2s_write_async(struct i2s_itf_data_s *itf_data, void *buffer,
                             size_t size, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    task->data[0] = (uint32_t) buffer;
    task->data[1] = size;
    task->data[2] = TX_CHANNEL;
    task->data[3] = (uint32_t) UDMA_CORE_CFG_DATA_SIZE(itf_data->word_size >> 4);
    task->data[4] = itf_data->device_id;
    //task->data[5] = 0;
    task->next    = NULL;

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Both HW buffers in use. */
    if (hw_buff_idx == -1)
    {
        I2S_TRACE("I2S(%d) : in use, enqueue transfer in SW buffer. "
                  "Task=%lx, l2_buf=%lx, size=%ld, channel=%d\n",
                  itf_data->device_id, task, task->data[0], task->data[1],
                  task->data[2]);
        pi_udma_fifo_sw_fifo_task_enqueue(udma_chan, task);
    }
    /* Enqueue the transfer. */
    else
    {
        I2S_TRACE("I2S(%d): enqueue transfer in HW buffer. "
                  "Task=%lx, l2_buf=%lx size=%ld, channel=%d\n",
                  itf_data->device_id, task, task->data[0], task->data[1],
                  task->data[2]);
        __pi_i2s_copy_exec(itf_data, task, hw_buff_idx);
        #if I2S_DOUBLE_BUFFER
        if ((task->data[1]) && (hw_buff_idx == 0))
        {
            __pi_i2s_copy_exec(itf_data, task, 1);
        }
        #endif  /* I2S_DOUBLE_BUFFER */
    }
    __restore_irq(irq);
    return 0;
}

int32_t __pi_i2s_write_status(struct pi_task *task)
{
    return task->data[1];
}
