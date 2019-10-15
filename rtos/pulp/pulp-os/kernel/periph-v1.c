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


RT_FC_TINY_DATA rt_periph_channel_t periph_channels[ARCHI_NB_PERIPH*2];


static inline __attribute__((always_inline)) void __rt_channel_enqueue_i2c(rt_periph_channel_t *channel, rt_periph_copy_t *copy, int addr, int size, int cfg, int div)
{
  copy->addr = addr;
  copy->size = size;
  copy->cfg = cfg;
  copy->u.i2c.div = div;
}

static inline void __rt_udma_enqueue_i2c(unsigned base, unsigned int periph_base, unsigned int addr, unsigned int size, unsigned int cfg, int div)
{
    hal_udma_i2c_setup_set(periph_base, hal_udma_i2c_setup_compute(1, div));
    plp_udma_enqueue(base, addr, size, cfg);
}

void rt_periph_copy_i2c(rt_periph_copy_t *copy, int channel_id, unsigned int addr, int size,
  unsigned int cfg, int div, rt_event_t *event)
{
  rt_trace(RT_TRACE_UDMA_COPY, "[UDMA] Enqueueing UDMA copy (node: 0x%x, l2Addr: 0x%x, size: 0x%x, channelId: %d)\n", (int)copy, addr, size, channel_id);

  int irq = rt_irq_disable();

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);

  unsigned int base = hal_udma_channel_base(channel_id);

  cfg |= UDMA_CHANNEL_CFG_EN;
  copy->size = size;
  copy->event = event;

  int is_first = !channel->first;

  __rt_channel_push(channel, copy);


  printf("%s %d\n", __FILE__, __LINE__);

  // Only 1 transfer can be enqueued to the I2C channel, due to the global registers.
  // If no transfer is present, we can directly enqueue it
  // Otherwise enqueue it in the SW queue, it will be handled later on by the interrupt handler
  if (is_first) {
  printf("%s %d\n", __FILE__, __LINE__);

    __rt_udma_enqueue_i2c(base, hal_udma_periph_base(channel_id/2), addr, size, cfg, div);
  } else {
  printf("%s %d\n", __FILE__, __LINE__);

    __rt_channel_enqueue_i2c(channel, copy, addr, size, cfg, div);
  }

  rt_irq_restore(irq);
}

void rt_periph_copy_spi(rt_periph_copy_t *copy, int channel_id, unsigned int addr, int size, int len,
  unsigned int cfg, unsigned int spi_status, rt_event_t *event)
{
  rt_trace(RT_TRACE_UDMA_COPY, "[UDMA] Enqueueing UDMA copy (node: 0x%x, l2Addr: 0x%x, size: 0x%x, channelId: %d)\n", (int)copy, addr, size, channel_id);

  int irq = rt_irq_disable();

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);

  unsigned int base = hal_udma_channel_base(channel_id);
  unsigned int periph_base = UDMA_PERIPH_OFFSET(UDMA_PERIPH_GET(base));

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  if (copy == NULL) {
    copy = &call_event->copy;
    rt_periph_copy_init(copy, 0);
  }

  cfg |= UDMA_CHANNEL_CFG_EN;
  copy->size = size;
  copy->event = call_event;

  int is_first = !channel->first;

  __rt_channel_push(channel, copy);

  plp_spi_disable(periph_base + 0x0);
  plp_spi_disable(periph_base + 0x10);

  // Only 1 transfer can be enqueued to the SPI channel, due to the global registers.
  // If no transfer is present, we can directly enqueue it
  // Otherwise enqueue it in the SW queue, it will be handled later on by the interrupt handler
  if (is_first) {
    hal_udma_spim_enqueue(periph_base, base, spi_status, addr, size, cfg, len);
  } else {
    __rt_channel_enqueue(channel, copy, addr, size, cfg);
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

static void __rt_handle_transfer_end(int channel_id)
{
  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  rt_periph_copy_t *copy = channel->first;
  if (copy->end_callback)
  {
    ((void (*)(rt_periph_copy_t *copy))copy->end_callback)(copy);
  }
  else
  {
    channel->first = copy->next;
    if (copy->event)
      __rt_push_event(copy->event->sched, copy->event);
  }
}

static void __rt_handle_transfer_end_i2c(int channel_id)
{
  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  rt_periph_copy_t *copy = channel->first;

  // First check if the copy has a termination callback, in which case
  // the callback will handle the end. This may keep the copy in the queue head
  // and directly reenqueue a UDMA transfer.

  if (copy->end_callback)
  {
    ((void (*)(rt_periph_copy_t *copy))copy->end_callback)(copy);
  }
  else
  {
    // Otherwise, update the queue
    rt_periph_copy_t *next = copy->next;
    channel->first = next;

    // And check if there is a pending copy to enqueue to the UDMA
    if (next)
    {
      __rt_udma_enqueue_i2c(hal_udma_channel_base(channel_id), hal_udma_periph_base(channel_id/2),
        next->addr, next->size, next->cfg, next->u.i2c.div);
    }

    // And finally push the copy event to the scheduler in order to handle
    // the copy termination
    if (copy->event) {
      __rt_push_event(copy->event->sched, copy->event);
    }
  }
}

static void __rt_spim0_tx_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_SPIM_ID(0) + 1);
}

