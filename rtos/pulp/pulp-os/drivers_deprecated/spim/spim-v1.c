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
#include <stdint.h>

static int __rt_spi_get_div(int spi_freq)
{
  int periph_freq = __rt_freq_periph_get();

  if (spi_freq >= periph_freq)
  {
    return 0;
  }
  else
  {
    // The divider set in the spi register will make a counter counts from 0
    // to this value (including) and will generate an edge each time it is reached.
    // So the final divider is (div + 1) * 2.
    // A divider of 0 also means the clock is actually divided by 2.


    // Round-up the divider to obtain an SPI frequency which is below the maximum
    int div = (__rt_freq_periph_get() + spi_freq - 1)/ spi_freq;

    // The SPIM always divide by 2 once we activate the divider, thus increase by 1
    // in case it is even to not go above the max frequency.
    if (div & 1) div += 1;
    div = div / 2 - 1;

    return div;
  }
}

void __rt_spim_set_cfg(rt_spim_t *spim)
{
  int datasize = spim->wordsize == RT_SPIM_WORDSIZE_32 && spim->big_endian ? 2 : 0;
  spim->cfg = hal_udma_spim_clkdiv_compute(datasize, datasize, spim->phase, spim->polarity, spim->div);
  hal_udma_spim_clkdiv_set(hal_udma_periph_base(spim->channel/2), spim->cfg);
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
    channel = conf->id;
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

  spim->channel = ARCHI_UDMA_SPIM_ID(channel) * 2;

  spim->wordsize = conf->wordsize;
  spim->big_endian = conf->big_endian;
  spim->polarity = conf->polarity;
  spim->phase = conf->phase;
  spim->max_baudrate = conf->max_baudrate;
  spim->cs = conf->cs;

  int div = __rt_spi_get_div(spim->max_baudrate);
  spim->div = div;

  __rt_spim_set_cfg(spim);

  rt_irq_restore(irq);

  return spim;

error:
  rt_warning("[SPIM] Failed to open spim device\n");
  return NULL;
}

void __rt_spim_control(rt_spim_t *handle, rt_spim_control_e cmd, uint32_t arg)
{
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

  __rt_spim_set_cfg(handle);
}

void rt_spim_close(rt_spim_t *handle, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_free(RT_ALLOC_FC_DATA, handle, sizeof(handle));

  rt_irq_restore(irq);
}

void __rt_spim_send(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Send bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;

  uint32_t spi_status = hal_udma_spim_status_compute(
    cs_mode == RT_SPIM_CS_KEEP, handle->cs, 0, qspi ? ARCHI_SPIM_WSTATUS_QWRITE : ARCHI_SPIM_WSTATUS_WRITE
  );

  rt_periph_copy_spi(copy, handle->channel + 1, (unsigned int)data, (len+7)>>3, len, UDMA_CHANNEL_CFG_SIZE_32, spi_status, event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}


void __rt_spim_receive(rt_spim_t *handle, void *data, size_t len, int qspi, rt_spim_cs_e cs_mode, rt_event_t *event)
{
  rt_trace(RT_TRACE_SPIM, "[SPIM] Receive bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;

  uint32_t spi_status = hal_udma_spim_status_compute(
    cs_mode == RT_SPIM_CS_KEEP, handle->cs, 0, qspi ? ARCHI_SPIM_WSTATUS_QREAD : ARCHI_SPIM_WSTATUS_READ
  );

  rt_periph_copy_spi(copy, handle->channel, (unsigned int)data, (len+7)>>3, len, UDMA_CHANNEL_CFG_SIZE_32, spi_status, event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void rt_spim_transfer(rt_spim_t *handle, void *tx_data, void *rx_data, size_t len, rt_spim_cs_e mode, rt_event_t *event)
{
  return;
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
