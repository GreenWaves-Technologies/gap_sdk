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
#include "pmsis/drivers/uart.h"
//#include "pmsis/implem/drivers/drivers.h"
//#include "pmsis/implem/drivers/udma/uart/uart_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define UART_DOUBLE_BUFFER 1

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static struct uart_itf_data_s *g_uart_itf_data[UDMA_NB_UART] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* IRQ handler. */
static void __pi_uart_error_handler(void *arg);
static void __pi_uart_handler(void *arg);

/* Execute a transfer. */
static void __pi_uart_copy_exec(struct uart_itf_data_s *itf_data,
                                struct pi_task *task, uint8_t hw_buff_idx);

/* Configure UART. */
static int32_t __pi_uart_conf_set(struct uart_itf_data_s *itf_data,
                                  struct pi_uart_conf *conf);

/* Abort current transfer and flush pending transfers. */
static void __pi_uart_rx_abort(struct uart_itf_data_s *itf_data);
static void __pi_uart_tx_abort(struct uart_itf_data_s *itf_data);

/* Enable channel. */
static inline void __pi_uart_rx_enable(struct uart_itf_data_s *itf_data);
static inline void __pi_uart_tx_enable(struct uart_itf_data_s *itf_data);

#if 0
/* Update clock divider when FC/SoC frequency changes. */
static void __pi_uart_freq_cb(void *args);
#endif

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_uart_error_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_UART_ID(0);
    //UART_TRACE("Uart IRQ %ld %ld\n", event, periph_id);
    struct uart_itf_data_s *itf_data = g_uart_itf_data[periph_id];
    uint32_t error = hal_udma_uart_error_get(periph_id);
    UART_TRACE_ERR("UART(%ld) ERROR IRQ : %lx\n", periph_id, error);
    //printf("UART(%ld) ERROR IRQ : %lx\n", periph_id, error);
}

static void __pi_uart_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_UART_ID(0);
    UART_TRACE("Uart IRQ %ld %ld\n", event, periph_id);

    struct uart_itf_data_s *itf_data = g_uart_itf_data[periph_id];
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan[channel]);
    struct pi_task *task = pi_udma_fifo_hw_fifo_task_pop(udma_chan);
    UART_TRACE("UART(%d) IRQ channel %d, pop task=%lx from HW buffer.\n",
               periph_id, channel, task);

    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Pending data on current transfer ? */
    if (task->data[1] != 0)
    {
        UART_TRACE("Reenqueue pending data on current transfer.\n");
        __pi_uart_copy_exec(itf_data, task, hw_buff_idx);
        #if UART_DOUBLE_BUFFER
        if ((task->data[1]) && (hw_buff_idx == 0))
        {
            __pi_uart_copy_exec(itf_data, task, 1);
        }
        #endif  /* UART_DOUBLE_BUFFER */
    }
    else
    {
        #if UART_DOUBLE_BUFFER
        if (task != pi_udma_fifo_hw_fifo_current_task_get(udma_chan, 0))
        #endif  /* UART_DOUBLE_BUFFER */
        {
            //UART_TRACE("UART(%d) : pending transfer ongoing, return.\n", periph_id);
            UART_TRACE("Handle end of current transfer, pop and start a new transfer if there is.\n");
            __pi_irq_handle_end_of_task(task);
        }
        task = pi_udma_fifo_sw_fifo_task_pop(udma_chan);
        UART_TRACE("UART(%d) : pop new task=%lx from SW fifo and handle it.\n",
                   itf_data->device_id, task);
        if (task != NULL)
        {
            __pi_uart_copy_exec(itf_data, task, hw_buff_idx);
        }
    }
}

static inline void __pi_uart_rx_enable(struct uart_itf_data_s *itf_data)
{
    UART_TRACE("UART(%d) : enable channel %d\n", itf_data->device_id, RX_CHANNEL);
    hal_udma_uart_rx_enable(itf_data->device_id);
}

static inline void __pi_uart_tx_enable(struct uart_itf_data_s *itf_data)
{
    UART_TRACE("UART(%d) : enable channel %d\n", itf_data->device_id, TX_CHANNEL);
    hal_udma_uart_tx_enable(itf_data->device_id);
}

