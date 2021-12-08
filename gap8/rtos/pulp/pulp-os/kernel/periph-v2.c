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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "string.h"

RT_FC_TINY_DATA void *__rt_udma_extra_callback[ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT];
RT_FC_TINY_DATA void *__rt_udma_extra_callback_arg[ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT];
RT_FC_TINY_DATA rt_periph_channel_t periph_channels[ARCHI_NB_PERIPH*2];
volatile unsigned int __rt_socevents_status[2];

#if defined(ARCHI_UDMA_HAS_HYPER)
static inline __attribute__((always_inline)) void handle_hyper_copy(rt_periph_channel_t *channel, unsigned int base, rt_periph_copy_t *copy, unsigned int addr,
  unsigned int size, unsigned int cfg)
{
  int is_first = channel->first == NULL;

  __rt_channel_push(channel, copy);

  if (is_first) {
      hal_hyper_enqueue(base, addr, copy->u.hyper.hyper_addr, size, cfg);
  } else {
    copy->addr = addr;
    copy->size = size;
    copy->cfg = cfg;
    copy->enqueue_callback = 0;
    if (channel->firstToEnqueue == NULL) channel->firstToEnqueue = copy;
  }
}
#endif

static inline __attribute__((always_inline)) void __rt_handle_special_copy(rt_periph_channel_t *channel, unsigned int base, rt_periph_copy_t *copy, int addr, int size, int cfg)
{
  switch (ARCHI_REG_FIELD_GET(copy->ctrl, RT_PERIPH_COPY_CTRL_TYPE_BIT, RT_PERIPH_COPY_CTRL_TYPE_WIDTH)) {
#if defined(ARCHI_UDMA_HAS_HYPER)
    case RT_PERIPH_COPY_HYPER:       handle_hyper_copy(channel, base, copy, addr, size, cfg); break;
#endif
  }
}

