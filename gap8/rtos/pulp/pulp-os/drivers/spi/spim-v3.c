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
#include <stdint.h>

L2_DATA static pi_spim_t __rt_spim[ARCHI_UDMA_NB_SPIM];

typedef struct 
{
  pi_spim_t *spim;
  int max_baudrate;
  unsigned int cfg;
  unsigned int periph_base;
  char cs;
  char wordsize;
  char big_endian;
  char channel;
  char byte_align;
  unsigned char div;
  char polarity;
  char phase;
} pi_spim_cs_t;

typedef struct {
    unsigned int cmd[4];
} rt_spim_cmd_t;


void __pi_handle_waiting_copy(pi_task_t *task);

void __rt_spi_handle_repeat(void *arg);


#ifndef __RT_SPIM_COPY_ASM

void __pi_spim_handle_eot(int event, void *arg)
{
  pi_spim_t *spim = (pi_spim_t *)arg;

  if (spim->pending_repeat_len)
  {
    __rt_spi_handle_repeat((void *)spim);
    return;
  }

  pi_task_t *task = spim->pending_copy;
  spim->pending_copy = NULL;
  __rt_event_handle_end_of_task(task);

  task = spim->waiting_first;
  if (task)
  {
    spim->waiting_first = task->implem.next;
    __pi_handle_waiting_copy(task);
  }
}

void __rt_spim_handle_rx_copy(int event, void *arg)
{
  soc_eu_fcEventMask_clearEvent(event);
  __pi_spim_handle_eot(event, arg);
}

void __rt_spim_handle_tx_copy(int event, void *arg)
{
  soc_eu_fcEventMask_clearEvent(event);
  __pi_spim_handle_eot(event, arg);
}

#else

extern void __rt_spim_handle_tx_copy();
extern void __rt_spim_handle_rx_copy();
extern void __pi_spim_handle_eot(void *arg);

#endif



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
    // in case it is even to not go above the max frequency.
    if (div & 1) div += 1;
    div >>= 1;

    return div;
  }
}



static inline int __rt_spim_get_byte_align(int wordsize, int big_endian)
{
  return wordsize == PI_SPI_WORDSIZE_32 && big_endian;
}

int pi_spi_open(struct pi_device *device)
{
  int irq = rt_irq_disable();

  struct pi_spi_conf *conf = (struct pi_spi_conf *)device->config;

  int periph_id = ARCHI_UDMA_SPIM_ID(conf->itf);

  pi_spim_t *spim = &__rt_spim[conf->itf];

  pi_spim_cs_t *spim_cs = pmsis_l2_malloc(sizeof(pi_spim_cs_t));
  if (spim_cs == NULL) goto error;

  device->data = (void *)spim_cs;

  spim_cs->channel = periph_id;
  spim_cs->periph_base = hal_udma_periph_base(periph_id);
  spim_cs->spim = spim;
  spim_cs->wordsize = conf->wordsize;
  spim_cs->big_endian = conf->big_endian;
  spim_cs->polarity = conf->polarity;
  spim_cs->phase = conf->phase;
  spim_cs->max_baudrate = conf->max_baudrate;
  spim_cs->cs = conf->cs;
  spim_cs->byte_align = __rt_spim_get_byte_align(conf->wordsize, conf->big_endian);

  int div = __rt_spi_get_div(spim_cs->max_baudrate);
  spim_cs->div = div;

  spim_cs->cfg = SPI_CMD_CFG(div, conf->polarity, conf->phase);

  spim->open_count++;
  if (spim->open_count == 1)
  {
    plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));
    __rt_udma_callback[periph_id] = __pi_spim_handle_eot;
    __rt_udma_callback_data[periph_id] = spim;
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_PERIPH_EVT_BASE(periph_id) + ARCHI_UDMA_SPIM_EOT_EVT);

    spim->pending_repeat_len = 0;
  }

  rt_irq_restore(irq);

  return 0;

