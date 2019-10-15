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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 *          Jie Chen, GreenWaves Technologies (jie.chen@greenwaves-technologies.com)
 */

#include "rt/rt_api.h"
#include <stdint.h>

static int __rt_spim_open_count[ARCHI_UDMA_NB_SPIM];

typedef struct {
    unsigned int cmd[4];
} rt_spim_cmd_t;

static inline int __rt_spim_id(int periph_id)
{
  return periph_id - ARCHI_UDMA_SPIM_ID(0);
}

static int __rt_spi_get_div(int spi_freq)
{
  int periph_freq = __rt_freq_periph_get();

  if (spi_freq >= periph_freq)
  {
    return 0;
  }
  else
  {
    // Round-up the divider to obtain an SPI frequency which is below the maximum
    int div = (__rt_freq_periph_get() + spi_freq - 1)/ spi_freq;

    // The SPIM always divide by 2 once we activate the divider, thus increase by 1
    // in case it is even to not go avove the max frequency.
    if (div & 1) div += 1;
    div >>= 1;

    return div;
  }
}

static inline int __rt_spim_get_byte_align(int wordsize, int big_endian)
{
  return wordsize == RT_SPIM_WORDSIZE_32 && big_endian;
}

rt_spim_t *rt_spim_open(char *dev_name, rt_spim_conf_t *conf, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_spim_conf_t def_conf;

  if (conf == NULL)
  {
    conf = &def_conf;
    rt_spim_conf_init(conf);
  }

  int channel = -1;

  if (conf->id != -1)
  {  
    rt_trace(RT_TRACE_DEV_CTRL, "[SPIM] Opening spim device (id: %d)\n", conf->id);
    channel = ARCHI_UDMA_SPIM_ID(conf->id);
  }
  else if (dev_name != NULL)
  {
    rt_trace(RT_TRACE_DEV_CTRL, "[SPIM] Opening spim device (name: %s)\n", dev_name);
  
    rt_dev_t *dev = rt_dev_get(dev_name);
    if (dev == NULL) goto error;

    channel = dev->channel;
  }

  if (channel == -1) goto error;

  rt_spim_t *spim = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_spim_t));
  if (spim == NULL) goto error;

  spim->channel = channel*2;

  spim->wordsize = conf->wordsize;
  spim->big_endian = conf->big_endian;
  spim->polarity = conf->polarity;
  spim->phase = conf->phase;
  spim->max_baudrate = conf->max_baudrate;
  spim->cs = conf->cs;
  spim->byte_align = __rt_spim_get_byte_align(conf->wordsize, conf->big_endian);

  int div = __rt_spi_get_div(spim->max_baudrate);
  spim->div = div;

  spim->cfg = SPI_CMD_CFG(div, conf->polarity, conf->phase);

  int id = channel - ARCHI_UDMA_SPIM_ID(0);

  __rt_spim_open_count[id]++;

  if (__rt_spim_open_count[id] == 1)
  {
    plp_udma_cg_set(plp_udma_cg_get() | (1<<channel));

    __rt_udma_extra_callback[ARCHI_SOC_EVENT_SPIM0_EOT - ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT + id] = __rt_spim_handle_eot;
    __rt_periph_channel(spim->channel+1)->callback = __rt_spim_handle_tx_end;
    __rt_periph_channel(spim->channel)->callback = __rt_spim_handle_rx_end;


    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_SPIM0_EOT + id);
  }

  rt_irq_restore(irq);

  return spim;

error:
  rt_warning("[SPIM] Failed to open spim device\n");
  return NULL;
}

void __rt_spim_control(rt_spim_t *handle, rt_spim_control_e cmd, uint32_t arg)
{
  int irq = rt_irq_disable();

  int polarity = (cmd >> __RT_SPIM_CTRL_CPOL_BIT) & 3;
  int phase = (cmd >> __RT_SPIM_CTRL_CPHA_BIT) & 3;
  int set_freq = (cmd >> __RT_SPIM_CTRL_SET_MAX_BAUDRATE_BIT) & 1;
  int wordsize = (cmd >> __RT_SPIM_CTRL_WORDSIZE_BIT) & 3;
  int big_endian = (cmd >> __RT_SPIM_CTRL_ENDIANNESS_BIT) & 3;

  if (set_freq)
  {
    handle->max_baudrate = arg;
    handle->div = __rt_spi_get_div(arg);
  }

  if (polarity) handle->polarity = polarity >> 1;
  if (phase) handle->phase = phase >> 1;
  if (wordsize) handle->wordsize = wordsize >> 1;
  if (big_endian) handle->big_endian = big_endian >> 1;


  handle->cfg = SPI_CMD_CFG(handle->div, handle->polarity, handle->phase);
  handle->byte_align = __rt_spim_get_byte_align(handle->wordsize, handle->big_endian);

  rt_irq_restore(irq);
}

