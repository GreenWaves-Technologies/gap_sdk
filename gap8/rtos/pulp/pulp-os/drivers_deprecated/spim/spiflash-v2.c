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

typedef struct rt_spiflash_s {
  rt_flash_t header;
  rt_spim_t *spim;
} rt_spiflash_t;

#define CMD_BUFF_SIZE 16

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


static RT_L2_DATA uint32_t cmd_buffer[CMD_BUFF_SIZE];
static RT_L2_DATA flashRead_t flash_read;
static volatile RT_L2_DATA uint32_t __rt_spiflash_rcv_buffer;



static void __rt_spiflash_free(rt_spiflash_t *flash)
{
  if (flash != NULL) {
    rt_free(RT_ALLOC_FC_DATA, (void *)flash, sizeof(rt_spiflash_t));
  }
}



static void __rt_spiflash_send_and_rcv(rt_spiflash_t *flash, void *snd_buff, int snd_size, void *rcv_buff, int rcv_size)
{
#if 0
  rt_event_t *event = __rt_wait_event_prepare_blocking();
  rt_periph_copy_t *copy = &event->copy;
  rt_periph_copy_init(copy, 0);
  copy->event = event;
  rt_periph_dual_copy_safe(copy, flash->channel, (unsigned int)snd_buff, snd_size, (int)rcv_buff, rcv_size, 2<<1);
  __rt_wait_event(event);
#endif
}



static void __rt_spiflash_send(rt_spiflash_t *flash, void *buff, int size)
{
#if 0
  rt_periph_copy(NULL, flash->channel+1, (unsigned int)buff, size, 2<<1, NULL);
#endif
}



static void __rt_spiflash_set_reg(rt_spiflash_t *flash, unsigned int addr, unsigned int val)
{
  int index = 0;

  cmd_buffer[index++] = SPI_CMD_SOT       (0),
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0x06, 8, 0),    //write enable 
  cmd_buffer[index++] = SPI_CMD_EOT       (0),
  cmd_buffer[index++] = SPI_CMD_SOT       (0);
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0x71, 8, 0),    //write any register 
  cmd_buffer[index++] = SPI_CMD_TX_DATA  (4*8, 0, SPI_CMD_BYTE_ALIGN_DIS);
  cmd_buffer[index++] = (addr << 8) | val;
  cmd_buffer[index++] = SPI_CMD_EOT       (0);

  __rt_spiflash_send(flash, &cmd_buffer, index*4);
}



static uint32_t __rt_spiflash_read_flash_id(rt_spiflash_t *flash)
{
  int index = 0;

  cmd_buffer[index++] = 0x9F;

  rt_spim_send(
    flash->spim, cmd_buffer, index*8, RT_SPIM_CS_KEEP, NULL
  );
  
  rt_spim_receive(
    flash->spim, (void *)&__rt_spiflash_rcv_buffer, 4*8, RT_SPIM_CS_AUTO, NULL
  );

  return __rt_spiflash_rcv_buffer;
}


static uint32_t __rt_spiflash_confreg_init(rt_spiflash_t *flash)
{
  int index = 0;

  //cmd_buffer[index++] = 0x03020101;
  cmd_buffer[index++] = 0x01000000;

  rt_spim_send(
    flash->spim, cmd_buffer, index*32, RT_SPIM_CS_KEEP, NULL
  );
  
  return __rt_spiflash_rcv_buffer;

}

static uint32_t __rt_spiflash_reg_read(rt_spiflash_t *flash, uint32_t addr)
{
  int index = 0;

  cmd_buffer[index++] = 0x00000065;
  cmd_buffer[index++] = addr;

  rt_spim_send(
    flash->spim, cmd_buffer, index*32, RT_SPIM_CS_KEEP, NULL
  );
  
  rt_spim_receive(
    flash->spim, (void *)&__rt_spiflash_rcv_buffer, 4*8, RT_SPIM_CS_AUTO, NULL
  );

  return __rt_spiflash_rcv_buffer;

}