void rt_periph_copy(rt_periph_copy_t *copy, int channel_id, unsigned int addr, int size,
  unsigned int cfg, rt_event_t *event)
{
  rt_trace(RT_TRACE_UDMA_COPY, "[UDMA] Enqueueing UDMA copy (node: 0x%x, l2Addr: 0x%x, size: 0x%x, channelId: %d)\n", (int)copy, addr, size, channel_id);

  int irq = rt_irq_disable();

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  unsigned int base = hal_udma_channel_base(channel_id);

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  if (copy == NULL) {
    copy = &call_event->implem.copy;
    rt_periph_copy_init(copy, 0);
  }

  cfg |= UDMA_CHANNEL_CFG_EN;
  copy->size = size;
  copy->event = call_event;

  if (ARCHI_REG_FIELD_GET(copy->ctrl, RT_PERIPH_COPY_CTRL_TYPE_BIT, RT_PERIPH_COPY_CTRL_TYPE_WIDTH) < RT_PERIPH_COPY_SPECIAL_ENQUEUE_THRESHOLD) {
    // If less than 2 transfers are enqueued in the channel, we can directly enqueue it
    // Otherwise enqueue it in the SW queue, it will be handled later on by the interrupt handler
    // We have to check if there is no transfer already waiting as since we masked interrupts, the
    // UDMA could have finished one transfer and we want to keep the transfers in-order
    __rt_channel_push(channel, copy);

    if (!channel->firstToEnqueue && plp_udma_canEnqueue(base)) {
      plp_udma_enqueue(base, addr, size, cfg);
    } else {
      copy->enqueue_callback = 0;
      __rt_channel_enqueue(channel, copy, addr, size, cfg);
    }
  } else {
    __rt_handle_special_copy(channel, base, copy, addr, size, cfg);
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



void rt_periph_single_copy(rt_periph_copy_t *copy, int channel_id, unsigned int addr, int size,
  unsigned int cfg, rt_event_t *event)
{
  rt_trace(RT_TRACE_UDMA_COPY, "[UDMA] Enqueueing UDMA copy (node: 0x%x, l2Addr: 0x%x, size: 0x%x, channelId: %d)\n", (int)copy, addr, size, channel_id);

  int irq = rt_irq_disable();

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  unsigned int base = hal_udma_channel_base(channel_id);

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  if (copy == NULL) {
    copy = &call_event->implem.copy;
    rt_periph_copy_init(copy, 0);
  }

  cfg |= UDMA_CHANNEL_CFG_EN;
  copy->size = size;
  copy->event = call_event;

  int is_first = channel->first == NULL;

  __rt_channel_push(channel, copy);

  if (is_first) {
      plp_udma_enqueue(base, addr, size, cfg);
  } else {
      copy->enqueue_callback = 0;
    __rt_channel_enqueue(channel, copy, addr, size, cfg);
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



void rt_periph_dual_copy_safe(rt_periph_copy_t *copy, int rx_channel_id,
  unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg)
{
  rt_trace(RT_TRACE_UDMA_COPY, "[UDMA] Enqueueing UDMA copy (node: 0x%x, tx_l2Addr: 0x%x, tx_size: 0x%x, rx_l2Addr: 0x%x, rx_size: 0x%x, rx_channel_id: %d)\n", (int)copy, tx_addr, tx_size, rx_addr, rx_size, rx_channel_id);

  // Dual copies assumes that more copies are always pushed to the TX channels than
  // to the RX channel, as an RX copy always needs a header in the TX channel.
  // Thus only the TX channel is checked to know if the transfer can be enqueued
  // directly.

  // The copy is enqueued to the RX channel so that the event is notified when the receive
  // is done.
  rt_periph_channel_t *channel = __rt_periph_channel(rx_channel_id);
  unsigned int periph_base = hal_udma_periph_base(rx_channel_id >> 1);
  unsigned int tx_base = periph_base + UDMA_CHANNEL_TX_OFFSET;
  unsigned int rx_base = periph_base + UDMA_CHANNEL_RX_OFFSET;

  cfg |= UDMA_CHANNEL_CFG_EN;
  copy->size = tx_size;

  __rt_channel_push(channel, copy);

  // If less than 2 transfers are enqueued in the channel, we can directly enqueue it
  // Otherwise enqueue it in the SW queue, it will be handled later on by the interrupt handler
  // We have to check if there is no transfer already waiting as since we masked interrupts, the
  // UDMA could have finished one transfer and we want to keep the transfers in-order
  if (likely(!channel->firstToEnqueue && plp_udma_canEnqueue(tx_base)))
  {
    plp_udma_enqueue(rx_base, rx_addr, rx_size, cfg);
    plp_udma_enqueue(tx_base, tx_addr, tx_size, cfg);
  } else {
    copy->u.dual.rx_addr = rx_addr;
    copy->u.dual.rx_size = rx_size;
    copy->enqueue_callback = 0;
    __rt_channel_enqueue(channel, copy, tx_addr, tx_size, cfg);
  }

}

void __rt_periph_dual_copy_noshadow_safe(rt_periph_copy_t *copy, int rx_channel_id,
  unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg)
{
  rt_trace(RT_TRACE_UDMA_COPY, "[UDMA] Enqueueing UDMA copy (node: 0x%x, tx_l2Addr: 0x%x, tx_size: 0x%x, rx_l2Addr: 0x%x, rx_size: 0x%x, rx_channel_id: %d)\n", (int)copy, tx_addr, tx_size, rx_addr, rx_size, rx_channel_id);

  // Dual copies assumes that more copies are always pushed to the TX channels than
  // to the RX channel, as an RX copy always needs a header in the TX channel.
  // Thus only the TX channel is checked to know if the transfer can be enqueued
  // directly.

  // The copy is enqueued to the RX channel so that the event is notified when the receive
  // is done.
  rt_periph_channel_t *channel = __rt_periph_channel(rx_channel_id);
  unsigned int periph_base = hal_udma_periph_base(rx_channel_id >> 1);
  unsigned int tx_base = periph_base + UDMA_CHANNEL_TX_OFFSET;
  unsigned int rx_base = periph_base + UDMA_CHANNEL_RX_OFFSET;

  cfg |= UDMA_CHANNEL_CFG_EN;
  copy->size = tx_size;

  __rt_channel_push(channel, copy);

  // If less than 2 transfers are enqueued in the channel, we can directly enqueue it
  // Otherwise enqueue it in the SW queue, it will be handled later on by the interrupt handler
  // We have to check if there is no transfer already waiting as since we masked interrupts, the
  // UDMA could have finished one transfer and we want to keep the transfers in-order
  if (likely(!channel->firstToEnqueue && !plp_udma_busy(tx_base)))
  {
    plp_udma_enqueue(rx_base, rx_addr, rx_size, cfg);
    plp_udma_enqueue(tx_base, tx_addr, tx_size, cfg);
  } else {
    copy->u.dual.rx_addr = rx_addr;
    copy->u.dual.rx_size = rx_size;
    __rt_channel_enqueue(channel, copy, tx_addr, tx_size, cfg);
  }

}

int __rt_periph_get_event(int event)
{
  int irq = rt_irq_disable();

  int index = 0;
  if (event >= 32)
  {
    index = 1;
    event -= 32;
  }

  int result = (__rt_socevents_status[index] >> event) & 1;

  rt_irq_restore(irq);

  return result;
}

void __rt_periph_wait_event(int event, int clear)
{
  int irq = rt_irq_disable();

  int index = 0;
  if (event >= 32)
  {
    index = 1;
    event -= 32;
  }

  while(!((__rt_socevents_status[index] >> event) & 1))
  {
    rt_wait_for_interrupt();
    rt_irq_enable();
    rt_irq_disable();
  }

  if (clear) __rt_socevents_status[index] &= ~(1<<event);

  rt_irq_restore(irq);
}

void __rt_periph_clear_event(int event)
{
  int irq = rt_irq_disable();

  int index = 0;
  if (event >= 32)
  {
    index = 1;
    event -= 32;
  }

  __rt_socevents_status[index] &= ~(1<<event);

  rt_irq_restore(irq);
}

RT_BOOT_CODE void __attribute__((constructor)) __rt_periph_init()
{
  for (unsigned int i=0; i<ARCHI_NB_PERIPH*2; i++) {
    rt_periph_channel_t *channel = &periph_channels[i];
    channel->first = NULL;
    channel->firstToEnqueue = NULL;
    channel->base = hal_udma_channel_base(i);
    channel->callback = udma_event_handler;
  }
  
  for (int i=0; i<ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT; i++)
  {
    __rt_udma_extra_callback[i] = __rt_soc_evt_no_udma;
  }

  __rt_socevents_status[0] = 0;
  __rt_socevents_status[1] = 0;
}
