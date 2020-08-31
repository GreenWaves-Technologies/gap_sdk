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
#include "pmsis/implem/drivers/udma/uart/uart_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static struct uart_itf_data_s *g_uart_itf_data[UDMA_NB_UART] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_uart_handle_end_of_task(struct pi_task *task);

/* IRQ handler. */
static void __pi_uart_handler(void *arg);

/* Enqueue a task in fifo. */
static uint32_t __pi_uart_task_fifo_enqueue(struct uart_itf_data_s *data,
                                            struct pi_task *task,
                                            udma_channel_e channel);

/* Pop a task from fifo. */
static struct pi_task *__pi_uart_task_fifo_pop(struct uart_itf_data_s *data,
                                               udma_channel_e channel);

/* Execute a transfer. */
static void __pi_uart_copy_exec(struct uart_itf_data_s *data,
                                struct pi_task *task);

/* Configure UART. */
static void __pi_uart_conf_set(struct uart_itf_data_s *data,
                               struct pi_uart_conf *conf);

/* Enable RX or TX channel. */
static inline void __pi_uart_channel_enable(struct uart_itf_data_s *data,
                                            udma_channel_e channel);

/* Abort current transfer and flush pending transfers. */
static void __pi_uart_rx_abort(struct uart_itf_data_s *data);

static void __pi_uart_tx_abort(struct uart_itf_data_s *data);

/* Enable channel. */
static inline void __pi_uart_channel_enable(struct uart_itf_data_s *data,
                                            udma_channel_e channel);

/* Enable flow control. */
static int32_t __pi_uart_flow_control_enable(struct uart_itf_data_s *data);

/* Execute a transfer with flow control enabled. */
static void __pi_uart_copy_exec_flow_control(struct uart_itf_data_s *data,
                                             struct pi_task *task);

/* Update clock divider when FC/SoC frequency changes. */
static void __pi_uart_freq_cb(void *args);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_uart_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        if (task->id == PI_TASK_CALLBACK_ID)
        {
            pi_task_push(task);
        }
    }
}

/* IRQ handler. */
static void __pi_uart_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_UART_ID(0);
    UART_TRACE("Uart IRQ %d %d\n", periph_id, event);

    struct uart_itf_data_s *data = g_uart_itf_data[periph_id];
    struct pi_task *task = data->fifo_head[channel];

    if (task->data[3] != 0)
    {
        if (data->flow_ctrl_ena)
        {
            UART_TRACE("Reenqueue pending data on current transfer(flow control enabled).\n");
            __pi_uart_copy_exec_flow_control(data, task);
        }
        else
        {
            UART_TRACE("Reenqueue pending data on current transfer.\n");
            __pi_uart_copy_exec(data, task);
        }
    }
    else
    {
        UART_TRACE("No pending data on current transfer.\n Handle end of "
                   "current transfer and pop and start a new transfer if there is.\n");
        task = __pi_uart_task_fifo_pop(data, channel);
        __pi_uart_handle_end_of_task(task);
        task = data->fifo_head[channel];
        if (task != NULL)
        {
            if (data->flow_ctrl_ena)
            {
                __pi_uart_copy_exec_flow_control(data, task);
            }
            else
            {
                __pi_uart_copy_exec(data, task);
            }
        }
    }
}

/* Enqueue a task in fifo. */
static inline uint32_t __pi_uart_task_fifo_enqueue(struct uart_itf_data_s *data,
                                                   struct pi_task *task,
                                                   udma_channel_e channel)
{
    uint8_t head = 0;
    uint32_t irq = __disable_irq();
    if (data->fifo_head[channel] == NULL)
    {
        /* Empty fifo. */
        /* Execute the transfer. */
        data->fifo_head[channel] = task;
        data->fifo_tail[channel] = data->fifo_head[channel];
        head = 0;
    }
    else
    {
        /* Transfer on going, enqueue this one to the list. */
        data->fifo_tail[channel]->next = task;
        data->fifo_tail[channel] = data->fifo_tail[channel]->next;
        head = 1;
    }
    __restore_irq(irq);
    return head;
}

