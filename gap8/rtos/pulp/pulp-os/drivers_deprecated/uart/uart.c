/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

#define __RT_UART_BAUDRATE 625000



static rt_uart_t __rt_uart[ARCHI_UDMA_NB_UART];



void rt_uart_conf_init(rt_uart_conf_t *conf)
{
  conf->baudrate = __RT_UART_BAUDRATE;
  conf->itf = -1;
}



static void __rt_uart_wait_tx_done(rt_uart_t *uart)
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



static void __rt_uart_setup(rt_uart_t *uart)
{
  int div =  (__rt_freq_periph_get() + uart->baudrate/2) / uart->baudrate;

  // The counter in the UDMA will count from 0 to div included
  // and then will restart from 0, so we must give div - 1 as
  // divider
  plp_uart_setup(uart->channel - ARCHI_UDMA_UART_ID(0), 0, div-1);
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
    rt_uart_t *uart = &__rt_uart[i];
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
    rt_uart_t *uart = &__rt_uart[i];
    if (uart->open_count)
    {
      __rt_uart_setup(uart);
    }
  }
  return 0;
}


rt_uart_t* __rt_uart_open(int channel, rt_uart_conf_t *conf, rt_event_t *event, char *name)
{
  int irq = rt_irq_disable();
  
  int baudrate = __RT_UART_BAUDRATE;
  if (conf) baudrate = conf->baudrate;

  if (name) rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (name: %s, baudrate: %d)\n", name, baudrate);
  else rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (channel: %d, baudrate: %d)\n", channel, baudrate);

  rt_uart_t *uart = &__rt_uart[channel - ARCHI_UDMA_UART_ID(0)];

  // In case the uart is already openened, return the same handle if the 
  // configuration is the same or return an error
  if (uart->open_count)
  {
    if (conf && conf->baudrate != uart->baudrate) return NULL;
    uart->open_count++;
    return uart;
  }

  uart->open_count++;
  uart->baudrate = baudrate;
  uart->channel = channel;

  // First activate uart device
  plp_udma_cg_set(plp_udma_cg_get() | (1<<channel));

  soc_eu_fcEventMask_setEvent(UDMA_EVENT_ID(channel));
  soc_eu_fcEventMask_setEvent(UDMA_EVENT_ID(channel)+1);

  // Then set it up
  __rt_uart_setup(uart);

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Successfully opened uart device (handle: %p)\n", uart);

  rt_irq_restore(irq);

  return uart;
}


rt_uart_t* rt_uart_open(char *dev_name, rt_uart_conf_t *conf, rt_event_t *event)
{

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (name: %s)\n", dev_name);

  rt_uart_conf_t def_conf;

  int channel;

  if (conf == NULL)
  {
    conf = &def_conf;
    rt_uart_conf_init(conf);
  }

  if (conf->itf != -1)
  { 
    channel = conf->itf + ARCHI_UDMA_UART_ID(0);
  }
  else
  {
    rt_dev_t *dev = rt_dev_get(dev_name);
    if (dev == NULL) goto error;
    channel = dev->channel;
  }

  return __rt_uart_open(channel, conf, event, dev_name);
  
error:
  rt_warning("[UART] Failed to open uart device\n");
  return NULL;
}



void rt_uart_close(rt_uart_t *uart, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Closing uart device (handle: %p)\n", uart);

  uart->open_count--;

  if (uart->open_count == 0)
  {
      // First wait for pending transfers to finish before stoppping uart in case
      // some printf are still pending
      __rt_uart_wait_tx_done(uart);

      // Set enable bits for uart channel back to 0 
      // This is needed to be able to propagate new configs when re-opening
      plp_uart_disable(uart->channel - ARCHI_UDMA_UART_ID(0));      

      // Then stop the uart
      plp_udma_cg_set(plp_udma_cg_get() & ~(1<<uart->channel));
  }

  rt_irq_restore(irq);
}

void __rt_uart_cluster_req_done(void *_req)
{
  int irq = rt_irq_disable();
  rt_uart_req_t *req = _req;
  req->done = 1;
#if defined(ARCHI_HAS_CLUSTER)
  __rt_cluster_notif_req_done(req->cid);
#endif
  rt_irq_restore(irq);
}

#if defined(ARCHI_HAS_CLUSTER)

static void __rt_uart_cluster_req(void *_req)
{
  int irq = rt_irq_disable();
  rt_uart_req_t *req = _req;
  rt_event_t *event = &req->event;
  __rt_init_event(event, rt_event_internal_sched(), __rt_uart_cluster_req_done, (void *)req);
  __rt_event_set_pending(event);
  rt_uart_write(req->uart, req->buffer, req->size, event);
  rt_irq_restore(irq);
}

void rt_uart_cluster_write(rt_uart_t *handle, void *buffer, size_t size, rt_uart_req_t *req)
{
  req->uart = handle;
  req->buffer = buffer;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_uart_cluster_req, (void *)req);
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

#endif


RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_uart_init()
{
  // In case the peripheral clock can dynamically change, we need to be notified
  // when this happens so that we flush pending transfers before updating the frequency
  int err = 0;

  err |= __rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_BEFORE, __rt_uart_setfreq_before, NULL);

  err |= __rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_AFTER, __rt_uart_setfreq_after, NULL);


  for (int i=0; i<ARCHI_UDMA_NB_UART; i++)
  {
    __rt_uart[i].open_count = 0;
  }

  if (err) rt_fatal("Unable to initialize uart driver\n");
}
