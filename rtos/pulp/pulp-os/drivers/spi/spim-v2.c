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


extern void __pos_spim_receive_handle_temp_buffer();
extern void __pos_spim_receive_enqueue_transfer();
extern void __pos_spim_send_enqueue_transfer();
extern void __pos_spim_handle_copy_eot();


L2_DATA static pi_spim_t __rt_spim[ARCHI_UDMA_NB_SPIM];
static PI_L2 pos_spim_l2_t pos_spim_l2[ARCHI_UDMA_NB_SPIM];



typedef struct 
{
  pi_spim_t *spim;
  uint32_t rx_cmd;
  uint32_t tx_cmd;
  uint8_t *receive_addr_ucode;
  uint32_t receive_addr_ucode_size;
  uint8_t *send_addr_ucode;
  uint32_t send_addr_ucode_size;
  uint32_t *udma_receive_cmd;
  uint32_t *udma_send_cmd;
  uint32_t udma_receive_cmd_size;
  uint32_t udma_send_cmd_size;
  uint8_t *ucode;
  uint32_t ucode_size;
  int freq;
  int max_baudrate;
  unsigned int cfg;
  uint32_t max_rcv_size;
  uint32_t max_snd_size;
  char cs;
  char wordsize;
  char big_endian;
  signed char cs_gpio;
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


static void pos_spim_handle_pending_task(pi_spim_t *spim)
{
  pi_task_t *task = spim->pending_copy;

  spim->pending_copy = NULL;

  __rt_event_enqueue(task);

  task = spim->waiting_first;
  if (task)
  {
    spim->waiting_first = task->implem.next;
    __pi_handle_waiting_copy(task);
  }
}



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



static int __rt_spi_get_div(pi_spim_cs_t *spim_cs, int spi_freq)
{
  int periph_freq = __rt_freq_periph_get();

  if (spi_freq >= periph_freq)
  {
    spim_cs->freq = periph_freq / 2;
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

    spim_cs->freq = periph_freq / (div * 2);
    return div;
  }
}



static inline int __rt_spim_get_byte_align(int wordsize, int big_endian)
{
  return wordsize == PI_SPI_WORDSIZE_32 && big_endian;
}



static void pos_spim_apply_conf(pi_spim_cs_t *spim_cs)
{
    if (spim_cs->udma_receive_cmd)
    {
        spim_cs->udma_receive_cmd[0] = spim_cs->cfg;
        spim_cs->udma_receive_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
    }

    if (spim_cs->udma_send_cmd)
    {
        spim_cs->udma_send_cmd[0] = spim_cs->cfg;
        spim_cs->udma_send_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
    }

    spim_cs->rx_cmd = SPI_CMD_RX_DATA(1, 0, spim_cs->byte_align);
    spim_cs->tx_cmd = SPI_CMD_TX_DATA(1, 0, spim_cs->byte_align);
}



int pi_spi_open(struct pi_device *device)
{
  int irq = rt_irq_disable();

  struct pi_spi_conf *conf = (struct pi_spi_conf *)device->config;

  int periph_id = ARCHI_UDMA_SPIM_ID(conf->itf);
  int channel_id = UDMA_EVENT_ID(periph_id);

  pi_spim_t *spim = &__rt_spim[conf->itf];
  pos_spim_l2_t *spim_l2 = &pos_spim_l2[conf->itf];

  pi_spim_cs_t *spim_cs = pi_l2_malloc(sizeof(pi_spim_cs_t));
  if (spim_cs == NULL) goto error;

  device->data = (void *)spim_cs;

  spim_cs->channel = channel_id;
  spim_cs->spim = spim;
  spim_cs->wordsize = conf->wordsize;
  spim_cs->big_endian = conf->big_endian;
  spim_cs->polarity = conf->polarity;
  spim_cs->phase = conf->phase;
  spim_cs->max_baudrate = conf->max_baudrate;
  spim_cs->cs = conf->cs;
  spim_cs->byte_align = __rt_spim_get_byte_align(conf->wordsize, conf->big_endian);

  spim_cs->udma_send_cmd = NULL;
  spim_cs->udma_receive_cmd = NULL;

  int div = __rt_spi_get_div(spim_cs, spim_cs->max_baudrate);
  /* Clock div does not fit on 8 bits, SoC frequency needs to be lowered. */
  if (div > 0xFF) goto error;
  spim_cs->div = div;

  spim_cs->cfg = SPI_CMD_CFG(div, conf->polarity, conf->phase);
  pos_spim_apply_conf(spim_cs);

  // Compute chunk size to respect CS low pulse width. We account the 6 dummy cycles, 8 cycles for command and address and some margin
  if (conf->max_rcv_chunk_size != -1)
    spim_cs->max_rcv_size = (conf->max_rcv_chunk_size / (2*1000000000 / spim_cs->freq) - 12) & ~0x3;
  else
    spim_cs->max_rcv_size = 8192;

  // We take more margin for sending as the TX buffer is enqueued after the command buffer
  // which makes the time between CS low and the end of transfer unpredictable, e.g. a cache miss
  // can increase it.
  if (conf->max_snd_chunk_size != -1)
    spim_cs->max_snd_size = (conf->max_snd_chunk_size / (2*1000000000 / spim_cs->freq) - 48) & ~0x3;
  else
    spim_cs->max_snd_size = 8192;

  spim->open_count++;
  if (spim->open_count == 1)
  {
    plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_SPIM0_EOT + conf->itf);
    __rt_udma_register_extra_callback(ARCHI_SOC_EVENT_SPIM0_EOT + conf->itf, __pi_spim_handle_eot, (void *)spim);
    __rt_udma_register_channel_callback(channel_id, __rt_spim_handle_rx_copy, (void *)spim);
    __rt_udma_register_channel_callback(channel_id+1, __rt_spim_handle_tx_copy, (void *)spim);

    spim->pending_repeat_len = 0;
    spim->periph_base = hal_udma_periph_base(periph_id);
    spim->pending_transfer_index = 0;
    spim->pending_transfer_read_index = 0;
    spim->udma_cmd = spim_l2->udma_cmd;
    spim->pending_transfers[0].temp_buff = &spim_l2->temp_buff[0];
    spim->pending_transfers[1].temp_buff = &spim_l2->temp_buff[1];
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
    spim_cs->div = __rt_spi_get_div(spim_cs, arg);
  }

  if (polarity) spim_cs->polarity = polarity >> 1;
  if (phase) spim_cs->phase = phase >> 1;
  if (wordsize) spim_cs->wordsize = wordsize >> 1;
  if (big_endian) spim_cs->big_endian = big_endian >> 1;


  spim_cs->cfg = SPI_CMD_CFG(spim_cs->div, spim_cs->polarity, spim_cs->phase);
  spim_cs->byte_align = __rt_spim_get_byte_align(spim_cs->wordsize, spim_cs->big_endian);

  rt_irq_restore(irq);
}