static uint32_t __rt_spiflash_confreg_read(rt_spiflash_t *flash)
{
  int index = 0;

  cmd_buffer[index++] = 0x35;

  rt_spim_send(
    flash->spim, cmd_buffer, 8, RT_SPIM_CS_KEEP, NULL
  );
  
  rt_spim_receive(
    flash->spim, (void *)&__rt_spiflash_rcv_buffer, 4*8, RT_SPIM_CS_AUTO, NULL
  );

  return __rt_spiflash_rcv_buffer;

}

static rt_flash_t *__rt_spiflash_open(rt_dev_t *dev, rt_flash_conf_t *conf, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_spiflash_t *flash = NULL;

  flash = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_spiflash_t));
  if (flash == NULL) goto error;

  rt_spim_conf_t spi_conf;
  rt_spim_conf_init(&spi_conf);
  spi_conf.max_baudrate = 1000000;
  spi_conf.polarity = 0;
  spi_conf.phase = 0;
  spi_conf.id = 0; 
  spi_conf.cs = 1;
  flash->spim = rt_spim_open(NULL, &spi_conf, NULL);

  uint32_t id = __rt_spiflash_read_flash_id(flash);
  printf("Got ID %lx\n", id);

  //__rt_spiflash_confreg_init(flash);

  printf("CR1V=%lx\n", __rt_spiflash_reg_read(flash, 0x00800002));
  printf("CR2V=%lx\n", __rt_spiflash_reg_read(flash, 0x00800003));
  printf("CR1V=%lx\n", __rt_spiflash_confreg_read(flash));


  flash_read.sot      = SPI_CMD_SOT       (0);
  flash_read.sendCmd  = SPI_CMD_SEND_CMD  (0x03, 8, 0);
  flash_read.sendAddr = SPI_CMD_SEND_ADDR (24, 0);
  flash_read.addr     = 0x00000000;
  flash_read.rxData   = SPI_CMD_RX_DATA   (1, 1, SPI_CMD_BYTE_ALIGN_ENA);
  flash_read.eot      = SPI_CMD_EOT       (0);

  __rt_spiflash_set_reg(flash, 0x00800002, 0x00); // quad mode disabled
  __rt_spiflash_set_reg(flash, 0x00800003, 0x08); // 8 dummy cycles
  //__rt_spiflash_set_reg(flash, 0x00800004, 0x10); //set page size to 512 bytes  CR3V[4] =1

  if (event) __rt_event_enqueue(event);

  rt_irq_restore(irq);

  return (rt_flash_t *)flash;

error:
  __rt_spiflash_free(flash);
  rt_irq_restore(irq);
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
  #if 0
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
  unsigned int flash_addr_cmd = SPI_CMD_SEND_ADDR_VALUE(((int)addr)<<8);
  unsigned int flash_size_cmd = SPI_CMD_RX_DATA(size*8, 0, SPI_CMD_BYTE_ALIGN_ENA);

  unsigned int cfg = (2<<1) | UDMA_CHANNEL_CFG_EN;
  copy->event = call_event;

  __rt_channel_push(channel, copy);

  if (likely(!channel->firstToEnqueue && !plp_udma_busy(tx_base)))
  {
    printf("Direct enqueue\n");
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
#endif
}


static uint32_t __rt_spiflash_send_cmd_and_rcv(rt_spiflash_t *flash, uint32_t cmd)
{
  int index = 0;

  cmd_buffer[index++] = SPI_CMD_SOT       (0);
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0x05, 8, 0);
  cmd_buffer[index++] = SPI_CMD_DUMMY     (32);
  cmd_buffer[index++] = SPI_CMD_RX_DATA   (8, 0, SPI_CMD_BYTE_ALIGN_DIS);
  cmd_buffer[index++] = SPI_CMD_EOT       (0);

  __rt_spiflash_rcv_buffer = 0;

  __rt_spiflash_send_and_rcv(flash, cmd_buffer, index*4, (void *)&__rt_spiflash_rcv_buffer, 4);

  return __rt_spiflash_rcv_buffer;
}



