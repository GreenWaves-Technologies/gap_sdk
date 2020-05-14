/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#define __RT_UART_BAUDRATE 115200

#define UDMA_UART_MAX_SIZE 0x1FFFF

#if defined(UART_FLOW_CONTROL_EMU) && PULP_CHIP == CHIP_VEGA
#undef UART_FLOW_CONTROL_EMU
#endif

/*
 * Uart IRQ handler, written in ASM.
 * This function will call __pi_uart_handle_copy(), with pi_uart_t arg.
 */
extern void __pi_uart_handle_copy_asm();
void __pi_uart_handle_copy(int event, void *arg);

/*
 * Disable RX channel, abort current transfer,
 * flush event tasks linked to the channel.
 */
static void __pi_uart_rx_abort(pi_uart_t *uart);

/*
 * Disable TX channel, abort current transfer,
 * flush event tasks linked to the channel.
 */
static void __pi_uart_tx_abort(pi_uart_t *uart);

/* Enable flow control. */
static int __pi_uart_flow_control_enable(pi_uart_t *uart);

/* Enqueu transfer in UDMA with flow control. */
static void __pi_uart_copy_enqueue_exec_flow_control(pi_uart_t *uart,
                                                     struct pi_task *task);

/* Enqueu transfer in UDMA. */
static void __pi_uart_copy_enqueue_exec(pi_uart_t *uart, struct pi_task *task);

/* Enqueue transfer in UDMA or in SW fifo. */
static void __pi_uart_copy_enqueue(pi_uart_t *uart, uint32_t l2_buf, uint32_t size,
                                   uint8_t channel, struct pi_task *task);


L2_DATA static pi_uart_t __rt_uart[ARCHI_UDMA_NB_UART];

void __pi_uart_handle_copy(int event, void *arg)
{
  pi_uart_t *uart = (pi_uart_t *) arg;
  int channel = event & 0x1;
  rt_udma_channel_t *udma_chan = (channel) ? &(uart->tx_channel) : &(uart->rx_channel);
  struct pi_task *task = udma_chan->pendings[0];

  if (task->implem.data[3])
  {
    if (uart->conf.use_ctrl_flow)
    {
      __pi_uart_copy_enqueue_exec_flow_control(uart, task);
    }
    else
    {
      __pi_uart_copy_enqueue_exec(uart, task);
    }
  }
  else
  {
    __rt_event_handle_end_of_task(task);
    task = udma_chan->waitings_first;
    udma_chan->pendings[0] = task;
    rt_compiler_barrier();
    if (task != NULL)
    {
      udma_chan->waitings_first = udma_chan->waitings_first->implem.next;
      if (udma_chan->waitings_first == NULL)
      {
        udma_chan->waitings_last = NULL;
      }
      if (uart->conf.use_ctrl_flow)
      {
        __pi_uart_copy_enqueue_exec_flow_control(uart, task);
      }
      else
      {
        __pi_uart_copy_enqueue_exec(uart, task);
      }
    }
  }
}


#if defined(UART_FLOW_CONTROL_EMU)
static void __pi_uart_copy_enqueue_exec_flow_control(pi_uart_t *uart,
                                                     struct pi_task *task)
{
  unsigned int base = task->implem.data[2];
  unsigned int l2_buf = task->implem.data[0] + task->implem.data[4];
  unsigned int size = 1;//task->implem.data[1];
  unsigned int cfg = UDMA_CHANNEL_CFG_SIZE_8 | UDMA_CHANNEL_CFG_EN;
  unsigned int channel = task->implem.data[5];
  rt_compiler_barrier();
  task->implem.data[4]++;       /* update l2 buffer pointer. */
  task->implem.data[3]--;       /* update remaining size. */
  if (channel == 1)
  {
    /* Wait for CTS. */
    uint32_t value = 0;
    value = pi_gpio_pin_read(NULL, PI_GPIO_A1_PAD_9_B3, &value);
    while (value == 1)
    {
      value = pi_gpio_pin_read(NULL, PI_GPIO_A1_PAD_9_B3, &value);
    }
  }
  plp_udma_enqueue(base, l2_buf, size, cfg);
  if (channel == 0)
  {
    /* Send RTS. */
    pi_pwm_ioctl(&(uart->pwm), PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_STOP);
    pi_pwm_ioctl(&(uart->pwm), PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_START);
  }
}
#else
static void __pi_uart_copy_enqueue_exec_flow_control(pi_uart_t *uart,
                                                     struct pi_task *task)
{
  return;
}
#endif  /* UART_FLOW_CONTROL_EMU */