static void __attribute__((noinline)) pos_spim_enqueue_to_pending(pi_spim_t *spim, pi_task_t *task, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4)
{
  task->implem.data[0] = data0;
  task->implem.data[1] = data1;
  task->implem.data[2] = data2;
  task->implem.data[3] = data3;
  task->implem.data[4] = data4;

  if (spim->waiting_first)
    spim->waiting_last->implem.next = task;
  else
    spim->waiting_first = task;

  spim->waiting_last = task;
  task->implem.next = NULL;
}



static void __attribute__((noinline)) pos_spim_enqueue_to_pending_7(pi_spim_t *spim, pi_task_t *task, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4, uint32_t data5, uint32_t data6, uint32_t data7)
{
  task->implem.data[0] = data0;
  task->implem.data[1] = data1;
  task->implem.data[2] = data2;
  task->implem.data[3] = data3;
  task->implem.data[4] = data4;
  task->implem.data[5] = data5;
  task->implem.data[6] = data6;
  task->implem.data[7] = data7;

  if (spim->waiting_first)
    spim->waiting_last->implem.next = task;
  else
    spim->waiting_first = task;

  spim->waiting_last = task;
  task->implem.next = NULL;
}

void pi_spi_close(struct pi_device *device)
{
  int irq = rt_irq_disable();
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;

  int channel = spim_cs->channel;
  int periph_id = UDMA_PERIPH_ID(channel);

  spim->open_count--;

  pi_spi_receive_ucode_set(device, NULL, 0);
  pi_spi_send_ucode_set(device, NULL, 0);

  if (spim->open_count == 0)
  {
    // Deactivate event routing
    soc_eu_fcEventMask_clearEvent(channel);

    // Reactivate clock-gating
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<periph_id));
  }

  pi_l2_free(spim_cs, sizeof(pi_spim_cs_t));

  rt_irq_restore(irq);
}

void __rt_spi_handle_repeat(void *arg)
{
  int irq = rt_irq_disable();

  pi_spim_t *spim = (pi_spim_t *)arg;
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)spim->pending_repeat_device->data;
  unsigned int len = spim->pending_repeat_len;
  int cs_mode = (spim->pending_repeat_flags >> 0) & 0x3;
  int qspi = ((spim->pending_repeat_flags >> 2) & 0x3) == 1;

  if (spim->pending_repeat_misaligned_size)
  {
    uint32_t size = spim->pending_repeat_misaligned_size;
    uint32_t addr = (spim->pending_repeat_addr - size) & ~0x3;
    uint32_t value = *(uint32_t *)addr;

    value = __BITINSERT_R(spim->buffer, value, 32-size*8, 0);
    *(uint32_t *)addr = value;

    spim->pending_repeat_misaligned_size = 0;

  }


  int size = (len + 7) >> 3;

  if (size > 8192)
  {
    size = 8192;
    len = 8192*8;
  }

  spim->pending_repeat_len -= len;

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

  rt_irq_restore(irq);
}


