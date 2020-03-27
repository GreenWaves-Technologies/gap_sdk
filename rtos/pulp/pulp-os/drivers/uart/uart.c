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


/*
 * Disable RX channel, abort current transfer,
 * flush event tasks linked to the channel.
 */
static void __rt_uart_rx_abort(pi_device_t *device);

/*
 * Disable TX channel, abort current transfer,
 * flush event tasks linked to the channel.
 */
static void __rt_uart_tx_abort(pi_device_t *device);


L2_DATA static pi_uart_t __rt_uart[ARCHI_UDMA_NB_UART];



void pi_uart_conf_init(struct pi_uart_conf *conf)
{
  conf->baudrate_bps = __RT_UART_BAUDRATE;
  conf->stop_bit_count = PI_UART_STOP_BITS_ONE;
  conf->parity_mode = PI_UART_PARITY_DISABLE;
  conf->word_size = PI_UART_WORD_SIZE_8_BITS;
  conf->enable_rx = 1;
  conf->enable_tx = 1;
  conf->uart_id = 0;
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

  // First activate uart device
  plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));

  soc_eu_fcEventMask_setEvent(channel);
  soc_eu_fcEventMask_setEvent(channel+1);

    // Redirect all UDMA cpi events to the standard callback
    __rt_udma_register_channel_callback(channel, __rt_udma_handle_copy, (void *)uart);
    __rt_udma_register_channel_callback(channel+1, __rt_udma_handle_copy, (void *)uart);

  // Then set it up
  __rt_uart_setup(uart);

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
  __rt_uart_tx_abort(device);
  __rt_uart_rx_abort(device);

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
  __rt_udma_copy_enqueue(task, uart->channel + 1, &uart->tx_channel, (uint32_t)buffer, size, UDMA_CHANNEL_CFG_SIZE_8);
  return 0;
}



int pi_uart_read_async(struct pi_device *device, void *buffer, uint32_t size, pi_task_t *task)
{
  __rt_task_init(task);
  pi_uart_t *uart = (pi_uart_t *)device->data;
  __rt_udma_copy_enqueue(task, uart->channel, &uart->rx_channel, (uint32_t)buffer, size, UDMA_CHANNEL_CFG_SIZE_8);
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
static void __rt_uart_conf_set(pi_device_t *device, struct pi_uart_conf *conf)
{
  pi_uart_t *uart = (pi_uart_t *) device->data;

  memcpy(&(uart->conf), conf, sizeof(struct pi_uart_conf));
  __rt_uart_setup(uart);
}

static void __rt_uart_rx_abort(pi_device_t *device)
{
  pi_uart_t *uart = (pi_uart_t *) device->data;
  plp_uart_rx_disable(uart->channel - ARCHI_UDMA_UART_ID(0));
  plp_uart_rx_clr(uart->channel - ARCHI_UDMA_UART_ID(0));
  /* Clear event tasks. */
  uart->rx_channel.pendings[0] = NULL;
  uart->rx_channel.pendings[1] = NULL;
  uart->rx_channel.waitings_first = NULL;
  uart->rx_channel.waitings_last = NULL;
}

static void __rt_uart_tx_abort(pi_device_t *device)
{
  pi_uart_t *uart = (pi_uart_t *) device->data;
  plp_uart_tx_disable(uart->channel - ARCHI_UDMA_UART_ID(0));
  plp_uart_tx_clr(uart->channel - ARCHI_UDMA_UART_ID(0));
  /* Clear event tasks. */
  uart->tx_channel.pendings[0] = NULL;
  uart->tx_channel.pendings[1] = NULL;
  uart->tx_channel.waitings_first = NULL;
  uart->tx_channel.waitings_last = NULL;
}

static void __rt_uart_rx_enable(pi_device_t *device)
{
  pi_uart_t *uart = (pi_uart_t *) device->data;
  plp_uart_rx_enable(uart->channel - ARCHI_UDMA_UART_ID(0));
}

static void __rt_uart_tx_enable(pi_device_t *device)
{
  pi_uart_t *uart = (pi_uart_t *) device->data;
  plp_uart_tx_enable(uart->channel - ARCHI_UDMA_UART_ID(0));
}

int pi_uart_ioctl(pi_device_t *device, uint32_t cmd, void *arg)
{
  switch(cmd)
  {
  case PI_UART_IOCTL_CONF_SETUP :
    __rt_uart_conf_set(device, (struct pi_uart_conf *) arg);
    break;

  case PI_UART_IOCTL_ABORT_RX :
    __rt_uart_rx_abort(device);
    break;

  case PI_UART_IOCTL_ABORT_TX :
    __rt_uart_tx_abort(device);
    break;

  case PI_UART_IOCTL_ENABLE_RX :
    __rt_uart_rx_enable(device);
    break;

  case PI_UART_IOCTL_ENABLE_TX :
    __rt_uart_tx_enable(device);
    break;

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