static void __pi_uart_copy_enqueue_exec(pi_uart_t *uart, struct pi_task *task)
{
  //unsigned int base = hal_udma_channel_base(uart->channel + task->implem.data[2]);
  unsigned int base = task->implem.data[2];
  unsigned int l2_buf = task->implem.data[0];
  unsigned int size = task->implem.data[1];
  unsigned int cfg = UDMA_CHANNEL_CFG_SIZE_8 | UDMA_CHANNEL_CFG_EN;
  uint32_t max_size = (uint32_t) UDMA_UART_MAX_SIZE - 4;
  if (size > max_size)
  {
    size = max_size;
  }
  rt_compiler_barrier();
  task->implem.data[0] += size;        /* update l2 buffer pointer */
  task->implem.data[3] = task->implem.data[1] - size; /* update remaining size */
  task->implem.data[1] = task->implem.data[3];        /* next size to enqueue */
  plp_udma_enqueue(base, l2_buf, size, cfg);
}

/* channel: rx=0, tx=1. */
static void __pi_uart_copy_enqueue(pi_uart_t *uart, uint32_t l2_buf, uint32_t size,
                                   uint8_t channel, struct pi_task *task)
{
  int irq = disable_irq();
  task->implem.data[0] = l2_buf; /* l2 buffer */
  task->implem.data[1] = uart->conf.use_ctrl_flow ? 1 : size; /* size to enqueue */
  task->implem.data[2] = hal_udma_channel_base(uart->channel + channel); /* periph base(rx or tx) */
  task->implem.data[3] = uart->conf.use_ctrl_flow ? size : 0; /* repeat size ? */
  task->implem.data[4] = 0;       /* l2 buffer pointer offset */
  task->implem.data[5] = channel;                             /* channel(rx or tx) */
  task->implem.next = NULL;

  rt_udma_channel_t *udma_chan = (channel) ? &(uart->tx_channel) : &(uart->rx_channel);
  if (udma_chan->pendings[0] == NULL)
  {
    udma_chan->pendings[0] = task;
    if (uart->conf.use_ctrl_flow)
    {
      __pi_uart_copy_enqueue_exec_flow_control(uart, task);
    }
    else
    {
      __pi_uart_copy_enqueue_exec(uart, task);
    }
  }
  else
  {
    if (udma_chan->waitings_first == NULL)
    {
      udma_chan->waitings_first = task;
    }
    else
    {
      udma_chan->waitings_last->implem.next = task;
    }
    udma_chan->waitings_last = task;
  }
  restore_irq(irq);
}


void pi_uart_conf_init(struct pi_uart_conf *conf)
{
  conf->baudrate_bps = __RT_UART_BAUDRATE;
  conf->stop_bit_count = PI_UART_STOP_BITS_ONE;
  conf->parity_mode = PI_UART_PARITY_DISABLE;
  conf->word_size = PI_UART_WORD_SIZE_8_BITS;
  conf->enable_rx = 1;
  conf->enable_tx = 1;
  conf->uart_id = 0;
  conf->use_ctrl_flow = 0;
}



static void __rt_uart_wait_tx_done(pi_uart_t *uart)
{
  // Wait for all pending transfers to finish
  while (plp_udma_busy(UDMA_UART_TX_ADDR(uart->channel - ARCHI_UDMA_UART_ID(0))))
  {
    rt_wait_for_interrupt();
  }

  // And flush the uart to make sure no bit is transfered anymore
  while(plp_uart_tx_busy(uart->channel - ARCHI_UDMA_UART_ID(0)));

#if 1

// There is a bug in the uart, between 2 bytes, the uart says it is not busy
// and so if we are not lucky, we can continue while the uart is actually
// still busy. Instead, wait for a few clock refs

#ifdef ITC_VERSION
  for (int i=0; i<50; i++)
  {
    rt_irq_clr(1<<ARCHI_FC_EVT_CLK_REF);
    rt_irq_mask_set(1<<ARCHI_FC_EVT_CLK_REF);
    rt_wait_for_interrupt();
    rt_irq_mask_clr(1<<ARCHI_FC_EVT_CLK_REF);
  }
#else
  volatile int i;
  for (i=0; i<2000; i++);
#endif

#endif
}