void pi_spi_send_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;
  int qspi = ((flags >> 2) & 0x3) == 1;
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

  unsigned int base = hal_udma_channel_base(spim_cs->channel + 1);

  if (len > 8192*8)
  {
    spim->pending_repeat_len = len - 8192*8;
    spim->pending_repeat_addr = (uint32_t)data + 8192;
    spim->pending_repeat_base = base;
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

  int cmd_size = 0;
  spim->udma_cmd[cmd_size++] = spim_cs->cfg;
  spim->udma_cmd[cmd_size++] = SPI_CMD_SOT(spim_cs->cs);
  spim->udma_cmd[cmd_size++] = SPI_CMD_TX_DATA(len, qspi, spim_cs->byte_align);

  if (cs_mode == PI_SPI_CS_AUTO && spim->pending_repeat_len == 0)
  {
    // CS auto mode. We handle the termination with an EOT so we have to enqueue
    // 3 transfers.
    // Enqueue fist SOT and user buffer.
    plp_udma_enqueue(base, (unsigned int)spim->udma_cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);
    plp_udma_enqueue(base, (unsigned int)data, size, UDMA_CHANNEL_CFG_EN);

    // Then wait until first one is finished
    while(!plp_udma_canEnqueue(base));

    // And finally enqueue the EOT.
    // The user notification will be sent as soon as the last transfer
    // is done and next pending transfer will be enqueued
    spim->udma_cmd[0] = SPI_CMD_EOT(1);
    plp_udma_enqueue(base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);
  }
  else
  {
    // CS keep mode.
    // Note this is also used for CS auto mode when we need to repeat the transfer to overcome
    // HW limitation, as CS must be kept low.
    // We cannot use EOT due to HW limitations (generating EOT is always releasing CS)
    // so we have to use TX event instead.
    // TX event is current inactive, enqueue first transfer first EOT.
    plp_udma_enqueue(base, (unsigned int)spim->udma_cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);
    // Then wait until it is finished (should be very quick).
    while(plp_udma_busy(base));
    // Then activateTX event and enqueue user buffer.
    // User notification and next pending transfer will be handled in the handler.
    soc_eu_fcEventMask_setEvent(spim_cs->channel + 1);
    plp_udma_enqueue(base, (unsigned int)data, size, UDMA_CHANNEL_CFG_EN);
  }

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
  uint32_t addr = (uint32_t)data;
  uint32_t size = (len + 7) >> 3;
  int irq = rt_irq_disable();
  uint32_t iter_size = 0;
  uint32_t misaligned_size = 0;

  __rt_task_init(task);

  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  pi_spim_t *spim = spim_cs->spim;
  int qspi = ((flags >> 2) & 0x3) == 1;
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


  //if ((addr & 0x3) || size < 4)
  //{
  //  iter_size = 4 - (addr & 0x3);
  //  if (iter_size > size)
  //    iter_size = size;
  //  misaligned_size = iter_size;
  //  addr = (uint32_t)&spim->buffer;
  //}
  //else 
  
  if (size > 8192)
  {
    iter_size = 8192;
  }


  unsigned int rx_base = hal_udma_channel_base(spim_cs->channel);

  if (iter_size)
  {
    spim->pending_repeat_misaligned_size = misaligned_size;
    spim->pending_repeat_len = len - iter_size*8;
    spim->pending_repeat_addr = (uint32_t)data + iter_size;
    spim->pending_repeat_base = rx_base;
    spim->pending_repeat_device = device;
    spim->pending_repeat_send = 0;
    spim->pending_repeat_flags = flags;
    len = iter_size*8;
    size = iter_size;
  }
  
  int cmd_size = 0;
  spim->udma_cmd[cmd_size++] = spim_cs->cfg;
  spim->udma_cmd[cmd_size++] = SPI_CMD_SOT(spim_cs->cs);
  spim->udma_cmd[cmd_size++] = SPI_CMD_RX_DATA(len, qspi, spim_cs->byte_align);

  if (cs_mode == PI_SPI_CS_AUTO && !spim->pending_repeat_len)
  {
    spim->udma_cmd[cmd_size++] = SPI_CMD_EOT(1);
  }

  if (cs_mode != PI_SPI_CS_AUTO || spim->pending_repeat_len)
  {
    soc_eu_fcEventMask_setEvent(spim_cs->channel);
  }

  unsigned int tx_base = hal_udma_channel_base(spim_cs->channel + 1);
  plp_udma_enqueue(rx_base, addr, size, UDMA_CHANNEL_CFG_EN | (2<<1));
  plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);

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

  int channel_id = spim_cs->channel;

  unsigned int rx_base = hal_udma_channel_base(channel_id);
  unsigned int tx_base = hal_udma_channel_base(channel_id+1);

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
  spim->udma_cmd[2] = SPI_CMD_FUL(len, spim_cs->byte_align);

  int size = (len + 7) >> 3;

  if (cs_mode == PI_SPI_CS_AUTO && spim->pending_repeat_len == 0)
  {
    plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, 3*4, UDMA_CHANNEL_CFG_EN);
    plp_udma_enqueue(rx_base, (unsigned int)rx_data, size, UDMA_CHANNEL_CFG_EN | (2<<1));
    plp_udma_enqueue(tx_base, (unsigned int)tx_data, size, UDMA_CHANNEL_CFG_EN);

    while(!plp_udma_canEnqueue(tx_base));

    spim->udma_cmd[0] = SPI_CMD_EOT(1);
    plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);
  }
  else
  {
    plp_udma_enqueue(tx_base, (unsigned int)spim->udma_cmd, 3*4, UDMA_CHANNEL_CFG_EN);
    while(plp_udma_busy(tx_base));
    soc_eu_fcEventMask_setEvent(channel_id);
    plp_udma_enqueue(rx_base, (unsigned int)rx_data, size, UDMA_CHANNEL_CFG_EN | (2<<1));
    plp_udma_enqueue(tx_base, (unsigned int)tx_data, size, UDMA_CHANNEL_CFG_EN);
  }