static void __rt_spim0_rx_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_SPIM_ID(0));
}

static void __rt_uart_tx_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_UART_ID(0) + 1);
}

static void __rt_uart_rx_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_UART_ID(0));
}

static void __rt_adc0_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_ADC_ID(0));
}

static void __rt_adc1_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_ADC_ID(0) + 1);
}

static void __rt_adc2_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_ADC_ID(1));
}

static void __rt_adc3_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_ADC_ID(1) + 1);
}

static void __rt_spim1_tx_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_SPIM_ID(1) + 1);
}

static void __rt_spim1_rx_handler()
{
  __rt_handle_transfer_end(2*ARCHI_UDMA_SPIM_ID(1));
}

static void __rt_i2c_tx_handler()
{
  __rt_handle_transfer_end_i2c(2*ARCHI_UDMA_I2C_ID(0) + 1);
}

static void __rt_i2c_rx_handler()
{
  __rt_handle_transfer_end_i2c(2*ARCHI_UDMA_I2C_ID(0));
}

RT_BOOT_CODE void __attribute__((constructor)) __rt_periph_init()
{
  for (unsigned int i=0; i<ARCHI_NB_PERIPH*2; i++) {
    rt_periph_channel_t *channel = &periph_channels[i];
    channel->first = NULL;
    channel->firstToEnqueue = NULL;
    channel->base = hal_udma_channel_base(i);
  }

#if PULP_CHIP_FAMILY == CHIP_VIVOSOC2
  udma_mapAllEvents(ARCHI_UDMA_EVT_SPIM_TX, ARCHI_UDMA_EVT_SPIM_RX, ARCHI_UDMA_EVT_UART_TX, ARCHI_UDMA_EVT_UART_RX, ARCHI_UDMA_EVT_ADC_CH0, ARCHI_UDMA_EVT_ADC_CH1, ARCHI_UDMA_EVT_ADC_CH2, ARCHI_UDMA_EVT_ADC_CH3);
  udma_mapAllEvents1(ARCHI_UDMA_EVT_SPIM1_TX, ARCHI_UDMA_EVT_SPIM1_RX, ARCHI_UDMA_EVT_I2C_TX, ARCHI_UDMA_EVT_I2C_RX, 0, 0, 0, 0);

  rt_irq_mask_set((1<<ARCHI_EVT_UDMA0) | (1<<ARCHI_EVT_UDMA1) | (1<<ARCHI_EVT_UDMA2) | (1<<ARCHI_EVT_UDMA3)
   | (1<<ARCHI_EVT_UDMA4) | (1<<ARCHI_EVT_UDMA5) | (1<<ARCHI_EVT_UDMA6) | (1<<ARCHI_EVT_UDMA7)
   | (1<<ARCHI_EVT_UDMA8) | (1<<ARCHI_EVT_UDMA9) | (1<<ARCHI_EVT_UDMA10));

  rt_irq_set_handler(ARCHI_EVT_UDMA0, __rt_spim0_tx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA1, __rt_spim0_rx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA2, __rt_uart_tx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA3, __rt_uart_rx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA4, __rt_adc0_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA5, __rt_adc1_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA6, __rt_adc2_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA7, __rt_adc3_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA8, __rt_spim1_tx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA9, __rt_spim1_rx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA10, __rt_i2c_tx_handler);

#elif PULP_CHIP_FAMILY == CHIP_FULMINE
  udma_mapAllEvents(ARCHI_UDMA_EVT_SPIM_TX, ARCHI_UDMA_EVT_SPIM_RX, ARCHI_UDMA_EVT_UART_TX, ARCHI_UDMA_EVT_UART_RX, ARCHI_UDMA_EVT_I2C_RX, ARCHI_UDMA_EVT_I2C_TX, 0, 0);
  
  rt_irq_mask_set((1<<ARCHI_EVT_UDMA0) | (1<<ARCHI_EVT_UDMA1) | (1<<ARCHI_EVT_UDMA2) | (1<<ARCHI_EVT_UDMA3)
   | (1<<ARCHI_EVT_UDMA4) | (1<<ARCHI_EVT_UDMA5));

  rt_irq_set_handler(ARCHI_EVT_UDMA0, __rt_spim0_tx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA1, __rt_spim0_rx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA2, __rt_uart_tx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA3, __rt_uart_rx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA4, __rt_i2c_tx_handler);
  rt_irq_set_handler(ARCHI_EVT_UDMA5, __rt_i2c_rx_handler);

#endif

}