static void __rt_uart_setup(pi_uart_t *uart)
{
  int baudrate = uart->conf.baudrate_bps;
  int div =  (__rt_freq_periph_get() + baudrate/2) / baudrate;

  // The counter in the UDMA will count from 0 to div included
  // and then will restart from 0, so we must give div - 1 as
  // divider
  plp_uart_setup_set(uart->channel - ARCHI_UDMA_UART_ID(0),
                     uart->conf.parity_mode,
                     uart->conf.word_size,
                     uart->conf.stop_bit_count,
                     uart->conf.enable_tx,
                     uart->conf.enable_rx,
                     div-1);
}



static int __rt_uart_setfreq_before(void *arg)
{
  // Before changing the periph frequency, we need to handle the pending buffers
  // as a frequency change in the middle of a transfer would corrupt a few data
  // Here is the procedure to handle that correctly:
  //   - We wait until all pending transfers are finished. The only way
  //     is to block the core execution. It is important that no one reenqueue another transfer
  //     meanwhile as we cannot stop the uart while there is at least one transfer in the udma.
  //     This is ensured by the fact we don't enqueue udma transfers from interrupt handlers and
  //     we block core execution.
  //   - We stop the uart, this way other transfers can be enqueued into the udma, the uart will not
  //     try to send them.
  //   - We leave the function and the fll driver will then apply the new frequency.
  //   - When we enter the other callback after the frequency is changed, we activate back the uart
  //     with the new frequency. Pending udma transfers will then start being handled.

  for (int i=0; i<ARCHI_UDMA_NB_UART; i++)
  {
    pi_uart_t *uart = &__rt_uart[i];
    if (uart->open_count) {
      // First wait for pending transfers to finish
      __rt_uart_wait_tx_done(uart);

      // Then deactivate the uart
      plp_uart_disable(uart->channel - ARCHI_UDMA_UART_ID(0));
    }
  }

  return 0;
}



static int __rt_uart_setfreq_after(void *arg)
{
  // Now apply the new frequency to all channels
  // This will also reactive the uart
  for (int i=0; i<ARCHI_UDMA_NB_UART; i++)
  {
    pi_uart_t *uart = &__rt_uart[i];
    if (uart->open_count)
    {
      __rt_uart_setup(uart);
    }
  }
  return 0;
}


int pi_uart_open(struct pi_device *device)
{
  int irq = rt_irq_disable();

  struct pi_uart_conf *conf = (struct pi_uart_conf *)device->config;

  int uart_id = conf->uart_id;
  int periph_id = ARCHI_UDMA_UART_ID(uart_id);
  int channel = UDMA_EVENT_ID(periph_id);
  int baudrate = conf->baudrate_bps;

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (id: %d, baudrate: %d)\n", uart_id, baudrate);

  pi_uart_t *uart = &__rt_uart[uart_id];

  device->data = (void *)uart;

  if (uart->open_count)
  {
    return -1;
  }

  uart->open_count++;
  //uart->baudrate = baudrate;
  memcpy(&(uart->conf), conf, sizeof(struct pi_uart_conf));
  uart->channel = channel;
  uart->tx_channel.pendings[0] = NULL;
  uart->tx_channel.pendings[1] = NULL;
  uart->tx_channel.waitings_first = NULL;
  uart->tx_channel.waitings_last = NULL;

  // First activate uart device
  plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));

  soc_eu_fcEventMask_setEvent(channel);
  soc_eu_fcEventMask_setEvent(channel+1);

  // Redirect all UDMA cpi events to the standard callback
  //printf("callback arg: uart %p\n", uart);
  __rt_udma_register_channel_callback(channel, __pi_uart_handle_copy_asm, (void *) uart);
  __rt_udma_register_channel_callback(channel+1, __pi_uart_handle_copy_asm, (void *) uart);

  // Then set it up
  __rt_uart_setup(uart);

  if (uart->conf.use_ctrl_flow)
  {
    if (__pi_uart_flow_control_enable(uart))
    {
      printf("Error enabling flow control\n");
      return -2;
    }
  }

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Successfully opened uart device (handle: %p)\n", uart);

  rt_irq_restore(irq);

  return 0;
}