void rt_spim_close(rt_spim_t *handle, rt_event_t *event)
{
  int irq = rt_irq_disable();

  int channel = handle->channel >> 1;
  int id = channel - ARCHI_UDMA_SPIM_ID(0);

  __rt_spim_open_count[id]--;

  if (__rt_spim_open_count[id] == 0)
  {
    __rt_udma_extra_callback[ARCHI_SOC_EVENT_SPIM0_EOT - ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT + id] = __rt_soc_evt_no_udma;
    __rt_periph_channel(handle->channel+1)->callback = udma_event_handler;
    __rt_periph_channel(handle->channel)->callback = udma_event_handler;
    soc_eu_fcEventMask_clearEvent(ARCHI_SOC_EVENT_SPIM0_EOT + id);
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<(handle->channel>>1)));
  }

  rt_free(RT_ALLOC_FC_DATA, handle, sizeof(handle));

  rt_irq_restore(irq);
}

static inline int __rt_spim_enqueue_to_channel(rt_periph_channel_t *channel, rt_periph_copy_t *copy)
{
  if (!channel->first)
  {
    channel->first = copy;
    return 1;
  }
  else
  {
    if (channel->firstToEnqueue)
      channel->last->next = copy;
    else
      channel->firstToEnqueue = copy;
    copy->next = NULL;
    channel->last = copy;
    return 0;
  }
}

void __rt_spim_send_async(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Send bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  rt_periph_copy_t *copy = &event->implem.copy;

  // First enqueue the header with SPI config, cs, and send command.
  // The rest will be sent by the assembly code.
  // First the user data and finally an epilogue with the EOT command.
  rt_spim_cmd_t *cmd = (rt_spim_cmd_t *)copy->periph_data;

  cmd->cmd[0] = handle->cfg;
  cmd->cmd[1] = SPI_CMD_SOT(handle->cs);
  cmd->cmd[2] = SPI_CMD_TX_DATA(len, qspi, handle->byte_align);

  copy->event = event;

  int channel_id = handle->channel;

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  int size = (len + 7) >> 3;
  unsigned int base = hal_udma_channel_base(channel_id+1);

  if (__rt_spim_enqueue_to_channel(channel, copy))
  {
    if (cs_mode == RT_SPIM_CS_AUTO)
    {
      // CS auto mode. We handle the termination with an EOT so we have to enqueue
      // 3 transfers.
      // Enqueue fist SOT and user buffer.
      plp_udma_enqueue(base, (unsigned int)cmd, 3*4, UDMA_CHANNEL_CFG_EN);
      plp_udma_enqueue(base, (unsigned int)data, size, UDMA_CHANNEL_CFG_EN);

      // Then wait until first one is finished
      while(!plp_udma_canEnqueue(base));

      // And finally enqueue the EOT.
      // The user notification will be sent as soon as the last transfer
      // is done and next pending transfer will be enqueued
      cmd->cmd[0] = SPI_CMD_EOT(1);
      plp_udma_enqueue(base, (unsigned int)cmd, 1*4, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
      // CS keep mode.
      // We cannot use EOT due to HW limitations, so we have to use TX event instead.
      // TX event is current inactive, enqueue first transfer first EOT.
      plp_udma_enqueue(base, (unsigned int)cmd, 3*4, UDMA_CHANNEL_CFG_EN);
      // Then wait until it is finished (should be very quick).
      while(plp_udma_busy(base));
      // Then activateTX event and enqueue user buffer.
      // User notification and next pending transfer will be handled in the handler.
      soc_eu_fcEventMask_setEvent(channel_id+1);
      plp_udma_enqueue(base, (unsigned int)data, size, UDMA_CHANNEL_CFG_EN);
    }
  }
  else
  {
    copy->u.raw.val[0] = base;
    copy->u.raw.val[1] = (int)cmd;
    copy->u.raw.val[2] = 3*4;
    copy->u.raw.val[3] = (int)data;
    copy->u.raw.val[4] = size;
    copy->u.raw.val[5] = channel_id + 1;
    copy->u.raw.val[6] = (int)(cs_mode == RT_SPIM_CS_AUTO ? __rt_spim_single_eot : __rt_spim_single_no_eot);
  }

  rt_irq_restore(irq);
}

void __rt_spim_send(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  __rt_spim_send_async(handle, data, len, qspi, cs_mode, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void __rt_spim_receive_async(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Receive bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  rt_periph_copy_t *copy = &event->implem.copy;

  copy->event = event;

  rt_spim_cmd_t *cmd = (rt_spim_cmd_t *)copy->periph_data;

  int channel_id = handle->channel;

  int cmd_size = 3*4;
  cmd->cmd[0] = handle->cfg;
  cmd->cmd[1] = SPI_CMD_SOT(handle->cs);
  cmd->cmd[2] = SPI_CMD_RX_DATA(len, qspi, handle->byte_align);
  if (cs_mode == RT_SPIM_CS_AUTO)
  {
    cmd->cmd[3] = SPI_CMD_EOT(1);
    cmd_size += 4;
  }

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  int size = (len + 7) >> 3;
  unsigned int rx_base = hal_udma_channel_base(channel_id);

  if (__rt_spim_enqueue_to_channel(channel, copy))
  {
    if (cs_mode != RT_SPIM_CS_AUTO)
    {
      soc_eu_fcEventMask_setEvent(channel_id);
    }

    unsigned int tx_base = hal_udma_channel_base(channel_id + 1);
    plp_udma_enqueue(rx_base, (unsigned int)data, size, UDMA_CHANNEL_CFG_EN | (2<<1));
    plp_udma_enqueue(tx_base, (unsigned int)cmd, cmd_size, UDMA_CHANNEL_CFG_EN);
  }
  else
  {
    copy->u.raw.val[0] = rx_base;
    copy->u.raw.val[1] = (int)cmd;
    copy->u.raw.val[2] = cmd_size;
    copy->u.raw.val[3] = (int)data;
    copy->u.raw.val[4] = size;
    copy->u.raw.val[5] = channel_id;
    copy->u.raw.val[6] = (int)(cs_mode == RT_SPIM_CS_AUTO ? __rt_spim_single_rx_eot : __rt_spim_single_rx_no_eot);
  }

  rt_irq_restore(irq);
}

void __rt_spim_receive(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  __rt_spim_receive_async(handle, data, len, qspi, cs_mode, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void rt_spim_transfer_async(rt_spim_t *handle, void *tx_data, void *rx_data, size_t len, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Transfering bitstream (handle: %p, tx_buffer: %p, rx_buffer: %p, len: 0x%x, keep_cs: %d, event: %p)\n", handle, tx_data, rx_data, len, cs_mode, event);

  int irq = rt_irq_disable();

  rt_periph_copy_t *copy = &event->implem.copy;

  // First enqueue the header with SPI config, cs, and send command.
  // The rest will be sent by the assembly code.
  // First the user data and finally an epilogue with the EOT command.
  rt_spim_cmd_t *cmd = (rt_spim_cmd_t *)copy->periph_data;

  cmd->cmd[0] = handle->cfg;
  cmd->cmd[1] = SPI_CMD_SOT(handle->cs);
  cmd->cmd[2] = SPI_CMD_FUL(len, handle->byte_align);

  copy->event = event;

  int channel_id = handle->channel;

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  int size = (len + 7) >> 3;
  unsigned int rx_base = hal_udma_channel_base(channel_id);
  unsigned int tx_base = hal_udma_channel_base(channel_id+1);

  if (__rt_spim_enqueue_to_channel(channel, copy))
  {
    if (cs_mode == RT_SPIM_CS_AUTO)
    {
      plp_udma_enqueue(tx_base, (unsigned int)cmd, 3*4, UDMA_CHANNEL_CFG_EN);
      plp_udma_enqueue(rx_base, (unsigned int)rx_data, size, UDMA_CHANNEL_CFG_EN | (2<<1));
      plp_udma_enqueue(tx_base, (unsigned int)tx_data, size, UDMA_CHANNEL_CFG_EN);

      while(!plp_udma_canEnqueue(tx_base));

      cmd->cmd[0] = SPI_CMD_EOT(1);
      plp_udma_enqueue(tx_base, (unsigned int)cmd, 1*4, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
      plp_udma_enqueue(tx_base, (unsigned int)cmd, 3*4, UDMA_CHANNEL_CFG_EN);
      soc_eu_fcEventMask_setEvent(channel_id);
      plp_udma_enqueue(rx_base, (unsigned int)rx_data, size, UDMA_CHANNEL_CFG_EN | (2<<1));
      plp_udma_enqueue(tx_base, (unsigned int)tx_data, size, UDMA_CHANNEL_CFG_EN);
    }
  }
  else
  {
    copy->u.raw.val[0] = rx_base;
    copy->u.raw.val[1] = (int)cmd;
    copy->u.raw.val[2] = 3*4;
    copy->u.raw.val[3] = (int)rx_data;
    copy->u.raw.val[4] = size;
    copy->u.raw.val[5] = (int)tx_data;
    copy->u.raw.val[6] = (int)(cs_mode == RT_SPIM_CS_AUTO ? __rt_spim_dup_eot : __rt_spim_dup_no_eot);
    copy->u.raw.val[7] = channel_id;
  }

  rt_irq_restore(irq);
}

void rt_spim_transfer(rt_spim_t *handle, void *tx_data, void *rx_data, size_t len, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_spim_transfer_async(handle, tx_data, rx_data, len, cs_mode, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void rt_spim_conf_init(rt_spim_conf_t *conf)
{
  conf->wordsize = RT_SPIM_WORDSIZE_8;
  conf->big_endian = 0;
  conf->max_baudrate = 10000000;
  conf->cs_gpio = -1;
  conf->cs = -1;
  conf->id = -1;
  conf->polarity = 0;
  conf->phase = 0;
}

RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_spim_init()
{
  for (int i=0; i<ARCHI_UDMA_NB_SPIM; i++)
  {
    __rt_spim_open_count[i] = 0;
  }
}