static void __rt_spiflash_send_cmd(rt_spiflash_t *flash, uint32_t cmd)
{
  int index = 0;

  cmd_buffer[index++] = SPI_CMD_SOT       (0);
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (cmd, 8, 0);
  cmd_buffer[index++] = SPI_CMD_EOT       (0);

  __rt_spiflash_send(flash, cmd_buffer, index*4);
}



static uint32_t __rt_spiflash_read_sr1v(rt_spiflash_t *flash)
{
  uint32_t val =  __rt_spiflash_send_cmd_and_rcv(flash, 0x05);

  printf("Got sr1v %lx\n", val);

  return __rt_spiflash_rcv_buffer;
}



static uint32_t __rt_spiflash_read_sr2v(rt_spiflash_t *flash)
{
  uint32_t val =  __rt_spiflash_send_cmd_and_rcv(flash, 0x07);

  printf("Got sr12v %lx\n", val);

  return __rt_spiflash_rcv_buffer;
}



static void __rt_spiflash_program(rt_flash_t *_dev, void *data, void *addr, size_t size, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_spiflash_t *flash = (rt_spiflash_t *)_dev;

  __rt_spiflash_send_cmd(flash, 0x06);

  int index = 0;
  cmd_buffer[index++] = SPI_CMD_EOT       (0),
  cmd_buffer[index++] = SPI_CMD_SOT       (0);
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0x02, 8, 0);
  cmd_buffer[index++] = SPI_CMD_SEND_ADDR (24, 0);
  cmd_buffer[index++] = ((int)addr)<<8;
  cmd_buffer[index++] = SPI_CMD_TX_DATA   (size*8, 0, SPI_CMD_BYTE_ALIGN_ENA);

  __rt_spiflash_send(flash, cmd_buffer, index*4);

  __rt_spiflash_send(flash, data, size);

  index = 0;
  cmd_buffer[index++] = SPI_CMD_EOT       (0),

  __rt_spiflash_send(flash, cmd_buffer, index*4);

  while ((__rt_spiflash_read_sr2v(flash) & 1) == 1)
  {
    rt_time_wait_us(100);
  }

  rt_irq_restore(irq);
}

static void __rt_spiflash_erase_sector(rt_flash_t *_dev, void *data, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_spiflash_t *flash = (rt_spiflash_t *)_dev;

  int index = 0;
  cmd_buffer[index++] = SPI_CMD_SOT       (0),
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0x06, 8, 0),    //write enable 
  cmd_buffer[index++] = SPI_CMD_EOT       (0),
  cmd_buffer[index++] = SPI_CMD_SOT       (0);
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0x20, 8, 0);
  cmd_buffer[index++] = SPI_CMD_SEND_ADDR (24, 0);
  cmd_buffer[index++] = ((int)data)<<8;
  cmd_buffer[index++] = SPI_CMD_EOT       (0);

  __rt_spiflash_send(flash, cmd_buffer, index*4);

  while ((__rt_spiflash_read_sr1v(flash) & 1) == 1)
  {
    rt_time_wait_us(100);
  }

  index = 0;
  cmd_buffer[index++] = SPI_CMD_SOT       (0);
  cmd_buffer[index++] = SPI_CMD_SEND_CMD  (0xD0, 8, 0);
  cmd_buffer[index++] = SPI_CMD_SEND_ADDR (24, 0);
  cmd_buffer[index++] = ((int)data)<<8;
  cmd_buffer[index++] = SPI_CMD_EOT       (0);

  __rt_spiflash_send(flash, cmd_buffer, index*4);

  uint32_t err = __rt_spiflash_read_sr2v(flash);

  printf("GOT err %ld\n", err);

  rt_irq_restore(irq);
}

static void __rt_spiflash_erase_chip(rt_flash_t *_dev, rt_event_t *event)
{
  printf("%s %d\n", __FILE__, __LINE__);
}

rt_flash_dev_t spiflash_desc = {
  .open         = &__rt_spiflash_open,
  .close        = &__rt_spiflash_close,
  .read         = &__rt_spiflash_read,
  .program      = &__rt_spiflash_program,
  .erase_chip   = &__rt_spiflash_erase_chip,
  .erase_sector = &__rt_spiflash_erase_sector
};