void pi_uart_close(struct pi_device *device)
{
  int irq = rt_irq_disable();

  pi_uart_t *uart = (pi_uart_t *)device->data;


  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Closing uart device (handle: %p)\n", uart);

  uart->open_count--;

  // First wait for pending transfers to finish before stoppping uart in case
  // some printf are still pending
  __rt_uart_wait_tx_done(uart);

  // Set enable bits for uart channel back to 0
  // This is needed to be able to propagate new configs when re-opening
  __pi_uart_tx_abort(uart);
  __pi_uart_rx_abort(uart);

  // Then stop the uart
  plp_udma_cg_set(plp_udma_cg_get() & ~(1<<uart->channel));

  rt_irq_restore(irq);
}



#if defined(ARCHI_HAS_CLUSTER)

static void __rt_uart_cluster_req_done(void *_req)
{
  pi_cl_uart_req_t *req = _req;
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}

static void __rt_uart_cluster_req(void *_req)
{
  pi_cl_uart_req_t *req = _req;
  pi_task_t *event = &req->event;
  pi_task_callback(event, __rt_uart_cluster_req_done, (void* )req);
  pi_uart_write_async(req->device, req->buffer, req->size, event);
}

int pi_cl_uart_write(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
  req->device = device;
  req->buffer = buffer;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  __rt_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __rt_uart_cluster_req, (void* )req);
  __rt_cluster_push_fc_event(&req->event);

  return 0;
}

#endif


int pi_uart_write_async(struct pi_device *device, void *buffer, uint32_t size, pi_task_t *task)
{
  __rt_task_init(task);
  pi_uart_t *uart = (pi_uart_t *)device->data;
  __pi_uart_copy_enqueue(uart, (uint32_t) buffer, size, 1, task);
  return 0;
}



int pi_uart_read_async(struct pi_device *device, void *buffer, uint32_t size, pi_task_t *task)
{
  __rt_task_init(task);
  pi_uart_t *uart = (pi_uart_t *)device->data;
  __pi_uart_copy_enqueue(uart, (uint32_t) buffer, size, 0, task);
  return 0;
}



int pi_uart_write(struct pi_device *device, void *buffer, uint32_t size)
{
  pi_task_t task;
  if (pi_uart_write_async(device, buffer, size, pi_task_block(&task)))
    return -1;
  pi_task_wait_on(&task);
  return 0;
}



int pi_uart_read(struct pi_device *device, void *buffer, uint32_t size)
{
  pi_task_t task;
  if (pi_uart_read_async(device, buffer, size, pi_task_block(&task)))
    return -1;
  pi_task_wait_on(&task);
  return 0;
}

int pi_uart_write_byte(pi_device_t *device, uint8_t *byte)
{
  int ret = pi_uart_write(device, byte, 1);
  return ret;
}

int pi_uart_write_byte_async(pi_device_t *device, uint8_t *byte, pi_task_t *callback)
{
  return pi_uart_write_async(device, byte, 1, callback);
}

/* Ioctl functions. */
static void __pi_uart_conf_set(pi_uart_t *uart, struct pi_uart_conf *conf)
{
  memcpy(&(uart->conf), conf, sizeof(struct pi_uart_conf));
  __rt_uart_setup(uart);
}

static void __pi_uart_rx_abort(pi_uart_t *uart)
{
  plp_uart_rx_disable(uart->channel - ARCHI_UDMA_UART_ID(0));
  plp_uart_rx_clr(uart->channel - ARCHI_UDMA_UART_ID(0));
  /* Clear event tasks. */
  uart->rx_channel.pendings[0] = NULL;
  uart->rx_channel.pendings[1] = NULL;
  uart->rx_channel.waitings_first = NULL;
  uart->rx_channel.waitings_last = NULL;
}