end:
  rt_irq_restore(irq);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flags)
{
  pi_task_t task;
  pi_spi_transfer_async(device, tx_data, rx_data, len, flags, pi_task_block(&task));
  pi_task_wait_on(&task);
}




void pi_spi_ucode_set(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size)
{
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
  spim_cs->ucode = ucode;
  spim_cs->ucode_size = ucode_size;
}



void __pi_handle_waiting_copy(pi_task_t *task)
{
  if (task->implem.data[0] == 0)
    pi_spi_send_async((struct pi_device *)task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task->implem.data[4], task);
  else if (task->implem.data[0] == 1)
    pi_spi_receive_async((struct pi_device *)task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task->implem.data[4], task);
  else if (task->implem.data[0] == 2)
    pi_spi_transfer_async((struct pi_device *)task->implem.data[1], (void *)task->implem.data[2], (void *)task->implem.data[3], task->implem.data[4], task->implem.data[5], task);
  else if (task->implem.data[0] == 3)
      pi_spi_copy_async((struct pi_device *)task->implem.data[1], task->implem.data[2], (void *)task->implem.data[3], task->implem.data[4], task->implem.data[5], task);
  else if (task->implem.data[0] == 4)
      pi_spi_copy_2d_async((struct pi_device *)task->implem.data[1], task->implem.data[2], (void *)task->implem.data[3], task->implem.data[4], task->implem.data[5], task->implem.data[6], task->implem.data[7], task);
}




void *pi_spi_receive_ucode_set(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;

    if (spim_cs->udma_receive_cmd)
        pi_l2_free(spim_cs->udma_receive_cmd, (spim_cs->udma_receive_cmd_size + 2)*4);

    if (ucode == NULL)
        return NULL;

    spim_cs->udma_receive_cmd = pi_l2_malloc(ucode_size + 4*4);
    if (spim_cs->udma_receive_cmd == NULL)
        return NULL;

    pos_spim_apply_conf(spim_cs);

    memcpy(&spim_cs->udma_receive_cmd[2], ucode, ucode_size);
    spim_cs->udma_receive_cmd_size = 2 + (ucode_size >> 2);

    return (void *)&spim_cs->udma_receive_cmd[2];
    return NULL;
}



void pi_spi_receive_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size)
{
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;

  spim_cs->receive_addr_ucode = ucode;
  spim_cs->receive_addr_ucode_size = ucode_size;
}



void *pi_spi_send_ucode_set(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;

    if (spim_cs->udma_send_cmd)
        pi_l2_free(spim_cs->udma_send_cmd, (spim_cs->udma_send_cmd_size + 2)*4);

    if (ucode == NULL)
        return NULL;

    spim_cs->udma_send_cmd = pi_l2_malloc(ucode_size + 4*4);
    if (spim_cs->udma_send_cmd == NULL)
        return NULL;

    pos_spim_apply_conf(spim_cs);

    memcpy(&spim_cs->udma_send_cmd[2], ucode, ucode_size);
    spim_cs->udma_send_cmd_size = 2 + (ucode_size >> 2);

    return (void *)&spim_cs->udma_send_cmd[2];

    return NULL;
}



void pi_spi_send_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size)
{
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;

  spim_cs->send_addr_ucode = ucode;
  spim_cs->send_addr_ucode_size = ucode_size;
}



void pos_spim_handle_copy_eot(int event, void *arg)
{
  pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)arg;
  pi_spim_t *spim = (pi_spim_t *)spim_cs->spim;

  __rt_udma_register_extra_callback(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, __pi_spim_handle_eot, (void *)spim);
  pos_spim_handle_pending_task(spim);
}




__attribute__((noinline)) void pos_spim_send_enqueue_transfer(int event, void *arg)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)arg;
    pi_spim_t *spim = (pi_spim_t *)spim_cs->spim;

    uint32_t addr = spim->pending_repeat_misaligned_addr;
    uint32_t size = spim->pending_repeat_misaligned_size;
    uint32_t chunk_size = spim_cs->max_snd_size;

    uint32_t cmd_size;
    uint32_t *cmd;

    //printf("Enqueueing send transfer (addr: 0x%lx, ram_addr: 0x%x, pending size: 0x%lx)\n", addr, spim->pending_repeat_misaligned_ram_addr, size);

    memcpy(spim_cs->send_addr_ucode, &spim->pending_repeat_misaligned_ram_addr, spim_cs->send_addr_ucode_size);



    cmd_size = spim_cs->udma_send_cmd_size;
    cmd = spim_cs->udma_send_cmd;

    if(addr & 3)
    {
        void *callback = __pos_spim_send_enqueue_transfer;

        uint32_t temp_size = 4 - (addr & 0x3);
        if (size > temp_size)
            size = temp_size;

        //printf("Enqueueing misaligned send chunk (addr: 0x%x, size: 0x%lx)\n", spim->pending_repeat_misaligned_addr, size);

        cmd[cmd_size++] = __BITINSERT(spim->tx_cmd, size*8-1, SPI_CMD_TX_DATA_SIZE_WIDTH, SPI_CMD_TX_DATA_SIZE_OFFSET);
        cmd[cmd_size++] = *(uint32_t *)spim->pending_repeat_misaligned_addr;
        cmd[cmd_size++] = SPI_CMD_EOT(1);

        spim->pending_repeat_misaligned_addr += size;
        spim->pending_repeat_misaligned_size -= size;
        spim->pending_repeat_misaligned_ram_addr += size;

        if (spim->pending_repeat_misaligned_size == 0)
        {
            if (spim->pending_repeat_misaligned_2d_size > 0)
            {
                uint32_t size = spim->pending_repeat_misaligned_length;

                if (size > spim->pending_repeat_misaligned_2d_size)
                {
                  size = spim->pending_repeat_misaligned_2d_size;
                }
                
                spim->pending_repeat_misaligned_2d_size -= size;

                spim->pending_repeat_misaligned_size = size;
                spim->pending_repeat_misaligned_ram_addr = spim->pending_repeat_misaligned_ram_addr - spim->pending_repeat_misaligned_length + spim->pending_repeat_misaligned_stride;
            }
            else
            {
                callback = __pos_spim_handle_copy_eot;
            }
        }

        __rt_udma_register_extra_callback_func(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, callback);

        uint32_t base = spim->periph_base + UDMA_CHANNEL_TX_OFFSET;
        plp_udma_enqueue(base, (unsigned int)cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
        void *callback = __pos_spim_send_enqueue_transfer;

        if (size > chunk_size)
        {
            size = chunk_size;
        }

        //printf("Enqueueing aligned send chunk (addr: 0x%x, size: 0x%lx)\n", spim->pending_repeat_misaligned_addr, size);
        
        spim->pending_repeat_misaligned_ram_addr += size;
        spim->pending_repeat_misaligned_addr += size;
        spim->pending_repeat_misaligned_size -= size;

        cmd[cmd_size++] = __BITINSERT(spim->tx_cmd, size*8-1, SPI_CMD_TX_DATA_SIZE_WIDTH, SPI_CMD_TX_DATA_SIZE_OFFSET);

        if (spim->pending_repeat_misaligned_size == 0)
        {
            if (spim->pending_repeat_misaligned_2d_size > 0)
            {
                uint32_t size = spim->pending_repeat_misaligned_length;

                if (size > spim->pending_repeat_misaligned_2d_size)
                {
                  size = spim->pending_repeat_misaligned_2d_size;
                }
                
                spim->pending_repeat_misaligned_2d_size -= size;

                spim->pending_repeat_misaligned_size = size;
                spim->pending_repeat_misaligned_ram_addr = spim->pending_repeat_misaligned_ram_addr - spim->pending_repeat_misaligned_length + spim->pending_repeat_misaligned_stride;
            }
            else
            {
                callback = __pos_spim_handle_copy_eot;
            }            
        }

        __rt_udma_register_extra_callback_func(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, callback);

        uint32_t base = spim->periph_base + UDMA_CHANNEL_TX_OFFSET;
        plp_udma_enqueue(base, (unsigned int)cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);
        plp_udma_enqueue(base, addr, size, UDMA_CHANNEL_CFG_EN | (2<<1));

        while(!plp_udma_canEnqueue(base));

        spim->udma_cmd[0] = SPI_CMD_EOT(1);
        plp_udma_enqueue(base, (unsigned int)spim->udma_cmd, 1*4, UDMA_CHANNEL_CFG_EN);
    }
}



