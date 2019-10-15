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

#if 0
static void __rt_spiflash_free(rt_spiflash_t *flash)
{
  if (flash != NULL) {
    rt_free(RT_ALLOC_FC_DATA, (void *)flash, sizeof(rt_spiflash_t));
  }
}

RT_L2_DATA static unsigned int cmd_flashId[] = {
  SPI_CMD_CFG      (8, 0, 0),
  SPI_CMD_SOT      (0),
  SPI_CMD_SEND_CMD (0x06, 8, 0),    //write enable 
  SPI_CMD_EOT      (0),
  SPI_CMD_SOT      (0),           
  SPI_CMD_SEND_CMD (0x0071, 8, 0),  //qpi enable:  CR2V[6] = 1
  SPI_CMD_SEND_ADDR(32, 0),                
  0x800003cf,
  SPI_CMD_EOT      (0)
};

typedef struct {
  unsigned int sot;
  unsigned int sendCmd;
  unsigned int sendAddr;
  unsigned int addr;
  unsigned int sendMode;
  unsigned int dummy;
  unsigned int rxData;
  unsigned int eot;
} flashRead_t;

RT_L2_DATA flashRead_t flash_read;

static rt_flash_t *__rt_spiflash_open(rt_dev_t *dev, rt_flash_conf_t *conf, rt_event_t *event)
{
  rt_spiflash_t *flash = NULL;

  flash = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_spiflash_t));
  if (flash == NULL) goto error;

  int periph_id = dev->channel;
  int channel_id = periph_id*2;

  flash->header.dev = dev;
  flash->channel = channel_id;

  plp_udma_cg_set(plp_udma_cg_get() | (1<<(periph_id>>1)));

  soc_eu_fcEventMask_setEvent(channel_id);
  soc_eu_fcEventMask_setEvent(channel_id+1);

  flash_read.sot      = SPI_CMD_SOT       (0);
  flash_read.sendCmd  = SPI_CMD_SEND_CMD  (0xEC, 8, 1);
  flash_read.sendAddr = SPI_CMD_SEND_ADDR (32, 1);
  flash_read.addr     = 0x00000000;
  flash_read.sendMode = SPI_CMD_SEND_CMD  (0x0A, 8, 1);
  flash_read.dummy    = SPI_CMD_DUMMY     (15);
  flash_read.rxData   = SPI_CMD_RX_DATA   (1, 1, SPI_CMD_BYTE_ALIGN_ENA);
  flash_read.eot      = SPI_CMD_EOT       (0);


  rt_periph_copy(NULL, channel_id+1, (unsigned int)&cmd_flashId, sizeof(cmd_flashId), 2<<1, event);

  if (event) __rt_event_enqueue(event);

  return (rt_flash_t *)flash;

error:
  __rt_spiflash_free(flash);
  return NULL;
}

static void __rt_spiflash_close(rt_flash_t *flash, rt_event_t *event)
{
  rt_spiflash_t *spi_flash = (rt_spiflash_t *)flash;
  __rt_spiflash_free(spi_flash);
}

void __rt_spiflash_enqueue_callback();

static void __rt_spiflash_read(rt_flash_t *_dev, void *data, void *addr, size_t size, rt_event_t *event)
{
  rt_trace(RT_TRACE_FLASH, "[UDMA] Enqueueing SPI flash read (dev: %p, data: %p, addr: %p, size 0x%x, event: %p)\n", _dev, data, addr, size, event);

  int irq = rt_irq_disable();

  rt_spiflash_t *flash = (rt_spiflash_t *)_dev;

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;
  int channel_id = flash->channel;

  rt_periph_copy_init_ctrl(copy, RT_PERIPH_COPY_SPIFLASH);

  rt_periph_channel_t *channel = __rt_periph_channel(channel_id);
  unsigned int periph_base = hal_udma_periph_base(channel_id >> 1);
  unsigned int tx_base = periph_base + UDMA_CHANNEL_TX_OFFSET;
  unsigned int rx_base = periph_base + UDMA_CHANNEL_RX_OFFSET;
  unsigned int tx_addr = (unsigned int)&flash_read;
  unsigned int rx_addr = (int)data;
  int tx_size = sizeof(flashRead_t);
  int rx_size = size;
  unsigned int flash_addr_cmd = SPI_CMD_SEND_ADDR_VALUE((int)addr);
  unsigned int flash_size_cmd = SPI_CMD_RX_DATA(size*8, 1, SPI_CMD_BYTE_ALIGN_ENA);

  unsigned int cfg = (2<<1) | UDMA_CHANNEL_CFG_EN;
  copy->event = call_event;

  __rt_channel_push(channel, copy);

  if (likely(!channel->firstToEnqueue && !plp_udma_busy(tx_base)))
  {
    flash_read.addr = flash_addr_cmd;
    flash_read.rxData = flash_size_cmd;

    plp_udma_enqueue(rx_base, rx_addr, rx_size, cfg);
    plp_udma_enqueue(tx_base, tx_addr, tx_size, cfg);
  } else {
    copy->size = tx_size;
    copy->cfg = cfg;
    copy->u.raw.val[0] = rx_addr;
    copy->u.raw.val[1] = rx_size;
    copy->u.raw.val[2] = flash_addr_cmd;
    copy->u.raw.val[3] = flash_addr_cmd;
    __rt_channel_enqueue(channel, copy, tx_addr, tx_size, cfg);
  }

__rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

rt_flash_dev_t spiflash_desc = {
  .open      = &__rt_spiflash_open,
  .close     = &__rt_spiflash_close,
  .read      = &__rt_spiflash_read
};

#endif

rt_flash_dev_t spiflash_desc = {
  .open      = NULL,
  .close     = NULL,
  .read      = NULL
};