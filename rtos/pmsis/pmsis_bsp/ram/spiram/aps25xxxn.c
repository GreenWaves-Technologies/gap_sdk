/*
 * Copyright (C) 2019 GreenWaves Technologies
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

// This drivers has only been tested with aps25608n

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/ram/aps25xxxn.h"
#include "pmsis/drivers/hyperbus.h"
#include "../extern_alloc.h"

// Max time in nanoseconds where chip select can stay active
#define APS25XXXN_CS_LOW_WIDTH 2000

#define APS25XXXN_DEFAULT_LATENCY 5

// Use linear bursts, commands are 0x20 for read and 0xA0 for writes
// but last R/W bit is automatically added and 1 byte command must be shifter left by 1 byte
#define APS25XXXN_SPI_CMD 0x2000

#define APS25XXXN_REG_SPI_CMD 0x4000

typedef struct
{
  struct pi_device hyper_device;
  extern_alloc_t alloc;
} aps25xxxn_t;


static int aps25xxxn_open(struct pi_device *device)
{
  struct pi_aps25xxxn_conf *conf = (struct pi_aps25xxxn_conf *)device->config;

  aps25xxxn_t *aps25xxxn = (aps25xxxn_t *)pmsis_l2_malloc(sizeof(aps25xxxn_t));
  if (aps25xxxn == NULL)
  {
      return -1;
  }

  device->data = (void *)aps25xxxn;

  int size = conf->ram_size;
  uint32_t start_addr = 0;

  if (conf->reserve_addr_0)
  {
    size -= 4;
    start_addr = 4;
  }

  if (extern_alloc_init(&aps25xxxn->alloc, (void *)start_addr, size))
  {
      goto error;
  }

  if (bsp_aps25xxxn_open(conf))
  {
      goto error2;
  }

  struct pi_hyper_conf hyper_conf;
  pi_hyper_conf_init(&hyper_conf);

  hyper_conf.burst_length = APS25XXXN_CS_LOW_WIDTH;
  hyper_conf.id = conf->spi_itf;
  hyper_conf.cs = conf->spi_cs;
  hyper_conf.type = PI_HYPER_TYPE_RAM;
  hyper_conf.is_spi = 1;
  hyper_conf.xip_en = conf->xip_en;
  hyper_conf.latency = APS25XXXN_DEFAULT_LATENCY;
  hyper_conf.spi_cmd = APS25XXXN_SPI_CMD;
  hyper_conf.reg_spi_cmd = APS25XXXN_REG_SPI_CMD;
  if (conf->baudrate)
  {
      hyper_conf.baudrate = conf->baudrate;
  }

  pi_open_from_conf(&aps25xxxn->hyper_device, &hyper_conf);

  int32_t error = pi_hyper_open(&aps25xxxn->hyper_device);
  if (error)
  {
      goto error2;
  }

  uint16_t reg = 0;
  pi_hyper_reg_get(&aps25xxxn->hyper_device, 8, (uint8_t *)&reg);
  reg |= 1 << 3;
  pi_hyper_reg_set(&aps25xxxn->hyper_device, 8, (uint8_t *)&reg);

  return 0;

error2:
  extern_alloc_deinit(&aps25xxxn->alloc);
error:
  pmsis_l2_malloc_free(aps25xxxn, sizeof(aps25xxxn_t));
  return -2;
}



static void aps25xxxn_close(struct pi_device *device)
{
  aps25xxxn_t *aps25xxxn = (aps25xxxn_t *)device->data;
  pi_hyper_close(&aps25xxxn->hyper_device);
  extern_alloc_deinit(&aps25xxxn->alloc);
  pmsis_l2_malloc_free(aps25xxxn, sizeof(aps25xxxn_t));
}



static void aps25xxxn_copy_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, int ext2loc, pi_task_t *task)
{
  aps25xxxn_t *aps25xxxn = (aps25xxxn_t *)device->data;

  if (ext2loc)
    pi_hyper_read_async(&aps25xxxn->hyper_device, addr, data, size, task);
  else
    pi_hyper_write_async(&aps25xxxn->hyper_device, addr, data, size, task);
}



static void aps25xxxn_copy_2d_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
  aps25xxxn_t *aps25xxxn = (aps25xxxn_t *)device->data;

  if (ext2loc)
    pi_hyper_read_2d_async(&aps25xxxn->hyper_device, addr, data, size, stride, length, task);
  else
    pi_hyper_write_2d_async(&aps25xxxn->hyper_device, addr, data, size, stride, length, task);
}




int aps25xxxn_alloc(struct pi_device *device, uint32_t *addr, uint32_t size)
{
  void *chunk;
  aps25xxxn_t *aps25xxxn = (aps25xxxn_t *)device->data;
  int err = extern_alloc(&aps25xxxn->alloc, size, &chunk);
  *addr = (uint32_t)chunk;
  return err;
}



int aps25xxxn_free(struct pi_device *device, uint32_t addr, uint32_t size)
{
  aps25xxxn_t *aps25xxxn = (aps25xxxn_t *)device->data;
  return extern_free(&aps25xxxn->alloc, size, (void *)addr);
}


static pi_ram_api_t aps25xxxn_api = {
  .open                 = &aps25xxxn_open,
  .close                = &aps25xxxn_close,
  .copy_async           = &aps25xxxn_copy_async,
  .copy_2d_async        = &aps25xxxn_copy_2d_async,
  .alloc                = &aps25xxxn_alloc,
  .free                 = &aps25xxxn_free,
};


void pi_aps25xxxn_conf_init(struct pi_aps25xxxn_conf *conf)
{
  conf->ram.api = &aps25xxxn_api;
  conf->baudrate = 0;
  conf->xip_en = 0;
  conf->reserve_addr_0 = 1;
  bsp_aps25xxxn_conf_init(conf);
}