static void __attribute__((noinline)) pos_spim_send_handle_misaligned(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, pi_spim_t *spim)
{
    //printf("Handling SPIM chunk (addr: 0x%lx, size: 0x%lx)\n", addr, size);

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = size;
    spim->pending_repeat_misaligned_2d_size = 0;

    __rt_udma_register_extra_callback_arg(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim_cs);

    pos_spim_send_enqueue_transfer(0, spim_cs);    
}


static void __attribute__((noinline)) pos_spim_send_handle_misaligned_2d(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, uint32_t stride, uint32_t length, pi_spim_t *spim)
{
    //printf("Sending SPIM 2D chunk (addr: 0x%lx, data: 0x%lx, size: 0x%lx, stride: 0x%lx, length: 0x%lx)\n", addr, data, size, stride, length);

    int transfer_size = size > length ? length : size;

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = transfer_size;
    spim->pending_repeat_misaligned_length = length;
    spim->pending_repeat_misaligned_stride = stride;
    spim->pending_repeat_misaligned_2d_size = size - transfer_size;

    __rt_udma_register_extra_callback_arg(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim_cs);

    pos_spim_send_enqueue_transfer(0, spim_cs);
}




__attribute__((noinline)) void pos_spim_receive_enqueue_transfer(int event, void *arg);

__attribute__((noinline)) void pos_spim_receive_handle_temp_buffer(int event, void *arg)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)arg;
    pi_spim_t *spim = (pi_spim_t *)spim_cs->spim;

    //printf("Handle misaligned\n");

    int transfer_index = spim->pending_transfer_read_index;
    spim->pending_transfer_read_index = transfer_index ^ 1;

    pos_spim_pending_transfert_t *transfer = &spim->pending_transfers[transfer_index];
    uint32_t prefix_addr = transfer->addr;
    uint32_t addr = prefix_addr & ~0x3;
    uint32_t size = transfer->size;

    //printf("Prefix addr %lx\n", prefix_addr);

    uint32_t value = *(uint32_t *)addr;
    int shift = (prefix_addr & 3)*8;
    value = __BITINSERT_R(value, *transfer->temp_buff, size*8, shift);
    //printf("@%lx %lx %lx -> %lx size %lx\n", addr, *(uint32_t *)addr, *transfer->temp_buff, value, size);
    *(uint32_t *)addr = value;

    if (transfer->end)
    {
        pos_spim_handle_copy_eot(event, arg);
    }
    else
        pos_spim_receive_enqueue_transfer(event, arg);
}