error:
  rt_irq_restore(irq);
  return -1;
}

void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *_arg)
{
  int irq = rt_irq_disable();
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  uint32_t arg = (uint32_t)_arg;

  int polarity = (cmd >> __PI_SPI_CTRL_CPOL_BIT) & 3;
  int phase = (cmd >> __PI_SPI_CTRL_CPHA_BIT) & 3;
  int set_freq = (cmd >> __PI_SPI_CTRL_SET_MAX_BAUDRATE_BIT) & 1;
  int wordsize = (cmd >> __PI_SPI_CTRL_WORDSIZE_BIT) & 3;
  int big_endian = (cmd >> __PI_SPI_CTRL_ENDIANNESS_BIT) & 3;

  if (set_freq)
  {
    spim_cs->max_baudrate = arg;
    spim_cs->div = __rt_spi_get_div(arg);
  }

  if (polarity) spim_cs->polarity = polarity >> 1;
  if (phase) spim_cs->phase = phase >> 1;
  if (wordsize) spim_cs->wordsize = wordsize >> 1;
  if (big_endian) spim_cs->big_endian = big_endian >> 1;


  spim_cs->cfg = SPI_CMD_CFG(spim_cs->div, spim_cs->polarity, spim_cs->phase);
  spim_cs->byte_align = __rt_spim_get_byte_align(spim_cs->wordsize, spim_cs->big_endian);

  rt_irq_restore(irq);
}

void pi_spi_close(struct pi_device *device)
{
  int irq = rt_irq_disable();
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;

  int channel = UDMA_EVENT_ID(spim_cs->channel);

  spim->open_count--;

  if (spim->open_count == 0)
  {
    // Deactivate event routing
    soc_eu_fcEventMask_clearEvent(channel);

    // Reactivate clock-gating
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<spim_cs->channel));
  }

  rt_irq_restore(irq);
}

void __rt_spi_handle_repeat(void *arg)
{
  int irq = rt_irq_disable();

  pi_spim_t *spim = (pi_spim_t *)arg;
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)spim->pending_repeat_device->data;
  unsigned int len = spim->pending_repeat_len;
  int cs_mode = (spim->pending_repeat_flags >> 0) & 0x3;
  int qspi = (spim->pending_repeat_flags & (0x3 << 2)) == PI_SPI_LINES_QUAD;
  if (len > 8192*8)
  {
    len = 8192*8;
  }
  spim->pending_repeat_len -= len;
  int size = (len + 7) >> 3;