static void __pi_uart_rx_abort(struct uart_itf_data_s *itf_data)
{
    uint8_t device_id = itf_data->device_id;
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_rx_saddr %lx uart->udma_rx_size %lx uart->udma_rx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.rx_saddr, uart(device_id)->udma.rx_size,
               uart(device_id)->udma.rx_cfg);
    hal_udma_uart_rx_disable(device_id);
    hal_udma_uart_rx_clear(device_id);
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_rx_saddr %lx uart->udma_rx_size %lx uart->udma_rx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.rx_saddr, uart(device_id)->udma.rx_size,
               uart(device_id)->udma.rx_cfg);
    pi_udma_fifo_init(&(itf_data->udma_chan[RX_CHANNEL]));
}

static void __pi_uart_tx_abort(struct uart_itf_data_s *itf_data)
{
    uint32_t device_id = itf_data->device_id;
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_tx_saddr %lx uart->udma_tx_size %lx uart->udma_tx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.tx_saddr, uart(device_id)->udma.tx_size,
               uart(device_id)->udma.tx_cfg);
    hal_udma_uart_tx_disable(device_id);
    hal_udma_uart_tx_clear(device_id);
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_tx_saddr %lx uart->udma_tx_size %lx uart->udma_tx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.tx_saddr, uart(device_id)->udma.tx_size,
               uart(device_id)->udma.tx_cfg);
    pi_udma_fifo_init(&(itf_data->udma_chan[TX_CHANNEL]));
}

/* Clock divider. */
static inline uint32_t __pi_uart_clk_div(uint32_t baudrate)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    uint32_t clk_div =  (periph_freq + baudrate/2) / baudrate - 1;
    if (clk_div > 0xFFFF)
    {
        UART_TRACE_ERR("Error computing clock divider : Fsoc=%ld, Fuart=%ld, clock_div=%lx\n",
                       periph_freq, baudrate, clk_div);
        return 0xFFFFFFFF;
    }
    // The counter in the UDMA will count from 0 to div included
    // and then will restart from 0, so we must give div - 1 as
    // divider
    clk_div--;
    return clk_div;
}

static int32_t __pi_uart_conf_set(struct uart_itf_data_s *itf_data,
                                  struct pi_uart_conf *conf)
{
    itf_data->baudrate_bps = conf->baudrate_bps;
    uint8_t device_id = itf_data->device_id;
    uint8_t clean_fifo = 0;
    uint8_t polling_ena = 0;
    uint32_t clk_div = __pi_uart_clk_div(itf_data->baudrate_bps);
    if (clk_div == 0xFFFFFFFF)
    {
        UART_TRACE_ERR("UART(%d) : error computing clock divider !\n", device_id);
        return -21;
    }

    UART_TRACE("UART(%d) setting: baudrate=%ld CLK_DIV=%ld, RX_ENA=%d, "
               "TX_ENA=%d, STOP_BITS=%d, WORD_SIZE=%d, PARITY_ENA=%d\n",
               device_id, itf_data->baudrate_bps, clk_div, conf->enable_rx,
               conf->enable_tx, conf->stop_bit_count + 1, conf->word_size + 5,
               conf->parity_mode);
    hal_udma_uart_setup_set(device_id, conf->parity_mode, conf->word_size,
                            conf->stop_bit_count, clean_fifo, polling_ena,
                            conf->enable_tx, conf->enable_rx, clk_div);
    UART_TRACE("UART(%d) setup=%lx\n", device_id, hal_udma_uart_setup_get(device_id));
    return 0;
}

static void __pi_uart_copy_exec(struct uart_itf_data_s *itf_data,
                                struct pi_task *task, uint8_t hw_buff_idx)
{
    uint8_t device_id = itf_data->device_id;
    uint32_t buff = 0, size = 0;
    uint32_t max_size = (uint32_t) UDMA_MAX_SIZE - 4;
    uint32_t udma_cfg = task->data[3];
    udma_channel_e channel = task->data[2];
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan[channel]);
    pi_udma_fifo_buffer_set(task, &buff, &size, max_size);
    pi_udma_fifo_hw_fifo_task_enqueue(udma_chan, task, hw_buff_idx);
    UART_TRACE("UART(%d) : HW enqueue single task=%lx, buf=%lx size=%ld, "
               "channel=%d, size_remain=%ld.\n",
               device_id, task, buff, size, channel, task->data[1]);
    hal_udma_uart_enqueue(device_id, buff, size, udma_cfg, channel);
}

#if 0
static void __pi_uart_freq_cb(void *args)
{
    uint32_t irq = __disable_irq();
    struct uart_itf_data_s *itf_data = (struct uart_itf_data_s *) args;
    uint8_t device_id = itf_data->device_id;
    /* Wait end of transfer. */
    while ((hal_udma_uart_rx_status_get(device_id)) ||
           (hal_udma_uart_tx_status_get(device_id)));
    /* Disable RX and TX. */
    hal_udma_uart_rx_disable(device_id);
    hal_udma_uart_tx_disable(device_id);
    /* Update clock divider. */
    UART_TRACE("UART_SETUP = %lx\n", uart(device_id)->setup);
    uint16_t clk_div = __pi_uart_clk_div(itf_data->baudrate_bps);
    hal_udma_uart_clkdiv_set(device_id, clk_div);
    UART_TRACE("UART_SETUP = %lx\n", uart(device_id)->setup);
    /* Enable RX and TX. */
    hal_udma_uart_rx_enable(device_id);
    hal_udma_uart_tx_enable(device_id);
    __restore_irq(irq);
}
#endif

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_uart_conf_init(struct pi_uart_conf *conf)
{
    conf->baudrate_bps = 115200;                  /* Default baudrate. */
    conf->parity_mode = PI_UART_PARITY_DISABLE;   /* No parity. */
    conf->stop_bit_count = PI_UART_STOP_BITS_ONE; /* One stop bit. */
    conf->word_size = PI_UART_WORD_SIZE_8_BITS;   /* 8 bits per word. */
    conf->enable_rx = 0;                          /* Disable RX. */
    conf->enable_tx = 0;                          /* Disable TX. */
    conf->uart_id = 0;                            /* Device ID. */
    conf->use_ctrl_flow = 0;                      /* Flow control. */
}

int32_t __pi_uart_open(struct pi_uart_conf *conf, struct uart_itf_data_s **device_data)
{
    uint32_t irq = __disable_irq();
    if ((uint8_t) UDMA_NB_UART < conf->uart_id)
    {
        UART_TRACE_ERR("Error : wrong interface ID, itf=%d !\n", conf->uart_id);
        __restore_irq(irq);
        return -11;
    }

    struct uart_itf_data_s *itf_data = g_uart_itf_data[conf->uart_id];
    if (itf_data == NULL)
    {
        /* Allocate driver data. */
        itf_data = pi_l2_malloc(sizeof(struct uart_itf_data_s));
        if (itf_data == NULL)
        {
            UART_TRACE_ERR("Driver data alloc failed !\n");
            __restore_irq(irq);
            return -12;
        }
        pi_udma_fifo_init(&(itf_data->udma_chan[RX_CHANNEL]));
        pi_udma_fifo_init(&(itf_data->udma_chan[TX_CHANNEL]));
        itf_data->device_id = conf->uart_id;
        itf_data->nb_open = 0;
        //itf_data->baudrate_bps = conf->baudrate_bps;
        g_uart_itf_data[itf_data->device_id] = itf_data;

        /* Open device for first time. */
        UART_TRACE("Opening UART(%d) udma_id=%ld @ %lx\n",
                   itf_data->device_id, UDMA_UART_ID(itf_data->device_id),
                   uart(itf_data->device_id));

        /* Disable UDMA CG. */
        hal_udma_ctrl_cg_disable(UDMA_UART_ID(itf_data->device_id));
        UART_TRACE("UART(%d) : disable CG for udma_id=%ld %p %lx\n",
                   itf_data->device_id, UDMA_UART_ID(itf_data->device_id),
                   &(udma_ctrl->cg), udma_ctrl_cg_get());

        hal_udma_uart_irq_enable(itf_data->device_id);

        UART_TRACE("Enable SoC events and set handlers : RX : %d -> %p | TX: %d -> %p\n",
                   SOC_EVENT_UDMA_UART_RX(itf_data->device_id), __pi_uart_handler,
                   SOC_EVENT_UDMA_UART_TX(itf_data->device_id), __pi_uart_handler);
        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_RX(itf_data->device_id), __pi_uart_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_TX(itf_data->device_id), __pi_uart_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_ERR(itf_data->device_id), __pi_uart_error_handler);

        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_RX(itf_data->device_id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_TX(itf_data->device_id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_ERR(itf_data->device_id));

        /* Configure UART. */
        __pi_uart_conf_set(itf_data, conf);
        hal_udma_uart_rx_fifo_clean(itf_data->device_id);

        #if 0
        /* Attach freq callback. */
        pi_freq_callback_init(&(data->uart_freq_cb), __pi_uart_freq_cb, data);
        pi_freq_callback_add(&(data->uart_freq_cb));
        #endif
    }
    itf_data->nb_open++;
    *device_data = g_uart_itf_data[conf->uart_id];
    UART_TRACE("UART(%d) : opened %ld time(s).\n",
               itf_data->device_id, itf_data->nb_open);

    __restore_irq(irq);
    return 0;
}

void __pi_uart_close(struct uart_itf_data_s *itf_data)
{
    uint32_t irq = __disable_irq();
    itf_data->nb_open--;
    UART_TRACE("UART(%d) : number of opened devices %ld.\n",
               itf_data->device_id, itf_data->nb_open);
    /* Close itf if no devices opened. */
    if (itf_data->nb_open == 0)
    {
        /* Disable UART IRQs. */
        hal_udma_uart_irq_disable(itf_data->device_id);

        #if 0
        /* Remove freq callback. */
        pi_freq_callback_remove(&(itf_data->uart_freq_cb));
        #endif

        /* Make sure all bits are transferred. */
        while (hal_udma_uart_tx_status_get(itf_data->device_id));

        /* Wait some time, until data are sent. */
        for (volatile uint32_t i=0; i<1000; i++);

        /* Disable both RX and TX channels and flush fifo. */
        __pi_uart_rx_abort(itf_data);
        __pi_uart_tx_abort(itf_data);

        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_UART_RX(itf_data->device_id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_UART_TX(itf_data->device_id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_UART_ERR(itf_data->device_id));

        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_UART_RX(itf_data->device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_UART_TX(itf_data->device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_UART_ERR(itf_data->device_id));

        /* Enable UDMA CG. */
        hal_udma_ctrl_cg_enable(UDMA_UART_ID(itf_data->device_id));

        /* Free allocated data. */
        g_uart_itf_data[itf_data->device_id] = NULL;
        pi_l2_free(itf_data, sizeof(struct uart_itf_data_s));
    }
    restore_irq(irq);
}

int32_t __pi_uart_ioctl(struct uart_itf_data_s *itf_data, uint32_t cmd, void *arg)
{
    uint32_t irq = __disable_irq();
    int32_t status = 0;
    switch(cmd)
    {
    case PI_UART_IOCTL_CONF_SETUP :
        status = __pi_uart_conf_set(itf_data, (struct pi_uart_conf *) arg);
        break;

    case PI_UART_IOCTL_ABORT_RX :
        __pi_uart_rx_abort(itf_data);
        break;

    case PI_UART_IOCTL_ABORT_TX :
        __pi_uart_tx_abort(itf_data);
        break;

    case PI_UART_IOCTL_ENABLE_RX :
        __pi_uart_rx_enable(itf_data);
        break;

    case PI_UART_IOCTL_ENABLE_TX :
        __pi_uart_tx_enable(itf_data);
        break;

    default :
        status = -1;
    }
    __restore_irq(irq);
    return status;
}

void __pi_uart_copy(struct uart_itf_data_s *itf_data, uint32_t l2_buf,
                    uint32_t size, udma_channel_e channel,
                    struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    task->data[0] = l2_buf;
    task->data[1] = size;
    task->data[2] = channel;
    task->data[3] = (uint32_t) UDMA_CORE_CFG_DATA_SIZE(UDMA_CORE_CFG_DATASIZE_8);
    task->data[4] = itf_data->device_id;
    task->next    = NULL;

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan[channel]);
    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Both HW buffers in use. */
    if (hw_buff_idx == -1)
    {
        UART_TRACE("UART(%d) : in use, enqueue transfer in SW buffer. "
                   "Task=%lx, l2_buf=%lx, size=%ld, channel=%d\n",
                   itf_data->device_id, task, task->data[0], task->data[1],
                   task->data[2]);
        pi_udma_fifo_sw_fifo_task_enqueue(udma_chan, task);
    }
    /* Enqueue the transfer. */
    else
    {
        UART_TRACE("UART(%d): enqueue transfer in HW buffer. "
                   "Task=%lx, l2_buf=%lx size=%ld, channel=%d\n",
                   itf_data->device_id, task, task->data[0], task->data[1],
                   task->data[2]);
        __pi_uart_copy_exec(itf_data, task, hw_buff_idx);
        #if UART_DOUBLE_BUFFER
        if ((task->data[1]) && (hw_buff_idx == 0))
        {
            __pi_uart_copy_exec(itf_data, task, 1);
        }
        #endif  /* UART_DOUBLE_BUFFER */
    }
    __restore_irq(irq);
}
