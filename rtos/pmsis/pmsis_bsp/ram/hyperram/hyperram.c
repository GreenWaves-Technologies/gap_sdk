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
 */

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/ram/hyperram.h"
#include "pmsis/drivers/hyperbus.h"
#include "../extern_alloc.h"

#if defined(__PULPOS2__)
#if defined(CONFIG_GAP9_V2)
PI_L2 uint32_t reg_value;
#endif
#endif

typedef struct
{
  struct pi_device hyper_device;
  extern_alloc_t alloc;
} hyperram_t;


static int hyperram_open(struct pi_device *device)
{
  struct pi_hyperram_conf *conf = (struct pi_hyperram_conf *)device->config;

  hyperram_t *hyperram = (hyperram_t *)pmsis_l2_malloc(sizeof(hyperram_t));
  if (hyperram == NULL)
  {
      return -1;
  }

  device->data = (void *)hyperram;

  int size = conf->ram_size;
  uint32_t start_addr = 0;

  if (conf->reserve_addr_0)
  {
    size -= 4;
    start_addr = 4;
  }

  if (extern_alloc_init(&hyperram->alloc, (void *)start_addr, size))
  {
      goto error;
  }

  if (bsp_hyperram_open(conf))
  {
      goto error2;
  }

  struct pi_hyper_conf hyper_conf;
  pi_hyper_conf_init(&hyper_conf);

  hyper_conf.burst_length = 4000;
  hyper_conf.id = conf->hyper_itf;
  hyper_conf.cs = conf->hyper_cs;
  hyper_conf.type = PI_HYPER_TYPE_RAM;
  hyper_conf.xip_en = conf->xip_en;
  if (conf->baudrate)
  {
      hyper_conf.baudrate = conf->baudrate;
  }

  pi_open_from_conf(&hyperram->hyper_device, &hyper_conf);

  int32_t error = pi_hyper_open(&hyperram->hyper_device);
  if (error)
  {
      goto error2;
  }

#if defined(__PULPOS2__)
#if defined(CONFIG_GAP9_V2)
  reg_value = 0;
  pi_hyper_read(&hyperram->hyper_device, 0x80001000, &reg_value, 2);
  reg_value &= ~(1 << 3);// Activate variable latency to avoid additionnal latency when there is no refresh
  reg_value &= ~0xf0;  // Use 3 cycles of latency instead of the default 6 cycles
  reg_value |= 0xe0;
  pi_hyper_write(&hyperram->hyper_device, 0x80001000, &reg_value, 2);

  pi_hyper_ioctl(&hyperram->hyper_device, PI_HYPER_IOCTL_SET_LATENCY, (void *)3);

#endif
#endif

  return 0;

error2:
  extern_alloc_deinit(&hyperram->alloc);
error:
  pmsis_l2_malloc_free(hyperram, sizeof(hyperram_t));
  return -2;
}



static void hyperram_close(struct pi_device *device)
{
  hyperram_t *hyperram = (hyperram_t *)device->data;
  pi_hyper_close(&hyperram->hyper_device);
  extern_alloc_deinit(&hyperram->alloc);
  pmsis_l2_malloc_free(hyperram, sizeof(hyperram_t));
}



static void hyperram_copy_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, int ext2loc, pi_task_t *task)
{
  hyperram_t *hyperram = (hyperram_t *)device->data;

  if (ext2loc)
    pi_hyper_read_async(&hyperram->hyper_device, addr, data, size, task);
  else
    pi_hyper_write_async(&hyperram->hyper_device, addr, data, size, task);
}



static void hyperram_copy_2d_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
  hyperram_t *hyperram = (hyperram_t *)device->data;

  if (ext2loc)
    pi_hyper_read_2d_async(&hyperram->hyper_device, addr, data, size, stride, length, task);
  else
    pi_hyper_write_2d_async(&hyperram->hyper_device, addr, data, size, stride, length, task);
}




int hyperram_alloc(struct pi_device *device, uint32_t *addr, uint32_t size)
{
  void *chunk;
  hyperram_t *hyperram = (hyperram_t *)device->data;
  int err = extern_alloc(&hyperram->alloc, size, &chunk);
  *addr = (uint32_t)chunk;
  return err;
}



int hyperram_free(struct pi_device *device, uint32_t addr, uint32_t size)
{
  hyperram_t *hyperram = (hyperram_t *)device->data;
  return extern_free(&hyperram->alloc, size, (void *)addr);
}


#if 0

void __pi_hyperram_alloc_cluster_req(void *_req)
{
  pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *)_req;
  req->result = pi_hyperram_alloc(req->device, req->size);
  req->done = 1;
  __pi_cluster_notif_req_done(req->cid);
}



void __pi_hyperram_free_cluster_req(void *_req)
{
  pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *)_req;
  pi_hyperram_free(req->device, req->chunk, req->size);
  req->done = 1;
  __pi_cluster_notif_req_done(req->cid);
}



void pi_cl_hyperram_alloc(struct pi_device *device, uint32_t size, pi_cl_hyperram_alloc_req_t *req)
{
  req->device = device;
  req->size = size;
  req->cid = pi_cluster_id();
  req->done = 0;
  __pi_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __pi_hyperram_alloc_cluster_req, (void* )req);
  __pi_cluster_push_fc_event(&req->event);
}



void pi_cl_hyperram_free(struct pi_device *device, uint32_t chunk, uint32_t size, pi_cl_hyperram_free_req_t *req)
{
  req->device = device;
  req->size = size;
  req->chunk = chunk;
  req->cid = pi_cluster_id();
  req->done = 0;
  __pi_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __pi_hyperram_free_cluster_req, (void* )req);
  __pi_cluster_push_fc_event(&req->event);
}

#endif

static pi_ram_api_t hyperram_api = {
  .open                 = &hyperram_open,
  .close                = &hyperram_close,
  .copy_async           = &hyperram_copy_async,
  .copy_2d_async        = &hyperram_copy_2d_async,
  .alloc                = &hyperram_alloc,
  .free                 = &hyperram_free,
};


void pi_hyperram_conf_init(struct pi_hyperram_conf *conf)
{
  conf->ram.api = &hyperram_api;
  conf->baudrate = 0;
  conf->xip_en = 0;
  conf->reserve_addr_0 = 1;
  bsp_hyperram_conf_init(conf);
}