#if 0
  if (spim->pending_repeat_send == 1)
  {
    spim->udma_cmd[0] = SPI_CMD_TX_DATA(len, qspi, spim_cs->byte_align);

    plp_udma_enqueue(spim->pending_repeat_base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);
    while(plp_udma_busy(spim->pending_repeat_base));

    if (spim->pending_repeat_len == 0 && cs_mode == PI_SPI_CS_AUTO)
    {
      plp_udma_enqueue(spim->pending_repeat_base, spim->pending_repeat_addr, size, UDMA_CHANNEL_CFG_EN);
      spim->udma_cmd[0] = SPI_CMD_EOT(1);
      plp_udma_enqueue(spim->pending_repeat_base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
      soc_eu_fcEventMask_setEvent(spim_cs->channel + 1);
      plp_udma_enqueue(spim->pending_repeat_base, spim->pending_repeat_addr, size, UDMA_CHANNEL_CFG_EN);
      spim->pending_repeat_addr += size;
    }
  }
  else if (spim->pending_repeat_send == 2)
  {
    unsigned int tx_base = hal_udma_channel_base(spim_cs->channel + 1);
    unsigned int rx_base = hal_udma_channel_base(spim_cs->channel);

    spim->udma_cmd[0] = SPI_CMD_FUL(len, spim_cs->byte_align);

    plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);

    if (spim->pending_repeat_len == 0 && cs_mode == PI_SPI_CS_AUTO)
    {
      plp_udma_enqueue(rx_base, (unsigned int)spim->pending_repeat_addr, size, UDMA_CHANNEL_CFG_EN | (2<<1));
      plp_udma_enqueue(tx_base, (unsigned int)spim->pending_repeat_dup_addr, size, UDMA_CHANNEL_CFG_EN);
      while(!plp_udma_canEnqueue(tx_base));
      spim->udma_cmd[0] = SPI_CMD_EOT(1);
      plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
      while(plp_udma_busy(tx_base));
      soc_eu_fcEventMask_setEvent(spim_cs->channel);
      plp_udma_enqueue(rx_base, (unsigned int)spim->pending_repeat_addr, size, UDMA_CHANNEL_CFG_EN | (2<<1));
      plp_udma_enqueue(tx_base, (unsigned int)spim->pending_repeat_dup_addr, size, UDMA_CHANNEL_CFG_EN);
      spim->pending_repeat_addr += size;
      spim->pending_repeat_dup_addr += size;
    }
  }
  else
  {
    int cmd_size = 0;
    unsigned int tx_base = hal_udma_channel_base(spim_cs->channel + 1);
    unsigned int rx_base = hal_udma_channel_base(spim_cs->channel);

    spim->udma_cmd[cmd_size++] = SPI_CMD_RX_DATA(len, qspi, spim_cs->byte_align);

    if (spim->pending_repeat_len == 0 && cs_mode == PI_SPI_CS_AUTO)
    {
      spim->udma_cmd[cmd_size++] = SPI_CMD_EOT(1);
    }
    else
    {
      soc_eu_fcEventMask_setEvent(spim_cs->channel);
    }

    plp_udma_enqueue(rx_base, (unsigned int)spim->pending_repeat_addr, size, UDMA_CHANNEL_CFG_EN | (2<<1));
    plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);

    spim->pending_repeat_addr += size;
  }
#endif

  rt_irq_restore(irq);
}


void pi_spi_send_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;
  int qspi = (flags & (0x3 << 2)) == PI_SPI_LINES_QUAD;
  int cs_mode = (flags >> 0) & 0x3;

  if (spim->pending_copy)
  {
    task->implem.data[0] = 0;
    task->implem.data[1] = (int)device;
    task->implem.data[2] = (int)data;
    task->implem.data[3] = len;
    task->implem.data[4] = flags;

    if (spim->waiting_first)
      spim->waiting_last->implem.next = task;
    else
      spim->waiting_first = task;

    spim->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  unsigned int cmd_base = spim_cs->periph_base + ARCHI_SPIM_CMD_OFFSET;
  unsigned int channel_base = spim_cs->periph_base + UDMA_CHANNEL_TX_OFFSET;
  int buffer_size = (len+7)/8;

  if (len > 8192*8)
  {
    spim->pending_repeat_len = len - 8192*8;
    spim->pending_repeat_addr = (uint32_t)data + 8192;
    spim->pending_repeat_base = spim_cs->periph_base;
    spim->pending_repeat_device = device;
    spim->pending_repeat_send = 1;
    spim->pending_repeat_flags = flags;
    len = 8192*8;
  }
  
  spim->pending_copy = task;

  int size = (len + 7) >> 3;

  // First enqueue the header with SPI config, cs, and send command.
  // The rest will be sent by the assembly code.
  // First the user data and finally an epilogue with the EOT command.

  spim->udma_cmd[0] = spim_cs->cfg;
  spim->udma_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
  spim->udma_cmd[2] = SPI_CMD_TX_DATA(len/32, SPI_CMD_1_WORD_PER_TRANSF, 32, qspi, SPI_CMD_MSB_FIRST);
  spim->udma_cmd[3] = SPI_CMD_EOT(1, cs_mode == RT_SPIM_CS_KEEP);

  int cfg = UDMA_CHANNEL_CFG_SIZE_32 | UDMA_CHANNEL_CFG_EN;
  plp_udma_enqueue(cmd_base, (int)spim->udma_cmd, 4*4, cfg);
  plp_udma_enqueue(channel_base, (int)data, buffer_size, cfg);

end:
  rt_irq_restore(irq);
}