__attribute__((noinline)) void pos_spim_receive_enqueue_transfer(int event, void *arg)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)arg;
    pi_spim_t *spim = (pi_spim_t *)spim_cs->spim;

    __rt_udma_register_extra_callback_func(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim->pending_callback);

    uint32_t addr = spim->pending_repeat_misaligned_addr;
    uint32_t size = spim->pending_repeat_misaligned_size;
    uint32_t chunk_size = spim_cs->max_rcv_size;

    if (size == 0)
      return;

    uint32_t cmd_size;
    uint32_t *cmd;

    //printf("Enqueueing receive transfer (addr: 0x%lx, ram_addr: 0x%x, pending size: 0x%lx)\n", addr, spim->pending_repeat_misaligned_ram_addr, size);

    memcpy(spim_cs->receive_addr_ucode, &spim->pending_repeat_misaligned_ram_addr, spim_cs->receive_addr_ucode_size);

    cmd_size = spim_cs->udma_receive_cmd_size;
    cmd = spim_cs->udma_receive_cmd;

    if(size < 4 || (addr & 0x3))
    {
        int transfer_index = spim->pending_transfer_index;
        spim->pending_transfer_index = transfer_index ^ 1;
        pos_spim_pending_transfert_t *transfer = &spim->pending_transfers[transfer_index];

        uint32_t temp_size = 4 - (addr & 0x3);
        if (size > temp_size)
            size = temp_size;

        //printf("Enqueueing misaligned receive chunk (addr: 0x%x, size: 0x%lx)\n", spim->pending_repeat_misaligned_addr, size);

        transfer->addr = spim->pending_repeat_misaligned_addr;
        transfer->size = size;
        transfer->end = 0;

        spim->pending_repeat_misaligned_addr += size;
        spim->pending_repeat_misaligned_size -= size;
        spim->pending_repeat_misaligned_ram_addr += size;

        spim->pending_callback = __pos_spim_receive_handle_temp_buffer;

        cmd[cmd_size++] = __BITINSERT(spim->rx_cmd, size*8-1, SPI_CMD_RX_DATA_SIZE_WIDTH, SPI_CMD_RX_DATA_SIZE_OFFSET);

        if (spim->pending_repeat_misaligned_size == 0)
        {
            if (spim->pending_repeat_misaligned_2d_size > 0)
            {
                transfer->end = 0;

                uint32_t size = spim->pending_repeat_misaligned_length;

                if (size > spim->pending_repeat_misaligned_2d_size)
                {
                  size = spim->pending_repeat_misaligned_2d_size;
                }
                
                spim->pending_repeat_misaligned_2d_size -= size;

                spim->pending_repeat_misaligned_size = size;
                spim->pending_repeat_misaligned_ram_addr = spim->pending_repeat_misaligned_ram_addr - spim->pending_repeat_misaligned_length + spim->pending_repeat_misaligned_stride;
            }
            else
            {
                transfer->end = 1;
            }
        }

        cmd[cmd_size++] = SPI_CMD_EOT(1);

        plp_udma_enqueue(spim->periph_base + UDMA_CHANNEL_RX_OFFSET, (unsigned int)transfer->temp_buff, size, UDMA_CHANNEL_CFG_EN | (2<<1));
        plp_udma_enqueue(spim->periph_base + UDMA_CHANNEL_TX_OFFSET, (unsigned int)cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);
    }
    else
    {
        spim->pending_callback = __pos_spim_receive_enqueue_transfer;

        if (size > chunk_size)
        {
            size = chunk_size;
        }
        else
        {
            size &= ~0x3;
        }

        //printf("Enqueueing aligned receive chunk (addr: 0x%x, size: 0x%lx)\n", spim->pending_repeat_misaligned_addr, size);
        
        spim->pending_repeat_misaligned_ram_addr += size;
        spim->pending_repeat_misaligned_addr += size;
        spim->pending_repeat_misaligned_size -= size;

        cmd[cmd_size++] = __BITINSERT(spim->rx_cmd, size*8-1, SPI_CMD_RX_DATA_SIZE_WIDTH, SPI_CMD_RX_DATA_SIZE_OFFSET);

        cmd[cmd_size++] = SPI_CMD_EOT(1);
        
        if (spim->pending_repeat_misaligned_size == 0)
        {
            if (spim->pending_repeat_misaligned_2d_size > 0)
            {
                uint32_t size = spim->pending_repeat_misaligned_length;

                if (size > spim->pending_repeat_misaligned_2d_size)
                {
                  size = spim->pending_repeat_misaligned_2d_size;
                }
                
                spim->pending_repeat_misaligned_2d_size -= size;

                spim->pending_repeat_misaligned_size = size;
                spim->pending_repeat_misaligned_ram_addr = spim->pending_repeat_misaligned_ram_addr - spim->pending_repeat_misaligned_length + spim->pending_repeat_misaligned_stride;
            }
            else
            {
                spim->pending_callback = __pos_spim_handle_copy_eot;
            }            
        }

        plp_udma_enqueue(spim->periph_base + UDMA_CHANNEL_RX_OFFSET, addr, size, UDMA_CHANNEL_CFG_EN | (2<<1));
        plp_udma_enqueue(spim->periph_base + UDMA_CHANNEL_TX_OFFSET, (unsigned int)cmd, cmd_size*4, UDMA_CHANNEL_CFG_EN);
    }
}




static void __attribute__((noinline)) pos_spim_receive_handle_misaligned(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, pi_spim_t *spim)
{
    //printf("Receiving SPIM chunk (addr: 0x%lx, size: 0x%lx)\n", addr, size);

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = size;
    spim->pending_repeat_misaligned_2d_size = 0;

    __rt_udma_register_extra_callback_arg(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim_cs);

    pos_spim_receive_enqueue_transfer(0, spim_cs);

    if (spim->pending_repeat_misaligned_size)
    {
        pos_spim_receive_enqueue_transfer(0, spim_cs);
    }
    else
    {      
        __rt_udma_register_extra_callback_func(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim->pending_callback);
    }
    
}