static void __pi_uart_tx_abort(pi_uart_t *uart)
{
  plp_uart_tx_disable(uart->channel - ARCHI_UDMA_UART_ID(0));
  plp_uart_tx_clr(uart->channel - ARCHI_UDMA_UART_ID(0));
  /* Clear event tasks. */
  uart->tx_channel.pendings[0] = NULL;
  uart->tx_channel.pendings[1] = NULL;
  uart->tx_channel.waitings_first = NULL;
  uart->tx_channel.waitings_last = NULL;
}

static void __pi_uart_rx_enable(pi_uart_t *uart)
{
  plp_uart_rx_enable(uart->channel - ARCHI_UDMA_UART_ID(0));
}

static void __pi_uart_tx_enable(pi_uart_t *uart)
{
  plp_uart_tx_enable(uart->channel - ARCHI_UDMA_UART_ID(0));
}

#if defined(UART_FLOW_CONTROL_EMU)
static int __pi_uart_flow_control_enable(pi_uart_t *uart)
{
  int status = 0;
  struct pi_device *pwm = &(uart->pwm);
  struct pi_pwm_conf pwm_conf = {0};
  pi_pwm_conf_init(&pwm_conf);
  pwm_conf.pwm_id = 0;
  pwm_conf.ch_id = PI_PWM_CHANNEL2;
  pwm_conf.timer_conf = PI_PWM_EVT_FALL | PI_PWM_CLKSEL_FLL | PI_PWM_UPDOWNSEL_ALT;
  pi_open_from_conf(pwm, &pwm_conf);
  status = pi_pwm_open(pwm);
  if (status)
  {
    printf("Error opening PWM device : %d\n", status);
    return -11;
  }
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
  uart->conf.use_ctrl_flow = 1;
  return status;
}
#else
static int __pi_uart_flow_control_enable(pi_uart_t *uart)
{
  return -12;
}
#endif  /* UART_FLOW_CONTROL_EMU */

int pi_uart_ioctl(pi_device_t *device, uint32_t cmd, void *arg)
{
  pi_uart_t *uart = (pi_uart_t *) device->data;
  switch(cmd)
  {
  case PI_UART_IOCTL_CONF_SETUP :
    __pi_uart_conf_set(uart, (struct pi_uart_conf *) arg);
    break;

  case PI_UART_IOCTL_ABORT_RX :
    __pi_uart_rx_abort(uart);
    break;

  case PI_UART_IOCTL_ABORT_TX :
    __pi_uart_tx_abort(uart);
    break;

  case PI_UART_IOCTL_ENABLE_RX :
    __pi_uart_rx_enable(uart);
    break;

  case PI_UART_IOCTL_ENABLE_TX :
    __pi_uart_tx_enable(uart);
    break;

  case PI_UART_IOCTL_ENABLE_FLOW_CONTROL :
    return __pi_uart_flow_control_enable(uart);

  default :
    return -1;
  }
  return 0;
}


RT_FC_BOOT_CODE void __attribute__((constructor)) __pi_uart_init()
{
  // In case the peripheral clock can dynamically change, we need to be notified
  // when this happens so that we flush pending transfers before updating the frequency
  int err = 0;

  err |= __rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_BEFORE, __rt_uart_setfreq_before, NULL);

  err |= __rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_AFTER, __rt_uart_setfreq_after, NULL);


  for (int i=0; i<ARCHI_UDMA_NB_UART; i++)
  {
    __rt_uart[i].open_count = 0;
    __rt_udma_channel_init(UDMA_EVENT_ID(ARCHI_UDMA_UART_ID(i))+1, &__rt_uart[i].tx_channel);
    __rt_udma_channel_init(UDMA_EVENT_ID(ARCHI_UDMA_UART_ID(i)), &__rt_uart[i].rx_channel);
  }

  if (err) rt_fatal("Unable to initialize uart driver\n");
}


#ifdef __ZEPHYR__

#include <zephyr.h>
#include <device.h>
#include <init.h>

static int uart_init(struct device *device)
{
  ARG_UNUSED(device);

  __rt_uart_init();

  return 0;
}

struct uart_config {
};

struct uart_data {
};

static const struct uart_config uart_cfg = {
};

static struct uart_data uart_data = {
};

DEVICE_INIT(uart, "uart", &uart_init,
    &uart_data, &uart_cfg,
    PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif
