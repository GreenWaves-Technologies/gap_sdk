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

#ifndef __RT_RT_PERIPH_H__
#define __RT_RT_PERIPH_H__

/// @cond IMPLEM

extern RT_FC_TINY_DATA volatile unsigned int __rt_socevents_status[];

#define RT_PERIPH_COPY_L2   0

static inline void rt_periph_copy_init(rt_periph_copy_t *copy, int flags);

void rt_periph_copy(rt_periph_copy_t *copy, int channel, unsigned int addr, int size,
  unsigned int cfg, rt_event_t *event);

void rt_periph_hyper_copy(rt_periph_copy_t *copy, int channel, unsigned int addr, int size,
  unsigned int cfg, rt_event_t *event);

void rt_periph_single_copy(rt_periph_copy_t *copy, int channel, unsigned int addr, int size,
  unsigned int cfg, rt_event_t *event);

void rt_periph_dual_copy_safe(rt_periph_copy_t *copy, int rx_channel_id, unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg);

static inline void rt_periph_dual_copy(rt_periph_copy_t *copy, int rx_channel_id,
  unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg, rt_event_t *event)
{
  copy->event = event;
  int irq = rt_irq_disable();
  rt_periph_dual_copy_safe(copy, rx_channel_id, tx_addr, tx_size, rx_addr, rx_size, cfg);
  rt_irq_restore(irq);
}

void __rt_periph_dual_copy_noshadow_safe(rt_periph_copy_t *copy, int rx_channel_id, unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg);

static inline void rt_periph_dual_copy_noshadow_safe(rt_periph_copy_t *copy, int rx_channel_id, unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg, rt_event_t *event)
{
  copy->event = event;
  __rt_periph_dual_copy_noshadow_safe(copy, rx_channel_id, tx_addr, tx_size,
    rx_addr, rx_size, cfg);
}

static inline void rt_periph_dual_copy_noshadow(rt_periph_copy_t *copy, int rx_channel_id,
  unsigned int tx_addr, int tx_size, unsigned int rx_addr, int rx_size,
  unsigned int cfg, rt_event_t *event)
{
  int irq = rt_irq_disable();
  rt_periph_dual_copy_noshadow_safe(copy, rx_channel_id, tx_addr, tx_size,
    rx_addr, rx_size, cfg, event);
  rt_irq_restore(irq);
}


static inline __attribute__((always_inline)) void __rt_channel_push(rt_periph_channel_t *channel, rt_periph_copy_t *copy)
{
  copy->next = NULL;
  if (likely(channel->first == NULL)) {
    channel->first = copy;
  } else {
    channel->last->next = copy;
  }
  channel->last = copy;
}

static inline __attribute__((always_inline)) void __rt_channel_enqueue(rt_periph_channel_t *channel, rt_periph_copy_t *copy, int addr, int size, int cfg)
{
  copy->addr = addr;
  copy->size = size;
  copy->cfg = cfg;
  if (channel->firstToEnqueue == NULL) channel->firstToEnqueue = copy;
}


static inline void rt_periph_copy_init(rt_periph_copy_t *copy, int flags)
{
  copy->ctrl = 0;
  copy->repeat = 0;

#if defined(UDMA_VERSION) && UDMA_VERSION == 1
  copy->end_callback = 0;
#endif
}

static inline void rt_periph_copy_init_callback(rt_periph_copy_t *copy, unsigned int callback)
{
  copy->ctrl = 0;
  copy->repeat = 0;
  copy->enqueue_callback = callback;
}

static inline void rt_periph_copy_init_ctrl(rt_periph_copy_t *copy, int ctrl)
{
  copy->ctrl = ctrl;
  copy->repeat = 0;
}

extern RT_FC_TINY_DATA rt_periph_channel_t periph_channels[];
static inline rt_periph_channel_t *__rt_periph_channel(int channel) {
  return &periph_channels[channel];
}

#if defined(UDMA_VERSION) && UDMA_VERSION < 3

extern RT_FC_TINY_DATA void *__rt_udma_extra_callback[];

#elif defined(UDMA_VERSION) && UDMA_VERSION >= 3

extern RT_FC_TINY_DATA void *__rt_udma_callback[ARCHI_NB_PERIPH];
extern RT_FC_TINY_DATA void *__rt_udma_callback_data[ARCHI_NB_PERIPH];

#endif

#if defined(UDMA_VERSION)
static inline void __rt_udma_register_channel_callback(unsigned int channel, void (*callback)(void *), void *arg)
{
#if UDMA_VERSION == 2
  __rt_periph_channel(channel)->callback = callback;
#else
  __rt_udma_callback[channel>>UDMA_NB_PERIPH_EVENTS_LOG2] = callback;
#endif
}

#if defined(UDMA_VERSION) && UDMA_VERSION < 3

static inline void __rt_udma_register_extra_callback(unsigned int event, void (*callback)(void *), void *arg)
{
  __rt_udma_extra_callback[event - ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT] = callback;
}

#endif

#endif


int __rt_periph_get_event(int event);

void __rt_periph_wait_event(int event, int clear);

void __rt_periph_clear_event(int event);

static inline void __rt_periph_clear_event_safe(int event)
{
  int index = 0;
  if (event >= 32)
  {
    index = 1;
    event -= 32;
  }

  __rt_socevents_status[index] &= ~(1<<event);
}

#if defined(UDMA_VERSION) && UDMA_VERSION == 1


void rt_periph_copy_i2c(rt_periph_copy_t *copy, int channel_id, unsigned int addr, int size,
  unsigned int cfg, int div, rt_event_t *event);

void rt_periph_copy_spi(rt_periph_copy_t *copy, int channel_id, unsigned int addr, int size, int len,
  unsigned int cfg, unsigned int spi_status, rt_event_t *event);
  
#endif

#if defined(UDMA_VERSION)

void udma_event_handler();

void __rt_spim_handle_eot();

void __rt_mram_handle_event();

#if UDMA_VERSION <= 2

void __rt_spim_handle_event();

void __rt_spim_enqueue_eot();

void __rt_spim_do_nothing();

void udma_event_handler_end();

void __rt_spim_handle_tx_end();

void __rt_spim_handle_rx_end();

void __rt_soc_evt_no_udma();

void __rt_spim_dup_no_eot();

void __rt_spim_dup_eot();

void __rt_spim_single_no_eot();

void __rt_spim_single_eot();

void __rt_spim_single_rx_no_eot();

void __rt_spim_single_rx_eot();

#else

void __rt_spim_handle_pending_dup();

void __rt_spim_handle_pending_single();

#endif

#endif

/// @endcond

#endif