/* Pop a task from fifo. */
static inline struct pi_task *__pi_uart_task_fifo_pop(struct uart_itf_data_s *data,
                                                      udma_channel_e channel)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    if (data->fifo_head[channel] != NULL)
    {
        task_to_return = data->fifo_head[channel];
        data->fifo_head[channel] = data->fifo_head[channel]->next;
        if (data->fifo_head[channel] == NULL)
        {
            data->fifo_tail[channel] = NULL;
        }
    }
    __restore_irq(irq);
    return task_to_return;
}

/* Abort current transfer and flush pending transfers. */
static void __pi_uart_rx_abort(struct uart_itf_data_s *data)
{
    uint32_t device_id = data->device_id;
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_rx_saddr %lx uart->udma_rx_size %lx uart->udma_rx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.rx_saddr, uart(device_id)->udma.rx_size,
               uart(device_id)->udma.rx_cfg);
    hal_uart_rx_disable(device_id);
    hal_uart_rx_clear(device_id);
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_rx_saddr %lx uart->udma_rx_size %lx uart->udma_rx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.rx_saddr, uart(device_id)->udma.rx_size,
               uart(device_id)->udma.rx_cfg);
    data->fifo_head[RX_CHANNEL] = NULL;
    data->fifo_tail[RX_CHANNEL] = NULL;
}

static void __pi_uart_tx_abort(struct uart_itf_data_s *data)
{
    uint32_t device_id = data->device_id;
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_tx_saddr %lx uart->udma_tx_size %lx uart->udma_tx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.tx_saddr, uart(device_id)->udma.tx_size,
               uart(device_id)->udma.tx_cfg);
    hal_uart_tx_disable(device_id);
    hal_uart_tx_clear(device_id);
    UART_TRACE("uart->setup %lx uart->status %lx\n"
               "uart->udma_tx_saddr %lx uart->udma_tx_size %lx uart->udma_tx_cfg %lx\n",
               uart(device_id)->setup, uart(device_id)->status,
               uart(device_id)->udma.tx_saddr, uart(device_id)->udma.tx_size,
               uart(device_id)->udma.tx_cfg);
    data->fifo_head[TX_CHANNEL] = NULL;
    data->fifo_tail[TX_CHANNEL] = NULL;
}

/* Clock divider. */
static inline uint16_t __pi_uart_clk_div(uint32_t baudrate)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_FC);
    uint16_t clk_div = periph_freq / baudrate;
    return clk_div;
}

/* Setup config. */
static inline void __pi_uart_conf_set(struct uart_itf_data_s *data,
                                      struct pi_uart_conf *conf)
{
    uint32_t device_id = data->device_id;
    uint16_t clk_div = __pi_uart_clk_div(conf->baudrate_bps);
    UART_TRACE("UART(%ld) setting: baudrate=%ld CLK_DIV=%ld, RX_ENA=%ld, "
               "TX_ENA=%ld, STOP_BITS=%ld, WORD_SIZE=%ld, PARITY_ENA=%ld\n",
               device_id, conf->baudrate_bps, clk_div, conf->enable_rx,
               conf->enable_tx, conf->stop_bit_count + 1, 5 + conf->word_size,
               conf->parity_mode);
    data->baudrate_bps = conf->baudrate_bps;
    hal_uart_setup_set(device_id, clk_div, conf->enable_rx, conf->enable_tx,
                       conf->stop_bit_count, conf->word_size, conf->parity_mode);

    UART_TRACE("UART(%ld) setup=%lx\n", device_id, uart(device_id)->setup);
}

static void __pi_uart_freq_cb(void *args)
{
    uint32_t irq = __disable_irq();
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) args;
    /* Wait end of transfer. */
    while ((hal_uart_rx_status_get(data->device_id)) ||
           (hal_uart_tx_status_get(data->device_id)));
    /* Disable RX and TX. */
    hal_uart_rx_disable(data->device_id);
    hal_uart_tx_disable(data->device_id);
    /* Update clock divider. */
    UART_TRACE("UART_SETUP = %lx\n", uart(data->device_id)->setup);
    uint16_t clk_div = __pi_uart_clk_div(data->baudrate_bps);
    hal_uart_clkdiv_set(data->device_id, clk_div);
    UART_TRACE("UART_SETUP = %lx\n", uart(data->device_id)->setup);
    /* Enable RX and TX. */
    hal_uart_rx_enable(data->device_id);
    hal_uart_tx_enable(data->device_id);
    __restore_irq(irq);
}

/* Enable channel. */
static inline void __pi_uart_channel_enable(struct uart_itf_data_s *data,
                                            udma_channel_e channel)
{
    uint32_t device_id = data->device_id;
    if (channel == RX_CHANNEL)
    {
        hal_uart_rx_enable(device_id);
    }
    else
    {
        hal_uart_tx_enable(device_id);
    }
}

#if defined(UART_FLOW_CONTROL_EMU)
static int32_t __pi_uart_flow_control_enable(struct uart_itf_data_s *data)
{
    int32_t status = 0;
    struct pi_device *pwm = &(data->pwm);
    struct pi_pwm_conf pwm_conf = {0};
    pi_pwm_conf_init(&pwm_conf);
    pwm_conf.pwm_id = 0;
    pwm_conf.ch_id = PI_PWM_CHANNEL2;
    pwm_conf.timer_conf = PI_PWM_EVT_FALL | PI_PWM_CLKSEL_FLL | PI_PWM_UPDOWNSEL_ALT;
    pi_open_from_conf(pwm, &pwm_conf);
    status = pi_pwm_open(pwm);
    if (status)
    {
        UART_TRACE_ERR("Error opening PWM device : %ld\n", status);
        return -11;
    }
    /* No IRQ. */
    /* Clear handler. */
    pi_fc_event_handler_clear(SOC_EVENT_PWM(pwm_conf.pwm_id));
    /* Disable SOC event propagation to FC. */
    hal_soc_eu_clear_fc_mask(SOC_EVENT_PWM(pwm_conf.pwm_id));
    /* Timer threshold. */
    pi_pwm_ioctl(pwm, PI_PWM_TIMER_THRESH, 0x0);
    /* Channel mode and threshold. */
    struct pi_pwm_ioctl_ch_config ch_conf = {0};
    ch_conf.ch_threshold = 0xFFFF;
    ch_conf.config = PI_PWM_SET;
    ch_conf.channel = pwm_conf.ch_id;
    pi_pwm_ioctl(pwm, PI_PWM_CH_CONFIG, &ch_conf);
    /* CMD : update, arm and start. */
    pi_pwm_ioctl(pwm, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_UPDATE);
    pi_pwm_ioctl(pwm, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_ARM);
    pi_pwm_ioctl(pwm, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_START);

    pi_pad_set_function(PI_PAD_33_B12_TIMER0_CH2, PI_PAD_FUNC0);  // SET B12 TIM0CH2 (RTS)
    pi_gpio_pin_configure(NULL, PI_GPIO_A1_PAD_9_B3, PI_GPIO_INPUT);  // GPIO1 ON B3 AS INPUT (CTS)
    pi_gpio_pin_configure(NULL, PI_GPIO_A0_PAD_8_A4, PI_GPIO_INPUT);  //GPIO0 ON A4 AS INPUT (UARRT_RX DETECT)
    data->flow_ctrl_ena = 1;
    UART_TRACE("Flow control enabled !\n");
    return status;
}

static void __pi_uart_copy_exec_flow_control(struct uart_itf_data_s *data,
                                             struct pi_task *task)
{
    uint32_t device_id = data->device_id;
    uint32_t l2_buf = task->data[0] + task->data[4];
    uint32_t size = 1;
    udma_channel_e channel = task->data[2];
    hal_compiler_barrier();
    task->data[4]++;            /* Buffer pointer. */
    task->data[3]--;            /* Size. */
    if (channel == TX_CHANNEL)
    {
        /* Wait for CTS. */
        uint32_t value = 0;
        value = pi_gpio_pin_read(NULL, PI_GPIO_A1_PAD_9_B3, &value);
        while (value == 1)
        {
            value = pi_gpio_pin_read(NULL, PI_GPIO_A1_PAD_9_B3, &value);
        }
    }
    UART_TRACE("FC UART(%ld): Execute %s transfer l2_buf=%lx size=%ld\n",
               device_id, ((channel == RX_CHANNEL) ? "RX" : "TX"), l2_buf, size);
    hal_uart_enqueue(device_id, l2_buf, size, 0, channel);
    if (channel == RX_CHANNEL)
    {
        /* Send RTS. */
        pi_pwm_ioctl(&(data->pwm), PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_STOP);
        pi_pwm_ioctl(&(data->pwm), PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_START);
    }
}
#else
static int32_t __pi_uart_flow_control_enable(struct uart_itf_data_s *data)
{
    return -12;
}