static void __attribute__((noinline)) pos_spim_receive_handle_misaligned_2d(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, uint32_t stride, uint32_t length, pi_spim_t *spim)
{
    //printf("Receiving SPIM 2D chunk (addr: 0x%lx, data: 0x%lx, size: 0x%lx, stride: 0x%lx, length: 0x%lx)\n", addr, data, size, stride, length);

    int transfer_size = size > length ? length : size;

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = transfer_size;
    spim->pending_repeat_misaligned_length = length;
    spim->pending_repeat_misaligned_stride = stride;
    spim->pending_repeat_misaligned_2d_size = size - transfer_size;

    __rt_udma_register_extra_callback_arg(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim_cs);

    pos_spim_receive_enqueue_transfer(0, spim_cs);

    if (spim->pending_repeat_misaligned_size)
    {
        pos_spim_receive_enqueue_transfer(0, spim_cs);
    }
    else
    {      
        __rt_udma_register_extra_callback_func(ARCHI_SOC_EVENT_SPIM0_EOT + spim->id, (void *)spim->pending_callback);
    }
    
}


void pi_spi_copy(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size,
  pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_copy_async(device, addr, data, size, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}

void pi_spi_copy_async(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size,
  pi_spi_flags_e flags, pi_task_t *task)
{
    //printf("Copy bitstream (device: %p, ext2loc: %d, addr: 0x%lx, buffer: %p, size: 0x%lx, flags: 0x%x, task: %p)\n", device, __BITEXTRACTU(flags, 1, 4), addr, data, size, flags, task);

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
    pi_spim_t *spim = spim_cs->spim;

    int irq = hal_irq_disable();

    if (likely(!spim->pending_copy))
    {
        int qspi = __BITEXTRACTU(flags, 2, 2) == 1;
        int cs_mode = __BITEXTRACTU(flags, 2, 0);
        int ext2loc = __BITEXTRACTU(flags, 1, 4);
        
        spim->pending_copy = task;

        if (ext2loc)
        {
            spim->rx_cmd = __BITINSERT(spim_cs->rx_cmd, qspi, SPI_CMD_RX_DATA_QPI_WIDTH, SPI_CMD_RX_DATA_QPI_OFFSET);
            pos_spim_receive_handle_misaligned(spim_cs, addr, (uint32_t)data, size, spim);
        }
        else
        {
            spim->tx_cmd = __BITINSERT(spim_cs->tx_cmd, qspi, SPI_CMD_TX_DATA_QPI_WIDTH, SPI_CMD_TX_DATA_QPI_OFFSET);
            pos_spim_send_handle_misaligned(spim_cs, addr, (uint32_t)data, size, spim);
        }
        
    }
    else
    {
        pos_spim_enqueue_to_pending_7(spim, task, 3, (int)device, addr, (int)data, size, flags, 0, 0);
    }

    hal_irq_restore(irq);
}


void pi_spi_copy_2d(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_copy_2d_async(device, addr, data, size, stride, length, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}

void pi_spi_copy_2d_async(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_spi_flags_e flags, pi_task_t *task)
{
    //printf("Copy 2D bitstream (device: %p, ext2loc: %d, addr: 0x%lx, buffer: %p, size: 0x%lx, stride: 0x%lx, length: 0x%lx, flags: 0x%x, task: %p)\n", device, __BITEXTRACT(flags, 1, 5), addr, data, size, stride, length, flags, task);

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
    pi_spim_t *spim = spim_cs->spim;

    int irq = hal_irq_disable();

    if (likely(!spim->pending_copy))
    {
        int qspi = __BITEXTRACTU(flags, 2, 2) == 1;
        int cs_mode = __BITEXTRACTU(flags, 2, 0);
        int ext2loc = __BITEXTRACTU(flags, 1, 4);

        spim->pending_copy = task;

        if (ext2loc)
        {
            spim->rx_cmd = __BITINSERT(spim_cs->rx_cmd, qspi, SPI_CMD_RX_DATA_QPI_WIDTH, SPI_CMD_RX_DATA_QPI_OFFSET);
            pos_spim_receive_handle_misaligned_2d(spim_cs, addr, (uint32_t)data, size, stride, length, spim);
        }
        else
        {
            spim->tx_cmd = __BITINSERT(spim_cs->tx_cmd, qspi, SPI_CMD_TX_DATA_QPI_WIDTH, SPI_CMD_TX_DATA_QPI_OFFSET);
            pos_spim_send_handle_misaligned_2d(spim_cs, addr, (uint32_t)data, size, stride, length, spim);
        }
    }
    else
    {
        pos_spim_enqueue_to_pending_7(spim, task, 4, (int)device, addr, (int)data, size, stride, length, flags);
    }

    hal_irq_restore(irq);
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
  conf->max_rcv_chunk_size = -1;
  conf->max_snd_chunk_size = -1;
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