void pi_spi_send(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags)
{
  pi_task_t task;
  pi_spi_send_async(device, data, len, flags, pi_task_block(&task));
  pi_task_wait_on(&task);
}



void pi_spi_receive_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
  //rt_trace(RT_TRACE_SPIM, "[SPIM] Receive bitstream (handle: %p, buffer: %p, len: 0x%x, qspi: %d, keep_cs: %d, event: %p)\n", handle, data, len, qspi, cs_mode, event);

  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;
  int qspi = (flags & (0x3 << 2)) == PI_SPI_LINES_QUAD;
  int cs_mode = (flags >> 0) & 0x3;

  if (spim->pending_copy)
  {
    task->implem.data[0] = 1;
    task->implem.data[1] = (int)device;
    task->implem.data[2] = (int)data;
    task->implem.data[3] = len;
    task->implem.data[4] = flags;

    if (spim->waiting_first)
      spim->waiting_last->implem.next = task;
    else
      spim->waiting_first = task;

    spim->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  spim->pending_copy = task;

  unsigned int rx_base = spim_cs->periph_base + UDMA_CHANNEL_RX_OFFSET;
  unsigned int cmd_base = spim_cs->periph_base + ARCHI_SPIM_CMD_OFFSET;

  if (len > 8192*8)
  {
    spim->pending_repeat_len = len - 8192*8;
    spim->pending_repeat_addr = (uint32_t)data + 8192;
    spim->pending_repeat_base = rx_base;
    spim->pending_repeat_device = device;
    spim->pending_repeat_send = 0;
    spim->pending_repeat_flags = flags;
    len = 8192*8;
  }
  
  int size = (len + 7) >> 3;

  spim->udma_cmd[0] = spim_cs->cfg;
  spim->udma_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
  spim->udma_cmd[2] = SPI_CMD_RX_DATA(len/32, SPI_CMD_1_WORD_PER_TRANSF, 32, qspi, SPI_CMD_MSB_FIRST);
  spim->udma_cmd[3] = SPI_CMD_EOT(1, cs_mode == RT_SPIM_CS_KEEP);

    int cfg = UDMA_CHANNEL_CFG_SIZE_32 | UDMA_CHANNEL_CFG_EN;
  plp_udma_enqueue(rx_base, (unsigned int)data, size, cfg);
  plp_udma_enqueue(cmd_base, (unsigned int)spim->udma_cmd, 4*4, cfg);

end:
  rt_irq_restore(irq);
}

void pi_spi_receive(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags)
{
  pi_task_t task;
  pi_spi_receive_async(device, data, len, flags, pi_task_block(&task));
  pi_task_wait_on(&task);
}


