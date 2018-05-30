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
 *          Jie Chen, GreenWaves Technologies (jie.chen@greenwaves-technologies.com)
 */

#include "rt/rt_api.h"
#include <stdint.h>

typedef struct {
    unsigned int cmd[4];
} rt_spim_cmd_t;

static rt_spim_t __rt_spim[ARCHI_UDMA_NB_SPIM];

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

  __rt_padframe_init();

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

  rt_spim_t *spim = &__rt_spim[__rt_spim_id(channel)];

  if (spim->open_count > 0) goto error;

  spim->open_count++;
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

  plp_udma_cg_set(plp_udma_cg_get() | (1<<channel));

  soc_eu_fcEventMask_setEvent(channel*2);
  soc_eu_fcEventMask_setEvent(channel*2 + 1);

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
}

void __rt_spim_send(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Send bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;

  // First enqueue the header with SPI config, cs, and send command.
  // The rest will be sent by the assembly code.
  // First the user data and finally an epilogue with the EOT command.
  int next_step;
  if (cs_mode == RT_SPIM_CS_AUTO) next_step = RT_PERIPH_COPY_SPIM_STEP2;
  else                            next_step = 0;
  rt_periph_copy_init_ctrl(copy, RT_PERIPH_COPY_SPIM_STEP1 << RT_PERIPH_COPY_CTRL_TYPE_BIT);

  rt_spim_cmd_t *cmd = (rt_spim_cmd_t *)copy->periph_data;
  unsigned int *udma_cmd = (unsigned int *)cmd->cmd;
  *udma_cmd++ = handle->cfg;
  *udma_cmd++ = SPI_CMD_SOT(handle->cs);
  *udma_cmd++ = SPI_CMD_TX_DATA(len, qspi, handle->byte_align);

  copy->cfg = UDMA_CHANNEL_CFG_EN;
  copy->u.raw.val[2] = (int)data;
  copy->u.raw.val[0] = (len + 7) >> 3;
  copy->u.raw.val[1] = next_step;

  rt_periph_single_copy(copy, handle->channel + 1, (unsigned int)cmd, 3*4, 0, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void __rt_spim_receive(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Receive bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;

  rt_periph_copy_init(copy, 0);

  rt_spim_cmd_t *cmd = (rt_spim_cmd_t *)copy->periph_data;
  unsigned int *udma_cmd = (unsigned int *)cmd->cmd;
  *udma_cmd++ = handle->cfg;
  *udma_cmd++ = SPI_CMD_SOT(handle->cs);
  *udma_cmd++ = SPI_CMD_RX_DATA(len, qspi, handle->byte_align);
  if (cs_mode == RT_SPIM_CS_AUTO)
  {
    *udma_cmd++ = SPI_CMD_EOT(0);
  }

  rt_periph_dual_copy(copy, handle->channel, (unsigned int)cmd, 4*4, (int)data, (len+7)>>3, 2<<1, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void rt_spim_transfer(rt_spim_t *handle, void *tx_data, void *rx_data, size_t len, rt_spim_cs_e mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Transfering bitstream (handle: %p, tx_buffer: %p, rx_buffer: %p, len: 0x%x, keep_cs: %d, event: %p)\n", handle, tx_data, rx_data, len, mode, event);

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
    __rt_spim[i].open_count = 0;
  }
}