static void __pi_uart_copy_exec_flow_control(struct uart_itf_data_s *data,
                                             struct pi_task *task)
{
    return;
}
#endif  /* UART_FLOW_CONTROL_EMU */

/* Execute transfer. */
static void __pi_uart_copy_exec(struct uart_itf_data_s *data,
                                struct pi_task *task)
{
    uint32_t device_id = data->device_id;
    uint32_t l2_buf = task->data[0];
    uint32_t size = task->data[1];
    udma_channel_e channel = task->data[2];
    uint32_t max_size = (uint32_t) UDMA_MAX_SIZE - 4;
    if (size > max_size)
    {
        size = max_size;
    }
    hal_compiler_barrier();
    task->data[0] += size;
    task->data[3] = task->data[1] - size;
    task->data[1] = task->data[3];
    UART_TRACE("UART(%ld): Execute %s transfer l2_buf=%lx size=%ld\n",
               device_id, ((channel == RX_CHANNEL) ? "RX" : "TX"), l2_buf, size);
    hal_uart_enqueue(device_id, l2_buf, size, 0, channel);
}

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

int32_t __pi_uart_open(struct uart_itf_data_s **device_data,
                       struct pi_uart_conf *conf)
{
    struct uart_itf_data_s *data = g_uart_itf_data[conf->uart_id];
    if (data == NULL)
    {
        /* Open device for first time. */
        data = (struct uart_itf_data_s *) pi_l2_malloc(sizeof(struct uart_itf_data_s));
        if (data == NULL)
        {
            UART_TRACE_ERR("Driver data alloc failed !\n");
            return -11;
        }
        memset((void *) data, 0, sizeof(struct uart_itf_data_s));
        data->device_id = conf->uart_id;
        data->nb_open = 1;
        data->baudrate_bps = conf->baudrate_bps;
        data->flow_ctrl_ena = 0;

        UART_TRACE("Device id=%ld opened for first time\n", data->device_id);
        UART_TRACE("uart(%ld) %p\n", data->device_id, uart(data->device_id));

        UART_TRACE("Enable SoC events and set handlers : RX : %d -> %p | TX: %d -> %p\n",
                   SOC_EVENT_UDMA_UART_RX(data->device_id), __pi_uart_handler,
                   SOC_EVENT_UDMA_UART_TX(data->device_id), __pi_uart_handler);
        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_RX(data->device_id), __pi_uart_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_TX(data->device_id), __pi_uart_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_RX(data->device_id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_TX(data->device_id));

        /* Disable UDMA CG and reset periph. */
        udma_ctrl_cg_disable(UDMA_UART_ID(data->device_id));
        UART_TRACE("Disable CG for uart(%ld): %ld %p %lx\n",
                   data->device_id, UDMA_UART_ID(data->device_id),
                   &(UDMA_GC->CG), UDMA_GC->CG);

        /* Configure UART. */
        __pi_uart_conf_set(data, conf);

        /* Control flow. */
        #if defined(UART_FLOW_CONTROL_EMU)
        if (conf->use_ctrl_flow)
        {
            data->flow_ctrl_ena = conf->use_ctrl_flow;
            if (__pi_uart_flow_control_enable(data))
            {
                UART_TRACE_ERR("UART flow control enable error !\n");
                return -12;
            }
        }
        #endif  /* UART_FLOW_CONTROL_EMU */

        /* Attach freq callback. */
        pi_freq_callback_init(&(data->uart_freq_cb), __pi_uart_freq_cb, data);
        pi_freq_callback_add(&(data->uart_freq_cb));

        g_uart_itf_data[data->device_id] = data;
        *device_data = g_uart_itf_data[data->device_id];
    }
    else
    {
        data->nb_open++;
        *device_data = g_uart_itf_data[data->device_id];
        UART_TRACE("Device id=%x already opened, now open=%d\n",
                   data->device_id, data->nb_open);
    }
    return 0;
}

void __pi_uart_close(struct uart_itf_data_s *data)
{
    if (data != NULL)
    {
        /* Decrement number of devices opened. */
        data->nb_open--;
        /* Free device and structure opened. */
        if (data->nb_open == 0)
        {
            /* Remove freq callback. */
            pi_freq_callback_remove(&(data->uart_freq_cb));

            /* Make sure all bits are transferred. */
            while (hal_uart_tx_status_get(data->device_id));

            /* Wait some time, until data are sent. */
            for (volatile uint32_t i=0; i<1000; i++);

            /* Disable both RX and TX channels and flush fifo. */
            __pi_uart_rx_abort(data);
            __pi_uart_tx_abort(data);

            /* Clear handlers. */
            pi_fc_event_handler_clear(SOC_EVENT_UDMA_UART_RX(data->device_id));
            pi_fc_event_handler_clear(SOC_EVENT_UDMA_UART_TX(data->device_id));
            /* Disable SOC events propagation to FC. */
            hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_UART_RX(data->device_id));
            hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_UART_TX(data->device_id));

            /* Free allocated data. */
            pi_l2_free(data, sizeof(struct uart_itf_data_s));
            g_uart_itf_data[data->device_id] = NULL;
        }
    }
}

int32_t __pi_uart_ioctl(struct uart_itf_data_s *data, uint32_t cmd, void *arg)
{
    uint32_t irq = disable_irq();
    int32_t status = 0;
    switch(cmd)
    {
    case PI_UART_IOCTL_CONF_SETUP :
        __pi_uart_conf_set(data, (struct pi_uart_conf *) arg);
        break;

    case PI_UART_IOCTL_ABORT_RX :
        __pi_uart_rx_abort(data);
        break;

    case PI_UART_IOCTL_ABORT_TX :
        __pi_uart_tx_abort(data);
        break;

    case PI_UART_IOCTL_ENABLE_RX :
        __pi_uart_channel_enable(data, RX_CHANNEL);
        break;

    case PI_UART_IOCTL_ENABLE_TX :
        __pi_uart_channel_enable(data, TX_CHANNEL);
        break;

    case PI_UART_IOCTL_ENABLE_FLOW_CONTROL :
        status = __pi_uart_flow_control_enable(data);
        break;

    default :
        restore_irq(irq);
        return -1;
    }
    restore_irq(irq);
    return status;
}

int32_t __pi_uart_copy(struct uart_itf_data_s *data, uint32_t l2_buf,
                       uint32_t size, udma_channel_e channel,
                       struct pi_task *task)
{
    uint32_t irq = disable_irq();
    // Due to udma restriction, we need to use an L2 address,
    // Since the stack is probably in FC tcdm, we have to either ensure users gave
    // us an L2 pointer or alloc ourselves
    if ((l2_buf & 0xFFF00000) != 0x1C000000)
    {
        UART_TRACE_ERR("UART(%ld): Error wrong buffer %lx !\n",
                       data->device_id, l2_buf);
        restore_irq(irq);
        return -11;
    }

    task->data[0] = l2_buf;
    task->data[1] = data->flow_ctrl_ena ? 1 : size;
    task->data[2] = channel;
    task->data[3] = data->flow_ctrl_ena ? size : 0; /* Repeat size ? */
    task->data[4] = 0;
    task->next    = NULL;
    uint8_t head = __pi_uart_task_fifo_enqueue(data, task, channel);
    if (head == 0)
    {
        /* Execute the transfer. */
        UART_TRACE("UART(%ld): Execute %s transfer l2_buf=%lx size=%ld\n",
                   data->device_id, ((task->data[2] == RX_CHANNEL) ? "RX" : "TX"),
                   task->data[0], task->data[1]);
        if (data->flow_ctrl_ena)
        {
            __pi_uart_copy_exec_flow_control(data, task);
        }
        else
        {
            __pi_uart_copy_exec(data, task);
        }
    }
    restore_irq(irq);
    return 0;
}