void pi_spi_transfer_async(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
  //rt_trace(RT_TRACE_SPIM, "[SPIM] Transfering bitstream (handle: %p, tx_buffer: %p, rx_buffer: %p, len: 0x%x, flags: 0x%x, event: %p)\n", handle, tx_data, rx_data, len, flags, event);

  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;
  int cs_mode = (flags >> 0) & 0x3;

  if (spim->pending_copy)
  {
    task->implem.data[0] = 2;
    task->implem.data[1] = (int)device;
    task->implem.data[2] = (int)tx_data;
    task->implem.data[3] = (int)rx_data;
    task->implem.data[4] = len;
    task->implem.data[5] = cs_mode;

    if (spim->waiting_first)
      spim->waiting_last->implem.next = task;
    else
      spim->waiting_first = task;

    spim->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  int channel_id = UDMA_CHANNEL_ID(spim_cs->channel);

  unsigned int cmd_base = spim_cs->periph_base + ARCHI_SPIM_CMD_OFFSET;
  unsigned int rx_base = spim_cs->periph_base + UDMA_CHANNEL_RX_OFFSET;
  unsigned int tx_base = spim_cs->periph_base + UDMA_CHANNEL_TX_OFFSET;

  if (len > 8192*8)
  {
    spim->pending_repeat_len = len - 8192*8;
    spim->pending_repeat_base = tx_base;
    spim->pending_repeat_addr = (uint32_t)rx_data + 8192;
    spim->pending_repeat_dup_addr = (uint32_t)tx_data + 8192;
    spim->pending_repeat_device = device;
    spim->pending_repeat_send = 2;
    spim->pending_repeat_flags = flags;
    len = 8192*8;
  }
  
  spim->pending_copy = task;

  // First enqueue the header with SPI config, cs, and send command.
  // The rest will be sent by the assembly code.
  // First the user data and finally an epilogue with the EOT command.
  spim->udma_cmd[0] = spim_cs->cfg;
  spim->udma_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
  spim->udma_cmd[2] = SPI_CMD_FUL(len/32, SPI_CMD_1_WORD_PER_TRANSF, 32, SPI_CMD_MSB_FIRST);
  spim->udma_cmd[3] = SPI_CMD_EOT(1, cs_mode == RT_SPIM_CS_KEEP);

  int size = (len + 7) >> 3;

  int cfg = UDMA_CHANNEL_CFG_SIZE_32 | UDMA_CHANNEL_CFG_EN;
  plp_udma_enqueue(cmd_base, (unsigned int)spim->udma_cmd, 4*4, UDMA_CHANNEL_CFG_EN);
  plp_udma_enqueue(rx_base, (unsigned int)rx_data, size, cfg);
  plp_udma_enqueue(tx_base, (unsigned int)tx_data, size, cfg);

end:
  rt_irq_restore(irq);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flags)
{
  pi_task_t task;
  pi_spi_transfer_async(device, tx_data, rx_data, len, flags, pi_task_block(&task));
  pi_task_wait_on(&task);
}

void __pi_handle_waiting_copy(pi_task_t *task)
{
  if (task->implem.data[0] == 0)
    pi_spi_send_async((struct pi_device *)task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task->implem.data[4], task);
  else if (task->implem.data[0] == 1)
    pi_spi_receive_async((struct pi_device *)task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task->implem.data[4], task);
  else
    pi_spi_transfer_async((struct pi_device *)task->implem.data[1], (void *)task->implem.data[2], (void *)task->implem.data[3], task->implem.data[4], task->implem.data[5], task);
}

void pi_spi_conf_init(struct pi_spi_conf *conf)
{
  conf->wordsize = PI_SPI_WORDSIZE_8;
  conf->big_endian = 0;
  conf->max_baudrate = 10000000;
  conf->cs = -1;
  conf->itf = 0;
  conf->polarity = 0;
  conf->phase = 0;
}

static void __attribute__((constructor)) __rt_spim_init()
{
  for (int i=0; i<ARCHI_UDMA_NB_SPIM; i++)
  {
    __rt_spim[i].open_count = 0;
    __rt_spim[i].pending_copy = NULL;
    __rt_spim[i].waiting_first = NULL;
    __rt_spim[i].id = i;
    __rt_udma_channel_reg_data(UDMA_EVENT_ID(ARCHI_UDMA_SPIM_ID(0) + i), &__rt_spim[i]);
    __rt_udma_channel_reg_data(UDMA_EVENT_ID(ARCHI_UDMA_SPIM_ID(0) + i)+1, &__rt_spim[i]);
  }
}

#ifdef __ZEPHYR__

#include <zephyr.h>
#include <device.h>
#include <init.h>

static int spi_init(struct device *device)
{
  ARG_UNUSED(device);

  __rt_spim_init();

  return 0;
}

struct spi_config {
};

struct spi_data {
};

static const struct spi_config spi_cfg = {
};

static struct spi_data spi_data = {
};

DEVICE_INIT(spi, "spi", &spi_init,
    &spi_data, &spi_cfg,
    PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif